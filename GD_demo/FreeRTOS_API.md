## 任务管理

#### 动态创建任务

```c
TaskHandle_t StartTask_Handler;						//定义任务句柄
void start_task(void *pvParameters);				//声明函数
xTaskCreate((TaskFunction_t )start_task,			//任务入口地址     
            (const char*    )"start_task",			//任务名称         
            (uint16_t       )128,  					//堆栈大小      
            (void*          )NULL,  				//输入参数          
            (UBaseType_t    )5,						//任务优先级       
            (TaskHandle_t*  )&StartTask_Handler); 	//任务句柄 
vTaskStartScheduler();          					//开启任务调度
```

#### 延时函数

```vTaskDelay(ms);```

#### 任务函数

```c
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    xTaskCreate((TaskFunction_t )print1Task,     	
                (const char*    )"print1Task",   	
                (uint16_t       )128, 
                (void*          )NULL,				
                (UBaseType_t    )3,	
                (TaskHandle_t*  )&print1Task_Handler);   
    xTaskCreate((TaskFunction_t )print2Task,     
                (const char*    )"print2Task",   
                (uint16_t       )128, 
                (void*          )NULL,
                (UBaseType_t    )2,
                (TaskHandle_t*  )&print2Task_Handler); 
	xTaskCreate((TaskFunction_t )led_task,     
                (const char*    )"led_task",   
                (uint16_t       )128, 
                (void*          )NULL,
                (UBaseType_t    )1,
                (TaskHandle_t*  )&ledTask_Handler); 		
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}
```

#### 静态创建任务

```c

//空闲任务任务堆栈
static StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
//空闲任务控制块
static StaticTask_t IdleTaskTCB;

//定时器服务任务堆栈
static StackType_t TimerTaskStack[configTIMER_TASK_STACK_DEPTH];
//定时器服务任务控制块
static StaticTask_t TimerTaskTCB;
//获取空闲任务地任务堆栈和任务控制块内存，因为本例程使用的
//静态内存，因此空闲任务的任务堆栈和任务控制块的内存就应该
//有用户来提供，FreeRTOS提供了接口函数vApplicationGetIdleTaskMemory()
//实现此函数即可。
//ppxIdleTaskTCBBuffer:任务控制块内存
//ppxIdleTaskStackBuffer:任务堆栈内存
//pulIdleTaskStackSize:任务堆栈大小
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
								   StackType_t **ppxIdleTaskStackBuffer, 
								   uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer=&IdleTaskTCB;
	*ppxIdleTaskStackBuffer=IdleTaskStack;
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;
}

//获取定时器服务任务的任务堆栈和任务控制块内存
//ppxTimerTaskTCBBuffer:任务控制块内存
//ppxTimerTaskStackBuffer:任务堆栈内存
//pulTimerTaskStackSize:任务堆栈大小
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
									StackType_t **ppxTimerTaskStackBuffer, 
									uint32_t *pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer=&TimerTaskTCB;
	*ppxTimerTaskStackBuffer=TimerTaskStack;
	*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;
}

StackType_t Task2TaskStack[TASK2_STK_SIZE];	//任务堆栈
StaticTask_t Task2TaskTCB;					//任务控制块
TaskHandle_t Task2Task_Handler;				//任务句柄
void task2_task(void *pvParameters);		//任务函数
Task2Task_Handler=xTaskCreateStatic((TaskFunction_t)task2_task,		//任务函数
		        					(const char* )"task2_task",		//任务名称
									(uint32_t 		)TASK2_STK_SIZE,//堆栈大小
									(void* 		  	)NULL,			//参数
									(UBaseType_t 	)5, 			//优先级
									(StackType_t*   )Task2TaskStack,//任务堆栈
									(StaticTask_t*  )&Task2TaskTCB);//任务控制块   
```

#### 任务挂起

```c
vTaskSuspend(Task1Task_Handler);//挂起任务1
vTaskResume(Task1Task_Handler);	//恢复任务1
vTaskDelete(StartTask_Handler); //删除开始任务
```

#### 任务信息查询

```c
TaskStatus_t TaskStatus;						//任务状态结构体变量
vTaskGetInfo((TaskHandle_t	)TaskHandle, 		//任务句柄
			 (TaskStatus_t*	)&TaskStatus, 		//任务信息结构体
			 (BaseType_t	)pdTRUE,			//允许统计任务堆栈历史最小剩余大小
		     (eTaskState	)eInvalid);			//函数自己获取任务运行壮态
printf("任务名:                %s\r\n",TaskStatus.pcTaskName);
printf("任务编号:              %d\r\n",(int)TaskStatus.xTaskNumber);
printf("任务壮态:              %d\r\n",TaskStatus.eCurrentState);
printf("任务当前优先级:        %d\r\n",(int)TaskStatus.uxCurrentPriority);
printf("任务基优先级:          %d\r\n",(int)TaskStatus.uxBasePriority);
printf("任务堆栈基地址:        %#x\r\n",(int)TaskStatus.pxStackBase);
printf("任务堆栈历史剩余最小值:%d\r\n",TaskStatus.usStackHighWaterMark);
vTaskGetRunTimeStats(RunTimeInfo);//函数自己获取任务运行壮态
printf("任务名\t\t\t运行时间\t运行所占百分比\r\n");
printf("%s\r\n",RunTimeInfo);
```

## 队列

```c
#include "queue.h"
QueueHandle_t Key_Queue=NULL; //定义队列句柄
Key_Queue=xQueueCreate(128,32);  //创建消息Key_Queue 128x32
remain_size=uxQueueSpacesAvailable(Key_Queue);//得到队列剩余大小
BaseType_t err;
err=xQueueSend(Key_Queue,&key,10);//句柄 发送数据地址 等待时间  portMAX_DELAY
BaseType_t = xQueueSendToFront(xQueue,pvItemToQueue,xTicksToWait);
#define errQUEUE_EMPTY	( ( BaseType_t ) 0 )
#define errQUEUE_FULL	( ( BaseType_t ) 0 )
xReturn = xQueueReceive(Key_Queue,&key,portMAX_DELAY) //句柄 接受数据地址 等待时间   
xReturn = xQueueReceiveFromISR( xQueue,&key,portMAX_DELAY);
BaseType_t = xQueueReset( QueueHandle_t pxQueue);//复位队列 
vQueueDelete( xQueue );//删除队列
UBaseType_t=uxQueueMessagesWaiting(xQueue);//返回队列可用数据数量
UBaseType_t= uxQueueSpacesAvailable(xQueue);//返回队列空余空间
```

## 信号量

#### 二值信号量

```c
#include "semphr.h"
SemaphoreHandle_t BinarySem_Handle =NULL;//定义二值信号量变量
BinarySem_Handle = xSemaphoreCreateBinary(); //创建二值信号量
xReturn = xSemaphoreTake(BinarySem_Handle,portMAX_DELAY); //获取信号量
xReturn = xSemaphoreGive( BinarySem_Handle );//释放信号量
```

#### 互斥量

```c
#include "semphr.h"
SemaphoreHandle_t MuxSem_Handle;//定义互斥量变量
MuxSem_Handle= xSemaphoreCreateMutex(); //创建互斥量
xReturn = xSemaphoreTake(MuxSem_Handle,portMAX_DELAY); //获取互斥量 
xReturn = xSemaphoreGive( MuxSem_Handle);//释放互斥量
```

#### 递归互斥量

```c
#include "semphr.h"
emaphoreHandle_t xMutex = NULL;//定义递归互斥量句柄 
xMutex = xSemaphoreCreateRecursiveMutex();//创建递归互斥量
xSemaphoreTakeRecursive( xMutex, ( TickType_t ) 10 ); //获取递归互斥量 句柄 等待时间
xSemaphoreGiveRecursive( xMutex );//释放递归互斥量
```

#### 计数信号量

```c
#include "semphr.h"
SemaphoreHandle_t CountSem_Handle =NULL;//定义计数信号量变量
CountSem_Handle = xSemaphoreCreateCounting(10,10);//创建信号量 最大值 初始值
xReturn = xSemaphoreTake(CountSem_Handle,0); //获取信号量 句柄 等待时间 +1
xReturn = xSemaphoreGive(CountSem_Handle);//释放信号量  句柄
```

```c
#define pdFALSE			( ( BaseType_t ) 0 )
#define pdTRUE			( ( BaseType_t ) 1 )
vSemaphoreDelete(BinarySem_Handle);//删除信号量
```

## 事件组

```c
#include "event_groups.h"
EventGroupHandle_t Event_Handle =NULL; //定义事件组结构体
Event_Handle=  xEventGroupCreate();//创建事件组
vEventGroupDelete(Event_Handle);//删除事件组
EventBits_t r_event; /* 定义一个事件接收变量 */
#define KEY1_EVENT  (0x01 << 0)//设置事件掩码的位0
#define KEY2_EVENT  (0x01 << 1)//设置事件掩码的位1
xEventGroupSetBits(Event_Handle,KEY1_EVENT); //置位时间组位 0-23 返回事件组的值
 r_event = xEventGroupWaitBits(Event_Handle,  /* 事件对象句柄 */
                                  KEY1_EVENT|KEY2_EVENT,/* 接收线程感兴趣的事件 */
                                  pdTRUE,   /* 退出时清除事件位 */
                                  pdTRUE,   //事件位同时发生 与 和 或
                                  portMAX_DELAY);/* 指定超时事件,一直等 */
uxBits = xEventGroupClearBits(Event_Handle, EVENT1 | EVENT2 );//清除事件位   
```

## 任务通知

```c
xTaskNotifyGive( xTask2 );//句柄 向Task2()发送一个任务通知，让其退出阻塞状态 
(uint32_t)  =ulTaskNotifyTake(xClearCountOnExit,xTicksToWait)//1 退出清除通知值 0不清除 等待时间通知值-1  返回当前通知值 在其减 1 或者清 0 之前  
    
xTaskNotify( xTask1Handle, ( 1UL << 8UL ), eSetBits );//任务通知值的位 8 置1
xTaskNotify( xTask2Handle, 0, eNoAction );//解除阻塞，不更新
xTaskNotify( xTask3Handle, 0x50, eSetValueWithOverwrite );//覆盖写
xTaskNotify( xTask3Handle, 0x50, eSetValueWithoutOverwrite);//不覆盖写
xTaskNotifyAndQuery(xTaskToNotify,ulValue,eAction,*pulPreviousNotifyValue )
BaseType_t xTaskNotifyWait(uint32_t ulBitsToClearOnEntry, 
                                  uint32_t ulBitsToClearOnExit, 
                                  uint32_t *pulNotificationValue, 
                                  TickType_t xTicksToWait );
```

## 软件定时器

```c
TimerHandle_t Swtmr1_Handle =NULL; 
Swtmr1_Handle = xTimerCreate(	const char * const pcTimerName		,
							const TickType_t xTimerPeriodInTicks,
							const UBaseType_t uxAutoReload		,
							void * const pvTimerID				,
                			TimerCallbackFunction_t Swtmr1_Callback) 
void Swtmr1_Callback(void* parameter) 
{
     /* 软件定时器的回调函数，用户自己实现 */
}
xTimerStart(Swtmr1_Handle,0);//启动软件定时器 句柄 启动定时器的等待时间
xTimerStop(Swtmr1_Handle,0); //停止软件定时器 句柄 停止定时器的等待时间  
xTimerDelete(Swtmr1_Handle,0); //删除软件定时器 句柄 删除定时器的等待时间
```

switch auto
switch manual
clean
open/close Vent
open/close Servo
open/close Humid
open/close Heat
open/close Water

open/close Light

checkout  XXX	串口任务
串口任务
串口任务
手动控制进程
手动控制进程
手动控制进程
手动控制进程
手动控制进程
自动控制进程	切换自动控制
切换手动控制
清除命令缓冲区
打开/关闭通风扇
打开/关闭通风口
打开/关闭加湿器
打开/关闭加热器
打开/关闭浇水机
切换XXX作物生存环境
