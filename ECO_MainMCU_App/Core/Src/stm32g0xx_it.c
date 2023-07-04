/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32g0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern FDCAN_HandleTypeDef hfdcan1;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart4_rx;
extern DMA_HandleTypeDef hdma_usart4_tx;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern TIM_HandleTypeDef htim17;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/******************************************************************************/
/* STM32G0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line 0 and line 1 interrupts.
  */
void EXTI0_1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_1_IRQn 0 */

  /* USER CODE END EXTI0_1_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(ETH_INT_Pin);
  /* USER CODE BEGIN EXTI0_1_IRQn 1 */

  /* USER CODE END EXTI0_1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel 1 interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 Ch4 to Ch7, DMA2 Ch1 to Ch5 and DMAMUX1 Overrun Interrupts.
  */
void DMA1_Ch4_7_DMA2_Ch1_5_DMAMUX1_OVR_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Ch4_7_DMA2_Ch1_5_DMAMUX1_OVR_IRQn 0 */

  /* USER CODE END DMA1_Ch4_7_DMA2_Ch1_5_DMAMUX1_OVR_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  HAL_DMA_IRQHandler(&hdma_usart3_rx);
  HAL_DMA_IRQHandler(&hdma_usart4_rx);
  HAL_DMA_IRQHandler(&hdma_usart4_tx);
  /* USER CODE BEGIN DMA1_Ch4_7_DMA2_Ch1_5_DMAMUX1_OVR_IRQn 1 */

  /* USER CODE END DMA1_Ch4_7_DMA2_Ch1_5_DMAMUX1_OVR_IRQn 1 */
}

/**
  * @brief This function handles TIM17, FDCAN1_IT1 and FDCAN2_IT1 Interrupt.
  */
void TIM17_FDCAN_IT1_IRQHandler(void)
{
  /* USER CODE BEGIN TIM17_FDCAN_IT1_IRQn 0 */

  /* USER CODE END TIM17_FDCAN_IT1_IRQn 0 */
  HAL_TIM_IRQHandler(&htim17);
  HAL_FDCAN_IRQHandler(&hfdcan1);
  /* USER CODE BEGIN TIM17_FDCAN_IT1_IRQn 1 */

  /* USER CODE END TIM17_FDCAN_IT1_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 + LPUART2 Interrupt.
  */
void USART2_LPUART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_LPUART2_IRQn 0 */

  /* USER CODE END USART2_LPUART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_LPUART2_IRQn 1 */

  /* USER CODE END USART2_LPUART2_IRQn 1 */
}

/**
  * @brief This function handles USART3, USART4, USART5, USART6, LPUART1 globlal Interrupts (combined with EXTI 28).
  */
void USART3_4_5_6_LPUART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_4_5_6_LPUART1_IRQn 0 */

  /* USER CODE END USART3_4_5_6_LPUART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  HAL_UART_IRQHandler(&huart4);
  /* USER CODE BEGIN USART3_4_5_6_LPUART1_IRQn 1 */

  /* USER CODE END USART3_4_5_6_LPUART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
