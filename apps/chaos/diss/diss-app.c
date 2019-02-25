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
 *         A2-Synchrotron disseminate application.
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
#include "diss.h"

static uint32_t diss_value = 0;
static uint16_t round_count_local = 0;
static uint8_t* flags;
static int complete = 0;
static uint16_t off_slot;

static void round_begin(const uint16_t round_count, const uint8_t id);

CHAOS_APP(chaos_diss_app, DISS_SLOT_LEN, DISS_ROUND_MAX_SLOTS, 1, diss_is_pending, round_begin);
#if NETSTACK_CONF_WITH_CHAOS_NODE_DYNAMIC
#include "join.h"
CHAOS_APPS(&join, &chaos_diss_app);
#else
CHAOS_APPS(&chaos_diss_app);
#endif /* NETSTACK_CONF_WITH_CHAOS_NODE_DYNAMIC */

PROCESS(chaos_diss_app_process, "Chaos Diss App Process");
AUTOSTART_PROCESSES(&chaos_diss_app_process);

PROCESS_THREAD(chaos_diss_app_process, ev, data)
{
	PROCESS_BEGIN();
  printf("{boot} Disseminate Test Application\n");

  NETSTACK_MAC.on();
  memset(rx_pkt_crc_err, 0, sizeof(rx_pkt_crc_err));
	while( 1 ){
		PROCESS_YIELD();
		if(chaos_has_node_index){
      printf("{rd %u res} diss: %08lx, fin: %u/%u, node id: %u, n: %u\n", round_count_local, diss_value, complete, off_slot, node_id, chaos_node_count);
//      int latency = complete *
//      printf("{rd %u prf} latency = %f, total slot time = %f\n", complete, off_slot);

      if(complete == 0){
        int i;
        printf("{rd %u fl}", round_count_local);
        for( i=0; i<diss_get_flags_length(); i++ ){
          printf(" %02x", flags[i]);
        }
        printf("\n");
      }

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
		} else {
      printf("{rd %u res} diss: waiting to join, n: %u\n", round_count_local, chaos_node_count);
		}
	}
	PROCESS_END();
}

static void round_begin(const uint16_t round_count, const uint8_t id){
  diss_value = node_id;
  complete = diss_round_begin(round_count, id, &diss_value, &flags);
  off_slot = diss_get_off_slot();
  round_count_local = round_count;
  process_poll(&chaos_diss_app_process);
}
