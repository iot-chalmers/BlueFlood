/**
 * \addtogroup nrf51822
 * @{
 * \defgroup nrf51-test
 *
 * @{
 * \file
 * Testcode for the nRF51822
 * \author CoenRoest
 */
#ifndef NRF_TEST_H_
#define NRF_TEST_H_

#define DEBUG 1
#if (DEBUG)
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif /* (DEBUG) */

#define PRINT_PACKET(packet, name, length)	\
	PRINTF("Packet (%s): ", name);		\
	for (int i = 0; i < length; i++){	\
	    PRINTF("%hu ", packet[i]);}		\
	PRINTF("\n");

#define PRINT_STATS(stats, name, length)	\
	PRINTF("Stats (%s): ", name);		\
	for (int i = 0; i < length; i++){	\
	    PRINTF("%.2f ", stats[i]);}		\
	PRINTF("\n");

#define PRINT_STATS_UINT16(stats, name, length)	\
	PRINTF("Stats (%s): ", name);		\
	for (int i = 0; i < length; i++){	\
	    PRINTF("%u ", stats[i]);}		\
	PRINTF("\n");

#define PRINT_ARRAY_UINT8(array, name, length)	\
	PRINTF("array (%s): ", name);		\
	for (int i = 0; i < length; i++){	\
	    PRINTF("%hu ", array[i]);}		\
	PRINTF("\n");

#endif /* NRF_TEST_H_ */
/**
 * @}
 * @}
 */
