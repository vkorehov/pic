/**
******************************************************************************
* File Name          : SPI.c
* Description        : This file provides code for the configuration
*                      of the SPI instances.
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
#include "main_32F0.h"

#define BPM280_SPI_READ_ID 0xD0
#define BPM280_SPI_READ_STATUS 0xF3
#define BPM280_SPI_WRITE_RESET 0x60
#define BPM280_SPI_WRITE_CTRL 0x74
#define BPM280_SPI_WRITE_CONFIG 0x75
#define BPM280_SPI_READ_CTRL 0xF4
#define BPM280_SPI_READ_CONFIG 0xF5

#define BPM280_SPI_READ_ALL 0xF7
#define BPM280_SPI_READ_CAL 0x88


#define BPM280_RESET 0xB6

#define BPM280_STATUS_BIT_CONV_DONE 0x08
#define BPM280_STATUS_BIT_NVM_TO_MEM_TRANSFER 0x01

#define BPM280_CTRL_OVRS_TEMP_2X 0x40
#define BPM280_CTRL_OVRS_TEMP_8X 0x80
#define BPM280_CTRL_OVRS_TEMP_16X 0xA0
#define BPM280_CTRL_OVRS_PRES_8X 0x10
#define BPM280_CTRL_OVRS_PRES_16X 0x14
#define BPM280_CTRL_PM_NORMAL 0x03

#define BPM280_CONFIG_4_WIRE_SPI 0x00
#define BPM280_CONFIG_0MS_STBY 0x000
#define BPM280_CONFIG_250MS_STBY 0x60

#define BPM280_CONFIG_FILTER_16X 0x14
#define BPM280_CONFIG_FILTER_2X  0x04
#define BPM280_CONFIG_FILTER_OFF 0x00

SPI_HandleTypeDef hspi1;
uint16_t dig_T1 = 0;
int16_t dig_T2 = 0;
int16_t dig_T3 = 0;

uint16_t dig_P1 = 0;
int16_t dig_P2 = 0;
int16_t dig_P3 = 0;
int16_t dig_P4 = 0;
int16_t dig_P5 = 0;
int16_t dig_P6 = 0;
int16_t dig_P7 = 0;
int16_t dig_P8 = 0;
int16_t dig_P9 = 0;

uint32_t BPM280_Init(void) {
  uint8_t outputBuffer[2] = {BPM280_SPI_WRITE_RESET, BPM280_RESET};
  uint8_t inputBuffer[26];  
  
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);    
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 2, 1000) != HAL_OK) {
    Error_Handler();
  }    
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);                    
  // READ ID
  while (1) {
    outputBuffer[0] = BPM280_SPI_READ_ID;    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);                      
    if (HAL_SPI_Transmit(&hspi1, outputBuffer, 1, 1000) != HAL_OK) {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);                
      continue;
    }
    if (HAL_SPI_Receive (&hspi1, inputBuffer, 1, 1000) != HAL_OK) {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);                
      continue;
    }    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);        
    
    if (inputBuffer[0] != 0x58) {
      continue;
    }
    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);          
    // READ STATUS
    outputBuffer[0] = BPM280_SPI_READ_STATUS;    
    if (HAL_SPI_Transmit(&hspi1, outputBuffer, 1, 1000) != HAL_OK) {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);            
      continue;
    }  
    if (HAL_SPI_Receive (&hspi1, inputBuffer, 1, 1000) != HAL_OK) {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);            
      continue;
    }
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);    
    
    if (inputBuffer[0] & BPM280_STATUS_BIT_NVM_TO_MEM_TRANSFER != 0) { // PoR still happens
      continue;
    }
    
    if (inputBuffer[0] & BPM280_STATUS_BIT_CONV_DONE != 0) { // still busy converting
      continue;
    }
    break;
  }
  
  // WRITE CONFIG
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  outputBuffer[0] = BPM280_SPI_WRITE_CONFIG;
  outputBuffer[1] = BPM280_CONFIG_4_WIRE_SPI | BPM280_CONFIG_0MS_STBY | BPM280_CONFIG_FILTER_OFF;
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 2, 1000) != HAL_OK) {
    Error_Handler();
  }     
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);    
  
  // READ CONFIG
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);    
  outputBuffer[0] = BPM280_SPI_READ_CONFIG;    
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 1, 1000) != HAL_OK) {
    Error_Handler();
  } 
  if (HAL_SPI_Receive (&hspi1, inputBuffer, 1, 1000) != HAL_OK) {
    Error_Handler();
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);    
  // Assert config OK
  if (inputBuffer[0] != (BPM280_CONFIG_4_WIRE_SPI | BPM280_CONFIG_0MS_STBY | BPM280_CONFIG_FILTER_OFF)) {
    Error_Handler();      
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);        
  // WRITE CTRL
  outputBuffer[0] = BPM280_SPI_WRITE_CTRL;
  outputBuffer[1] = BPM280_CTRL_OVRS_TEMP_2X | BPM280_CTRL_OVRS_PRES_16X | BPM280_CTRL_PM_NORMAL;    
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 2, 1000) != HAL_OK) {
    Error_Handler();
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);        
  
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);            
  // READ CTRL
  outputBuffer[0] = BPM280_SPI_READ_CTRL;    
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 1, 1000) != HAL_OK) {
    Error_Handler();
  } 
  if (HAL_SPI_Receive (&hspi1, inputBuffer, 1, 1000) != HAL_OK) {
    Error_Handler();
  }    
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  // Assert CTRL OK
  if (inputBuffer[0] != (BPM280_CTRL_OVRS_TEMP_2X | BPM280_CTRL_OVRS_PRES_16X | BPM280_CTRL_PM_NORMAL)) {
    Error_Handler();    
  }
  //
  // Read Calibration Data
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  // READ CTRL
  outputBuffer[0] = BPM280_SPI_READ_CAL;    
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 1, 1000) != HAL_OK) {
    Error_Handler();
  } 
  if (HAL_SPI_Receive (&hspi1, inputBuffer, 26, 1000) != HAL_OK) {
    Error_Handler();
  }    
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  
  // Map CAL data
  dig_T1 = (inputBuffer[1] << 8) | inputBuffer[0];
  dig_T2 = (inputBuffer[3] << 8) | inputBuffer[2];
  dig_T3 = (inputBuffer[5] << 8) | inputBuffer[4];
  
  dig_P1 = (inputBuffer[7] << 8) | inputBuffer[6];
  dig_P2 = (inputBuffer[9] << 8) | inputBuffer[8];
  dig_P3 = (inputBuffer[11] << 8) | inputBuffer[10];
  dig_P4 = (inputBuffer[13] << 8) | inputBuffer[12];
  dig_P5 = (inputBuffer[15] << 8) | inputBuffer[14];
  dig_P6 = (inputBuffer[17] << 8) | inputBuffer[16];
  dig_P7 = (inputBuffer[19] << 8) | inputBuffer[18];
  dig_P8 = (inputBuffer[21] << 8) | inputBuffer[20];
  dig_P9 = (inputBuffer[23] << 8) | inputBuffer[22];
  
  return 1;
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
int32_t bmp280_compensate_T(int32_t adc_T, int32_t* t_fine)
{
  int32_t var1, var2, T;
  var1 = ((((adc_T>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
  var2 = (((((adc_T>>4) - ((int32_t)dig_T1)) * ((adc_T>>4) - ((int32_t)dig_T1))) >> 12) *
          ((int32_t)dig_T3)) >> 14;
  *t_fine = var1 + var2;
  T = ((*t_fine) * 5 + 128) >> 8;
  return T;
}

// Returns pressure in Pa as unsigned 32 bit integer. Output value of “96386” equals 96386 Pa = 963.86 hPa
uint32_t bmp280_compensate_P(int32_t adc_P, int32_t* t_fine)
{
  int32_t var1, var2;
  uint32_t p;
  var1 = (((int32_t)(*t_fine))>>1) - (int32_t)64000;
  var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((int32_t)dig_P6);
  var2 = var2 + ((var1*((int32_t)dig_P5))<<1);
  var2 = (var2>>2)+(((int32_t)dig_P4)<<16);
  var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((int32_t)dig_P2) * var1)>>1))>>18;
  var1 =((((32768+var1))*((int32_t)dig_P1))>>15);
  if (var1 == 0)
  {
    return 0; // avoid exception caused by division by zero
  }
  p = (((uint32_t)(((int32_t)1048576)-adc_P)-(var2>>12)))*3125;
  if (p < 0x80000000)
  {
    p = (p << 1) / ((uint32_t)var1);
  }
  else
  {
    p = (p / (uint32_t)var1) * 2;
  }
  var1 = (((int32_t)dig_P9) * ((int32_t)(((p>>3) * (p>>3))>>13)))>>12;
  var2 = (((int32_t)(p>>2)) * ((int32_t)dig_P8))>>13;
  p = (uint32_t)((int32_t)p + ((var1 + var2 + dig_P7) >> 4));
  return p;
}

uint32_t BPM280_Read_Pressure(void) {
  uint8_t outputBuffer[1] = {BPM280_SPI_READ_ALL};
  uint8_t inputBuffer[6];
  
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  
  // READ TEMP
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 1, 1000) != HAL_OK) {
    Error_Handler();
  }  
  if (HAL_SPI_Receive (&hspi1, inputBuffer, 6, 1000) != HAL_OK) {
    Error_Handler();
  }
  
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  int32_t pressure_raw = (inputBuffer[0] << 12) | (inputBuffer[1] << 4) | (inputBuffer[2] >> 4);
  int32_t temp_raw = (inputBuffer[3] << 12) | (inputBuffer[4] << 4) | (inputBuffer[5] >> 4);
  int32_t t_fine = 0;
  
  int32_t temp = bmp280_compensate_T(temp_raw, &t_fine);
  int32_t press = bmp280_compensate_P(pressure_raw, &t_fine);
  
  return press;
}

/* SPI1 init function */
void MX_SPI1_Init(void)
{
  
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{
  
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
    /* USER CODE BEGIN SPI1_MspInit 0 */
    
    /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* USER CODE BEGIN SPI1_MspInit 1 */
    
    /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{
  
  if(spiHandle->Instance==SPI1)
  {
    /* USER CODE BEGIN SPI1_MspDeInit 0 */
    
    /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();
    
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
    
    /* USER CODE BEGIN SPI1_MspDeInit 1 */
    
    /* USER CODE END SPI1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
