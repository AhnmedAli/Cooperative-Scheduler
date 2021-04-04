/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct queueNode
{
    void (*que_funcPointer)(void);
    int que_priority; 
    struct queueNode* next;
} readyQueueNode;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
/*APIs defintion*/
void Init(void);

void QueTask(void (*funcPointer)(void),int priority);
void dispatch (void);

/*Queue defintion*/
void queueInsertNewTask(void (*funcPointer)(void),int priority);

/* Tasks definition */
void printHi(void);
void printHow(void);
void printAre(void);
void printYou(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*Array of Pointers to tasks functions*/
void (*tasks_ptr[4])(void)={&printHi,&printHow,&printAre,&printYou};
readyQueueNode ** queueHead_ptr = NULL;
readyQueueNode * queueHead;

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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	
	Init();
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
			dispatch();
			HAL_Delay(1000);

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/**@brief APIs Definition
	*/

void Init(void)
{
		queueInsertNewTask(tasks_ptr[0],3);
		queueInsertNewTask(tasks_ptr[1],2);
		queueInsertNewTask(tasks_ptr[2],4);
		queueInsertNewTask(tasks_ptr[3],1);
}

void QueTask(void (*funcPointer)(void),int priority)
{
	/*Check Task priority Valid or Not and Fix it
		priority>8 is assigned loweest priority "=8" 
		priority <0 assigned Highest priority "=0" */
	if(priority <0)
		priority = 0;
	else if(priority >8)
		priority = 8;
	
	/*Adding Task to the Queue*/
	queueInsertNewTask(funcPointer,priority);	
}

void dispatch (void)
{
	void (*DoTask)(void);
	if(queueHead_ptr!=NULL)
	{
		readyQueueNode* temp = *queueHead_ptr;
		(*queueHead_ptr) = (*queueHead_ptr)->next;
		DoTask = temp -> que_funcPointer;
		DoTask();
		free(temp);
	}
}

void queueInsertNewTask(void (*funcPointer)(void),int priority)
{
	//First Node
	if(queueHead == NULL)
	{
		/*Creating First Node*/
		queueHead = (readyQueueNode*)malloc(sizeof(readyQueueNode));
		queueHead-> que_funcPointer = funcPointer;
		queueHead-> que_priority = priority;
		queueHead-> next = NULL;
		queueHead_ptr = &queueHead;
	}
	else
	{
		/*Creating Node*/
		readyQueueNode* temp = (readyQueueNode*)malloc(sizeof(readyQueueNode));
		temp-> que_funcPointer = funcPointer;
		temp-> que_priority = priority;
		temp-> next = NULL;
		
		/*Adding to queue According to Priority*/
		/*If new task has a higher priority than highest priority task in the queue switch them*/
		if (queueHead->que_priority > priority)
		{
			temp->next = *queueHead_ptr;
			*queueHead_ptr = temp;
		}
		else
		{
			readyQueueNode* queueStart = (*queueHead_ptr);
			/*If new task has a lower priority travarse the queue and add the task according to its priority*/
			while (queueStart->next != NULL && queueStart->next->que_priority < priority)
					queueStart = queueStart->next;
			temp->next = queueStart->next;
			queueStart->next = temp;
		}
	}
}

/**@brief Tasks Initialization
	*/
void printHi(void)
{
	HAL_UART_Transmit(&huart1,(uint8_t *) "HI",2,HAL_MAX_DELAY);
}
void printHow (void)
{
	HAL_UART_Transmit(&huart1,(uint8_t *) "How",3,HAL_MAX_DELAY);
}
void printAre (void)
{
	HAL_UART_Transmit(&huart1,(uint8_t *) "Are",3,HAL_MAX_DELAY);
}
void printYou (void)
{
	HAL_UART_Transmit(&huart1,(uint8_t *) "You",3,HAL_MAX_DELAY);
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
