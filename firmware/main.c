/**
  ******************************************************************************
  * @file   app.c
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

#include "usbd_audio_core.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "stm324xg_eval.h"

#include <minilib.h>

volatile uint32_t clock = 0;

USB_OTG_CORE_HANDLE USB_OTG_dev;

USART_InitTypeDef debug = {
	230400*3, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, USART_Mode_Tx, USART_HardwareFlowControl_None
};

char LOG[1024] = { 0 };

void writestr(char *str) {
	strcat(LOG, str);
	while (*str) {
		while (!USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TXE));
		USART_SendData(EVAL_COM1, *str++);
	}
}

int main(void)
{
	int blip = 0;

	SystemInit();
	STM_EVAL_COMInit(COM1, &debug);

	if (SysTick_Config(SystemCoreClock / 100)) { 
		while (1);
	} 

	STM_EVAL_LEDInit(LED1);
	STM_EVAL_LEDInit(LED2);
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	while (clock < 25);
	STM_EVAL_LEDOn(LED1);
	while (clock < 50);
	STM_EVAL_LEDOn(LED3);
	while (clock < 75);
	STM_EVAL_LEDOn(LED4);
	while (clock < 100);
	STM_EVAL_LEDOn(LED2);
	writestr("Hello!\n");

	USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID);
	
	/* Main loop */
	while (1) {
		if (clock % 16 == 0) {
			if (!blip) {
				blip = 1;
				STM_EVAL_LEDToggle(LED1);
			}
		} else {
			blip = 0;
		}
	}
} 
