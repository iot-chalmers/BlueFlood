/*******************************************************************************
 * BSD 3-Clause License
 *
 * Copyright (c) 2017 Beshr Al Nahas, Valentin Poirot and Olaf Landsiedel.
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
 *         A2-Synchrotron Paxos application.
 * \author
 *         Beshr Al Nahas <beshr@chalmers.se>
 *         Valentin Poirot <poirotv@chalmers.se>
 *         Olaf Landsiedel <olafl@chalmers.se>
 *
 */

#include "contiki.h"
#include <stdio.h> /* For printf() */
#include "net/netstack.h"

#include "chaos-control.h"
#include "node.h"
#include "paxos.h"

static paxos_value_t paxos_value = 0;
static uint8_t is_proposer = 0;
static uint16_t round_count_local = 0;
static uint8_t* flags;
static uint8_t phase = 0;
static uint16_t complete = 0;
static uint16_t off_slot;

static void round_begin(const uint16_t round_count, const uint8_t id);

CHAOS_APP(chaos_paxos_app, PAXOS_SLOT_LEN, PAXOS_ROUND_MAX_SLOTS, 1, paxos_is_pending, round_begin);
#if NETSTACK_CONF_WITH_CHAOS_NODE_DYNAMIC
#include "join.h"
CHAOS_APPS(&join, &chaos_paxos_app);
#else
CHAOS_APPS(&chaos_paxos_app);
#endif /* NETSTACK_CONF_WITH_CHAOS_NODE_DYNAMIC */

PROCESS(chaos_paxos_app_process, "Chaos Paxos App Process");
AUTOSTART_PROCESSES(&chaos_paxos_app_process);

PROCESS_THREAD(chaos_paxos_app_process, ev, data)
{
	PROCESS_BEGIN();
  printf("{boot} Paxos Test Application\n");

  NETSTACK_MAC.on();
#if FAULTY_NODE_ID
  memset(rx_pkt_crc_err, 0, sizeof(rx_pkt_crc_err));
#endif
	while( 1 ){
		PROCESS_YIELD();
		if(chaos_has_node_index){
		  paxos_t* state = paxos_get_local();
      //printf("{rd %u res} paxos %u: b %x p %x v %u f %u, fin: %i/%i, node id: %u, n: %u\n", round_count_local, is_proposer, state->ballot.n, state->proposal.n, state->value, state->phase, complete, off_slot, node_id, chaos_node_count);
      paxos_state_t* pstate = paxos_get_state();
      //printf("{rd %u state} paxos %u: mp %x %x ap %x %x pb %x av %u %u pv %u ph %u m %u\n", round_count_local, is_proposer, pstate->acceptor.min_proposal.n, pstate->rx_min_proposal.n, pstate->acceptor.accepted_proposal.n, pstate->rx_accepted_proposal.n, pstate->proposer.proposed_ballot.n, pstate->acceptor.accepted_value, pstate->rx_accepted_value, pstate->proposer.proposed_value, pstate->proposer.phase, pstate->proposer.got_majority_at_slot);
      if (is_proposer) {
        printf("{rd %u state} paxos: A(mp %x, ap %x, av %u) mem(mp %x, ap %x, av %u) P(pb %x, pv %u, ph %u, Pmaj %u, Amaj %u)\n", round_count_local, pstate->acceptor.min_proposal.n, pstate->acceptor.accepted_proposal.n, pstate->acceptor.accepted_value, pstate->rx_min_proposal.n, pstate->rx_accepted_proposal.n, pstate->rx_accepted_value, pstate->proposer.proposed_ballot.n, pstate->proposer.proposed_value, pstate->proposer.phase, pstate->proposer.updated_phase_at, pstate->proposer.got_majority_at_slot);
      } else {
        printf("{rd %u state} paxos: A(mp %x, ap %x, av %u) mem(mp %x, ap %x, av %u)\n", round_count_local, pstate->acceptor.min_proposal.n, pstate->acceptor.accepted_proposal.n, pstate->acceptor.accepted_value, pstate->rx_min_proposal.n, pstate->rx_accepted_proposal.n, pstate->rx_accepted_value);
      }


      // Plots debug
      int i;

      /*printf("{rd %u phase} ", round_count_local);
      for( i=0; i<off_slot; i++ ){
        //commit_field_t cc = chaos_join_commit_log[i];
        //printf("%u", cc.slot_count, cc.overflow, cc.commit);
        printf("%u,", chaos_paxos_phase_log[i]);
      }
      printf("\n");*/


      printf("{rd %u fl} ", round_count_local);
      for( i=0; i<off_slot; i++ ){
        printf("%u,", chaos_paxos_flags_log[i]);
      }
      printf("\n");


      printf("{rd %u val} ", round_count_local);
      for( i=0; i<off_slot; i++ ){
        printf("%u,", chaos_paxos_value_log[i]);
      }
      printf("\n");


      printf("{rd %u minP} ", round_count_local);
      for( i=0; i<off_slot; i++ ){
        printf("%u,", chaos_paxos_min_proposal_log[i]);
      }
      printf("\n");


      printf("{rd %u acP} ", round_count_local);
      for( i=0; i<off_slot; i++ ){
        printf("%u,", chaos_paxos_accepted_proposal_log[i]);
      }
      printf("\n");

      printf("{rd %u chV} ", round_count_local);
      for( i=0; i<off_slot; i++ ){
        printf("%u,", chaos_paxos_learned_value_log[i]);
      }
      printf("\n");     

      printf("{rd %u sch} ", round_count_local);
      for( i=0; i<off_slot; i++ ){
        printf("%u,", chaos_paxos_schedule[i]);
      }
      printf("\n");  


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
      printf("{rd %u res} paxos: waiting to join, n: %u\n", round_count_local, chaos_node_count);
		}
	}
	PROCESS_END();
}

static void round_begin(const uint16_t round_count, const uint8_t id){
  paxos_value = chaos_node_index + 1;
  // We add one proposer every ten round
  if (IS_INITIATOR() /*|| (chaos_node_index > 0 && chaos_node_index < (round_count/256) )*/ ) {
	  is_proposer = 1;
  }
  //round_count_prop += 1;

  complete = paxos_round_begin(round_count, id, is_proposer, &paxos_value, &phase, &flags);
  off_slot = paxos_get_off_slot();
  round_count_local = round_count;
  process_poll(&chaos_paxos_app_process);
}
