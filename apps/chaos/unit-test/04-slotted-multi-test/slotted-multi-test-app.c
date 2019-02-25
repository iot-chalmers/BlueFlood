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
 * slotted-multi-test-app.c
 *
 *  Created on: Feb 16, 2015
 *      Author: olaf
 */

#include "contiki.h"
#include <stdio.h> /* For printf() */
#include "net/netstack.h"

#include "chaos-control.h"
#include "slotted-multi-test.h"

PROCESS(chaos_slotted_multi_test_app_process, "Chaos Slotted Multi Test App Process");
AUTOSTART_PROCESSES(&chaos_slotted_multi_test_app_process);

static void round_begin(const uint16_t round_count, const uint8_t id);

CHAOS_APP(slotted_multi_test, SLOTTED_MULTI_TEST_SLOT_LEN, SLOTTED_MULTI_TEST_ROUND_MAX_SLOTS, 1, slotted_multi_test_is_pending, round_begin);
CHAOS_APPS(&slotted_multi_test);

static void round_begin(const uint16_t round_count, const uint8_t id){
  slotted_multi_test_round_begin(round_count, id);
  process_poll(&chaos_slotted_multi_test_app_process);
}

PROCESS_THREAD(chaos_slotted_multi_test_app_process, ev, data)
{
	PROCESS_BEGIN();
	printf("Chaos Slotted Multi Test App Process started\n");

	NETSTACK_MAC.on();

	while( 1 ){
		PROCESS_YIELD();
		printf("Chaos round complete!\n");
	}
	PROCESS_END();
}
