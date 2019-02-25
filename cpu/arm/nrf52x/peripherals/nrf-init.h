/**
 * \addtogroup nrf51822
 * @{
 * \defgroup nrf51-init
 *
 * @{
 * \file
 * Initializations for the GPIO, shortcuts and PPI channels
 * \author CoenRoest
 */
#ifndef NRF_INIT_H_
#define NRF_INIT_H_


#include "nrf.h"
#include "contiki-conf.h"

void nrf_init(void);
void ppi_init(void);
void gpiote_init(void);

#endif /* NRF_INIT_H_ */
/**
 * @}
 * @}
 */
