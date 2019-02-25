/**
 * \addtogroup nrf51822
 * @{
 * \defgroup nrf51-watchdog
 *
 * @{
 * \file
 * nRF51 Watchdog driver
 * \author CoenRoest
 */
#ifndef WATCHDOG_H_
#define WATCHDOG_H_

#include "nrf.h"
#include "contiki-conf.h"

void watchdog_init(void);
void watchdog_start(void);
void watchdog_periodic(void);
void watchdog_stop(void);
void watchdog_reboot(void);

#endif /* WATCHDOG_H_ */
/**
 * @}
 * @}
 */
