/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM3210C_EVAL_H
#define __STM3210C_EVAL_H

#include "stm32l1xx_hal.h"

/*##################### SPI3 ###################################*/
#define EVAL_SPIx                               SPI1
#define EVAL_SPIx_CLK_ENABLE()                  __HAL_RCC_SPI1_CLK_ENABLE()

#define EVAL_SPIx_SCK_GPIO_PORT                 GPIOA             /* PC.10*/
#define EVAL_SPIx_SCK_PIN                       GPIO_PIN_5
#define EVAL_SPIx_SCK_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define EVAL_SPIx_SCK_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOA_CLK_DISABLE()

#define EVAL_SPIx_MISO_MOSI_GPIO_PORT           GPIOA
#define EVAL_SPIx_MISO_MOSI_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()
#define EVAL_SPIx_MISO_MOSI_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOA_CLK_DISABLE()
#define EVAL_SPIx_MISO_PIN                      GPIO_PIN_6       /* PC.11*/
#define EVAL_SPIx_MOSI_PIN                      GPIO_PIN_7       /* PC.12*/
/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the SPI communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define EVAL_SPIx_TIMEOUT_MAX                   1000

/**
  * @}
  */ 

/*##################### SD ###################################*/
/* Chip Select macro definition */
#define SD_CS_LOW()       HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_RESET)
#define SD_CS_HIGH()      HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_SET)

/**
  * @brief  SD Control Interface pins
  */
#define SD_CS_PIN                               GPIO_PIN_4        /* PA.04*/
#define SD_CS_GPIO_PORT                         GPIOA
#define SD_CS_GPIO_CLK_ENABLE()                 __HAL_RCC_GPIOA_CLK_ENABLE()
#define SD_CS_GPIO_CLK_DISABLE()                __HAL_RCC_GPIOA_CLK_DISABLE()

/**
  * @brief  SD Detect Interface pins
  */
#define SD_DETECT_PIN                           GPIO_PIN_0
#define SD_DETECT_GPIO_PORT                     GPIOA
#define SD_DETECT_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOA_CLK_ENABLE()
#define SD_DETECT_GPIO_CLK_DISABLE()            __HAL_RCC_GPIOA_CLK_DISABLE()
#define SD_DETECT_EXTI_IRQn                     EXTI0_IRQn

#endif

