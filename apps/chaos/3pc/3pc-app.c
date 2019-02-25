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
 *         A2-Synchrotron three-phase commit application.
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
#include "3pc.h"

#define AGREEMENT_WORD (0xBAC0FFEEUL)

static uint32_t three_pc_value = 0;
static uint32_t three_pc_agreement_value = 0;
static uint16_t round_count_local = 0;
static uint8_t* flags;
static uint8_t phase = 0;
static uint16_t complete = 0;
static uint16_t off_slot;

static void round_begin(const uint16_t round_count, const uint8_t id);

CHAOS_APP(chaos_3pc_app, THREE_PC_SLOT_LEN, THREE_PC_ROUND_MAX_SLOTS, 1, three_pc_is_pending, round_begin);
#if NETSTACK_CONF_WITH_CHAOS_NODE_DYNAMIC
#include "join.h"
CHAOS_APPS(&join, &chaos_3pc_app);
#else
CHAOS_APPS(&chaos_3pc_app);
#endif /* NETSTACK_CONF_WITH_CHAOS_NODE_DYNAMIC */

PROCESS(chaos_3pc_app_process, "Chaos 3PC App Process");
AUTOSTART_PROCESSES(&chaos_3pc_app_process);

PROCESS_THREAD(chaos_3pc_app_process, ev, data)
{
  PROCESS_BEGIN();
  three_pc_agreement_value = AGREEMENT_WORD;
  printf("{boot} 3PC Test Application\n");

  NETSTACK_MAC.on();

  while( 1 ){
    PROCESS_YIELD();

    if(chaos_has_node_index){
      /* 3PC decision making - helps recovery */
      static int final_phase = PHASE_UNKNOWN;

      if(phase == PHASE_PROPOSE){
        final_phase = PHASE_ABORT_TIMEOUT_APP;
//        if(IS_INITIATOR()){
//          /* we do not agree so we safely abort on timeout */
//          final_phase = PHASE_ABORT_TIMEOUT;
//        } else {
////          if(three_pc_agreed() && three_pc_did_tx()){
////            /* we agree but we timeout */
////            final_phase = PHASE_ABORT_TIMEOUT; //PHASE_UNCERTAIN;
////          } else if(!three_pc_agreed() || !three_pc_did_tx()){
////            /* we do not agree so we safely abort on timeout */
////            final_phase = PHASE_ABORT_TIMEOUT;
////          }
//          final_phase = PHASE_ABORT_TIMEOUT;
//        }
      } else if(phase == PHASE_PRE_COMMIT) {
        if(IS_INITIATOR() /* && !three_pc_did_tx() */){
          //we can detect node failures with this
          int node_failed = off_slot < THREE_PC_ROUND_MAX_SLOTS -1;
          if(node_failed){
            //XXX we could detect initiator failure and take a decision!
            final_phase = PHASE_COMMIT_TIMEOUT;
          } else {
            final_phase = PHASE_ABORT_TIMEOUT_APP;
          }
          //final_phase = PHASE_ABORT_TIMEOUT_APP;
        } else {
          final_phase = PHASE_COMMIT_TIMEOUT;
        }
      } else if(phase == PHASE_COMMIT) {
          final_phase = PHASE_COMMIT;
      } else if(phase == PHASE_ABORT) {
        final_phase = PHASE_ABORT;
      } else if(phase == PHASE_ABORT_TIMEOUT) {
        final_phase = PHASE_ABORT_TIMEOUT;
      }

      if(!three_pc_agreed() && (phase == PHASE_COMMIT || phase == PHASE_COMMIT_TIMEOUT)){
        final_phase = PHASE_FAIL;
      }

      printf("{rd %u res} 3pc: %08lx, fin: %i/%i, ph: %i, node id: %u, n: %u\n", round_count_local, three_pc_value, complete, off_slot, phase, node_id, chaos_node_count);
      printf("{rd %u phase} %s, fin: %u\n", round_count_local, PHASE_TO_STR(final_phase), complete);

#if THREE_PC_LOG_FLAGS
      int i;
      printf("{rd %u fl}", round_count_local);
      for( i=0; i<three_pc_get_flags_length(); i++ ){
        printf(" %02x", flags[i]);
      }
      printf("\n");
      printf("{rd %u votes}", round_count_local);
      flags += three_pc_get_flags_length();
      for( i=0; i<three_pc_get_flags_length(); i++ ){
        printf(" %02x", flags[i]);
      }
      printf("\n");
      printf("{rd %u phase} ", round_count_local);
      for( i=0; i<off_slot; i++ ){
        //commit_field_t cc = chaos_join_commit_log[i];
        //printf("%u", cc.slot_count, cc.overflow, cc.commit);
        printf("%u,", chaos_3pc_phase_log[i]);
      }
      printf("\n{rd %u flags} ", round_count_local);
      for( i=0; i<off_slot; i++ ){
        //commit_field_t cc = chaos_join_commit_log[i];
        //printf("%u", cc.slot_count, cc.overflow, cc.commit);
        printf("%u,", chaos_3pc_flags_log[i]);
      }
      printf("\n");
#endif
    } else {
      printf("{rd %u res} 3pc: waiting to join, n: %u\n", round_count_local, chaos_node_count);
    }
  }
  PROCESS_END();
}

static void round_begin(const uint16_t round_count, const uint8_t id){
  three_pc_value = three_pc_agreement_value;
  complete = three_pc_round_begin(round_count, id, &three_pc_value, &phase, &flags);
  off_slot = three_pc_get_off_slot();
  round_count_local = round_count;
  process_poll(&chaos_3pc_app_process);
}
