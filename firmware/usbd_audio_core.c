/**
  ******************************************************************************
  * @file    usbd_audio_core.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides the high layer firmware functions to manage the 
  *          following functionalities of the USB Audio Class:
  *           - Initialization and Configuration of high and low layer
  *           - Enumeration as Audio Streaming Device
  *           - Audio Streaming data transfer
  *           - AudioControl requests management
  *           - Error management
  *           
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/

#include "usbd_audio_core.h"
#include "usbd_audio_out_if.h"
#include "stm324xg_eval.h"

/*********************************************
   AUDIO Requests management functions
 *********************************************/
static void AUDIO_Req_SetCurrent(void *pdev, USB_SETUP_REQ *req);

uint8_t  IsocOutBuff [TOTAL_OUT_BUF_SIZE * 2];
uint8_t* IsocOutWrPtr = IsocOutBuff;
uint8_t* IsocOutRdPtr = IsocOutBuff;

/* Main Buffer for Audio Control Rrequests transfers and its relative variables */
uint8_t  AudioCtl[64];
uint8_t  AudioCtlCmd = 0;
uint32_t AudioCtlLen = 0;
uint8_t  AudioCtlUnit = 0;

static uint32_t PlayFlag = 0;

static __IO uint32_t  usbd_audio_AltSet = 0;

/* USB AUDIO device Configuration Descriptor */
static uint8_t usbd_audio_CfgDesc[AUDIO_CONFIG_DESC_SIZE] =
{
  /* Configuration 1 */
  0x09,                                 /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE,    /* bDescriptorType */
  LOBYTE(AUDIO_CONFIG_DESC_SIZE),       /* wTotalLength  109 bytes*/
  HIBYTE(AUDIO_CONFIG_DESC_SIZE),      
  0x02,                                 /* bNumInterfaces */
  0x01,                                 /* bConfigurationValue */
  0x00,                                 /* iConfiguration */
  0xC0,                                 /* bmAttributes  BUS Powred*/
  0x32,                                 /* bMaxPower = 100 mA*/
  /* 09 byte*/
  
  /* USB Speaker Standard interface descriptor */
  AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  0x00,                                 /* bInterfaceNumber */
  0x00,                                 /* bAlternateSetting */
  0x00,                                 /* bNumEndpoints */
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
  AUDIO_SUBCLASS_AUDIOCONTROL,          /* bInterfaceSubClass */
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
  0x00,                                 /* iInterface */
  /* 09 byte*/
  
  /* USB Speaker Class-specific AC Interface Descriptor */
  AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_CONTROL_HEADER,                 /* bDescriptorSubtype */
  0x00,          /* 1.00 */             /* bcdADC */
  0x01,
  0x27,                                 /* wTotalLength = 39*/
  0x00,
  0x01,                                 /* bInCollection */
  0x01,                                 /* baInterfaceNr */
  /* 09 byte*/
  
  /* USB Speaker Input Terminal Descriptor */
  AUDIO_INPUT_TERMINAL_DESC_SIZE,       /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_CONTROL_INPUT_TERMINAL,         /* bDescriptorSubtype */
  0x01,                                 /* bTerminalID */
  0x01,                                 /* wTerminalType AUDIO_TERMINAL_USB_STREAMING   0x0101 */
  0x01,
  0x00,                                 /* bAssocTerminal */
  0x01,                                 /* bNrChannels */
  0x00,                                 /* wChannelConfig 0x0000  Mono */
  0x00,
  0x00,                                 /* iChannelNames */
  0x00,                                 /* iTerminal */
  /* 12 byte*/
  
  /* USB Speaker Audio Feature Unit Descriptor */
  0x09,                                 /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_CONTROL_FEATURE_UNIT,           /* bDescriptorSubtype */
  AUDIO_OUT_STREAMING_CTRL,             /* bUnitID */
  0x01,                                 /* bSourceID */
  0x01,                                 /* bControlSize */
  AUDIO_CONTROL_MUTE | AUDIO_CONTROL_VOLUME,               /* bmaControls(0) */
  0,       /* bmaControls(1) */
  0x00,                                 /* iTerminal */
  /* 09 byte*/
  
  /*USB Speaker Output Terminal Descriptor */
  0x09,      /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_CONTROL_OUTPUT_TERMINAL,        /* bDescriptorSubtype */
  0x03,                                 /* bTerminalID */
  0x01,                                 /* wTerminalType  0x0301*/
  0x03,
  0x00,                                 /* bAssocTerminal */
  0x02,                                 /* bSourceID */
  0x00,                                 /* iTerminal */
  /* 09 byte*/
  
  /* USB Speaker Standard AS Interface Descriptor - Audio Streaming Zero Bandwith */
  /* Interface 1, Alternate Setting 0                                             */
  AUDIO_INTERFACE_DESC_SIZE,  /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  0x01,                                 /* bInterfaceNumber */
  0x00,                                 /* bAlternateSetting */
  0x00,                                 /* bNumEndpoints */
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
  AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
  0x00,                                 /* iInterface */
  /* 09 byte*/
  
  /* USB Speaker Standard AS Interface Descriptor - Audio Streaming Operational */
  /* Interface 1, Alternate Setting 1                                           */
  AUDIO_INTERFACE_DESC_SIZE,  /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  0x01,                                 /* bInterfaceNumber */
  0x01,                                 /* bAlternateSetting */
  0x01,                                 /* bNumEndpoints */
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
  AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
  0x00,                                 /* iInterface */
  /* 09 byte*/
  
  /* USB Speaker Audio Streaming Interface Descriptor */
  AUDIO_STREAMING_INTERFACE_DESC_SIZE,  /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_STREAMING_GENERAL,              /* bDescriptorSubtype */
  0x01,                                 /* bTerminalLink */
  0x01,                                 /* bDelay */
  0x01,                                 /* wFormatTag AUDIO_FORMAT_PCM  0x0001*/
  0x00,
  /* 07 byte*/
  
  /* USB Speaker Audio Type III Format Interface Descriptor */
  0x0B,                                 /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_STREAMING_FORMAT_TYPE,          /* bDescriptorSubtype */
  AUDIO_FORMAT_TYPE_III,                /* bFormatType */ 
  0x02,                                 /* bNrChannels */
  0x02,                                 /* bSubFrameSize :  2 Bytes per frame (16bits) */
  16,                                   /* bBitResolution (16-bits per sample) */ 
  0x01,                                 /* bSamFreqType only one frequency supported */ 
  SAMPLE_FREQ(USBD_AUDIO_FREQ),         /* Audio sampling frequency coded on 3 bytes */
  /* 11 byte*/
  
  /* Endpoint 1 - Standard Descriptor */
  AUDIO_STANDARD_ENDPOINT_DESC_SIZE,    /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */
  AUDIO_OUT_EP,                         /* bEndpointAddress 1 out endpoint*/
  USB_ENDPOINT_TYPE_ISOCHRONOUS,        /* bmAttributes */
  AUDIO_PACKET_SZE(USBD_AUDIO_FREQ),    /* wMaxPacketSize in Bytes (Freq(Samples)*2(Stereo)*2(HalfWord)) */
  0x01,                                 /* bInterval */
  0x00,                                 /* bRefresh */
  0x00,                                 /* bSynchAddress */
  /* 09 byte*/
  
  /* Endpoint - Audio Streaming Descriptor*/
  AUDIO_STREAMING_ENDPOINT_DESC_SIZE,   /* bLength */
  AUDIO_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType */
  AUDIO_ENDPOINT_GENERAL,               /* bDescriptor */
  0x00,                                 /* bmAttributes */
  0x00,                                 /* bLockDelayUnits */
  0x00,                                 /* wLockDelay */
  0x00,
  /* 07 byte*/
} ;

/**
* @brief  USB_Class_Init
*         Initilaizes the AUDIO interface.
* @param  pdev: device instance
* @param  cfgidx: Configuration index
* @retval status
*/
uint8_t  USB_Class_Init (void  *pdev, 
                                 uint8_t cfgidx)
{  
  /* Open EP OUT */
  DCD_EP_Open(pdev, AUDIO_OUT_EP, AUDIO_OUT_PACKET, USB_OTG_EP_ISOC);

  /* Initialize the Audio output Hardware layer */
  if (AUDIO_OUT_fops.Init(USBD_AUDIO_FREQ, DEFAULT_VOLUME, 0) != USBD_OK)
    return USBD_FAIL;
 
  /* Prepare Out endpoint to receive audio data */
  DCD_EP_PrepareRx(pdev, AUDIO_OUT_EP,
                   (uint8_t*)IsocOutBuff, AUDIO_OUT_PACKET);  
  
  return USBD_OK;
}

/**
* @brief  USB_Class_Init
*         DeInitializes the AUDIO layer.
* @param  pdev: device instance
* @param  cfgidx: Configuration index
* @retval status
*/
uint8_t  USB_Class_DeInit (void  *pdev, 
                                   uint8_t cfgidx)
{ 
  DCD_EP_Close (pdev , AUDIO_OUT_EP);
  
  /* DeInitialize the Audio output Hardware layer */
  if (AUDIO_OUT_fops.DeInit(0) != USBD_OK)
  {
    return USBD_FAIL;
  }
  
  return USBD_OK;
}

/**
  * @brief  USB_Class_Setup
  *         Handles the Audio control request parsing.
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */

extern void writestr(char *s);
uint8_t handle_get_request(void *pdev, USB_SETUP_REQ *req) {
  uint8_t cs = req->wValue >> 8;
  uint8_t cn = req->wValue & 0xFF;

  if (cn != 0xFF && cn != 0) {
    /* Only one channel has controls... */
    USBD_CtlError (pdev, req);
    return USBD_FAIL;
  }

  if (cs == 1 && req->wLength == 1) {
    /* Not muted */
    AudioCtl[0] = 0;
    USBD_CtlSendData(pdev, AudioCtl, req->wLength);
    return USBD_OK;
  }

  if (cs != 2 || req->wLength != 2) {
    /* We only reply for the volume control... */
    char c[10];
    c[0] = 'b';
    btohex(c + 1, cs);
    btohex(c + 3, req->wLength);
    c[5] = '\n';
   c[6]  = 0;
    writestr(c); 
    USBD_CtlError (pdev, req);
    return USBD_FAIL;
  }

  if (req->bRequest == AUDIO_REQ_GET_CUR) {
  STM_EVAL_LEDToggle(LED2);
    AudioCtl[0] = 0x00;
    AudioCtl[1] = 0x00;
  } else if (req->bRequest == AUDIO_REQ_GET_RES) {
    AudioCtl[0] = 0x20;
    AudioCtl[1] = 0;
  } else if (req->bRequest == AUDIO_REQ_GET_MIN) {
    AudioCtl[0] = 0x20; 
    AudioCtl[1] = 0xC0;
  STM_EVAL_LEDToggle(LED3);
  } else if (req->bRequest == AUDIO_REQ_GET_MAX) {
    AudioCtl[0] = 0x00;
    AudioCtl[1] = 0x00;
  } else {
    writestr("?!\n"); 
    USBD_CtlError (pdev, req);
    return USBD_FAIL;
  }

  USBD_CtlSendData(pdev, AudioCtl, req->wLength);
  return USBD_OK;
}

uint8_t  USB_Class_Setup (void *pdev, USB_SETUP_REQ *req) {
  uint16_t len=USB_AUDIO_DESC_SIZ;
  uint8_t *pbuf=usbd_audio_CfgDesc + 18;
  
  switch (req->bmRequest & USB_REQ_TYPE_MASK) {
  case USB_REQ_TYPE_CLASS :    
    /* Audio class requests */
    switch (req->bRequest)
    {
    case AUDIO_REQ_SET_CUR:
      AUDIO_Req_SetCurrent(pdev, req);   
      break;

    case AUDIO_REQ_GET_CUR:
    case AUDIO_REQ_GET_MIN:
    case AUDIO_REQ_GET_MAX:
    case AUDIO_REQ_GET_RES:
      return handle_get_request(pdev, req);
    default:
      writestr("ucl\n");
      USBD_CtlError (pdev, req);
      return USBD_FAIL;
    }
    break;
    
    /* Standard Requests -------------------------------*/
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_DESCRIPTOR: 
      if( (req->wValue >> 8) == AUDIO_DESCRIPTOR_TYPE)
      {
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
        pbuf = usbd_audio_Desc;   
#else
        pbuf = usbd_audio_CfgDesc + 18;
#endif 
        len = MIN(USB_AUDIO_DESC_SIZ , req->wLength);
      }
      
      USBD_CtlSendData (pdev, pbuf, len);
      break;
      
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev, (uint8_t *)&usbd_audio_AltSet, 1);
      break;
      
    case USB_REQ_SET_INTERFACE :
      if ((uint8_t)(req->wValue) < AUDIO_TOTAL_IF_NUM)
      {
        usbd_audio_AltSet = (uint8_t)(req->wValue);
      }
      else
      {
        /* Call the error management function (command will be nacked */
        USBD_CtlError (pdev, req);
      }
      break;
    }
    break;
  default:
   { char c[4]; btohex(c, req->bmRequest & USB_REQ_TYPE_MASK); c[2] = ' '; c[3] = 0; writestr(c);
    writestr("uk\n");
}
  }
  return USBD_OK;
}

/**
  * @brief  USB_Class_EP0_RxReady
  *         Handles audio control requests data.
  * @param  pdev: device device instance
  * @retval status
  */
uint8_t  USB_Class_EP0_RxReady (void  *pdev)
{ 
  /* Check if an AudioControl request has been issued */
  if (AudioCtlCmd == AUDIO_REQ_SET_CUR)
  {/* In this driver, to simplify code, only SET_CUR request is managed */
    /* Check for which addressed unit the AudioControl request has been issued */
    if (AudioCtlUnit == AUDIO_OUT_STREAMING_CTRL)
    {/* In this driver, to simplify code, only one unit is manage */
      /* Call the audio interface mute function */
      AUDIO_OUT_fops.MuteCtl(AudioCtl[0]);
      
      /* Reset the AudioCtlCmd variable to prevent re-entering this function */
      AudioCtlCmd = 0;
      AudioCtlLen = 0;
    }
  } 
  
  return USBD_OK;
}

/**
  * @brief  USB_Class_DataIn
  *         Handles the audio IN data stage.
  * @param  pdev: instance
  * @param  epnum: endpoint number
  * @retval status
  */
uint8_t  USB_Class_DataIn (void *pdev, uint8_t epnum)
{
  return USBD_OK;
}

/**
  * @brief  USB_Class_DataOut
  *         Handles the Audio Out data stage.
  * @param  pdev: instance
  * @param  epnum: endpoint number
  * @retval status
  */
uint8_t  USB_Class_DataOut (void *pdev, uint8_t epnum)
{     
  if (epnum == AUDIO_OUT_EP)
  {    
    /* Increment the Buffer pointer or roll it back when all buffers are full */
    if (IsocOutWrPtr >= (IsocOutBuff + (AUDIO_OUT_PACKET * OUT_PACKET_NUM)))
    {/* All buffers are full: roll back */
      IsocOutWrPtr = IsocOutBuff;
    }
    else
    {/* Increment the buffer pointer */
      IsocOutWrPtr += AUDIO_OUT_PACKET;
    }
    
    /* Toggle the frame index */  
    ((USB_OTG_CORE_HANDLE*)pdev)->dev.out_ep[epnum].even_odd_frame = 
      (((USB_OTG_CORE_HANDLE*)pdev)->dev.out_ep[epnum].even_odd_frame)? 0:1;
      
    /* Prepare Out endpoint to receive next audio packet */
    DCD_EP_PrepareRx(pdev,
                     AUDIO_OUT_EP,
                     (uint8_t*)(IsocOutWrPtr),
                     AUDIO_OUT_PACKET);
      
    /* Trigger the start of streaming only when half buffer is full */
    if ((PlayFlag == 0) && (IsocOutWrPtr >= (IsocOutBuff + ((AUDIO_OUT_PACKET * OUT_PACKET_NUM) / 2))))
    {
      /* Enable start of Streaming */
      PlayFlag = 1;
    }
  }
  
  return USBD_OK;
}

/**
  * @brief  USB_Class_SOF
  *         Handles the SOF event (data buffer update and synchronization).
  * @param  pdev: instance
  * @param  epnum: endpoint number
  * @retval status
  */
uint8_t  USB_Class_SOF (void *pdev)
{     
  /* Check if there are available data in stream buffer.
    In this function, a single variable (PlayFlag) is used to avoid software delays.
    The play operation must be executed as soon as possible after the SOF detection. */
  if (PlayFlag)
  {      
    /* Start playing received packet */
    AUDIO_OUT_fops.AudioCmd((uint8_t*)(IsocOutRdPtr),  /* Samples buffer pointer */
                            AUDIO_OUT_PACKET,          /* Number of samples in Bytes */
                            AUDIO_CMD_PLAY);           /* Command to be processed */
    
    /* Increment the Buffer pointer or roll it back when all buffers all full */  
    if (IsocOutRdPtr >= (IsocOutBuff + (AUDIO_OUT_PACKET * OUT_PACKET_NUM)))
    {/* Roll back to the start of buffer */
      IsocOutRdPtr = IsocOutBuff;
    }
    else
    {/* Increment to the next sub-buffer */
      IsocOutRdPtr += AUDIO_OUT_PACKET;
    }
    
    /* If all available buffers have been consumed, stop playing */
    if (IsocOutRdPtr == IsocOutWrPtr)
    {    
      /* Pause the audio stream */
      AUDIO_OUT_fops.AudioCmd((uint8_t*)(IsocOutBuff),   /* Samples buffer pointer */
                              AUDIO_OUT_PACKET,          /* Number of samples in Bytes */
                              AUDIO_CMD_PAUSE);          /* Command to be processed */
      
      /* Stop entering play loop */
      PlayFlag = 0;
      
      /* Reset buffer pointers */
      IsocOutRdPtr = IsocOutBuff;
      IsocOutWrPtr = IsocOutBuff;
    }
  }
  
  return USBD_OK;
}

/**
  * @brief  USB_Class_IsoOUTIncomplete
  *         Handles the iso out incomplete event.
  * @param  pdev: instance
  * @retval status
  */
uint8_t  USB_Class_IsoOUTIncomplete (void  *pdev)
{
  return USBD_OK;
}

/******************************************************************************
     AUDIO Class requests management
******************************************************************************/

/**
  * @brief  AUDIO_Req_SetCurrent
  *         Handles the SET_CUR Audio control request.
  * @param  pdev: instance
  * @param  req: setup class request
  * @retval status
  */
static void AUDIO_Req_SetCurrent(void *pdev, USB_SETUP_REQ *req)
{ 
  if (req->wLength)
  {
    /* Prepare the reception of the buffer over EP0 */
    USBD_CtlPrepareRx (pdev, 
                       AudioCtl,
                       req->wLength);
    
    /* Set the global variables indicating current request and its length 
    to the function USB_Class_EP0_RxReady() which will process the request */
    AudioCtlCmd = AUDIO_REQ_SET_CUR;     /* Set the request value */
    AudioCtlLen = req->wLength;          /* Set the request data length */
    AudioCtlUnit = HIBYTE(req->wIndex);  /* Set the request target unit */
  }
}

/**
  * @brief  USB_Class_GetConfigDescriptor 
  *         Returns configuration descriptor.
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
uint8_t  *USB_Class_GetConfigDescriptor (uint8_t speed, uint16_t *length)
{
  *length = sizeof (usbd_audio_CfgDesc);
  return usbd_audio_CfgDesc;
}
uint8_t  *USB_Class_GetOtherConfigDescriptor (uint8_t speed, uint16_t *length) {
	return USB_Class_GetConfigDescriptor(speed, length);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/



uint8_t USB_Class_EP0_TxSent(void *pdev) { return 0; }
uint8_t USB_Class_IsoINIncomplete(void *pdev) { return 0; }
