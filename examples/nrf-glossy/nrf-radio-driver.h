#ifndef _NRF_RADIO_DRIVER_H_
#define _NRF_RADIO_DRIVER_H_

#ifndef RADIO_TEST_TX_CARRIER
#define RADIO_TEST_TX_CARRIER false
#endif

#define TRIGGER_RADIO_START_WITH_TIMER (true)
#define NRF_RADIO_DEBUG_STATE true
#define RADIO_REV_C_OR_RADIO_REV_1 false
/*---------------------------------------------------------------------------*/
#define US_TO_RTIMERTICKS(D)    ((int64_t)(D) << 4L)
#define RTIMERTICKS_TO_US(T)    ((int64_t)(T) >> 4L)
#define RTIMERTICKS_TO_US_64(T) RTIMERTICKS_TO_US(T)
/*---------------------------------------------------------------------------*/
#define BLE_MODE_BIT_TIME_X2(M) ( (M==RADIO_MODE_MODE_Ble_1Mbit) ? 2 : ((M==RADIO_MODE_MODE_Ble_2Mbit) ? 1 : ((M==RADIO_MODE_MODE_Ble_LR500Kbit) ? 4 : ((M==RADIO_MODE_MODE_Ble_LR125Kbit) ? 16 : 0))) )
#define PACKET_PAYLOAD_AIR_TIME(S,M) (US_TO_RTIMERTICKS((4*(S))*BLE_MODE_BIT_TIME_X2(M))) /* 8*size_in-bytes*symbol_time_times_2/2 */
#define PACKET_HEADER_AIR_TIME(M) US_TO_RTIMERTICKS( (M==RADIO_MODE_MODE_Ble_1Mbit) ? 8+4*8 : ((M==RADIO_MODE_MODE_Ble_2Mbit) ? 8+4*8*2 : ((M==RADIO_MODE_MODE_Ble_LR500Kbit) || (M==RADIO_MODE_MODE_Ble_LR125Kbit) ? 80+256+16+24 : 0)) )
#define PACKET_CRC_FOOTER_AIR_TIME(M) ((US_TO_RTIMERTICKS(4*CRC_LEN*BLE_MODE_BIT_TIME_X2(M))) + 3*BLE_MODE_BIT_TIME_X2(M)*((M==RADIO_MODE_MODE_Ble_LR500Kbit)||(M==RADIO_MODE_MODE_Ble_LR125Kbit))/2)
#define PACKET_AIR_TIME(S,M) (PACKET_HEADER_AIR_TIME(M)+PACKET_CRC_FOOTER_AIR_TIME(M)+PACKET_PAYLOAD_AIR_TIME(S,M))
/*---------------------------------------------------------------------------*/
#define USE_WHITENING 1
#ifndef BLE_DEFAULT_RF_POWER
#define BLE_DEFAULT_RF_POWER (256-40) /* -40dBm */
#endif
#ifndef RADIO_MODE_CONF
  #ifdef NRF51
  #define RADIO_MODE_CONF RADIO_MODE_MODE_Ble_1Mbit
  #else
  #define RADIO_MODE_CONF RADIO_MODE_MODE_Ble_1Mbit //RADIO_MODE_MODE_Ble_2Mbit //RADIO_MODE_MODE_Ble_1Mbit //RADIO_MODE_MODE_Ble_LR125Kbit //RADIO_MODE_MODE_Ble_LR500Kbit
  #endif
#endif
/*---------------------------------------------------------------------------*/
#ifndef OVERRIDE_BLE_CHANNEL_37
#define OVERRIDE_BLE_CHANNEL_37 false
#endif /* ifndef OVERRIDE_BLE_CHANNEL_37 */
#if OVERRIDE_BLE_CHANNEL_37
#define BLE_CHANNEL_37_FREQ 98 /* 2498MHz, outside BLE, WiFi and ZigBee channels */
#else
#define BLE_CHANNEL_37_FREQ 2 /* 2402MHz, standard */
#endif /* OVERRIDE_BLE_CHANNEL_37 */
#ifndef BLE_DEFAULT_CHANNEL
#define BLE_DEFAULT_CHANNEL 37
#endif /* ifndef BLE_DEFAULT_CHANNEL */
/*---------------------------------------------------------------------------*/
/* RF center frequency for each channel index (offset from 2400 MHz) */
#ifndef NUMBER_OF_CHANNELS
#define NUMBER_OF_CHANNELS 1
#endif /* ifndef NUMBER_OF_CHANNELS */
extern const uint8_t ble_hw_frequency_channels[40];
extern const uint8_t ble_channels_list[NUMBER_OF_CHANNELS];

#if (NUMBER_OF_CHANNELS <= 1)

  extern const uint8_t ble_hopping_list[1];
  #define HOPPING_LIST ((uint8_t[1]){0,})
  #define CHANNEL_LIST ( (uint8_t[1]){BLE_DEFAULT_CHANNEL,} )
  #define GET_CHANNEL(round,slot) (ble_hw_frequency_channels[BLE_DEFAULT_CHANNEL])

#else  /* NUMBER_OF_CHANNELS <= 40 */

  #if NUMBER_OF_CHANNELS == 3
  extern const uint8_t ble_hopping_list[3];
  #define HOPPING_LIST ((uint8_t[3]){0,1,2,})
  #define CHANNEL_LIST ( (uint8_t[NUMBER_OF_CHANNELS]){37,37,37,} )

  #else
  extern const uint8_t ble_hopping_list[128];
  #define HOPPING_LIST ((uint8_t[128]){26,33,0,32,7,11,2,22,23,24,1,29,16,21,15,3,34,13,9,28,35,10,6,5,31,30,36,18,27,4,19,14,12,8,37,39,17,25,38,20,17,14,10,24,11,26,6,7,39,35,3,30,28,23,27,32,12,5,21,38,36,29,1,34,25,20,33,2,4,31,22,9,19,16,8,0,18,37,15,13,35,5,22,10,2,26,27,18,33,34,6,3,19,25,15,29,16,36,20,13,30,12,0,11,14,28,31,17,9,7,23,24,32,4,38,37,8,39,1,21,10,35,32,5,39,12,16,11})

  #if NUMBER_OF_CHANNELS == 40
  #define CHANNEL_LIST ((uint8_t[NUMBER_OF_CHANNELS]){26,33,0,32,7,11,2,22,23,24,1,29,16,21,15,3,34,13,9,28,35,10,6,5,31,30,36,18,27,4,19,14,12,8,37,39,17,25,38,20})
  #elif NUMBER_OF_CHANNELS == 18
  #define CHANNEL_LIST ( (uint8_t[NUMBER_OF_CHANNELS]){0,5,10,15,20,25,37,38,39,3,8,13,18,23,28,33,7,17,} )
  #elif NUMBER_OF_CHANNELS == 9
  #define CHANNEL_LIST ( (uint8_t[NUMBER_OF_CHANNELS]){0,5,10,15,20,25,37,38,39,} )
  #else /* NUMBER_OF_CHANNELS */
  #error "!BlueFlood supports 1,3,9,18,40 channels only"
  #endif /* NUMBER_OF_CHANNELS == 40,18,9 */

  #endif /* NUMBER_OF_CHANNELS == 3 */
  
  #define HOP_INDEX(round,slot) (((round)*16+(slot))%sizeof(ble_hopping_list))
  //#define GET_CHANNEL(round,slot) ( ( 40-(NUMBER_OF_CHANNELS) ) + ( (round)*16 + (slot) )%(NUMBER_OF_CHANNELS) )
  #define GET_CHANNEL(round,slot) (ble_hw_frequency_channels[ble_channels_list[ble_hopping_list[HOP_INDEX(round,slot)]%NUMBER_OF_CHANNELS]])

#endif /* NUMBER_OF_CHANNELS */


/*---------------------------------------------------------------------------*/
#if USE_WHITENING
#define WHITENING_CONF RADIO_PCNF1_WHITEEN_Enabled
#else
#define WHITENING_CONF RADIO_PCNF1_WHITEEN_Disabled
#endif
/*---------------------------------------------------------------------------*/
// #if USE_HAMMING_CODE
// #define CRC_LEN 0
// #else
// #define CRC_LEN 3
// #endif
#define CRC_LEN 3
#define RADIO_PACKET_MAX_LEN 251
/*---------------------------------------------------------------------------*/
#define BLE_LL_CRCINIT_ADV      (0x555555UL)
/* Access address for advertising channels */
#define BLE_ACCESS_ADDR_ADV      (0x8E89BED6UL)
/*---------------------------------------------------------------------------*/
/* Timing */
#define MY_RADIO_FAST_RAMPUP 1
#ifdef NRF51
#define MY_RADIO_RAMPUP_TIME_US (140)
#else
#if MY_RADIO_FAST_RAMPUP
#define MY_RADIO_RAMPUP_TIME_US (40)
#else
#define MY_RADIO_RAMPUP_TIME_US (140)
#endif
#endif
#define MY_RADIO_RAMPUP_TIME_TIMER1_TICKS (US_TO_RTIMERTICKS(MY_RADIO_RAMPUP_TIME_US))

#define CRC_AIR_T PACKET_CRC_FOOTER_AIR_TIME(RADIO_MODE_CONF)

#if RADIO_MODE_CONF == RADIO_MODE_MODE_Ble_1Mbit
//all times assume 1Mbps --> 1 bit = 1 us
#define EXTRA_TIME_ADDRESS_EVENT_T_TX_OFFSET 0
#define EXTRA_TIME_TX_CHAIN_DELAY_US_X32 0

#elif RADIO_MODE_CONF == RADIO_MODE_MODE_Ble_2Mbit
//all times assume 2Mbps --> 2 bit = 1 us
#define EXTRA_TIME_ADDRESS_EVENT_T_TX_OFFSET (-248)
#define EXTRA_TIME_TX_CHAIN_DELAY_US_X32 (-157)

#elif RADIO_MODE_CONF == RADIO_MODE_MODE_Ble_LR500Kbit
//all times assume 0.5Mbps --> 1 bit = 2 us
#define EXTRA_TIME_ADDRESS_EVENT_T_TX_OFFSET (4559) //464.24us*16=7428 //80+256
#define EXTRA_TIME_TX_CHAIN_DELAY_US_X32 (2365-335-5+3) //73.92us*16=1183 * 2 = 2365.44 ... -5 to compensate for variations

#elif RADIO_MODE_CONF == RADIO_MODE_MODE_Ble_LR125Kbit
//all times assume 1/8Mbps --> 1 bit = 8 us
#define EXTRA_TIME_ADDRESS_EVENT_T_TX_OFFSET (4559)//(7428-2869) //464.24us*16=7428
#define EXTRA_TIME_TX_CHAIN_DELAY_US_X32 (2365-335-5+3) //73.92us*16=1183 * 2 = 2365.44 ... -5 to compensate for variations

#endif /* RADIO_MODE_CONF */

#if MY_RADIO_FAST_RAMPUP
#define ADDRESS_EVENT_T_TX_OFFSET_RAMPUP (-1600)
#else
#define ADDRESS_EVENT_T_TX_OFFSET_RAMPUP (0)
#endif /* MY_RADIO_FAST_RAMPUP */

#ifdef NRF51
//with 140us waiting to rampup using timer1 for deterministic timing
//(137.24 "rampup" + 49.24 "ready to address")=186.48 us *16 = 2983.68 ticks
// 6 "for preamble, address and prefix"
// 5.6 tx chain delay (address on tx to address on rx)
#define TX_CHAIN_DELAY_US_X32 (180) //5.6us * 32
#define TX_CHAIN_DELAY (((US_TO_RTIMERTICKS(TX_CHAIN_DELAY_US_X32)+(1<<4U))>>5U)) //+(1<<4U) for rounding up, since we scale the number up with a factor of 32
#define ADDRESS_EVENT_T_TX_OFFSET (2984-TX_CHAIN_DELAY-22-16*(!TRIGGER_RADIO_START_WITH_TIMER)) //3044U = 190.25 us - instructions delay or cpu delay -- 3044U-128-44

#elif defined(NRF52840_XXAA)
#define TX_CHAIN_DELAY_US_X32 (335+EXTRA_TIME_TX_CHAIN_DELAY_US_X32) //10.42us * 32 = 333.44
#define TX_CHAIN_DELAY (((US_TO_RTIMERTICKS(TX_CHAIN_DELAY_US_X32)+(1<<4U))>>5U)) //+(1<<4U) for rounding up, since we scale the number up with a factor of 32
#define ADDRESS_EVENT_T_TX_OFFSET (2869+ADDRESS_EVENT_T_TX_OFFSET_RAMPUP+1+EXTRA_TIME_ADDRESS_EVENT_T_TX_OFFSET-16*(!TRIGGER_RADIO_START_WITH_TIMER)) //2959.36 = 184.96 us --  2959U-128-44+160

#endif /* NRF51 elif defined(NRF52840_XXAA) */
/*---------------------------------------------------------------------------*/
/*
 * We configure the nrf with a 1 byte S0 field, 8 bit length field, and
 * zero bit S1 field. The preamble is 8 bits long.
 */
#define NRF_LFLEN_BITS          (8)
#define NRF_S0LEN               (1)
#define NRF_S1LEN_BITS          (0)
#define NRF_CILEN_BITS          (2)
#define NRF_TERMLEN_BITS        (3)
/* NRF_RADIO->PCNF0 configuration values */
#define NRF_PCNF0               (NRF_LFLEN_BITS << RADIO_PCNF0_LFLEN_Pos) | \
                                (RADIO_PCNF0_S1INCL_Msk) | \
                                (NRF_S0LEN << RADIO_PCNF0_S0LEN_Pos) | \
                                (NRF_S1LEN_BITS << RADIO_PCNF0_S1LEN_Pos)
#define NRF_PCNF0_1M            (NRF_PCNF0) | \
                                (RADIO_PCNF0_PLEN_8bit << RADIO_PCNF0_PLEN_Pos)
#define NRF_PCNF0_2M            (NRF_PCNF0) | \
                                (RADIO_PCNF0_PLEN_16bit << RADIO_PCNF0_PLEN_Pos)
#define NRF_PCNF0_CODED         (NRF_PCNF0) | \
                                (RADIO_PCNF0_PLEN_LongRange << RADIO_PCNF0_PLEN_Pos) | \
                                (NRF_CILEN_BITS << RADIO_PCNF0_CILEN_Pos) | \
                                (NRF_TERMLEN_BITS << RADIO_PCNF0_TERMLEN_Pos)
/* Packet configuration 
    Little endian, 
    base address: 3 bytes (+1 prefix ==> 4bytes for BLE access address)
    0 static packet length
    and configure max packet length */
#define NRF_PCNF1               ( (WHITENING_CONF << RADIO_PCNF1_WHITEEN_Pos)           | \
                                  (RADIO_PCNF1_ENDIAN_Little << RADIO_PCNF1_ENDIAN_Pos) | \
                                  (3UL << RADIO_PCNF1_BALEN_Pos)          | \
                                  (0 << RADIO_PCNF1_STATLEN_Pos)          | \
                                  (RADIO_PACKET_MAX_LEN << RADIO_PCNF1_MAXLEN_Pos))                                
/*---------------------------------------------------------------------------*/
/* Output radio state on GPIO */
#define PORT(P,PIN)                     (P*32uL+PIN)
#define RADIO_ADDRESS_EVENT_PIN         PORT(1,10) // address      
// #define RADIO_READY_EVENT_PIN           PORT(1,11) // disabled
#define ROUND_INDICATOR_PIN           PORT(1,11) 

#define RADIO_TXEN_PIN                  PORT(1,12) // txen 
#define RADIO_RXEN_PIN                  PORT(1,13) // rxen 
// #define RADIO_PAYLOAD_PIN            PORT(1,14) // payload

// Peripheral channel assignments
#define RADIO_ADDRESS_EVENT_GPIOTE_CH   0UL //PPI
#define RADIO_READY_EVENT_GPIOTE_CH     1UL //PPI
#define RADIO_TXEN_GPIOTE_CH     2UL //manual control
#define RADIO_RXEN_GPIOTE_CH     3UL //manual control
#define RADIO_PAYLOAD_GPIOTE_CH     4UL //manual control

#define RADIO_ADDRESS_EVENT_PPI_CH      0UL
#define RADIO_END_EVENT_PPI_CH          1UL
#define RADIO_READY_EVENT_PPI_CH        2UL
#define RADIO_T0_TX_EVENT_PPI_CH     3UL
#define RADIO_T0_RX_EVENT_PPI_CH     4UL
// #define RADIO_PAYLOAD_EVENT_PPI_CH     5UL
#define RADIO_DISABLED_EVENT_PPI_CH 6UL

#define TIMER0C0_TIMER1_START_PPI_CH 7UL
#define TIMER1_RADIO_START_PPI_CH 8UL

#if 0
#define DEBUG_GPIO_OUTPUT_TXEN() do{ NRF_GPIOTE->TASKS_OUT[RADIO_TXEN_GPIOTE_CH] = 1UL; }while(0)
#define DEBUG_GPIO_OUTPUT_RXEN() do{ NRF_GPIOTE->TASKS_OUT[RADIO_RXEN_GPIOTE_CH] = 1UL; }while(0)
#else
#define DEBUG_GPIO_OUTPUT_TXEN() do{ }while(0)
#define DEBUG_GPIO_OUTPUT_RXEN() do{ }while(0)
#endif
/*---------------------------------------------------------------------------*/
#define BUSYWAIT_UNTIL(cond, max_time) \
  do { \
    volatile rtimer_clock_t t0; \
    t0 = RTIMER_NOW(); \
    while(!(cond) && RTIMER_CLOCK_LT(RTIMER_NOW(), t0 + (max_time))); \
  } while(0)
#define MAX(A,B) (((A)>(B))? (A) : (B))
#define get_rx_rssi() ((int8_t)(-(int8_t)(NRF_RADIO->RSSISAMPLE)))
#define get_rx_ts() (NRF_TIMER0->CC[TIMESTAMP_ADDR_REG])
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)
#define RADIO_MODE_TO_STR(X)  (((X) == RADIO_MODE_MODE_Ble_1Mbit) ? "Ble_1Mbit" : \
                              (((X) == RADIO_MODE_MODE_Ble_2Mbit) ? "Ble_2Mbit" : \
                              (((X) == RADIO_MODE_MODE_Ble_LR125Kbit) ? "Ble_LR125Kbit" : \
                              (((X) == RADIO_MODE_MODE_Ble_LR500Kbit) ? "Ble_LR500Kbit" : TOSTRING(X)) ) ) ) 
/*---------------------------------------------------------------------------*/
void my_radio_init(uint32_t* my_id, void* my_tx_buffer);
void my_radio_set_tx_power(uint8_t p);
void my_radio_off_to_tx(void);
void my_radio_off_to_rx(void);
uint8_t my_radio_rx(uint8_t* buf, int channel);
void my_radio_send(uint8_t* buf, int channel);
void schedule_rx_abs(uint8_t* buf, int channel, rtimer_clock_t t_abs);
void my_radio_off_completely(void);
/* TX after t ticks from the last timestamp */
void schedule_tx_t_from_rx(uint8_t* buf, int channel, rtimer_clock_t t);
/* TX at t_abs */
void schedule_tx_abs(uint8_t* buf, int channel, rtimer_clock_t t_abs);
/* TX after t ticks from now */
void schedule_tx(uint8_t* buf, int channel, rtimer_clock_t t);
void testbed_cofigure_pins();
#endif /* _NRF_RADIO_DRIVER_H_ */
