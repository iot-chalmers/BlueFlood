/**
 * \addtogroup nrf51-radio-driver
 * @{
 * \file
 * nrf51822 radio driver
 *  \author CoenRoest
 */

#include <string.h>
#include <inttypes.h>
#include <math.h>

#include "contiki.h"
#include "nrf-radio.h"
// Debug purposes
#include "nrf-gpio.h"
#include "nrf-test.h"

#include "net/packetbuf.h"
#include "net/netstack.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...) do {} while (0)
#endif

/*---------------------------------------------------------------------------*/
PROCESS(nrf_radio_process, "nRF Radio driver");
/*---------------------------------------------------------------------------*/

/* Local functions prototypes */
static void on(void);
static void off(void);

const struct radio_driver nrf_radio_driver =
{
    nrf_radio_init,
    nrf_radio_prepare,
    nrf_radio_transmit,
    nrf_radio_send,
    nrf_radio_read,
    nrf_radio_fast_send,
    /* detected_energy, */
    //nrf_radio_cca,
    nrf_radio_receiving_packet,
    nrf_radio_pending_packet,
    nrf_radio_on,
    nrf_radio_off,
};

/*---------------------------------------------------------------------------*/
static int channel;
//static int max_channels;
static uint8_t volatile interrupt_enabled = 0;
rtimer_clock_t time, ref_time = 0;

/*---------------------------------------------------------------------------*/
int
nrf_radio_init(void)
{
    /* Reset all states in the radio peripheral */
    NRF_RADIO->POWER = 0;
    NRF_RADIO->POWER = 1;

    //disable interrupt
    nrf_radio_set_interrupt_enable(0);

    /* Radio config */
    nrf_radio_set_txpower(NRF_RADIO_TXPOWER_MAX);
    nrf_radio_set_channel(NRF_RADIO_DEFAULT_CHANNEL);	// Frequency bin 25, 2425MHz
    nrf_radio_set_radio_mode(NRF_RADIO_MODE);

    /* This value needs to correspond to the channel being used */
    // TODO CR: this is not correct with channel hopping
    NRF_RADIO->DATAWHITEIV = nrf_radio_get_channel();

    /* Configure Access Address according to the BLE standard */
    NRF_RADIO->PREFIX0 = 0x8e;
    NRF_RADIO->BASE0 = 0x89bed500;

    NRF_RADIO->BASE0   = 0UL;//0xE7E7E7E7;
    NRF_RADIO->BASE1   = 0UL; //0x43434343
    NRF_RADIO->PREFIX0 = 0x55; //0x23C343E7
    NRF_RADIO->PREFIX1 = 0x55; //0x13E363A3

    /* Use logical address 0 (prefix0 + base0) = 0x8E89BED6 when transmitting and receiving */
    NRF_RADIO->TXADDRESS = 0x00;
    NRF_RADIO->RXADDRESSES = 0x01;

    /* Packet configuration */
    NRF_RADIO->PCNF0 = (PACKET0_S1_SIZE << RADIO_PCNF0_S1LEN_Pos) |
		       (PACKET0_S0_SIZE << RADIO_PCNF0_S0LEN_Pos) |
		       (PACKET0_PAYLOAD_SIZE << RADIO_PCNF0_LFLEN_Pos); //lint !e845 "The right argument to operator '|' is certain to be 0"

    /* Packet configuration */
     NRF_RADIO->PCNF1 = (RADIO_PCNF1_WHITEEN_Disabled << RADIO_PCNF1_WHITEEN_Pos)    |
			(RADIO_PCNF1_ENDIAN_Little << RADIO_PCNF1_ENDIAN_Pos)       |
			(PACKET1_BASE_ADDRESS_LENGTH << RADIO_PCNF1_BALEN_Pos)      |
			(PACKET1_STATIC_LENGTH << RADIO_PCNF1_STATLEN_Pos)          |
			(PACKET1_PAYLOAD_SIZE << RADIO_PCNF1_MAXLEN_Pos); //lint !e845 "The right argument to operator '|' is certain to be 0"

    /* Configure default buffer */
    NRF_RADIO->PACKETPTR = (uint32_t)tx_buffer;

    /* CRC Config */
    //NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos); // Number of checksum bits
     NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos); // Number of checksum bits

    //disable CRC
    //NRF_RADIO->CRCINIT = 0x0UL;      /* CRC initial value */
    //NRF_RADIO->CRCPOLY = 0x0UL;      /* CRC polynomial in explicit +1 format */                
    //NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Disabled << RADIO_CRCCNF_LEN_Pos); // Number of checksum bits

    if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos))
    {
      NRF_RADIO->CRCINIT = 0xFFFFUL;      // Initial value
      NRF_RADIO->CRCPOLY = 0x11021UL;     // CRC poly: x^16+x^12^x^5+1
    }
    else if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_One << RADIO_CRCCNF_LEN_Pos))
    {
      NRF_RADIO->CRCINIT = 0xFFUL;        // Initial value
      NRF_RADIO->CRCPOLY = 0x107UL;       // CRC poly: x^8+x^2^x^1+1
    }

    else if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos))
    {
      NRF_RADIO->CRCINIT = 0x555555;                                                  /* Initial value of CRC */
      NRF_RADIO->CRCPOLY = 0x00065B;                                                  /* CRC polynomial function */
    }

  /* Config Shortcuts like in page 86 and 88 of nRF series ref man */
#if RADIO_SHORTS_ENABLED
  NRF_RADIO->SHORTS |= 	(RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos)
		    |	(RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos);
#endif

#if 0 //RADIO_BCC_ENABLED
  NRF_RADIO->SHORTS |= RADIO_SHORTS_ADDRESS_BCSTART_Enabled << RADIO_SHORTS_ADDRESS_BCSTART_Pos;
  /* How many bits do we want to count? */
  NRF_RADIO->BCC = 192;

  /* TODO CR:	should this be here or in RADIO_INTERRUPT_ENABLED? */

  NRF_RADIO->INTENSET |= RADIO_INTENSET_BCMATCH_Msk;

  NVIC_SetPriority (RADIO_IRQn, 10);
  NVIC_ClearPendingIRQ (RADIO_IRQn);
  NVIC_EnableIRQ (RADIO_IRQn);

#endif

#if RADIO_RSSI_ENABLED
  NRF_RADIO->SHORTS |= RADIO_SHORTS_ADDRESS_RSSISTART_Msk;
  NRF_RADIO->SHORTS |= RADIO_SHORTS_DISABLED_RSSISTOP_Msk;
#endif

    process_start(&nrf_radio_process, NULL);

    return 1;
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_prepare(const void *payload, unsigned short payload_len)
{
  GET_LOCK();

  NRF_RADIO->PACKETPTR = (uint32_t)tx_buffer; /* Switch the buffer to TX */

#if RADIO_SHORTS_ENABLED

  /* If we have a packet with a length field which is exclusive, use this one */
#if (PACKET1_STATIC_LENGTH != 0)
    {
      // Frame to be send (payload_len + HEADER_LEN bytes (for length field))
      uint8_t frame_length = payload_len + HEADER_LEN;
      uint8_t frame[frame_length];

      // Set the length field of the packet
      frame[0] = payload_len;

      // Copy the payload to the frame
      memcpy (frame + HEADER_LEN, payload, payload_len);

      // Copy the frame to the send buffer
      memcpy (tx_buffer, (uint32_t*) frame, frame_length);
    }
#else
    {
      // Copy the packet to the send buffer
      memcpy (tx_buffer, (uint32_t*) payload, payload_len);
      NRF_RADIO_RESET_EVENTS();
    }
#endif

#else

  NRF_RADIO->EVENTS_READY = 0; 		/* Clear ready register */
  NRF_RADIO->TASKS_TXEN = 1; 		/* Enable the radio in TX mode */

  // Copy the packet to the send buffer
  memcpy (tx_buffer, (uint32_t*) payload, payload_len);

  while (NRF_RADIO->EVENTS_READY == 0U); 	/* Wait for the radio to ramp up */
  tx_ready = 1;

  NRF_RADIO->EVENTS_END = 0UL;			/* Clear END event register */
  NRF_RADIO->EVENTS_ADDRESS = 0UL;		/* Clear ADDRESS event register */

#endif

  RELEASE_LOCK();
  return 1;
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_transmit(unsigned short transmit_len)
{
  if(locked) {
    return 0;
  }
  GET_LOCK();


  /* Disable the radio if it was in RX mode */
  NRF_RADIO_FAST_OFF();

  NRF_RADIO->PACKETPTR = (uint32_t)tx_buffer; /* Switch the buffer to TX */

#if (! interrupt_enabled)		/* Transmitting without interrupts */

  NRF_RADIO->EVENTS_END = 0U;			/* Clear END event register */
  NRF_RADIO->EVENTS_ADDRESS = 0U;		/* Clear ADDRESS event register */

  if (RADIO_SHORTS_ENABLED)
    {
      NRF_RADIO->TASKS_TXEN = 1;		/* Enable the radio in TX mode and start sending */
    }
  else
    {
      NRF_RADIO->EVENTS_READY = 0;		/* Clear ready register */
      NRF_RADIO->TASKS_TXEN = 1;		/* Enable the radio in TX mode */
      while(NRF_RADIO->EVENTS_READY == 0U);	/* Wait for the radio to ramp up */
      NRF_RADIO->TASKS_START = 1;		/* Start the transmission */
    }

  while(NRF_RADIO->EVENTS_ADDRESS == 0U);		/* Wait for the ADDRESS event to occur */

  if (! RADIO_SHORTS_ENABLED)
    {
      while(NRF_RADIO->EVENTS_END == 0U);		/* Wait for the transmission to finish */
      NRF_RADIO_FAST_OFF();				/* Switch the radio off */
    }
#endif

  RELEASE_LOCK();
  PRINTF("PACKET SEND\n\r");

#warning "parameter transmit_len not used"

  return 1;
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_send(const void *payload, unsigned short payload_len)
{
  nrf_radio_prepare(payload, payload_len);
  return nrf_radio_transmit(payload_len);
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_read(void *buf, unsigned short buf_len)
{
  int length = 0;

  if(locked) {
    return 0;
  }
  GET_LOCK();

  if (NRF_RADIO->CRCSTATUS == RADIO_CRCSTATUS_CRCSTATUS_CRCOk)
    {
      //PRINTF("PACKET RECEIVED\n\r");

      /* Read the length of the packet */
      length = rx_buffer[0];

      if (length > buf_len)
	{
	  PRINTF("ERROR: packet is too large!\n");
	}
      else
	{
	  /* Copy contents of the rx_buffer (without length field) to buf */
	  memcpy (buf, (const uint8_t *) (rx_buffer), length);

	  // Use this if packet length is excluded..
	  //memcpy (buf, (const char *) (rx_buffer + HEADER_LEN ), length);
	}
    }

  else if (NRF_RADIO->CRCSTATUS == RADIO_CRCSTATUS_CRCSTATUS_CRCError)
    {
      PRINTF("PACKET RECEIVE FAILED\n");
    }

  if (RADIO_SHORTS_ENABLED)
    {
      /* Radio switches of automatically after receiving a packet */
      receive_on = 0;
    }

  RELEASE_LOCK ();
  // Return length of the packet
  return length;
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_receiving_packet(void)
{

      /*
       * Poll for ADDRESS event
       *
       * The register must be cleared beforehand in radio_on()
       */
      //while (NRF_RADIO->EVENTS_ADDRESS == 0);
      return NRF_RADIO->EVENTS_ADDRESS != 0;
  }
/*---------------------------------------------------------------------------*/
int
nrf_radio_pending_packet(void)
{

      /*
       * Poll for END event
       *
       * The register must be cleared beforehand in radio_on()
       */
      //while (NRF_RADIO->EVENTS_END == 0);
      return NRF_RADIO->EVENTS_END != 0;
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_on(void)
{
  if(receive_on) {
      return 1;
    }
    if(locked) {
      lock_on = 1;
      return 1;
    }

    GET_LOCK();
    on();
    RELEASE_LOCK();
    return 1;

}
/*---------------------------------------------------------------------------*/
int
nrf_radio_off(void)
{
  /* Don't do anything if we are already turned off. */
  if(receive_on == 0) {
    return 1;
  }

  /* If we are called when the driver is locked, we indicate that the
     radio should be turned off when the lock is unlocked. */
  if(locked) {
    /*    printf("Off when locked (%d)\n", locked);*/
    lock_off = 1;
    return 1;
  }

  GET_LOCK();
  off();
  RELEASE_LOCK();
  return 1;
}
/*---------------------------------------------------------------------------*/
void
on(void)
{
  /* Switch the buffer to RX */
  NRF_RADIO->PACKETPTR = (uint32_t)rx_buffer;

  if (! interrupt_enabled)
      {
        /* Clear ADDRESS register
         *
         * Is this needed for receiving_packet() function? */
        NRF_RADIO->EVENTS_ADDRESS = 0U;

        /* Clear END register
         *
         * Is this needed for pending_packet() function? */
        NRF_RADIO->EVENTS_END = 0U;
      }

    if (RADIO_SHORTS_ENABLED)
      {
        NRF_RADIO->TASKS_RXEN = 1U;		/* Enable the radio in RX mode, radio will do the rest */
      }
    else
      {
        NRF_RADIO->EVENTS_READY = 0U;		/* Clear ready register */
        NRF_RADIO->TASKS_RXEN = 1U;		/* Enable the radio in RX mode */
        while(NRF_RADIO->EVENTS_READY == 0U);	/* Wait for the radio to ramp up */
        NRF_RADIO->TASKS_START = 1U;		/* Start the reception */
      }

    receive_on = 1;
}
/*---------------------------------------------------------------------------*/
void
off(void)
{
  receive_on = 0;
  tx_ready = 0;

  /* Clear event register */
  NRF_RADIO->EVENTS_DISABLED = 0U;
  /* Disable radio */
  NRF_RADIO->TASKS_DISABLE = 1U;
  while(NRF_RADIO->EVENTS_DISABLED == 0U);

}
/*---------------------------------------------------------------------------*/
void
nrf_radio_flushrx(void)
{
  // Flush the rx_buffer
  memset(rx_buffer, 0, 1);
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_set_radio_mode(int m)
{
  switch(m) {
    case RADIO_MODE_MODE_Ble_1Mbit:

      /* Override if this is enabled by the device */
      if (NRF_FICR->OVERRIDEEN == (FICR_OVERRIDEEN_BLE_1MBIT_Override << FICR_OVERRIDEEN_BLE_1MBIT_Pos))
        {
	printf("Overriding the BLE registers\n");
  	/* Enable the override */
  	NRF_RADIO->OVERRIDE4= RADIO_OVERRIDE4_ENABLE_Enabled << RADIO_OVERRIDE4_ENABLE_Pos;
  	/* Override the registers */
  	NRF_RADIO->OVERRIDE0 = (NRF_FICR->BLE_1MBIT[0]) << RADIO_OVERRIDE0_OVERRIDE0_Pos;
  	NRF_RADIO->OVERRIDE1 = (NRF_FICR->BLE_1MBIT[1]) << RADIO_OVERRIDE1_OVERRIDE1_Pos;
  	NRF_RADIO->OVERRIDE2 = (NRF_FICR->BLE_1MBIT[2]) << RADIO_OVERRIDE2_OVERRIDE2_Pos;
  	NRF_RADIO->OVERRIDE3 = (NRF_FICR->BLE_1MBIT[3]) << RADIO_OVERRIDE3_OVERRIDE3_Pos;
  	NRF_RADIO->OVERRIDE4 = (NRF_FICR->BLE_1MBIT[4]) << RADIO_OVERRIDE4_OVERRIDE4_Pos;
      }

      NRF_RADIO->MODE = (RADIO_MODE_MODE_Ble_1Mbit << RADIO_MODE_MODE_Pos);
      return RADIO_MODE_MODE_Ble_1Mbit;

    case RADIO_MODE_MODE_Nrf_250Kbit:
      NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_250Kbit << RADIO_MODE_MODE_Pos);
      return RADIO_MODE_MODE_Nrf_250Kbit;

    case RADIO_MODE_MODE_Nrf_2Mbit:
      NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_2Mbit << RADIO_MODE_MODE_Pos);
      return RADIO_MODE_MODE_Nrf_2Mbit;

    case RADIO_MODE_MODE_Nrf_1Mbit:
      /* Override if this is enabled by the device */
      if (NRF_FICR->OVERRIDEEN == (FICR_OVERRIDEEN_NRF_1MBIT_Override << FICR_OVERRIDEEN_NRF_1MBIT_Pos))
        {
	printf("Overriding the NRF registers\n");
  	/* Enable the override */
  	NRF_RADIO->OVERRIDE4= RADIO_OVERRIDE4_ENABLE_Enabled << RADIO_OVERRIDE4_ENABLE_Pos;
  	/* Override the registers */
  	NRF_RADIO->OVERRIDE0 = (NRF_FICR->NRF_1MBIT[0]) << RADIO_OVERRIDE0_OVERRIDE0_Pos;
  	NRF_RADIO->OVERRIDE1 = (NRF_FICR->NRF_1MBIT[1]) << RADIO_OVERRIDE1_OVERRIDE1_Pos;
  	NRF_RADIO->OVERRIDE2 = (NRF_FICR->NRF_1MBIT[2]) << RADIO_OVERRIDE2_OVERRIDE2_Pos;
  	NRF_RADIO->OVERRIDE3 = (NRF_FICR->NRF_1MBIT[3]) << RADIO_OVERRIDE3_OVERRIDE3_Pos;
  	NRF_RADIO->OVERRIDE4 = (NRF_FICR->NRF_1MBIT[4]) << RADIO_OVERRIDE4_OVERRIDE4_Pos;
      }

      NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_1Mbit << RADIO_MODE_MODE_Pos);
      return RADIO_MODE_MODE_Nrf_1Mbit;

    default:
      printf ("RADIO MODE NOT SET!\n");
      return 0;
    }
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_set_channel(int c)
{
#if (RADIO_MODE == RADIO_MODE_MODE_Ble_1Mbit)
  uint8_t frequency = 0;

  if (c < 0 || c > (MAX_CHANNELS - 1))
    {
      PRINTF("Channel NOT set!\n\r");
      return 0;
    }

  channel = c;

  switch(channel)
  {
    /* Data Channels */
    case 0 ... 10:
      frequency = (2 * channel) + 4;
      break;
    case 11 ... 36:
      frequency = (2 * channel) + 6;
      break;

    /* Advertisement Channels */
    case 37:
      frequency = 2;
      break;
    case 38:
      frequency = 26;
      break;
    case 39:
      frequency = 80;
      break;
    default:
      frequency = 26;
      break;
  }

  NRF_RADIO->FREQUENCY = frequency;
  return c;

#else

    channel = c;

    NRF_RADIO->FREQUENCY = (uint8_t)channel;
    return c;
#endif

}
/*---------------------------------------------------------------------------*/
int
nrf_radio_get_channel(void)
{
  /* TODO CR: get frequency from actual NRF_RADIO->FREQUENCY register
  and convert it back to the channel */

  return channel;
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_set_txpower(int power)
{

  /* TODO CR: make an input check here */

  NRF_RADIO->TXPOWER = power << RADIO_TXPOWER_TXPOWER_Pos;
  return 1;
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_get_txpower(void)
{
  return NRF_RADIO->TXPOWER;
}
/*---------------------------------------------------------------------------*/
int
nrf_radio_get_rssi(void)
{
  int rssi = 0;

  rssi = NRF_RADIO->RSSISAMPLE;

  return rssi;
}
/*---------------------------------------------------------------------------*/
rtimer_clock_t
nrf_radio_read_timestamp(int event)
{
  /* Read the last address or end timestamp from the TIMER0 capture register */

  if (event == ADDRESS_EVENT)
    {
      return NRF_TIMER0->CC[TIMESTAMP_ADDR_REG];
    }
  else if (event == END_EVENT)
    {
      return NRF_TIMER0->CC[TIMESTAMP_END_REG];
    }
  else
    {
      return 0;
    }

}
/*---------------------------------------------------------------------------*/
int
nrf_radio_sfd_sync(uint8_t capture_start_sfd, uint8_t capture_end_sfd)
{
  /* Configures timer 0 to capture SFD edge (start, end, both) */

    if(capture_start_sfd && capture_end_sfd) {

	/* Capture both Address and End events of radio
	 * Pre-defined channel 26: Address Event -> TIMER0-CC[1]
	 * Pre-defined channel 27: End Event -> TIMER0-CC[2] */
	NRF_PPI->CHEN |= (PPI_CHEN_CH26_Enabled << PPI_CHEN_CH26_Pos) |
			 (PPI_CHEN_CH27_Enabled << PPI_CHEN_CH27_Pos);
	return 1;

    } else if(capture_end_sfd){

	/* Pre-defined channel: END Event -> Timer0-CC[2] */
	//NRF_PPI->CHEN |= (PPI_CHEN_CH27_Enabled << PPI_CHEN_CH27_Pos);
	return 0;

    } else if(capture_start_sfd) {
	/* Pre-defined channel: Address Event -> Timer0-CC[1] */
	NRF_PPI->CHEN |= (PPI_CHEN_CH26_Enabled << PPI_CHEN_CH26_Pos);
	return 1;

    } else {
	/* Disable sfd sync */
	NRF_PPI->CHEN &= (PPI_CHEN_CH26_Disabled << PPI_CHEN_CH26_Pos) |
			 (PPI_CHEN_CH27_Disabled << PPI_CHEN_CH27_Pos);
	return 0;
    }

}
/*---------------------------------------------------------------------------*/
void
nrf_radio_set_interrupt_enable(uint8_t e)
{
  GET_LOCK();

  interrupt_enabled = e;

  if (e)
    {
      /* Enable interrupts for specific events */
      NRF_RADIO->INTENSET |= RADIO_INTENSET_ADDRESS_Msk;
      NRF_RADIO->INTENSET |= RADIO_INTENSET_END_Msk;
      NRF_RADIO->INTENSET |= RADIO_INTENSET_BCMATCH_Msk;

      NVIC_SetPriority (RADIO_IRQn, 10);
      NVIC_ClearPendingIRQ (RADIO_IRQn);
      NVIC_EnableIRQ (RADIO_IRQn);
    }
  else
    {
      NRF_RADIO->INTENCLR |= RADIO_INTENSET_ADDRESS_Msk;
      NRF_RADIO->INTENCLR |= RADIO_INTENSET_END_Msk;
      NRF_RADIO->INTENCLR |= RADIO_INTENSET_BCMATCH_Msk;
      NVIC_DisableIRQ (RADIO_IRQn);
    }
  RELEASE_LOCK ();
}
/*---------------------------------------------------------------------------*/
void
RADIO_IRQHandler(void)
{

#if RADIO_BCC_ENABLED
  if (NRF_RADIO->EVENTS_BCMATCH == 1)
    {
      //Clear the interrupt register
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_BCMATCH_Clear << RADIO_INTENCLR_BCMATCH_Pos;

      //Clear the event register
      NRF_RADIO->EVENTS_BCMATCH = 0;

      //Read out the capture registers of the Address event and the BCMatch event
      time = NRF_TIMER0->CC[BCC_REG];
      ref_time = NRF_TIMER0->CC[TIMESTAMP_ADDR_REG];

      /*
       Disable the Bit counter, it will be restarted by the shortcut
       * between Address event and the BCStart task. */

      NRF_RADIO->TASKS_BCSTOP = 1;

      //Re-enable the interrupt
      NRF_RADIO->INTENSET |= RADIO_INTENSET_BCMATCH_Msk;

      PRINTF("BC MATCH! \t\t Measured timer ticks: %u -----\n\r", (uint)(time - ref_time));
    }
#endif
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nrf_radio_process, ev, data)
{
  int len;
  PROCESS_BEGIN();

  PRINTF("nrf_radio_process: started\n\r");

  while(1) {
    PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);

    PRINTF("nrf_radio_process: calling receiver callback\n\r");

    packetbuf_clear();
    //packetbuf_set_attr(PACKETBUF_ATTR_TIMESTAMP, last_packet_timestamp);
    len = nrf_radio_read(rx_buffer, PACKETBUF_SIZE);

    //nrf_radio_read(rxbuffer, 8);

    //packetbuf_set_datalen(len);

    NETSTACK_RDC.input();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

