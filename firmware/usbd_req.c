/**
  ******************************************************************************
  * @file    usbd_req.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides the standard USB requests following chapter 9.
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
#include "usbd_req.h"
#include "usbd_ioreq.h"
#include "usbd_desc.h"

extern __IO USB_OTG_DCTL_TypeDef SET_TEST_MODE;

uint32_t USBD_ep_status = 0;
uint32_t USBD_default_cfg = 0;
uint32_t USBD_cfg_status = 0;
uint8_t USBD_StrDesc[USB_MAX_STR_DESC_SIZ] ALIGNED(4);

#define USBD_VID                        0x0483
#define USBD_PID                        0x5730
#define USBD_LANGID_STRING              0x409
#pragma pack(4)

/* USB Standard Device Descriptor */
uint8_t USBD_DeviceDesc[USB_SIZ_DEVICE_DESC] =
  {
    0x12,                       /*bLength */
    USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
    0x00,                       /*bcdUSB */
    0x02,
    0x00,                       /*bDeviceClass*/
    0x00,                       /*bDeviceSubClass*/
    0x00,                       /*bDeviceProtocol*/
    USB_OTG_MAX_EP0_SIZE,       /*bMaxPacketSize*/
    LOBYTE(USBD_VID),           /*idVendor*/
    HIBYTE(USBD_VID),           /*idVendor*/
    LOBYTE(USBD_PID),           /*idVendor*/
    HIBYTE(USBD_PID),           /*idVendor*/
    0x00,                       /*bcdDevice rel. 2.00*/
    0x02,
    USBD_IDX_MFC_STR,           /*Index of manufacturer  string*/
    USBD_IDX_PRODUCT_STR,       /*Index of product string*/
    USBD_IDX_SERIAL_STR,        /*Index of serial number string*/
    USBD_CFG_MAX_NUM            /*bNumConfigurations*/
  }
  ; /* USB_DeviceDescriptor */

/* USB Standard Device Descriptor */
uint8_t USBD_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/* USB Standard Device Descriptor */
uint8_t USBD_LangIDDesc[USB_SIZ_STRING_LANGID] =
{
     USB_SIZ_STRING_LANGID,
     USB_DESC_TYPE_STRING,
     LOBYTE(USBD_LANGID_STRING),
     HIBYTE(USBD_LANGID_STRING),
};

/**
* @brief  USBD_StdItfReq
*         Handle standard usb interface requests
* @param  pdev: USB OTG device instance
* @param  req: usb request
* @retval status
*/
USBD_Status  USBD_StdItfReq (USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ  *req)
{
  USBD_Status ret = USBD_OK;

  switch (pdev->dev.device_status)
  {
  case USB_OTG_CONFIGURED:

    if (LOBYTE(req->wIndex) <= USBD_ITF_MAX_NUM)
    {
      USB_Class_Setup (pdev, req);

      if((req->wLength == 0)&& (ret == USBD_OK))
      {
         USBD_CtlSendStatus(pdev);
      }
    }
    else
    {
       USBD_CtlError(pdev, req);
    }
    break;

  default:
     USBD_CtlError(pdev, req);
    break;
  }
  return ret;
}

/**
* @brief  USBD_StdEPReq
*         Handle standard usb endpoint requests
* @param  pdev: USB OTG device instance
* @param  req: usb request
* @retval status
*/
USBD_Status  USBD_StdEPReq (USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ  *req)
{

  uint8_t   ep_addr;
  USBD_Status ret = USBD_OK;

  ep_addr  = LOBYTE(req->wIndex);

  switch (req->bRequest)
  {

  case USB_REQ_SET_FEATURE :

    switch (pdev->dev.device_status)
    {
    case USB_OTG_ADDRESSED:
      if ((ep_addr != 0x00) && (ep_addr != 0x80))
        DCD_EP_Stall(pdev, ep_addr);
      break;	

    case USB_OTG_CONFIGURED:
      if (req->wValue == USB_FEATURE_EP_HALT)
      {
        if ((ep_addr != 0x00) && (ep_addr != 0x80))
          DCD_EP_Stall(pdev, ep_addr);
      }
      USB_Class_Setup (pdev, req);
      USBD_CtlSendStatus(pdev);
      break;

    default:
      USBD_CtlError(pdev, req);
      break;
    }
    break;

  case USB_REQ_CLEAR_FEATURE :
    switch (pdev->dev.device_status)
    {
    case USB_OTG_ADDRESSED:
      if ((ep_addr != 0x00) && (ep_addr != 0x80))
        DCD_EP_Stall(pdev, ep_addr);
      break;	

    case USB_OTG_CONFIGURED:
      if (req->wValue == USB_FEATURE_EP_HALT)
      {
        if ((ep_addr != 0x00) && (ep_addr != 0x80))
        {
          DCD_EP_ClrStall(pdev, ep_addr);
          USB_Class_Setup (pdev, req);
        }
        USBD_CtlSendStatus(pdev);
      }
      break;

    default:
       USBD_CtlError(pdev, req);
      break;
    }
    break;

  case USB_REQ_GET_STATUS:
    switch (pdev->dev.device_status)
    {
    case USB_OTG_ADDRESSED:
      if ((ep_addr != 0x00) && (ep_addr != 0x80))
      {
        DCD_EP_Stall(pdev, ep_addr);
      }
      break;	

    case USB_OTG_CONFIGURED:

      if ((ep_addr & 0x80)== 0x80)
      {
        if(pdev->dev.in_ep[ep_addr & 0x7F].is_stall)
          USBD_ep_status = 0x0001;
        else
          USBD_ep_status = 0x0000;
      }
      else if ((ep_addr & 0x80)== 0x00)
      {
        if(pdev->dev.out_ep[ep_addr].is_stall)
          USBD_ep_status = 0x0001;
        else
          USBD_ep_status = 0x0000;
      }
      USBD_CtlSendData (pdev, (uint8_t *)&USBD_ep_status, 2);
      break;

    default:
       USBD_CtlError(pdev, req);
      break;
    }
    break;

  default:
    break;
  }
  return ret;
}
/**
* @brief  USBD_GetDescriptor
*         Handle Get Descriptor requests
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_GetDescriptor(USB_OTG_CORE_HANDLE  *pdev,
                               USB_SETUP_REQ *req)
{
  uint16_t len;
  uint8_t *pbuf;


  switch (req->wValue >> 8)
  {
  case USB_DESC_TYPE_DEVICE:
    len = sizeof(USBD_DeviceDesc);
    pbuf = USBD_DeviceDesc;
    if ((req->wLength == 64) ||( pdev->dev.device_status == USB_OTG_DEFAULT))
    {
      len = 8;
    }
    break;

  case USB_DESC_TYPE_CONFIGURATION:
      pbuf   = (uint8_t *)USB_Class_GetConfigDescriptor(pdev->cfg.speed, &len);
#ifdef USB_OTG_HS_CORE
    if((pdev->cfg.speed == USB_OTG_SPEED_FULL )&&
       (pdev->cfg.phy_itface  == USB_OTG_ULPI_PHY))
    {
      pbuf   = (uint8_t *)USB_Class_GetOtherConfigDescriptor(pdev->cfg.speed, &len);
    }
#endif
    pbuf[1] = USB_DESC_TYPE_CONFIGURATION;
    pdev->dev.pConfig_descriptor = pbuf;
    break;

  case USB_DESC_TYPE_STRING:
    switch ((uint8_t)(req->wValue))
    {
    case USBD_IDX_LANGID_STR:
      pbuf = USBD_LangIDDesc;
      len = sizeof(USBD_LangIDDesc);
      break;

    case USBD_IDX_MFC_STR:
      USBD_GetString("J4Labs", USBD_StrDesc, &len);
      pbuf = USBD_StrDesc;
      break;

    case USBD_IDX_PRODUCT_STR:
      USBD_GetString(pdev->cfg.speed ? "2C-I [FS]" : "2C-I [HS]", USBD_StrDesc, &len);
      pbuf = USBD_StrDesc;
      break;

    case USBD_IDX_SERIAL_STR:
      USBD_GetString(pdev->cfg.speed ? "000123" : "000124", USBD_StrDesc, &len);
      pbuf = USBD_StrDesc;
      break;

    case USBD_IDX_CONFIG_STR:
      USBD_GetString ("Audio Config", USBD_StrDesc, &len);
      pbuf = USBD_StrDesc;
      break;

    case USBD_IDX_INTERFACE_STR:
      USBD_GetString ("Audio Interface", USBD_StrDesc, &len);
      pbuf = USBD_StrDesc;
      break;

    default:
#ifdef USB_SUPPORT_USER_STRING_DESC
      pbuf = USB_Class_GetUsrStrDescriptor(pdev->cfg.speed, (req->wValue), &len);
      break;
#else
       USBD_CtlError(pdev, req);
      return;
#endif /* USBD_CtlError(pdev, req); */
    }
    break;
  case USB_DESC_TYPE_DEVICE_QUALIFIER:
#ifdef USB_OTG_HS_CORE
    if(pdev->cfg.speed == USB_OTG_SPEED_HIGH  )
    {

      pbuf   = (uint8_t *)USB_Class_GetConfigDescriptor(pdev->cfg.speed, &len);

      USBD_DeviceQualifierDesc[4]= pbuf[14];
      USBD_DeviceQualifierDesc[5]= pbuf[15];
      USBD_DeviceQualifierDesc[6]= pbuf[16];

      pbuf = USBD_DeviceQualifierDesc;
      len  = USB_LEN_DEV_QUALIFIER_DESC;
      break;
    }
    else
    {
      USBD_CtlError(pdev, req);
      return;
    }
#else
      USBD_CtlError(pdev, req);
      return;
#endif

  case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
#ifdef USB_OTG_HS_CORE

    if(pdev->cfg.speed == USB_OTG_SPEED_HIGH  )
    {
      pbuf   = (uint8_t *)USB_Class_GetOtherConfigDescriptor(pdev->cfg.speed, &len);
      pbuf[1] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
      break;
    }
    else
    {
      USBD_CtlError(pdev, req);
      return;
    }
#else
      USBD_CtlError(pdev, req);
      return;
#endif


  default:
     USBD_CtlError(pdev, req);
    return;
  }

  if((len != 0)&& (req->wLength != 0))
  {

    len = MIN(len, req->wLength);

    USBD_CtlSendData (pdev,
                      pbuf,
                      len);
  }

}

/**
* @brief  USBD_SetAddress
*         Set device address
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_SetAddress(USB_OTG_CORE_HANDLE  *pdev,
                            USB_SETUP_REQ *req)
{
  uint8_t  dev_addr;

  if ((req->wIndex == 0) && (req->wLength == 0))
  {
    dev_addr = (uint8_t)(req->wValue) & 0x7F;

    if (pdev->dev.device_status == USB_OTG_CONFIGURED)
    {
      USBD_CtlError(pdev, req);
    }
    else
    {
      pdev->dev.device_address = dev_addr;
      DCD_EP_SetAddress(pdev, dev_addr);
      USBD_CtlSendStatus(pdev);
      pdev->dev.device_status = (dev_addr == 0 ? USB_OTG_DEFAULT : USB_OTG_ADDRESSED);
    }
  }
  else
  {
     USBD_CtlError(pdev, req);
  }
}

/**
* @brief  USBD_SetConfig
*         Handle Set device configuration request
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_SetConfig(USB_OTG_CORE_HANDLE  *pdev,
                           USB_SETUP_REQ *req)
{

  static uint8_t  cfgidx;

  cfgidx = (uint8_t)(req->wValue);

  if (cfgidx > USBD_CFG_MAX_NUM )
  {
     USBD_CtlError(pdev, req);
  }
  else
  {
    switch (pdev->dev.device_status)
    {
    case USB_OTG_ADDRESSED:
      if (cfgidx)
      {                                			   							   							   				
        pdev->dev.device_config = cfgidx;
        pdev->dev.device_status = USB_OTG_CONFIGURED;
        USBD_SetCfg(pdev, cfgidx);
        USBD_CtlSendStatus(pdev);
      }
      else
      {
         USBD_CtlSendStatus(pdev);
      }
      break;

    case USB_OTG_CONFIGURED:
      if (cfgidx == 0)
      {
        pdev->dev.device_status = USB_OTG_ADDRESSED;
        pdev->dev.device_config = cfgidx;
        USBD_ClrCfg(pdev, cfgidx);
        USBD_CtlSendStatus(pdev);

      }
      else  if (cfgidx != pdev->dev.device_config)
      {
        /* Clear old configuration */
        USBD_ClrCfg(pdev, pdev->dev.device_config);

        /* set new configuration */
        pdev->dev.device_config = cfgidx;
        USBD_SetCfg(pdev, cfgidx);
        USBD_CtlSendStatus(pdev);
      }
      else
      {
        USBD_CtlSendStatus(pdev);
      }
      break;

    default:					
       USBD_CtlError(pdev, req);
      break;
    }
  }
}

/**
* @brief  USBD_GetConfig
*         Handle Get device configuration request
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_GetConfig(USB_OTG_CORE_HANDLE  *pdev,
                           USB_SETUP_REQ *req)
{

  if (req->wLength != 1)
  {
     USBD_CtlError(pdev, req);
  }
  else
  {
    switch (pdev->dev.device_status )
    {
    case USB_OTG_ADDRESSED:
      USBD_CtlSendData (pdev, (uint8_t *)&USBD_default_cfg, 1);
      break;

    case USB_OTG_CONFIGURED:
      USBD_CtlSendData (pdev, &pdev->dev.device_config, 1);
      break;

    default:
      USBD_CtlError(pdev, req);
      break;
    }
  }
}

/**
* @brief  USBD_GetStatus
*         Handle Get Status request
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_GetStatus(USB_OTG_CORE_HANDLE  *pdev,
                           USB_SETUP_REQ *req)
{


  switch (pdev->dev.device_status)
  {
  case USB_OTG_ADDRESSED:
  case USB_OTG_CONFIGURED:

#ifdef USBD_SELF_POWERED
    USBD_cfg_status = USB_CONFIG_SELF_POWERED;
#else
    USBD_cfg_status = 0x00;
#endif

    if (pdev->dev.DevRemoteWakeup)
    {
      USBD_cfg_status |= USB_CONFIG_REMOTE_WAKEUP;
    }

    USBD_CtlSendData (pdev,
                      (uint8_t *)&USBD_cfg_status,
                      2);
    break;

  default :
    USBD_CtlError(pdev, req);
    break;
  }
}


/**
* @brief  USBD_SetFeature
*         Handle Set device feature request
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_SetFeature(USB_OTG_CORE_HANDLE  *pdev,
                            USB_SETUP_REQ *req)
{

  USB_OTG_DCTL_TypeDef     dctl;
  uint8_t test_mode = 0;

  if (req->wValue == USB_FEATURE_REMOTE_WAKEUP)
  {
    pdev->dev.DevRemoteWakeup = 1;
    USB_Class_Setup (pdev, req);
    USBD_CtlSendStatus(pdev);
  }

  else if ((req->wValue == USB_FEATURE_TEST_MODE) &&
           ((req->wIndex & 0xFF) == 0))
  {
    dctl.d32 = USB_OTG_READ_REG32(&pdev->regs.DREGS->DCTL);

    test_mode = req->wIndex >> 8;
    switch (test_mode)
    {
    case 1: // TEST_J
      dctl.b.tstctl = 1;
      break;

    case 2: // TEST_K	
      dctl.b.tstctl = 2;
      break;

    case 3: // TEST_SE0_NAK
      dctl.b.tstctl = 3;
      break;

    case 4: // TEST_PACKET
      dctl.b.tstctl = 4;
      break;

    case 5: // TEST_FORCE_ENABLE
      dctl.b.tstctl = 5;
      break;
    }
    SET_TEST_MODE = dctl;
    pdev->dev.test_mode = 1;
    USBD_CtlSendStatus(pdev);
  }

}


/**
* @brief  USBD_ClrFeature
*         Handle clear device feature request
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_ClrFeature(USB_OTG_CORE_HANDLE  *pdev,
                            USB_SETUP_REQ *req)
{
  switch (pdev->dev.device_status)
  {
  case USB_OTG_ADDRESSED:
  case USB_OTG_CONFIGURED:
    if (req->wValue == USB_FEATURE_REMOTE_WAKEUP)
    {
      pdev->dev.DevRemoteWakeup = 0;
      USB_Class_Setup (pdev, req);
      USBD_CtlSendStatus(pdev);
    }
    break;

  default :
     USBD_CtlError(pdev, req);
    break;
  }
}

/**
* @brief  USBD_ParseSetupRequest
*         Copy buffer into setup structure
* @param  pdev: device instance
* @param  req: usb request
* @retval None
*/

void USBD_ParseSetupRequest( USB_OTG_CORE_HANDLE  *pdev,
                            USB_SETUP_REQ *req)
{
  req->bmRequest     = *(uint8_t *)  (pdev->dev.setup_packet);
  req->bRequest      = *(uint8_t *)  (pdev->dev.setup_packet +  1);
  req->wValue        = SWAPBYTE      (pdev->dev.setup_packet +  2);
  req->wIndex        = SWAPBYTE      (pdev->dev.setup_packet +  4);
  req->wLength       = SWAPBYTE      (pdev->dev.setup_packet +  6);

  pdev->dev.in_ep[0].ctl_data_len = req->wLength  ;
  pdev->dev.device_state = USB_OTG_EP0_SETUP;
}

/**
* @brief  USBD_CtlError
*         Handle USB low level Error
* @param  pdev: device instance
* @param  req: usb request
* @retval None
*/

void USBD_CtlError( USB_OTG_CORE_HANDLE  *pdev,
                            USB_SETUP_REQ *req)
{

  DCD_EP_Stall(pdev, 0x80);
  DCD_EP_Stall(pdev, 0);
  USB_OTG_EP0_OutStart(pdev);
}


/**
  * @brief  USBD_GetString
  *         Convert Ascii string into unicode one
  * @param  desc : descriptor buffer
  * @param  unicode : Formatted string buffer (unicode)
  * @param  len : descriptor length
  * @retval None
  */
void USBD_GetString(char *desc, uint8_t *unicode, uint16_t *len)
{
  uint8_t idx = 2;
  if (!desc) return;
  unicode[1] = USB_DESC_TYPE_STRING;
  while (*desc) {
    unicode[idx++] = *desc++;
    unicode[idx++] = 0;
  }
  *len = unicode[0] = idx;
}

/**
* @brief  USBD_StdDevReq
*         Handle standard usb device requests
* @param  pdev: device instance
* @param  req: usb request
*/
void USBD_StdDevReq (USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ  *req)
{
  switch (req->bRequest)
  {
  case USB_REQ_GET_DESCRIPTOR:
    USBD_GetDescriptor (pdev, req);
    break;

  case USB_REQ_SET_ADDRESS:
    USBD_SetAddress(pdev, req);
    break;

  case USB_REQ_SET_CONFIGURATION:
    USBD_SetConfig (pdev, req);
    break;

  case USB_REQ_GET_CONFIGURATION:
    USBD_GetConfig (pdev, req);
    break;

  case USB_REQ_GET_STATUS:
    USBD_GetStatus (pdev, req);
    break;

  case USB_REQ_SET_FEATURE:
    USBD_SetFeature (pdev, req);
    break;

  case USB_REQ_CLEAR_FEATURE:
    USBD_ClrFeature (pdev, req);
    break;

  default:
    USBD_CtlError(pdev, req);
    break;
  }
}
