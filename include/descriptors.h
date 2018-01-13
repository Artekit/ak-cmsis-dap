/***************************************************************************//**
 * @file descriptors.h
 * @brief USB descriptors
 * @author Energy Micro AS
 * @version 1.01
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2012 Energy Micro AS, http://www.energymicro.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 4. The source and compiled code may only be used on Energy Micro "EFM32"
 *    microcontrollers and "EFR4" radios.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/* !TODO: change these names */
#define USB_MAX_IF_NUM          8		/*!< Max interface number used for building USBDL_Lib. DON'T CHANGE. */
#define USB_MAX_EP_NUM          5		/*!< Max number of EP used for building USBD ROM. DON'T CHANGE. */
#define USB_MAX_PACKET0         64		/*!< Max EP0 packet size used for building USBD ROM. DON'T CHANGE. */
#define USB_FS_MAX_BULK_PACKET  64		/*!< MAXP for FS bulk EPs used for building USBD ROM. DON'T CHANGE. */
#define USB_HS_MAX_BULK_PACKET  512		/*!< MAXP for HS bulk EPs used for building USBD ROM. DON'T CHANGE. */
#define USB_DFU_XFER_SIZE       2048	/*!< Max DFU transfer size used for building USBD ROM. DON'T CHANGE. */
	
static const uint8_t HID_ReportDescriptor[] __attribute__ ((aligned(4)))= {
    0x06, 0x00, 0xff,              // USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x40,                    //   REPORT_COUNT (64)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x40,                    //   REPORT_COUNT (64)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)
    0xc0                           // END_COLLECTION
};

/* Device Descriptor. Refer to the USB 2.0 Specification, chapter 9.6 */
EFM32_ALIGN(4)
static const USB_DeviceDescriptor_TypeDef deviceDesc __attribute__ ((aligned(4)))=
{
	.bLength            = USB_DEVICE_DESCSIZE,            /* Size of the Descriptor in Bytes */
	.bDescriptorType    = USB_DEVICE_DESCRIPTOR,          /* Device Descriptor type */
	.bcdUSB             = 0x0200,                         /* USB 2.0 compliant */
	.bDeviceClass       = 0x00,                           /* Vendor unique device */
	.bDeviceSubClass    = 0x00,                           /* Ignored for vendor unique device */            
	.bDeviceProtocol    = 0x00,                           /* Ignored for vendor unique device */
	.bMaxPacketSize0    = USB_MAX_PACKET0, 		          /* Max packet size for EP0 */

	/* Note: The usage of the following VID/PID was authorized by Silicon Laboratories
	 * to be used by Artekit Labs only. If you are deriving this project, please use
	 * your own VID/PID combination.
	 */

	.idVendor           = 0x10C4,                         /* Silabs VID */
	.idProduct          = 0x8B54,                         /* Artekit Labs AK-CMSIS-DAP PID */
	.bcdDevice          = 0x0100,                         /* Device Release number */
	.iManufacturer      = 1,                              /* Index of Manufacturer String Descriptor */
	.iProduct           = 2,                              /* Index of Product String Descriptor */
	.iSerialNumber      = 3,                              /* Index of Serial Number String Descriptor */
	.bNumConfigurations = 1                               /* Number of Possible Configurations */
};

/* This array contains the Configuration Descriptor and all
 * Interface and Endpoint Descriptors for the device.
 * Refer to the USB 2.0 Specification, chapter 9.6. */
EFM32_ALIGN(4)
static const uint8_t configDesc[] __attribute__ ((aligned(4)))=
{
	/*** Configuration descriptor ***/
	USB_CONFIG_DESCSIZE,                  /* bLength              */
	USB_CONFIG_DESCRIPTOR,                /* bDescriptorType      */

	USB_CONFIG_DESCSIZE +                 /* wTotalLength (LSB)   */
	USB_INTERFACE_DESCSIZE +
	USB_HID_DESCSIZE +
	(USB_ENDPOINT_DESCSIZE * NUM_EP_USED),

	(USB_CONFIG_DESCSIZE +                /* wTotalLength (MSB)   */
	USB_INTERFACE_DESCSIZE +
	USB_HID_DESCSIZE +
	(USB_ENDPOINT_DESCSIZE * NUM_EP_USED))>>8,

	1,                                    /* bNumInterfaces       */
	1,                                    /* bConfigurationValue  */
	0,                                    /* iConfiguration       */
	CONFIG_DESC_BM_RESERVED_D7 |          /* bmAttrib             */
	CONFIG_DESC_BM_SELFPOWERED,
	CONFIG_DESC_MAXPOWER_mA( 100 ),       /* bMaxPower: 100 mA    */

	/*** Interface descriptor, HID Class ***/
	USB_INTERFACE_DESCSIZE,               /* bLength              */
	USB_INTERFACE_DESCRIPTOR,             /* bDescriptorType      */
	0,                                    /* bInterfaceNumber     */
	0,                                    /* bAlternateSetting    */
	NUM_EP_USED,                          /* bNumEndpoints        */
	0x03,                                 /* bInterfaceClass (HID)*/
	0,                                    /* bInterfaceSubClass   */
	0,                                    /* bInterfaceProtocol   */
	0x04,                                 /* iInterface           */
	
	/*** HID Class descriptor ***/
	USB_HID_DESCSIZE,					/* bLength */
	USB_HID_DESCRIPTOR,					/* bDescriptorType */
	0x11,								/* bcdHID : 1.11*/
	0x01,
	0x00,								/* bCountryCode */
	0x01,								/* bNumDescriptors */
	USB_HID_REPORT_DESCRIPTOR,
	sizeof(HID_ReportDescriptor) & 0xFF,/* wDescriptorLength */
	(sizeof(HID_ReportDescriptor) >> 8) & 0xFF,
		
	/*** Endpoint, HID Interrupt In ***/
	USB_ENDPOINT_DESCSIZE,                /* bLength              */
	USB_ENDPOINT_DESCRIPTOR,              /* bDescriptorType      */
	EP_IN,                                /* bEndpointAddress     */
	USB_EPTYPE_INTR,            	      /* bmAttributes         */
	USB_FS_INTR_EP_MAXSIZE,               /* wMaxPacketSize (LSB) */
	0,                                    /* wMaxPacketSize (MSB) */
	0x01,                                 /* bInterval            */    

	/*** Endpoint, HID Interrupt Out ***/
	USB_ENDPOINT_DESCSIZE,                /* bLength              */
	USB_ENDPOINT_DESCRIPTOR,              /* bDescriptorType      */
	EP_OUT,                               /* bEndpointAddress     */
	USB_EPTYPE_INTR,        	          /* bmAttributes         */
	USB_FS_INTR_EP_MAXSIZE,               /* wMaxPacketSize (LSB) */
	0,                                    /* wMaxPacketSize (MSB) */
	0x01,                                 /* bInterval            */
	/* Terminator */
	0									  /* bLength */
};

/* Define the String Descriptor for the device. String must be properly
 * aligned and unicode encoded. The first element defines the language id. 
 * Here 0x04 = United States, 0x09 = English. 
 * Refer to the USB Language Identifiers documentation. */
STATIC_CONST_STRING_DESC_LANGID( langID, 0x04, 0x09 );
STATIC_CONST_STRING_DESC( iManufacturer, 'A','R','T','E','K','I','T',' ','L','A','B','S');
STATIC_CONST_STRING_DESC( iProduct     , 'A','K','-','C','M','S','I','S','-','D','A','P');

SL_PACK_START(1)
typedef struct
{
  uint8_t  len;
  uint8_t  type;
  char16_t name[33];
} SL_ATTRIBUTE_PACKED _iSerialNumber;
SL_PACK_END()

STATIC_CONST_STRING_DESC( iHID, 'H','I','D');
extern _iSerialNumber iSerialNumber;

static const void * const strings[] =
{
  &langID,
  &iManufacturer,
  &iProduct,
  &iSerialNumber,
  &iHID
};

#ifdef __cplusplus
}
#endif
