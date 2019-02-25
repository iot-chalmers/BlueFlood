/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */  

/**
*
* \addtogroup nrf51822-simple-uart
* @{
* \file
* Simple UART driver implementation for nrf51822
*/
  
#include "nrf.h"
#include "simple-uart.h"
#include "nrf-delay.h"
#include "nrf-gpio.h"
#include "board.h"
#include "contiki-conf.h"
#include "dev/serial-line.h"

/*---------------------------------------------------------------------------*/
void
UART0_IRQHandler(void) 
{
	NRF_UART0->EVENTS_RXDRDY = 0;
	serial_line_input_byte((uint8_t) NRF_UART0->RXD);}/*---------------------------------------------------------------------------*/
 uint8_t
simple_uart_get(void)
{	while(NRF_UART0->EVENTS_RXDRDY != 1){
        // Wait for RXD data to be received
    }
	NRF_UART0->EVENTS_RXDRDY = 0;
	return (uint8_t) NRF_UART0->RXD;
}
/*---------------------------------------------------------------------------*/
 bool
simple_uart_get_with_timeout(int32_t timeout_ms, uint8_t * rx_data)
{
  bool ret = true;
  while(NRF_UART0->EVENTS_RXDRDY != 1){
	  if(timeout_ms-- >= 0){
		  /* wait in 1ms chunk before checking for status */
		  nrf_delay_us(1000);	  } else {		  ret = false;
		  break;
      }
  }		/* Wait for RXD data to be received */
    if(timeout_ms >= 0){
	  /* clear the event and set rx_data with received byte */
      NRF_UART0->EVENTS_RXDRDY = 0;      *rx_data = (uint8_t) NRF_UART0->RXD;
  }
  return ret;
}
/*---------------------------------------------------------------------------*/
void
simple_uart_put(uint8_t cr)
{
	NRF_UART0->TXD = (uint8_t) cr;
	while(NRF_UART0->EVENTS_TXDRDY != 1){
		// Wait for TXD data to be sent
    }
	NRF_UART0->EVENTS_TXDRDY = 0;}
/*---------------------------------------------------------------------------*/

/** \brief Function to redirect the printf stream of stdio.h to UART
 * \param fd File descriptor, which is not used here
 * \param str Pointer to the string which needs to be sent by UART
 * \param len Length of the string sent
 * \return Returns length of the stream sent, as this will always be successful
 */
uint32_t
_write(int fd, char * str, int len){
	int i;
	for (i = 0; i < len; i++){
		simple_uart_put(str[i]);
	}

	/* Make the cursor start at the beginning of the line */
	//simple_uart_put('\r');

	return len;
}/*---------------------------------------------------------------------------*/
 void
simple_uart_putstring(const uint8_t * str)
{	uint_fast8_t i = 0;	uint8_t ch = str[i++];	while(ch != '\0'){		simple_uart_put(ch);		ch = str[i++];	}}
/*---------------------------------------------------------------------------*/

void
simple_uart_init(){
	  simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER,
			  RX_PIN_NUMBER, UART_BAUDRATE, 3, HWFC);
	  serial_line_init();
}/*---------------------------------------------------------------------------*/
 void
simple_uart_config(uint8_t rts_pin_number,uint8_t txd_pin_number,
                   uint8_t cts_pin_number, uint8_t rxd_pin_number,
                   uint32_t baud_rate, uint8_t irq_priority, bool hwfc) 
{
    nrf_gpio_cfg_output(txd_pin_number);	nrf_gpio_cfg_input(rxd_pin_number, NRF_GPIO_PIN_NOPULL);	NRF_UART0->PSELTXD = txd_pin_number;	NRF_UART0->PSELRXD = rxd_pin_number;

	if(hwfc){	/* Enable hardware flow control */		nrf_gpio_cfg_output(rts_pin_number);		nrf_gpio_cfg_input(cts_pin_number, NRF_GPIO_PIN_NOPULL);		NRF_UART0->PSELCTS = cts_pin_number;		NRF_UART0->PSELRTS = rts_pin_number;		NRF_UART0->CONFIG = (UART_CONFIG_HWFC_Enabled << UART_CONFIG_HWFC_Pos);	}
	NRF_UART0->BAUDRATE = (baud_rate << UART_BAUDRATE_BAUDRATE_Pos);	NRF_UART0->ENABLE = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);	NRF_UART0->TASKS_STARTTX = 1;	NRF_UART0->TASKS_STARTRX = 1;	NRF_UART0->EVENTS_RXDRDY = 0;

	// Enable UART interrupt
	NRF_UART0->INTENCLR = 0xffffffffUL;	NRF_UART0->INTENSET =
		(UART_INTENSET_RXDRDY_Set << UART_INTENSET_RXDRDY_Pos)
		// | (UART_INTENSET_TXDRDY_Set << UART_INTENSET_TXDRDY_Pos)
		// | (UART_INTENSET_ERROR_Set << UART_INTENSET_ERROR_Pos)
		;
	NVIC_ClearPendingIRQ(UART0_IRQn);	NVIC_SetPriority(UART0_IRQn, irq_priority);	NVIC_EnableIRQ(UART0_IRQn);
  }
/*---------------------------------------------------------------------------*/

/**
 * @}
 */
