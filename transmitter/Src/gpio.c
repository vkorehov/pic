/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

EXTI_HandleTypeDef hexti1;
EXTI_HandleTypeDef hexti7;
/** Pinout Configuration
*/
void MX_GPIO_Init(void)
{
  EXTI_ConfigTypeDef EXTI_ConfigStructure;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_SYSCFG_CLK_ENABLE();  
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  
  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = 0;  
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /*Configure GPIO pin : PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = 0;  
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Set configuration except Interrupt and Event mask of Exti line 1*/
  EXTI_ConfigStructure.Line = EXTI_LINE_1;
  EXTI_ConfigStructure.Trigger = EXTI_TRIGGER_FALLING;
  EXTI_ConfigStructure.GPIOSel = EXTI_GPIOA;
  EXTI_ConfigStructure.Mode = EXTI_MODE_INTERRUPT;
  HAL_EXTI_SetConfigLine(&hexti1, &EXTI_ConfigStructure);  
  HAL_EXTI_RegisterCallback(&hexti1, HAL_EXTI_COMMON_CB_ID, Btn1FallingCb);

  /* Set configuration except Interrupt and Event mask of Exti line 7*/
  EXTI_ConfigStructure.Line = EXTI_LINE_7;
  EXTI_ConfigStructure.Trigger = EXTI_TRIGGER_FALLING;
  EXTI_ConfigStructure.GPIOSel = EXTI_GPIOB;
  EXTI_ConfigStructure.Mode = EXTI_MODE_INTERRUPT;
  HAL_EXTI_SetConfigLine(&hexti7, &EXTI_ConfigStructure);  
  HAL_EXTI_RegisterCallback(&hexti7, HAL_EXTI_COMMON_CB_ID, Btn2FallingCb);
    
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);  
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
