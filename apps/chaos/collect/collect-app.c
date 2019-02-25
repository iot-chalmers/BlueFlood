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
 *         A2-Synchrotron collect application.
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
#include "chaos-collect.h"

static collect_value_t collect_value = 0;
static collect_value_t* collect_value_store = NULL;
static uint8_t* flags = NULL;
static uint16_t round_count_local = 0;
static uint16_t complete = 0;
static uint16_t off_slot;

static void round_begin(const uint16_t round_count, const uint8_t id);

CHAOS_APP(chaos_collect_app, CHAOS_COLLECT_SLOT_LEN, CHAOS_COLLECT_ROUND_MAX_SLOTS, 1, chaos_collect_is_pending, round_begin);
#if NETSTACK_CONF_WITH_CHAOS_NODE_DYNAMIC
#include "join.h"
CHAOS_APPS(&join, &chaos_collect_app);
#else
CHAOS_APPS(&chaos_collect_app);
#endif /* NETSTACK_CONF_WITH_CHAOS_NODE_DYNAMIC */

PROCESS(chaos_collect_app_process, "Chaos Collect App Process");
AUTOSTART_PROCESSES(&chaos_collect_app_process);

PROCESS_THREAD(chaos_collect_app_process, ev, data)
{
  PROCESS_BEGIN();
  printf("{boot} Collect Test Application\n");

  NETSTACK_MAC.on();

  while( 1 ){
    PROCESS_YIELD();
    if(chaos_has_node_index){
      uint16_t total_collected = 0;
      int i, j;
      for( i=0; i<chaos_collect_get_flags_length(); i++ ){
        for( j=0; j<8; j++){
          total_collected += ( (flags[i]) & (1u<<j) ) != 0;
        }
      }

      printf("{rd %u res} collect: %u, fin: %i/%i, node id: %u, n: %u\n", round_count_local, total_collected, complete, off_slot, node_id, chaos_node_count);

      if(complete == 0){
        printf("{rd %u fl}", round_count_local);
        for( i=0; i<chaos_collect_get_flags_length(); i++ ){
          printf(" %02x", flags[i]);
        }
        printf("\n");
      }
  //    printf("{rd %u values}", round_count_local);
  //    for( i = 0; i < chaos_node_count; i++ ){
  //      printf(" %lu", (uint32_t)collect_value_store[i]);
  //    }
  //    printf("\n");
    } else {
      printf("{rd %u res} collect: waiting to join, n: %u\n", round_count_local, chaos_node_count);
    }
  }
  PROCESS_END();
}

static void round_begin(const uint16_t round_count, const uint8_t id) {
  collect_value = chaos_node_index;
  collect_value_store = &collect_value;
  complete = chaos_collect_round_begin(round_count, id, &collect_value_store, &flags);
  off_slot = chaos_collect_get_off_slot();
  round_count_local = round_count;
  process_poll(&chaos_collect_app_process);
}
