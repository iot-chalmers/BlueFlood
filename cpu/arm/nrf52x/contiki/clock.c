/**
 * \addtogroup nrf51822
 * @{
 *
 * \defgroup nrf51-contiki-clock Contiki Clock implementation for nrf51822 SoC
 * This module contains the implementation of the clock library
 * of Contiki for nrf51822. This module uses the \ref nrf51-clock
 * module for its implementation.
 * @{
 *
 * \file
 * File containing the functions which call the nrf51822's clock implementation of Contiki
 *  \author prithvi
 */

#include "clock-nrf.h"
#include "sys/clock.h"
#include "nrf-delay.h"

/**
 * \brief nrf51822 specific the Contiki clock initialization with RTC1.
 *
 * This function initializes the clock library and should be called
 * from the main() function of the system. nrf51822's high frequency
 * clock (16 MHz), low frequency 32.768 kHz clock and RTC1 which is
 * used for Contiki's clock.
 *
 */
void
clock_init(void)
{
  hfclk_xtal_init();
  lfclk_init();
  rtc_init();
}

/**
 * \brief Get the current clock time.
 *
 * This function returns the current system clock time.
 *
 * \return The current clock time, measured in system ticks.
 */
CCIF clock_time_t
clock_time(void)
{
  return nrf_clock_time();
}

/**
 * \brief Get the current value of the platform seconds.
 *
 * This could be the number of seconds since startup, or
 * since a standard epoch.
 *
 * \return The seconds passed.
 */
CCIF unsigned long
clock_seconds(void)
{
  return nrf_clock_seconds();
}
/**
 * \brief Set the value of the platform seconds.
 * \param sec   The value to set for seconds.
 *
 */
void
clock_set_seconds(unsigned long sec)
{
  nrf_clock_set_seconds(sec);
}

/**
 * \brief Wait for a given number of ticks.
 * \param t   How many ticks.
 *
 */
void
clock_wait(clock_time_t t)
{
  if(t) {
    clock_time_t end_t = t + nrf_clock_time();

    while(nrf_clock_time() < end_t) {
    }
  }
}

/**
 * \brief Delay a given number of microseconds.
 * 		  This implementation uses NOP statements of CPU to while
 * 		  alway the required amount of time. So, it is not power
 * 		  efficient and accurate.
 * \param dt How many milliseconds to delay.
 *
 * \note Interrupts could increase the delay by a variable amount.
 */
void
clock_delay(unsigned int dt)
{
  nrf_delay_ms(dt);
}

/**
 * \brief Delay a given number of microseconds.
 *		  This implementation uses NOP statements of CPU to while
 * 		  alway the required amount of time. So, it is not power
 * 		  efficient and accurate.
 * \param dt   How many microseconds to delay.
 *
 * \note Interrupts could increase the delay by a variable amount.
 */
void
clock_delay_usec(uint16_t dt)
{
  nrf_delay_us(dt);
}

/** \brief Function initializes code to call etimer poll based on expiration time received
 *			The counter compare interrupt is initialized so that the interrupt occurs when
 *			the expiration occurs and etimer poll is called from the ISR.
 * \param expiration_time The value of current_clock at which etimer expiration occurs
 * \warning Since the RTC is a 24 bit counter, the expiration time must be less that 2^24
 * 			even though the current_clock is 32 bit variable.
 */
void
clock_update_expiration_time(clock_time_t expiration_time)
{
  nrf_clock_update_expiration_time(expiration_time);
}

/**
 * @}
 * @}
 */
