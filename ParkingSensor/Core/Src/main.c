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
typedef struct queue2Node
{
    void (*que_funcPointer)(void);
    int ticks; 
    struct queue2Node* next;
} delayedQueueNode;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */
/*Queue Pointers*/
readyQueueNode ** queueHead_ptr = NULL;
readyQueueNode * queueHead = NULL;
delayedQueueNode ** delayedqueueHead_ptr = NULL;
delayedQueueNode * delayedqueueHead = NULL;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */
/*APIs declaration*/
void Init(void);
void QueTask(void (*funcPointer)(void),int priority);
void dispatch (void);
void ReRunMe(int tick);
/*Queue functions declaration*/
void readyQueueInsertNewTask(void (*funcPointer)(void),int priority);
void delayQueueInsertNewTask(void (*funcPointer)(void),int ticks);
void delayQueueTickDec (int ticks_counter);
/* Tasks declaration */
void dist(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*Array of Pointers to tasks functions*/
void (*tasks_ptr[])(void)={&dist};
uint8_t buffer;
uint32_t trigger = 0;
uint32_t echo = 0;
uint32_t difference = 0;
uint8_t isFirstValue = 0;
uint8_t distance  = 0;
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
  MX_TIM1_Init();
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
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 80;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/**@brief APIs Definition
	*/

void Init(void)
{
		HAL_SYSTICK_Config(0x3D08CE);//79999*50
		HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
		QueTask(tasks_ptr[0],1);
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
	readyQueueInsertNewTask(funcPointer,priority);	
}

void dispatch (void)
{
	void (*DoTask)(void);
	if(queueHead!=NULL)
	{
		readyQueueNode* temp = *queueHead_ptr;
		DoTask = temp -> que_funcPointer;
		DoTask();
		(*queueHead_ptr) = (*queueHead_ptr)->next;
		free(temp);
	}
}

void readyQueueInsertNewTask(void (*funcPointer)(void),int priority)
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

void delayQueueInsertNewTask(void (*funcPointer)(void),int ticks)
{
	//First Node
	if(delayedqueueHead == NULL)
	{
		/*Creating First Node*/
		delayedqueueHead = (delayedQueueNode*)malloc(sizeof(delayedQueueNode));
		delayedqueueHead-> que_funcPointer = funcPointer;
		delayedqueueHead-> ticks = ticks;
		delayedqueueHead-> next = NULL;
		delayedqueueHead_ptr = &delayedqueueHead;
	}
	else
	{
		/*Creating Node*/
		delayedQueueNode* temp = (delayedQueueNode*)malloc(sizeof(delayedQueueNode));
		temp-> que_funcPointer = funcPointer;
		temp-> ticks = ticks;
		temp-> next = NULL;
		
		/*Adding to queue According to Priority*/
		/*If new task has a higher priority than highest priority task in the queue switch them*/
		if (delayedqueueHead->ticks > ticks)
		{
			temp-> next = *delayedqueueHead_ptr;
			*delayedqueueHead_ptr = temp;
		}
		else
		{
			delayedQueueNode* queueStart = (*delayedqueueHead_ptr);
			/*If new task has a lower priority travarse the queue and add the task according to its priority*/
			while (queueStart->next != NULL && queueStart->next->ticks < ticks)
					queueStart = queueStart->next;
			temp->next = queueStart->next;
			queueStart->next = temp;
		}
	}
}
void delayQueueTickDec (int ticks_counter)
{
	delayedQueueNode* temp = delayedqueueHead;
	while(temp != NULL)
	{
		temp->ticks-=ticks_counter;
		if(temp->ticks==0)
		{
			QueTask(temp->que_funcPointer,0);
			delayedQueueNode* remove = *delayedqueueHead_ptr;
			(*delayedqueueHead_ptr) = (*delayedqueueHead_ptr)->next;
			free(remove);
		}
		temp=temp->next;
	}
	free(temp);
	ticks_counter=0;
}
void ReRunMe(int tick)
{
	if(tick==0)
		QueTask(queueHead->que_funcPointer,queueHead->que_priority);
	else
		delayQueueInsertNewTask(queueHead->que_funcPointer,tick);

}
/**@brief Tasks Initialization
	*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	//check for interrupt source
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		if (isFirstValue==0)
		{
			trigger = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			isFirstValue = 1;  
			//Capture value from falling edge again
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
		}

		else if (isFirstValue==1) 
		{
			echo = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			__HAL_TIM_SET_COUNTER(htim, 0);
			if (echo > trigger)
				difference = echo-trigger;
			else if (trigger > echo)
				difference = (0xffff - trigger) + echo;
			distance = difference * 0.034/2;
			isFirstValue = 0;
			//Capture value from rising edge again
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(&htim1, TIM_IT_CC1);
		}
	}
}

void delay (uint16_t time)
{
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while (__HAL_TIM_GET_COUNTER (&htim1) < time);
}

void dist(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0,GPIO_PIN_SET);
	delay(10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1,GPIO_PIN_SET);
	delay((400-distance)*2000);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1,GPIO_PIN_RESET);
	delay((400-distance)*2000);
	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1);
	ReRunMe(2);
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
