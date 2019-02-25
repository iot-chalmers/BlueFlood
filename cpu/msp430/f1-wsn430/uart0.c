/*
 * Copyright (c) 2006, Swedish Institute of Computer Science
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
 */

/*
 * Machine dependent MSP430 UART0 code.
 */

#include "contiki.h"
#include "sys/energest.h"
#include "dev/uart0.h"
#include "dev/watchdog.h"
#include "sys/ctimer.h"
#include "lib/ringbuf.h"
#include "isr_compat.h"

static int (*uart0_input_handler)(unsigned char c);
static volatile uint8_t rx_in_progress;

static volatile uint8_t transmitting;

#ifdef UART0_CONF_TX_WITH_INTERRUPT
#define TX_WITH_INTERRUPT UART0_CONF_TX_WITH_INTERRUPT
#else /* UART0_CONF_TX_WITH_INTERRUPT */
#define TX_WITH_INTERRUPT 0
#endif /* UART0_CONF_TX_WITH_INTERRUPT */

#ifdef UART0_CONF_RX_WITH_DMA
#define RX_WITH_DMA UART0_CONF_RX_WITH_DMA
#else /* UART0_CONF_RX_WITH_DMA */
#define RX_WITH_DMA 1
#endif /* UART0_CONF_RX_WITH_DMA */

#if TX_WITH_INTERRUPT
#define TXBUFSIZE 128

static struct ringbuf txbuf;
static uint8_t txbuf_data[TXBUFSIZE];
#endif /* TX_WITH_INTERRUPT */

#if RX_WITH_DMA
#define RXBUFSIZE 128

static uint8_t rxbuf[RXBUFSIZE];
static uint16_t last_size;
static struct ctimer rxdma_timer;

static void
handle_rxdma_timer(void *ptr)
{
  uint16_t size;
  size = DMA0SZ; /* Note: loop requires that size is less or eq to RXBUFSIZE */
  while(last_size != size) {
/*     printf("read: %c [%d,%d]\n", (unsigned char)rxbuf[RXBUFSIZE - last_size], */
/* 	   last_size, size); */
    uart0_input_handler((unsigned char)rxbuf[RXBUFSIZE - last_size]);
    last_size--;
    if(last_size == 0) last_size = RXBUFSIZE;
  }

  ctimer_reset(&rxdma_timer);
}
#endif /* RX_WITH_DMA */

/*---------------------------------------------------------------------------*/
uint8_t
uart0_active(void)
{
  return ((~ UTCTL0) & TXEPT) | rx_in_progress | transmitting;
}
/*---------------------------------------------------------------------------*/
void
uart0_set_input(int (*input)(unsigned char c))
{
#if RX_WITH_DMA /* This needs to be called after ctimer process is started */
  ctimer_set(&rxdma_timer, CLOCK_SECOND/64, handle_rxdma_timer, NULL);
#endif
  uart0_input_handler = input;
}
/*---------------------------------------------------------------------------*/
void
uart0_writeb(unsigned char c)
{
  watchdog_periodic();
#if TX_WITH_INTERRUPT

  /* Put the outgoing byte on the transmission buffer. If the buffer
     is full, we just keep on trying to put the byte into the buffer
     until it is possible to put it there. */
  while(ringbuf_put(&txbuf, c) == 0);

  /* If there is no transmission going, we need to start it by putting
     the first byte into the UART. */
  if(transmitting == 0) {
    transmitting = 1;

    /* Loop until the transmission buffer is available. */
    /*while((IFG1 & UTXIFG0) == 0);*/
    TXBUF0 = ringbuf_get(&txbuf);
  }

#else /* TX_WITH_INTERRUPT */

  /* Loop until the transmission buffer is available. */
  while((IFG1 & UTXIFG0) == 0);

  /* Transmit the data. */
  TXBUF0 = c;
#endif /* TX_WITH_INTERRUPT */
}
/*---------------------------------------------------------------------------*/
/**
 * Initalize the RS232 port.
 *
 */
void
uart0_init(unsigned long ubr)
{
  /* RS232 */
  P3DIR &= ~0x20;			/* Select P35 for input (UART0RX) */
  P3DIR |= 0x10;			/* Select P34 for output (UART0TX) */
  P3SEL |= 0x30;			/* Select P3.4,P3.5 for UART0{TX,RX} */

  UCTL0 = SWRST | CHAR;                 /* 8-bit character, UART mode */

#if 0
  U0RCTL &= ~URXEIE; /* even erroneous characters trigger interrupts */
#endif

  UTCTL0 = SSEL1;                       /* UCLK = MCLK */

  UBR00 = ubr;
  UBR10 = ubr >> 8;
  /*
   * UMCTL0 values calculated using
   * http://mspgcc.sourceforge.net/baudrate.html
   */
  switch(ubr) {

#if F_CPU == 3900000ul

  case UART0_BAUD2UBR(115200ul):
    UMCTL0 = 0xF7;
    break;
  case UART0_BAUD2UBR(57600ul):
    UMCTL0 = 0xED;
    break;
  case UART0_BAUD2UBR(38400ul):
    UMCTL0 = 0xD6;
    break;
  case UART0_BAUD2UBR(19200ul):
    UMCTL0 = 0x08;
    break;
  case UART0_BAUD2UBR(9600ul):
    UMCTL0 = 0x22;
    break;

#elif F_CPU == 3932160ul

  case UART0_BAUD2UBR(115200ul):
    UMCTL0 = 0x08;
    break;
  case UART0_BAUD2UBR(57600ul):
    UMCTL0 = 0x22;
    break;
  case UART0_BAUD2UBR(38400ul):
    UMCTL0 = 0x29;
    break;
  case UART0_BAUD2UBR(19200ul):
    UMCTL0 = 0x7B;
    break;
  case UART0_BAUD2UBR(9600ul):
    UMCTL0 = 0xB5;
    break;

#elif F_CPU == 4194304ul

  case UART0_BAUD2UBR(115200ul):
    UMCTL0 = 0x29;
    break;
  case UART0_BAUD2UBR(57600ul):
    UMCTL0 = 0x7B;
    break;
  case UART0_BAUD2UBR(38400ul):
    UMCTL0 = 0x44;
    break;
  case UART0_BAUD2UBR(19200ul):
    UMCTL0 = 0xAA;
    break;
  case UART0_BAUD2UBR(9600ul):
    UMCTL0 = 0xDF;
    break;

#elif F_CPU == 8000000ul

  case UART0_BAUD2UBR(115200ul):
    UMCTL0 = 0xAA;
    break;

#elif F_CPU == 2457600ul

  case UART0_BAUD2UBR(115200ul):
    UMCTL0 = 0x4A;
    break;
  case UART0_BAUD2UBR(57600ul):
    UMCTL0 = 0x5B;
    break;
  default:
    /* 9600, 19200, 38400 don't require any correction */
    UMCTL0 = 0x00;

#else

#error Unsupported CPU speed in uart0.c

#endif
  }

  ME1 &= ~USPIE0;     /* USART0 SPI module disable */
  ME1 |= (UTXE0 | URXE0);               /* Enable USART0 TXD/RXD */

  UCTL0 &= ~SWRST;

  /* XXX Clear pending interrupts before enable!!! */
  IFG1 &= ~URXIFG0;
  U0TCTL |= URXSE;

  rx_in_progress = 0;

  transmitting = 0;

  IE1 |= URXIE0;                        /* Enable USART0 RX interrupt  */
#if TX_WITH_INTERRUPT
  ringbuf_init(&txbuf, txbuf_data, sizeof(txbuf_data));
  IE1 |= UTXIE0;                        /* Enable USART0 TX interrupt  */
#endif /* TX_WITH_INTERRUPT */

#if RX_WITH_DMA
  IE1 &= ~URXIE0; /* disable USART0 RX interrupt  */
  /* UART0_RX trigger */
  DMACTL0 = DMA0TSEL_3;

  /* source address = RXBUF0 */
  DMA0SA = (unsigned int) &RXBUF0;
  DMA0DA = (unsigned int) &rxbuf;
  DMA0SZ = RXBUFSIZE;
  last_size = RXBUFSIZE;
  DMA0CTL = DMADT_4 + DMASBDB + DMADSTINCR_3 + DMAEN + DMAREQ;// DMAIE;

  msp430_add_lpm_req(MSP430_REQUIRE_LPM1);
#endif /* RX_WITH_DMA */
}
/*---------------------------------------------------------------------------*/
#if !RX_WITH_DMA
ISR(UART0RX, uart0_rx_interrupt)
{
  uint8_t c;
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  if(!(URXIFG0 & IFG1)) {
    /* Edge detect if IFG not set? */
    U0TCTL &= ~URXSE; /* Clear the URXS signal */
    U0TCTL |= URXSE;  /* Re-enable URXS - needed here?*/
    rx_in_progress = 1;
    LPM4_EXIT;
  } else {
    rx_in_progress = 0;
    /* Check status register for receive errors. */
    if(URCTL0 & RXERR) {
      c = RXBUF0;   /* Clear error flags by forcing a dummy read. */
    } else {
      c = RXBUF0;
      if(uart0_input_handler != NULL) {
	if(uart0_input_handler(c)) {
	  LPM4_EXIT;
	}
      }
    }
  }

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
#endif /* !RX_WITH_DMA */
/*---------------------------------------------------------------------------*/
#if TX_WITH_INTERRUPT
ISR(UART0TX, uart0_tx_interrupt)
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  if(ringbuf_elements(&txbuf) == 0) {
    transmitting = 0;
  } else {
    TXBUF0 = ringbuf_get(&txbuf);
  }

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
#endif /* TX_WITH_INTERRUPT */
/*---------------------------------------------------------------------------*/
