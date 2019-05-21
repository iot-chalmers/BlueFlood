#include <stdio.h> /* For printf() */
#include <inttypes.h>
#include "contiki.h"
//#include "dev/radio.h"
#include "clock-nrf.h"
#include "nrf-radio.h"
#include "nrf.h"
#include "nrf-radio-driver.h"

/*---------------------------------------------------------------------------*/
/*
 * Advertising PDU format:
 * -> 2 byte header
 *      -> LSB contains pdu type, txadd and rxadd bits.
 *      -> MSB contains length (6 bits). Length is length of payload. Does
 *         not include the header length itself.
 * -> Payload (max 37 bytes)
 */
#define BLE_ADV_PDU_HDR_TYPE_MASK           (0x0F)
#define BLE_ADV_PDU_HDR_CHSEL_MASK          (0x20)
#define BLE_ADV_PDU_HDR_TXADD_MASK          (0x40)
#define BLE_ADV_PDU_HDR_RXADD_MASK          (0x80)
/*---------------------------------------------------------------------------*/
/* RF center frequency for each channel index (offset from 2400 MHz) */
const uint8_t ble_hw_frequency_channels[40] = {
     4,  6,  8, 10, 12, 14, 16, 18, 20, 22, /* 0-9 */
    24, 28, 30, 32, 34, 36, 38, 40, 42, 44, /* 10-19 */
    46, 48, 50, 52, 54, 56, 58, 60, 62, 64, /* 20-29 */
    66, 68, 70, 72, 74, 76, 78,  BLE_CHANNEL_37_FREQ, 26, 80, /* 30-39 */
};
const uint8_t ble_channels_list[] = CHANNEL_LIST;
const uint8_t ble_hopping_list[] = HOPPING_LIST;
static uint8_t nrf_tx_power = BLE_DEFAULT_RF_POWER;
/* Bluetooth advertising channels: 0-39 */
// const uint8_t ble_adv_channels[128] = {3, 15, 12, 21, 8, 6, 3, 20, 8, 30, 3, 8, 23, 36, 3, 34, 5, 9, 21, 14, 26, 35, 7, 29, 38, 4, 18, 15, 28, 26, 23, 39, 1, 4, 38, 24, 23, 9, 32, 21, 6, 30, 11, 4, 20, 37, 33, 14, 35, 15, 24, 37, 36, 33, 19, 20, 34, 22, 29, 34, 27, 23, 21, 12, 30, 33, 11, 4, 17, 14, 33, 20, 15, 22, 38, 37, 34, 31, 3, 2, 10, 6, 14, 10, 15, 16, 1, 4, 7, 1, 24, 33, 5, 11, 21, 33, 13, 11, 33, 24, 8, 9, 12, 4, 22, 37, 4, 27, 34, 29, 11, 13, 36, 21, 11, 3, 26, 30, 8, 1, 38, 14, 27, 36, 13, 20, 5, 8};
/*---------------------------------------------------------------------------*/
static void output_radio_events_gpio_init(void);
static void timer1_init(void);
/*---------------------------------------------------------------------------*/
void my_radio_set_tx_power(uint8_t p){
  nrf_tx_power = p;
}
/*---------------------------------------------------------------------------*/
void my_radio_get_id(uint32_t* my_id){  
  if(my_id != NULL){
    *my_id = NRF_FICR->DEVICEID[0];
    //*my_id = NRF_FICR->DEVICEID[1] << (uint64_t)32;
  }
}
/*---------------------------------------------------------------------------*/
void my_radio_init(uint32_t* my_id, void* my_tx_buffer) 
{
  /* Reset all states in the radio peripheral */
  NRF_RADIO->POWER = 0;
  NRF_RADIO->POWER = 1;

  //disable interrupt
  nrf_radio_set_interrupt_enable(0);

  /* Radio config */
  my_radio_set_tx_power(BLE_DEFAULT_RF_POWER);
  NRF_RADIO->TXPOWER = nrf_tx_power;
  NRF_RADIO->FREQUENCY = ble_hw_frequency_channels[BLE_DEFAULT_CHANNEL];
  NRF_RADIO->DATAWHITEIV = BLE_DEFAULT_CHANNEL;     /* This value needs to correspond to the channel being used */

  #ifdef NRF51
  NRF_RADIO->MODECNF0 = (RADIO_MODECNF0_DTX_Center << RADIO_MODECNF0_DTX_Pos);
  //nrf_radio_set_radio_mode(RADIO_MODE_MODE_Nrf_1Mbit);
  nrf_radio_set_radio_mode(RADIO_MODE_CONF);

  #else 
  NRF_RADIO->MODECNF0 = (RADIO_MODECNF0_RU_Fast << RADIO_MODECNF0_RU_Pos) 
                      | (RADIO_MODECNF0_DTX_Center << RADIO_MODECNF0_DTX_Pos);
  NRF_RADIO->MODE = RADIO_MODE_CONF;
  #endif

  /* Packet configuration 
    BLE compatible: S0: 1 byte, LEN: 8bits, S1: 0 bits */
  /*
    * Workarounds should be applied only when switching to/from LE Coded PHY
    * so no need to apply them every time.
    *
    * nRF52840 Engineering A Errata v1.2
    * [164] RADIO: Low sensitivity in long range mode
    *
    * nRF52840 Rev 1 Errata
    * [191] RADIO: High packet error rate in BLE Long Range mode
    */    
  #if (RADIO_MODE_CONF == RADIO_MODE_MODE_Ble_1Mbit || RADIO_MODE_CONF == RADIO_MODE_MODE_Ble_2Mbit)
  
  NRF_RADIO->PCNF0 = (RADIO_MODE_CONF == RADIO_MODE_MODE_Ble_1Mbit) ? NRF_PCNF0_1M : NRF_PCNF0_2M; 

  /* [164] */
  *(volatile uint32_t *)0x4000173C &= ~0x80000000;
  #if(RADIO_REV_C_OR_RADIO_REV_1) //for radio chip revision c and revision 1
  /* [191] */
  *(volatile uint32_t *) 0x40001740 =
                  ((*((volatile uint32_t *) 0x40001740)) & 0x7FFFFFFF);
  #endif
  #elif (RADIO_MODE_CONF == RADIO_MODE_MODE_Ble_LR125Kbit || RADIO_MODE_CONF == RADIO_MODE_MODE_Ble_LR500Kbit)

  NRF_RADIO->PCNF0 = NRF_PCNF0_CODED; 
  
  /* [164] */
  *(volatile uint32_t *)0x4000173C |= 0x80000000;
  *(volatile uint32_t *)0x4000173C =
                  ((*(volatile uint32_t *)0x4000173C & 0xFFFFFF00) | 0x5C);
  #if(RADIO_REV_C_OR_RADIO_REV_1) //for radio chip revision c and revision 1
  /* [191] */
  *(volatile uint32_t *) 0x40001740 =
                  ((*((volatile uint32_t *) 0x40001740)) & 0x7FFF00FF) |
                  0x80000000 | (((uint32_t)(196)) << 8);
  #endif
  #endif

  NRF_RADIO->PCNF1 = NRF_PCNF1; 

  /* Configure default buffer */
  NRF_RADIO->PACKETPTR = (uint32_t)my_tx_buffer;

  /* CRC Config */

  if(CRC_LEN == 0){
    NRF_RADIO->CRCINIT = 0x0UL;      /* CRC initial value */
    NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Disabled << RADIO_CRCCNF_LEN_Pos); // Number of checksum bits
  } else if(CRC_LEN == 3){
    NRF_RADIO->CRCCNF = RADIO_CRCCNF_SKIP_ADDR_Msk | (RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos); // Number of checksum bits
    //NRF_RADIO->CRCINIT = 0x0FFFFFFFUL;    /* Initial value of CRC */
    //NRF_RADIO->CRCPOLY = 0x17b01bd;    /* CRC polynomial function gives HD=6 for upto 2026bits https://users.ece.cmu.edu/~koopman/crc/hd6.html https://users.ece.cmu.edu/~koopman/crc/index.html */
    //NRF_RADIO->CRCINIT = 0x555555UL;                                                  /* Initial value of CRC */
    //NRF_RADIO->CRCPOLY = 0x00065BUL;                                                  /* CRC polynomial function */
    NRF_RADIO->CRCPOLY = 0x0100065B;
    NRF_RADIO->CRCINIT = BLE_LL_CRCINIT_ADV;

  }

  NRF_RADIO->SHORTS = (RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos);

  #if !TRIGGER_RADIO_START_WITH_TIMER
  NRF_RADIO->SHORTS |= (RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos);
  #endif
  
  //RSSI shorts
  NRF_RADIO->SHORTS |= RADIO_SHORTS_ADDRESS_RSSISTART_Msk;
  NRF_RADIO->SHORTS |= RADIO_SHORTS_DISABLED_RSSISTOP_Msk;

  // NRF_RADIO->TEST       = (RADIO_TEST_CONST_CARRIER_Enabled << RADIO_TEST_CONST_CARRIER_Pos)
	// 		                     | (RADIO_TEST_PLL_LOCK_Enabled << RADIO_TEST_PLL_LOCK_Pos);

  //NRF_RADIO->BASE0   = 0x89BED600UL; 
  NRF_RADIO->BASE1   = 0x43434343UL;  
  NRF_RADIO->PREFIX0 = 0x23C34300UL; 
  NRF_RADIO->PREFIX1 = 0x13E363A3UL; 

  /* Set base0 with the advertising access address */
  NRF_RADIO->BASE0 = (BLE_ACCESS_ADDR_ADV << 8) & 0xFFFFFF00;
  NRF_RADIO->PREFIX0 |= (BLE_ACCESS_ADDR_ADV >> 24) & 0xFF;

  #if 0
  NRF_RADIO->BASE0   = 0UL;//0xE7E7E7E7;
  NRF_RADIO->BASE1   = 0UL; //0x43434343
  NRF_RADIO->PREFIX0 = 0x55; //0x23C343E7
  NRF_RADIO->PREFIX1 = 0x55; //0x13E363A3
  #endif

  /* Use logical address 0 (prefix0 + base0) = 0x8E89BED6 when transmitting and receiving */
  NRF_RADIO->TXADDRESS = 0x00;
  NRF_RADIO->RXADDRESSES = 0x01;
	/* Pre-defined channel: Address Event -> Timer0-TASK_CAPTURE[1] */
	NRF_PPI->CHEN |= (PPI_CHEN_CH26_Enabled << PPI_CHEN_CH26_Pos);

  my_radio_get_id(my_id);

  timer1_init();
  output_radio_events_gpio_init();
}
/*---------------------------------------------------------------------------*/
/* TX after t ticks from now */
void schedule_tx(uint8_t* buf, int channel, rtimer_clock_t t)
{
  /* captrue timer and put now in cc 0*/
  NRF_TIMER0->TASKS_CAPTURE[NOW_REG] = 1UL;
  rtimer_clock_t t_abs = t + NRF_TIMER0->CC[NOW_REG];
  /* Put the scheduled time in a compare register */
  NRF_TIMER0->CC[0] = t_abs;
  my_radio_off_to_tx();
  NRF_RADIO->FREQUENCY = (uint8_t)ble_hw_frequency_channels[channel];
  NRF_RADIO->DATAWHITEIV = channel;     /* This value needs to correspond to the channel being used */
	NRF_RADIO->PACKETPTR = (uint32_t)buf;
}
/*---------------------------------------------------------------------------*/
/* TX at t_abs */
void schedule_tx_abs(uint8_t* buf, int channel, rtimer_clock_t t_abs)
{
  /* Put the scheduled time in a compare register */
  my_radio_off_to_tx();
  NRF_TIMER0->CC[0] = t_abs;
  NRF_RADIO->FREQUENCY = (uint8_t)ble_hw_frequency_channels[channel];
  NRF_RADIO->DATAWHITEIV = channel;     /* This value needs to correspond to the channel being used */
	NRF_RADIO->PACKETPTR = (uint32_t)buf;
}
/*---------------------------------------------------------------------------*/
/* TX after t ticks from the last timestamp */
void schedule_tx_t_from_rx(uint8_t* buf, int channel, rtimer_clock_t t)
{
  rtimer_clock_t t_abs = t + get_rx_ts(); //CC[1]
  /* Put the scheduled time in a compare register */
  NRF_TIMER0->CC[0] = t_abs;
  my_radio_off_to_tx();
  NRF_RADIO->FREQUENCY = (uint8_t)ble_hw_frequency_channels[channel];
  NRF_RADIO->DATAWHITEIV = channel;     /* This value needs to correspond to the channel being used */
	NRF_RADIO->PACKETPTR = (uint32_t)buf;
}
/*---------------------------------------------------------------------------*/
void my_radio_off_completely(void)
{
  /* Disable pscheduled tx */
  NRF_PPI->CHEN |= (PPI_CHEN_CH20_Disabled << PPI_CHEN_CH20_Pos);
  NRF_TIMER0->EVENTS_COMPARE[0] = 0U;
  //NRF_TIMER1->TASKS_STOP  = 1;
  //NRF_TIMER1->TASKS_CLEAR = 1;
  NRF_TIMER1->TASKS_SHUTDOWN  = 1;
  /* Enable the Compare event on channel 0 */
  NRF_TIMER1->EVENTS_COMPARE[0] = 0;
  /* count 140 us waiting for radio rampup time */
  NRF_TIMER1->CC[0]       = MY_RADIO_RAMPUP_TIME_TIMER1_TICKS; 

  /* Clear event register */
  NRF_RADIO->EVENTS_DISABLED = 0U;
  /* Disable radio */
  NRF_RADIO->TASKS_DISABLE = 1U;
  while(NRF_RADIO->EVENTS_DISABLED == 0U);
  NRF_RADIO->EVENTS_DISABLED = 0U;
  NRF_RADIO->EVENTS_END = 0U;
  NRF_RADIO->EVENTS_ADDRESS = 0U;
  NRF_RADIO->EVENTS_PAYLOAD = 0U;
  NRF_RADIO->EVENTS_READY = 0U;

  #if NRF_RADIO_DEBUG_STATE
  NRF_PPI->CHEN &= ~(1 << RADIO_T0_TX_EVENT_PPI_CH); //disable TXen debug pin PPI
  NRF_PPI->CHEN &= ~(1 << RADIO_T0_RX_EVENT_PPI_CH); //disable RXen debug pin PPI
  #endif /* NRF_RADIO_DEBUG_STATE */

  NRF_RADIO->TXPOWER = nrf_tx_power;
}
/*---------------------------------------------------------------------------*/
void my_radio_off_to_tx(void)
{
  /* Enable scheduled tx 
   * Pre-defined channel: Timer0-compare[0] -> TxEn */
	NRF_PPI->CHEN |= (PPI_CHEN_CH20_Enabled << PPI_CHEN_CH20_Pos);
  /* DISABLE RX SCHEDULING SHORTCUT */
	NRF_PPI->CHEN &= ~(PPI_CHEN_CH21_Enabled << PPI_CHEN_CH21_Pos);
  #if NRF_RADIO_DEBUG_STATE
  NRF_PPI->CHEN |= (1 << RADIO_T0_TX_EVENT_PPI_CH); //enable TXen debug pin PPI --> Timer 0 compare event toggles TXen debug pin
  NRF_PPI->CHEN &= ~(1 << RADIO_T0_RX_EVENT_PPI_CH); //disable RXen debug pin PPI
  #endif /* NRF_RADIO_DEBUG_STATE */
  //NRF_TIMER1->TASKS_STOP  = 1;
  //NRF_TIMER1->TASKS_CLEAR = 1;
  NRF_TIMER1->TASKS_SHUTDOWN  = 1;
  /* Enable the Compare event on channel 0 */
  NRF_TIMER1->EVENTS_COMPARE[0] = 0;
  /* count 140 us waiting for radio rampup time */
  NRF_TIMER1->CC[0]       = MY_RADIO_RAMPUP_TIME_TIMER1_TICKS; 

  NRF_TIMER0->EVENTS_COMPARE[0] = 0;
  /* Clear event register */
  NRF_RADIO->EVENTS_DISABLED = 0U;
  /* Disable radio */
  NRF_RADIO->TASKS_DISABLE = 1U;
  while(NRF_RADIO->EVENTS_DISABLED == 0U);
  NRF_RADIO->EVENTS_DISABLED = 0U;
  NRF_RADIO->EVENTS_END = 0U;
  NRF_RADIO->EVENTS_ADDRESS = 0U;
  NRF_RADIO->EVENTS_READY = 0U;
  NRF_RADIO->TXPOWER = nrf_tx_power;
}
/*---------------------------------------------------------------------------*/
/* TX at t_abs */
void schedule_rx_abs(uint8_t* buf, int channel, rtimer_clock_t t_abs)
{
  /* Put the scheduled time in a compare register */
  my_radio_off_to_rx();
  NRF_TIMER0->CC[0] = t_abs;
  NRF_RADIO->FREQUENCY = (uint8_t)ble_hw_frequency_channels[channel];
  NRF_RADIO->DATAWHITEIV = channel;     /* This value needs to correspond to the channel being used */
	NRF_RADIO->PACKETPTR = (uint32_t)buf;
}
/*---------------------------------------------------------------------------*/
void my_radio_off_to_rx(void)
{
  /* Enable scheduled tx 
   * Pre-defined channel: Timer0-compare[0] -> RxEn */
	NRF_PPI->CHEN |= (PPI_CHEN_CH21_Enabled << PPI_CHEN_CH21_Pos);
  /* DISABLE TX SCHEDULING SHORTCUT */
  NRF_PPI->CHEN &= ~(PPI_CHEN_CH20_Enabled << PPI_CHEN_CH20_Pos);
  //NRF_TIMER1->TASKS_STOP  = 1;
  //NRF_TIMER1->TASKS_CLEAR = 1;
  NRF_TIMER1->TASKS_SHUTDOWN  = 1;
  /* Enable the Compare event on channel 0 */
  NRF_TIMER1->EVENTS_COMPARE[0] = 0;
  /* count 140 us waiting for radio rampup time */
  NRF_TIMER1->CC[0]       = MY_RADIO_RAMPUP_TIME_TIMER1_TICKS; 

  NRF_TIMER0->EVENTS_COMPARE[0] = 0;
  /* Clear event register */
  NRF_RADIO->EVENTS_DISABLED = 0U;
  /* Disable radio */
  NRF_RADIO->TASKS_DISABLE = 1U;
  while(NRF_RADIO->EVENTS_DISABLED == 0U);
  NRF_RADIO->EVENTS_DISABLED = 0U;
  NRF_RADIO->EVENTS_END = 0U;
  NRF_RADIO->EVENTS_ADDRESS = 0U;
  NRF_RADIO->EVENTS_PAYLOAD = 0U;
  NRF_RADIO->EVENTS_READY = 0U;

  #if NRF_RADIO_DEBUG_STATE
  NRF_PPI->CHEN |= (1 << RADIO_T0_RX_EVENT_PPI_CH); //enable RXen debug pin PPI
  NRF_PPI->CHEN &= ~(1 << RADIO_T0_TX_EVENT_PPI_CH); //disable TXen debug pin PPI
  #endif /* NRF_RADIO_DEBUG_STATE */
}
/*---------------------------------------------------------------------------*/
void my_radio_send(uint8_t* buf, int channel) 
{
  #if NRF_RADIO_DEBUG_STATE
  NRF_PPI->CHEN |= (1 << RADIO_T0_TX_EVENT_PPI_CH); //enable TXen debug pin PPI
  NRF_PPI->CHEN &= ~(1 << RADIO_T0_RX_EVENT_PPI_CH); //disable RXen debug pin PPI
  #endif /* NRF_RADIO_DEBUG_STATE */
  //NRF_TIMER1->TASKS_STOP  = 1;
  //NRF_TIMER1->TASKS_CLEAR = 1;
  NRF_TIMER1->TASKS_SHUTDOWN  = 1;
    /* Enable the Compare event on channel 0 */
  NRF_TIMER1->EVENTS_COMPARE[0] = 0;
  /* count 140 us waiting for radio rampup time */
  NRF_TIMER1->CC[0]       = MY_RADIO_RAMPUP_TIME_TIMER1_TICKS; 
  /* Clear event register */
  NRF_RADIO->EVENTS_DISABLED = 0U;
  NRF_RADIO->EVENTS_END = 0U;
  NRF_RADIO->EVENTS_ADDRESS = 0U;
  NRF_RADIO->EVENTS_PAYLOAD = 0U;
  NRF_RADIO->FREQUENCY = (uint8_t)ble_hw_frequency_channels[channel];
  NRF_RADIO->DATAWHITEIV = channel;     /* This value needs to correspond to the channel being used */
  NRF_RADIO->PACKETPTR = (uint32_t)buf;
  NRF_RADIO->TXPOWER = nrf_tx_power;
  DEBUG_GPIO_OUTPUT_TXEN();
  NRF_RADIO->EVENTS_READY = 0;		/* Clear ready register */
  NRF_RADIO->TASKS_TXEN = 1;		/* Enable the radio in TX mode */
  //while(NRF_RADIO->EVENTS_READY == 0U);	/* Wait for the radio to ramp up */
  //NRF_RADIO->TASKS_START = 1;		/* Start the transmission */
  #if TRIGGER_RADIO_START_WITH_TIMER && !RADIO_TEST_TX_CARRIER
    NRF_TIMER1->TASKS_START = 1;
  #endif
  //printf("DIRTY CHANNEL --- %u ---\n", channel);
  //while(NRF_RADIO->EVENTS_END == 0U);

}
/*---------------------------------------------------------------------------*/
uint8_t my_radio_rx(uint8_t* buf, int channel) 
{
  #if NRF_RADIO_DEBUG_STATE
  NRF_PPI->CHEN |= (1 << RADIO_T0_RX_EVENT_PPI_CH); //enable RXen debug pin PPI
  NRF_PPI->CHEN &= ~(1 << RADIO_T0_TX_EVENT_PPI_CH); //disable TXen debug pin PPI
  #endif /* NRF_RADIO_DEBUG_STATE */
  //NRF_TIMER1->TASKS_STOP  = 1;
  //NRF_TIMER1->TASKS_CLEAR = 1;
  NRF_TIMER1->TASKS_SHUTDOWN  = 1;
  /* Enable the Compare event on channel 0 */
  NRF_TIMER1->EVENTS_COMPARE[0] = 0;
  /* count 140 us waiting for radio rampup time */
  NRF_TIMER1->CC[0]       = MY_RADIO_RAMPUP_TIME_TIMER1_TICKS; 
  /* Clear event register */
  NRF_RADIO->EVENTS_DISABLED = 0U;
  NRF_RADIO->EVENTS_END = 0U;
  NRF_RADIO->EVENTS_ADDRESS = 0U;
  NRF_RADIO->EVENTS_PAYLOAD = 0U;
  NRF_RADIO->FREQUENCY = (uint8_t)ble_hw_frequency_channels[channel];
  NRF_RADIO->DATAWHITEIV = channel;     /* This value needs to correspond to the channel being used */
  NRF_RADIO->PACKETPTR = (uint32_t)buf;
  DEBUG_GPIO_OUTPUT_RXEN();

  NRF_RADIO->EVENTS_READY = 0;		/* Clear ready register */
  NRF_RADIO->TASKS_RXEN = 1;		/* Enable the radio in TX mode */
  //while(NRF_RADIO->EVENTS_READY == 0U);	/* Wait for the radio to ramp up */
  //NRF_RADIO->TASKS_START = 1;		/* Start rx */
  #if TRIGGER_RADIO_START_WITH_TIMER && !RADIO_TEST_TX_CARRIER
    NRF_TIMER1->TASKS_START = 1;
  #endif
  //printf("Listen --- %u ---\n", channel);
  /*BUSYWAIT_UNTIL(NRF_RADIO->EVENTS_ADDRESS != 0U, SLOT_LEN - RX_SLOT_LEN - 10);
  if(NRF_RADIO->EVENTS_ADDRESS){
    BUSYWAIT_UNTIL(NRF_RADIO->EVENTS_END != 0U, RX_SLOT_LEN);
  }
  return NRF_RADIO->EVENTS_END && NRF_RADIO->CRCSTATUS == RADIO_CRCSTATUS_CRCSTATUS_CRCOk;*/
  return 0;
}
/*---------------------------------------------------------------------------*/
static void 
output_radio_events_gpio_init(void) 
{  
  #if NRF_RADIO_DEBUG_STATE
  #ifdef NRF51
  NRF_GPIOTE->POWER = 0UL;
  NRF_GPIOTE->POWER = 1UL;
  #endif
  //Disable GPIOTE interrupts
  NRF_GPIOTE->INTENCLR = 0xffffffffUL;

  //configure GPIOTE
  NRF_GPIOTE->CONFIG[RADIO_ADDRESS_EVENT_GPIOTE_CH] = (GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos) | 
                                        (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos) | 
                                        (RADIO_ADDRESS_EVENT_PIN << GPIOTE_CONFIG_PSEL_Pos) | 
                                        (GPIOTE_CONFIG_OUTINIT_Low << GPIOTE_CONFIG_OUTINIT_Pos);

  NRF_GPIOTE->CONFIG[RADIO_READY_EVENT_GPIOTE_CH] = (GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos) | 
                                        (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos) | 
                                        (RADIO_READY_EVENT_PIN << GPIOTE_CONFIG_PSEL_Pos) | 
                                        (GPIOTE_CONFIG_OUTINIT_Low << GPIOTE_CONFIG_OUTINIT_Pos);
  
  NRF_GPIOTE->CONFIG[RADIO_TXEN_GPIOTE_CH] = (GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos) | 
                                        (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos) | 
                                        (RADIO_TXEN_PIN << GPIOTE_CONFIG_PSEL_Pos) | 
                                        (GPIOTE_CONFIG_OUTINIT_Low << GPIOTE_CONFIG_OUTINIT_Pos);

  NRF_GPIOTE->CONFIG[RADIO_RXEN_GPIOTE_CH] = (GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos) | 
                                        (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos) | 
                                        (RADIO_RXEN_PIN << GPIOTE_CONFIG_PSEL_Pos) | 
                                        (GPIOTE_CONFIG_OUTINIT_Low << GPIOTE_CONFIG_OUTINIT_Pos);

  NRF_GPIOTE->CONFIG[RADIO_PAYLOAD_GPIOTE_CH] = (GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos) | 
                                        (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos) | 
                                        (RADIO_PAYLOAD_PIN << GPIOTE_CONFIG_PSEL_Pos) | 
                                        (GPIOTE_CONFIG_OUTINIT_Low << GPIOTE_CONFIG_OUTINIT_Pos);

  //Link radio events to GPIOTE
  NRF_PPI->CH[RADIO_ADDRESS_EVENT_PPI_CH].EEP = (uint32_t)&NRF_RADIO->EVENTS_ADDRESS;
  NRF_PPI->CH[RADIO_ADDRESS_EVENT_PPI_CH].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[RADIO_ADDRESS_EVENT_GPIOTE_CH];
  NRF_PPI->CH[RADIO_END_EVENT_PPI_CH].EEP = (uint32_t)&NRF_RADIO->EVENTS_END;
  NRF_PPI->CH[RADIO_END_EVENT_PPI_CH].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[RADIO_ADDRESS_EVENT_GPIOTE_CH];
  
  NRF_PPI->CH[RADIO_READY_EVENT_PPI_CH].EEP = (uint32_t)&NRF_RADIO->EVENTS_READY;
  NRF_PPI->CH[RADIO_READY_EVENT_PPI_CH].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[RADIO_READY_EVENT_GPIOTE_CH];
  NRF_PPI->CH[RADIO_DISABLED_EVENT_PPI_CH].EEP = (uint32_t)&NRF_RADIO->EVENTS_DISABLED;
  NRF_PPI->CH[RADIO_DISABLED_EVENT_PPI_CH].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[RADIO_READY_EVENT_GPIOTE_CH];

  NRF_PPI->CH[RADIO_PAYLOAD_EVENT_PPI_CH].EEP = (uint32_t)&NRF_RADIO->EVENTS_PAYLOAD;
  NRF_PPI->CH[RADIO_PAYLOAD_EVENT_PPI_CH].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[RADIO_PAYLOAD_GPIOTE_CH];

  NRF_PPI->CH[RADIO_T0_TX_EVENT_PPI_CH].EEP = (uint32_t)&NRF_TIMER0->EVENTS_COMPARE[0];
  NRF_PPI->CH[RADIO_T0_TX_EVENT_PPI_CH].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[RADIO_TXEN_GPIOTE_CH];
  NRF_PPI->CH[RADIO_T0_RX_EVENT_PPI_CH].EEP = (uint32_t)&NRF_TIMER0->EVENTS_COMPARE[0];
  NRF_PPI->CH[RADIO_T0_RX_EVENT_PPI_CH].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[RADIO_RXEN_GPIOTE_CH];

  //Enable PPI channels
  NRF_PPI->CHEN |= (1 << RADIO_READY_EVENT_PPI_CH) | (1 << RADIO_DISABLED_EVENT_PPI_CH) | (1 << RADIO_PAYLOAD_EVENT_PPI_CH) | (1 << RADIO_ADDRESS_EVENT_PPI_CH) | (1 << RADIO_END_EVENT_PPI_CH);
  #endif
}
/*---------------------------------------------------------------------------*/
static void timer1_init(void)
{
  /* Check if the HF clock is running*/
  if ((NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKSTAT_STATE_Msk) == 0) {
    hfclk_xtal_init ();
  }

  /* Clear the task to make sure the timer is stopped */
  //NRF_TIMER1->TASKS_STOP  = 1;
  //NRF_TIMER1->TASKS_CLEAR = 1;
  NRF_TIMER1->TASKS_SHUTDOWN  = 1;
  /* Set the timer in Timer Mode */
  NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;
  /* Prescaler 0 produces 16MHz timer tick frequency */
  NRF_TIMER1->PRESCALER = 0;
  /* 32 bit mode */
  NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
  /* Enable the Compare event on channel 0 */
  NRF_TIMER1->EVENTS_COMPARE[0] = 0;
  /* count 140 us waiting for radio rampup time */
  NRF_TIMER1->CC[0]       = 140*16UL; 

  /* Enable overflow event and overflow interrupt */
  NRF_TIMER1->INTENCLR = 0xffffffffUL; 

  /* Enable shorts for auto-reset
   * */
  NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_STOP_Msk | TIMER_SHORTS_COMPARE0_CLEAR_Msk;;

  /* Start the timer */

  //NRF_TIMER1->TASKS_START = 1UL;

  /* configure PPI */
  /* TIMER0 COMPARE[0] triggers timer1 start (together with tx/rx enable) --> to count 140 us */
  NRF_PPI->CH[TIMER0C0_TIMER1_START_PPI_CH].EEP = (uint32_t)&NRF_TIMER0->EVENTS_COMPARE[0];
  NRF_PPI->CH[TIMER0C0_TIMER1_START_PPI_CH].TEP = (uint32_t)&NRF_TIMER1->TASKS_START;
  /* TIMER1 COMPARE[0] triggers timer1 start (together with tx/rx enable) --> to count 140 us */
  NRF_PPI->CH[TIMER1_RADIO_START_PPI_CH].EEP = (uint32_t)&NRF_TIMER1->EVENTS_COMPARE[0];
  NRF_PPI->CH[TIMER1_RADIO_START_PPI_CH].TEP = (uint32_t)&NRF_RADIO->TASKS_START;
  #if TRIGGER_RADIO_START_WITH_TIMER
  NRF_PPI->CHEN |= (1 << TIMER0C0_TIMER1_START_PPI_CH) | (1 << TIMER1_RADIO_START_PPI_CH);
  #endif
}
