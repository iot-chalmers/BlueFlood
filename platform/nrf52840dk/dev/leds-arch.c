/*
 * leds-arch.c
 *
 *  Created on: 03-Feb-2014
 *      Author: prithvi
 */

/**
 * \addtogroup PCA10000
 * @{
 *
 * \defgroup PCA10000-leds PCA10000's LED driver
 *
 * LED driver implementation for the RGB LED of PCA10000 platform.
 * Since the RGB LED is in a SMD package, a combination of LEDS can
 * create a different color. For example switching on green and blue
 * will produce cyan color.
 *
 * @{
 *
 * \file
 * LED driver implementation for the RGB LED of PCA10000 platform
 *
 * \author prithvi
 */

#include "leds.h"
#include "nrf-gpio.h"
#include "board.h"

/*---------------------------------------------------------------------------*/
/**@brief Function to initialize the LEDS in the PCA10000 board,
 * 		  also switch off all of them
 */
void
leds_arch_init(void){
	  nrf_gpio_cfg_output(LEDS_CONF_RED);
	  nrf_gpio_pin_set(LEDS_CONF_RED);
	  nrf_gpio_cfg_output(LEDS_CONF_GREEN);
	  nrf_gpio_pin_set(LEDS_CONF_GREEN);
	  nrf_gpio_cfg_output(LEDS_CONF_BLUE);
	  nrf_gpio_pin_set(LEDS_CONF_BLUE);
	  nrf_gpio_cfg_output(LEDS_CONF_YELLOW);
	  nrf_gpio_pin_set(LEDS_CONF_YELLOW);
	  nrf_gpio_cfg_output(LEDS_CONF_CYAN);
	  nrf_gpio_pin_set(LEDS_CONF_CYAN);
}
/*---------------------------------------------------------------------------*/
/** \brief Get the current status of the LEDs in the last three bits
 * \return 2nd bit: Red; 1st bit: Blue; 0th bit: Green; Other bits: Zero
 * 		   1 if LED is on, 0 if off
 */
uint8_t
leds_arch_get(void){
	uint8_t temp = 	(nrf_gpio_pin_read(LEDS_CONF_RED)*LEDS_RED)|
			(nrf_gpio_pin_read(LEDS_CONF_BLUE)*LEDS_BLUE)|
			(nrf_gpio_pin_read(LEDS_CONF_GREEN)*LEDS_GREEN);
	return ((~temp)&(LEDS_RED|LEDS_BLUE|LEDS_GREEN));
}
/*---------------------------------------------------------------------------*/
/**
 * \brief 		Set the LEDs of the RGB LED unit with last three bits
 * \param leds	2nd bit: Red; 1st bit: Blue; 0th bit: Green; Other bits: No effect
 * 				LED will light up if the bit is 1, switch off if bit is 0.
 */
void
leds_arch_set(uint8_t leds){
	/* Invert the input since the LEDs are active low */
	//leds = ~leds;
	nrf_gpio_pin_write(LEDS_CONF_GREEN,(leds & LEDS_GREEN));
	nrf_gpio_pin_write(LEDS_CONF_BLUE,(leds & LEDS_BLUE));
	nrf_gpio_pin_write(LEDS_CONF_RED,(leds & LEDS_RED));
	nrf_gpio_pin_write(LEDS_CONF_YELLOW,(leds & LEDS_YELLOW));
	nrf_gpio_pin_write(LEDS_CONF_CYAN,(leds & LEDS_RED));
}

/**
 * @}
 * @}
 */
