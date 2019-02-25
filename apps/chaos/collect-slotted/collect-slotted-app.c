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
 *         A2-Synchrotron collect-slotted application.
 * \author
 *         Beshr Al Nahas <beshr@chalmers.se>
 *         Olaf Landsiedel <olafl@chalmers.se>
 *
 */

#include "contiki.h"
#include <stdio.h> /* For printf() */

#include "collect-slotted.h"
#include "net/netstack.h"

#include "chaos-control.h"
#include "node.h"

//static int8_t first_rx_hop_count = 0;
//static uint16_t avg_first_rx_hop_count100 = 0;
//static const uint16_t ranks_cut_off[] = {0, 100, 150, 200, 250, 300, 350, 400};
//static uint8_t rank = 0xFF;

//static int complete = 0;
static uint16_t round_count_local;

static void round_begin(const uint16_t round_count, const uint8_t id);

CHAOS_APP(chaos_collect_slotted_app, COLLECT_SLOTTED_SLOT_LEN, COLLECT_SLOTTED_ROUND_MAX_SLOTS, 1, collect_slotted_is_pending, round_begin);
CHAOS_APPS(&chaos_collect_slotted_app);

PROCESS(chaos_collect_slotted_app_process, "Chaos Collect-Slotted App Process");
AUTOSTART_PROCESSES(&chaos_collect_slotted_app_process);

PROCESS_THREAD(chaos_collect_slotted_app_process, ev, data)
{
	PROCESS_BEGIN();
  printf("{boot} Collect Slotted Application\n");


  NETSTACK_MAC.on();

	while( 1 ){
		PROCESS_YIELD();
    //rank stuff
//    if(first_rx_hop_count >= 0){
//      avg_first_rx_hop_count100 = (avg_first_rx_hop_count100 * 8) / 10  + (first_rx_hop_count * 100 * 2 / 10);
//      int i = 0;
//      while( avg_first_rx_hop_count100 > ranks_cut_off[i] && i < sizeof(ranks_cut_off) / sizeof(ranks_cut_off[0])){
//        i++;
//      }
//      rank = i;
//    }
//    printf("{rd %u rank} frx: %i, avgfrx: %u.%02u, rank: %u, fin: %i, n: %u\n", round_count_local, first_rx_hop_count, avg_first_rx_hop_count100/100, avg_first_rx_hop_count100%100, rank, complete, chaos_node_count);


	}
	PROCESS_END();
}

static void round_begin(const uint16_t round_count, const uint8_t id){
  round_count_local = round_count;
  collect_slotted_round_begin(round_count, id);
  process_poll(&chaos_collect_slotted_app_process);
}
