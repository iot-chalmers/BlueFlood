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
  
#ifndef SIMPLE_UART_H
#define SIMPLE_UART_H
  
/*lint ++flb "Enter library region" */ 
  
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
  
/**
*
* \addtogroup nrf51822
* @{
* \defgroup nrf51822-simple-uart Simple UART driver for nrf51822 SoC
* This module contains the UART driver for nrf51822
* @{
* \file
*  Simple UART driver declarations for nrf51822
*/ 
  
/** \brief Function for reading a character from UART.
Execution is blocked until UART peripheral detects character has been received.
\return cr Received character.
*/ 
  uint8_t simple_uart_get(void);
/*---------------------------------------------------------------------------*/
 
/** \brief Function for reading a character from UART with timeout on
 *  how long to wait for the byte to be received.
 *  Execution is blocked until UART peripheral detects character has been
 *  received or until the timeout expires, which even occurs first.
\return bool True, if byte is received before timeout, else returns False.
\param timeout_ms maximum time to wait for the data.
\param rx_data pointer to the memory where the received data is stored.
*/ 
  bool simple_uart_get_with_timeout(int32_t timeout_ms, uint8_t * rx_data);
/*---------------------------------------------------------------------------*/ 
/** \brief Function for sending a character to UART.
Execution is blocked until UART peripheral reports character to have been send.
\param cr Character to send.
*/ 
void simple_uart_put(uint8_t cr);
/*---------------------------------------------------------------------------*/

/** \brief Function for sending a string to UART.
Execution is blocked until UART peripheral reports all characters to have been send.
Maximum string length is 254 characters including null character in the end.
\param str Null terminated string to send.
*/ 
void simple_uart_putstring(const uint8_t * str);
/*---------------------------------------------------------------------------*/

/**  \brief Function to initialize the UART peripheral and also the serial line process
 *   		of Contiki
 */void simple_uart_init();
/*---------------------------------------------------------------------------*/

/** \brief Function to initialize the UART peripheral of nrf51822.
 *			This function configures four pins to be used by UART
 *			peripheral, configures the baudrate used, initializes
 *			the receive interrupt and enables hardware flow control.
\param rts_pin_number Chip pin number to be used for UART RTS
\param txd_pin_number Chip pin number to be used for UART TXD
\param cts_pin_number Chip pin number to be used for UART CTS
\param rxd_pin_number Chip pin number to be used for UART RXD
\param baud_rate Specify the baud rate of UART
\param irq_priority Specify the priority of the UART interrupt (lower number is higher interrupt)
\param hwfc Enable hardware flow control
*/ 
void simple_uart_config(uint8_t rts_pin_number, uint8_t txd_pin_number,
                        uint8_t cts_pin_number, uint8_t rxd_pin_number,
                        uint32_t baud_rate, uint8_t irq_priority, bool hwfc);
/*---------------------------------------------------------------------------*/ 
/**
 *@}
 *@}
 **/ 
  
/*lint --flb "Leave library region" */ 
#endif  /*  */
