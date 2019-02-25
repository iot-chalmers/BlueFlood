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
 *         Light sensor driver app.
 * \author
 *         Beshr Al Nahas <beshr@chalmers.se>
 *         Olaf Landsiedel <olafl@chalmers.se>
 *
 */
#include "contiki.h"
#include "dev/light-sensor.h"
#include "dev/light.h"
#include "dev/leds.h"
#include "stdio.h"

/*---------------------------------------------------------------------------*/
PROCESS (lightSensor_process, "Light Sensor");
AUTOSTART_PROCESSES (&lightSensor_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD (lightSensor_process, ev, data)
{
  PROCESS_BEGIN ();
  uint16_t light, lux;

  SENSORS_ACTIVATE (light_sensor);

  while (1)
    {
      static struct etimer et;
      
      etimer_set (&et, CLOCK_SECOND * 2);
      PROCESS_WAIT_EVENT_UNTIL (etimer_expired (&et));

      //Light Sensor

    /*
	 * Most of this information taken from
	 * http://www.moteiv.com/community/Getting_Data_from_Tmote_Sky%27s_Sensors
	 *
	 * The Photosynthetically Active Radiation (PAR) sensor as well as the
	 * Total Solar Radiation (TSR) sensor uses the 2.5V reference voltage
	 * to produce the raw ADC value.

	 * The voltage across each sensor is:
	 *
	 * VsensorPAR = ADCValuePAR/4096 * Vref       (1a)
	 * VsensorTSR = ADCValueTSR/4096 * Vref       (1b)
	 * where Vref = 2.5V
	 *
	 * This voltage creates a current through a resistor R=100KOhm and this
	 * current has a linear relationship with the light intensity in Lux.
	 * IPAR = VsensorPAR / 100,000             (2a) 
	 * ITSR = VsensorTSR / 100,000             (2b) 
	 *
	 * S1087    (PAR) lx = 1e6 * IPAR * 1000   (3a)  
	 * S1087-01 (TSR) lx = 1e5 * ITSR * 1000   (3b)
	 *
	 * lxPAR = 10e9 * ADCValuePAR *(1/4096)* Vref * 10e-5  or
	 * lxPAR = 3125* ADCvaluePAR / 512  
	 * and 
	 * lxTSR = 10e8 * ADCValueTSR *(1/4096)* Vref * 10e-5  or
	 * lxTSR = 625* ADCvalueTSR / 1024  
	*/
	
      light = light_sensor.value (LIGHT_SENSOR_PHOTOSYNTHETIC);
      lux = 3125 * light / 512; // This is for sky platform
      printf ("Light level reading: %d (Lux)\n", lux);

      if (lux <= 65)
        {
          leds_off (LEDS_ALL);
        }

      else if (lux > 65 && lux <= 70)
        {
          leds_on (LEDS_RED);
          leds_off (LEDS_GREEN);
          leds_off (LEDS_YELLOW);
        }

      else if (lux > 70 && lux <= 85)
        {
          leds_on (LEDS_RED);
          leds_on (LEDS_GREEN);
          leds_off (LEDS_YELLOW);
        }

      else
        {
          leds_on (LEDS_ALL);
        }
    }
  SENSORS_DEACTIVATE (light_sensor);
  PROCESS_END ();
}

