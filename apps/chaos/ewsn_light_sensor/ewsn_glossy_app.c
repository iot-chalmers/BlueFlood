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
 *         A2-Synchrotron flooding light sensor application.
 * \author
 *         Beshr Al Nahas <beshr@chalmers.se>
 *         Olaf Landsiedel <olafl@chalmers.se>
 *
 */

#include "contiki.h"
#include <string.h>
#include <stdio.h> /* For PRINTF() */
#include "net/netstack.h"
#include "dev/light-sensor.h"
#include "dev/leds.h"
#include "clock.h"

#include "chaos-control.h"
#include "chaos.h"
#include "chaos-random-generator.h"
#include "node.h"
#include "chaos-config.h"

#include "ewsn_glossy_app.h"

#define ENABLE_COOJA_DEBUG COOJA
#include "dev/cooja-debug.h"


#define FLAGS_LEN 0

//final flood for the flooding policy, for the sink
#define FLOOD_N_TX 2

#ifndef GLOSSY_N_TX
#define GLOSSY_N_TX 3
#define GLOSSY_N_TX_ASSOCIATE 6
#endif

#ifndef CHAOS_RESTART_MIN
#define CHAOS_RESTART_MIN 3
#endif

#ifndef CHAOS_RESTART_MAX
#define CHAOS_RESTART_MAX 6
#endif

static void round_begin(const uint16_t round_count, const uint8_t id);
/* App definition */
CHAOS_APP(chaos_ewsn_glossy_app, EWSN_SLOT_LEN, EWSN_ROUND_MAX_SLOTS, 1, ewsn_is_pending, round_begin);
CHAOS_APPS(&chaos_ewsn_glossy_app);
/*---------------------------------------------------------------------------*/
PROCESS(chaos_ewsn_glossy_app_process, "Chaos EWSN App Process");
AUTOSTART_PROCESSES( &chaos_ewsn_glossy_app_process );
/*---------------------------------------------------------------------------*/
static volatile uint32_t light_on_value = 0;
static uint16_t round_count_local = 0;
static uint8_t sync_slot = 255, num_slots = 255;
static volatile uint8_t complete_app = 255;
static uint8_t* flags;
/*---------------------------------------------------------------------------*/
/* Activate, read and then disable the light sensor.
 * It takes at least ~7ms */
inline static uint32_t
sample_light_sensor(void)
{
#if TESTBED == flocklabewsn
  volatile uint32_t light_sensor_raw = 0;
#else
  uint32_t light_sensor_raw = 0;
#endif
  SENSORS_ACTIVATE( light_sensor );
  BUSYWAIT_UNTIL( light_sensor.status( SENSORS_READY ), RTIMER_SECOND/1024 );
  light_sensor_raw = light_sensor.value( LIGHT_SENSOR_TOTAL_SOLAR );
  SENSORS_DEACTIVATE( light_sensor );
#if TESTBED == flocklabewsn
  flocklab_sensor_simulator_init();
  light_sensor_raw = (flocklab_sensor_simulator_is_set()) ?
     light_sensor_raw + LAMP_ON_LUX_THRESHOLD :
      MIN(light_sensor_raw, LAMP_ON_LUX_THRESHOLD-1);
#endif
  return light_sensor_raw;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(chaos_ewsn_glossy_app_process, ev, data)
{
	PROCESS_BEGIN();
	PRINTF("EWSN Glossy App Process started\n");
	{
	  lamp_init();
	  lamp_on();
	  lamp_off();
	}
	NETSTACK_MAC.on();

	while( 1 ){
		PROCESS_WAIT_EVENT_UNTIL( PROCESS_EVENT_POLL );
		PRINTF("{rd-%u st-%u-%u complete: %u} Light: %lu\n", round_count_local, sync_slot, num_slots, complete_app, light_on_value);
#if EWSN_LEDS_AFTER_ROUND
		if( round_count_local % 4 ){
      leds_on(LEDS_BLUE);
      clock_delay(400);
      leds_off(LEDS_BLUE);
		}
#endif
		sync_slot = 255;
		num_slots = 255;
		complete_app = 255;
		light_on_value = 0;
	}
	PROCESS_END();
}

static void
round_begin(const uint16_t round_count, const uint8_t id)
{
  light_on_value = 0;
  if( IS_INITIATOR() ){
    /* sampling the light sensor takes at least ~7ms */
    if( sample_light_sensor() >= LAMP_ON_LUX_THRESHOLD ){
      light_on_value = 1;
    }
  }
  complete_app = ewsn_round_begin( round_count, id, &light_on_value, &flags );
  round_count_local = round_count;
  process_poll( &chaos_ewsn_glossy_app_process );
}
/*---------------------------------------------------------------------------*/
/* Glossy-like functions */
static int tx_count;
static int one_valid_rx;
static unsigned short restart_threshold;
static int complete = 0;
static volatile uint8_t light_on = 0, light_old = 0;

/* merge and control lamp */
static uint8_t
merge_and_control_lamp( chaos_state_t current_state, int chaos_txrx_success, size_t payload_length, uint8_t* rx_payload, uint8_t* tx_payload )
{
  if( current_state == CHAOS_RX && chaos_txrx_success ){
    light_on |= rx_payload[0];
    tx_payload[0] = light_on;
    if( light_on > 1 && !IS_SINK() ) { //this means the sink has ACKed
      complete = 1;
    } else if( rx_payload[0] > 1 && IS_SINK() ) { //this means the sink ACK went through
      complete = 1;
    }

    /* if sink, switch light status */
    if( IS_SINK() ){
      //if( ( light_old & 1 ) != ( light_on & 1 ) )
      {
        if( light_on & 1 ){
          lamp_on();
        } else {
          lamp_off();
        }
      }
      light_on |= 0x80;
      tx_payload[0] = light_on;
      light_old = light_on;
  #if EWSN_TX_POLICY == EWSN_FLOOD_POLICY
      //don't wait to hear ACK!
      complete = 1;
  #endif
      return 1;
    }
  }
  return 0;
}

#if 0
static chaos_state_t
process_glossy(uint16_t round_count, uint8_t slot_count, chaos_state_t current_state, int chaos_txrx_success, size_t payload_length, uint8_t* rx_payload, uint8_t* tx_payload, uint8_t** app_flags)
{
  merge_and_control_lamp( current_state, chaos_txrx_success, payload_length, rx_payload, tx_payload );

  /* next_state */
  chaos_state_t next_state = CHAOS_RX;
  uint8_t association_helping_round = 0;
  association_helping_round = round_count < (get_sync_round() + ASSOCIATE_ROUND_NUMBER);
	if( IS_INITIATOR() && current_state == CHAOS_INIT ){
			next_state = CHAOS_TX; //for the first tx of the initiator: no increase of tx_count here
  } else if( current_state == CHAOS_RX && chaos_txrx_success ){
    if( !one_valid_rx ){
      one_valid_rx = 1;
      sync_slot = slot_count;
    }
    next_state = CHAOS_TX;
    tx_count++;
  } else if( current_state == CHAOS_TX ){
    next_state = CHAOS_RX;
    if( (tx_count >= GLOSSY_N_TX && (complete || !GLOSSY_WAIT_ACK)) && !association_helping_round ){ /* XXX wait till completion ?? */
      next_state = CHAOS_OFF;
    }
  } else if( IS_INITIATOR() && !one_valid_rx && ((slot_count + 1) % restart_threshold) == 0 ){
    /* restart */
    next_state = CHAOS_TX;
  }

  if( slot_count >= EWSN_ROUND_MAX_APP_SLOTS -1 && !association_helping_round ){
    next_state = CHAOS_OFF;
  }
  num_slots = slot_count;
	return next_state;
}
#endif

static chaos_state_t
process_flood(uint16_t round_count, uint8_t slot_count, chaos_state_t current_state, int chaos_txrx_success, size_t payload_length, uint8_t* rx_payload, uint8_t* tx_payload, uint8_t** app_flags)
{
  merge_and_control_lamp( current_state, chaos_txrx_success, payload_length, rx_payload, tx_payload );

  /* next_state */
  chaos_state_t next_state = CHAOS_RX;
  uint8_t association_helping_round = 0;
  association_helping_round = round_count < (get_sync_round() + ASSOCIATE_ROUND_NUMBER) || (IS_INITIATOR() && !one_valid_rx);
   if( current_state == CHAOS_RX && chaos_txrx_success ){
    if( !one_valid_rx ){
      one_valid_rx = 1;
      sync_slot = slot_count;
    }
  }
  if( IS_INITIATOR() && current_state == CHAOS_INIT ){
     next_state = CHAOS_TX;
  } else if( (one_valid_rx && !complete) || IS_INITIATOR()){
    /* tx with a 50 % probability */
    uint8_t tx = ((uint8_t)chaos_random_generator_produce() % 2);
    if( tx ){
      next_state = CHAOS_TX;
    }
  } else if( complete && tx_count < FLOOD_N_TX ){
    /* final flood */
    next_state = CHAOS_TX;
    tx_count++;
  }

  if( (slot_count >= EWSN_ROUND_MAX_APP_SLOTS -1 || ( complete && tx_count > FLOOD_N_TX )) && !association_helping_round ){
    next_state = CHAOS_OFF;
  }
  num_slots = slot_count;
  return next_state;
}

static chaos_state_t
process(uint16_t round_count, uint8_t slot_count, chaos_state_t current_state, int chaos_txrx_success, size_t payload_length, uint8_t* rx_payload, uint8_t* tx_payload, uint8_t** app_flags)
{
  if(EWSN_TX_POLICY == EWSN_GLOSSY_POLICY){
    return process_glossy(round_count, slot_count, current_state, chaos_txrx_success, payload_length, rx_payload, tx_payload, app_flags);
  } else if(EWSN_TX_POLICY == EWSN_FLOOD_POLICY){
    return process_flood(round_count, slot_count, current_state, chaos_txrx_success, payload_length, rx_payload, tx_payload, app_flags);
  } else {
    //should not reach here!!
    return CHAOS_OFF;
  }
#if EWSN_TX_POLICY != EWSN_FLOOD_POLICY && EWSN_TX_POLICY != EWSN_GLOSSY_POLICY
#error "EWSN_TX_POLICY should be either EWSN_GLOSSY_POLICY or EWSN_FLOOD_POLICY"
#endif /* EWSN_TX_POLICY == EWSN_GLOSSY_POLICY */
}

int ewsn_is_pending(const uint16_t round_count){
  return 1;
}

uint16_t ewsn_get_slot_length_dco(){
  return EWSN_SLOT_LEN_DCO;
}

int ewsn_get_max_slots(){
  return EWSN_ROUND_MAX_SLOTS;
}

int ewsn_get_flags_length() {
  return FLAGS_LEN;
}

int ewsn_get_interval_rtimer(){
  return CHAOS_INTERVAL;
}

int ewsn_round_begin(const uint16_t round_number, const uint8_t app_id, uint32_t* light_on_value, uint8_t** final_flags)
{
  tx_count = 0;
  one_valid_rx = 0;
  light_on = (uint8_t)*light_on_value;
  complete = 0; //no complete status because we are not using flags here

  /* init random restart threshold */
  if( round_number < ASSOCIATE_ROUND_NUMBER && IS_INITIATOR() ){
    restart_threshold = ASSOCIATE_ROUND_RESTART_THRESHOLD; // allow fast retry
  } else {
    restart_threshold = chaos_random_generator_fast() % (CHAOS_RESTART_MAX - CHAOS_RESTART_MIN) + CHAOS_RESTART_MIN;
  }

  chaos_round(round_number, app_id, (const uint8_t const*)&light_on, sizeof(light_on) + ewsn_get_flags_length(), EWSN_SLOT_LEN_DCO, EWSN_ROUND_MAX_SLOTS, ewsn_get_flags_length(), process);
  *light_on_value = light_on;
  *final_flags = 0;
  return complete;
}


