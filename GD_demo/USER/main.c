/*********************************************************************************
  *Copyright(C):  ANHJZU
  *FileName:      Template
  *Author:        Zhihao Nian
  *Version:       V1.0
  *Description:   This template for STM32F407
  *Others:        Non
  *Function List: Non
	*Date:          2022 9 30 
  *History:  
     1.Date:
       Author:
       Modification:
     2.Non
**********************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "stdio.h"	
#include "string.h"	

#include "sys.h" 
#include "oled.h"//PB12（SCL）  PB13（SDA） 
#include "ADC.h"//PA5(sun_sonsor)  PA4(soil_moisture)
#include "DHT11.h"//PB9
#include "pwm.h"//PA6
#include "sgp30.h"
//#include "myiic.h"
#include "usart.h"//PA9接蓝牙RXD  PA10接蓝牙TXD
#include "timer.h"
#include "led.h"
#include "key.h"//A1 A2 A3 A4
#include "FreeRTOS.h"
#include "task.h" 
#include "semphr.h"


#include "VentilationControl.h"
#include "HeatingControl.h"
#include "WateringControl.h"
#include "HumidifierControl.h"

#define DEVICE_OPEN  	(0x00000001 << 0)					//设置设备开启掩码的位0
#define DEVICE_CLOSE   	(0x00000001 << 1)					//设置设备关闭掩码的位1
SemaphoreHandle_t UsartMuxSem_Handle;						//定义串口互斥量变量
void startTask						(void *pvParameters);
TaskHandle_t 						StartTask_Handler;
		
void UsartCommandTask				(void *pvParameters);
TaskHandle_t 						UsartCommandTask_Handler;
		
void DataAcquisitionTask			(void *pvParameters);
TaskHandle_t 						DataAcquisitionTask_Handler;
		
void AutoModeTask					(void *pvParameters);
TaskHandle_t 						AutoModeTask_Handler;
		
void ManualModTask					(void *pvParameters);
TaskHandle_t 						ManualModTask_Handler;
		
void VentilationControlTask			(void *pvParameters);
TaskHandle_t 						VentilationControlTask_Handler;
		
void ServoControlTask				(void *pvParameters);
TaskHandle_t 						ServoControlTask_Handler;
		
void HumidifierControlTask			(void *pvParameters);
TaskHandle_t 						HumidifierControlTask_Handler;
		
void HeatingControlTask				(void *pvParameters);
TaskHandle_t 						HeatingControlTask_Handler;
		
void WateringControlTask			(void *pvParameters);
TaskHandle_t 						WateringControlTask_Handler;
		
void OledDisplayTask				(void *pvParameters);
TaskHandle_t 						OledDisplayTask_Handler;
uint16_t CO2Data = 0;
int main(void)
{ 
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4);
	delay_init(168);
	uart_init(115200);
	LED_Init();
	LED_Close();
	OLED_Init();
	OLED_Clear();
	DHT11_Init();
	Adc_Init();//光敏电阻 通道5
	Adc2_Init();//土壤湿度 通道4
//	adc_DMA_init();
	TIM2_PWM_Init(19999,83);//舵机 PA0 500-2000
	SGP30_Init();
	LED_Open();
	VentilationControl_Init();
	WateringControl_Init();
	HeatingControl_Init();
	HumidifierControl_Init();
	//创建开始任务
	xTaskCreate((TaskFunction_t )startTask,										//任务入口地址           
            (const char*    )"startTask",										//任务名称         
            (uint16_t       )128,  												//任务堆栈大小      
            (void*          )NULL,  											//任务输入参数               
            (UBaseType_t    )5,													//任务优先级       
            (TaskHandle_t*  )&StartTask_Handler); 								//任务句柄    
    vTaskStartScheduler();          
}


void startTask(void *pvParameters) 
{
    taskENTER_CRITICAL();           //进入临界区
	UsartMuxSem_Handle = xSemaphoreCreateMutex(); //创建互斥量
	//创建串口命令任务
	xTaskCreate((TaskFunction_t )UsartCommandTask,								//任务入口地址           
				(const char*    )"UsartCommandTask",							//任务名称         
				(uint16_t       )128,  											//任务堆栈大小      
				(void*          )NULL,  										//任务输入参数               
				(UBaseType_t    )5,												//任务优先级       
				(TaskHandle_t*  )&UsartCommandTask_Handler); 					//任务句柄 
    //创建数据采集任务
	xTaskCreate((TaskFunction_t )DataAcquisitionTask,							//任务入口地址           
				(const char*    )"DataAcquisitionTask",							//任务名称         
				(uint16_t       )512,  											//任务堆栈大小      
				(void*          )NULL,  										//任务输入参数               
				(UBaseType_t    )7,												//任务优先级       
				(TaskHandle_t*  )&DataAcquisitionTask_Handler); 				//任务句柄 
	//创建自动模式任务			
	xTaskCreate((TaskFunction_t )AutoModeTask,									//任务入口地址           
				(const char*    )"AutoModeTask",								//任务名称         
				(uint16_t       )512,  											//任务堆栈大小      
				(void*          )NULL,  										//任务输入参数               
				(UBaseType_t    )5,												//任务优先级       
				(TaskHandle_t*  )&AutoModeTask_Handler); 						//任务句柄 	
	//创建手动模式任务
	xTaskCreate((TaskFunction_t )ManualModTask,									//任务入口地址           
				(const char*    )"ManualModTask",								//任务名称         
				(uint16_t       )512,  											//任务堆栈大小      
				(void*          )NULL,  										//任务输入参数               
				(UBaseType_t    )5,												//任务优先级       
				(TaskHandle_t*  )&ManualModTask_Handler);		 				//任务句柄 
	//创建通风控制任务				
	xTaskCreate((TaskFunction_t )VentilationControlTask,						//任务入口地址           
				(const char*    )"VentilationControlTask",						//任务名称         
				(uint16_t       )128,  											//任务堆栈大小      
				(void*          )NULL,  										//任务输入参数               
				(UBaseType_t    )6,												//任务优先级       
				(TaskHandle_t*  )&VentilationControlTask_Handler); 				//任务句柄 
	//创建舵机控制任务				
	xTaskCreate((TaskFunction_t )ServoControlTask,								//任务入口地址           
				(const char*    )"ServoControlTask",							//任务名称         
				(uint16_t       )128,  											//任务堆栈大小      
				(void*          )NULL,  										//任务输入参数               
				(UBaseType_t    )6,												//任务优先级       
				(TaskHandle_t*  )&ServoControlTask_Handler); 					//任务句柄 	
	//创建加湿器控制任务				
	xTaskCreate((TaskFunction_t )HumidifierControlTask,							//任务入口地址           
				(const char*    )"HumidifierControlTask",						//任务名称         
				(uint16_t       )128,  											//任务堆栈大小      
				(void*          )NULL,  										//任务输入参数               
				(UBaseType_t    )6,												//任务优先级       
				(TaskHandle_t*  )&HumidifierControlTask_Handler); 				//任务句柄 
	//创建加热控制任务				
	xTaskCreate((TaskFunction_t )HeatingControlTask,							//任务入口地址           
				(const char*    )"HeatingControlTask",							//任务名称         
				(uint16_t       )128,  											//任务堆栈大小      
				(void*          )NULL,  										//任务输入参数               
				(UBaseType_t    )6,												//任务优先级       
				(TaskHandle_t*  )&HeatingControlTask_Handler); 					//任务句柄 	
	//创建浇水控制任务				
	xTaskCreate((TaskFunction_t )WateringControlTask,							//任务入口地址           
				(const char*    )"WateringControlTask",							//任务名称         
				(uint16_t       )128,  											//任务堆栈大小      
				(void*          )NULL,  										//任务输入参数               
				(UBaseType_t    )6,												//任务优先级       
				(TaskHandle_t*  )&WateringControlTask_Handler); 				//任务句柄 
	//创建oled显示任务				
	xTaskCreate((TaskFunction_t )OledDisplayTask,								//任务入口地址           
				(const char*    )"OledDisplayTask",								//任务名称         
				(uint16_t       )128,  											//任务堆栈大小      
				(void*          )NULL,  										//任务输入参数               
				(UBaseType_t    )3,												//任务优先级       
				(TaskHandle_t*  )&OledDisplayTask_Handler); 					//任务句柄 

	vTaskSuspend(ManualModTask_Handler);
	vTaskResume(AutoModeTask_Handler);   			
    vTaskDelete(StartTask_Handler); 											//删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}
/*****************串口命令任务******************/
void UsartCommandTask(void *pvParameters)
{
	BaseType_t xReturn = 0;
    while(1)
    {
		if(usart_idle_flag == 1)
		{
			usart_idle_flag = 0 ;
			if(strcmp((const char*)USART_RX_BUF, "switch auto") ==  0)
			{   
				vTaskSuspend(ManualModTask_Handler);
				vTaskResume(AutoModeTask_Handler);
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF));
				rx_cnt = 0; 				
			} 
			else if(strcmp((const char*)USART_RX_BUF, "switch manual") == 0)
			{   
				vTaskSuspend(AutoModeTask_Handler);
				vTaskResume(ManualModTask_Handler);
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0; 

			}
			else if(strcmp((const char*)USART_RX_BUF, "clean") == 0)
			{   
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0; 
			}							
		}
		vTaskDelay(200);	
	}
}	
/*****************数据采集任务******************/
void DataAcquisitionTask(void *pvParameters)
{
	uint32_t sgp30_dat = 0;
    while(1)
    {
		SGP30_Write(0x20,0x08);
		sgp30_dat = SGP30_Read();
		CO2Data = (sgp30_dat & 0xffff0000) >> 16;                                                   
		printf("%d\n",CO2Data);
		vTaskDelay(10000);		
    }
}
/*****************自动模式任务******************/
void AutoModeTask(void *pvParameters)
{
	LED_Open();
	xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 				
	printf("自动模式\n");
	xSemaphoreGive(UsartMuxSem_Handle);
    while(1)
    {	
//		memset(USART_RX_BUF,0,sizeof(USART_RX_BUF));
//		xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
//		printf("错误命令，手动模式不可调节传感器阈值");
//		xSemaphoreGive(UsartMuxSem_Handle);
//		rx_cnt = 0; 
		vTaskDelay(200);		
    }
}
/*****************手动模式任务******************/
void ManualModTask(void *pvParameters)
{
	LED_Close();
	xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
	printf("手动模式\n");
	xSemaphoreGive(UsartMuxSem_Handle);	
    while(1)
    {	
		if(strcmp((const char*)USART_RX_BUF, "open Vent") == 0)
		{   
			xTaskNotify( VentilationControlTask_Handler,DEVICE_OPEN, eSetBits );
			memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
			rx_cnt = 0; 
		}
		else if(strcmp((const char*)USART_RX_BUF, "close Vent") == 0)
		{   
			xTaskNotify( VentilationControlTask_Handler,DEVICE_CLOSE, eSetBits );
			memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
			rx_cnt = 0; 
		}
		else if(strcmp((const char*)USART_RX_BUF, "open Servo") == 0)
		{   
			xTaskNotify( ServoControlTask_Handler,DEVICE_OPEN, eSetBits );
			memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
			rx_cnt = 0; 
		}
		else if(strcmp((const char*)USART_RX_BUF, "close Servo") == 0)
		{   
			xTaskNotify( ServoControlTask_Handler,DEVICE_CLOSE, eSetBits );
			memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
			rx_cnt = 0; 
		}
		else if(strcmp((const char*)USART_RX_BUF, "open Humid") == 0)
		{   
			xTaskNotify( HumidifierControlTask_Handler,DEVICE_OPEN, eSetBits );
			memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
			rx_cnt = 0; 
		}
		else if(strcmp((const char*)USART_RX_BUF, "close Humid") == 0)
		{   
			xTaskNotify( HumidifierControlTask_Handler,DEVICE_CLOSE, eSetBits );
			memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
			rx_cnt = 0; 
		}
		else if(strcmp((const char*)USART_RX_BUF, "open Heat") == 0)
		{   
			xTaskNotify( HeatingControlTask_Handler,DEVICE_OPEN, eSetBits );
			memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
			rx_cnt = 0; 
		}
		else if(strcmp((const char*)USART_RX_BUF, "close Heat") == 0)
		{   
			xTaskNotify( HeatingControlTask_Handler,DEVICE_CLOSE, eSetBits );
			memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
			rx_cnt = 0; 
		}
		else if(strcmp((const char*)USART_RX_BUF, "open Water") == 0)
		{   
			xTaskNotify( HumidifierControlTask_Handler,DEVICE_OPEN, eSetBits );
			memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
			rx_cnt = 0; 
		}
		else if(strcmp((const char*)USART_RX_BUF, "close Water") == 0)
		{   
			xTaskNotify( HumidifierControlTask_Handler,DEVICE_CLOSE, eSetBits );
			memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
			rx_cnt = 0; 
		}		
		vTaskDelay(200);	
    }
}
/*****************通风控制任务******************/
void VentilationControlTask(void *pvParameters)
{
	uint32_t VentilationControl_Notify = 0;
    while(1)
    {	
		xTaskNotifyWait(0,DEVICE_OPEN | DEVICE_CLOSE ,&VentilationControl_Notify,portMAX_DELAY);
		if(VentilationControl_Notify == DEVICE_OPEN)
		{
			VentilationControl_Open();
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("通风机已开启\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
		if(VentilationControl_Notify == DEVICE_CLOSE)
		{
			VentilationControl_Close();
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("通风机已关闭\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
    }
}
/*****************舵机控制任务******************/
void ServoControlTask(void *pvParameters)
{
	uint32_t ServoControl_Notify = 0;
    while(1)
    {	
		xTaskNotifyWait(0,DEVICE_OPEN | DEVICE_CLOSE ,&ServoControl_Notify,portMAX_DELAY);
		if(ServoControl_Notify == DEVICE_OPEN)
		{
			ServoControl_Open();
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("通风口已开启\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
		if(ServoControl_Notify == DEVICE_CLOSE)
		{
			ServoControl_Close();
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("通风口已关闭\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
    }
}
/*****************加湿器控制任务****************/
void HumidifierControlTask(void *pvParameters)
{
	uint32_t HumidifierControl_Notify = 0;
    while(1)
    {	
		xTaskNotifyWait(0,DEVICE_OPEN | DEVICE_CLOSE ,&HumidifierControl_Notify,portMAX_DELAY);
		if(HumidifierControl_Notify == DEVICE_OPEN)
		{
			HumidifierControl_Open();
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("加湿器已开启\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
		if(HumidifierControl_Notify == DEVICE_CLOSE)
		{
			HumidifierControl_Close();
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("加湿器已关闭\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
    }
}
/*****************加热控制任务****************/
void HeatingControlTask(void *pvParameters)
{	
	uint32_t HeatingControl_Notify = 0;
    while(1)
    {	
		xTaskNotifyWait(0,DEVICE_OPEN | DEVICE_CLOSE ,&HeatingControl_Notify,portMAX_DELAY);
		if(HeatingControl_Notify == DEVICE_OPEN)
		{
			HeatingControl_Open();
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("加热已开启\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
		if(HeatingControl_Notify == DEVICE_CLOSE)
		{
			HeatingControl_Close();
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("加热已关闭\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
    }
}
/*****************浇水控制任务****************/
void WateringControlTask(void *pvParameters)
{
	uint32_t WateringControl_Notify = 0;
    while(1)
    {	
		xTaskNotifyWait(0,DEVICE_OPEN | DEVICE_CLOSE ,&WateringControl_Notify,portMAX_DELAY);
		if(WateringControl_Notify == DEVICE_OPEN)
		{
			WateringControl_Open();
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("浇水已开启\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
		if(WateringControl_Notify == DEVICE_CLOSE)
		{
			WateringControl_Close();
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("浇水已关闭\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
    }
}
/*****************OLED显示任务****************/
void OledDisplayTask(void *pvParameters)
{
	
    while(1)
    {	


    }
}
