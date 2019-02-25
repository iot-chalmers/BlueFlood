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
 *         Header file for the MSP430-specific rtimer code
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#ifndef RTIMER_ARCH_H_
#define RTIMER_ARCH_H_

#include "contiki.h"
#include "sys/rtimer.h"


#ifdef RTIMER_CONF_SECOND
#define RTIMER_ARCH_SECOND RTIMER_CONF_SECOND
#else
#define RTIMER_ARCH_SECOND (4096U*8)
#endif

#if CONTIKI_TARGET_SKY || CONTIKI_TARGET_Z1
#define TLF TAR
#define THF TBR
#define T_TICK_CAPTURE_REG TBCCR6
#else
#define TLF TBR
#define THF TAR
#define T_TICK_CAPTURE_REG TACCR2
#endif

/* used in RTIMER_DCO_SYNC() */
extern volatile rtimer_clock_t rtimer_ref, dco_ref;

//VHT f = 2^16 * RTIMER_F --> With RTIMER_F=32KHz ==> f=2GHz==2^31Hz
#define RT_VHT_PHI (1uL<<16uL)
#define CLOCK_PHI  ((vht_clock_t)F_CPU / RTIMER_ARCH_SECOND) //in cooja: 3932160ul/32768 = 120 vs. 128
#define DCO_VHT_PHI (((vht_clock_t)(RT_VHT_PHI))/CLOCK_PHI)//in cooja:

typedef uint32_t vht_clock_t;

typedef struct vht_rtimer_dco_clockt {
  vht_clock_t vht;
  rtimer_clock_t rtimer;
  rtimer_clock_t dco;
} vht_rtimer_dco_clockt_t;

/* timer macros */
#define DCO_NOW() (rtimer_arch_now_dco())
#define VHT_NOW() (rtimer_arch_now_vht())
#define RTIMER_DCO_SYNC() (rtimer_sync_dco())
#define VHT_RTIMER_DCO_NOW() (rtimer_arch_now_vht_rtimer_dco())

//captured dco tick on last rtimer tick
#define VHT_DCO_NOW() (rtimer_arch_now_vht_dco())
#define RTIMER_TO_DCO(X) ((uint16_t)((X) * CLOCK_PHI))
#define DCO_TO_RTIMER(X) ((uint16_t)((X) / CLOCK_PHI))
#define RTIMER_TO_DCO_U32(X) ((uint32_t)((X) * CLOCK_PHI))
#define RTIMER_TO_US(X) ((uint32_t)((X*1000uL) / (RTIMER_ARCH_SECOND/1000uL)))

#define RTIMER_LT(a,b) ((int16_t)((a)-(b)) < 0)
#define DCO_LT(a,b) ((int16_t)((a)-(b)) < 0)
#define VHT_LT(a,b) ((int32_t)((a)-(b)) < 0)
#define VHT_TO_RTIMER(X) ( (rtimer_clock_t)   ((X) / (vht_clock_t)(RT_VHT_PHI)  )  )
#define RTIMER_TO_VHT(X) ((vht_clock_t)(X)*(vht_clock_t)(RT_VHT_PHI) )
#define DCO_TO_VHT(X) ((vht_clock_t)((X)*DCO_VHT_PHI))
#define VHT_TO_DCO(X) ( (rtimer_clock_t) ((X)/DCO_VHT_PHI) )
#define VHT_TO_DCO_PART(X) ( (rtimer_clock_t)(((X)%(RT_VHT_PHI)))/DCO_VHT_PHI )
#define RTIMER_DCO_TO_VHT(X,Y) (((vht_clock_t)(X)*(RT_VHT_PHI)) + ((vht_clock_t)(Y)*DCO_VHT_PHI))
#define VHT_TO_US(X) ( (X) / ((RTIMER_ARCH_SECOND*RT_VHT_PHI)/1000000uL) )
#define DCO_TO_US(X) ( (X) / (F_CPU/1000000uL) )

#define CAPTURE_NEXT_CLOCK_TICK(t_h, t_l) \
      do { \
        volatile rtimer_clock_t t_cap_l = 0; \
        volatile rtimer_clock_t t_cap_h = 0; \
        t_cap_l = RTIMER_NOW(); \
        while(RTIMER_NOW()==t_cap_l); \
        t_h=VHT_DCO_NOW(); \
        t_l=RTIMER_NOW(); \
      } while(0)

static ALWAYS_INLINE vht_rtimer_dco_clockt_t
vht_to_vht_rtimer_dco(vht_clock_t vht){
  vht_rtimer_dco_clockt_t vht_rtimer_dco = {vht, VHT_TO_RTIMER(vht), VHT_TO_DCO_PART(vht)};
  return vht_rtimer_dco;
}

static ALWAYS_INLINE rtimer_clock_t
rtimer_arch_now(void)
{
  volatile uint16_t t1, t2;
  do {
    t1 = TLF;
    t2 = TLF;
  } while(t1 != t2);
  return t1;
}

static ALWAYS_INLINE rtimer_clock_t
rtimer_arch_now_fast(void)
{
  return TLF;
}
//captured dco tick on last rtimer tick
static ALWAYS_INLINE rtimer_clock_t
rtimer_arch_now_vht_dco(void)
{ //get the dco fraction of VHT
#if !COOJA && !CONTIKI_TARGET_WSN430
  volatile uint16_t t1, t2;
  do {
    t1 = T_TICK_CAPTURE_REG;
    t2 = T_TICK_CAPTURE_REG;
  } while( t1 != t2 );
  return t1;
#else
  return THF;
#endif
}

static ALWAYS_INLINE rtimer_clock_t
rtimer_arch_now_dco(void)
{
  /* it is changing too quickly to capture */
  /*rtimer_clock_t t1, t2;
  do {
    t1 = THF;
    t2 = THF;
  } while(t1 != t2);
  return t1;*/
  uint16_t t1 = THF;
  return t1;
}

static ALWAYS_INLINE vht_rtimer_dco_clockt_t
rtimer_arch_now_vht_rtimer_dco(void)
{
  vht_clock_t t;
  volatile uint16_t tr, tr2, tb;
#if 1
#if COOJA || CONTIKI_TARGET_WSN430
  uint16_t tt;
  do {
    tr = TLF;
    tb = THF;
    tr2 = TLF;
  } while(tr != tr2);
  tt = tb - (dco_ref + (rtimer_clock_t)((rtimer_clock_t)(tr - rtimer_ref) * CLOCK_PHI));
  t = (tt % CLOCK_PHI) * (vht_clock_t)DCO_VHT_PHI + tr * (vht_clock_t)RT_VHT_PHI;
#else
  /* Using the capture register, but does not work in COOJA */
    volatile uint16_t tb2;
    do {
      tr = TLF;
      tb = T_TICK_CAPTURE_REG;
      tr2 = TLF;
      tb2 = T_TICK_CAPTURE_REG;
    } while(tr != tr2 || tb != tb2);
    t = tr * (vht_clock_t)RT_VHT_PHI;
    tb = THF;
    t = (((tb - tb2) % CLOCK_PHI)) * (vht_clock_t)DCO_VHT_PHI + t;
#endif
#else
    /* Inspired by TinyOS module Msp430HybridAlarmCounterP */
    uint16_t td, tb1;
    // now from 32khz
    tr = TLF;
    // now from DCO
    tb1 = THF;
    // wait until the 32khz clock ticks
    while(tr == TLF);
    // elapsed time since entering this function in Micro
    tb = THF;
    td = tb - tb1;
    // hi byte of hybrid time is 32khz tick
    t = (vht_clock_t)(tr) * RT_VHT_PHI;
    // adjust with the elapsed micro time
    t -= (vht_clock_t)td * DCO_VHT_PHI;
#endif
    vht_rtimer_dco_clockt_t v = {t, tr, tb};
    return v;
}

static ALWAYS_INLINE vht_clock_t
rtimer_arch_now_vht(void)
{
  return rtimer_arch_now_vht_rtimer_dco().vht;
}

static ALWAYS_INLINE void
rtimer_sync_dco(void)
{
  /* Sync DCO (TB) with RTIMER (TA) */
  rtimer_ref = rtimer_arch_now();
  while(rtimer_arch_now_fast() == rtimer_ref);
  dco_ref = rtimer_arch_now_dco();
  rtimer_ref = rtimer_arch_now();
}

#endif /* RTIMER_ARCH_H_ */
