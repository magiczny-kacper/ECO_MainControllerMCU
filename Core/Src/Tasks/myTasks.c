#include "myTasks.h"

/* USER CODE BEGIN Header_TestTask */
/**
* @brief Function implementing the Test thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TestTask */
void TestTask(void const * argument)
{
  /* USER CODE BEGIN TestTask */

  /* Infinite loop */
  for(;;)
  {

    vTaskDelay(1000);
  }
  /* USER CODE END TestTask */
}
