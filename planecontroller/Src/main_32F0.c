/**
******************************************************************************
* @file    main_32F0.c
* @author  IPC Rennes
* @version V2.0.0
* @date    November 9th, 2018
* @brief   This file provides a set of functions needed to configure STM32 MCU
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main_32F0.h"

/* Variables -----------------------------------------------------------------*/
extern SipMotorDriver_TypeDef STSPIN32F0MotorDriver;
extern TIM_HandleTypeDef IF_TIMx;
extern TIM_HandleTypeDef HF_TIMx;
extern TIM_HandleTypeDef LF_TIMx;
extern TIM_HandleTypeDef ZC_TIMx;
extern ADC_HandleTypeDef ADCx;
extern UART_HandleTypeDef huart;
TIM_HandleTypeDef SERVO_TIM17;
TIM_HandleTypeDef SERVO_TIM16;

#define GPS_RX_BUFFER_MAX 255
uint8_t GPSRxBuffer[GPS_RX_BUFFER_MAX+1] = {0};

#define GPS_COMMAND_MAX 255
uint8_t GPSCommand[GPS_COMMAND_MAX] = {0};
uint8_t GPSCommandIndex = 0;


int16_t gps_elevation = 0;
int8_t latitude = 0; //(N=pos/S=neg)  
uint16_t latitude_frac = 0;  
int8_t longitude = 0; //(E=pos/W=neg)  
uint16_t longitude_frac = 0;  
uint8_t ground_speed = 0;  

void GPS_GGA(char* value) {
  uint32_t latitude_min = 0;
  uint32_t latitude_min_frac = 0;
  
  uint32_t longitude_min = 0;  
  uint32_t longitude_min_frac = 0;
  //
  char old = value[19];
  value[19] = 0;
  latitude = (int8_t)atoi(&value[17]);
  value[19] = old;
  latitude_min = atoi(&value[19]);
  latitude_min_frac = atoi(&value[22]);
  latitude_frac = (uint32_t)(latitude_min*100000 + latitude_min_frac) / (uint32_t)60;
  if (value[28] != 'N') {
    latitude = -latitude;
  }  
  // 
  old = value[33];
  value[33] = 0;  
  longitude = (int16_t)atoi(&value[30]);
  value[33] = old;
  longitude_min = atoi(&value[33]);
  longitude_min_frac = atoi(&value[36]);  
  longitude_frac = (uint32_t)(longitude_min*100000 + longitude_min_frac) / (uint32_t)60;
  //
  if (value[42] != 'E') {
    longitude = -longitude;
  }
  //  
  gps_elevation = atoi((char const*) &value[54]);
}

void GPS_VTG(char* value) {
  ground_speed = value[20];
  uint8_t coma = 0;
  // skip 7 comas
  uint8_t i;
  for(i = 0; i < 255; i++) {
    if (value[i] == ',') {
      coma++;
    }
    if (coma == 7) {
      break;
    }
  }
  //
  ground_speed = atoi(&value[i+1]);
}

void GPS_Received_Value(char* value) {
  // $GNGLL Position, time and fix status.
  // $GNGGA Time, position, and fix related data of the receiver.
  // $GNGSA Used to represent the ID’s of satellites which are used for position fix.
  // $GLGSV Satellite information about elevation, azimuth and CNR
  // $GPGSV Satellite information about elevation, azimuth and CNR  
  // $GNRMC Time, date, position, course and speed data.
  // $GNVTG Course and speed relative to the ground.
// "GL" (GLONASS),
//"BD" or "GB" (BeiDou),
//"GA" (Galileo), and
//"GN" (mixed)  
  
  // Parse only GGA message and VTG for now
  if(value[0] != '$') {
    return;
  }
  if (value[3] == 'G' || value[4] == 'G' || value[5] == 'A') {
    GPS_GGA(value);
    return;
  }
  if (value[3] == 'V' || value[4] == 'T' || value[5] == 'G') {
    GPS_VTG(value);
    return;
  }  
}

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
#if defined(POTENTIOMETER)
static void MX_ADC_Init(void);
#endif
#if defined(ST_PWM_INTERFACE)
static void MX_IF_TIMx_Init(void);
#endif
static void MX_HF_TIMx_Init(void);
static void MX_LF_TIMx_Init(void);
static void MX_ZC_TIMx_Init(void);
#if defined(UART_COMM)
static void MX_UART_Init(void);
#endif
static void SERVO_TIM17_Init(void);
static void SERVO_TIM16_Init(void);


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *unused)
{
  uint8_t size = huart.pRxBuffPtr - GPSRxBuffer;
  for (uint8_t i = 0; i < size; i++) {
    if(GPSRxBuffer[i] == '\n') {
      GPS_Received_Value((char*)GPSCommand); // Process GPS Command
      // Clear command
      for (uint8_t j = 0; j < GPS_COMMAND_MAX; j++) {
        GPSCommand[j] = 0;
      }
      GPSCommandIndex = 0;
    } else {
      // Copy characters to current GPS Command
      GPSCommand[GPSCommandIndex++] = GPSRxBuffer[i];
    }            
  }
  // Restart Transfer
  //huart.RxXferCount = huart.RxXferSize;
  //huart.pRxBuffPtr = GPSRxBuffer;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *unused)
{
}

int main(void)
{
  
  /* MCU Configuration----------------------------------------------------------*/
  
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  
  /* Configure the system clock */
  SystemClock_Config();
  
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  BPM280_Init();
  EXTILine1_Config();    
  SX1976_Init();
#if (defined(POTENTIOMETER)||defined(CURRENT_SENSE_ADC)||defined(VBUS_SENSE_ADC)||defined(TEMP_SENSE_ADC))
  MX_ADC_Init();
#endif
#if defined(ST_PWM_INTERFACE)  
  MX_IF_TIMx_Init();
#endif
  MX_LF_TIMx_Init();
  MX_HF_TIMx_Init();
  MX_ZC_TIMx_Init();
#if defined(UART_COMM)
  MX_UART_Init();
#endif
  SERVO_TIM17_Init();
  SERVO_TIM16_Init();
#ifndef UART_COMM
  GPS_UART_Init();
#endif
  /* **************************************************************************** 
  ==============================================================================   
###### This function initializes 6-Step lib ######
  ============================================================================== 
  **************************************************************************** */     
  MC_SixStep_INIT();
  /****************************************************************************/
  ServoPB7Angle(0);
  ServoPB6Angle(0);
    
  /* Infinite loop */
  while (1)
  {
    // Continuously Receive GPS data
    HAL_UART_Receive_IT(&huart, (uint8_t *)GPSRxBuffer, 255);    

    /*! **************************************************************************
    ==============================================================================   
###### How to use the 6Step FW Example project ######
    ==============================================================================     
    This workspace contains the middleware layer with Motor Control library to 
    drive a motor connected on STEVAL-ESC002V1 board performing a 6-step control
    algorithm allowing the motor speed regulation through PWM interface or UART 
    commands.
    The 6-step algorithm is voltage mode sensorless.
    
    A list of APIs in 6Step_Lib.h is provided to send user command to the 6Step
    library, for instance:
    
    (#)  MC_StartMotor() -> Start the motor
    
    (#)  MC_StoptMotor() -> Stop the motor
    
    (#)  MC_Set_Speed(...) -> Set the new motor speed
    
    The MC_SixStep_param_32F0.h contains the files names of the MC parameters
    files. The user can add its own parameter file using one of the existing file
    as an example.
    
    ==============================================================================   
###### USER SPACE ######
    ==============================================================================      
    *****************************************************************************/    
    
    
    /****************************************************************************/
  }
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{
  
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();
  
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) Error_Handler(); 
  
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  
}

#if (defined(POTENTIOMETER)||defined(CURRENT_SENSE_ADC)||defined(VBUS_SENSE_ADC)||defined(TEMP_SENSE_ADC))
/* ADC init function */
void MX_ADC_Init(void)
{
  ADC_ChannelConfTypeDef sConfig;
  
  /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  ADCx.Instance = BSP_ADCx;
  ADCx.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  ADCx.Init.Resolution = ADC_RESOLUTION_12B;
  ADCx.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  ADCx.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  ADCx.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  ADCx.Init.LowPowerAutoWait = DISABLE;
  ADCx.Init.LowPowerAutoPowerOff = DISABLE;
  ADCx.Init.ContinuousConvMode = DISABLE;
  ADCx.Init.DiscontinuousConvMode = DISABLE;
  ADCx.Init.ExternalTrigConv = BSP_ADCx_EXT_TRIG_CONV;
  ADCx.Init.ExternalTrigConvEdge = BSP_ADCx_EXT_TRIG_EDGE;
  ADCx.Init.DMAContinuousRequests = DISABLE;
  ADCx.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&ADCx) != HAL_OK) Error_Handler();
  
  /**Configure for the selected ADC regular channel to be converted. 
  */
  sConfig.Channel = ADC_CH_INIT;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_CH_INIT_ST;
  if (HAL_ADC_ConfigChannel(&ADCx, &sConfig) != HAL_OK) Error_Handler();
}
#endif

/* IF timer init function */
void SERVO_TIM17_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  
  SERVO_TIM17.Instance = TIM17;
  SERVO_TIM17.Init.Prescaler = 14;
  SERVO_TIM17.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1; //  TIM_COUNTERMODE_UP;
  SERVO_TIM17.Init.Period = 0xfb00;
  SERVO_TIM17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
  SERVO_TIM17.Init.RepetitionCounter = 0;
  SERVO_TIM17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&SERVO_TIM17) != HAL_OK) Error_Handler(); 
  
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&SERVO_TIM17, &sMasterConfig); 
  
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  sConfigOC.Pulse = 4800; // 3200 == 1mS
  
  if (HAL_TIM_PWM_ConfigChannel(&SERVO_TIM17, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) Error_Handler();
  
  HAL_TIMEx_PWMN_Start(&SERVO_TIM17, TIM_CHANNEL_1);   
}

/* IF timer init function */
void SERVO_TIM16_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  
  SERVO_TIM16.Instance = TIM16;
  SERVO_TIM16.Init.Prescaler = 14;
  SERVO_TIM16.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1; //  TIM_COUNTERMODE_UP;
  SERVO_TIM16.Init.Period = 0xfb00;
  SERVO_TIM16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
  SERVO_TIM16.Init.RepetitionCounter = 0;
  SERVO_TIM16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&SERVO_TIM16) != HAL_OK) Error_Handler(); 
  
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&SERVO_TIM16, &sMasterConfig); 
  
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  sConfigOC.Pulse = 4800;
  
  if (HAL_TIM_PWM_ConfigChannel(&SERVO_TIM16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) Error_Handler();
  
  HAL_TIMEx_PWMN_Start(&SERVO_TIM16, TIM_CHANNEL_1);   
}


#if defined(ST_PWM_INTERFACE)
/* IF timer init function */
void MX_IF_TIMx_Init(void)
{
  TIM_IC_InitTypeDef sICConfig;
  
  IF_TIMx.Instance = BSP_BOARD_IF_TIMx;
  IF_TIMx.Init.Prescaler = BSP_BOARD_IF_TIMx_PSC;
  IF_TIMx.Init.CounterMode = BSP_BOARD_IF_TIMx_COUNTER_MODE;
  IF_TIMx.Init.Period = BSP_BOARD_IF_TIMx_ARR;
  IF_TIMx.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  IF_TIMx.Init.RepetitionCounter = 0;
  IF_TIMx.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;  
  if (HAL_TIM_IC_Init(&IF_TIMx) != HAL_OK) Error_Handler(); 
  
  sICConfig.ICPolarity  = TIM_ICPOLARITY_RISING;
  sICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sICConfig.ICPrescaler = TIM_ICPSC_DIV1;
  sICConfig.ICFilter    = 2;   
  if(HAL_TIM_IC_ConfigChannel(&IF_TIMx, &sICConfig, TIM_CHANNEL_1) != HAL_OK) Error_Handler(); 
  
  if(HAL_TIM_IC_Start_IT(&IF_TIMx, TIM_CHANNEL_1) != HAL_OK) Error_Handler();
  
  __HAL_TIM_ENABLE_IT(&IF_TIMx, TIM_IT_UPDATE);
  
}
#endif

/* HF timer init function */
void MX_HF_TIMx_Init(void)
{
  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
  TIM_OC_InitTypeDef sConfigOC;
  
  HF_TIMx.Instance = BSP_HF_TIMx;
  HF_TIMx.Init.Prescaler = HF_TIMX_PSC;
  HF_TIMx.Init.CounterMode = BSP_HF_TIMx_COUNTER_MODE;
  HF_TIMx.Init.Period = HF_TIMX_ARR;
  HF_TIMx.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HF_TIMx.Init.RepetitionCounter = 0;
  if (HAL_TIM_PWM_Init(&HF_TIMx) != HAL_OK) Error_Handler();
  
  sSlaveConfig.SlaveMode = BSP_BOARD_HF_TIMx_SLAVE_MODE;
  sSlaveConfig.InputTrigger = BSP_BOARD_HF_TIMx_TS_ITR;
  if (HAL_TIM_SlaveConfigSynchronization(&HF_TIMx, &sSlaveConfig) != HAL_OK) Error_Handler();
  
  sMasterConfig.MasterOutputTrigger = BSP_BOARD_HF_TIMx_TRGO;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&HF_TIMx, &sMasterConfig) != HAL_OK) Error_Handler();
  
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = DEAD_TIME;
  sBreakDeadTimeConfig.BreakState = BSP_BOARD_HF_TIMx_BREAK_STATE;
  sBreakDeadTimeConfig.BreakPolarity = BSP_BOARD_HF_TIMx_BREAK_POL;
  sBreakDeadTimeConfig.AutomaticOutput = BSP_BOARD_HF_TIMx_BREAK_OUT;
  if (HAL_TIMEx_ConfigBreakDeadTime(&HF_TIMx, &sBreakDeadTimeConfig) != HAL_OK) Error_Handler();
  
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  sConfigOC.Pulse = HF_TIMX_ARR;
  
  if (HAL_TIM_PWM_ConfigChannel(&HF_TIMx, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) Error_Handler();
  if (HAL_TIM_PWM_ConfigChannel(&HF_TIMx, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) Error_Handler();
  if (HAL_TIM_PWM_ConfigChannel(&HF_TIMx, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) Error_Handler();
  if (HAL_TIM_PWM_ConfigChannel(&HF_TIMx, &sConfigOC, TIM_CHANNEL_4) != HAL_OK) Error_Handler();  
}

/* LF_TIMx init function */
void MX_LF_TIMx_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;  
  
  
  LF_TIMx.Instance = BSP_BOARD_LF_TIMx;
  LF_TIMx.Init.Prescaler = LF_TIMX_PSC;
  LF_TIMx.Init.CounterMode = BSP_BOARD_LF_TIMx_COUNTER_MODE ;
  LF_TIMx.Init.Period = LF_TIMX_ARR;
  LF_TIMx.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;  
  if (HAL_TIM_Base_Init(&LF_TIMx) != HAL_OK) Error_Handler();
  
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&LF_TIMx, &sClockSourceConfig) != HAL_OK) Error_Handler(); 
  
  sMasterConfig.MasterOutputTrigger = BSP_BOARD_LF_TIMx_TRGO;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&LF_TIMx, &sMasterConfig) != HAL_OK) Error_Handler();
}

/* ZC_TIMx init function */
void MX_ZC_TIMx_Init(void)
{
  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_OC_InitTypeDef sConfig;
  
  ZC_TIMx.Instance = BSP_BOARD_ZC_TIMx;
  ZC_TIMx.Init.Prescaler         = ZC_TIMX_PSC;
  ZC_TIMx.Init.Period            = (uint32_t)(SystemCoreClock / (ZC_TIMX_FREQUENCY_HZ*(ZC_TIMX_PSC+1))) - 1;
  ZC_TIMx.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  ZC_TIMx.Init.CounterMode       = BSP_BOARD_ZC_TIMx_COUNTER_MODE ;
  ZC_TIMx.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&ZC_TIMx) != HAL_OK) Error_Handler();
  
  sSlaveConfig.SlaveMode = BSP_BOARD_ZC_TIMx_SLAVE_MODE;
  sSlaveConfig.InputTrigger =  BSP_BOARD_ZC_TIMx_TS_ITR;
  if (HAL_TIM_SlaveConfigSynchronization(&ZC_TIMx, &sSlaveConfig) != HAL_OK) Error_Handler();
  
  sConfig.OCMode       = TIM_OCMODE_PWM1;
  sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
  sConfig.Pulse = PWM_EDGE_TO_ZC_READ_EXTRA_DELAY_CYC;
  if (HAL_TIM_PWM_ConfigChannel(&ZC_TIMx, &sConfig, TIM_CHANNEL_1) != HAL_OK) Error_Handler();
  
  __HAL_TIM_CLEAR_IT(&ZC_TIMx, TIM_IT_CC1);
  __HAL_TIM_ENABLE_IT(&ZC_TIMx, TIM_IT_CC1);
}

#if defined(UART_COMM)
/* UART init function */
void MX_UART_Init(void)
{
  huart.Instance = BSP_UART;
  huart.Init.BaudRate = BSP_UART_BAUD_RATE;
  huart.Init.WordLength = UART_WORDLENGTH_8B;
  huart.Init.StopBits = UART_STOPBITS_1;
  huart.Init.Parity = UART_PARITY_NONE;
  huart.Init.Mode = UART_MODE_TX_RX;
  huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart.Init.OverSampling = UART_OVERSAMPLING_16;
  huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  HAL_UART_Init(&huart);
}
#endif

/* UART init function */
void GPS_UART_Init(void)
{
  huart.Instance = USART1;
  huart.Init.BaudRate = 9600;
  huart.Init.WordLength = UART_WORDLENGTH_8B;
  huart.Init.StopBits = UART_STOPBITS_1;
  huart.Init.Parity = UART_PARITY_NONE;
  huart.Init.Mode = UART_MODE_TX_RX;
  huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart.Init.OverSampling = UART_OVERSAMPLING_16;
  huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  HAL_UART_Init(&huart);
}


/** Configure pins as 
* Analog 
* Input 
* Output
* EVENT_OUT
* EXTI
*/
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();  
  __HAL_RCC_GPIOF_CLK_ENABLE();
  
  /*Configure the FAULT LED GPIO pin (RGB1 = RED) */
  //  GPIO_InitStruct.Pin = BSP_BOARD_FAULT_LED_PIN;
  //  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  //  GPIO_InitStruct.Pull = GPIO_PULLUP;
  //  HAL_GPIO_Init(BSP_BOARD_FAULT_LED_PORT, &GPIO_InitStruct);
  //  BSP_BOARD_FAULT_LED_OFF();
  
  /*Configure the other LEDs (RGB2 = GREEN, RGB3 = BLUE) */
  //  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  //  GPIO_InitStruct.Pull = GPIO_PULLUP;
  //  GPIO_InitStruct.Pin = BSP_BOARD_RGB2_LED_PIN;
  //  HAL_GPIO_Init(BSP_BOARD_RGB2_LED_PORT, &GPIO_InitStruct);
  //  BSP_BOARD_RGB2_LED_OFF();
  
  //  GPIO_InitStruct.Pin = BSP_BOARD_RGB3_LED_PIN;
  //  HAL_GPIO_Init(BSP_BOARD_RGB3_LED_PORT, &GPIO_InitStruct);
  //  BSP_BOARD_RGB3_LED_OFF();  
  
  
  /*Configure overcurrent threshold GPIO pins */
  GPIO_InitStruct.Pin = BSP_OC_TH_STBY_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BSP_OC_TH_STBY_PORT, &GPIO_InitStruct);
  
  /*Set the overcurrent threshold */
  STSPIN32F0MotorDriver.Overcurrent_Threshold_Setvalue(BSP_OC_TH_100mV);
  
  /*Configure the overcurrent selection GPIO pin  */
  GPIO_InitStruct.Pin = BSP_OC_SEL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BSP_OC_SEL_PORT, &GPIO_InitStruct);
  
  /*Set the overcurrent selection */
  STSPIN32F0MotorDriver.Overcurrent_Selection(BSP_SEL_VIS_FROM_MCU_AND_GATE_LOGIC);
  
  /*Configure ZCU, ZCV and ZCW pins */
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  
  GPIO_InitStruct.Pin = BSP_BOARD_ZCU_PIN;
  HAL_GPIO_Init(BSP_BOARD_ZCU_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = BSP_BOARD_ZCV_PIN;
  HAL_GPIO_Init(BSP_BOARD_ZCV_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = BSP_BOARD_ZCW_PIN;
  HAL_GPIO_Init(BSP_BOARD_ZCW_PORT, &GPIO_InitStruct);
  
  /*Configure debug GPIO pins */
#if (GPIO_ZERO_CROSS!=0)
  GPIO_InitStruct.Pin = GPIO_CH_ZCR;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIO_PORT_ZCR, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_CH_TRIG;
  HAL_GPIO_Init(GPIO_PORT_TRIG, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIO_PORT_TRIG,GPIO_CH_TRIG,GPIO_PIN_RESET);
#endif
  
#if (GPIO_COMM!=0)
  GPIO_InitStruct.Pin = GPIO_CH_COMM;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIO_PORT_COMM, &GPIO_InitStruct);
#endif
  
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  
  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);  
}

/**
* @brief  This function is executed in case of error occurrence.
* @param  None
* @retval None
*/
void Error_Handler(void)
{
  // Red LED blinks
  BSP_BOARD_RGB1_LED_ON();
  while (1)
  {
    HAL_Delay(500);
    BSP_BOARD_RGB1_LED_TOGGLE();
  }
}

#ifdef USE_FULL_ASSERT

/**
* @brief Reports the name of the source file and the source line number
* where the assert_param error has occurred.
* @param file: pointer to the source file name
* @param line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
  
}

#endif

/**
* @}
*/ 

/**
* @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
