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

#define SX1976_FRF_READ_MSB 0x06
#define SX1976_FRF_READ_MID 0x07
#define SX1976_FRF_READ_LSB 0x08

#define SX1976_FRF_WRITE_MSB 0x86
#define SX1976_FRF_WRITE_MID 0x87
#define SX1976_FRF_WRITE_LSB 0x88

//FRF = 14226227
#define FRF_868_3_MHZ_MSB 0xD9
#define FRF_868_3_MHZ_MID 0x13
#define FRF_868_3_MHZ_LSB 0x33

#define SX1976_FSK_PA_BOOST_READ 0x4d
#define SX1976_FSK_PA_BOOST_WRITE 0xcd

#define SX1976_FSK_PA_BOOST_ON 0x87
#define SX1976_FSK_PA_BOOST_OFF 0x84


#define SX1976_OP_MODE_READ 0x01
#define SX1976_OP_MODE_WRITE 0x81

#define OP_MODE_SLEEP 0x00
#define OP_MODE_STDBY 0x01
#define OP_MODE_TX    0x03
#define OP_MODE_RX_CONT    0x05
#define OP_MODE_RX_SINGLE    0x06
#define OP_MODE_CAD    0x07

#define OP_MODE_LF    0x08
#define OP_MODE_HF   0x00

#define OP_MODE_LORA   0x80

#define SX1976_PA_SELECT_READ 0x09
#define SX1976_PA_SELECT_WRITE 0x89

#define PA_SELECT_BOOST   0x80
#define PA_SELECT_2dBm    0x0f // Pout=17-(15-OutputPower)
#define PA_SELECT_3dBm    0x0e // Pout=17-(15-OutputPower)
#define PA_SELECT_4dBm    0x0d // Pout=17-(15-OutputPower)
#define PA_SELECT_5dBm    0x0c // Pout=17-(15-OutputPower)
#define PA_SELECT_6dBm    0x0b // Pout=17-(15-OutputPower)
#define PA_SELECT_7dBm    0x0a // Pout=17-(15-OutputPower)
#define PA_SELECT_8dBm    0x09 // Pout=17-(15-OutputPower)
#define PA_SELECT_9dBm    0x08 // Pout=17-(15-OutputPower)
#define PA_SELECT_10dBm    0x07 // Pout=17-(15-OutputPower)
#define PA_SELECT_11dBm    0x06 // Pout=17-(15-OutputPower)
#define PA_SELECT_12dBm    0x05 // Pout=17-(15-OutputPower)
#define PA_SELECT_13dBm    0x04 // Pout=17-(15-OutputPower)
#define PA_SELECT_14dBm    0x03 // Pout=17-(15-OutputPower)
#define PA_SELECT_15dBm    0x02 // Pout=17-(15-OutputPower)
#define PA_SELECT_16dBm    0x01 // Pout=17-(15-OutputPower)
#define PA_SELECT_17dBm    0x00 // Pout=17-(15-OutputPower)


#define SX1976_OCP_READ 0x0B
#define SX1976_OCP_WRITE 0x8B

#define SX1976_OCP_ON 0x20
#define SX1976_OCP_TRIM_240ma 27
#define SX1976_OCP_TRIM_230ma 26
#define SX1976_OCP_TRIM_220ma 25
#define SX1976_OCP_TRIM_210ma 24
#define SX1976_OCP_TRIM_200ma 23
#define SX1976_OCP_TRIM_190ma 22
#define SX1976_OCP_TRIM_180ma 21
#define SX1976_OCP_TRIM_170ma 20
#define SX1976_OCP_TRIM_160ma 19
#define SX1976_OCP_TRIM_150ma 18
#define SX1976_OCP_TRIM_140ma 17
#define SX1976_OCP_TRIM_130ma 16
#define SX1976_OCP_TRIM_120ma 15
#define SX1976_OCP_TRIM_115ma 14
#define SX1976_OCP_TRIM_110ma 13
#define SX1976_OCP_TRIM_105ma 12
#define SX1976_OCP_TRIM_100ma 11


#define SX1976_LNA_READ 0x0C
#define SX1976_LNA_WRITE 0x8C

#define SX1976_LNA_BOOST_HF_ON 0x03
#define SX1976_LNA_GAIN_HIGHEST 0x20

#define SX1976_DIO_MAPPING1_READ 0x40
#define SX1976_DIO_MAPPING1_WRITE 0xC0
#define SX1976_DIO0_RX_DONE 0x00
#define SX1976_DIO0_TX_DONE 0x40
#define SX1976_DIO0_CAD_DONE 0x80


#define SX1976_MODEM_CONFIG1_READ 0x1D
#define SX1976_MODEM_CONFIG1_WRITE 0x9D

#define MODEM_CONFIG1_HM_EXPLICIT 0x00
#define MODEM_CONFIG1_HM_IMPLICIT 0x01
#define MODEM_CONFIG1_BW_125khz 0x70
#define MODEM_CONFIG1_CR_4_5 0x02

#define SX1976_MODEM_CONFIG2_READ 0x1E
#define SX1976_MODEM_CONFIG2_WRITE 0x9E

#define MODEM_CONFIG2_CRC_ON 0x04
#define MODEM_CONFIG2_CRC_OFF 0x00

#define MODEM_CONFIG2_SF_64 0x60
#define MODEM_CONFIG2_SF_128 0x70
#define MODEM_CONFIG2_SF_256 0x80
#define MODEM_CONFIG2_SF_512 0x90
#define MODEM_CONFIG2_SF_1024 0xa0
#define MODEM_CONFIG2_SF_2048 0xb0
#define MODEM_CONFIG2_SF_4096 0xc0

#define SX1976_PAYLOAD_LEN_READ 0x22
#define SX1976_PAYLOAD_LEN_WRITE 0xA2

#define SX1976_MAX_PAYLOAD_LEN_READ 0x23
#define SX1976_MAX_PAYLOAD_LEN_WRITE 0xA3

#define SX1976_FIFO_ADDR_PTR_READ 0x0D
#define SX1976_FIFO_ADDR_PTR_WRITE 0x8D

#define SX1976_FIFO_ADDR_RX_BASE_READ 0x0F
#define SX1976_FIFO_ADDR_RX_BASE_WRITE 0x8F

#define SX1976_FIFO_ADDR_TX_BASE_READ 0x0E
#define SX1976_FIFO_ADDR_TX_BASE_WRITE 0x8E

#define SX1976_FIFO_ADDR_RX_CURRENT_READ 0x10
#define SX1976_FIFO_ADDR_RX_CURRENT_WRITE 0x90

#define SX1976_FIFO_RX_BYTES_NB_READ 0x13
#define SX1976_FIFO_RX_BYTES_NB_WRITE 0x93

#define SX1976_IRQ_FLAGS_READ 0x12
#define SX1976_IRQ_FLAGS_WRITE 0x92

#define IRQ_RX_TIMEOUT 0x80
#define IRQ_RX_DONE 0x40
#define IRQ_PAYLOAD_CRC_ERROR 0x20
#define IRQ_VALID_HEADER 0x10
#define IRQ_TX_DONE 0x08
#define IRQ_CAD_DONE 0x04
#define IRQ_FHSS_CHANGE_CHANNEL 0x02
#define IRQ_CAD_DETECTED 0x01

#define SX1976_FIFO_READ 0x00
#define SX1976_FIFO_WRITE 0x80

uint8_t sx1276TxBuffer[50];
extern UART_HandleTypeDef huart;

void SX1976_OnRX(uint8_t discard);

static void write_and_retry(uint8_t addr, uint8_t val) {  
  while(1) {
    // Write
    uint8_t outputBuffer[2] = {addr | 0x80, val};
    uint8_t inputBuffer[1] = {0};
    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 
    if (HAL_SPI_Transmit(&hspi1, outputBuffer, 2, 1000) != HAL_OK) {
      Error_Handler();
    }
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);                        
    
    // Verify
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 
    outputBuffer[0] = addr;
    if (HAL_SPI_Transmit(&hspi1, outputBuffer, 1, 1000) != HAL_OK) {
      Error_Handler();
    }
    if (HAL_SPI_Receive (&hspi1, inputBuffer, 1, 1000) != HAL_OK) {
      Error_Handler();
    }        
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
    if (inputBuffer[0] == val) {
      break;  
    }  
  }
}

static void write_and_verify(uint8_t addr, uint8_t val) {  
  // Write
  uint8_t outputBuffer[2] = {addr | 0x80, val};
  uint8_t inputBuffer[1] = {0};
  
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 2, 1000) != HAL_OK) {
    Error_Handler();
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);                        
  
  // Verify
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 
  outputBuffer[0] = addr;
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 1, 1000) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_SPI_Receive (&hspi1, inputBuffer, 1, 1000) != HAL_OK) {
    Error_Handler();
  }        
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  if (inputBuffer[0] != val) {
    Error_Handler();
  }  
}

static uint8_t read(uint8_t addr) {
  uint8_t outputBuffer[1] = {addr};
  uint8_t inputBuffer[1] = {0};
    
  // Read
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 1, 1000) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_SPI_Receive (&hspi1, inputBuffer, 1, 1000) != HAL_OK) {
    Error_Handler();
  }        
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  
  return inputBuffer[0];
}

static void write(uint8_t addr, uint8_t val) {
  uint8_t outputBuffer[2] = {addr | 0x80, val};
    
  // Read
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 2, 1000) != HAL_OK) {
    Error_Handler();
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}


void Exti1RisingCb(void)
{
  while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)) {
    // Read Int Flags
    uint8_t flags = read(SX1976_IRQ_FLAGS_READ);
    
    if (flags & IRQ_RX_DONE) {
      SX1976_OnRX(flags & IRQ_PAYLOAD_CRC_ERROR);
    } else {
      sprintf((char *)sx1276TxBuffer, ">> NO RX\r\n >");
      HAL_UART_Transmit(&huart, (uint8_t *)sx1276TxBuffer, (strlen((char *)sx1276TxBuffer)),5000);                        
    }
    // Reset IRQs
    write(SX1976_IRQ_FLAGS_READ, flags);
  }  
}

void SX1976_RX_Reset(void) {  
  // Configure FIFO buffer ptr to be set to RX Base Addr
  write_and_verify(SX1976_FIFO_ADDR_PTR_READ, 0x00);  
  // Configure RX Current Write to be aligned as well
  write_and_verify(SX1976_FIFO_ADDR_RX_CURRENT_READ, 0x00);  
}

void SX1976_OnRX(uint8_t discard) {
  uint8_t outputBuffer[2] = {0,0};
  uint8_t inputBuffer[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  // Read Current Addr
  uint8_t currentAddr = read(SX1976_FIFO_ADDR_RX_CURRENT_READ);
  
  // move Ptr to current addr
  write(SX1976_FIFO_ADDR_PTR_READ, currentAddr);
  
  // Read number of bytes
  uint8_t bytesCount = read(SX1976_FIFO_RX_BYTES_NB_READ);
  uint8_t bytesCountOrig = bytesCount;
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 
  outputBuffer[0] = SX1976_FIFO_READ;
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 1, 1000) != HAL_OK) {
    Error_Handler();
  }
  // it should never be > packetSize?
  while (bytesCount > 0) {
    // Read Fifo
    if (HAL_SPI_Receive (&hspi1, inputBuffer, 16, 1000) != HAL_OK) {
      Error_Handler();
    }
    bytesCount -= 16;
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  
  sprintf((char *)sx1276TxBuffer, ">>RX: %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u DISCARD? %u Addr: %u Bytes: %u\r\n >",
          inputBuffer[0], inputBuffer[1], inputBuffer[2], inputBuffer[3]
            , inputBuffer[4], inputBuffer[5], inputBuffer[6], inputBuffer[7]
              , inputBuffer[8], inputBuffer[9], inputBuffer[10], inputBuffer[11]
                , inputBuffer[12], inputBuffer[13], inputBuffer[14], inputBuffer[15], discard, currentAddr, bytesCountOrig);
  HAL_UART_Transmit(&huart, (uint8_t *)sx1276TxBuffer, (strlen((char *)sx1276TxBuffer)),5000);                  
}

uint32_t SX1976_Init(void) {
  uint8_t outputBuffer[4] = {0,0,0,0};
  uint8_t inputBuffer[3];
    
  // Go to sleep and Set Lora mode
  write_and_retry(SX1976_OP_MODE_READ, OP_MODE_SLEEP | OP_MODE_HF | OP_MODE_LORA);  
  write_and_retry(SX1976_OP_MODE_READ, OP_MODE_STDBY | OP_MODE_HF | OP_MODE_LORA);  
  // Reset ALL IRQs
  write(SX1976_IRQ_FLAGS_READ, 0xff);  
  
  // Set FRF
  outputBuffer[0] = SX1976_FRF_WRITE_MSB;
  outputBuffer[1] = FRF_868_3_MHZ_MSB;
  outputBuffer[2] = FRF_868_3_MHZ_MID;
  outputBuffer[3] = FRF_868_3_MHZ_LSB;
  
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 4, 1000) != HAL_OK) {
    Error_Handler();
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);                      
  // Read FRF
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 
  outputBuffer[0] = SX1976_FRF_READ_MSB;
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 1, 1000) != HAL_OK) {
    Error_Handler();
  }    
  if (HAL_SPI_Receive (&hspi1, inputBuffer, 3, 1000) != HAL_OK) {
    Error_Handler();
  }      
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);                      
  // assert
  if (inputBuffer[0] != FRF_868_3_MHZ_MSB ||
      inputBuffer[1] != FRF_868_3_MHZ_MID ||
        inputBuffer[2] != FRF_868_3_MHZ_LSB)
  {
    Error_Handler();
  }
  
  // Set power level and select PA_BOOST
  write_and_verify(SX1976_PA_SELECT_READ, PA_SELECT_2dBm | PA_SELECT_BOOST);  
  // Configure OCP
  write_and_verify(SX1976_OCP_READ, SX1976_OCP_ON | SX1976_OCP_TRIM_110ma);  
  // Configure LNA
  write_and_verify(SX1976_LNA_READ, SX1976_LNA_BOOST_HF_ON | SX1976_LNA_GAIN_HIGHEST);  
  // Configure Implicit header mode
  write_and_verify(SX1976_MODEM_CONFIG1_READ, MODEM_CONFIG1_HM_IMPLICIT | MODEM_CONFIG1_BW_125khz | MODEM_CONFIG1_CR_4_5);  
    
  // Configure CRC ON
  write_and_verify(SX1976_MODEM_CONFIG2_READ, MODEM_CONFIG2_CRC_ON | MODEM_CONFIG2_SF_128);  
    
  // configure PayloadLength to be fixed 16 Bytes
  write_and_verify(SX1976_PAYLOAD_LEN_READ, 16);
    
  // configure MaxPayloadLength to be fixed 16 Bytes
  write_and_verify(SX1976_MAX_PAYLOAD_LEN_READ, 16);
    
  // Configure TX Base Addr
  write_and_verify(SX1976_FIFO_ADDR_TX_BASE_READ, 0x80);
    
  // Configure RX Base Addr
  write_and_verify(SX1976_FIFO_ADDR_RX_BASE_READ, 0x00);
  
  SX1976_RX_Reset(); // Reset pointers
  
  // Configure DIO0 as RXDONE
  write_and_verify(SX1976_DIO_MAPPING1_READ, SX1976_DIO0_RX_DONE);
    
  // Go to continous RX
  write_and_retry(SX1976_OP_MODE_READ, OP_MODE_RX_CONT | OP_MODE_HF | OP_MODE_LORA);
  
  return 0;
}

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
  outputBuffer[1] = BPM280_CONFIG_4_WIRE_SPI | BPM280_CONFIG_0MS_STBY | BPM280_CONFIG_FILTER_16X;
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
  if (inputBuffer[0] != (BPM280_CONFIG_4_WIRE_SPI | BPM280_CONFIG_0MS_STBY | BPM280_CONFIG_FILTER_16X)) {
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
