/**
 * \addtogroup nrf51822
 * @{
 * \defgroup nrf51-radio-driver Contiki radio driver implementation for nrf51822 SoC
 * 			This module contains the radio driver implementation for nrf51822 SoC
 *
 * @{
 * \file
 * nrf51822 architecture specific declarations for the nRF51 radio
 * \author CoenRoest
 */
#ifndef NRF_RADIO_H_
#define NRF_RADIO_H_

#include "contiki.h"
#include "dev/radio.h"
#include "string.h"

/*---------------------------------------------------------------------------*/

#ifndef RADIO_SHORTS_ENABLED
#define RADIO_SHORTS_ENABLED false
#endif

#ifndef RADIO_BCC_ENABLED
#define RADIO_BCC_ENABLED false
#endif

#ifndef RADIO_RSSI_ENABLED
#define RADIO_RSSI_ENABLED false
#endif

/** Defines for the capture/compare registers in TIMER0*/
#define SCHEDULE_REG 		2
#define TIMESTAMP_ADDR_REG 	1
#define TIMESTAMP_END_REG	1
#define NOW_REG			3


/** Defines for reading out the SFD timestamp register */
#define ADDRESS_EVENT 	0
#define END_EVENT 	1

#define RADIO_MODE RADIO_MODE_MODE_Nrf_1Mbit

/*---------------------------------------------------------------------------*/
#if RADIO_MODE == RADIO_MODE_MODE_Ble_1Mbit
#define MAX_CHANNELS 40
#else
#define MAX_CHANNELS 84
#endif

/*---------------------------------------------------------------------------*/
int nrf_radio_init(void);

int nrf_radio_prepare(const void *payload, unsigned short payload_len);
int nrf_radio_transmit(unsigned short transmit_len);
int nrf_radio_send(const void *payload, unsigned short payload_len);

int nrf_radio_read(void *buf, unsigned short buf_len);

inline __attribute__((always_inline)) static int nrf_radio_fast_send(const void *payload, unsigned short payload_len);

int nrf_radio_receiving_packet(void);
int nrf_radio_pending_packet(void);

int nrf_radio_on(void);
int nrf_radio_off(void);

void nrf_radio_flushrx(void);

int nrf_radio_set_radio_mode(int m);
int nrf_radio_set_channel(int c);
int nrf_radio_get_channel(void);
int nrf_radio_get_rssi(void);

rtimer_clock_t nrf_radio_read_timestamp(int event);
int nrf_radio_sfd_sync(uint8_t capture_start_sfd, uint8_t capture_end_sfd);

void nrf_radio_set_interrupt_enable(uint8_t e);

void RADIO_IRQHandler(void);

extern const struct radio_driver nrf_radio_driver;

int nrf_radio_set_txpower(int power);
int nrf_radio_get_txpower(void);
#define NRF_RADIO_TXPOWER_MAX  4
#define NRF_RADIO_TXPOWER_MIN  -30
#define NRF_RADIO_DEFAULT_CHANNEL 26

/** Set the radio mode for the NRF Radio */
#define NRF_RADIO_MODE RADIO_MODE_MODE_Nrf_1Mbit
//#define RADIO_MODE RADIO_MODE_MODE_Nrf_250Kbit

/*---------------------------------------------------------------------------*/
#define PACKET0_S1_SIZE                  (0UL)  //!< S1 size in bits
#define PACKET0_S0_SIZE                  (0UL)  //!< S0 size in bits
#define PACKET0_PAYLOAD_SIZE             (8UL) 	//!< payload size (length) in bits
#define PACKET1_BASE_ADDRESS_LENGTH      (4UL)  //!< base address length in bytes
#define PACKET1_STATIC_LENGTH            (0UL) 	//!< static length in bytes
#define PACKET1_PAYLOAD_SIZE             (250UL)//!< maximum payload size in bytes

#define HEADER_LEN (uint8_t)ceil(PACKET0_PAYLOAD_SIZE / 8)
uint8_t tx_buffer[PACKET1_PAYLOAD_SIZE];  	///< tx buffer for packets
uint8_t rx_buffer[PACKET1_PAYLOAD_SIZE];  	///< rx buffer for packets
static uint8_t tx_ready;

/*---------------------------------------------------------------------------*/
static uint8_t locked, lock_on, lock_off;
static uint8_t receive_on = 0;

#define GET_LOCK() locked++
static void RELEASE_LOCK(void) {
  if(locked == 1) {
    if(lock_on) {
      lock_on = 0;
    }
    if(lock_off) {
      lock_off = 0;
    }
  }
  locked--;
}
/*---------------------------------------------------------------------------*/

/* Macro to switch off the radio fast */
#define NRF_RADIO_FAST_OFF()				\
	  receive_on = 0;				\
	  tx_ready = 0;				\
	  NRF_RADIO->EVENTS_DISABLED = 0UL;		\
	  NRF_RADIO->TASKS_DISABLE = 1UL;		\
	  while (NRF_RADIO->EVENTS_DISABLED == 0UL);

/* Event status */
#define NRF_RADIO_SFD_IS_1   (NRF_RADIO->EVENTS_ADDRESS == 1UL)
#define NRF_RADIO_END_IS_1   (NRF_RADIO->EVENTS_END == 1UL)

#define NRF_RADIO_RESET_EVENTS()		 \
	    NRF_RADIO->EVENTS_END = 0UL;	 \
	    NRF_RADIO->EVENTS_ADDRESS = 0UL;

/*---------------------------------------------------------------------------*/
inline __attribute__((always_inline)) static int
nrf_radio_fast_send(const void *payload, unsigned short payload_len)
{

#if (RADIO_SHORTS_ENABLED)

  /* Enable the radio in TX mode, it takes 140us before sending */
  NRF_RADIO->TASKS_TXEN = 1;

#else

#warning "Potentially dangerous! No check if the radio is ramped up yet"
  /* Start the transmission */
  NRF_RADIO->TASKS_START = 1UL;

#endif

  /* TODO CR: why doesn't this work anymore? Keeps looping infinitely on this
   * 	 	after some x rounds of tx-test
   */
  //while(NRF_RADIO->EVENTS_ADDRESS == 0U);	/* Wait for the ADDRESS event to occur */
  return 1;
}



/*---------------------------------------------------------------------------*/

// TODO CR use this for input check at set_txpower() ?
#define TXPOWER_ENUM(S) \
  ( (S==0) ? RADIO_TXPOWER_TXPOWER_Pos4dBm : \
    (S==1) ? RADIO_TXPOWER_TXPOWER_0dBm : \
    (S==2) ? RADIO_TXPOWER_TXPOWER_Neg4dBm : \
    (S==3) ? RADIO_TXPOWER_TXPOWER_Neg8dBm : \
    (S==4) ? RADIO_TXPOWER_TXPOWER_Neg12dBm : \
    (S==5) ? RADIO_TXPOWER_TXPOWER_Neg16dBm : \
    (S==6) ? RADIO_TXPOWER_TXPOWER_Neg20dBm : \
    (S==7) ? RADIO_TXPOWER_TXPOWER_Neg30dBm : RADIO_TXPOWER_TXPOWER_Pos4dBm)

/*---------------------------------------------------------------------------*/

/************************************************************************/
/* Generic names for special functions */
/************************************************************************/

#define NETSTACK_RADIO_sfd_sync(S,E)            nrf_radio_sfd_sync((S),(E))
#define NETSTACK_RADIO_read_sfd_timer(E)        nrf_radio_read_timestamp(E)
#define NETSTACK_RADIO_set_channel(C)           nrf_radio_set_channel((C))
#define NETSTACK_RADIO_get_channel()            nrf_radio_get_channel()
#define NETSTACK_RADIO_radio_raw_rx_on()        nrf_radio_on();
#define NETSTACK_RADIO_set_txpower(X)           nrf_radio_set_txpower(X)
#define NETSTACK_RADIO_get_txpower()            nrf_radio_get_txpower()
#define NETSTACK_RADIO_set_radio_mode(R)         nrf_radio_set_radio_mode(R)
#define NETSTACK_RADIO_fast_send(S, E)     	nrf_radio_fast_send(S, E)
#define NETSTACK_RADIO_flushrx()     		nrf_radio_flushrx()
#define NETSTACK_RADIO_FAST_OFF()     		NRF_RADIO_FAST_OFF()
#define NETSTACK_RADIO_set_interrupt_enable(E)  nrf_radio_set_interrupt_enable((E))

//#define NETSTACK_RADIO_address_decode(E)      cc2420_address_decode((E))
//#define NETSTACK_RADIO_set_cca_threshold(X)   cc2420_set_cca_threshold(X)
//#define NETSTACK_RADIO_set_autoack(p)		cc2420_set_autoack((p))


#endif /* NRF_RADIO_H_ */
