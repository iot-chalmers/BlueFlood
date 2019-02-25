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
#define LEDS_CONF_GREEN	18
/** 1st bit in Contiki leds.c */
#define LEDS_CONF_BLUE  19
/** 2nd bit in Contiki leds.c */
#define LEDS_CONF_RED   20

#define LEDS_CONF_YELLOW 21

#define LEDS_CONF_CYAN 22

#define LEDS_CONF_ALL	15

/*---------------------------------------------------------------------------*/

#define BUTTON_0       16
#define BUTTON_1       17

/*---------------------------------------------------------------------------*/
/** @} */

/** \name
 * 	UART Pin definitions
 * 	@{
 */
#define RX_PIN_NUMBER  	11
#define TX_PIN_NUMBER  	9
#define CTS_PIN_NUMBER 	10
#define RTS_PIN_NUMBER 	8
/** Hardware flow control is not required in Core51822 */
#define HWFC           	false

/*---------------------------------------------------------------------------*/
/** @} */
  
#endif  /* BOARD_H */

/**
 * @}
 */
