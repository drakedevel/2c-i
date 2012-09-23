/**
  ******************************************************************************
  * @file    stm32fxxx_it.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides all exceptions handler and peripherals interrupt
  *          service routine.
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
#include "stm32fxxx_it.h"
#include "stm324xg_usb_audio_codec.h"
#include "usb_core.h"
#include "usbd_core.h"
#include "usbd_audio_core.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
extern USB_OTG_CORE_HANDLE           USB_OTG_dev;
extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
extern uint32_t USBD_OTG_EP1IN_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
extern uint32_t USBD_OTG_EP1OUT_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
#endif

extern AUDIO_FOPS_TypeDef  AUDIO_OUT_fops;

uint32_t PrevOutput = OUTPUT_DEVICE_HEADPHONE;
uint32_t OutputState = OUTPUT_DEVICE_HEADPHONE;

/******************************************************************************/
/*             Cortex-M Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  if (OutputState != PrevOutput)
  {
    Codec_SwitchOutput(OutputState);
    PrevOutput = OutputState;
  }
}

/**
  * @brief  This function handles OTG_FS_WKUP Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_FS
void OTG_FS_WKUP_IRQHandler(void)
{
  if(USB_OTG_dev.cfg.low_power)
  {
    *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ; 
    SystemInit();
    USB_OTG_UngateClock(&USB_OTG_dev);
  }
  EXTI_ClearITPendingBit(EXTI_Line18);
}
#endif /* USE_USB_OTG_FS */

/**
  * @brief  This function handles OTG_HS_WKUP Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_HS
void OTG_HS_WKUP_IRQHandler(void)
{
  if(USB_OTG_dev.cfg.low_power)
  {
    *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ; 
    SystemInit();
    USB_OTG_UngateClock(&USB_OTG_dev);
  }
  EXTI_ClearITPendingBit(EXTI_Line20);
}
#endif /* USE_USB_OTG_HS */

/**
  * @brief  This function handles OTG HS or FS Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_HS
void OTG_HS_IRQHandler(void)
#else
void OTG_FS_IRQHandler(void)
#endif /* USE_USB_OTG_HS */
{
  USBD_OTG_ISR_Handler (&USB_OTG_dev);
}

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
/**
  * @brief  This function handles EP1_IN Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_IN_IRQHandler(void)
{
  USBD_OTG_EP1IN_ISR_Handler (&USB_OTG_dev);
}

/**
  * @brief  This function handles EP1_OUT Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_OUT_IRQHandler(void)
{
  USBD_OTG_EP1OUT_ISR_Handler (&USB_OTG_dev);
}
#endif /* USB_OTG_HS_DEDICATED_EP1_ENABLED */

/**
  * @brief  This function handles EXTI15_10 or EXTI9_5 interrupt request.
  * @param  None
  * @retval None
  */
#if defined (USE_STM322xG_EVAL)
void EXTI15_10_IRQHandler(void)
#elif defined(USE_STM324xG_EVAL)
void EXTI15_10_IRQHandler(void)
#elif defined (USE_STM3210C_EVAL)
void EXTI9_5_IRQHandler(void)
#endif /* USE_STM322xG_EVAL */
{
  static uint8_t FallingEdge = 0;
  
  /* Check if the interrupt source is the KEY EXTI line */
  if (EXTI_GetFlagStatus(KEY_BUTTON_EXTI_LINE) == SET)
  {
    /* Check if the push button is pushed or released (to ignore the button released event) */
    if (FallingEdge == 0)
    {
      /* Switch the output target between Speaker and Headphone */
      OutputState = (OutputState == OUTPUT_DEVICE_SPEAKER)? \
                    OUTPUT_DEVICE_HEADPHONE : OUTPUT_DEVICE_SPEAKER;
    
    /* Increment the variable to indicate that next interrupt corresponds to push
    button released event: Release event should be ignored */
    FallingEdge++;
    }
    else
    {
      /* Reset the variable to indicate that the next interrupt corresponds to push
      button pushed event. */
      FallingEdge = 0;
    }
    
    /* Clear the interrupt line flag */
    EXTI_ClearITPendingBit(KEY_BUTTON_EXTI_LINE);
  }
}
