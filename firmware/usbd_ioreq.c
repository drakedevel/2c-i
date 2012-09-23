/**
  ******************************************************************************
  * @file    usbd_ioreq.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides the IO requests APIs for control endpoints.
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

#include "usbd_ioreq.h"

/**
* @brief  USBD_CtlSendData
*         send data on the ctl pipe
* @param  pdev: device instance
* @param  buff: pointer to data buffer
* @param  len: length of data to be sent
*/
void USBD_CtlSendData (USB_OTG_CORE_HANDLE *pdev, uint8_t *pbuf, uint16_t len)
{
  pdev->dev.in_ep[0].total_data_len = len;
  pdev->dev.in_ep[0].rem_data_len   = len;
  pdev->dev.device_state = USB_OTG_EP0_DATA_IN;
  DCD_EP_Tx (pdev, 0, pbuf, len);
}

/**
* @brief  USBD_CtlContinueSendData
*         continue sending data on the ctl pipe
* @param  pdev: device instance
* @param  buff: pointer to data buffer
* @param  len: length of data to be sent
* @retval status
*/
void USBD_CtlContinueSendData (USB_OTG_CORE_HANDLE *pdev, uint8_t *pbuf, uint16_t len)
{
  DCD_EP_Tx (pdev, 0, pbuf, len);
}

/**
* @brief  USBD_CtlPrepareRx
*         receive data on the ctl pipe
* @param  pdev: USB OTG device instance
* @param  buff: pointer to data buffer
* @param  len: length of data to be received
* @retval status
*/
void USBD_CtlPrepareRx (USB_OTG_CORE_HANDLE *pdev, uint8_t *pbuf, uint16_t len)
{
  pdev->dev.out_ep[0].total_data_len = len;
  pdev->dev.out_ep[0].rem_data_len   = len;
  pdev->dev.device_state = USB_OTG_EP0_DATA_OUT;
  DCD_EP_PrepareRx (pdev, 0, pbuf, len);
}

/**
* @brief  USBD_CtlContinueRx
*         continue receive data on the ctl pipe
* @param  pdev: USB OTG device instance
* @param  buff: pointer to data buffer
* @param  len: length of data to be received
* @retval status
*/
void USBD_CtlContinueRx (USB_OTG_CORE_HANDLE *pdev, uint8_t *pbuf, uint16_t len)
{
  DCD_EP_PrepareRx (pdev, 0, pbuf, len);
}

/**
* @brief  USBD_CtlSendStatus
*         send zero lzngth packet on the ctl pipe
* @param  pdev: USB OTG device instance
* @retval status
*/
void USBD_CtlSendStatus (USB_OTG_CORE_HANDLE *pdev)
{
  pdev->dev.device_state = USB_OTG_EP0_STATUS_IN;
  DCD_EP_Tx (pdev, 0, NULL, 0); 
  USB_OTG_EP0_OutStart(pdev);  
}

/**
* @brief  USBD_CtlReceiveStatus
*         receive zero lzngth packet on the ctl pipe
* @param  pdev: USB OTG device instance
* @retval status
*/
void USBD_CtlReceiveStatus (USB_OTG_CORE_HANDLE *pdev)
{
  pdev->dev.device_state = USB_OTG_EP0_STATUS_OUT;  
  DCD_EP_PrepareRx ( pdev, 0, NULL, 0);  
  USB_OTG_EP0_OutStart(pdev);
}

/**
* @brief  USBD_GetRxCount
*         returns the received data length
* @param  pdev: USB OTG device instance
*         epnum: endpoint index
* @retval Rx Data blength
*/
uint16_t  USBD_GetRxCount (USB_OTG_CORE_HANDLE *pdev , uint8_t epnum)
{
  return pdev->dev.out_ep[epnum].xfer_count;
}

