
#include "main_32F0.h"

EXTI_HandleTypeDef hexti;

//For PA1
void EXTILine1_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  EXTI_ConfigTypeDef EXTI_ConfigStructure;

  /* Enable SYSCFG clock */  
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  /* Enable GPIOA clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /* Configure PA1 pin as input floating */
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
  /* Set configuration except Interrupt and Event mask of Exti line 1*/
  EXTI_ConfigStructure.Line = EXTI_LINE_1;
  EXTI_ConfigStructure.Trigger = EXTI_TRIGGER_RISING;
  EXTI_ConfigStructure.GPIOSel = EXTI_GPIOA;
  EXTI_ConfigStructure.Mode = EXTI_MODE_INTERRUPT;
  HAL_EXTI_SetConfigLine(&hexti, &EXTI_ConfigStructure);
  
  HAL_EXTI_RegisterCallback(&hexti, HAL_EXTI_COMMON_CB_ID, Exti1RisingCb);
  
  /* Enable and set EXTI Line1 Interrupt to the lowest priority so far */
  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0x03, 0x00);
  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
}

void EXTI0_1_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hexti);
}
