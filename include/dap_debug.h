/***************************************************************************
 * Artekit AK-CMSIS-DAP JTAG
 * https://www.artekit.eu/products/debug/ak-cmsis-dap
 *
 * Written by Ruben Meleca
 * Copyright (c) 2018 Artekit Labs
 * https://www.artekit.eu

### dap_debug.h

# This file is part of AK-CMSIS-DAP.

# AK-CMSIS-DAP is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# AK-CMSIS-DAP is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with AK-CMSIS-DAP.  If not, see <http://www.gnu.org/licenses/>.

***************************************************************************/

#ifndef __DAP_DEBUG_H__
#define __DAP_DEBUG_H__

// Comment or uncomment to to disable or enable debug
// #define DAPDEBUG

#ifdef DAPDEBUG

	#define DAPDEBUG_IRQ_NAME	USART1_RX_IRQHandler		/* USART IRQ Handler */
	#define DAPDEBUG_CLK		cmuClock_USART1				/* HFPER Clock */
	#define DAPDEBUG_IRQn		USART1_RX_IRQn				/* IRQ number */
	#define DAPDEBUG_UART		USART1						/* USART instance */
	#define DAPDEBUG_TX			USART_Tx					/* Set TX to USART_Tx */
	#define DAPDEBUG_RX			USART_Rx					/* Set RX to USART_Rx */
	#define DAPDEBUG_LOCATION	USART_ROUTE_LOCATION_LOC0	/* Location of of the USART I/O pins */
	#define DAPDEBUG_TXPORT		gpioPortC					/* USART transmission port */
	#define DAPDEBUG_TXPIN		0							/* USART transmission pin */
	#define DAPDEBUG_RXPORT		gpioPortC					/* USART reception port */
	#define DAPDEBUG_RXPIN		1							/* USART reception pin */
	#define DAPDEBUG_USART		1							/* Includes em_usart.h */
	#define DAPDEBUG_ENPORT		gpioPortC					/* USART transmission port */
	#define DAPDEBUG_ENPIN		0							/* USART transmission pin */
	#define	DAPDEBUG_BUFLEN		32							/* RX Buffer lenght */
	#define	DAPDEBUG_ECHO		1							/* RX Buffer lenght */

	#ifdef __cplusplus
		extern "C" {
	#endif

		void dapDebugString(char *str);
		void dapDebugInit(void);
		void dapDumpIncomingData(uint8_t* data, uint32_t len);
		void dapDumpSWJSequence(uint8_t* data);
		void dapDumpJTAGSequence(uint8_t* data);

	#ifdef __cplusplus
		}
	#endif

	#define	DEBUG_STRING(str)				dapDebugString(str)
	#define	DEBUG_DUMP_INCOMING(data,len)	dapDumpIncomingData(data, len)
	#define	DEBUG_INIT()					dapDebugInit()
	#define DEBUG_JTAG_SEQ(data)			dapDumpJTAGSequence(data)

#else

	#define	DEBUG_STRING(str)
	#define	DEBUG_DUMP_INCOMING(data,len)
	#define	DEBUG_INIT()
	#define DEBUG_JTAG_SEQ(data)

#endif /* DAPDEBUG */

#endif /* __DAP_DEBUG_H__ */
