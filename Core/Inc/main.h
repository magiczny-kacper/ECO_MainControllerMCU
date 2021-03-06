/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void NRF_ChipSelect (void);
void NRF_ChipDeselect (void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BUILT_IN_LED_Pin GPIO_PIN_13
#define BUILT_IN_LED_GPIO_Port GPIOC
#define MASTER_TE_Pin GPIO_PIN_1
#define MASTER_TE_GPIO_Port GPIOA
#define ETH_CS_Pin GPIO_PIN_4
#define ETH_CS_GPIO_Port GPIOA
#define ETH_INT_Pin GPIO_PIN_1
#define ETH_INT_GPIO_Port GPIOB
#define ETH_INT_EXTI_IRQn EXTI1_IRQn
#define DAC_CS_Pin GPIO_PIN_2
#define DAC_CS_GPIO_Port GPIOB
#define PG24V_Pin GPIO_PIN_12
#define PG24V_GPIO_Port GPIOB
#define PG24V_EXTI_IRQn EXTI15_10_IRQn
#define FLASH_CS_Pin GPIO_PIN_13
#define FLASH_CS_GPIO_Port GPIOB
#define NRF_CS_Pin GPIO_PIN_14
#define NRF_CS_GPIO_Port GPIOB
#define NRF_CE_Pin GPIO_PIN_15
#define NRF_CE_GPIO_Port GPIOB
#define NRF_INT_Pin GPIO_PIN_15
#define NRF_INT_GPIO_Port GPIOA
#define NRF_INT_EXTI_IRQn EXTI15_10_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
