/**
 * \addtogroup nrf51-contiki-rtimer
 * @{
 * \file
 * nrf51822 architecture specific implementation for Contiki's Rtimer
 *  \author prithvi
 *  \author CoenRoest
 */

#include "sys/rtimer.h"
#include "clock-nrf.h"
#include "rtimer-arch.h"

/** Boolean value to store if a Rtimer task has been scheduled */
static bool rtimer_scheduled = false;

/** \brief Initialize the RTIMER using TIMER0.
* This function makes sure the high frequency clock is running,
* initializes TIMER0 based on the parameters defined in
* \ref nrf-rtimer-definitions "Macros for RTIMER using nrf51822"
* and initializes the compare 0 interrupt.
 */
void
rtimer_arch_init (void)
{
  /* Check if the HF clock is running*/
  if ((NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKSTAT_STATE_Msk) == 0)
    {
      hfclk_xtal_init ();
    }

  /* Clear the task to make sure the timer is stopped */
  NRF_TIMER0->TASKS_CLEAR = 1;
  /* Set the timer in Timer Mode */
  NRF_TIMER0->MODE = TIMER_MODE_MODE_Timer;
  /* Prescaler 0 produces 16MHz timer tick frequency */
  NRF_TIMER0->PRESCALER = TIMER_PRESCALER;
  /* 32 bit mode */
  NRF_TIMER0->BITMODE = TIMER_BITSIZE;
  /* Enable the Compare event on 0th channel */
  NRF_TIMER0->EVENTS_COMPARE[SCHEDULE_REG] = 0;

  /* Enable overflow event and overflow interrupt */
  NRF_TIMER0->INTENSET = TIMER_INTENSET_COMPARE3_Msk; //##SCHEDULE_REG##

  /* Enable shorts for auto-reset
   *
   * This messes with the timestamping and RTIMER_NOW()
   * Maybe not particularly needed right now.
   *
   * */
  //NRF_TIMER0->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;

  NVIC_SetPriority (TIMER0_IRQn, TIMER0_IRQ_PRI);

  /* Start the timer */
  NRF_TIMER0->TASKS_START = 1UL;
}

/** Schedule the next Rtimer task
 * \brief Places the scheduled time in a compare register
 * and enables the interrupt handler.
 *
 * \param t time at which next Rtimer task should be executed
 */
void
rtimer_arch_schedule (rtimer_clock_t t)
{
  /* Put the scheduled time in a compare register */
  NRF_TIMER0->CC[SCHEDULE_REG] = t;

  /* Enable the interrupt handler */
  NVIC_EnableIRQ (TIMER0_IRQn);

  /* Set the scheduled flag */
  rtimer_scheduled = true;
}

/*
*
 * \brief      Get the current clock time \n
 * 		This function calls the nRF51 capture task to put the current
 * 		counter value in a register.
 * \return     This function returns what is in the capture register.

rtimer_clock_t rtimer_arch_now (void)
{
  NRF_TIMER0->TASKS_CAPTURE[NOW_REG] = 1UL;
  return NRF_TIMER0->CC[NOW_REG];
}
*/

/** \brief Function for handling the TIMER1 interrupt.
 * TIMER1 interrupt is triggered on COMPARE[0]. \n
 * In this interrupt routine the rtimer count is incremented.
 * If an rtimer task is scheduled then the interrupt is disabled and
 * rtimer's next run function is called.
 */
void
TIMER0_IRQHandler ()
{
  /* Check if this is a compare event and not an overflow */
  if (NRF_TIMER0->EVENTS_COMPARE[SCHEDULE_REG] == 1)
    {
      /* Reset the compare event */
      NRF_TIMER0->EVENTS_COMPARE[SCHEDULE_REG] = 0;
      if (rtimer_scheduled)
	{
	  NVIC_DisableIRQ (TIMER0_IRQn);
	  rtimer_scheduled = false;
	  rtimer_run_next ();
	}
    } /* else it is overflow */
  else
    printf("OVERFLOW\n\r");
}
/**
 * @}
 */
