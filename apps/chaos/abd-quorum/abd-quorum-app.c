/*******************************************************************************
 * BSD 3-Clause License
 *
 * Copyright (c) 2018 Beshr Al Nahas and Olaf Landsiedel.
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
 *         A2-Synchrotron ABD Quorum application.
 * \author
 *          
 *         Beshr Al Nahas <beshr@chalmers.se>
 *         Konstantinos Peratinos <konper@student.chalmers.se>
 *
 */

#include "contiki.h"
#include <stdio.h> /* For printf() */
#include "net/netstack.h"

#include "chaos-control.h"
#include "node.h"
#include "mw-quorum.h"

static uint16_t value = 0;
static uint16_t tag = 0;
static uint8_t operation = 1;
static uint16_t writer_id = 0;
static uint16_t round_count_local = 0;
static uint8_t* flags;
static uint16_t complete = 0;
static uint16_t off_slot;

static void round_begin(const uint16_t round_count, const uint8_t id);

CHAOS_APP(chaos_quorum, MAX_SLOT_LEN, MAX_ROUND_MAX_SLOTS, 1, abd_quorum_is_pending, round_begin);
#if NETSTACK_CONF_WITH_CHAOS_NODE_DYNAMIC
#include "join.h"
CHAOS_APPS(&join, &chaos_quorum);
#else
CHAOS_APPS(&chaos_quorum);
#endif /* NETSTACK_CONF_WITH_CHAOS_NODE_DYNAMIC */

PROCESS(abd_quorum_process, "ABD Quorum Process");
AUTOSTART_PROCESSES(&abd_quorum_process);

PROCESS_THREAD(abd_quorum_process, ev, data)
{
	PROCESS_BEGIN();
  printf("{boot} ABD Quorum Application\n");

  NETSTACK_MAC.on();
#if FAULTY_NODE_ID
  memset(rx_pkt_crc_err, 0, sizeof(rx_pkt_crc_err));
#endif
	while( 1 ){
		PROCESS_YIELD();
		if(chaos_has_node_index){
        if ((IS_INITIATOR() || node_id == chaos_node_count) && (round_count_local % 2 == 1)) {
          printf("{rd %u res} written: %u, ts: %u, fin: %i/%i, n: %u\n",
           round_count_local, value, tag, complete, off_slot, chaos_node_count);
        }
        else { 
          printf("{rd %u res} read: %u, ts: %u, fin: %i/%i, writer id: %u, n: %u\n",
           round_count_local, value, tag, complete, off_slot, writer_id
           , chaos_node_count);
        }
      if(complete == 0){
        int i;
        printf("{rd %u fl}", round_count_local);
        for( i=0; i<abd_quorum_get_flags_length(); i++ ){
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
      printf("{rd %u res} Quorum: waiting to join, n: %u\n", round_count_local, chaos_node_count);
		}
	}
	PROCESS_END();
}

static void round_begin(const uint16_t round_count, const uint8_t id){
  // Always Set to Read -- Only change for writers
  // 1 ==  Read --- 0 == Write
  operation = 1;

  // Nodes > 2  --- %2 is a writing round
  if((IS_INITIATOR() || node_id == chaos_node_count) && (round_count_local % 2 == 0)) {
    operation = 0;
   	tag = tag+1;
   	writer_id = node_id;
   	value = node_id*2 + tag;	
  }
  complete = abd_quorum_round_begin(round_count, id, &value, &tag, operation, &writer_id, &flags);
  off_slot = abd_quorum_get_off_slot();
  round_count_local = round_count;
  //printf("wid %u\n", writer_id);
  //id = node_id;

  process_poll(&abd_quorum_process);
}