# Cooperative-Scheduler
# Table of Contents 
### [Introduction](#introduction)
### [Requirements](#requirements)
### [APIs](#apis)
### [Schedular Building Process](#schedular-building-process) 
### [Applications](#applications)

# Introduction 
The objective of this short project is to develop a cooperative scheduler for embedded systems. The cooperative scheduler only executes tasks that occur at a time periodic interval. If two tasks are due to run at the same time, the task higher up in the task list runs first followed by the second and so on. Also we provide two main applications a **ambient temprature monitor** and a **parking sensor**.

# Requirements
  ### Softwares
  * STM32CubeMX.
  * Keil uVision5.
  * Tera Term
  ### Hardware
  * STM32 Nucleo-32 development board with STM32L432KC MCU.
  * USB to TTL bridge
  * Buzzer

# APIs
+  QueTask(void (*funcPointer)(void) , int priority)** -- Insert Task into the ready queue from (ISRs , Other Tasks). 
	  * void (*funcPointer)(void): Pointer to the function (function name) that implements the task
	  * int priority: Task’s priority.
- **Dispatch(void)** --  Remove the highest priority task from the queue and run it.
+ **ReRunMe(int tick)** -- Insert task into the ready queue called by the task itself.
  + int tick: After expiration of ticks (1 tick = 50ms) the task is added to the ready queue.

# Schedular Building Process
  + ### Tasks
       + Is a function that returns no data and accepts no arguments.
       - Example:
       -      void printHi(void);
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
     - #### Delay Queue
      	      Delay queue struct consists of function (task) pointer, number of ticks and a pointer to the next node.
	      ```
	      typedef struct queue2Node
		{
		    void (*que_funcPointer)(void);
		    int ticks; 
		    struct queue2Node* next;
		} delayedQueueNode;
		```
		
  - #### Schedular intialization function
 	This function creates and initializes all needed data structures, where it adjusts the systick tick to 50ms and then insert the tasks into the ready queue.
	```
	void Init(void)
	{      
	  HAL_SYSTICK_Config(0x3D08CE);//79999*50       
	  QueTask(tasks_ptr[0],2);
	  QueTask(tasks_ptr[1],1);
	}
	
# Applications
  ## General Application "Testing"
   CubeMx General Settings
    
   Application
  ## Ambient Temperature Monitor
  + #### Objective
    Read the ambient temperature using a sensor every 30 sec. Produce an alarm
    (LED flashing) when the temperature exceeds a threshold. The threshold is set using a text command sent to the
    embedded system from a PC over an asynchronous serial link
    
   CubeMX Additional Settings
   
   Application
   
  ## Parking Sensor
   + #### Objective
     Produce a sound that reflects how close is the car from the nearest object. A buzzer will be used
     to produce beeps and the duration between the beeps reflects how far is the object. The object distance will be
     determined by the ultrasound sensor.
  
   CubeMx Additional Settings
   
   Application
