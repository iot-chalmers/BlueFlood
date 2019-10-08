
#include <stdio.h> /* For printf() */
#include <inttypes.h>
#include <string.h>
#include <random.h>
#include "contiki.h"
#include "dev/leds.h"
#include "simple-uart.h"
#include "nrf-radio-driver.h"
#include "watchdog.h"

#include "uuids.h"
#include "ble-beacon-header.h"
#include "encode-decode-hamming-crc24.h"

#include "testbed.h"
#include "nrf-gpio.h"
/*---------------------------------------------------------------------------*/
#ifndef NTX
#define ROUND_LEN (5U)
#else
#define ROUND_LEN ((NTX)+1)
#endif /* NTX */
#ifndef ROUND_PERIOD_CONF_US
#error "Define round period!"
#endif
#define ROUND_PERIOD (ROUND_PERIOD_CONF_US - ROUND_LEN * SLOT_LEN)
/*---------------------------------------------------------------------------*/
#define IBEACON_SIZE  (sizeof(ble_beacon_t))
#define BLUETOOTH_BEACON_PDU(S) (8+(S))
#define PACKET_AIR_TIME_MIN (PACKET_AIR_TIME(BLUETOOTH_BEACON_PDU(IBEACON_SIZE),RADIO_MODE_CONF))
#define PAYLOAD_AIR_TIME_MIN (PACKET_AIR_TIME(IBEACON_SIZE,RADIO_MODE_CONF))
#define RX_SLOT_LEN (SLOT_PROCESSING_TIME+TX_CHAIN_DELAY+ US_TO_RTIMERTICKS(MY_RADIO_RAMPUP_TIME_US) + PACKET_AIR_TIME_MIN)
#define SLOT_LEN (RX_SLOT_LEN+2*GUARD_TIME_SHORT)
#define SLOT_LEN_NOTSYNCED (RX_SLOT_LEN+GUARD_TIME)
#define FIRST_SLOT_OFFSET (SLOT_PROCESSING_TIME + GUARD_TIME + ADDRESS_EVENT_T_TX_OFFSET)
/*---------------------------------------------------------------------------*/
/* Log config */
#define TESTBED_LOG_STYLE (TESTBED!=WIRED_TESTBED)
#define PRINT_TS_DELTA true
#define PRINT_RSSI true
#define PRINT_LAST_RX false
#define PRINT_RX_STATS false
#define PRINT_NODE_CONFIG true
#define PRINT_CUSTOM_DEBUG_MSG false
#define PRINT_NODE_REJOIN_WARNING false
#ifndef FIRMWARE_TIMESTAMP_STR
#define FIRMWARE_TIMESTAMP_STR (__DATE__ " " __TIME__)
#endif
/*---------------------------------------------------------------------------*/
const uint8_t uuids_array[UUID_LIST_LENGTH][16] = UUID_ARRAY;
const uint32_t testbed_ids[] = TESTBED_IDS;
enum {MSG_TURN_BROADCAST=0xff, MSG_TURN_NONE=0xfe};
/*---------------------------------------------------------------------------*/
#if ROUND_ROBIN_INITIATOR
volatile uint8_t initiator_node_index = INITATOR_NODE_INDEX;
#define tx_node_id        (TESTBED_IDS[initiator_node_index])
#else
#define tx_node_id        (TESTBED_IDS[INITATOR_NODE_INDEX])
#endif /* ROUND_ROBIN_INITIATOR */
#define IS_INITIATOR() (my_id == tx_node_id)
/*---------------------------------------------------------------------------*/
#if PRINT_CUSTOM_DEBUG_MSG
static char dbgmsg[256]="", dbgmsg2[256]="";
#endif
static uint8_t my_tx_buffer[255] = {0};
static uint8_t my_rx_buffer[255] = {0};
#if USE_HAMMING_CODE
static uint8_t encode_decode_buffer[255] = {0};
#endif 
static ble_beacon_t msg;
#if PRINT_LAST_RX
static ble_beacon_t msg_errors[ROUND_LEN];
#endif /* PRINT_LAST_RX */
static uint64_t corrupt_msg_index = 0;
static uint32_t my_id = 0;
/*---------------------------------------------------------------------------*/
PROCESS(tx_process, "TX process");
AUTOSTART_PROCESSES(&tx_process);
/*---------------------------------------------------------------------------*/
static int get_testbed_index(uint32_t my_id, const uint32_t *testbed_ids, uint8_t testbed_size){
  int i;
  for( i=0; i<testbed_size; i++ ){
    if( my_id == testbed_ids[i] ){
      return i;
    }
  }
  return -1;
}
/*---------------------------------------------------------------------------*/
static void init_ibeacon_packet(ble_beacon_t *pkt, const uint8_t* uuid, uint16_t round, uint16_t slot){
#if (RADIO_MODE_CONF == RADIO_MODE_MODE_Ieee802154_250Kbit)
    pkt->radio_len = sizeof(ble_beacon_t) - 1; /* execlude len field */
#else
  pkt->radio_len = sizeof(ble_beacon_t) - 2; /* len + pdu_header */ //length of the rest of the packet

#endif
  pkt->pdu_header = 0x42; //pdu type: 0x02 ADV_NONCONN_IND, rfu 0, rx 0, tx 1 //2;
  pkt->adv_address_low = MY_ADV_ADDRESS_LOW;
  pkt->adv_address_hi = MY_ADV_ADDRESS_HI;
  memcpy(pkt->uuid, uuid, sizeof(pkt->uuid));
  pkt->round = round;
  //pkt->minor = 0;
  pkt->slot = slot;
  pkt->turn = MSG_TURN_NONE;

  #if (PACKET_IBEACON_FORMAT)
  pkt->ad_flags_length = 2; //2bytes flags
  pkt->ad_flags_type = 1; //1=flags
  pkt->ad_flags_data = 6; //(non-connectable, undirected advertising, single-mode device)
  pkt->ad_length = 0x1a; //26 bytes, the remainder of the packet
  pkt->ad_type = 0xff; //manufacturer specific
  pkt->company_id = 0x004cU; //Apple ID
  pkt->beacon_type = 0x1502;//0x0215U; //proximity ibeacon
  pkt->power = 0;//256 - 60; //RSSI = -60 dBm; Measured Power = 256 – 60 = 196 = 0xC4
  #endif
}
/*---------------------------------------------------------------------------*/
/* Checks if the current time has passed a ref time + offset. Assumes
 * a single overflow and ref time prior to now. */
static uint8_t
check_timer_miss(rtimer_clock_t ref_time, rtimer_clock_t offset, rtimer_clock_t now)
{
  rtimer_clock_t target = ref_time + offset;
  uint8_t now_has_overflowed = now < ref_time;
  uint8_t target_has_overflowed = target < ref_time;

  if(now_has_overflowed == target_has_overflowed) {
    /* Both or none have overflowed, just compare now to the target */
    return target <= now;
  } else {
    /* Either now or target of overflowed.
     * If it is now, then it has passed the target.
     * If it is target, then we haven't reached it yet.
     *  */
    return now_has_overflowed;
  }
}
/*---------------------------------------------------------------------------*/
#if !BLUEFLOOD_BUSYWAIT
void rtc_schedule(uint32_t ticks);

// static int roundtimer_scheduled = false;
// void TIMER0_IRQHandler()
// {
//   /* Check if this is a compare event and not an overflow */
//   if (NRF_TIMER0->EVENTS_COMPARE[SCHEDULE_REG] == 1) {
//     /* Reset the compare event */
//     NRF_TIMER0->EVENTS_COMPARE[SCHEDULE_REG] = 0;
//     if (roundtimer_scheduled) {
//       NVIC_DisableIRQ(TIMER0_IRQn);
//     } else {

//     }
//   } /* else it is overflow */
//   // else
//   //   printf("OVERFLOW\n\r");
// }
#endif
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(tx_process, ev, data)
{
  static uint8_t my_turn = 0;
  static uint8_t failed_rounds = 0;
  static int8_t my_index = -1;
  static uint16_t round = 0, slot = 0, logslot=0, join_round = -1, sync_slot = UINT16_MAX;
  static uint16_t rx_ok = 0, rx_crc_failed = 0, rx_none = 0, tx_done=0, 
  berr = 0 /* bit errors per round */, 
  berr_per_pkt_max = 0, berr_per_byte_max = 0;
  static uint32_t rx_ok_total = 0, rx_failed_total = 0, berr_total = 0;
  #if TESTBED!=WIRED_TESTBED
  static int8_t rx_rssi[3*ROUND_LEN]={0};
  static uint8_t berr_pkts[3*ROUND_LEN]={0};
  static int32_t rx_ts_delta[3*ROUND_LEN]={0UL};
  static char tx_status[3*ROUND_LEN+1]={0};
  #else
  static int8_t rx_rssi[ROUND_LEN]={0};
  static uint8_t berr_pkts[ROUND_LEN]={0};
  static int32_t rx_ts_delta[ROUND_LEN]={0UL};
  static char tx_status[ROUND_LEN+1]={0};
  #endif
  volatile static rtimer_clock_t tt =0, t_start_round = 0;
  static bool do_tx = 0, do_rx = 0, synced = 0, joined = 0;
  static volatile bool  last_crc_is_ok = 0;
  uint32_t guard_time = 0;
  int i;
  uint8_t last_rx_ok = 0;
  PROCESS_BEGIN();

  #if TEST_HELLO_WORLD
    my_radio_init(&my_id, my_tx_buffer);
    my_index = get_testbed_index(my_id, testbed_ids, TESTBED_SIZE);
    init_ibeacon_packet(&msg, &uuids_array[0][0], round, slot);
    //put radio in tx idle mode to send continuous carrier
    #if RADIO_TEST_TX_CARRIER
    my_radio_send(my_tx_buffer, BLE_DEFAULT_CHANNEL);
    #endif
    while(1){
      printf("#@ %s, ID: 0x%lx, master: 0x%lx, tx power: %d dBm, channel %u = %u MHz (%s), idx %d\n", 
      FIRMWARE_TIMESTAMP_STR, my_id, tx_node_id, (int8_t)BLE_DEFAULT_RF_POWER, BLE_DEFAULT_CHANNEL, 2400u+ble_hw_frequency_channels[BLE_DEFAULT_CHANNEL], OVERRIDE_BLE_CHANNEL_37 ? "not std" : "std", my_index);
      watchdog_periodic();
    }
  #elif TEST_SLEEP_NODE_FOREVER
    while(1){
      printf("#@ %s, ID: 0x%lx, master: 0x%lx, tx power: %d dBm, channel %u = %u MHz (%s)\n", 
      FIRMWARE_TIMESTAMP_STR, my_id, tx_node_id, (int8_t)BLE_DEFAULT_RF_POWER, BLE_DEFAULT_CHANNEL, 2400u+ble_hw_frequency_channels[BLE_DEFAULT_CHANNEL], OVERRIDE_BLE_CHANNEL_37 ? "not std" : "std");
      watchdog_periodic();
      __disable_irq(); // __enable_irq()
      // nrf_gpio_range_cfg_output(0,31);

      nrf_gpio_cfg_input(PORT(0,8), NRF_GPIO_PIN_NOPULL); //UART RX
      nrf_gpio_cfg_output(PORT(0,6)); //UART TX
      nrf_gpio_pin_clear(PORT(0,6));

      testbed_cofigure_pins();
      testbed_clear_debug_pins();
      //Enter System-on idle mode
      // __WFE();
      __SEV();
      __WFE();			
      __WFE();	
      //Enter System-off
      NRF_POWER->SYSTEMOFF = 1;
    }
  #endif

  testbed_cofigure_pins();
  testbed_clear_debug_pins();
  my_radio_init(&my_id, my_tx_buffer);
  // leds_off(LEDS_ALL);
  my_index = get_testbed_index(my_id, testbed_ids, TESTBED_SIZE);
  init_ibeacon_packet(&msg, &uuids_array[0][0], round, slot);
  watchdog_periodic();

  if(IS_INITIATOR()){
    // BUSYWAIT_UNTIL(0, RTIMER_SECOND);
    nrf_delay_ms(1000);
  }

  // tt = 0;
  t_start_round = RTIMER_NOW() + FIRST_SLOT_OFFSET;
  joined = 0;
  join_round = UINT16_MAX;

  // #if !BLUEFLOOD_BUSYWAIT
  // roundtimer_scheduled = false;
  // #endif

  #if ROUND_ROBIN_INITIATOR
  initiator_node_index = INITATOR_NODE_INDEX;
  #endif

  while(1)
  {
    rx_ok = 0, rx_crc_failed = 0, rx_none = 0; tx_done=0; berr = 0; berr_per_pkt_max = 0, berr_per_byte_max = 0; corrupt_msg_index = 0;
    tx_status[0] = ':';
    guard_time = GUARD_TIME;
    synced = 0;
    sync_slot = UINT16_MAX;
    my_turn = 0;

    #if (TESTBED==WIRED_TESTBED)
    #define ROUND_LEN_RULE (((!IS_INITIATOR()) && synced && (slot < ROUND_LEN)) || ((IS_INITIATOR() || !synced) && (slot < ROUND_LEN)) )
    #else
    #define ROUND_LEN_RULE (((!IS_INITIATOR()) && synced && (slot < sync_slot + ROUND_LEN)) || ((IS_INITIATOR() || !synced) && (slot < 2*ROUND_LEN)) )
    #endif /* TESTBED==WIRED_TESTBED */

    #if ROUND_ROBIN_INITIATOR
    if(joined){
      initiator_node_index = round % TESTBED_SIZE;
    } else {
      initiator_node_index = INITATOR_NODE_INDEX;
    } 
    #endif /* ROUND_ROBIN_INITIATOR */
    // nrf_gpio_cfg_output(ROUND_INDICATOR_PIN);
    nrf_gpio_pin_toggle(ROUND_INDICATOR_PIN);
    for(slot = 0; ROUND_LEN_RULE; slot++){
      logslot = slot + 1;
      tt = t_start_round + slot * SLOT_LEN;
      // BUSYWAIT_UNTIL(1, tt - guard_time);
      #if ROUND_ROBIN_INITIATOR
      do_tx = ( IS_INITIATOR() && (joined || (slot % 2 == 0))) || (!IS_INITIATOR() && synced && my_turn);
      #else
      do_tx = (IS_INITIATOR() && !synced && (slot % 2 == 0)) || (!IS_INITIATOR() && synced && (slot > 0) && my_turn);
      // do_tx = (IS_INITIATOR() && (slot < 4) && (slot % 2 == 0)) || (!IS_INITIATOR() && synced && my_turn && (slot % 2 != 0));
      // do_tx = (IS_INITIATOR()) || (!IS_INITIATOR() && synced && my_turn);
      #endif /* ROUND_ROBIN_INITIATOR */
      
      //do_tx = my_id == tx_node_id;
      do_rx = !do_tx;
      if(do_tx){
        msg.slot = slot;
        msg.round = round;
        joined = 1;
        guard_time = GUARD_TIME_SHORT;
        #if 0 //TWO_NODES_EXPERIMENT
        if(IS_INITIATOR()){
          static int turn;
          if(join_round == UINT16_MAX || join_round == -1){
            turn=(my_index + 1) % TESTBED_SIZE;
          } else {
            if(round - join_round >= 10 && (turn == (my_index + 1) % TESTBED_SIZE)){
              turn=((turn+1) % TESTBED_SIZE);
            } else if(round - join_round >= 20 && (turn == (my_index + 2) % TESTBED_SIZE) ){
              turn = MSG_TURN_BROADCAST;
            } else {
              turn = MSG_TURN_BROADCAST;
            }
          }
          // turn = (join_round == UINT16_MAX || join_round == -1) ? -1 : ((round - join_round)/10); 
          // turn=(turn % TESTBED_SIZE) +1 ; //give turn to node 1 by default

          // if(turn <= 0 || turn == my_index + 1){
          //   turn=(turn+1 % TESTBED_SIZE) +1; //give turn to node 1 by default
          // } else if(turn > TESTBED_SIZE){
          //   turn = MSG_TURN_BROADCAST;
          // } else {
          //   //msg.turn = turn;
          // }
          msg.turn = turn;
        } else {
          if(msg.turn != MSG_TURN_BROADCAST){
            msg.turn = MSG_TURN_NONE;
          }
        }
        #else
        msg.turn = MSG_TURN_BROADCAST; //all nodes are alowed to send
        #endif
        
        #if TEST_CE
        memcpy(msg.uuid, &uuids_array[(round+my_id&0xf)%UUID_LIST_LENGTH], sizeof(msg.uuid));
        //msg.uuid[0] = (round % 2) ? &uuids_array[round%UUID_LIST_LENGTH][0] : my_id >> 24UL;
        #endif
        uint8_t *tx_msg = (uint8_t *)&msg;
        #if USE_HAMMING_CODE
        tx_msg = encode_decode_buffer;
        encode_ble_packet(&msg, tx_msg);
        #endif
        //memcpy(my_tx_buffer, &msg, sizeof(msg));
        schedule_tx_abs(tx_msg, GET_CHANNEL(round,slot), tt - ADDRESS_EVENT_T_TX_OFFSET + ARTIFICIAL_TX_OFFSET);
        //memset(my_tx_buffer, 0, sizeof(my_tx_buffer));
        // while(!NRF_TIMER0->EVENTS_COMPARE[0]){watchdog_periodic();};
        BUSYWAIT_UNTIL(NRF_TIMER0->EVENTS_COMPARE[0] != 0U, RX_SLOT_LEN);
        if(!NRF_TIMER0->EVENTS_COMPARE[0]){
          tx_status[logslot] = 'T';
        } else {
          if(slot == 0){
            nrf_gpio_pin_toggle(SLOT1_INDICATOR_PIN);
            // nrf_gpio_pin_toggle(ROUND_INDICATOR_PIN);
          }
          BUSYWAIT_UNTIL(NRF_RADIO->EVENTS_END != 0U, RX_SLOT_LEN);
          if(!NRF_RADIO->EVENTS_END){
            tx_status[logslot] = 'R';
          } else {
            tx_done++;
            //tx_status[logslot] = 'X';
            tx_status[logslot] =  ( msg.turn == MSG_TURN_NONE ) ? 'X' : ( ( msg.turn == MSG_TURN_BROADCAST ) ? 'B' : ( ( msg.turn <= TESTBED_SIZE ) ? HEXC(msg.turn) : 'U' ) );
          }
        }
        rx_ts_delta[logslot] = get_rx_ts() - tt;
        rx_rssi[logslot] = get_radio_rssi();
      } else if(do_rx){
        static int join_trial = 0;
        uint8_t got_payload_event, got_address_event, got_end_event, slot_started;
        do{
          got_payload_event=0, got_address_event=0, got_end_event = 0, slot_started = 0, last_crc_is_ok = 0, last_rx_ok = 0;
          uint8_t channel = 0;
          if(!joined){ /* slave bootstrap code */
            int r=0, s=0;
            /* hop the channel when we have waited long enough on one channel: 2*N/(NTX/2) rounds */
            if( (join_trial % (MAX(12,2*NUMBER_OF_CHANNELS)/NTX) == 0)){
              s=random_rand();
              // channel=GET_CHANNEL(r,s);
              join_trial++;
            }
            channel = GET_CHANNEL(r, s);
            my_radio_rx(my_rx_buffer, channel);
            rtimer_clock_t to = 2UL*ROUND_PERIOD+random_rand()%ROUND_PERIOD;
            #if (RADIO_MODE_CONF == RADIO_MODE_MODE_Ieee802154_250Kbit)
            BUSYWAIT_UNTIL(NRF_RADIO->EVENTS_FRAMESTART != 0UL, to);
            got_address_event = NRF_RADIO->EVENTS_FRAMESTART;
            #else
            BUSYWAIT_UNTIL(NRF_RADIO->EVENTS_ADDRESS != 0UL, to);
            got_address_event = NRF_RADIO->EVENTS_ADDRESS;
            #endif
            r++; s++;
            watchdog_periodic();
            slot_started = 1;
          } else {
            rtimer_clock_t rx_target_time, rx_tn, rx_tref, rx_toffset, t_proc;
            uint8_t rx_missed_slot = 0;
            /* 
            * Note that: tt = t_start_round + slot * SLOT_LEN
            * Round logic started at rx_tref = t_start_round - FIRST_SLOT_OFFSET
            * We want to start rx at rx_target_time
            * rx_toffset = t_start_round + slot * SLOT_LEN - ADDRESS_EVENT_T_TX_OFFSET - guard_time - (t_start_round - FIRST_SLOT_OFFSET);
            */
            join_trial = 0;
            rx_target_time = tt - ADDRESS_EVENT_T_TX_OFFSET - guard_time;
            rx_tn = RTIMER_NOW();
            rx_tref = t_start_round - FIRST_SLOT_OFFSET;
            rx_toffset = slot * SLOT_LEN + FIRST_SLOT_OFFSET - ADDRESS_EVENT_T_TX_OFFSET - guard_time;
            rx_missed_slot = check_timer_miss(rx_tref, rx_toffset, rx_tn);
            nrf_gpio_pin_toggle(ROUND_INDICATOR_PIN);
            if(!rx_missed_slot){
              // t_proc = RTIMER_NOW();
              schedule_rx_abs(my_rx_buffer, GET_CHANNEL(round, slot), rx_target_time);
              t_proc = RTIMER_NOW() - rx_tn;
              BUSYWAIT_UNTIL_ABS(NRF_TIMER0->EVENTS_COMPARE[0] != 0U, rx_target_time + 2*guard_time + SLOT_PROCESSING_TIME );
              slot_started = NRF_TIMER0->EVENTS_COMPARE[0];
              if(slot_started){
                nrf_gpio_pin_toggle(ROUND_INDICATOR_PIN);
                #if (RADIO_MODE_CONF == RADIO_MODE_MODE_Ieee802154_250Kbit)
                BUSYWAIT_UNTIL_ABS(NRF_RADIO->EVENTS_FRAMESTART != 0U, rx_target_time + 2*guard_time + SLOT_PROCESSING_TIME + ADDRESS_EVENT_T_TX_OFFSET );
                got_address_event = NRF_RADIO->EVENTS_FRAMESTART;
                #else
                BUSYWAIT_UNTIL_ABS(NRF_RADIO->EVENTS_ADDRESS != 0U, rx_target_time + 2*guard_time + SLOT_PROCESSING_TIME + ADDRESS_EVENT_T_TX_OFFSET );
                got_address_event = NRF_RADIO->EVENTS_ADDRESS;
                #endif
              }
            }  
            if(rx_missed_slot || !slot_started) {
              #if PRINT_CUSTOM_DEBUG_MSG
              sprintf(dbgmsg, "t %" PRIu32 " %" PRIu32 " n %" PRIu32 " p %" PRIu32 " m %d %d", (t_start_round), rx_target_time, rx_tn, t_proc, rx_missed_slot, slot_started );
              #endif
            }
          }

          if(got_address_event) {
            #if (RADIO_MODE_CONF == RADIO_MODE_MODE_Ieee802154_250Kbit)
            //no EVENTS_PAYLOAD is emitted
            BUSYWAIT_UNTIL_ABS(NRF_RADIO->EVENTS_END != 0U, get_rx_ts() + PAYLOAD_AIR_TIME_MIN + CRC_AIR_T);

            got_end_event = NRF_RADIO->EVENTS_END;
            last_rx_ok = got_payload_event = got_end_event;
            last_crc_is_ok = USE_HAMMING_CODE || ((got_end_event != 0U) && (NRF_RADIO->CRCSTATUS & RADIO_CRCSTATUS_CRCSTATUS_CRCOk));
            // last_crc_is_ok = 1; //XXX
            #else
            BUSYWAIT_UNTIL_ABS(NRF_RADIO->EVENTS_PAYLOAD != 0U, get_rx_ts() + PAYLOAD_AIR_TIME_MIN);
            got_payload_event = NRF_RADIO->EVENTS_PAYLOAD;
            last_rx_ok = got_payload_event;
            if(got_payload_event){
              BUSYWAIT_UNTIL(NRF_RADIO->EVENTS_END != 0U, CRC_AIR_T);
              got_end_event = NRF_RADIO->EVENTS_END;
              last_crc_is_ok = USE_HAMMING_CODE || ((got_end_event != 0U) && (NRF_RADIO->CRCSTATUS & RADIO_CRCSTATUS_CRCSTATUS_CRCOk));
            }
            #endif /* (RADIO_MODE_CONF == RADIO_MODE_MODE_Ieee802154_250Kbit) */
          }
          /* check if it is a valid packet: a. our uuid and b. CRC ok */
          if(last_rx_ok && last_crc_is_ok){
            ble_beacon_t *rx_pkt = (ble_beacon_t *) my_rx_buffer;
            #if USE_HAMMING_CODE
            rx_pkt = (ble_beacon_t *) encode_decode_buffer;
            last_crc_is_ok = decode_ble_packet(my_rx_buffer, encode_decode_buffer) == 0;
            #endif

            /* check if it is our beacon packet */
            last_rx_ok = last_crc_is_ok ? (( rx_pkt->adv_address_low == MY_ADV_ADDRESS_LOW ) && ( rx_pkt->adv_address_hi == MY_ADV_ADDRESS_HI )) : 0;
            // last_rx_ok = last_crc_is_ok; //XXX!

            if(last_rx_ok){
              memcpy(&msg, &my_rx_buffer, rx_pkt->radio_len + 1);
              if(!synced){
                guard_time = GUARD_TIME_SHORT;
                synced = 1;
                if(!IS_INITIATOR()){
                  slot = rx_pkt->slot;
                  logslot = slot + 1;
                  round = rx_pkt->round;
                  sync_slot = slot;
                  t_start_round = get_rx_ts() - TX_CHAIN_DELAY - slot * SLOT_LEN;
                  my_turn = (rx_pkt->turn == my_index) || (rx_pkt->turn == MSG_TURN_BROADCAST);
                  #if 0
                  printf("pkt: ");
                  for(i=0; i<sizeof(my_rx_buffer); i++){
                    printf("%d, ", my_rx_buffer[i]);
                  }
                  printf("\n");
                  #endif
                }
              }
              if(sync_slot == UINT16_MAX){ //for the initiator
                sync_slot = rx_pkt->slot;
              }
              if(!joined || join_round == UINT16_MAX){
                join_round = round;
                joined = 1;
              }
            }
          } else {
            /* if the radio got stuck in bootstrap mode, then turn it off and on again. it is needed when we get a crc error */
            my_radio_off_completely();
          }
        } while(!joined);

        if(last_rx_ok && last_crc_is_ok) {
          tx_status[logslot] = '-';
        } else if(!slot_started) {
          tx_status[logslot] = 'M';
        }  else if(!got_address_event) {
          tx_status[logslot] = 'A';
        } else if(!got_payload_event) {
          tx_status[logslot] = 'P';
        } else if(!got_end_event) {
          tx_status[logslot] = 'E';
        } else if(!last_crc_is_ok) {
          tx_status[logslot] = 'C';
        } else {
          tx_status[logslot] = '?';
        }

        rx_ok += last_rx_ok && last_crc_is_ok;
        if(CRC_LEN > 0 || USE_HAMMING_CODE){
          rx_crc_failed += got_address_event && !last_crc_is_ok;
        } else {
          rx_crc_failed += memcmp(&my_rx_buffer, &msg, msg.radio_len - 5) != 0;
        }
        rx_none += (!got_address_event || !got_end_event) && !last_rx_ok;
        rx_ts_delta[logslot] = get_rx_ts() - TX_CHAIN_DELAY - tt;
        rx_rssi[logslot] = get_rx_rssi(&my_rx_buffer);

        if(last_rx_ok && !last_crc_is_ok){
          corrupt_msg_index |= (1UL << logslot);
          #if PRINT_LAST_RX
          uint8_t * pmsg = (uint8_t*)&msg;
          uint8_t * pmsg_errors = (uint8_t*)&msg_errors[logslot];
          int i; 
          uint8_t berr_byte, berr_per_pkt, berr_xor;
          berr_byte = 0;
          for(i = 0; i < sizeof(ble_beacon_t); i++){
            berr_xor = my_rx_buffer[i] ^ pmsg[i]; /* berr_xor = ones if there is a difference */
            pmsg_errors[i] = berr_xor;
            if(berr_xor){ 
              berr_byte = ((berr_xor & 1) != 0) + ((berr_xor & 2) != 0) + ((berr_xor & 4) != 0) + ((berr_xor & 8) != 0)
              + ((berr_xor & 16)!= 0) + ((berr_xor & 32)!= 0) + ((berr_xor & 64)!= 0) + ((berr_xor & 128)!= 0);
              berr_per_pkt += berr_byte;
              berr_per_byte_max = berr_per_byte_max >= berr_byte ? berr_per_byte_max : berr_byte;
            }
          }
          berr_pkts[logslot] = berr_per_pkt;
          berr += berr_per_pkt;
          berr_per_pkt_max = berr_per_pkt_max >= berr_per_pkt ? berr_per_pkt_max : berr_per_pkt;
          #endif /* PRINT_LAST_RX */
        }
      }
    }
    my_radio_off_completely();
    // nrf_gpio_cfg_output(ROUND_INDICATOR_PIN);
    nrf_gpio_pin_toggle(ROUND_INDICATOR_PIN);
    
    rx_ok_total += rx_ok;
    berr_total += berr;
    rx_failed_total += rx_crc_failed + rx_none;
    uint32_t rx_ok_percent = (rx_ok_total*100) / (MAX(1, rx_ok_total+rx_failed_total));

    /*printf("BERRs: ");
    for(i=0; i<sizeof(berr_pkts)/sizeof(berr_pkts[0]); i++){
      printf("%d, ", berr_pkts[i]);
    }
    printf("bits per packet\n");*/
    memset(berr_pkts, 0, sizeof(berr_pkts));

  #if TESTBED_LOG_STYLE
    #if PRINT_RX_STATS
    printf("{rx-%d} %u, %u, %u, %u, %lu, %lu, %u, %u, %u, %lu, %d\n", round, rx_ok, rx_crc_failed, rx_none, tx_done, rx_ok_total, rx_ok_total+rx_failed_total, berr_per_byte_max, berr_per_pkt_max, berr /* bit errors per round */, berr_total, sync_slot);
    #endif
    #if PRINT_RSSI
    printf("{rssi-%d} ", round);
    for(i=0; i<sizeof(rx_rssi)/sizeof(rx_rssi[0]); i++){
      printf("%d, ", rx_rssi[i]);
    }
    printf("\n");
    memset(rx_rssi, 111, sizeof(rx_rssi));
    #endif /* PRINT_RSSI */

    #if PRINT_TS_DELTA
    printf("{td-%d} ", round);
    for(i=0; i<sizeof(rx_ts_delta)/sizeof(rx_ts_delta[0]); i++){
      printf("%" PRId32 ", ", rx_ts_delta[i]);
    }
    printf("\n");
    memset(rx_ts_delta, 0, sizeof(rx_ts_delta));
    #endif /* PRINT_TS_DELTA */

    printf("{tx-%d} %s\n", round, tx_status);
    #if PRINT_CUSTOM_DEBUG_MSG
    if(dbgmsg[0]!=0){
      printf("{dg-%d} %s\n", round, dbgmsg);
      dbgmsg[0]=0;
    }
    if(dbgmsg2[0]!=0){
    printf("{dg2-%d} %s\n", round, dbgmsg2);
    dbgmsg2[0]=0;
    }
    #endif /* PRINT_DEBUG_MSG */

  #else /* TESTBED_LOG_STYLE */
    printf("rx_ok %u, crc %u, none %u, tx %u: OK %lu of %lu, berr b%u p%u r%u %lu, sync %d\n", rx_ok, rx_crc_failed, rx_none, tx_done, rx_ok_total, rx_ok_total+rx_failed_total, berr_per_byte_max, berr_per_pkt_max, berr /* bit errors per round */, berr_total, sync_slot);

    #if PRINT_RSSI
    printf("Rssi: ");
    for(i=0; i<sizeof(rx_rssi)/sizeof(rx_rssi[0]); i++){
      printf("%d, ", rx_rssi[i]);
    }
    printf("dB\n");
    memset(rx_rssi, 111, sizeof(rx_rssi));
    #endif /* PRINT_RSSI */

    #if PRINT_TS_DELTA
    printf("Ts delta: ");
    for(i=0; i<sizeof(rx_ts_delta)/sizeof(rx_ts_delta[0]); i++){
      printf("%" PRId32 ", ", rx_ts_delta[i]);
    }
    printf("ticks\n");
    memset(rx_ts_delta, 0, sizeof(rx_ts_delta));
    #endif /* PRINT_TS_DELTA */
    
    printf("Tx status: %s\n", tx_status);
  #endif /* TESTBED_LOG_STYLE */

    memset(tx_status, '.', sizeof(tx_status));
    tx_status[sizeof(tx_status)-1] = '\0';
    #if PRINT_LAST_RX
    printf("{err-%d} ", round);
    uint8_t *pmsg = (uint8_t *)&msg;
    for(i=0; i<=msg.radio_len; i++){
      printf("%02x ", pmsg[i]);
    }
    printf("CRC: %lx.", NRF_RADIO->RXCRC);
    printf("\n");
    if(corrupt_msg_index == 0){
      printf("No errors.\n");
    } else {
      int s;
      for(s=1; s<ROUND_LEN; s++){
        if(corrupt_msg_index & (1UL << s)){
          printf("[%2d] ", s);
          uint8_t *pmsg_errors = (uint8_t *)&msg_errors[s];
          for(i=0; i<=msg.radio_len; i++){
            printf("%02x ", pmsg_errors[i]);
          }
          printf("\n");
        }
      }
    }
    memset(&msg_errors, 0, ROUND_LEN*msg.radio_len);
    #endif /* PRINT_LAST_RX */

    memset(my_rx_buffer, 0, msg.radio_len);

    #if PRINT_NODE_CONFIG
    if(round % 1024 == 0){
      printf("#R %u, ID: 0x%lx %d, master: 0x%lx, tx power: %d dBm, channel %u = %u MHz (%s), msg: %d bytes, mode: %s, CE: %d, @ %s\n", 
              round, my_id, my_index, tx_node_id, (int8_t)BLE_DEFAULT_RF_POWER, BLE_DEFAULT_CHANNEL, 2400u+ble_hw_frequency_channels[BLE_DEFAULT_CHANNEL], OVERRIDE_BLE_CHANNEL_37 ? "not std" : "std", sizeof(ble_beacon_t), RADIO_MODE_TO_STR(RADIO_MODE_CONF), TEST_CE, FIRMWARE_TIMESTAMP_STR);
    }
    #endif

    if(rx_ok == 0){
      //did not receive for X round: resync
      if(failed_rounds > 10){
        synced = 0;
        joined = 0;
        #if PRINT_NODE_REJOIN_WARNING
        printf("{fr-%d} Rejoining: failed rounds %d, joined %d, synced %d\n", (int)round, (int)failed_rounds, (int)joined, (int)synced);
        #endif
      }
      failed_rounds++;
    } else {
      failed_rounds = 0;
      rx_ok = 0;
    }

    round++;
    init_ibeacon_packet(&msg, &uuids_array[0][0], round, slot);
    //msg.round=round;
    rtimer_clock_t now, t_start_round_old;
    now = RTIMER_NOW();
    #define TIMER_GUARD 16
    uint8_t round_is_late = check_timer_miss(t_start_round, ROUND_PERIOD-TIMER_GUARD, now);
    t_start_round += ROUND_PERIOD;

    if(round_is_late){
      printf("#!{%d}PRE GO late: %ld\n", round, now - t_start_round);
      round_is_late = check_timer_miss(t_start_round, ROUND_PERIOD-TIMER_GUARD, now);
      t_start_round += ROUND_PERIOD;
    }
    


  #if BLUEFLOOD_BUSYWAIT
    /* wait at the end of the round */
    NRF_TIMER0->CC[0] = t_start_round - FIRST_SLOT_OFFSET;;
    while(!NRF_TIMER0->EVENTS_COMPARE[0]){
      watchdog_periodic();
    }
  #else
    rtimer_clock_t tnow = RTIMER_NOW();
    uint32_t rtc_ticks = RTIMER_TO_RTC((t_start_round - tnow))-RTC_GUARD; //save one RTC tick for preprocessing!
    rtimer_clock_t sleep_period = (t_start_round - tnow);
    // printf("going to sleep: now %lu for %" PRId32 " hf = %lu hf %lu lf\n", tnow, sleep_period, RTC_TO_RTIMER(rtc_ticks), rtc_ticks);
    rtc_schedule(rtc_ticks);
    /* go to sleep mode: put prepherals to sleep then sleep the CPU */
    // NRF_RADIO->POWER = 0;
    // /* Unonfigure the channel as the caller expects */
    // for (int i = 0; i < 8; i++)
    // {
    //   NRF_GPIOTE->CONFIG[i] = (GPIOTE_CONFIG_MODE_Disabled << GPIOTE_CONFIG_MODE_Pos) |
    //                           (31UL << GPIOTE_CONFIG_PSEL_Pos) |
    //                           (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos);
    // }
    // /* Three NOPs are required to make sure configuration is written before setting tasks or getting events */
    // __NOP();
    // __NOP();
    // __NOP();
    /* Go to sleep sequence: 
     * SEV Set event and WFE wait for event first to consume any previously set event if any, then wait for event to sleep the CPU until an event happens. 
    */
    __SEV();
    __WFE();
    __WFE();
    // testbed_cofigure_pins();
    // my_radio_init(&my_id, my_tx_buffer);    /* turn LEDs off: active low, so set the pins */
    // rtimer_clock_t tnow2 = RTIMER_NOW();
    // rtimer_clock_t sleep_period2 = (tnow2 - tnow);
    // //correct the round timer based on the sleep time, because timer0 was sleeping // NO!
    // //XXX Timer0 is still counting. No need to adjust it.
    // // t_start_round -= RTC_TO_RTIMER(rtc_ticks);
    // // printf("wakeup: now %lu for %" PRId32 " hf = %lu hf %lu lf\n", tnow2, sleep_period2, RTIMER_TO_RTC(sleep_period2), rtc_ticks);
    guard_time = GUARD_TIME;
    tt = t_start_round - FIRST_SLOT_OFFSET;
    NRF_TIMER0->EVENTS_COMPARE[0]=0;
    NRF_TIMER0->CC[0] = tt;
    BUSYWAIT_UNTIL_ABS(NRF_TIMER0->EVENTS_COMPARE[0] != 0UL, tt);
#endif
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
#if !BLUEFLOOD_BUSYWAIT

void rtc_schedule(uint32_t ticks)
{     
  //trick: RTC schedule function will trigger overflow event to mirror that on the GPIO for debugging purposes
  #if 1
  // NVIC_DisableIRQ(RTC1_IRQn);
  // NRF_RTC1->EVTENSET |= RTC_EVTENSET_OVRFLW_Msk;
  // NRF_RTC1->TASKS_TRIGOVRFLW=1;
  // NRF_RTC1->TASKS_CLEAR=1;
  // NRF_RTC1->EVTENCLR |= RTC_EVTENSET_OVRFLW_Msk;
  // nrf_gpio_cfg_output(RTC_SCHEDULE_PIN);
  nrf_gpio_pin_toggle(RTC_SCHEDULE_PIN);
  #endif
  /* Set prescaler so that TICK freq is CLOCK_SECOND */
  NRF_RTC1->PRESCALER = RTC_PRESCALER-1; //fRTC [kHz] = 32.768 / (PRESCALER + 1 )
  NRF_RTC1->TASKS_CLEAR=1;
  NRF_RTC1->CC[1]=ticks;
  /* Enable comapre event and compaer interrupt */
  NRF_RTC1->EVTENSET      = RTC_EVTENSET_COMPARE1_Msk;;
  NRF_RTC1->INTENSET      = RTC_INTENSET_COMPARE1_Msk;
  /* Enable Interrupt for RTC1 in the core */
  NVIC_SetPriority(RTC1_IRQn, 3);
  NVIC_EnableIRQ(RTC1_IRQn);
  NRF_RTC1->TASKS_START = 1;
  //poll the WDT so it does not fire early. we keep it running though so it wakes us up if something wrong happened...
  watchdog_periodic();
}

/** \brief Function for handling the RTC1 interrupts.
 */
void RTC1_IRQHandler()
{
  if(NRF_RTC1->EVENTS_COMPARE[1] == 1){
    nrf_gpio_cfg_output(RTC_SCHEDULE_PIN);
    nrf_gpio_pin_toggle(RTC_SCHEDULE_PIN);
    NRF_RTC1->EVENTS_COMPARE[1] = 0;
    // Disable COMPARE1 event and COMPARE1 interrupt:
    NRF_RTC1->EVTENCLR      = RTC_EVTENSET_COMPARE1_Msk;
    NRF_RTC1->INTENCLR      = RTC_INTENSET_COMPARE1_Msk;
    //printf("poll\n");
    NRF_RTC1->TASKS_STOP = 1;
    NVIC_DisableIRQ(RTC1_IRQn);
  }
}
#endif