/**
  ******************************************************************************
  * @file    usbd_usr.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file includes the user application layer
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
#include "usbd_usr.h"
#include "usb_conf.h"
#include "stm324xg_eval.h"
#include "stm32f4xx_i2c.h"
#include "usbd_ioreq.h"
#include "stm324xg_usb_audio_codec.h"

void USBD_USR_Init(void) {  
  /* Initialize LEDs */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);   

  // XXX outputf("USB Device Init\n");
}

void USBD_USR_DeviceReset(uint8_t speed) {
  // XXX outputf("USB Device Reset\n");
}

void USBD_USR_DeviceConfigured (void) {
  // XXX outputf("USB Device Configured\n");
}

void USBD_USR_DeviceSuspended(void) {
  // XXX outputf("USB Device Suspended\n");
}

void USBD_USR_DeviceResumed(void) {
  // XXX outputf("USB Device Resumed\n");
}

void USBD_USR_DeviceConnected (void) {
  // XXX outputf("USB Device Connected\n");
}

void USBD_USR_DeviceDisconnected (void) {
  // XXX outputf("USB Device Disconnected\n");
}

void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size)
{
}

/**
  * @brief  Codec_TIMEOUT_UserCallback
  * @param  None
  * @retval None
  */
extern void writestr(char *s);
uint32_t Codec_TIMEOUT_UserCallback(void)
{
  I2C_InitTypeDef I2C_InitStructure;
  writestr("TIMEOUT ");

  I2C_GenerateSTOP(I2C1, ENABLE);
  I2C_SoftwareResetCmd(I2C1, ENABLE);
  I2C_SoftwareResetCmd(I2C1, DISABLE);
    
  I2C_DeInit(I2C1);
  
  /* CODEC_I2C peripheral configuration */
  I2C_DeInit(I2C1);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x33;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
  /* Enable the I2C peripheral */
  I2C_Cmd(I2C1, ENABLE);  
  I2C_Init(I2C1, &I2C_InitStructure);  
  
  return 0;
}
