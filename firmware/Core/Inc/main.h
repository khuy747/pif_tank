/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ENC_LEFTA_Pin GPIO_PIN_0
#define ENC_LEFTA_GPIO_Port GPIOA
#define ENC_LEFTB_Pin GPIO_PIN_1
#define ENC_LEFTB_GPIO_Port GPIOA
#define PWM_M1_Pin GPIO_PIN_6
#define PWM_M1_GPIO_Port GPIOA
#define PWM_M2_Pin GPIO_PIN_7
#define PWM_M2_GPIO_Port GPIOA
#define DIR_L1_Pin GPIO_PIN_12
#define DIR_L1_GPIO_Port GPIOB
#define DIR_L2_Pin GPIO_PIN_13
#define DIR_L2_GPIO_Port GPIOB
#define ENC_RIGHTA_Pin GPIO_PIN_6
#define ENC_RIGHTA_GPIO_Port GPIOB
#define ENC_RIGHTB_Pin GPIO_PIN_7
#define ENC_RIGHTB_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
