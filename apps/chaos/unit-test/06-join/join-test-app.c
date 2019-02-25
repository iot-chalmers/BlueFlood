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
/*
 * rx-test-app.c
 *
 *  Created on: Feb 16, 2015
 *      Author: olaf
 */

#include "contiki.h"
#include <stdio.h> /* For printf() */
#include "net/netstack.h"

#include "chaos-control.h"
#include "slotted-test.h"
#include "join.h"
#include "node.h"

static uint16_t round_count_local = 0;

static void round_begin(const uint16_t round_count, const uint8_t app_id);

CHAOS_APP(slotted_test, SLOTTED_TEST_SLOT_LEN, SLOTTED_TEST_ROUND_MAX_SLOTS, 1, slotted_test_is_pending, round_begin);
CHAOS_APPS(&join, &slotted_test);

PROCESS(chaos_join_test_app_process, "Chaos Join Test App Process");
AUTOSTART_PROCESSES(&chaos_join_test_app_process);

PROCESS_THREAD(chaos_join_test_app_process, ev, data)
{
  PROCESS_BEGIN();
  printf("{boot} Chaos Join Test Application (::slotted)\n");

  NETSTACK_MAC.on();

  while( 1 ){
    PROCESS_YIELD();
    printf("{rd %u commit %d res} joined: %u, idx: %u, n: %u\n", round_count_local, join_get_commit_slot(), chaos_has_node_index, chaos_node_index, chaos_node_count);
  }
  PROCESS_END();
}

static void round_begin(const uint16_t round_count, const uint8_t app_id){
  slotted_test_round_begin(round_count, app_id);
  round_count_local = round_count;
  process_poll(&chaos_join_test_app_process);
}
