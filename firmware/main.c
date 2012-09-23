/**
  ******************************************************************************
  * @file    app.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides all the Application firmware functions.
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
#include "usbd_usr.h"
#include "usb_conf.h"

#include "stm324xg_eval.h"

#pragma pack(4) 

USB_OTG_CORE_HANDLE           USB_OTG_dev;

int main(void)
{
  __IO uint32_t i = 0;
  SystemInit();

  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  STM_EVAL_LEDOn(LED1);
  STM_EVAL_LEDOn(LED2);
  STM_EVAL_LEDOn(LED3);
  STM_EVAL_LEDOn(LED4);

  USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID);

  /* Setup SysTick Timer for 10 msec interrupts 
     This interrupt is used to display the current state of the Audio Player and
     the output state (Speaker/Headphone) */
  if (SysTick_Config(SystemCoreClock / 100))
  { 
    /* Capture error */ 
    while (1);
  } 
  
  /* Main loop */
  while (1)
  {    
    if (i++ == 0x100000) {
      STM_EVAL_LEDToggle(LED1);
      i = 0;
    }
  }
} 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
