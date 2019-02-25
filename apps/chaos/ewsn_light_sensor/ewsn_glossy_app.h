/*******************************************************************************
 * BSD 3-Clause License
 *
 * Copyright (c) 2017 Beshr Al Nahas and Olaf Landsiedel.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
/**
 * \file
 *         A2-Synchrotron flooding light sensor application.
 * \author
 *         Beshr Al Nahas <beshr@chalmers.se>
 *         Olaf Landsiedel <olafl@chalmers.se>
 *
 */

#ifndef _EWSN__GLOSSY_H_
#define _EWSN__GLOSSY_H_

#include "contiki.h"
#include <legacymsp430.h>

#include "chaos.h"
#include "testbed.h"
#include "chaos-config.h"

/* choose chaos policy */
#define EWSN_FLOOD_POLICY 1
#define EWSN_GLOSSY_POLICY 2

#if COOJA
#define SINK_NODE_ID (15)
#endif

#define IS_SINK()              (node_id == SINK_NODE_ID)

//slot: 2ms + 160us --> could not go under that
//#define EWSN_SLOT_LEN          (4*(RTIMER_SECOND/1000)/2+3)    //1 rtimer tick == 2*31.52 us
#define EWSN_SLOT_LEN          (4*(RTIMER_SECOND/1000)/2-3)    //1 rtimer tick == 2*31.52 us

#define EWSN_SLOT_LEN_DCO      (EWSN_SLOT_LEN*CLOCK_PHI)

/* configure the lamp port */
/* flocklab GPIO from FE glossy */
#define SET_IOPIN(a,b)          do { P##a##OUT |=  BV(b); } while (0)
#define UNSET_IOPIN(a,b)        do { P##a##OUT &= ~BV(b); } while (0)
#define TOGGLE_IOPIN(a,b)       do { P##a##OUT ^=  BV(b); } while (0)
#define INIT_IOPIN_IN(a,b)      do { P##a##SEL &= ~BV(b); P##a##DIR &= ~BV(b); } while (0)
#define INIT_IOPIN_OUT(a,b)     do { P##a##SEL &= ~BV(b); P##a##DIR |=  BV(b); } while (0)
#define IOPIN_IS_SET(a,b)       (    P##a##IN  &   BV(b))

#ifndef TESTBED
#error "TESTBED not defined!!"
#endif

/* lamp control functions */
#if TESTBED == cooja || TESTBED == ewsnmotes
#define lamp_init()
#define lamp_on()            leds_on(LEDS_GREEN)
#define lamp_off()           leds_off(LEDS_GREEN)
#elif TESTBED == flocklabewsn
#define lamp_init()          INIT_IOPIN_OUT(6,0) /* INT1 */
#define lamp_on()            SET_IOPIN(6,0)
#define lamp_off()           UNSET_IOPIN(6,0)
// For simulating the light sensor: UserINT (P2.7) == SIG1
#define flocklab_sensor_simulator_init()      INIT_IOPIN_IN(2,7)
#define flocklab_sensor_simulator_is_set()    IOPIN_IS_SET(2,7)
#else /* the actual pin settings for ewsn */
#define lamp_init()          INIT_IOPIN_OUT(2,3) /* GIO2 (P2.3) */
#define lamp_on()            SET_IOPIN(2,3)
#define lamp_off()           UNSET_IOPIN(2,3)
#endif

int ewsn_round_begin(const uint16_t round_number, const uint8_t app_id, uint32_t* light_on_value, uint8_t** final_flags);
int ewsn_is_pending(const uint16_t round_count);
int ewsn_get_flags_length(void);

/* Lamp settings: sensor threshold for detecting light on */
#define LAMP_ON_LUX_THRESHOLD (100)

#endif /* _EWSN__GLOSSY_H_ */
