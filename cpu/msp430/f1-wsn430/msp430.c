/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
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
 */

#include "contiki.h"
#include "dev/watchdog.h"
#include "stdio.h"

/* dco_required set to 1 will cause the CPU not to go into
   sleep modes where the DCO clock stopped */
int msp430_dco_required;

#if defined(__MSP430__) && defined(__GNUC__)
#define asmv(arg) __asm__ __volatile__(arg)
#endif

/*---------------------------------------------------------------------------*/
#if defined(__MSP430__) && defined(__GNUC__) && MSP430_MEMCPY_WORKAROUND
void *
w_memcpy(void *out, const void *in, size_t n)
{
  uint8_t *src, *dest;
  src = (uint8_t *) in;
  dest = (uint8_t *) out;
  while(n-- > 0) {
    *dest++ = *src++;
  }
  return out;
}
#endif /* __GNUC__ &&  __MSP430__ && MSP430_MEMCPY_WORKAROUND */
/*---------------------------------------------------------------------------*/
#if defined(__MSP430__) && defined(__GNUC__) && MSP430_MEMCPY_WORKAROUND
void *
w_memset(void *out, int value, size_t n)
{
  uint8_t *dest;
  dest = (uint8_t *) out;
  while(n-- > 0) {
    *dest++ = value & 0xff;
  }
  return out;
}
#endif /* __GNUC__ &&  __MSP430__ && MSP430_MEMCPY_WORKAROUND */
/*---------------------------------------------------------------------------*/
/***************************************************************
 * we have to wait OFIFG to be sure the switch is ok
 * slau049e.pdf page 4-12 [pdf page 124]
 ***************************************************************/

#define WAIT_CRISTAL()                                                 \
do {                                                                   \
  int i;                                                               \
  do {                                                                 \
    IFG1 &= ~OFIFG;                  /* Clear OSCFault flag  */        \
    for (i = 0xff; i > 0; i--)       /* Time for flag to set */        \
       nop();                        /*                      */        \
  }  while ((IFG1 & OFIFG) != 0);    /* OSCFault flag still set? */    \
} while (0)

void
msp430_init_dco(void)
{

#if F_CPU == 8000000uL
#warning "Using 8MHz XTAL instead of DCO!"
  //set_mcu_speed_xt2_mclk_8MHz_smclk_8MHz
  DCOCTL  = 0;
  BCSCTL1 = 0;
  BCSCTL2 = SELM_2 | SELS;

  WAIT_CRISTAL();
#else
  /* This code taken from the FU Berlin sources and reformatted. */
#define DELTA    ((MSP430_CPU_SPEED) / (32768 / 8))

  unsigned int compare, oldcapture = 0;
  unsigned int i;

  BCSCTL1 = 0xa4; /* ACLK is devided by 4. RSEL=6 no division for MCLK
         and SSMCLK. XT2 is off. */

  BCSCTL2 = 0x00; /* Init FLL to desired frequency using the 32762Hz
         crystal DCO frquenzy = 2,4576 MHz  */

  BCSCTL1 |= DIVA1 + DIVA0;             /* ACLK = LFXT1CLK/8 */
  for(i = 0xffff; i > 0; i--) {         /* Delay for XTAL to settle */
    asm("nop");
  }

  CCTL2 = CCIS0 + CM0 + CAP;            /* Define CCR2, CAP, ACLK */
  TACTL = TASSEL1 + TACLR + MC1;        /* SMCLK, continous mode */


  while(1) {

    while((CCTL2 & CCIFG) != CCIFG);    /* Wait until capture occured! */
    CCTL2 &= ~CCIFG;                    /* Capture occured, clear flag */
    compare = CCR2;                     /* Get current captured SMCLK */
    compare = compare - oldcapture;     /* SMCLK difference */
    oldcapture = CCR2;                  /* Save current captured SMCLK */

    if(DELTA == compare) {
      break;                            /* if equal, leave "while(1)" */
    } else if(DELTA < compare) {        /* DCO is too fast, slow it down */
      DCOCTL--;
      if(DCOCTL == 0xFF) {              /* Did DCO role under? */
  BCSCTL1--;
      }
    } else {                            /* -> Select next lower RSEL */
      DCOCTL++;
      if(DCOCTL == 0x00) {              /* Did DCO role over? */
  BCSCTL1++;
      }
                                        /* -> Select next higher RSEL  */
    }
  }

  CCTL2 = 0;                            /* Stop CCR2 function */
  TACTL = 0;                            /* Stop Timer_A */

  BCSCTL1 &= ~(DIVA1 + DIVA0);          /* remove /8 divisor from ACLK again */
#endif
}
/*---------------------------------------------------------------------------*/
static void
init_ports(void)
{
  /* Turn everything off, device drivers enable what is needed. */

  /* All configured for digital I/O */
#ifdef P1SEL
  P1SEL = 0;
#endif
#ifdef P2SEL
  P2SEL = 0;
#endif
#ifdef P3SEL
  P3SEL = 0;
#endif
#ifdef P4SEL
  P4SEL = 0;
#endif
#ifdef P5SEL
  P5SEL = 0;
#endif
#ifdef P6SEL
  P6SEL = 0;
#endif

  /* All available inputs */
#ifdef P1DIR
  P1DIR = 0;
  P1OUT = 0;
#endif
#ifdef P2DIR
  P2DIR = 0;
  P2OUT = 0;
#endif
#ifdef P3DIR
  P3DIR = 0;
  P3OUT = 0;
#endif
#ifdef P4DIR
  P4DIR = 0;
  P4OUT = 0;
#endif

#ifdef P5DIR
  P5DIR = 0;
  P5OUT = 0;
#endif

#ifdef P6DIR
  P6DIR = 0;
  P6OUT = 0;
#endif

#ifdef P7DIR
  P7DIR = 0;
  P7OUT = 0;
#endif

#ifdef P8DIR
  P8DIR = 0;
  P8OUT = 0;
#endif

  P1IE = 0;
  P2IE = 0;
}
/*---------------------------------------------------------------------------*/
/* msp430-ld may align _end incorrectly. Workaround in cpu_init. */
#if defined(__MSP430__) && defined(__GNUC__)
extern int _end;		/* Not in sys/unistd.h */
static char *cur_break = (char *)&_end;
#endif

/*---------------------------------------------------------------------------*/
/* add/remove_lpm_req - for requiring a specific LPM mode. currently Contiki */
/* jumps to LPM3 to save power, but DMA will not work if DCO is not clocked  */
/* so some modules might need to enter their LPM requirements                */
/* NOTE: currently only works with LPM1 (e.g. DCO) requirements.             */
/*---------------------------------------------------------------------------*/
void
msp430_add_lpm_req(int req)
{
  if(req <= MSP430_REQUIRE_LPM1) {
    msp430_dco_required++;
  }
}

void
msp430_remove_lpm_req(int req)
{
  if(req <= MSP430_REQUIRE_LPM1) {
    msp430_dco_required--;
  }
}

void
msp430_cpu_init(void)
{
  dint();
  watchdog_init();
  init_ports();
  msp430_init_dco();
  eint();
#if defined(__MSP430__) && defined(__GNUC__)
  if((uintptr_t)cur_break & 1) { /* Workaround for msp430-ld bug! */
    cur_break++;
  }
#endif

  msp430_dco_required = 0;
}
/*---------------------------------------------------------------------------*/

#define STACK_EXTRA 32

/*
 * Allocate memory from the heap. Check that we don't collide with the
 * stack right now (some other routine might later). A watchdog might
 * be used to check if cur_break and the stack pointer meet during
 * runtime.
 */
#if defined(__MSP430__) && defined(__GNUC__)
void *
sbrk(int incr)
{
  char *stack_pointer;

  asmv("mov r1, %0" : "=r" (stack_pointer));
  stack_pointer -= STACK_EXTRA;
  if(incr > (stack_pointer - cur_break))
    return (void *)-1;		/* ENOMEM */

  void *old_break = cur_break;
  cur_break += incr;
  /*
   * If the stack was never here then [old_break .. cur_break] should
   * be filled with zeros.
  */
  return old_break;
}
#endif
/*---------------------------------------------------------------------------*/
/*
 * Mask all interrupts that can be masked.
 */
int
splhigh_(void)
{
  register int sr=0;
  /* Clear the GIE (General Interrupt Enable) flag. */
#ifdef __IAR_SYSTEMS_ICC__
  sr = __get_SR_register();
  __bic_SR_register(GIE);
#else
  asmv("mov r2, %0" : "=r" (sr));
  asmv("bic %0, r2" : : "i" (GIE));
#endif
  return sr & GIE;		/* Ignore other sr bits. */
}
/*---------------------------------------------------------------------------*/
/*
 * Restore previous interrupt mask.
 */
/* void */
/* splx_(int sr) */
/* { */
/* #ifdef __IAR_SYSTEMS_ICC__ */
/*   __bis_SR_register(sr); */
/* #else */
/*   /\* If GIE was set, restore it. *\/ */
/*   asmv("bis %0, r2" : : "r" (sr)); */
/* #endif */
/* } */
/*---------------------------------------------------------------------------*/
#ifdef __IAR_SYSTEMS_ICC__
int __low_level_init(void)
{
  /* turn off watchdog so that C-init will run */
  WDTCTL = WDTPW + WDTHOLD;
  /*
   * Return value:
   *
   *  1 - Perform data segment initialization.
   *  0 - Skip data segment initialization.
   */
  return 1;
}
#endif
/*---------------------------------------------------------------------------*/
#if F_CPU!=8000000uL
#if DCOSYNCH_CONF_ENABLED
/* this code will always start the TimerB if not already started */
void
msp430_sync_dco(void) {
  uint16_t last = 0;
  uint16_t diff = 0;
  uint8_t count = 0, i;
//  uint32_t dcoctl_start = DCOCTL;
  /* DELTA_2 assumes an ACLK of 32768 Hz */
#define DELTA_2    ((MSP430_CPU_SPEED) / (32768U))
  /* Repeat sync loop for 3 times (at most) where we measure 2 ticks every time
   * PS: An even number could remove unnecessary adjustments to clk divider */
#define SYNC_TICKS_32KHZ 4U
#define ACLK_TICKS 2U
  /* Select SMCLK clock, and capture on ACLK for TBCCR6 */
  TBCTL = TBSSEL1 | TBCLR;
  TBCCTL6 = CCIS0 + CM0 + CAP;
  /* start the timer */
  TBCTL |= MC1;
  /* wait for next Capture */
  TBCCTL6 &= ~CCIFG;
  while (!(TBCCTL6 & CCIFG));
  last = TBCCR6;
  while (count++ < SYNC_TICKS_32KHZ) {
    for(i = 0; i < ACLK_TICKS; i++) {
      TBCCTL6 &= ~CCIFG;
      /* wait for next Capture - and calculate difference */
      while (!(TBCCTL6 & CCIFG));
      diff += TBCCR6 - last;
      last = TBCCR6;
    }
    /* resynchronize the DCO speed if not at target */
    if (ACLK_TICKS * DELTA_2 == diff) {
      break; /* if equal, leave "while(...)" */
    } else if (ACLK_TICKS * DELTA_2 < diff) { /* DCO is too fast, slow it down */
      DCOCTL--;
      if (DCOCTL == 0xFF) { /* Did DCO role under? */
        BCSCTL1--;
      }
    } else if (ACLK_TICKS * DELTA_2 > diff) {
      DCOCTL++;
      if (DCOCTL == 0x00) { /* Did DCO role over? */
        BCSCTL1++;
      }
    }
  }
  /* Stop timer - conserves energy according to user guide */
  TBCTL = 0;
//  printf("Last TAR diff: %d target: %ld ", diff, ACLK_TICKS * DELTA_2);
//  printf("CPU Speed: %lu DCOCTL: %d <- %d, count %u\n", diff * 32768 / ACLK_TICKS, DCOCTL,
//      dcoctl_start, count);
}
#endif /* DCOSYNCH_CONF_ENABLED */

/* this code will always start the TimerB if not already started */
void
msp430_sync_dco_log(uint16_t round_number, uint8_t enable_logging) {
  uint16_t last = 0;
  uint16_t diff = 0, diff_abs = 0;
  uint8_t count = 0, ticks = 0;
  uint32_t dcoctl_start = DCOCTL, dcoctl_end = 0;
  /* DELTA_2 assumes an ACLK of 32768 Hz */
#define DELTA_2    ((MSP430_CPU_SPEED) / (32768U))
  /* Repeat sync loop for 3 times (at most) where we measure 2 ticks every time
   * PS: An even number could remove unnecessary adjustments to clk divider */
#define SYNC_TICKS_32KHZ 3U
#define ACLK_TICKS 2U
  /* Select SMCLK clock, and capture on ACLK for TBCCR6 */
  TBCTL = TBSSEL1 | TBCLR;
  TBCCTL6 = CCIS0 + CM0 + CAP;
  /* start the timer */
  TBCTL |= MC1;
  /* wait for next Capture */
  TBCCTL6 &= ~CCIFG;
  while (!(TBCCTL6 & CCIFG));
  last = TBCCR6;
  do {
    diff = 0;
    ticks = 0;
    do {
      TBCCTL6 &= ~CCIFG;
      /* wait for next Capture - and calculate difference */
      while (!(TBCCTL6 & CCIFG));
      diff += TBCCR6 - last;
      last = TBCCR6;
    } while(++ticks < ACLK_TICKS);
    /* resynchronize the DCO speed if not at target */
    if (ACLK_TICKS * DELTA_2 == diff) {
      break; /* if equal, leave "do ... while(...)" */
    } else if (ACLK_TICKS * DELTA_2 < diff) { /* DCO is too fast, slow it down */
      diff_abs = diff - ACLK_TICKS * DELTA_2;
      DCOCTL--;
      if (DCOCTL == 0xFF) { /* Did DCO role under? */
        BCSCTL1--;
      }
    } else if (ACLK_TICKS * DELTA_2 > diff) {
      diff_abs = ACLK_TICKS * DELTA_2 - diff;
      DCOCTL++;
      if (DCOCTL == 0x00) { /* Did DCO role over? */
        BCSCTL1++;
      }
    }
  } while(++count < SYNC_TICKS_32KHZ || diff_abs > DELTA_2/2);
  dcoctl_end = DCOCTL;
  /* Stop timer - conserves energy according to user guide */
  TBCTL = 0;
  if(enable_logging) {
    printf("{rd %u dco} phi %u ctl-d %d c %u\n",
        round_number, diff,
        (int16_t)(dcoctl_end - dcoctl_start), count);
  }
}
#else

void
msp430_sync_dco(void) {

}

void
msp430_sync_dco_log(uint16_t round_number, uint8_t enable_logging) {

}
#endif /* F_CPU!=8000000uL */

/*---------------------------------------------------------------------------*/

/**
 * HW Random number generator.
 *
 * Takes up to 2 * 16 * 5 ACLK ticks
 *
 * Algorithm from TI SLAA338:
 * http://www.ti.com/sc/docs/psheets/abstract/apps/slaa338.htm
 * Uses some code from:
 * https://github.com/0/msp430-rng/blob/master/rand.c
 *
 * @return 16 random bits generated from a hardware source.
 */
unsigned int
msp430_rand(void) {
  int i, j;
  unsigned int result = 0;

  /* Save state */
  unsigned int BCSCTL1_old = BCSCTL1;
  unsigned int DCOCTL_old = DCOCTL;
  unsigned int TACCTL2_old = TACCTL2;
  unsigned int TACTL_old = TACTL;

  /* Set up timer */
  TACTL = 0; //halt
  /* Select SMCLK clock */
  TACTL = TASSEL1 | TACLR;
  TACCTL2 = CCIS0 + CM1 + CAP; /* capture on ACLK positive edge */
  TACCTL2 &= ~CCIE; /* disable interrupt */
  /* start the timer */
  TACTL |= MC1;
  /* wait for next Capture */
  TACCTL2 &= ~CCIFG;

  /* Generate bits */
  for (i = 0; i < 16; i++) {
    unsigned int ones = 0;

    for (j = 0; j < 5; j++) {
      while (!(CCIFG & TACCTL2));       // Wait for interrupt

      TACCTL2 &= ~CCIFG;                // Clear interrupt

      if (1 & TACCR2) {                   // If LSb set, count it
        ones++;
      }
      //change ACLK divider
      BCSCTL1 = ((1 & TACCR2) ^ (result & 1)) ? BCSCTL1 | DIVA0 : BCSCTL1 & ~(DIVA0);
      DCOCTL+=5; //change SMCLK frequency
      if (DCOCTL == 0x00) { /* Did DCO role over? */
        DCOCTL-=20;
      }
    }

    result >>= 1;                         // Save previous bits

    if (ones >= 3) {                        // Best out of 5
      result |= 0x8000;                 // Set MSb
    }
  }
  TACTL = 0; //halt
  TACCTL2 &= ~CCIFG;                // Clear interrupt

  /* Restore state */
  DCOCTL = DCOCTL_old;
  BCSCTL1 = BCSCTL1_old;
  TACCTL2 = TACCTL2_old;
  TACTL = TACTL_old;

  return result;
}
