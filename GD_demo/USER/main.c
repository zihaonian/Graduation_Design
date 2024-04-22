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

#include "usart.h"//PA9接蓝牙RXD  PA10接蓝牙TXD
#include "timer.h"

#include "key.h"//A1 A2 A3 A4
#include "FreeRTOS.h"
#include "task.h" 
#include "semphr.h"


#include "VentilationControl.h"
#include "HeatingControl.h"
#include "WateringControl.h"
#include "HumidifierControl.h"

#define DEVICE_OPEN  	(0x00000001 << 0)		//设置设备开启掩码的位0
#define DEVICE_CLOSE   	(0x00000001 << 1)		//设置设备关闭掩码的位1

typedef struct
{
	uint32_t 	sgp30_dat ;
	uint32_t 	CO2Data ;
	uint16_t 	Soil_Moisture ;
	uint16_t 	Light_Intensity ;
	float   	temperature;
	float  		humidity;
}Env_Data_t;									//环境参数结构体

typedef struct
{
	uint32_t 	MAX_CO2_Concentration ;
	uint32_t 	MIN_CO2_Concentration ;
	uint16_t 	MAX_Soil_Moisture ;
	uint16_t 	MIN_Soil_Moisture ;
	uint16_t 	MAX_Light_Intensity ;
	uint16_t 	MIN_Light_Intensity ;
	float   	MAX_temperature;
	float   	MIN_temperature;
	float  		MAX_humidity;
	float  		MIN_humidity;
}Env_Para_Range_t;								//环境参数结构体

typedef struct
{
	u8	Vent_flag;
	u8	Servo_flag;
	u8	Humidifier_flag;
	u8	Heating_flag;
	u8	Watering_flag;
}Device_status_t;								 //环境参数结构体

Env_Data_t 				Env_Data_Struct			;//定义环境参数结构体变量
Env_Para_Range_t		Env_Para_Range_Struct	;//定义环境参数范围结构体变量
Device_status_t 		Dev_status__Struct 		;//定义设备状态结构体变量
SemaphoreHandle_t 		UsartMuxSem_Handle		;//定义串口互斥量变量
TaskHandle_t			OledDisplayTask_Handler	;

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

int main(void)
{ 
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4);
	delay_init(168);
	uart_init(115200);
	OLED_Init();
	OLED_Clear();
	DHT11_Init();
	Adc_Init();//光敏电阻 通道5 PA5
	Adc2_Init();//土壤湿度 通道4 PA4
	TIM2_PWM_Init(19999,83);//舵机 PA0 500-2000
	SGP30_Init();
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
				
	VentilationControl_Close();
	ServoControl_Close();
	HumidifierControl_Close();
	HeatingControl_Close();
	WateringControl_Close();
				
	Dev_status__Struct.Heating_flag		=0;
	Dev_status__Struct.Humidifier_flag	=0;
	Dev_status__Struct.Servo_flag		=0;
	Dev_status__Struct.Vent_flag		=0;
	Dev_status__Struct.Watering_flag	=0;
				
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
				xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
				printf("缓冲区已清除\n");
				xSemaphoreGive(UsartMuxSem_Handle);
			}							
		}
		vTaskDelay(100);	
	}
}	
/*****************数据采集任务******************/
void DataAcquisitionTask(void *pvParameters)
{
    while(1)
    {
		Env_Data_Struct.Light_Intensity =Get_Adc(5);//采集光照强度
		Env_Data_Struct.Soil_Moisture = Get_Adc2(4);//采集土壤湿度
		DHT11_Read_Data(&(Env_Data_Struct.temperature),&(Env_Data_Struct.humidity));//采集温度，湿度
		SGP30_Write(0x20,0x08);
		Env_Data_Struct.sgp30_dat = SGP30_Read();//采集二氧化碳浓度
		Env_Data_Struct.CO2Data = (Env_Data_Struct.sgp30_dat & 0xffff0000) >> 16;
		vTaskDelay(2000);		
    }
}

/*****************自动模式任务******************/
void AutoModeTask(void *pvParameters)
{
	xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 				
	printf("已切换自动模式\n");
	xSemaphoreGive(UsartMuxSem_Handle);
    while(1)
    {	
		if(strcmp((const char*)USART_RX_BUF, "checkout  ") == 0)
		{
			memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
			rx_cnt = 0;
			Env_Para_Range_Struct.MAX_CO2_Concentration =0 	;
			Env_Para_Range_Struct.MIN_CO2_Concentration =0	;
			Env_Para_Range_Struct.MAX_humidity			=0	;
			Env_Para_Range_Struct.MIN_humidity			=0	;
			Env_Para_Range_Struct.MAX_Light_Intensity   =0	;
			Env_Para_Range_Struct.MIN_Light_Intensity   =0	;
			Env_Para_Range_Struct.MAX_Soil_Moisture		=0	;
			Env_Para_Range_Struct.MIN_Soil_Moisture		=0	;
			Env_Para_Range_Struct.MAX_temperature		=0	;
			Env_Para_Range_Struct.MIN_temperature		=0	;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("自动模式：正在切换至 大蒜 环境\n");
			xSemaphoreGive(UsartMuxSem_Handle);			
		}
		else if(strcmp((const char*)USART_RX_BUF, "checkout  ") == 0)
		{
			memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
			rx_cnt = 0;
			Env_Para_Range_Struct.MAX_CO2_Concentration =0 	;
			Env_Para_Range_Struct.MIN_CO2_Concentration =0	;
			Env_Para_Range_Struct.MAX_humidity			=0	;
			Env_Para_Range_Struct.MIN_humidity			=0	;
			Env_Para_Range_Struct.MAX_Light_Intensity   =0	;
			Env_Para_Range_Struct.MIN_Light_Intensity   =0	;
			Env_Para_Range_Struct.MAX_Soil_Moisture		=0	;
			Env_Para_Range_Struct.MIN_Soil_Moisture		=0	;
			Env_Para_Range_Struct.MAX_temperature		=0	;
			Env_Para_Range_Struct.MIN_temperature		=0	;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("自动模式：正在切换至 大蒜 环境\n");
			xSemaphoreGive(UsartMuxSem_Handle);				
		}
		else if(strcmp((const char*)USART_RX_BUF, "checkout  ") == 0)
		{
			memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
			rx_cnt = 0;
			Env_Para_Range_Struct.MAX_CO2_Concentration =0 	;
			Env_Para_Range_Struct.MIN_CO2_Concentration =0	;
			Env_Para_Range_Struct.MAX_humidity			=0	;
			Env_Para_Range_Struct.MIN_humidity			=0	;
			Env_Para_Range_Struct.MAX_Light_Intensity   =0	;
			Env_Para_Range_Struct.MIN_Light_Intensity   =0	;
			Env_Para_Range_Struct.MAX_Soil_Moisture		=0	;
			Env_Para_Range_Struct.MIN_Soil_Moisture		=0	;
			Env_Para_Range_Struct.MAX_temperature		=0	;
			Env_Para_Range_Struct.MIN_temperature		=0	;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("自动模式：正在切换至 大蒜 环境\n");
			xSemaphoreGive(UsartMuxSem_Handle);				
		}
		else if(strcmp((const char*)USART_RX_BUF, "checkout  ") == 0)
		{
			memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
			rx_cnt = 0;
			Env_Para_Range_Struct.MAX_CO2_Concentration =0 	;
			Env_Para_Range_Struct.MIN_CO2_Concentration =0	;
			Env_Para_Range_Struct.MAX_humidity			=0	;
			Env_Para_Range_Struct.MIN_humidity			=0	;
			Env_Para_Range_Struct.MAX_Light_Intensity   =0	;
			Env_Para_Range_Struct.MIN_Light_Intensity   =0	;
			Env_Para_Range_Struct.MAX_Soil_Moisture		=0	;
			Env_Para_Range_Struct.MIN_Soil_Moisture		=0	;
			Env_Para_Range_Struct.MAX_temperature		=0	;
			Env_Para_Range_Struct.MIN_temperature		=0	;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("自动模式：正在切换至 大蒜 环境\n");
			xSemaphoreGive(UsartMuxSem_Handle);				
		}
//		else 
//		{
//			memset(USART_RX_BUF,0,sizeof(USART_RX_BUF));
//			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
//			printf("错误命令，手动模式不可调节传感器阈值");
//			xSemaphoreGive(UsartMuxSem_Handle);
//			rx_cnt = 0; 	
//		}
		if(Env_Data_Struct.CO2Data > Env_Para_Range_Struct.MAX_CO2_Concentration && (Dev_status__Struct.Servo_flag == 0 || Dev_status__Struct.Vent_flag == 0))
		{
			ServoControl_Open();//开启通风口
			VentilationControl_Open();//开启通风扇
			Dev_status__Struct.Servo_flag = 1;
			Dev_status__Struct.Vent_flag  = 1;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("自动模式：二氧化碳浓度过高，已开启 通风扇与通风口\n");
			xSemaphoreGive(UsartMuxSem_Handle);				
		}
		else if(Env_Data_Struct.CO2Data < Env_Para_Range_Struct.MIN_CO2_Concentration && (Dev_status__Struct.Servo_flag == 1 || Dev_status__Struct.Vent_flag == 1))
		{
			ServoControl_Close();//关闭通风口
			VentilationControl_Close();//关闭通风扇
			Dev_status__Struct.Servo_flag = 0;
			Dev_status__Struct.Vent_flag  = 0;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("自动模式：二氧化碳浓度过低，已关闭 通风扇与通风口\n");
			xSemaphoreGive(UsartMuxSem_Handle);	
		}
		else if(Dev_status__Struct.Servo_flag == 0 || Dev_status__Struct.Vent_flag == 1)
		{
			ServoControl_Open();//开启通风口
			VentilationControl_Close();//关闭通风扇
			Dev_status__Struct.Servo_flag = 1;
			Dev_status__Struct.Vent_flag  = 0;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("自动模式：二氧化碳浓度适宜，已关闭 通风扇与通风口\n");
			xSemaphoreGive(UsartMuxSem_Handle);	
		}
		
		if((Env_Data_Struct.humidity > Env_Para_Range_Struct.MAX_humidity) && (Dev_status__Struct.Humidifier_flag == 1))
		{
			HumidifierControl_Close();//关闭加湿器
			Dev_status__Struct.Humidifier_flag  = 0;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("自动模式：空气湿度过高，已关闭 加湿器 \n");
			xSemaphoreGive(UsartMuxSem_Handle);				
		}
		else if((Env_Data_Struct.humidity < Env_Para_Range_Struct.MIN_humidity) && (Dev_status__Struct.Humidifier_flag == 0))
		{
			HumidifierControl_Open();//开启加湿器
			Dev_status__Struct.Humidifier_flag  = 1;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("自动模式：空气湿度过低，已开启 加湿器 \n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
		else if(Dev_status__Struct.Humidifier_flag == 1)
		{
			HumidifierControl_Close();//关闭加湿器
			Dev_status__Struct.Humidifier_flag  = 0;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("自动模式：空气湿度适宜，已关闭 加湿器 \n");
			xSemaphoreGive(UsartMuxSem_Handle);	
		}
///////////////////////////待修改  光照强度		
		if((Env_Data_Struct.Light_Intensity > Env_Para_Range_Struct.MAX_Light_Intensity) && (Dev_status__Struct.Humidifier_flag == 1))
		{
			HumidifierControl_Close();//关闭加湿器
			Dev_status__Struct.Humidifier_flag  = 0;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("自动模式：光照强度过高，已关闭 补光灯 \n");
			xSemaphoreGive(UsartMuxSem_Handle);				
		}
		else if((Env_Data_Struct.Light_Intensity < Env_Para_Range_Struct.MAX_Light_Intensity) && (Dev_status__Struct.Humidifier_flag == 0))
		{
			HumidifierControl_Open();//开启加湿器
			Dev_status__Struct.Humidifier_flag  = 1;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("自动模式：光照强度过低，已开启 补光灯 \n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
///////////////////////////待修改	////////////////////////////
		
		
		if((Env_Data_Struct.Soil_Moisture > Env_Para_Range_Struct.MAX_Soil_Moisture) && (Dev_status__Struct.Watering_flag == 1))
		{
			WateringControl_Close();//关闭浇水机
			Dev_status__Struct.Watering_flag  = 0;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("自动模式：土壤湿度过高，已关闭 浇水机 \n");
			xSemaphoreGive(UsartMuxSem_Handle);				
		}
		else if((Env_Data_Struct.humidity < Env_Para_Range_Struct.MIN_humidity) && (Dev_status__Struct.Humidifier_flag == 0))
		{
			WateringControl_Open();//开启浇水机
			Dev_status__Struct.Watering_flag  = 1;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("自动模式：土壤湿度过低，已开启 浇水机 \n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
		else if(Dev_status__Struct.Watering_flag == 1)
		{
			WateringControl_Close();//关闭浇水机
			Dev_status__Struct.Watering_flag  = 0;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("自动模式：土壤湿度适宜，已关闭 浇水机 \n");
			xSemaphoreGive(UsartMuxSem_Handle);	
		}

		if((Env_Data_Struct.temperature > Env_Para_Range_Struct.MAX_temperature) && Dev_status__Struct.Servo_flag == 0 && Dev_status__Struct.Vent_flag == 1 && Dev_status__Struct.Heating_flag == 1)
		{
			ServoControl_Open();//开启通风口
			VentilationControl_Open();//开启通风扇
			HeatingControl_Close();//关闭加热器
			Dev_status__Struct.Servo_flag 	= 1;
			Dev_status__Struct.Vent_flag  	= 1;
			Dev_status__Struct.Heating_flag	= 0;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("自动模式：空气温度过高，已开启 通风扇,通风口 已关闭 加热器\n");
			xSemaphoreGive(UsartMuxSem_Handle);				
		}
		else if((Env_Data_Struct.temperature < Env_Para_Range_Struct.MIN_temperature) && Dev_status__Struct.Heating_flag == 0)
		{
			HeatingControl_Open();//开启加热器 
			Dev_status__Struct.Heating_flag  = 1;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("自动模式：空气温度过低，已开启 加热器 \n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
		else if(Dev_status__Struct.Heating_flag == 1)
		{
			HeatingControl_Close();//关闭加热器
			Dev_status__Struct.Heating_flag  = 0;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("自动模式：空气温度适宜,已关闭 加热器 \n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}

		
		vTaskDelay(1000);		
    }
}
/*****************手动模式任务******************/
void ManualModTask(void *pvParameters)
{
	xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
	printf("已切换手动模式\n");
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
		vTaskDelay(100);	
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
			printf("通风扇已开启\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
		if(VentilationControl_Notify == DEVICE_CLOSE)
		{
			VentilationControl_Close();
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("通风扇已关闭\n");
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
			printf("加热器已开启\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
		if(HeatingControl_Notify == DEVICE_CLOSE)
		{
			HeatingControl_Close();
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("加热器已关闭\n");
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
			printf("浇水机已开启\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
		if(WateringControl_Notify == DEVICE_CLOSE)
		{
			WateringControl_Close();
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("浇水机已关闭\n");
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

//int main()
//{	
////	uint32_t sgp30_dat = 0;
////	uint32_t CO2Data =0;
////	uint16_t Soil_Moisture =0;
////	uint16_t Light_Intensity =0;
////	float temperature=0,humidity=0;
//	Environmental_Data_t Environmental_Data_Struct;
//	delay_init(168);
//	SGP30_Init();
//	uart_init(115200);
//	DHT11_Init();
//	Adc_Init();//光敏电阻 通道5 PA5
//	Adc2_Init();//土壤湿度 通道4 PA4
//	while(1)
//	{	Environmental_Data_Struct.Light_Intensity =Get_Adc(5);
//		Environmental_Data_Struct.Soil_Moisture = Get_Adc2(4);
//		DHT11_Read_Data(&(Environmental_Data_Struct.temperature),&(Environmental_Data_Struct.humidity));
//		SGP30_Write(0x20,0x08);
//		Environmental_Data_Struct.sgp30_dat = SGP30_Read();
//		Environmental_Data_Struct.CO2Data = (Environmental_Data_Struct.sgp30_dat & 0xffff0000) >> 16;
//		
//		printf("temperature %f\n",Environmental_Data_Struct.temperature);
//		printf("humidity %f\n",Environmental_Data_Struct.humidity);
//		printf("Light_Intensity %d\n",Environmental_Data_Struct.Light_Intensity);
//		printf("Soil_Moisture %d\n",Environmental_Data_Struct.Soil_Moisture);
//		delay_ms(2000);
//	}
//   
//}