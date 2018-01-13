/***************************************************************************
 * Artekit AK-CMSIS-DAP JTAG
 * https://www.artekit.eu/products/debug/ak-cmsis-dap
 *
 * Written by Ruben Meleca
 * Copyright (c) 2018 Artekit Labs
 * https://www.artekit.eu

### defs.h

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

#ifndef __HW_H__
#define __HW_H__

#include "stdint.h"

#if defined ( __GNUC__ )
#define __ALWAYS_INLINE __attribute__((always_inline)) inline
#define __forceinline	__ALWAYS_INLINE
#define __weak			__attribute__ ((weak))
#endif

/* PORT assignment */

#define	DAP_TCK_SWDCLK_PORT			gpioPortF				/* Port for TCK/SWDCLK */
#define	DAP_TMS_SWDIO_PORT			gpioPortE				/* Port for TMS/SWDIO */
#define	DAP_TDO_PORT				gpioPortE				/* Port for TDO */
#define	DAP_TDI_PORT				gpioPortB				/* Port for TDI */
#define	DAP_TRST_PORT				gpioPortB				/* Port for TRST */
#define	DAP_BUFREV_PORT				gpioPortB				/* Port for BUFREV */
#define	DAP_RES_INOUT_PORT			gpioPortB				/* Port for RES_INOUT */
#define	DAP_BOOT_PORT				gpioPortA				/* Port for BOOT KEY */
#define	DAP_LED_PORT				gpioPortB				/* Port for STATUS LED */
#define DAP_TXD_PORT				gpioPortC				/* Port for UART TX */
#define DAP_RXD_PORT				gpioPortC				/* Port for UART TX */

/* PIN assignment */

#define	DAP_TCK_SWDCLK_PIN			2						/* Pin for TCK/SWDCLK */
#define	DAP_TMS_SWDIO_PIN			12						/* Pin for TMS/SWDIO */
#define	DAP_TDO_PIN					13						/* Pin for TDO */
#define	DAP_TDI_PIN					7						/* Pin for TDI */
#define	DAP_TRST_PIN				8						/* Pin for TRST */
#define	DAP_BUFREV_PIN				11						/* Pin for BUFREV */
#define	DAP_RES_INOUT_PIN			13						/* Pin for RES_INOUT */
#define	DAP_BOOT_PIN				0						/* Pin for BOOT KEY */
#define	DAP_LED_PIN					14						/* Pin for STATUS LED */
#define DAP_TXD_PIN					0						/* Pin for UART TX */
#define DAP_RXD_PIN					1						/* Pin for UART RX */

/* USART */

#define DAP_USARTG_IRQ_NAME			USART1_RX_IRQHandler	/* USART IRQ Handler */
#define DAP_USART_CLK				cmuClock_USART1			/* HFPER Clock */
#define DAP_USART_IRQn				USART1_RX_IRQn			/* IRQ number */
#define DAP_USART					USART1					/* USART instance */
#define DAP_USART_TX				USART_Tx				/* Set TX to USART_Tx */
#define DAP_USART_RX				USART_Rx				/* Set RX to USART_Rx */
#define DAP_USART_LOCATION			USART_ROUTE_LOCATION_LOC0	/* Location of of the USART I/O pins */

/* LED MODES */

#define	DAP_LEDMODE_0				0						/* Status LED OFF, when the DAP is disconnected */
#define	DAP_LEDMODE_1				1						/* Status LED blinking slowly, connected to the debugger */
#define	DAP_LEDMODE_2				2						/* Status LED blinking fast, DAP debugging */

#define DAP_LEDMODE_1_DELAY			1000					/* 1 second blink */
#define DAP_LEDMODE_2_DELAY			100						/* 150 mS  blink */

/* BUFREV status */

#define	DAP_BUFREV_OUT				0						/* BUFREV is in output mode */
#define	DAP_BUFREV_INP				1						/* BUFREV is in input mode */

#endif // __HW_H__

