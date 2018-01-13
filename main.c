/***************************************************************************
 * Artekit AK-CMSIS-DAP JTAG
 * https://www.artekit.eu/products/debug/ak-cmsis-dap
 *
 * Written by Ruben Meleca
 * Copyright (c) 2018 Artekit Labs
 * https://www.artekit.eu

### main.c

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

#include <string.h>
#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_usb.h"
#include "em_gpio.h"
#include "em_emu.h"
#include "descriptors.h"
#include "DAP_config.h"
#include "DAP.h"
#include "defs.h"
#include "dap_debug.h"

#define CMSIS_DAP_MAX_PACKETS	DAP_PACKET_COUNT
#define CMSIS_DAP_PACKET_SIZE	DAP_PACKET_SIZE

static uint32_t u32LedCount;
static uint8_t u8LedToggle;

uint32_t u32LedDelay = 1000;
uint8_t	u8LedMode = DAP_LEDMODE_0;

_iSerialNumber iSerialNumber;

/* Structure definition for incoming/outgoing packets */
typedef struct __cmsis_dap_buffer
{
	uint8_t data[CMSIS_DAP_MAX_PACKETS][CMSIS_DAP_PACKET_SIZE];
	volatile uint8_t rdidx;		/* Read index */
	volatile uint8_t wridx;		/* Write index */
	volatile uint8_t count;		/* Packet count */
} CMSIS_DAP_BUFFER;

/* Our main USB packet buffers */
CMSIS_DAP_BUFFER incoming __attribute__ ((aligned(4)));
CMSIS_DAP_BUFFER outgoing __attribute__ ((aligned(4)));
uint8_t rx_buffer[CMSIS_DAP_PACKET_SIZE] __attribute__ ((aligned(4)));

/* TX busy flag */
volatile uint8_t tx_busy = 0;

/* Pointer to HID descriptor within configuration descriptor */
const void *hid_desc_ptr = (void*) &configDesc[ USB_CONFIG_DESCSIZE + USB_INTERFACE_DESCSIZE ];

/* Forward data_sent_callback declaration */
int data_sent_callback(USB_Status_TypeDef status, uint32_t xferred, uint32_t remaining);

static const uint8_t buff_multiplier[NUM_EP_USED + 1] = 
{ 
	1,  /* Control */
	1,  /* Interrupt */
	1   /* Interrupt */
};

static void transmit_next(CMSIS_DAP_BUFFER* buffer)
{
	uint8_t* data;

	if (outgoing.count)
	{
		data = buffer->data[buffer->rdidx];
		buffer->rdidx = (buffer->rdidx + 1) % CMSIS_DAP_MAX_PACKETS;

		__disable_irq();
		buffer->count--;
		__enable_irq();

		USBD_Write(EP_IN, data, CMSIS_DAP_PACKET_SIZE, data_sent_callback);
	}
}

int data_sent_callback(USB_Status_TypeDef status, uint32_t xferred, uint32_t remaining)
{
	if ( status == USB_STATUS_OK )
	{
		if (outgoing.count == 0)
		{
			tx_busy = 0;
		} else {
			transmit_next(&outgoing);
		}
	}
	
	return USB_STATUS_OK;
}

int data_received_callback(USB_Status_TypeDef status, uint32_t xferred, uint32_t remaining)
{
	/* Check status to verify that the transfer has completed successfully */
	if ( status == USB_STATUS_OK )
	{
		if (incoming.count < CMSIS_DAP_MAX_PACKETS)
		{
			memcpy(incoming.data[incoming.wridx], rx_buffer, xferred);
			incoming.wridx = (incoming.wridx + 1) % CMSIS_DAP_MAX_PACKETS;
			incoming.count++;
			
			/* Read again */
			USBD_Read(EP_OUT, rx_buffer, CMSIS_DAP_PACKET_SIZE, data_received_callback);
		}		
	}
	else
	{
		/* Handle errors here.  */
	}

	return USB_STATUS_OK;
}

void state_change_callback(USBD_State_TypeDef oldState, USBD_State_TypeDef newState)
{
	if (newState == USBD_STATE_CONFIGURED)
	{
		/* Start waiting for the 'tick' messages */
		USBD_Read(EP_OUT, rx_buffer, CMSIS_DAP_PACKET_SIZE, data_received_callback);
	}
	else if ( newState != USBD_STATE_SUSPENDED )
	{

	}
}

int setup_cmd_callback( const USB_Setup_TypeDef *setup )
{
	STATIC_UBUF( hidDesc, USB_HID_DESCSIZE );

	int retVal = USB_STATUS_REQ_UNHANDLED;

	if (( setup->Type      == USB_SETUP_TYPE_STANDARD       ) &&
		( setup->Direction == USB_SETUP_DIR_IN              ) &&
		( setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE ))
	{
		/* A HID device must extend the standard GET_DESCRIPTOR command */
		/* with support for HID descriptors.                            */
		switch (setup->bRequest)
		{
			case GET_DESCRIPTOR:
				if ( ( setup->wValue >> 8 ) == USB_HID_REPORT_DESCRIPTOR )
				{
					USBD_Write( 0, (void*) HID_ReportDescriptor, EFM32_MIN(sizeof(HID_ReportDescriptor), setup->wLength), NULL );
					retVal = USB_STATUS_OK;
				}
				else if ( ( setup->wValue >> 8 ) == USB_HID_DESCRIPTOR )
				{
					/* The HID descriptor might be misaligned ! */
					memcpy( hidDesc, hid_desc_ptr, USB_HID_DESCSIZE );
					USBD_Write( 0, hidDesc, EFM32_MIN(USB_HID_DESCSIZE, setup->wLength), NULL );
					retVal = USB_STATUS_OK;
				}
				break;
		}
	}

	else if (( setup->Type == USB_SETUP_TYPE_CLASS) &&
			(setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE) &&
			(setup->wIndex == DAP_INTERFACE_NO))
	{
		/* Implement the necessary HID class specific commands */
		switch ( setup->bRequest )
		{
			case USB_HID_SET_REPORT:
				retVal = USB_STATUS_OK;
				break;

			case USB_HID_GET_REPORT:
				retVal = USB_STATUS_OK;
				break;

			case USB_HID_SET_IDLE:
				if (((setup->wValue & 0xFF) == 0) &&  /* Report ID     */
					(setup->wLength == 0) &&
					(setup->Direction != USB_SETUP_DIR_IN))
					{
						retVal = USB_STATUS_OK;
					}
					break;

			case USB_HID_GET_IDLE:
				if ((setup->wValue == 0 ) &&  /* Report ID     */
					(setup->wLength == 1 ) &&
					(setup->Direction == USB_SETUP_DIR_IN))
				{
					uint8_t tmpBuffer = 0;
					USBD_Write(0, &tmpBuffer, 1, NULL );
					retVal = USB_STATUS_OK;
				}
				break;
		}
	}

	return retVal;
}

/* Define callbacks that are called by the USB stack on different events. */
static const USBD_Callbacks_TypeDef callbacks =
{
	.usbReset        = NULL,
	.usbStateChange  = state_change_callback,
	.setupCmd        = setup_cmd_callback,
	.isSelfPowered   = NULL,
	.sofInt          = NULL
};

/* Fill the init struct. This struct is passed to USBD_Init() in order 
 * to initialize the USB Stack */
static const USBD_Init_TypeDef initstruct =
{
	.deviceDescriptor    = &deviceDesc,
	.configDescriptor    = configDesc,
	.stringDescriptors   = strings,
	.numberOfStrings     = sizeof(strings)/sizeof(void*),
	.callbacks           = &callbacks,
	.bufferingMultiplier = buff_multiplier,
	.reserved            = 0
};

void SysTick_Handler(void)
{
	switch(u8LedMode)
	{
		case DAP_LEDMODE_0:
			break;
		case DAP_LEDMODE_1:
		case DAP_LEDMODE_2:
			if (++u32LedCount >= u32LedDelay)
			{
				if (!u8LedToggle)
				{
					u8LedToggle = 1;
					GPIO_PinOutSet(DAP_LED_PORT, DAP_LED_PIN);
				} else {
					u8LedToggle = 0;
					GPIO_PinOutClear(DAP_LED_PORT, DAP_LED_PIN);
				}
				u32LedCount = 0;
			}
			break;
	}
}

void gpio_init(void)
{
	/* Enable clock for IO pins */
	CMU_ClockEnable(cmuClock_GPIO, true);

	/* Set pin mode for JTAG signals */
	GPIO_PinModeSet(DAP_TCK_SWDCLK_PORT, DAP_TCK_SWDCLK_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(DAP_TMS_SWDIO_PORT, DAP_TMS_SWDIO_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(DAP_TDO_PORT, DAP_TDO_PIN, gpioModeInput, 0);
	GPIO_PinModeSet(DAP_TDI_PORT, DAP_TDI_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(DAP_TRST_PORT, DAP_TRST_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(DAP_BUFREV_PORT, DAP_BUFREV_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(DAP_RES_INOUT_PORT, DAP_RES_INOUT_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(DAP_BOOT_PORT, DAP_BOOT_PIN, gpioModeInput, 0);
	GPIO_PinModeSet(DAP_LED_PORT, DAP_LED_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(DAP_TXD_PORT, DAP_TXD_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(DAP_RXD_PORT, DAP_RXD_PIN, gpioModeInput, 0);
}

static void ToUnicode(char16_t* dst, uint8_t* src, uint32_t len)
{
	uint8_t i;
	uint8_t offs = 0;
	uint8_t* ptr = (uint8_t*) dst;
	uint8_t val;
	ptr[offs++] = 'A';
	ptr[offs++] = 0;
	ptr[offs++] = 'K';
	ptr[offs++] = 0;

	for (i = 0; i < len / 2; i++)
	{
		val = *src >> 4;
		if (val <= 9)
		{
			val += 0x30;
		} else {
			val += 0x37;
		}
		ptr[offs++] = val;
		
		ptr[offs++] = 0;
			
		val = *src & 0x0F;
		if (val <= 9)
		{
			val += 0x30;
		} else {
			val += 0x37;
		}
			
		ptr[offs++] =  val;
		ptr[offs++] =  0;
		src++;		
	}
	
	dst[len] = 0;
}

static void GetSerialNumber()
{
	uint8_t* unique_id = (uint8_t*) 0x0FE081F0;
	
	ToUnicode(iSerialNumber.name, unique_id, 8);
	iSerialNumber.len = sizeof(_iSerialNumber) - 2;
	iSerialNumber.type = USB_STRING_DESCRIPTOR;
}

int main(void)
{
	/* Chip errata */
	CHIP_Init();
	
	/* Use USHFRCO as main clock at 24MHz */
	CMU_USHFRCOBandSet(cmuUSHFRCOBand_24MHz);
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_USHFRCODIV2);
	SystemCoreClockGet();
	SysTick_Config(24000000 / 1000);
	
	GetSerialNumber();

	/* Set up GPIO */
	gpio_init();

	/* Initialize debug */
	DEBUG_INIT();

	memset(&incoming, 0, sizeof(incoming));
	memset(&outgoing, 0, sizeof(outgoing));

	/* Start USB stack. Callback routines in callbacks.c will be called
	* when connected to a host.  */
	USBD_Init(&initstruct);;
	
	SystemCoreClockGet();

	/*
	* When using a debugger it is practical to uncomment the following three
	* lines to force host to re-enumerate the device.
	*/
	USBD_Disconnect();
	USBTIMER_DelayMs( 1000 );
	USBD_Connect();

	while(1)
	{
		if (!incoming.count)
			continue;

    	DAP_ProcessCommand(incoming.data[incoming.rdidx], outgoing.data[outgoing.wridx]);
		incoming.rdidx = (incoming.rdidx + 1) % CMSIS_DAP_MAX_PACKETS;
		outgoing.wridx = (outgoing.wridx + 1) % CMSIS_DAP_MAX_PACKETS;
		
		__disable_irq();
		incoming.count--;
		outgoing.count++;
		__enable_irq();

		if (!tx_busy)
		{
			tx_busy = 1;
			transmit_next(&outgoing);
		}
	}
}
