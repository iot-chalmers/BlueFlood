/**
 * \addtogroup nrf51822
 * @{
 * \defgroup nrf51-contiki-rtimer Contiki Rtimer implementation for nrf51822 SoC
 * 			This module contains the Rtimer implementation for nrf51822 SoC using
 * 			TIMER1 peripheral running on the High Frequency clock
 *
 * @{
 * \file
 * nrf51822 architecture specific declarations for Contiki's Rtimer
 * \author prithvi
 */

#ifndef RTIMER_ARCH_H_
#define RTIMER_ARCH_H_

#include <stdint.h>
#include "nrf.h"
#include "contiki-conf.h"
//#include "rtimer.h"

#define TIMER0_IRQ_PRI			3

void rtimer_arch_init(void);
void rtimer_arch_schedule(rtimer_clock_t t);



inline __attribute__((always_inline)) rtimer_clock_t rtimer_arch_now(void)
{
  NRF_TIMER0->TASKS_CAPTURE[NOW_REG] = 1UL;
  return NRF_TIMER0->CC[NOW_REG];
}

#endif /* RTIMER_ARCH_H_ */
/**
 * @}
 * @}
 */
