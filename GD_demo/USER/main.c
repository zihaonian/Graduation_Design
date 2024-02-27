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
#include "FreeRTOS.h"
#include "task.h" 
#include "delay.h"
#include "sys.h" 
#include "oled.h"//PB12（SCL）  PB13（SDA） 
#include "ADC.h"//PA5(sun_sonsor)  PA4(soil_moisture)
#include "DHT11.h"//PB9
#include "pwm.h"
#include "stdio.h"	
#include "usart.h"//PA9接蓝牙RXD  PA10接蓝牙TXD
#include "pwm.h" 

#include "timer.h"
#include "led.h"
#include "key.h"//A1 A2 A3 A4

void start_task(void *pvParameters);
void print1Task(void *pvParameters);
void print2Task(void *pvParameters);
void led_task(void *pvParameters);
TaskHandle_t StartTask_Handler;
TaskHandle_t print1Task_Handler;
TaskHandle_t print2Task_Handler;
TaskHandle_t ledTask_Handler;
int main(void)
{ 
	u8 PS_value=0;
	u8 TR_value=0;
	u8 temperature;
	u8 humidity;
	delay_init(168);
	uart_init(115200);
	LED_Init();
	LED_Close();
	OLED_Init();
	OLED_Clear();
	Adc_Init();//光敏电阻 通道5
	DHT11_Init();
//	Adc2_Init();//土壤湿度 通道4
//	adc_DMA_init();
	TIM2_PWM_Init(19999,83);//舵机 PA0 500-2000
	LED_Open();
//	while(1)
//	{
//		printf("sdwasd");
////		PS_value = Get_Adc(5);
////		OLED_ShowNumber(0,1,PS_value,3,12);
////		OLED_ShowNumber(0,2,Adc_Value[1],4,12);
////		OLED_ShowNumber(0,3,Adc_Value[2],4,12);
//		delay_ms(1500);
//		LED_Toggle();
//	}
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )128,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )5,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}


void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建LED0任务
    xTaskCreate((TaskFunction_t )print1Task,     	
                (const char*    )"print1Task",   	
                (uint16_t       )128, 
                (void*          )NULL,				
                (UBaseType_t    )3,	
                (TaskHandle_t*  )&print1Task_Handler);   
    //创建LED1任务
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

void print1Task(void *pvParameters)
{
    while(1)
    {
		printf("123456\n");
        vTaskDelay(100);
    }
}   

//LED1任务函数
void print2Task(void *pvParameters)
{
    while(1)
    {
		printf("789\n");
        vTaskDelay(50);
    }
}
void led_task(void *pvParameters)
{
    while(1)
    {
		LED_Toggle();
        vTaskDelay(500);
    }
}

//		OLED_ShowString(0,0,"vhjhjv",12);
//		delay_ms(1500);
//		OLED_ShowNumber(0,1,1234,4,12);
//		delay_ms(1500);
//		OLED_Float(3,0,13.14,2); 
//		delay_ms(1500);

//		PS_value = Get_Adc(5);
//		OLED_ShowNumber(0,1,PS_value,3,12);
//		delay_ms(500);
//		DHT11_Read_Data(&temperature,&humidity);
//		OLED_ShowNumber(0,2,temperature,3,12);
//		OLED_ShowNumber(0,3,humidity,3,12);
//		TR_value = Get_Adc2(4);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             