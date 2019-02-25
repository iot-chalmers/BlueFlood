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
 *         A2-Synchrotron project config.
 * \author
 *         Beshr Al Nahas <beshr@chalmers.se>
 *         Olaf Landsiedel <olafl@chalmers.se>
 *
 */
#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_
#warning "using project-conf.h"

//remember to disable logs
#undef WITH_CHAOS_LOG
#define WITH_CHAOS_LOG 1
//don't disable PRINTF if logs are enabled ;)
#undef CHAOS_DEBUG_PRINTF
#define CHAOS_DEBUG_PRINTF 1

#if WITH_CHAOS_LOG && !CHAOS_DEBUG_PRINTF
#error "Fix me: PRINTF disabled and CHAOS_LOG enabled!"
#endif

#undef CHAOS_USE_SRC_ID
#define CHAOS_USE_SRC_ID 1

#undef CHAOS_USE_DST_ID
#define CHAOS_USE_DST_ID 1

#undef CHAOS_USE_SRC_RANK
#define CHAOS_USE_SRC_RANK 1

/* Chaos timing */
#undef CHAOS_INTERVAL
#define CHAOS_INTERVAL   (19*(RTIMER_SECOND/10))

/* how many failed rounds to allow before associating again? */
#undef CHAOS_FAILED_ROUNDS_RESYNC_THRESHOLD
#define CHAOS_FAILED_ROUNDS_RESYNC_THRESHOLD (1)

//lower tick rate
//#undef CLOCK_CONF_SECOND
//#define CLOCK_CONF_SECOND 1UL

#undef RTIMER_CONF_SECOND
#define RTIMER_CONF_SECOND (4096U*8)

//#define SKY_DISABLE_WDT 1

//chaos hack for maximum app payload!!
#define EWSN_COMPETITION (0)
#define EWSN_COMPETITION_MAX_PAYLOAD_SIZE (1)

/* no need to touch these */

#undef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM                0

#undef CC2420_CONF_AUTOACK
#define CC2420_CONF_AUTOACK              0

#undef DCOSYNCH_CONF_ENABLED
#define DCOSYNCH_CONF_ENABLED            0

/* we have to disable DMA in order to save power and enable LPM3 for the CPU */
#undef UART1_CONF_RX_WITH_DMA
#define UART1_CONF_RX_WITH_DMA           0

#undef UART1_CONF_TX_WITH_DMA
#define UART1_CONF_TX_WITH_DMA           0

//remove processes names to avoid printing them
#undef PROCESS_CONF_NO_PROCESS_NAMES
#define PROCESS_CONF_NO_PROCESS_NAMES 0

#undef LOG_CONF_ENABLED
#define LOG_CONF_ENABLED 0

#undef ENERGEST_CONF_ON
#define ENERGEST_CONF_ON 0


#endif /* PROJECT_CONF_H_ */
