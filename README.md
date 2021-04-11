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
+ #### void QueTask(void (*funcPointer)(void),int priority)
       This API checks if the priority of the current task is valid or not to fix it, 
       if it is less than 0 it will assign with priority 0
       , and if it is greater than 8 it will assign priority 8
       , and then it will ad the task to the readyqueue
       
       void QueTask(void (*funcPointer)(void),int priority)
       
	
		{
	
			priority>8 is assigned loweest priority "=8" 
	
			priority <0 assigned Highest priority "=0" */
	
			if(priority <0)
	
			priority = 0;
	
			else if(priority >8)
	
			priority = 8;
	
			readyQueueInsertNewTask(funcPointer,priority);	
	
		}
     - #### void dispatch (void)
       This API is used to remove the highest priority task from the queue and run it
       
           It checks first if there is a ready task in the queue to run it, and then point to the following task
	   
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
     - #### void readyQueueInsertNewTask(void (*funcPointer)(void),int priority)
     	 This API checks if the ready queue is empty, and if then it inserts this task on the top of the queue and if not it inserts it in the queue as any task.
	 
	   if(queueHead == NULL)
	   
	   {
	   
		queueHead = (readyQueueNode*)malloc(sizeof(readyQueueNode));
		
		queueHead-> que_funcPointer = funcPointer;
		
		queueHead-> que_priority = priority;
		
		queueHead-> next = NULL;
		
		queueHead_ptr = &queueHead;
		
	   }
	   
	else
	
	   {
		readyQueueNode* temp = (readyQueueNode*)malloc(sizeof(readyQueueNode));
		
		temp-> que_funcPointer = funcPointer;
		
		temp-> que_priority = priority;
		
		temp-> next = NULL;

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
      + ##### Ready queue struct consists of function (task) pointer, queue priority and a pointer to the next node.
        typedef struct queueNode
        {
        
             void (*que_funcPointer)(void);
             
             int que_priority; 
             
             struct queueNode* next
         } readyQueueNode;
      - #### Delay Queue
      + #####  Delay queue struct consists of function (task) pointer, number of ticks and a pointer to the next node.
        typedef struct queue2Node
        
        {
        
            void (*que_funcPointer)(void);
        
            int ticks; 
        
            struct queue2Node* next;
        
        } delayedQueueNode;
  - ### Schedular intialization function 
    - #### void Init(void)
      This function creates and initializes all needed data structures.
       void Init(void)
       
       {
       
       	  HAL_SYSTICK_Config(0x3D08CE);//79999*5
       
          QueTask(tasks_ptr[0],2);
		   
          QueTask(tasks_ptr[1],1);
       
       }
     

# Applications
  ## General Application "Testing"
   CubeMx General Settings
    
   Application
  ## Ambient Temperature Monitor
   CubeMX Additional Settings
    
   Application
  ## Parking Sensor
   CubeMx Additional Settings
   
   Application
