# Cooperative-Scheduler
The objective of this project is to develop a cooperative scheduler for embedded systems.
# Table of Contents 
 [Introduction](#introduction)

 [Requirements](#requirements)

 [APIs](#apis)

 [Schedular Building Process](#schedular-building-process) 

 [Applications](#applications)

 [References](#references)

# Introduction 
The objective of this short project is to develop a cooperative scheduler for embedded systems. The cooperative scheduler only executes tasks that occur at a time periodic interval. If two tasks are due to run at the same time, the task higher up in the task list runs first followed by the second and so on. Also we provide two main applications an **ambient temprature monitor** and a **parking sensor**.

# Requirements
  ### Softwares
  * STM32CubeMX.
  * Keil uVision5.
  * Tera Term.
  ### Hardware
  * STM32 Nucleo-32 development board with STM32L432KC MCU.
  * USB to TTL bridge.
  * Buzzer.
  * DS3221 -  I2C real-time clock (RTC) with an integrated temperature-compensated crystal oscillator (TCXO) and crystal.
  * HC-sr04 Ultrasound Sensor.

# APIs
+  QueTask(void (*funcPointer)(void) , int priority)** -- Insert Task into the ready queue from (ISRs , Other Tasks). 
	  * void (*funcPointer)(void): Pointer to the function (function name) that implements the task
	  * int priority: Task’s priority.
	 
- **Dispatch(void)** --  Remove the highest priority task from the queue and run it.

+ **ReRunMe(int tick)** -- Insert task into the ready queue called by the task itself.
  + int tick: After expiration of ticks (1 tick = 50ms) the task is added to the ready queue.

# Schedular Building Process
  + ### Tasks
       A function that returns no data and accepts no arguments.
       - Example:
       
              void printHi(void);
              void (*tasks_ptr)(void)={&printHi,&printHow,&printAre,&printYou};
              void printHi(void)
              {
                HAL_UART_Transmit(&huart1,(uint8_t *) "HI",2,100);
              }
	      
          + This is a simple task that prints HI. 
          + void printHi(void) This is task decleration.
          + void (*tasks_ptr)(void)={&printHi,&printHow,&printAre,&printYou}; This is an array of function pointers, it carries the addresses of tasks to be executed.
          + Below it the task defintion i.e what the task does.
          + All tasks should follow the same initialization procedures. 
  - ### Queues
     Queues are implemnetned as a liked list of structs. Each struct is consisted of some variables and a pointer to the next struct.
      - #### Ready Queue
		Ready queue struct consists of function (task) pointer, queue priority and a pointer to the next node.
		```
		typedef struct queueNode
		{
		     void (*que_funcPointer)(void);
		     int que_priority; 
		     struct queueNode* next
		} readyQueueNode;
		```		
     + #### Delay Queue
     	Delay queue struct consists of function (task) pointer, number of ticks and a pointer to the next node.
	      ```
	      typedef struct queue2Node
		{
		    void (*que_funcPointer)(void);
		    int ticks; 
		    struct queue2Node* next;
		} delayedQueueNode;
		```
	
     - ### Task Enqueue and Dequeue.
      * #### Enqueue
		Tasks enter the ready and delay queue by the same way but ready queue compare priorities and delay queue compare number of ticks.
		
          ~~~
               void delayQueueInsertNewTask(void (*funcPointer)(void),int ticks);
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
         ~~~
	 
	* #### Dequeue
	   	Dispatch get the task out of ready queue and run it.
		
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
         
     + ### Ticks.
      	 delayQueueTickDec function decrement ticks in all nodes whenever systick interrupt occurs (handled by systick interrupt handler) and add tasks to ready queue if   	     ticks is equal to 0.
	 
		 ~~~
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
		~~~
  - ### Schedular intialization function
 	This function creates and initializes all needed data structures, where it adjusts the systick tick to 50ms and then insert the tasks into the ready queue.
	
	```
	void Init(void)
	{      
	  HAL_SYSTICK_Config(0x3D08CE);//79999*50       
	  QueTask(tasks_ptr[0],2);
	  QueTask(tasks_ptr[1],1);
	}
	```
# Applications
  ## General Application "Testing"
   ### Objective
   The application send data through UART2 to Tera Term and display the message “Hi How Are You”, then “HI” and “You” messages are added to the ready queue after 40 ticks.
   This	program tests that the scheduler works fine through adding tasks in the ready queue in different order other than their priorities and dequeue elements of different
   priorities at same time from the delay queue.
   
   **NOTE** This program does not need any additional hardware other than the STM32L432KC connected to the device using the USB Port. 

   ### CubeMx General Settings
   Open STM32CubeMX
   
   Select STM32L432KCUx
   
   Set HCLK at 80MHz
   
   ![image](https://user-images.githubusercontent.com/74613419/114302334-e7eae600-9ac8-11eb-95b7-1c775e156043.png)

   Choose SYS-> Debug -> Serial Wire 
   
   ![image](https://user-images.githubusercontent.com/74613419/114302385-1b2d7500-9ac9-11eb-80c5-56aee96370a4.png)

   Select USART2 -> Mode -> Asynchronous 
   
   ![image](https://user-images.githubusercontent.com/74613419/114302420-37c9ad00-9ac9-11eb-88c6-ff71c7e6fb15.png)
   
   Project Manager-> Toolchain/IDE choose MDK-ARM -> V5
   
   Generate CODE
   
   ![image](https://user-images.githubusercontent.com/74613419/114302437-4dd76d80-9ac9-11eb-8977-dc1b47f2724a.png)

   ### Application
	void (*tasks_ptr[])(void)={&printHi,&printHow,&printAre,&printYou};
	void Init(void)
	{
			HAL_SYSTICK_Config(0x3D08CE);//79999*50
			QueTask(tasks_ptr[2],2);
			QueTask(tasks_ptr[1],1);
			QueTask(tasks_ptr[3],3);
			QueTask(tasks_ptr[0],0);
	}
	void printHi(void)
	{
		HAL_UART_Transmit(&huart2,(uint8_t *) "HI",2,100);
		ReRunMe(40);
	}
	void printHow (void)
	{
		HAL_UART_Transmit(&huart2,(uint8_t *) "How",3,100);
	}
	void printAre (void)
	{
		HAL_UART_Transmit(&huart2,(uint8_t *) "Are",3,100);
	}
	void printYou (void)
	{
		HAL_UART_Transmit(&huart2,(uint8_t *) "You\n\r",5,100);
		ReRunMe(40);
	}
  
  “You can use this project to develop your own tasks, just download and run the project under CooperativeSchedularGeneral Folder generate code and replace the above tasks definition with yours or add new tasks in the samy way then build and load the project using keil software”.	
   
  ![image](https://user-images.githubusercontent.com/74613419/114302512-b1619b00-9ac9-11eb-96ed-0186186035ac.png)
 
  ## Ambient Temperature Monitor
   ### Objective
   Read the ambient temperature using a sensor every 30 sec. Produce an alarm
   (LED flashing) when the temperature exceeds a threshold. The threshold is set using a text command sent to the
   embedded system from a PC over an asynchronous serial link
   
   **NOTE** This program requires DS3231 to get the ambient temprature readings and USB to TTL bridge. 
   
   ### CubeMX Additional Settings
   
   Enable I2C1 before generating code and notice SDA and SCL pins. 
   
   I2C1 -> Mode -> I2C.
   
   Enable GPIOB->3 as output to flash the LED when temperature exceed threshold.
   
   ![image](https://user-images.githubusercontent.com/74613419/114302816-0eaa1c00-9acb-11eb-8c42-007422252475.png)

   Use USB to TTL bridge to show data on Tera Term, so we need to enable USART1.
   
   ![image](https://user-images.githubusercontent.com/74613419/114302852-4022e780-9acb-11eb-9951-f5f4cdd5c318.png)

   ### Application
   
   How program works
   
   Allow user to input threshold temperature through terminal using UART1 and set it as a high priority task of priority 1.
   
   Read the ambient temperature using temperature sensor in DS3221 and sit is as a lower priority task of priority 2.
   
   Temperature entry should be in this format **+/- folowed by 2 digits**.

   When ambient temperature exceeds the threshold temperature, the terminal shows a warning message and the led flashes for 5 times. 

   If the next reading is lower than the temperature threshold the led does not flash and the warning message does not show up.

   
   **Connections**
   
   ![image](https://user-images.githubusercontent.com/74613419/114302877-66488780-9acb-11eb-8dec-ce5fd579cd3c.png)
   
   **Tera Term output**
   
   **In case of passing the threshold temperature**
   
   ![ezgif com-gif-maker](https://user-images.githubusercontent.com/74613419/114307657-ecbb9400-9ae0-11eb-9298-d7830574c51a.gif)

   ![image](https://user-images.githubusercontent.com/74613419/114302912-93953580-9acb-11eb-8a07-915af8f94008.png)
   
   **In ideal case**
   
   ![image](https://user-images.githubusercontent.com/74613419/114302922-a1e35180-9acb-11eb-9c27-d571c8f67d08.png)
	

  ## Parking Sensor
   ### Objective
   Produce a sound that reflects how close is the car from the nearest object. A buzzer will be used
   to produce beeps and the duration between the beeps reflects how far is the object. The object distance will be
   determined by the ultrasound sensor.
  
   ### CubeMx Additional Settings
   Allow TIM1 -> Set Channel1 as input Capture Direct mode -> Set Prescaler to 80 “bring the timer clock to 1 MHz since the HCSR04 sends the signal in microseconds”.
   
   Allow TIM1 capture compare interrupt to catch the difference between timestamps representing rising edge and falling edge of ECHO pin output.
   
   Set PA0 as output -> will be used as TRIG Pin.
   
   Set PA1 as output -> generate wave to the buzzer.
   
   **NOTE** You will need to use a passive buzzer, Ultrasound Sensor and no need to enable USART in this application.
   
   ![image](https://user-images.githubusercontent.com/74613419/114304033-5a5fc400-9ad1-11eb-87a0-1e138c423e3f.png)

   ### Application
   Delay function in microseconds
   ~~~
   void delay (uint16_t time)
   {
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while (__HAL_TIM_GET_COUNTER (&htim1) < time);
   }
   ~~~
   
   Function Works in rising edge and falling edge of echo pin output to capture the difference between the timestamps to calculate distance in cm.
   ~~~
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
   ~~~
   
   Function raise the trig pin for 10 microseconds and the pull it low, so the sesnor starts the measurements and GPIOA pin1 is connected to the buzzer to generate buzz at      frequency relative to distance difference.
   ~~~
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
   ~~~
   **Connections**
   
   ![parkingsesnor](https://user-images.githubusercontent.com/74613419/114305226-70708300-9ad7-11eb-88d4-c61e96d35a07.jpeg)

   ### References
   1-STM32L432KC Datasheet
   https://www.st.com/resource/en/datasheet/stm32l432kc.pdf
   
   2-HC-sr04 ultrasonic sensor Datasheet
   https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
   
   3-DS3231
   https://datasheets.maximintegrated.com/en/ds/DS3231-DS3231S.pdf
