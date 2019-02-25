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
 *         MSP430-specific rtimer code
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"

#include "sys/energest.h"
#include "sys/rtimer.h"
#include "sys/process.h"
#include "dev/watchdog.h"
#include "isr_compat.h"
#include "dev/leds.h"

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define LEDS_ON(X) leds_on((X))
#define LEDS_OFF(X) leds_off((X))
#define ENABLE_COOJA_DEBUG 1
#else
#define PRINTF(...)
#define LEDS_ON(X)
#define LEDS_OFF(X)
#define ENABLE_COOJA_DEBUG 0
#endif

#include "dev/cooja-debug.h"

//static uint16_t read_tar(void);
/*---------------------------------------------------------------------------*/
ISR(TIMERA0, timera0)
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);
  LEDS_ON(LEDS_BLUE);
  /* HW timer bug fix: Interrupt handler called before TR==CCR.
   * Occurs when timer state is toggled between STOP and CONT. */
//  int delta = (int16_t)((uint16_t)TACCR0 - read_tar());
//  if( delta < -10 || delta > 10){
//    COOJA_DEBUG_STR("rtimer_arch_ISR BUG");
//    LEDS_OFF(LEDS_BLUE);
//    ENERGEST_OFF(ENERGEST_TYPE_IRQ);
//    return;
//  }
//  COOJA_DEBUG_STR("rtimer_arch_ISR");

  watchdog_start();
  rtimer_run_next();
  if(process_nevents() > 0) {
    LPM4_EXIT;
  }

  watchdog_stop();

  LEDS_OFF(LEDS_BLUE);
  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
void
rtimer_arch_init(void)
{
  dint();

  /* CCR0 interrupt enabled, interrupt occurs when timer equals CCR0. */
  TACCTL0 = CCIE;

  /* Enable interrupts. */
  eint();
}
///*---------------------------------------------------------------------------*/
//static uint16_t
//read_tar(void)
//{
//  uint16_t t1, t2;
//  do {
//    t1 = TAR;
//    t2 = TAR;
//  } while(t1 != t2);
//  return t1;
//}
/*---------------------------------------------------------------------------*/
//rtimer_clock_t
//rtimer_arch_now(void)
//{
//  rtimer_clock_t t1, t2;
//  do {
//    t1 = TAR;
//    t2 = TAR;
//  } while(t1 != t2);
//  return t1;
//}
/*---------------------------------------------------------------------------*/
//rtimer_clock_t
//rtimer_arch_now_dco(void)
//{
//	/* it is changing too quickly to capture */
//  /*rtimer_clock_t t1, t2;
//  do {
//    t1 = TBR;
//    t2 = TBR;
//  } while(t1 != t2);
//  return t1;*/
//	return (TBR);
//}
/*---------------------------------------------------------------------------*/
void
rtimer_arch_schedule(rtimer_clock_t t)
{
  PRINTF("rtimer_arch_schedule time %u\n", t);

  TACCR0 = t;
}
/*---------------------------------------------------------------------------*/
