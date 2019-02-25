/**
 * \addtogroup nrf51-clock
 *
 * @{
 *
 * \file
 * nrf51822 architecture specific implementation for Contiki's Clock
 * \author prithvi
 */

#include "clock-nrf.h"
#include "sys/etimer.h"

/** \brief Function starting the LFCLK oscillator.
 * 	The \ref SRC_LFCLK in the contiki-conf.h file is used
 * 	to configure the source of LF Clock.
 */
void
lfclk_init(void)
{
  NRF_CLOCK->LFCLKSRC = (SRC_LFCLK << CLOCK_LFCLKSRC_SRC_Pos);
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_LFCLKSTART = 1;
  while(NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {
    //Do nothing.
  }
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
}

/** \brief Function stopping the LFCLK oscillator.
 */
void
lfclk_deinit(void)
{
  NRF_CLOCK->TASKS_LFCLKSTOP = 1;
}

/** \brief Function starting the HFCLK XTAL 16 MHz crystal oscillator.
 */
void
hfclk_xtal_init(void)
{
  /* Start 16 MHz crystal oscillator. */
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_HFCLKSTART = 1;

  /* Wait for the external oscillator to start up. */
  while(NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
    // Do nothing.
  }
}

/** \brief Function stopping the HFCLK XTAL 16 MHz crystal oscillator.
 * 			MCU will run on 16 MHz RC oscillator
 */
void
hfclk_xtal_deinit(void)
{
  NRF_CLOCK->TASKS_HFCLKSTOP = 1;
}

/** \brief Function for initializing the RTC1 for Contiki clock.
 * Configures the RTC to increments at a freq of \ref CLOCK_CONF_SECOND Hz
 * and initializes the overflow interrupt if \ref TICKLESS is TRUE,
 * else initializes the counter tick interrupt
 */
void
rtc_init(void)
{
  /* Set prescaler so that TICK freq is CLOCK_SECOND */
  NRF_RTC1->PRESCALER = COUNTER_PRESCALER;

 if (TICKLESS){
  /* Enable overflow event and overflow interrupt */
  NRF_RTC1->EVTENSET      = RTC_EVTENSET_OVRFLW_Msk;;
  NRF_RTC1->INTENSET      = RTC_INTENSET_OVRFLW_Msk;
 }else{
  /* Enable TICK event and TICK interrupt: */
  NRF_RTC1->EVTENSET = RTC_EVTENSET_TICK_Msk;
  NRF_RTC1->INTENSET = RTC_INTENSET_TICK_Msk;
 }

  /* Enable Interrupt for RTC1 in the core */
  NVIC_SetPriority(RTC1_IRQn, RTC1_IRQ_PRI);
  NVIC_EnableIRQ(RTC1_IRQn);

  NRF_RTC1->TASKS_START = 1;

}

/** \brief Function to return the number of ticks of the clock
 *  \return The current clock time
 */
clock_time_t
nrf_clock_time(void)
{
  if(TICKLESS){
	return NRF_RTC1->COUNTER;
  }else{
    return current_clock;
  }
}

/** \brief Function to return the \ref current_seconds
 * 			The number of seconds passed since power up or
 * 			since \ref nrf_clock_set_seconds is called.
 *  \return The \ref current_seconds
 */
unsigned long
nrf_clock_seconds(void)
{
  if(TICKLESS){
	current_seconds = seconds_offset + ((seconds_ovr*16777216 + NRF_RTC1->COUNTER) / CLOCK_SECOND);
  }
  return current_seconds;
}

/** \brief Function to set the \ref current_seconds
 *
 * @param sec The value to set the \ref current_seconds to
 */
void
nrf_clock_set_seconds(unsigned long sec)
{
  if(TICKLESS){
	seconds_offset = sec - ((seconds_ovr*16777216 + NRF_RTC1->COUNTER) / CLOCK_SECOND);
  }else{
  	current_seconds = sec;
  }
}

/** \brief Function initializes code to call etimer poll based on expiration time received
 *			The counter compare interrupt is initialized so that the interrupt occurs when
 *			the expiration occurs and etimer poll is called from the ISR.
 * \param expiration_time The value of clock at which etimer expiration occurs
 * \warning Since the RTC is a 24 bit counter, the expiration time must be less that 2^24
 * 			even though the current clock and expiration time are 32 bit variable.
 * \todo Take care of the condition when an expiration time of value greater than 2^24 is passed.
 */

void
nrf_clock_update_expiration_time(clock_time_t expiration_time)
{
  // Enable COMPARE1 event and COMPARE1 interrupt:
  NRF_RTC1->EVTENSET      = RTC_EVTENSET_COMPARE1_Msk;
  NRF_RTC1->INTENSET      = RTC_INTENSET_COMPARE1_Msk;
  // Compare1 interrupt at expiration time
  NRF_RTC1->CC[1]         = expiration_time;
  NRF_RTC1->EVENTS_COMPARE[1] = 0;
}

/** \brief Function for handling the RTC1 interrupts.
 * If the \ref TICKLESS is TRUE then the interrupt sources can be
 * either the counter overflow or counter compare. When overflow
 * occurs \ref seconds_ovr variable can be updated so that the seconds
 * passed can be read. When counter compare interrupt occurs the
 * etimer expiration has occurred and etimer poll must be called.
 * \n If \ref TICKLESS is FALSE then the interrupt will occur every tick
 * of RTC. Here the current clock and \ref current_seconds are
 * updated. Also the etimer expiration is checked every time and
 * etimer poll is called if expiration has occurred.
 *
 */
void
RTC1_IRQHandler()
{
  if(TICKLESS){
	  if(NRF_RTC1->EVENTS_OVRFLW == 1){
		  NRF_RTC1->EVENTS_OVRFLW = 0;
		  seconds_ovr++;
	  }
	  if(NRF_RTC1->EVENTS_COMPARE[1] == 1){
		  NRF_RTC1->EVENTS_COMPARE[1] = 0;
		  // Disable COMPARE1 event and COMPARE1 interrupt:
		  NRF_RTC1->EVTENCLR      = RTC_EVTENSET_COMPARE1_Msk;
		  NRF_RTC1->INTENCLR      = RTC_INTENSET_COMPARE1_Msk;
		  //printf("poll\n");
		  etimer_request_poll();
	  }
  }else{
      NRF_RTC1->EVENTS_TICK = 0;
	  current_clock++;

	  if(etimer_pending()){
		if(etimer_next_expiration_time() <= current_clock) {
		  etimer_request_poll();
		  /* printf("%d,%d\n", clock_time(),etimer_next_expiration_time   ()); */
		}
	  }

	  if(--second_countdown == 0) {
		current_seconds++;
		second_countdown = CLOCK_SECOND;
	  }
  }
}

/**
 * @}
 */
