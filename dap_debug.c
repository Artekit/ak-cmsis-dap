/***************************************************************************
 * Artekit AK-CMSIS-DAP JTAG
 * https://www.artekit.eu/products/debug/ak-cmsis-dap
 *
 * Written by Ruben Meleca
 * Copyright (c) 2018 Artekit Labs
 * https://www.artekit.eu

### dap_debug.c

#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

***************************************************************************/

#include <stdio.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_int.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "dap_debug.h"

#ifdef DAPDEBUG

USART_TypeDef *usart = DAPDEBUG_UART;
USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;

void dapDebugInit(void)
{
	// Enable clock for IO pins
	CMU_ClockEnable(cmuClock_GPIO, true);

	// Set pin mode for Tx and Rx
	GPIO_PinModeSet(DAPDEBUG_TXPORT, DAPDEBUG_TXPIN, gpioModePushPull, 1);
	GPIO_PinModeSet(DAPDEBUG_RXPORT, DAPDEBUG_RXPIN, gpioModeInput, 0);

	// Enable peripheral clocks
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(DAPDEBUG_CLK, true);

	// Set usart as async
	init.enable = usartDisable;
	USART_InitAsync(usart, &init);

	// Set UART/USART location
	usart->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | DAPDEBUG_LOCATION;

	// Enable USART
	USART_Enable(usart, usartEnable);
}

void dapDebugString(char *str)
{
	char *p = str;

	while (*p) {
		if (USART1->STATUS & (1 << 6))
		{
			USART1->TXDATA = *p;
			++p;
		}
	} 
}

void dapDumpIncomingData(uint8_t* data, uint32_t len)
{
	uint32_t i = 0;
	uint8_t b = 0;
	char c;

	for (i = 0; i < len; i++)
	{
		for (b = 8; b > 0; b--)
		{
			c = ((data[i] >> (b - 1)) & 0x01) + 0x30;
			dapDebugString(&c);
		}

		dapDebugString(" ");
	}

	dapDebugString("\r\n");
}

void dapPrintByte(uint8_t c)
{
	uint32_t	b;
	char		d[2];

	d[1] = 0;

	for (b = 8; b > 0; b--)
	{
		d[0] = ((c >> (b - 1)) & 0x01) + 0x30;
		dapDebugString(d);
	}
}

void dapDumpSWJSequence(uint8_t* data)
{
	uint8_t offs = 0;
	uint8_t b = 0;
	uint32_t len;
	char c[2];

	len = *data;
	if (!len) len = 256;

	data++;
	c[1] = 0;

	while (len)
	{
		for (b = 8; b > 0; b--)
		{
			c[0] = ((data[offs] >> (b - 1)) & 0x01) + 0x30;
			dapDebugString(c);

			len--;
			if (!len)
				break;
		}

		offs++;
	}

	dapDebugString("\r\n");
}

void dapDumpJTAGSequence(uint8_t* data)
{
	dapDebugString("Count=");
	dapPrintByte(*data);

	dapDebugString(" Info=");
	dapPrintByte(*(data + 1));

	dapDebugString(" Data=");
	dapPrintByte(*(data + 2));

	dapDebugString("\r\n");
}

#endif /* DAPDEBUG */
