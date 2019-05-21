/** \addtogroup Core51822
 * @{
 *
 * \file
 * Header file with definitions related to the pins of LED, Buttons
 * and serial port connections in the Core51822 platform
 *  \author coenroest
 */

#ifndef BOARD_H
#define BOARD_H
 
/*---------------------------------------------------------------------------*/
/** \name
 * 	LED Pin definitions
 * 	@{
 */
/** 0th bit in Contiki leds.c */
#define LEDS_CONF_GREEN	13
/** 1st bit in Contiki leds.c */
#define LEDS_CONF_BLUE  14
/** 2nd bit in Contiki leds.c */
#define LEDS_CONF_RED   15

#define LEDS_CONF_YELLOW 16

#define LEDS_CONF_CYAN 13

#define LEDS_CONF_ALL	(13|14|15|16uL)

/*---------------------------------------------------------------------------*/

#define BUTTON_0       11
#define BUTTON_1       12

/*---------------------------------------------------------------------------*/
/** @} */

/** \name
 * 	UART Pin definitions
 * 	@{
 */
#define RX_PIN_NUMBER  	8
#define TX_PIN_NUMBER  	6
#define CTS_PIN_NUMBER 	7
#define RTS_PIN_NUMBER 	5


/*---------------------------------------------------------------------------*/
/** @} */
  
#endif  /* BOARD_H */

/**
 * @}
 */
