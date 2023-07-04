/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bootutil/bootutil.h"
#include "bootutil/image.h"
#include "logging.h"
#include "mcuboot_config/mcuboot_logging.h"
#include "W25Q64_Flash.h"
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
static void do_boot(struct boot_rsp *rsp) {
	EXAMPLE_LOG("Starting Main Application");
	EXAMPLE_LOG("  Image Start Offset: 0x%x", (int)rsp->br_image_off);

  // We run from internal flash. Base address of this medium is 0x0
  uint32_t vector_table = 0x08000000 + rsp->br_image_off + rsp->br_hdr->ih_hdr_size;

  uint32_t *app_code = (uint32_t *)vector_table;
  uint32_t app_sp = app_code[0];
  uint32_t app_start = app_code[1];
  
    EXAMPLE_LOG("  Vector Table Start Address: 0x%x. PC=0x%x, SP=0x%x",
              (int)vector_table, app_start, app_sp);


	  Logging_Wait();
	  HAL_UART_MspDeInit(&huart2);
	  HAL_SPI_DeInit(&hspi2);
	  HAL_RCC_DeInit();
	  HAL_DeInit();

	  // Reset SysTicka
	  SysTick->CTRL = 0;
	  SysTick->LOAD = 0;
	  SysTick->VAL = 0;
      
  // We need to move the vector table to reflect the location of the main application
  SCB->VTOR = (vector_table & 0xFFFFFFF8);

  void (*application_reset_handler)(void) = (void *)app_start;
  __set_MSP(app_sp);
  application_reset_handler();
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  Logging_Init();

  EXAMPLE_LOG("\n\n___  ________ _   _ _                 _   \r\n");
  EXAMPLE_LOG("|  \\/  /  __ \\ | | | |               | |  \r\n");
  EXAMPLE_LOG("| .  . | /  \\/ | | | |__   ___   ___ | |_ \r\n");
  EXAMPLE_LOG("| |\\/| | |   | | | | '_ \\ / _ \\ / _ \\| __|\r\n");
  EXAMPLE_LOG("| |  | | \\__/\\ |_| | |_) | (_) | (_) | |_ \r\n");
  EXAMPLE_LOG("\\_|  |_/\\____/\\___/|_.__/ \\___/ \\___/ \\__|\r\n");

  EXAMPLE_LOG("==Starting Bootloader==\r\n");

  if(W25Q64_Init() == HAL_OK)
  {
	  struct boot_rsp rsp;
	  int rv = boot_go(&rsp);

	  if (rv == 0) {
		  do_boot(&rsp);
	  }
      else
      {
        EXAMPLE_LOG("No bootable image found.");
      }
  }
  else
  {
	  EXAMPLE_LOG("Failed to initialize flsah memory");
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
      HAL_Delay(100);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
      HAL_Delay(200);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
      HAL_Delay(100);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
      HAL_Delay(200);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
      HAL_Delay(100);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
      HAL_Delay(300);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
      HAL_Delay(500);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
      HAL_Delay(200);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
      HAL_Delay(500);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
      HAL_Delay(200);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
      HAL_Delay(500);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
      HAL_Delay(300);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
      HAL_Delay(100);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
      HAL_Delay(200);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
      HAL_Delay(100);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
      HAL_Delay(200);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
      HAL_Delay(100);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
      HAL_Delay(2000);
    /* USER CODE END WHILE */

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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == huart2.Instance)
	{
		Logging_Callback();
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
