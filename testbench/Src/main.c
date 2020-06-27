/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


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

void SX1976_OnTX(void);
void SX1976_Poll(void);

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

void SX1976_Poll(void)
{
    // Read Int Flags
    uint8_t flags = read(SX1976_IRQ_FLAGS_READ);
    
    if (flags & IRQ_TX_DONE) {
      SX1976_OnTX();
    }
    // Reset IRQs
    write(SX1976_IRQ_FLAGS_READ, flags);
    // Go to TX
    write_and_retry(SX1976_OP_MODE_READ, OP_MODE_TX | OP_MODE_HF | OP_MODE_LORA);    
}

void SX1976_TX_Reset(void) {  
  // Configure FIFO buffer ptr to be set to RX Base Addr
  write_and_verify(SX1976_FIFO_ADDR_PTR_READ, 0x80);  
}

uint8_t counter = 0;
void SX1976_OnTX(void) {
  uint8_t outputBuffer[17] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,counter++};
  // move Ptr to beginning of mem
  write(SX1976_FIFO_ADDR_PTR_READ, 0x80);
  
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 
  outputBuffer[0] = SX1976_FIFO_WRITE;
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 17, 1000) != HAL_OK) {
    Error_Handler();
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);  
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
  write_and_verify(SX1976_PA_SELECT_READ, PA_SELECT_10dBm | PA_SELECT_BOOST);  
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
  
  SX1976_TX_Reset(); // Reset pointers
  
  // Configure DIO0 as RXDONE
  write_and_verify(SX1976_DIO_MAPPING1_READ, SX1976_DIO0_RX_DONE);
      
  return 0;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  SX1976_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    SX1976_Poll();
    HAL_Delay(1000);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) {
    
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
