/**
  ******************************************************************************
  * @file    stm324xg_eval.h
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    05-March-2012
  * @brief   This file contains definitions for STM324xG_EVAL's Leds, push-buttons
  *          and COM ports hardware resources.
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM324xG_EVAL_H
#define __STM324xG_EVAL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32_eval_legacy.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
   
typedef enum 
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
  LED4 = 3
} Led_TypeDef;

typedef enum 
{
  COM1 = 0,
  COM2 = 1
} COM_TypeDef;

/** @addtogroup STM324xG_EVAL_LOW_LEVEL_LED
  * @{
  */
#define LED_GPIO_PORT                   GPIOD
#define LED_GPIO_CLK                    RCC_AHB1Periph_GPIOD 
#define LED1_PIN                        GPIO_Pin_13
#define LED2_PIN                        GPIO_Pin_12
#define LED3_PIN                        GPIO_Pin_14
#define LED4_PIN                        GPIO_Pin_15

static const uint16_t GPIO_PIN[] = {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN};

static inline void STM_EVAL_LEDInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* Enable the GPIO_LED Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
}

static inline void STM_EVAL_LEDOn(Led_TypeDef Led) {
  LED_GPIO_PORT->BSRRL = GPIO_PIN[Led];
}

static inline void STM_EVAL_LEDOff(Led_TypeDef Led) {
  LED_GPIO_PORT->BSRRH = GPIO_PIN[Led];
}

static inline void STM_EVAL_LEDToggle(Led_TypeDef Led) {
  LED_GPIO_PORT->ODR ^= GPIO_PIN[Led];
}

/**
 * @brief Definition for COM port1, connected to USART3
 */ 
#define EVAL_COM1                        USART3
#define EVAL_COM1_CLK                    RCC_APB1Periph_USART3
#define EVAL_COM1_TX_PIN                 GPIO_Pin_8
#define EVAL_COM1_TX_GPIO_PORT           GPIOD
#define EVAL_COM1_TX_GPIO_CLK            RCC_AHB1Periph_GPIOD
#define EVAL_COM1_TX_SOURCE              GPIO_PinSource8
#define EVAL_COM1_TX_AF                  GPIO_AF_USART3
#define EVAL_COM1_RX_PIN                 GPIO_Pin_11
#define EVAL_COM1_RX_GPIO_PORT           GPIOC
#define EVAL_COM1_RX_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define EVAL_COM1_RX_SOURCE              GPIO_PinSource11
#define EVAL_COM1_RX_AF                  GPIO_AF_USART3
#define EVAL_COM1_IRQn                   USART3_IRQn

static inline void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(EVAL_COM1_TX_GPIO_CLK, ENABLE);
  RCC_APB1PeriphClockCmd(EVAL_COM1_CLK, ENABLE);

  GPIO_PinAFConfig(EVAL_COM1_TX_GPIO_PORT, EVAL_COM1_TX_SOURCE, EVAL_COM1_TX_AF);

  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = EVAL_COM1_TX_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(EVAL_COM1_TX_GPIO_PORT, &GPIO_InitStructure);

  USART_Init(EVAL_COM1, USART_InitStruct);
  USART_Cmd(EVAL_COM1, ENABLE);
}


void STM_EVAL_LEDInit(Led_TypeDef Led);
void STM_EVAL_LEDOn(Led_TypeDef Led);
void STM_EVAL_LEDOff(Led_TypeDef Led);
void STM_EVAL_LEDToggle(Led_TypeDef Led);
void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct); 
#endif /* __STM324xG_EVAL_H */
