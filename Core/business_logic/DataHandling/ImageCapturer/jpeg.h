/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : libjpeg.h
  * Description        : This file provides code for the configuration
  *                      of the libjpeg instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __libjpeg_H
#define __libjpeg_H
#ifdef __cplusplus
 extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>


/* Includes ------------------------------------------------------------------*/
#include "../Middlewares/Third_Party/LibJPEG/include/jpeglib.h"
 //"jpeglib.h"

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/* Global variables ---------------------------------------------------------*/

/* USER CODE BEGIN 1 */
/* USER CODE END 1 */
 typedef struct
 {
 	 short image_width;
 	 short image_heigh;
 	 uint8_t image_byte_per_pixel;
 	 uint8_t grayscale;
 }ov2640_ImageStateHandle;

 void ov2640_decodeJPEG(uint8_t *image_buffer, uint16_t buffer_length,uint8_t greyscale);
 void ov2640_encodeJPEG(uint8_t **image_buffer, unsigned long *image_size, uint8_t image_quality);
/* LIBJPEG init function */
void MX_LIBJPEG_Init(void);

/* USER CODE BEGIN 2 */
/* USER CODE END 2 */

#ifdef __cplusplus
}
#endif
#endif /*__libjpeg_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
