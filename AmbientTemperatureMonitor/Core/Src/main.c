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
#include "stdlib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
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
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

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
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
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
void Temp(void);
void TempThreshold(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*Array of Pointers to tasks functions*/
void (*tasks_ptr[])(void)={&Temp,&TempThreshold};
uint8_t tempThreshold [3];
uint8_t buffer;
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
  MX_I2C1_Init();
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10909CEC;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
/**@brief APIs Definition
	*/

void Init(void)
{
		HAL_SYSTICK_Config(0x3D08CE);//79999*50
		QueTask(tasks_ptr[0],2);
		QueTask(tasks_ptr[1],1);
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
void Temp(void)
{
	uint8_t controlRegister[2],statusRegister[2],tempUpperByte[2],tempLowerByte[2];
	//Registers config
	statusRegister [0]=0x0F;
	statusRegister [1]=0x00;
	controlRegister[0]=0x0E;
	controlRegister[1]=0x00;
	tempUpperByte[0]=0x11;
	tempUpperByte[1]=0x00;
	tempLowerByte[0]=0x12;
	tempLowerByte[1]=0x00;
	//Receive via I2C and forward to UART
	uint8_t out[] = {'+',0,0,'.',0,0,'°','C','\r','\n'};
	HAL_I2C_Master_Transmit(&hi2c1, 0xD1, statusRegister, 1, 10);
	HAL_I2C_Master_Receive(&hi2c1, 0xD1, statusRegister+1, 1, 10);
	if((statusRegister[1]&0x04) != 0x01)
	{
		HAL_I2C_Master_Receive(&hi2c1, 0xD1, controlRegister, 1, 10);
		controlRegister[1]=controlRegister[1]|0x20;
		HAL_I2C_Master_Transmit(&hi2c1, 0xD0, controlRegister, 1, 10);
		HAL_I2C_Master_Transmit(&hi2c1, 0xD0, tempUpperByte, 1, 10);
		HAL_I2C_Master_Receive(&hi2c1, 0xD1, tempUpperByte+1, 1, 10);
		HAL_I2C_Master_Transmit(&hi2c1, 0xD0, tempLowerByte, 1, 10);
		HAL_I2C_Master_Receive(&hi2c1, 0xD1, tempLowerByte+1, 1, 10);
		if((tempUpperByte[1]>>7)&0x01) out[0] = '-'; else out[0] = '+';
		out[1]=tempUpperByte[1]&0x7F;
		out[4]=25*(tempLowerByte[1]>>6);
		if(out[1]/10!=0)
		{
			out[2]=out[1]%10+'0';
			out[1]=out[1]/10+'0';
		}
		else
		{
			out[2]=out[1]+'0';
			out[1]=0+'0';
		}
		if(out[4]/10!=0)
		{
			out[5]=out[4]%10+'0';
			out[4]=out[4]/10+'0';
		}
		else
		{
			out[5]=out[4]+'0';
			out[4]=0+'0';
		}
		// transmit temp to UART
		HAL_UART_Transmit(&huart1,(uint8_t*) "\n\rCurrent Temprature is: ",25, 10);
		HAL_UART_Transmit(&huart1,out, sizeof(out), 10);
		if(out[0]<=tempThreshold[0] && out[1]>=tempThreshold[1] && out[2]>=tempThreshold[2])
		{
			HAL_UART_Transmit(&huart1,(uint8_t*) "\n\rWARNING!! Temp EXCEDDED THE TEMP THRESHOLD!!\n",48, 10);
			for(int i=0;i<10;i=i+1)
			{
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
				HAL_Delay(5);
			}
		}
		ReRunMe(600);
	}
}
void TempThreshold(void)
{	
	HAL_UART_Transmit(&huart1,(uint8_t*)"\n\rEnter Your temp Threshold ", 28, 10);
	for(int i=0;i<3;i=i+1)
	{
		HAL_UART_Receive(&huart1,&buffer, sizeof(buffer), HAL_MAX_DELAY);
		tempThreshold[i]=buffer;
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
