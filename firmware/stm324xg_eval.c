/**
  ******************************************************************************
  * @file    stm324xg_eval.c
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    05-March-2012
  * @brief   This file provides
  *            - set of firmware functions to manage Leds, push-button and COM ports
  *            - low level initialization functions for SD card (on SDIO) and
  *              serial EEPROM (sEE)
  *          available on STM324xG-EVAL evaluation board(MB786) RevB from
  *          STMicroelectronics.
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
#include "stm324xg_eval.h"
#include "stm32f4xx_sdio.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_i2c.h"

USART_TypeDef* COM_USART[COMn] = {EVAL_COM1}; 
GPIO_TypeDef* COM_TX_PORT[COMn] = {EVAL_COM1_TX_GPIO_PORT};
GPIO_TypeDef* COM_RX_PORT[COMn] = {EVAL_COM1_RX_GPIO_PORT};
const uint32_t COM_USART_CLK[COMn] = {EVAL_COM1_CLK};
const uint32_t COM_TX_PORT_CLK[COMn] = {EVAL_COM1_TX_GPIO_CLK};
const uint16_t COM_TX_PIN[COMn] = {EVAL_COM1_TX_PIN};
const uint16_t COM_TX_PIN_SOURCE[COMn] = {EVAL_COM1_TX_SOURCE};
const uint16_t COM_TX_AF[COMn] = {EVAL_COM1_TX_AF};

/**
  * @brief  Configures COM port.
  * @param  COM: Specifies the COM port to be configured.
  *   This parameter can be one of following parameters:    
  *     @arg COM1
  *     @arg COM2  
  * @param  USART_InitStruct: pointer to a USART_InitTypeDef structure that
  *   contains the configuration information for the specified USART peripheral.
  * @retval None
  */
void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(EVAL_COM1_TX_GPIO_CLK, ENABLE);
  RCC_APB1PeriphClockCmd(EVAL_COM1_CLK, ENABLE);

  GPIO_PinAFConfig(EVAL_COM1_TX_GPIO_PORT, EVAL_COM1_TX_SOURCE, EVAL_COM1_TX_AF);

  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(COM_TX_PORT, &GPIO_InitStructure);

  USART_Init(EVAL_COM1, USART_InitStruct);
  USART_Cmd(EVAL_COM1, ENABLE);
}

