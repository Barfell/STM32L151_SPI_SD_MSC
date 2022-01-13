/* Includes ------------------------------------------------------------------*/
#include "stm3210c_eval.h"
#include "stm32l1xx_hal.h"

  
/* LINK SD Card */
#define SD_DUMMY_BYTE      0xFF    
#define SD_NO_RESPONSE_EXPECTED  0x80
 
/**
 * @brief BUS variables
 */

uint32_t SpixTimeout = EVAL_SPIx_TIMEOUT_MAX;        /*<! Value of Timeout when SPI communication fails */
static SPI_HandleTypeDef heval_Spi;


/* SPIx bus function */

static void               SPIx_Init(void);
static void               SPIx_Write(uint8_t Value);
static uint32_t           SPIx_Read(void);
static void               SPIx_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength);
static void               SPIx_WriteData(const uint8_t *Data, uint16_t DataLength);
static void               SPIx_ReadData(const uint8_t *Data, uint16_t DataLength);
static void               SPIx_Error(void);
static void               SPIx_MspInit(void);

/* Link function for LCD peripheral over SPI */
void                      LCD_IO_Init(void);
void                      LCD_IO_WriteMultipleData(uint8_t *pData, uint32_t Size);
void                      LCD_IO_WriteReg(uint8_t Reg);
uint16_t                  LCD_IO_ReadData(uint16_t RegValue);
void                      LCD_Delay (uint32_t delay);

/* Link functions for SD Card peripheral over SPI */
void                      SD_IO_Init(void);
void                      SD_IO_CSState(uint8_t state);
void                      SD_IO_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength);
void                      SD_IO_WriteData(const uint8_t *Data, uint16_t DataLength);
void                      SD_IO_ReadData(const uint8_t *Data, uint16_t DataLength);
uint8_t                   SD_IO_WriteByte(uint8_t Data);


/******************************* SPI Routines**********************************/

/**
  * @brief  Initializes SPI MSP.
  */
static void SPIx_MspInit(void)
{
  GPIO_InitTypeDef  gpioinitstruct = {0};
  
  /*** Configure the GPIOs ***/  
  /* Enable GPIO clock */
  EVAL_SPIx_SCK_GPIO_CLK_ENABLE();
  EVAL_SPIx_MISO_MOSI_GPIO_CLK_ENABLE();
  //__HAL_RCC_AFIO_CLK_ENABLE();
  //__HAL_AFIO_REMAP_SPI3_ENABLE();
  
  /* configure SPI SCK */
  gpioinitstruct.Pin        = EVAL_SPIx_SCK_PIN;
  gpioinitstruct.Mode       = GPIO_MODE_AF_PP;
  gpioinitstruct.Pull       = GPIO_NOPULL;
  gpioinitstruct.Speed      = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(EVAL_SPIx_SCK_GPIO_PORT, &gpioinitstruct);

  /* configure SPI MISO and MOSI */
  gpioinitstruct.Pin        = (EVAL_SPIx_MISO_PIN | EVAL_SPIx_MOSI_PIN);
  gpioinitstruct.Mode       = GPIO_MODE_AF_PP;
  gpioinitstruct.Pull       = GPIO_NOPULL;
  gpioinitstruct.Speed      = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(EVAL_SPIx_MISO_MOSI_GPIO_PORT, &gpioinitstruct);

  /*** Configure the SPI peripheral ***/ 
  /* Enable SPI clock */
  EVAL_SPIx_CLK_ENABLE();
}

/**
  * @brief  Initializes SPI HAL.
  */
static void SPIx_Init(void)
{
  /* DeInitializes the SPI peripheral */
  heval_Spi.Instance = EVAL_SPIx;
  HAL_SPI_DeInit(&heval_Spi);

  /* SPI Config */
  /* SPI baudrate is set to 9 MHz (PCLK2/SPI_BaudRatePrescaler = 72/8 = 9 MHz) */
  heval_Spi.Init.BaudRatePrescaler  = SPI_BAUDRATEPRESCALER_8;
  heval_Spi.Init.Direction          = SPI_DIRECTION_2LINES;
  heval_Spi.Init.CLKPhase           = SPI_PHASE_2EDGE;
  heval_Spi.Init.CLKPolarity        = SPI_POLARITY_HIGH;
  heval_Spi.Init.CRCCalculation     = SPI_CRCCALCULATION_DISABLE;
  heval_Spi.Init.CRCPolynomial      = 7;
  heval_Spi.Init.DataSize           = SPI_DATASIZE_8BIT;
  heval_Spi.Init.FirstBit           = SPI_FIRSTBIT_MSB;
  heval_Spi.Init.NSS                = SPI_NSS_SOFT;
  heval_Spi.Init.TIMode             = SPI_TIMODE_DISABLE;
  heval_Spi.Init.Mode               = SPI_MODE_MASTER;
  
  SPIx_MspInit();
  if (HAL_SPI_Init(&heval_Spi) != HAL_OK)
  {
    /* Should not occur */
    while(1) {};
  }
}

/**
  * @brief SPI Read 4 bytes from device
  * @retval Read data
*/
static uint32_t SPIx_Read(void)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t          readvalue = 0;
  uint32_t          writevalue = 0xFFFFFFFF;
  
  status = HAL_SPI_TransmitReceive(&heval_Spi, (uint8_t*) &writevalue, (uint8_t*) &readvalue, 1, SpixTimeout);
  
  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Execute user timeout callback */
    SPIx_Error();
  }

  return readvalue;
}

/**
  * @brief  SPI Write a byte to device
  * @param  DataIn: value to be written
  * @param  DataOut: value to be read
  * @param  DataLength: length of data
  */
static void SPIx_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_SPI_TransmitReceive(&heval_Spi, (uint8_t*) DataIn, DataOut, DataLength, SpixTimeout);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Execute user timeout callback */
    SPIx_Error();
  }
}

/**
  * @brief  SPI Write Data to device
  * @param  Data: value to be written
  * @param  DataLength: length of data
  */
static void SPIx_WriteData(const uint8_t *Data, uint16_t DataLength)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_SPI_Transmit(&heval_Spi, (uint8_t*) Data, DataLength, SpixTimeout);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Execute user timeout callback */
    SPIx_Error();
  }
}

/**
  * @brief  SPI Read Data from device
  * @param  Data: value to be read
  * @param  DataLength: length of data
  */
static void SPIx_ReadData(const uint8_t *Data, uint16_t DataLength)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_SPI_Receive(&heval_Spi, (uint8_t*) Data, DataLength, SpixTimeout);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Execute user timeout callback */
    SPIx_Error();
  }
}

/**
  * @brief  SPI Write a byte to device.
  * @param  Value: value to be written
  */
static void SPIx_Write(uint8_t Value)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t data;

  status = HAL_SPI_TransmitReceive(&heval_Spi, (uint8_t*) &Value, &data, 1, SpixTimeout);
    
  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Execute user timeout callback */
    SPIx_Error();
  }
}

/**
  * @brief  SPI error treatment function
  * @retval None
  */
static void SPIx_Error (void)
{
  /* De-initialize the SPI communication BUS */
  HAL_SPI_DeInit(&heval_Spi);

  /* Re-Initiaize the SPI communication BUS */
  SPIx_Init();
}

/******************************** LINK SD Card ********************************/

/**
  * @brief  Initializes the SD Card and put it into StandBy State (Ready for 
  *         data transfer).
  */
void SD_IO_Init(void)
{
  GPIO_InitTypeDef  gpioinitstruct;
  uint8_t counter;

  /* SD_CS_GPIO and SD_DETECT_GPIO Periph clock enable */
  SD_CS_GPIO_CLK_ENABLE();
  SD_DETECT_GPIO_CLK_ENABLE();

  /* Configure SD_CS_PIN pin: SD Card CS pin */
  gpioinitstruct.Pin    = SD_CS_PIN;
  gpioinitstruct.Mode   = GPIO_MODE_OUTPUT_PP;
  gpioinitstruct.Pull   = GPIO_PULLUP;
  gpioinitstruct.Speed  = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SD_CS_GPIO_PORT, &gpioinitstruct);

  /* Configure SD_DETECT_PIN pin: SD Card detect pin */
  gpioinitstruct.Pin    = SD_DETECT_PIN;
  gpioinitstruct.Mode   = GPIO_MODE_IT_RISING_FALLING;
  gpioinitstruct.Pull   = GPIO_PULLUP;
  HAL_GPIO_Init(SD_DETECT_GPIO_PORT, &gpioinitstruct);

  /*------------Put SD in SPI mode--------------*/
  /* SD SPI Config */
  SPIx_Init();
  
  /* SD chip select high */
  SD_CS_HIGH();
  
  /* Send dummy byte 0xFF, 10 times with CS high */
  /* Rise CS and MOSI for 80 clocks cycles */
  for (counter = 0; counter <= 9; counter++)
  {
    /* Send dummy byte 0xFF */
    SD_IO_WriteByte(SD_DUMMY_BYTE);
  }
}

/**
  * @brief  Set the SD_CS pin.
  * @param  pin value.
  * @retval None
  */
void SD_IO_CSState(uint8_t val)
{
  if(val == 1) 
  {
    SD_CS_HIGH();
}
  else
  {
    SD_CS_LOW();
  }
}

/**
  * @brief Write a byte on the SD.
  * @param  DataIn: value to be written
  * @param  DataOut: value to be read
  * @param  DataLength: length of data
  */
void SD_IO_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength)
{
  /* Send the byte */
  SPIx_WriteReadData(DataIn, DataOut, DataLength);
}

/**
  * @brief  Write a byte on the SD.
  * @param  Data: value to be written
  * @param  DataLength: length of data
  */
void SD_IO_WriteData(const uint8_t *Data, uint16_t DataLength)
{
  /* Send the byte */
  SPIx_WriteData(Data, DataLength);
}

/**
  * @brief  Read a byte from the SD.
  * @param  Data: value to be read
  * @param  DataLength: length of data
  */
void SD_IO_ReadData(const uint8_t *Data, uint16_t DataLength)
{
  /* Send the byte */
  SPIx_ReadData(Data, DataLength);
}

/**
  * @brief  Writes a byte on the SD.
  * @param  Data: byte to send.
  */
uint8_t SD_IO_WriteByte(uint8_t Data)
{
  uint8_t tmp;
  /* Send the byte */
  SPIx_WriteReadData(&Data,&tmp,1);
  return tmp;
}






