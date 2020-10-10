
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"


uint8_t convert(uint8_t in) {
  return ~in;
}

//FRF = 14226227
///int32_t  Ferr = 4000;
uint32_t Ftarget = 868300000 - 4000;
uint32_t frf = 0;
#define AFC

void SX1276_OnRX(uint8_t discard);
void SX1276_AFC(int32_t ferr);


#define SX1276_FRF_READ_MSB 0x06
#define SX1276_FRF_READ_MID 0x07
#define SX1276_FRF_READ_LSB 0x08

#define SX1276_FRF_WRITE_MSB 0x86
#define SX1276_FRF_WRITE_MID 0x87
#define SX1276_FRF_WRITE_LSB 0x88

//FRF = 14226227
#define FRF_868_3_MHZ_MSB 0xD9
#define FRF_868_3_MHZ_MID 0x13
#define FRF_868_3_MHZ_LSB 0x33

#define SX1276_FSK_PA_BOOST_READ 0x4d
#define SX1276_FSK_PA_BOOST_WRITE 0xcd

#define SX1276_FSK_PA_BOOST_ON 0x87
#define SX1276_FSK_PA_BOOST_OFF 0x84


#define SX1276_OP_MODE_READ 0x01
#define SX1276_OP_MODE_WRITE 0x81

#define OP_MODE_SLEEP 0x00
#define OP_MODE_STDBY 0x01
#define OP_MODE_TX    0x03
#define OP_MODE_RX_CONT    0x05
#define OP_MODE_RX_SINGLE    0x06
#define OP_MODE_CAD    0x07

#define OP_MODE_LF    0x08
#define OP_MODE_HF   0x00

#define OP_MODE_LORA   0x80

#define SX1276_PA_SELECT_READ 0x09
#define SX1276_PA_SELECT_WRITE 0x89

#define PA_SELECT_BOOST   0x80
#define PA_SELECT_NO_BOOST   0x00

#define PA_SELECT_NO_BOOST_MINUS_4dBm    0x00 // Pout=10.8-(15-OutputPower)
#define PA_SELECT_NO_BOOST_MINUS_3dBm    0x01 // Pout=10.8-(15-OutputPower)
#define PA_SELECT_NO_BOOST_MINUS_2dBm    0x02 // Pout=10.8-(15-OutputPower)
#define PA_SELECT_NO_BOOST_MINUS_1dBm    0x03 // Pout=10.8-(15-OutputPower)
#define PA_SELECT_NO_BOOST_0dBm    0x04 // Pout=10.8-(15-OutputPower)
#define PA_SELECT_NO_BOOST_1dBm    0x05 // Pout=10.8-(15-OutputPower)
#define PA_SELECT_NO_BOOST_2dBm    0x06 // Pout=10.8-(15-OutputPower)
#define PA_SELECT_NO_BOOST_3dBm    0x07 // Pout=10.8-(15-OutputPower)
#define PA_SELECT_NO_BOOST_4dBm    0x08 // Pout=10.8-(15-OutputPower)
#define PA_SELECT_NO_BOOST_5dBm    0x09 // Pout=10.8-(15-OutputPower)
#define PA_SELECT_NO_BOOST_6dBm    0x0a // Pout=10.8-(15-OutputPower)
#define PA_SELECT_NO_BOOST_7dBm    0x0b // Pout=10.8-(15-OutputPower)
#define PA_SELECT_NO_BOOST_8dBm    0x0c // Pout=10.8-(15-OutputPower)
#define PA_SELECT_NO_BOOST_9dBm    0x0d // Pout=10.8-(15-OutputPower)
#define PA_SELECT_NO_BOOST_10dBm    0x0e // Pout=10.8-(15-OutputPower)
#define PA_SELECT_NO_BOOST_11dBm    0x0f // Pout=10.8-(15-OutputPower)

#define PA_SELECT_BOOST_2dBm    0x00 // Pout=17-(15-OutputPower)
#define PA_SELECT_BOOST_3dBm    0x01 // Pout=17-(15-OutputPower)
#define PA_SELECT_BOOST_4dBm    0x02 // Pout=17-(15-OutputPower)
#define PA_SELECT_BOOST_5dBm    0x03 // Pout=17-(15-OutputPower)
#define PA_SELECT_BOOST_6dBm    0x04 // Pout=17-(15-OutputPower)
#define PA_SELECT_BOOST_7dBm    0x05 // Pout=17-(15-OutputPower)
#define PA_SELECT_BOOST_8dBm    0x06 // Pout=17-(15-OutputPower)
#define PA_SELECT_BOOST_9dBm    0x07 // Pout=17-(15-OutputPower)
#define PA_SELECT_BOOST_10dBm    0x08 // Pout=17-(15-OutputPower)
#define PA_SELECT_BOOST_11dBm    0x09 // Pout=17-(15-OutputPower)
#define PA_SELECT_BOOST_12dBm    0x0a // Pout=17-(15-OutputPower)
#define PA_SELECT_BOOST_13dBm    0x0b // Pout=17-(15-OutputPower)
#define PA_SELECT_BOOST_14dBm    0x0c // Pout=17-(15-OutputPower)
#define PA_SELECT_BOOST_15dBm    0x0d // Pout=17-(15-OutputPower)
#define PA_SELECT_BOOST_16dBm    0x0e // Pout=17-(15-OutputPower)
#define PA_SELECT_BOOST_17dBm    0x0f // Pout=17-(15-OutputPower)


#define SX1276_OCP_READ 0x0B
#define SX1276_OCP_WRITE 0x8B

#define SX1276_OCP_ON 0x20
#define SX1276_OCP_TRIM_240ma 27
#define SX1276_OCP_TRIM_230ma 26
#define SX1276_OCP_TRIM_220ma 25
#define SX1276_OCP_TRIM_210ma 24
#define SX1276_OCP_TRIM_200ma 23
#define SX1276_OCP_TRIM_190ma 22
#define SX1276_OCP_TRIM_180ma 21
#define SX1276_OCP_TRIM_170ma 20
#define SX1276_OCP_TRIM_160ma 19
#define SX1276_OCP_TRIM_150ma 18
#define SX1276_OCP_TRIM_140ma 17
#define SX1276_OCP_TRIM_130ma 16
#define SX1276_OCP_TRIM_120ma 15
#define SX1276_OCP_TRIM_115ma 14
#define SX1276_OCP_TRIM_110ma 13
#define SX1276_OCP_TRIM_105ma 12
#define SX1276_OCP_TRIM_100ma 11


#define SX1276_LNA_READ 0x0C
#define SX1276_LNA_WRITE 0x8C

#define SX1276_LNA_BOOST_HF_ON 0x03
#define SX1276_LNA_BOOST_HF_OFF 0x00

#define SX1276_LNA_GAIN_HIGHEST 0x20
#define SX1276_LNA_GAIN_G1 0x20
#define SX1276_LNA_GAIN_G2 0x40
#define SX1276_LNA_GAIN_G3 0x60
#define SX1276_LNA_GAIN_G4 0x80
#define SX1276_LNA_GAIN_G5 0xa0
#define SX1276_LNA_GAIN_G6 0xc0
#define SX1276_LNA_GAIN_LOWEST 0xc0

#define SX1276_DIO_MAPPING1_READ 0x40
#define SX1276_DIO_MAPPING1_WRITE 0xC0
#define SX1276_DIO0_RX_DONE 0x00
#define SX1276_DIO0_TX_DONE 0x40
#define SX1276_DIO0_CAD_DONE 0x80


#define SX1276_MODEM_CONFIG1_READ 0x1D
#define SX1276_MODEM_CONFIG1_WRITE 0x9D

#define MODEM_CONFIG1_HM_EXPLICIT 0x00
#define MODEM_CONFIG1_HM_IMPLICIT 0x01
#define MODEM_CONFIG1_BW_125khz 0x70
#define MODEM_CONFIG1_CR_4_5 0x02

#define SX1276_MODEM_CONFIG2_READ 0x1E
#define SX1276_MODEM_CONFIG2_WRITE 0x9E

#define MODEM_CONFIG2_CRC_ON 0x04
#define MODEM_CONFIG2_CRC_OFF 0x00

#define MODEM_CONFIG2_SF_64 0x60
#define MODEM_CONFIG2_SF_128 0x70
#define MODEM_CONFIG2_SF_256 0x80
#define MODEM_CONFIG2_SF_512 0x90
#define MODEM_CONFIG2_SF_1024 0xa0
#define MODEM_CONFIG2_SF_2048 0xb0
#define MODEM_CONFIG2_SF_4096 0xc0

#define SX1276_MODEM_CONFIG3_READ 0x26
#define SX1276_MODEM_CONFIG3_WRITE 0xa6

#define MODEM_CONFIG3_AGC_ON 0x04
#define MODEM_CONFIG3_AGC_OFF 0x00
#define MODEM_CONFIG3_LDR_OPT 0x08
#define MODEM_CONFIG3_NO_LDR_OPT 0x00

#define SX1276_PAYLOAD_LEN_READ 0x22
#define SX1276_PAYLOAD_LEN_WRITE 0xA2

#define SX1276_MAX_PAYLOAD_LEN_READ 0x23
#define SX1276_MAX_PAYLOAD_LEN_WRITE 0xA3

#define SX1276_FIFO_ADDR_PTR_READ 0x0D
#define SX1276_FIFO_ADDR_PTR_WRITE 0x8D

#define SX1276_FIFO_ADDR_RX_BASE_READ 0x0F
#define SX1276_FIFO_ADDR_RX_BASE_WRITE 0x8F

#define SX1276_FIFO_ADDR_TX_BASE_READ 0x0E
#define SX1276_FIFO_ADDR_TX_BASE_WRITE 0x8E

#define SX1276_FIFO_ADDR_RX_CURRENT_READ 0x10
#define SX1276_FIFO_ADDR_RX_CURRENT_WRITE 0x90

#define SX1276_FIFO_RX_BYTES_NB_READ 0x13
#define SX1276_FIFO_RX_BYTES_NB_WRITE 0x93

#define SX1276_IRQ_FLAGS_READ 0x12
#define SX1276_IRQ_FLAGS_WRITE 0x92

#define IRQ_RX_TIMEOUT 0x80
#define IRQ_RX_DONE 0x40
#define IRQ_PAYLOAD_CRC_ERROR 0x20
#define IRQ_VALID_HEADER 0x10
#define IRQ_TX_DONE 0x08
#define IRQ_CAD_DONE 0x04
#define IRQ_FHSS_CHANGE_CHANNEL 0x02
#define IRQ_CAD_DETECTED 0x01

#define SX1276_FIFO_READ 0x00
#define SX1276_FIFO_WRITE 0x80

#define SX1276_PKT_SNR 0x19
#define SX1276_PKT_RSSI 0x1A

#define SX1276_FEI_LSB 0x2A
#define SX1276_FEI_MID 0x29
#define SX1276_FEI_MSB 0x28

#define SX1276_PPM_CORRECTION 0x27

void SX1276_OnTX(void);
void SX1276_Poll(void);

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
    inputBuffer[0] = convert(inputBuffer[0]);
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
  inputBuffer[0] = convert(inputBuffer[0]);
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
  inputBuffer[0] = convert(inputBuffer[0]);
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

uint8_t tx = 1;

void SX1276_Poll(void)
{
    // Read Int Flags
    uint8_t flags = read(SX1276_IRQ_FLAGS_READ);
    
    if (!tx && (flags & IRQ_RX_DONE)) {
      SX1276_OnRX(flags & IRQ_PAYLOAD_CRC_ERROR);
    }
    if (flags & IRQ_TX_DONE) {
      SX1276_OnTX();
    }
    // Reset IRQs
    write(SX1276_IRQ_FLAGS_READ, flags);
    if (tx && (flags & IRQ_TX_DONE)) {
      // Go to TX
      write(SX1276_OP_MODE_READ, OP_MODE_TX | OP_MODE_HF | OP_MODE_LORA);
    }
}

void SX1276_RX_Reset(void) {  
  // Configure FIFO buffer ptr to be set to RX Base Addr
  write_and_verify(SX1276_FIFO_ADDR_PTR_READ, 0x00);  
  // Configure RX Current Write to be aligned as well
  write_and_verify(SX1276_FIFO_ADDR_RX_CURRENT_READ, 0x00);  
}

void SX1276_TX_Reset(void) {  
  // Configure FIFO buffer ptr to be set to RX Base Addr
  write_and_verify(SX1276_FIFO_ADDR_PTR_READ, 0x80);  
}

uint8_t counter = 0;
void SX1276_OnTX(void) {
  uint8_t outputBuffer[17] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,counter++};
  // move Ptr to beginning of mem
  write(SX1276_FIFO_ADDR_PTR_READ, 0x80);
  
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 
  outputBuffer[0] = SX1276_FIFO_WRITE;
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 17, 1000) != HAL_OK) {
    Error_Handler();
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);  
}

int32_t Ferr_old = 100000; // moved by 100000 to positive side to avoid issues while calc IIR.
uint8_t suspend_afr = 16;

void SX1276_OnRX(uint8_t discard) {
  uint8_t outputBuffer[2] = {0,0};
  uint8_t inputBuffer[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  // Read Current Addr
  uint8_t currentAddr = read(SX1276_FIFO_ADDR_RX_CURRENT_READ);
  
  // move Ptr to current addr
  write(SX1276_FIFO_ADDR_PTR_READ, currentAddr);
  
  // Read number of bytes
  uint8_t bytesCount = read(SX1276_FIFO_RX_BYTES_NB_READ);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 
  outputBuffer[0] = SX1276_FIFO_READ;
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 1, 1000) != HAL_OK) {
    Error_Handler();
  }
  // it should never be > packetSize?
  while (bytesCount > 0) {
    // Read Fifo
    if (HAL_SPI_Receive (&hspi1, inputBuffer, 16, 1000) != HAL_OK) {
      Error_Handler();
    }
    inputBuffer[0] = convert(inputBuffer[0]);
    inputBuffer[1] = convert(inputBuffer[1]);
    inputBuffer[2] = convert(inputBuffer[2]);
    inputBuffer[3] = convert(inputBuffer[3]);
    inputBuffer[4] = convert(inputBuffer[4]);
    inputBuffer[5] = convert(inputBuffer[5]);
    inputBuffer[6] = convert(inputBuffer[6]);
    inputBuffer[7] = convert(inputBuffer[7]);
    inputBuffer[8] = convert(inputBuffer[8]);
    inputBuffer[9] = convert(inputBuffer[9]);
    inputBuffer[10] = convert(inputBuffer[10]);
    inputBuffer[11] = convert(inputBuffer[11]);
    inputBuffer[12] = convert(inputBuffer[12]);
    inputBuffer[13] = convert(inputBuffer[13]);
    inputBuffer[14] = convert(inputBuffer[14]);
    inputBuffer[15] = convert(inputBuffer[15]);    
    bytesCount -= 16;
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

  uint8_t rssi = read(SX1276_PKT_RSSI);
  int8_t snr = read(SX1276_PKT_RSSI);

  int16_t snrDb = snr / 4;
  int16_t rssiDb;
  if( snrDb >= 0) {
    rssiDb = -157 + rssi;
    if (rssiDb > -100) {
      // recalc with slope comp.
      rssiDb = -157 + ((16 * rssi) / 15);      
    }
  } else {
    rssiDb = -157 + rssi + snrDb;
  }
    
  uint8_t fei_msb = read(SX1276_FEI_MSB);
  uint8_t fei_mid = read(SX1276_FEI_MID);
  uint8_t fei_lsb = read(SX1276_FEI_LSB);
  
  int32_t fei = (((fei_msb & 0x0f) << 28) | (fei_mid << 20) | (fei_lsb << 12));
  fei = fei >> 12;
  // Move to positive side to avoid issues while crossing zero
  int32_t Ferr = (fei * 100) / 764  + 100000;
    
#ifdef AFC
  // dont use AFC if we have bad CRC, we will re-enable it once we will have stable link, i.e. 16 non-errored packets.
  if (discard != 0) {
    suspend_afr = 16;
  }
  if (discard == 0 && suspend_afr > 0) {
    suspend_afr--;
  }
  //
  if (suspend_afr == 0) {
    // IIR filter Ferr with Coeff 8
    Ferr_old = (Ferr_old * 7 + Ferr) / 8;
    //
    SX1276_AFC(Ferr_old - 100000);
  }
#endif
}

void SX1276_AFC(int32_t ferr) {    
  frf = (uint32_t)(((int64_t)Ftarget - (int64_t)ferr) * 100000000 / 6103515625);
  int32_t offset = ((int32_t)ferr * 950000) / (int32_t)Ftarget;
  int8_t offset_8 = (int8_t)offset;
  // Set Ferr offset
  write_and_verify(SX1276_PPM_CORRECTION, offset_8);  
  
  uint8_t outputBuffer[4] = {SX1276_FRF_WRITE_MSB, (frf >> 16) & 0xff, (frf >> 8) & 0xff, frf & 0xff};
  uint8_t inputBuffer[3];
  
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 4, 1000) != HAL_OK) {
    Error_Handler();
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);  
  
  // Read FRF
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 
  outputBuffer[0] = SX1276_FRF_READ_MSB;
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 1, 1000) != HAL_OK) {
    Error_Handler();
  }    
  if (HAL_SPI_Receive (&hspi1, inputBuffer, 3, 1000) != HAL_OK) {
    Error_Handler();
  }      
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);                      
  inputBuffer[0] = convert(inputBuffer[0]);
  inputBuffer[1] = convert(inputBuffer[1]);
  inputBuffer[2] = convert(inputBuffer[2]);  
  // assert
  if (inputBuffer[0] != ((frf >> 16) & 0xff) ||
      inputBuffer[1] != ((frf >> 8) & 0xff) ||
        inputBuffer[2] != (frf & 0xff))
  {
    Error_Handler();
  }  
}


uint32_t SX1276_Init(void) {
  uint8_t outputBuffer[4] = {0,0,0,0};
  uint8_t inputBuffer[3];
    
  // Go to sleep and Set Lora mode
  write_and_retry(SX1276_OP_MODE_READ, OP_MODE_SLEEP | OP_MODE_HF | OP_MODE_LORA);  
  write_and_retry(SX1276_OP_MODE_READ, OP_MODE_STDBY | OP_MODE_HF | OP_MODE_LORA);  
  // Reset ALL IRQs
  write(SX1276_IRQ_FLAGS_READ, 0xff);  
  
  // Set FRF
  outputBuffer[0] = SX1276_FRF_WRITE_MSB;
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
  outputBuffer[0] = SX1276_FRF_READ_MSB;
  if (HAL_SPI_Transmit(&hspi1, outputBuffer, 1, 1000) != HAL_OK) {
    Error_Handler();
  }    
  if (HAL_SPI_Receive (&hspi1, inputBuffer, 3, 1000) != HAL_OK) {
    Error_Handler();
  }      
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);                      
  inputBuffer[0] = convert(inputBuffer[0]);
  inputBuffer[1] = convert(inputBuffer[1]);
  inputBuffer[2] = convert(inputBuffer[2]);    
  // assert
  if (inputBuffer[0] != FRF_868_3_MHZ_MSB ||
      inputBuffer[1] != FRF_868_3_MHZ_MID ||
        inputBuffer[2] != FRF_868_3_MHZ_LSB)
  {
    Error_Handler();
  }
  
  // Set power level and select PA_BOOST
  write_and_verify(SX1276_PA_SELECT_READ, PA_SELECT_NO_BOOST_MINUS_4dBm | PA_SELECT_NO_BOOST);
  // Configure OCP
  write_and_verify(SX1276_OCP_READ, SX1276_OCP_ON | SX1276_OCP_TRIM_110ma);  
  // Configure LNA
  write(SX1276_LNA_READ, SX1276_LNA_BOOST_HF_ON | SX1276_LNA_GAIN_HIGHEST);
  // Configure Implicit header mode
  write_and_verify(SX1276_MODEM_CONFIG1_READ, MODEM_CONFIG1_HM_IMPLICIT | MODEM_CONFIG1_BW_125khz | MODEM_CONFIG1_CR_4_5);    
  // Configure CRC ON
  write_and_verify(SX1276_MODEM_CONFIG2_READ, MODEM_CONFIG2_CRC_ON | MODEM_CONFIG2_SF_128);  
  // Configure AGC ON
  write_and_verify(SX1276_MODEM_CONFIG3_READ, MODEM_CONFIG3_AGC_ON | MODEM_CONFIG3_NO_LDR_OPT);  
    
  // configure PayloadLength to be fixed 16 Bytes
  write_and_verify(SX1276_PAYLOAD_LEN_READ, 16);
    
  // configure MaxPayloadLength to be fixed 16 Bytes
  write_and_verify(SX1276_MAX_PAYLOAD_LEN_READ, 16);
    
  // Configure TX Base Addr
  write_and_verify(SX1276_FIFO_ADDR_TX_BASE_READ, 0x80);
    
  // Configure RX Base Addr
  write_and_verify(SX1276_FIFO_ADDR_RX_BASE_READ, 0x00);
  
  if (tx) {
    SX1276_TX_Reset(); // Reset pointers
  } else {
    SX1276_RX_Reset(); // Reset pointers    
  }
  
  if (tx) {
    // Configure DIO0 as TXDONE
    write_and_verify(SX1276_DIO_MAPPING1_READ, SX1276_DIO0_TX_DONE);
  } else {
    // Configure DIO0 as RXDONE
    write_and_verify(SX1276_DIO_MAPPING1_READ, SX1276_DIO0_RX_DONE);
  }
  
  if (tx) {
    // Go to TX
    write(SX1276_OP_MODE_READ, OP_MODE_TX | OP_MODE_HF | OP_MODE_LORA);
  } else {
    // Go to continous RX
    write_and_retry(SX1276_OP_MODE_READ, OP_MODE_RX_CONT | OP_MODE_HF | OP_MODE_LORA);
  }

  return 0;
}
