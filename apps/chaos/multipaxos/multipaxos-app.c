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
 *         A2-Synchrotron MultiPaxos application.
 * \author
*         Valentin Poirot <poirotv@chalmers.se>
 *         Beshr Al Nahas <beshr@chalmers.se>
 *         Olaf Landsiedel <olafl@chalmers.se>
 *
 */

#include "contiki.h"
#include <stdio.h> /* For printf() */
#include "net/netstack.h"

#include "chaos-control.h"
#include "node.h"
#include "multipaxos.h"

static multipaxos_value_t paxos_value = 0;
static uint8_t is_proposer = 0;
static uint16_t round_count_local = 0;
static uint8_t* flags;
static uint16_t complete = 0;
static uint16_t off_slot;

static void round_begin(const uint16_t round_count, const uint8_t id);

CHAOS_APP(chaos_multipaxos_app, MULTIPAXOS_SLOT_LEN, MULTIPAXOS_ROUND_MAX_SLOTS, 1, multipaxos_is_pending, round_begin);
#if NETSTACK_CONF_WITH_CHAOS_NODE_DYNAMIC
#include "join.h"
CHAOS_APPS(&join, &chaos_multipaxos_app);
#else
CHAOS_APPS(&chaos_multipaxos_app);
#endif /* NETSTACK_CONF_WITH_CHAOS_NODE_DYNAMIC */

PROCESS(chaos_multipaxos_app_process, "Chaos MultiPaxos App Process");
AUTOSTART_PROCESSES(&chaos_multipaxos_app_process);

PROCESS_THREAD(chaos_multipaxos_app_process, ev, data)
{
	PROCESS_BEGIN();
  printf("{boot} MultiPaxos Test Application\n");

  NETSTACK_MAC.on();

#if FAULTY_NODE_ID
  memset(rx_pkt_crc_err, 0, sizeof(rx_pkt_crc_err));
#endif
	while( 1 ){
		PROCESS_YIELD();
		if(chaos_has_node_index){
		  multipaxos_t* state = multipaxos_get_local();
      multipaxos_state_t* pstate = paxos_get_state();
      printf("{rd %u res} paxos %u: r %u, b %x p %x v %u f %u, fin: %i/%i, node id: %u, n: %u\n", round_count_local, (*pstate).leader.is_leader, pstate->acceptor.last_round_participation, state->ballot.n, state->proposals[0].n, state->values[0], state->phase, complete, off_slot, node_id, chaos_node_count);
      //printf("{rd %u state} paxos %u: mp %x %x ap %x %x pb %x av %u %u pv %u ph %u m %u\n", round_count_local, is_proposer, pstate->acceptor.min_proposal.n, pstate->rx_min_proposal.n, pstate->acceptor.accepted_proposal.n, pstate->rx_accepted_proposal.n, pstate->proposer.proposed_ballot.n, pstate->acceptor.accepted_value, pstate->rx_accepted_value, pstate->proposer.proposed_value, pstate->proposer.phase, pstate->proposer.got_majority_at_slot);
      if (is_proposer) {
        //printf("{rd %u state} paxos: A(mp %x, ap %x, av %u) mem(mp %x, ap %x, av %u) P(pb %x, pv %u, ph %u, Pmaj %u, Amaj %u)\n", round_count_local, pstate->acceptor.min_proposal.n, pstate->acceptor.accepted_proposals[0].n, pstate->acceptor.accepted_values[0], pstate->rx_min_proposal.n, pstate->rx_accepted_proposals[0].n, pstate->rx_accepted_value, pstate->proposer.proposed_ballot.n, pstate->proposer.proposed_value, pstate->proposer.phase, pstate->proposer.updated_phase_at, pstate->proposer.got_majority_at_slot);
      } else {
        //printf("{rd %u state} paxos: A(mp %x, ap %x, av %u) mem(mp %x, ap %x, av %u)\n", round_count_local, pstate->acceptor.min_proposal.n, pstate->acceptor.accepted_proposal.n, pstate->acceptor.accepted_value, pstate->rx_min_proposal.n, pstate->rx_accepted_proposal.n, pstate->rx_accepted_value);
      }


      // Plots debug
      #if MULTIPAXOS_LOG_FLAGS
      int i;
      printf("{rd %u fl} ", round_count_local);
      for( i=0; i<off_slot; i++ ){
        printf("%u,", chaos_multipaxos_flags_log[i]);
      }
      printf("\n");
      printf("{rd %u val} ", round_count_local);
      for( i=0; i<MULTIPAXOS_LOG_SIZE; i++ ){
        printf("%u,", chaos_multipaxos_value_log[i]);
      }
      printf("\n");
      printf("{rd %u minP} ", round_count_local);
      for( i=0; i<off_slot; i++ ){
        //commit_field_t cc = chaos_join_commit_log[i];
        //printf("%u", cc.slot_count, cc.overflow, cc.commit);
        printf("%u,", chaos_multipaxos_min_proposal_log[i]);
      }
      printf("\n");
      printf("{rd %u acP} ", round_count_local);
      for( i=0; i<off_slot; i++ ){
        //commit_field_t cc = chaos_join_commit_log[i];
        //printf("%u", cc.slot_count, cc.overflow, cc.commit);
        printf("%u,", chaos_multipaxos_accepted_proposal_log[i]);
      }
      printf("\n");
      #endif

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
  if ((chaos_node_index == 1/* || chaos_node_index == 2*/) && round_count <= 12) {
	  is_proposer = 1;
  }

  complete = multipaxos_round_begin(round_count, id, is_proposer, &paxos_value, &flags);
  off_slot = multipaxos_get_off_slot();
  round_count_local = round_count;
  process_poll(&chaos_multipaxos_app_process);
}

/* If this node haven't heard of the current leader for BECOME_LEADER_AFTER, this function decides if the node proposes itself as the new leader */
uint8_t app_multipaxos_propose_as_leader(multipaxos_state_t *multipaxos_state) {
  return (chaos_random_generator_fast() % (chaos_node_count / 4) == 0);
}

/* Logic called by the Paxos Primitive if this node proposed itself as the new leader, called every time it proposes itself */
void app_multipaxos_first_time_leader(multipaxos_state_t *multipaxos_state) {
  uint8_t i;
    for (i=0;i<MULTIPAXOS_PKT_SIZE;++i)
      multipaxos_state->leader.proposed_values[i] = multipaxos_state->acceptor.accepted_values[(multipaxos_state->acceptor.last_round_participation-MULTIPAXOS_PKT_SIZE+1+i)%MULTIPAXOS_LOG_SIZE] + 1;
}

/* Logic called by the Paxos Primitive if the previous round was successful */
void app_multipaxos_leader_logic(multipaxos_state_t *multipaxos_state) {
    uint8_t i;
    for (i=0;i<MULTIPAXOS_PKT_SIZE;++i)
      multipaxos_state->leader.proposed_values[i] = multipaxos_state->acceptor.accepted_values[(multipaxos_state->acceptor.last_round_participation-MULTIPAXOS_PKT_SIZE+1+i)%MULTIPAXOS_LOG_SIZE] + 1;
}