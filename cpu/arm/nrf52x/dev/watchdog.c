/**
 * \addtogroup nRF51822
 * @{
 *
 * \defgroup nRF51822-wdt nRF51822 watchdog timer driver
 *
 * Driver for the nRF51822 Watchdog Timer
 * @{
 *
 * \file
 * Implementation of the nRF51822 watchdog driver.
 */
#include "contiki.h"
#include "nrf.h"
#include "watchdog.h"

/*---------------------------------------------------------------------------*/
/* Enabled by default */
#ifndef WATCHDOG_CONF_ENABLE
#define WATCHDOG_CONF_ENABLE 1
#endif

#define WATCHDOG_SECONDS 15
static uint8_t wdt_initialized = 0;
/*---------------------------------------------------------------------------*/
/** \brief Initialisation function for the WDT. Currently simply explicitly
 * sets the WDT interval to WATCHDOG_SECONDS interval */
void
watchdog_init(void)
{
  NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | //pause when the debugger halts the cpu
		    ( WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos); //keep running in sleep mode
  NRF_WDT->CRV = WATCHDOG_SECONDS * 32768 - 1; //  timeout [s] = ( CRV + 1 ) / 32768
  NRF_WDT->RREN = WDT_RREN_RR0_Enabled << WDT_RREN_RR0_Pos;

  wdt_initialized = 1;
}
/*---------------------------------------------------------------------------*/
/** \brief Starts the WDT */
void
watchdog_start(void)
{
  if(wdt_initialized) {
    // Start the watchdog
    NRF_WDT->TASKS_START = 1;
  }
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Writes the WDT clear sequence.
 */
void
watchdog_periodic(void)
{
  // Kick the watchdog
  if(wdt_initialized && ( (NRF_WDT->REQSTATUS & 1) != 0 ) ){
    NRF_WDT->RR[0] = WDT_RR_RR_Reload;//WDT_RR_RR_Reload;
  }
}
/*---------------------------------------------------------------------------*/
/** \brief This function is defined here to satisfy API requirements.
 * If the watchdog is configured to generate
an interrupt on the TIMEOUT event, the watchdog reset will be postponed with two 32.768 kHz clock
cycles after the TIMEOUT event has been generated. Once the TIMEOUT event has been generated, the
impending watchdog reset will always be effectuated.
 */
void
watchdog_stop(void)
{
  NRF_WDT->INTENCLR = 1; //if interrupt has been enabled, disable it, but this does not stop the WDT
  return;
}
/*---------------------------------------------------------------------------*/
/** \brief  */
void
watchdog_reboot(void)
{
  NVIC_SystemReset();
}
/**
 * @}
 * @}
 */
