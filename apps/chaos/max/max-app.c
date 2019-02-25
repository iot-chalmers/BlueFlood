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
 *         A2-Synchrotron Max application.
 * \author
 *         Beshr Al Nahas <beshr@chalmers.se>
 *         Olaf Landsiedel <olafl@chalmers.se>
 *
 */

#include "contiki.h"
#include <stdio.h> /* For printf() */
#include "net/netstack.h"

#include "chaos-control.h"
#include "node.h"
#include "max.h"

static uint16_t max_value = 0;
static uint16_t round_count_local = 0;
static uint8_t* flags;
static uint16_t complete = 0;
static uint16_t off_slot;

static void round_begin(const uint16_t round_count, const uint8_t id);

CHAOS_APP(chaos_max_app, MAX_SLOT_LEN, MAX_ROUND_MAX_SLOTS, 1, max_is_pending, round_begin);
#if NETSTACK_CONF_WITH_CHAOS_NODE_DYNAMIC
#include "join.h"
CHAOS_APPS(&join, &chaos_max_app);
#else
CHAOS_APPS(&chaos_max_app);
#endif /* NETSTACK_CONF_WITH_CHAOS_NODE_DYNAMIC */

PROCESS(chaos_max_app_process, "Chaos Max App Process");
AUTOSTART_PROCESSES(&chaos_max_app_process);

PROCESS_THREAD(chaos_max_app_process, ev, data)
{
	PROCESS_BEGIN();
  printf("{boot} Max Test Application\n");

  NETSTACK_MAC.on();
#if FAULTY_NODE_ID
  memset(rx_pkt_crc_err, 0, sizeof(rx_pkt_crc_err));
#endif
	while( 1 ){
		PROCESS_YIELD();
		if(chaos_has_node_index){
      printf("{rd %u res} max: %u, fin: %i/%i, node id: %u, n: %u\n", round_count_local, max_value, complete, off_slot, node_id, chaos_node_count);
//      int latency = complete *
//      printf("{rd %u prf} latency = %f, total slot time = %f\n", complete, off_slot);

      if(complete == 0){
        int i;
        printf("{rd %u fl}", round_count_local);
        for( i=0; i<max_get_flags_length(); i++ ){
          printf(" %02x", flags[i]);
        }
        printf("\n");
      }

#if FAULTY_NODE_ID
      //print the faulty CRC but passing packet (discovered by MIC check)
      if(rx_pkt_crc_err[sizeof(rx_pkt_crc_err)-1]){
        int i;
        printf("{rd %u crc}", round_count_local);
        for( i=0; i<sizeof(rx_pkt_crc_err); i++ ){
          printf(" %02x", rx_pkt_crc_err[i]);
        }
        printf("\n");
        memset(rx_pkt_crc_err, 0, sizeof(rx_pkt_crc_err));
      }
#endif
		} else {
      printf("{rd %u res} max: waiting to join, n: %u\n", round_count_local, chaos_node_count);
		}
	}
	PROCESS_END();
}

static void round_begin(const uint16_t round_count, const uint8_t id){
  max_value = node_id;
  complete = max_round_begin(round_count, id, &max_value, &flags);
  off_slot = max_get_off_slot();
  round_count_local = round_count;
  process_poll(&chaos_max_app_process);
}
