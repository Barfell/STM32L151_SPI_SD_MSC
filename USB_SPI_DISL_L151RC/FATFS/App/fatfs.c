/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
#include "fatfs.h"

uint8_t retUSER;    /* Return value for USER */
char USERPath[4];   /* USER logical drive path */
FATFS USERFatFS;    /* File system object for USER logical drive */
FIL USERFile;       /* File object for USER */

/* USER CODE BEGIN Variables */
#include "stdio.h"
#include "stm32l1xx_hal.h"
uint8_t Retry = 0;

char test_buffer[32];

UINT s1, s2, cnt;
/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the USER driver ###########################*/
  retUSER = FATFS_LinkDriver(&USER_Driver, USERPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
	Retry = 0;
	do{
		HAL_Delay(100);
		retUSER = f_mount(NULL, USERPath, 1);
		retUSER = f_mount(&USERFatFS, USERPath, 1);
		Retry++;

	}while((retUSER !=FR_OK) && (Retry <= 5));	
	retUSER = f_open(&USERFile,"TEST.tst",FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
	retUSER = f_write(&USERFile,"test",4,&s2);
	f_close(&USERFile);
	retUSER = f_open(&USERFile,"TEST.tst",FA_READ);	
	f_lseek(&USERFile, 0);
	retUSER = f_read(&USERFile,&test_buffer,f_size(&USERFile),&s2);			
	f_close(&USERFile);		
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */

/* USER CODE END Application */
