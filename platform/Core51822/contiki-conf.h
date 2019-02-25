/**
 * \addtogroup Core51822
 * @{
 *
 * \file
 * Platform specific configurations for Core51822.
 * Core51822's platform specific definitions required for Contiki
 * The parameters defined are Clock and RTIMER's frequency, UART baud rate,
 * Low frequency clock source and typedef related to clock, rtimer and uip stats.
 *  \author CoenRoest
 */

#ifndef CONTIKI_CONF_H_
#define CONTIKI_CONF_H_


#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "nrf.h"

/*---------------------------------------------------------------------------*/
/** Contiki defines */

#ifndef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     csma_driver
#endif /* NETSTACK_CONF_MAC */

#ifndef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     contikimac_driver
#endif /* NETSTACK_CONF_RDC */

#ifndef NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE
#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE 8
#endif /* NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE */

#ifndef NETSTACK_CONF_RADIO
#define NETSTACK_CONF_RADIO   nrf_radio_driver
#endif /* NETSTACK_CONF_RADIO */

#if NETSTACK_CONF_WITH_IPV6
/* Network setup for IPv6 */
#define NETSTACK_CONF_NETWORK sicslowpan_driver

/* Specify a minimum packet size for 6lowpan compression to be
   enabled. This is needed for ContikiMAC, which needs packets to be
   larger than a specified size, if no ContikiMAC header should be
   used. */
#define SICSLOWPAN_CONF_COMPRESSION_THRESHOLD 63

#define CXMAC_CONF_ANNOUNCEMENTS         0
#define XMAC_CONF_ANNOUNCEMENTS          0

#ifndef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM                8
#endif

#elif NETSTACK_CONF_WITH_CHAOS /* NETSTACK_CONF_WITH_IPV6 */

#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     chaosmac_driver

#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     nordc_driver

#undef NETSTACK_CONF_NETWORK
#define NETSTACK_CONF_NETWORK chaosnet_driver

#else /* NETSTACK_CONF_WITH_IPV6, NETSTACK_CONF_WITH_CHAOS */
/* Network setup for non-IPv6 (rime). */

#define NETSTACK_CONF_NETWORK rime_driver

#define COLLECT_CONF_ANNOUNCEMENTS       1
#define CXMAC_CONF_ANNOUNCEMENTS         0
#define XMAC_CONF_ANNOUNCEMENTS          0
#define CONTIKIMAC_CONF_ANNOUNCEMENTS    0

#ifndef COLLECT_NEIGHBOR_CONF_MAX_COLLECT_NEIGHBORS
#define COLLECT_NEIGHBOR_CONF_MAX_COLLECT_NEIGHBORS     32
#endif /* COLLECT_NEIGHBOR_CONF_MAX_COLLECT_NEIGHBORS */

#ifndef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM                16
#endif /* QUEUEBUF_CONF_NUM */

#ifndef TIMESYNCH_CONF_ENABLED
#define TIMESYNCH_CONF_ENABLED           0
#endif /* TIMESYNCH_CONF_ENABLED */

#if TIMESYNCH_CONF_ENABLED
/* CC2420 SDF timestamps must be on if timesynch is enabled. */
#undef CC2420_CONF_SFD_TIMESTAMPS
#define CC2420_CONF_SFD_TIMESTAMPS       1
#endif /* TIMESYNCH_CONF_ENABLED */

#endif /* NETSTACK_CONF_WITH_IPV6 */

/*---------------------------------------------------------------------------*/
/** Set the radio shortcuts */
#define RADIO_SHORTS_ENABLED true

#define ENERGEST_CONF_ON 1

/*---------------------------------------------------------------------------*/
/** Specify the UART baud rate */
#ifndef UART_BAUDRATE
#define UART_BAUDRATE UART_BAUDRATE_BAUDRATE_Baud230400
#endif

/** Defines for the capture/compare registers in TIMER0*/
#define SCHEDULE_REG 		2
#define TIMESTAMP_ADDR_REG 	1
#define TIMESTAMP_END_REG	1
#define NOW_REG			3

/** Defines for the SFD events*/
#define ADDRESS_EVENT 		0
#define END_EVENT 		1

/* CPU target speed in Hz */
#define F_CPU 16000000uL

/** \name Compiler configurations.
 * CCIF and CLIF are defined but used only in Windows based platforms
 * \code#define CCIF
 *  #define CLIF \endcode
 * @{
 */
/** \showinitializer */
#define CCIF
/** \showinitializer */
#define CLIF
/** @} */

/** TRUE if a is less than b, otherwise FALSE.
 * rtimer.h typedefs rtimer_clock_t as unsigned short.
 * We need to define RTIMER_CLOCK_LT here to override this.
 */
#define RTIMER_CLOCK_LT(a,b)    ((int32_t)((a)-(b)) < 0)

/** \name Platform typedefs are all unsigned 32 bit
 * @{ */
typedef uint32_t clock_time_t;
typedef uint32_t uip_stats_t;
typedef uint32_t rtimer_clock_t;
/** @} */

/** \anchor nrf-rtimer-definitions
 * \name Definitions for the TIMER1 initialization
 * Used in \ref nrf51-contiki-rtimer module
 * @{*/
/** The Rtimer's TIMER tick freq is HF freq divided by two to power of value defined here*/
#define TIMER_PRESCALER 	0
/** \brief Number of bits used for the Rtimer's TIMER */
#define TIMER_BITSIZE TIMER_BITMODE_BITMODE_32Bit

#if TIMER_BITSIZE == TIMER_BITMODE_BITMODE_08Bit
/** Since the Rtimer is 8 bit the CC interrupt occurs every 2^8 ticks of the TIMER */
#define TIMER_COMPARE_FREQ	256
#endif

#if TIMER_BITSIZE == TIMER_BITMODE_BITMODE_32Bit
#define TIMER_COMPARE_FREQ	1
#endif

/** HF Clock frequency in Hertz, constant.
 *  When RC is used it is always 16 MHz. When a crystal is
 *  used it is either 16 MHz or 32 MHz based on the crystal.
 */
#define HFCLK_FREQUENCY		  (16000000UL)

/** \brief 16MHz clock based on TIMER0 for the RTIMER
 *  16MHz = 16MHz/((1<<0)x1). To decrease this frequency \ref TIMER_PRESCALER
 *  can be increased till 9 */
#define RTIMER_ARCH_SECOND 		(((HFCLK_FREQUENCY)/(1<<TIMER_PRESCALER))/TIMER_COMPARE_FREQ)
/** @} */

/** \name Definitions for the RTC1 initialization
 * Used in \ref nrf51-contiki-clock module
 * @{*/
/** \brief Frequency of main clock, for which RTC1 is used
 */
#define CLOCK_CONF_SECOND 64
/* Check if CLOCK_CONF_SECOND is power of 2 and less than or equal to 32.768 kHz */
#if (!(!(CLOCK_CONF_SECOND & (CLOCK_CONF_SECOND-1)) && CLOCK_CONF_SECOND && (CLOCK_CONF_SECOND<=32768)))
#error CLOCK_CONF_SECOND must be a power of 2 with a maximum frequency of 32768 Hz
#endif

/** Specify the source of the Low freq clock for the platform */
#define SRC_LFCLK CLOCK_LFCLKSRC_SRC_Xtal

/** Specify if TICKLESS implementation is required*/
#define TICKLESS true
/** @} */
/*---------------------------------------------------------------------------*/

#endif /* CONTIKI_CONF_H_ */
/** @} */
