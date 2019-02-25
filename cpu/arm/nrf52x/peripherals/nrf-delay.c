/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
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
/** \file
 *  Create approximate millisecond(s) delay using by calling CPU intensive microseconds delays in nrf-delay.h
 *
 */
#include <stdio.h> 
#include "compiler_abstraction.h"
#include "nrf.h"
#include "nrf-delay.h"
  
/*lint --e{438} "Variable not used" */ 
  void
nrf_delay_ms(uint32_t volatile number_of_ms) 
{
  while(number_of_ms != 0)
     {
    number_of_ms--;
    nrf_delay_us(999);
    }
}

