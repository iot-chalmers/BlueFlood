/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         CC2420 driver header file
 * \author
 *         Adam Dunkels <adam@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 *         Konrad Krentz <konrad.krentz@gmail.com>
 */

#ifndef CC2420_H_
#define CC2420_H_

#include "contiki.h"
#include "dev/spi.h"
#include "dev/radio.h"
#include "cc2420_const.h"
#include "lib/aes-128.h"
#include "net/llsec/llsec802154.h"
#include "net/mac/chaos/flocklab-gpio.h"
#include "net/mac/chaos/chaos.h"
#include "net/mac/chaos/chaos-header.h"
#include "dev/cooja-debug.h"

#if CONTIKI_TARGET_SKY || CONTIKI_TARGET_Z1
#define SFD_CAPTURE_REG TBCCR1
#else
#define SFD_CAPTURE_REG TACCR2
#endif

int cc2420_init(void);

#define CC2420_MAX_PACKET_LEN      127

#define ACK_LEN 3

int cc2420_set_channel(int channel);
int cc2420_get_channel(void);

void cc2420_set_pan_addr(unsigned pan,
                                unsigned addr,
                                const uint8_t *ieee_addr);

extern signed char cc2420_last_rssi;
extern uint8_t cc2420_last_correlation;

int cc2420_rssi(void);

extern const struct radio_driver cc2420_driver;

/**
 * \param power Between 1 and 31.
 */
void cc2420_set_txpower(uint8_t power);
int cc2420_get_txpower(void);
#define CC2420_TXPOWER_MAX  31
#define CC2420_TXPOWER_MIN   0

/**
 * Interrupt function, called from the simple-cc2420-arch driver.
 *
 */
int cc2420_interrupt(void);

/* XXX hack: these will be made as Chameleon packet attributes */
extern rtimer_clock_t cc2420_time_of_arrival,
  cc2420_time_of_departure;
extern int cc2420_authority_level_of_sender;

int cc2420_on(void);
int cc2420_off(void);

void cc2420_set_cca_threshold(int value);

extern const struct aes_128_driver cc2420_aes_128_driver;

/* Configures timer B to capture SFD edge (start, end, both),
 * and sets the link start time for calculating synchronization in ACK */
void cc2420_sfd_sync(uint8_t capture_start_sfd, uint8_t capture_end_sfd);

/* Read the timer value when the last SFD edge was captured,
 * this depends on SFD timer configuration */
uint16_t cc2420_read_sfd_timer(void);

/* Turn on/off address decoding.
 * Disabling address decoding would enable reception of
 * frames not compliant with the 802.15.4-2003 standard */
void cc2420_address_decode(uint8_t enable);

/* Enable or disable radio interrupts (both FIFOP and SFD timer capture) */
void cc2420_set_interrupt_enable(uint8_t e);

/* Get radio interrupt enable status */
uint8_t cc2420_get_interrupt_enable(void);

/* put in tx mode and then copy packet on the fly */
static ALWAYS_INLINE int cc2420_fast_send(const register uint8_t *buffer, uint16_t *sfd);

int cc2420_rx_byte_available(void);

void cc2420_get_rx_data(uint8_t *buffer, int count);

void cc2420_flushrx(void);

void cc2420_set_fifop(uint8_t p);

void cc2420_set_autoack(uint8_t enable);
void cc2420_set_security(uint8_t security_level, uint8_t clear_text_bytes);
void cc2420_set_key(const uint8_t *key);
void cc2420_set_nonce(uint8_t *nonce, uint8_t rx);
void cc2420_decrypt_rx(void);

ALWAYS_INLINE void cc2420_set_channel_fast(int c);
/************************************************************************/
/* Generic names for special functions */
/************************************************************************/
#define NETSTACK_RADIO_address_decode(E)        cc2420_address_decode((E))
#define NETSTACK_RADIO_set_interrupt_enable(E)  cc2420_set_interrupt_enable((E))
#define NETSTACK_RADIO_sfd_sync(S,E)            cc2420_sfd_sync((S),(E))
#define NETSTACK_RADIO_read_sfd_timer()         cc2420_read_sfd_timer()
#define NETSTACK_RADIO_set_channel(C)           cc2420_set_channel_fast((C))
#define NETSTACK_RADIO_get_channel()            cc2420_get_channel()
#define NETSTACK_RADIO_radio_raw_rx_on()        cc2420_on();
#define NETSTACK_RADIO_set_txpower(X)           cc2420_set_txpower(X)
#define NETSTACK_RADIO_set_cca_threshold(X)     cc2420_set_cca_threshold(X)
#define NETSTACK_RADIO_fast_send(X,S)     			cc2420_fast_send(X,S)
#define NETSTACK_RADIO_fast_rx(sfd_vht, round_synced, app_id, association, rx_packet, slot_length) \
        cc2420_fast_rx(sfd_vht, round_synced, app_id, association, rx_packet, slot_length)
#define NETSTACK_RADIO_rx_byte_available()     	cc2420_rx_byte_available()
#define NETSTACK_RADIO_get_rx_byte(B)     			CC2420_GET_RX_BYTE((B))
#define NETSTACK_RADIO_flushrx()     						cc2420_flushrx()
#define NETSTACK_RADIO_set_fifop(p)							cc2420_set_fifop((p))
#define NETSTACK_RADIO_set_autoack(p)						cc2420_set_autoack((p))
#define NETSTACK_RADIO_set_pan_addr(PAN, ADDR, LONGADDR)  cc2420_set_pan_addr(PAN, ADDR, LONGADDR)

/* Read status of the CC2420 */
#define CC2420_GET_STATUS(s)                    \
  do {                                          \
    CC2420_SPI_ENABLE();                        \
    SPI_WRITE(CC2420_SNOP);                     \
    s = SPI_RXBUF;                              \
    CC2420_SPI_DISABLE();                       \
  } while (0)

/*---------------------------------------------------------------------------*/
#define FASTSPI_RX_ADDR(a)		\
		do {											\
			SPI_TXBUF = (a);				\
			SPI_WAITFOREOTx();			\
		} while (0)

#define CC2420_GET_RX_BYTE(b)                   \
  do {                                          \
    CC2420_SPI_ENABLE();                        \
  	FASTSPI_RX_ADDR(CC2420_RXFIFO | 0x40);			\
    (void) SPI_RXBUF;                           \
    SPI_READ((b));															\
	  clock_delay(1);															\
    CC2420_SPI_DISABLE();                       \
  } while (0)

#define CC2420_GET_RX_NO_WAIT(p,c)              \
  do {                                          \
    uint8_t spiCnt;                             \
    CC2420_SPI_ENABLE();                        \
    FASTSPI_RX_ADDR(CC2420_RXFIFO | 0x40);      \
    (void) SPI_RXBUF;                           \
    for (spiCnt = 0; spiCnt < (c); spiCnt++) {  \
      SPI_READ(((uint8_t*)(p))[spiCnt]);        \
    }                                           \
    clock_delay(1);                             \
    CC2420_SPI_DISABLE();                       \
  } while (0)

static ALWAYS_INLINE unsigned int
CC2420_GET_STATUS_FAST()
{
  uint16_t s;
  CC2420_SPI_ENABLE();
  SPI_WRITE(CC2420_SNOP);
  s = SPI_RXBUF;
  CC2420_SPI_DISABLE();
  return s;
}

static ALWAYS_INLINE unsigned int
CC2420_GET_STATUS_FAST_X2()
{
  uint16_t s;
  SPI_WRITE(CC2420_SNOP);
  s = SPI_RXBUF;
  return s;
}

static ALWAYS_INLINE void
cc2420_fast_on(void)
{
  CC2420_SPI_ENABLE();
  SPI_WRITE(CC2420_SRXON);
  CC2420_SPI_DISABLE();
  ENERGEST_ON(ENERGEST_TYPE_LISTEN);
}

#define BUSYWAIT_TIMEOUT_ACTION(cond, timeout, action)        \
  while(!(cond)) {                        \
    if (!RTIMER_CLOCK_LT(RTIMER_NOW(), timeout)) {        \
      { action }                        \
    }                             \
  }
/*---------------------------------------------------------------------------*/
static ALWAYS_INLINE void
cc2420_fast_off(void)
{
  /* Wait for transmission to end before turning radio off. */
//  {
//    uint16_t s;
//    rtimer_clock_t t0;
//    t0 = RTIMER_NOW();
//    while((CC2420_GET_STATUS_FAST() & BV(CC2420_TX_ACTIVE))
//        && RTIMER_CLOCK_LT(RTIMER_NOW(), t0 + (RTIMER_SECOND / 10)));
//  }

  ENERGEST_OFF(ENERGEST_TYPE_LISTEN);
  CC2420_SPI_ENABLE();
  SPI_WRITE(CC2420_SRFOFF);
  CC2420_SPI_DISABLE();
  if(!CC2420_FIFOP_IS_1) {
    cc2420_flushrx();
  }
}

/* put in tx mode and then copy packet on the fly */
static ALWAYS_INLINE int
cc2420_fast_send(const register uint8_t * const buffer, uint16_t *sfd)
{
  /* payload includes length byte in the beginning
   * we copy len - 1 (=len(packet)-2(CRC)+1(length field)) because
   * the CRC is auto-generated,
   * and len does not count for the length byte in the beginning.
   *
   * MIC bytes are generated on the fly. Don't copy them.
   */
  register int8_t i = 0, count = buffer[0] - 1 - LLSEC802154_MIC_LENGTH, sfd_captured = 0;
  int tx_active = 0;
  SET_PIN_ADC0;
  CC2420_SPI_ENABLE(); /* enable spi */
  SPI_WRITE_FAST(CC2420_SFLUSHTX); /* flush tx fifo */
  SPI_WRITE_FAST(CC2420_STXON); /* put in tx mode */
#if ENERGEST_CONF_ON
  //ENERGEST_OFF(ENERGEST_TYPE_LISTEN);
  //ENERGEST_ON(ENERGEST_TYPE_TRANSMIT);
#endif /* ENERGEST_CONF_ON */
  /* copy payload to tx fifo on the fly */
  SPI_WRITE_FAST(CC2420_TXFIFO);
  for(i = 0; i < count; i++) {
    SPI_WRITE_FAST(buffer[i]);
    if(CC2420_SFD_IS_1 && !sfd_captured){
      *sfd = DCO_NOW();
      sfd_captured = 1;
    }
  }
  SPI_WAITFORTx_ENDED(); /* required after last SPI_WRITE_FAST */
  CC2420_SPI_DISABLE(); /* disable spi */
  if( !sfd_captured ){
    BUSYWAIT_UNTIL(CC2420_SFD_IS_1, CHAOS_PACKET_DURATION(127));
    *sfd = DCO_NOW();
  }
  /* are we transmitting or something wrong happened? */
  tx_active = CC2420_GET_STATUS_FAST() & BV(CC2420_TX_ACTIVE);

#if !CONTIKI_TARGET_WSN430
  clock_delay(40);
  *sfd = NETSTACK_RADIO_read_sfd_timer();
#endif
  /*wait_for_transmission(void)*/
  {
    rtimer_clock_t t0;
    t0 = RTIMER_NOW();
    while((CC2420_GET_STATUS_FAST() & BV(CC2420_TX_ACTIVE))
        && RTIMER_CLOCK_LT(RTIMER_NOW(), t0 + (RTIMER_SECOND / 10)));
  }
  UNSET_PIN_ADC0;
  return tx_active;
}

static cc2420_rx_chaos_packet(rtimer_clock_t begin, int rx, vht_clock_t* sfd_vht, int round_synced, uint8_t app_id, uint8_t association, uint8_t * const rx_packet, rtimer_clock_t slot_length){
  int rx_state = rx ? CHAOS_TXRX_OK : CHAOS_RX_NO_SFD;
  if( rx_state == CHAOS_TXRX_OK ){
    chaos_header_t* const rx_header = (chaos_header_t*)((uint32_t *)rx_packet);
    rtimer_clock_t timeout;
    timeout = begin + CHAOS_PACKET_DURATION(2);
    int bytes_cnt = 0;

    BUSYWAIT_TIMEOUT_ACTION(CC2420_FIFO_IS_1, timeout, {NETSTACK_RADIO_flushrx(); COOJA_DEBUG_STR("timeout byte 1"); return CHAOS_RX_TIMEOUT;})
    #if COOJA
    //OL: Cooja only, do not ask why
    clock_delay(40);
    #endif

    // read the first byte (i.e., the len field) from the RXFIFO
    NETSTACK_RADIO_get_rx_byte(rx_packet[bytes_cnt++]);
    uint8_t max_packet_size = sizeof(chaos_header_t) + FOOTER_LEN + LLSEC802154_MIC_LENGTH + CHAOS_MAX_PAYLOAD_LEN;
    if((rx_packet[0] < sizeof(chaos_header_t) + FOOTER_LEN + LLSEC802154_MIC_LENGTH) || rx_packet[0] > max_packet_size ) {
      //COOJA_DEBUG_STR("length error");
      COOJA_DEBUG_STRX("length err ", max_packet_size, 3);
      NETSTACK_RADIO_flushrx();
      return CHAOS_TXRX_ERROR;
    }

    if(association){
      timeout = begin + CHAOS_PACKET_DURATION(rx_packet[0]);
    } else {
    //update the timeout, as we now know the correct length
      timeout = begin + slot_length;
    }

    //get the FCF, slotnumber and addresses
    while (bytes_cnt < PRE_HW_SECURITY_HEADER_LEN) {
      BUSYWAIT_TIMEOUT_ACTION(CC2420_FIFO_IS_1, timeout, {NETSTACK_RADIO_flushrx(); COOJA_DEBUG_STR("timeout body");return CHAOS_RX_TIMEOUT;})
      NETSTACK_RADIO_get_rx_byte(rx_packet[bytes_cnt++]);
    }

    // check header
    if ((rx_header->chaos_fcf_0 != CHAOS_FCF_0
          && rx_header->chaos_fcf_0 != CHAOS_FCF_0_NO_SECURITY)
        || rx_header->chaos_fcf_1 != CHAOS_FCF_1) {
      // packet with a wrong header: abort packet reception
      NETSTACK_RADIO_flushrx();
      COOJA_DEBUG_STR("wrong header");
      return CHAOS_RX_HEADER_ERROR;
    }

    // keep receiving only if it has the right header
    if (rx_header->dst_pan_id != CHAOS_PANID
#if CHAOS_USE_DST_ID
        || (rx_header->dst_node_id != FRAME802154_BROADCASTADDR
            && rx_header->dst_node_id != node_id)
#endif
            ) {
      // packet with a wrong header: abort packet reception
      NETSTACK_RADIO_flushrx();
      COOJA_DEBUG_STR("wrong pan id or node id");
      return CHAOS_RX_HEADER_ERROR;
    }

#if CHAOS_HW_SECURITY
  /* update security frame counter based on local knowledge (round number)
   * and received slot number in order to update the rx nonce for decryption */
//    chaos_security_frame_counter_t frame_counter;
//    uint8_t rx_nonce[16]={0};
//    uint8_t mic_enabled = rx_header->security_control & 3;
//    uint8_t enc_enabled = rx_header->security_control & (1 << 2);
    uint8_t security_enabled_frame = CHAOS_IS_SECURITY_ENABLED_FRAME(rx_header);

    if(security_enabled_frame) {
      COOJA_DEBUG_STR("security_enabled_frame");

//      // update nonce based on slotnumber
//      frame_counter.slot_number = rx_header->slot_number;
//      frame_counter.seq_number = rx_header->slot_number;
//      frame_counter.round_number = rx_header->round_number;
//
//      //frame_counter.security_frame_counter = rx_header->security_frame_counter; //encrypted
//      chaos_make_nonce(rx_nonce,
//          chaos_get_extended_address(rx_header->src_node_id),
//          frame_counter.security_frame_counter);
//      chaos_make_const_nonce(rx_nonce);
//      cc2420_set_nonce(rx_nonce, 1); /* upload rx nonce */

      // Decrypt and/or authenticate frame
      cc2420_decrypt_rx();
      BUSYWAIT_UNTIL((CC2420_FIFO_IS_1), CHAOS_PACKET_DURATION(15)); //should not take more than 400us
    }
#endif /* CHAOS_HW_SECURITY */

    //get the rest of the packet after decryption is done
    while (bytes_cnt < CHAOS_PACKET_RADIO_LENGTH(rx_header->length) && bytes_cnt < RADIO_MAX_PACKET_LEN) {
      BUSYWAIT_TIMEOUT_ACTION(CC2420_FIFO_IS_1, timeout, {NETSTACK_RADIO_flushrx(); COOJA_DEBUG_STR("timeout body");return CHAOS_RX_TIMEOUT+bytes_cnt;})
      NETSTACK_RADIO_get_rx_byte(rx_packet[bytes_cnt++]);
    }

    //check crc
    //I think we don't need to check crc when authentication is enabled (MIC)
    if((CHAOS_CRC_FIELD(rx_packet) & FOOTER1_CRC_OK) == 0) {
      // packet with a wrong crc: report
      NETSTACK_RADIO_flushrx();
      COOJA_DEBUG_STR("wrong crc");
      return CHAOS_RX_CRC_ERROR;
    }

#if CHAOS_HW_SECURITY
    // check MIC
#define MIC_VALID 0
#define MIC_INVALID 0xff
    extern volatile uint8_t rx_pkt_crc_err[129];
    if(/*security_enabled_frame && mic_enabled && */(CHAOS_MIC_FIELD(rx_packet) != MIC_VALID)) {
      rx_pkt_crc_err[128] = 1; //to announce a packet
//      memcpy(rx_pkt_crc_err, rx_packet, max_packet_size);
      // packet with a wrong mic: report
      NETSTACK_RADIO_flushrx();
      COOJA_DEBUG_STR("wrong mic");
      return CHAOS_RX_MIC_ERROR;
    }
#endif /* CHAOS_HW_SECURITY */

  } else {
    COOJA_DEBUG_STR("no sfd");
  }
  NETSTACK_RADIO_flushrx();
  //COOJA_DEBUG_PRINTF("sfd %u - %u = %u", *sfd_dco, t_cap_h, *sfd_dco - t_cap_h);
  UNSET_PIN_ADC1;

//  if(!rx_pkt_crc_err[128]){
//    //make a copy anyway
//    extern volatile uint8_t rx_pkt_copy[129];
//    memcpy(rx_pkt_copy, rx_packet, rx_packet[0]+1);
//  }
  return rx_state;
}

static ALWAYS_INLINE int
cc2420_fast_rx(vht_clock_t* sfd_vht, int round_synced, uint8_t app_id, uint8_t association, uint8_t * const rx_packet, rtimer_clock_t slot_length)
{

  COOJA_DEBUG_STR("0");
  volatile uint8_t rx = 0;
  volatile rtimer_clock_t t_cap_l = 0;
  rtimer_clock_t t_cap_h = 0;
//  CAPTURE_NEXT_CLOCK_TICK(t_cap_h, t_cap_l);
  t_cap_l = RTIMER_NOW();
  do{
    while( RTIMER_NOW_FAST() == t_cap_l );
    t_cap_h = VHT_DCO_NOW(); //captured dco tick on last rtimer tick
  } while( RTIMER_NOW() == t_cap_l );
  t_cap_l = RTIMER_NOW();

  COOJA_DEBUG_STR("2");
  if( round_synced ){
    BUSYWAIT_UNTIL((rx = CC2420_SFD_IS_1), (DCO_TO_RTIMER(SFD_DETECTION_TIME_MIN) + (RX_GUARD_TIME)));
  } else if( !round_synced && !association ){
    BUSYWAIT_UNTIL((rx = CC2420_SFD_IS_1), DCO_TO_RTIMER(SFD_DETECTION_TIME_MIN) + (ROUND_GUARD_TIME));
  } else if( association ){
    BUSYWAIT_UNTIL((rx = CC2420_SFD_IS_1), (slot_length));
  }
#if 0 //CONTIKI_TARGET_WSN430
  rtimer_clock_t sfd_dco = DCO_NOW();
#else
  rtimer_clock_t sfd_dco = NETSTACK_RADIO_read_sfd_timer() + RX_SFD_OFFSET;
#endif

  COOJA_DEBUG_STR("3");
  SET_PIN_ADC1;

  // round up for the smallest fraction of CLOCK_PHI
//#define CLOCK_ROUND_UP (CLOCK_PHI/2) //to round up on 0.5
//#define CLOCK_ROUND_UP (CLOCK_PHI - 1) //to get ceiling
  vht_clock_t begin_vht = RTIMER_DCO_TO_VHT(t_cap_l, (sfd_dco - t_cap_h));
  rtimer_clock_t begin = VHT_TO_RTIMER(begin_vht);
  *sfd_vht = begin_vht;

  return cc2420_rx_chaos_packet(begin, rx, sfd_vht, round_synced, app_id, association, rx_packet, slot_length);

}

#endif /* CC2420_H_ */
