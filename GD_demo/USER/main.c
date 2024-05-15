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
#include "MyI2C.h"
#include "OLED.h"//PB8��SCL��  PB9��SDA�� 
#include "ADC.h"//PA5(sun_sonsor)  PA4(soil_moisture)
#include "DHT11.h"//PB9
#include "pwm.h"//PA6
#include "sgp30.h"//PB0(SCL)  PB1(SDA) 

#include "usart.h"//PA9������RXD  PA10������TXD
#include "timer.h"

#include "FreeRTOS.h"
#include "task.h" 
#include "semphr.h"
#include "timers.h"

#include "VentilationControl.h"
#include "HeatingControl.h"
#include "WateringControl.h"
#include "HumidifierControl.h"
#include "Light.h"

#define DEVICE_OPEN  	(0x00000001 << 0)		//�����豸���������λ0
#define DEVICE_CLOSE   	(0x00000001 << 1)		//�����豸�ر������λ1
#define OPEN  	 		1						//�����豸���������λ0
#define CLOSE   		0						//�����豸�ر������λ1
typedef struct
{
	uint32_t 	sgp30_dat ;
	uint32_t 	CO2Data ;
	uint16_t 	Soil_Moisture ;
	uint16_t 	Light_Intensity ;
	float   	temperature;
	float  		humidity;
}Env_Data_t;									//���������ṹ��

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
}Env_Para_Range_t;								//���������ṹ��

typedef struct
{
	u8	Vent_flag;
	u8	Servo_flag;
	u8	Humidifier_flag;
	u8	Heating_flag;
	u8	Watering_flag;
	u8  Light_flag;
}Device_status_t;								 //���������ṹ��

Env_Data_t 				Env_Data_Struct			;//���廷�������ṹ�����
Env_Para_Range_t		Env_Para_Range_Struct	;//���廷��������Χ�ṹ�����
Device_status_t 		Dev_status__Struct 		;//�����豸״̬�ṹ�����
SemaphoreHandle_t 		UsartMuxSem_Handle		;//���崮�ڻ���������

void DataAcquisition_Callback(void* parameter); 
TimerHandle_t DataAcquisition_Handle =NULL; 

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
	
void LightControlTask			(void *pvParameters);
TaskHandle_t 						LightControlTask_Handler;

void OledDisplayTask				(void *pvParameters);
TaskHandle_t			OledDisplayTask_Handler	;
///*****************������******************/
int main(void)
{ 

	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4);
	delay_init(168);
	uart_init(115200);
	DWT_Init();
	

	Adc_Init();//�������� ͨ��5 PA5
	Adc2_Init();//����ʪ�� ͨ��4 PA4
	SGP30_Init();
	DHT11_Init();
	
	IIC_Init();
	OLED_Init();
	
	TIM2_PWM_Init(19999,83);//��� PA0 500-2000
	VentilationControl_Init();
	WateringControl_Init();
	HeatingControl_Init();
	HumidifierControl_Init();
	LightControl_Init();
	//������ʼ����
	xTaskCreate((TaskFunction_t )startTask,										//������ڵ�ַ           
            (const char*    )"startTask",										//��������         
            (uint16_t       )128,  												//�����ջ��С      
            (void*          )NULL,  											//�����������               
            (UBaseType_t    )5,													//�������ȼ�       
            (TaskHandle_t*  )&StartTask_Handler); 								//������    
    vTaskStartScheduler();          
}


/*****************�******************/
void startTask(void *pvParameters) 
{
    taskENTER_CRITICAL();           //�����ٽ���
	UsartMuxSem_Handle = xSemaphoreCreateMutex(); //����������
	//����������������
	xTaskCreate((TaskFunction_t )UsartCommandTask,								//������ڵ�ַ           
				(const char*    )"UsartCommandTask",							//��������         
				(uint16_t       )128,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )2,												//�������ȼ�       
				(TaskHandle_t*  )&UsartCommandTask_Handler); 					//������ 
    //�������ݲɼ�����
	xTaskCreate((TaskFunction_t )DataAcquisitionTask,							//������ڵ�ַ           
				(const char*    )"DataAcquisitionTask",							//��������         
				(uint16_t       )512,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )5,												//�������ȼ�       
				(TaskHandle_t*  )&DataAcquisitionTask_Handler); 				//������ 
	//�����Զ�ģʽ����			
	xTaskCreate((TaskFunction_t )AutoModeTask,									//������ڵ�ַ           
				(const char*    )"AutoModeTask",								//��������         
				(uint16_t       )512,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )2,												//�������ȼ�       
				(TaskHandle_t*  )&AutoModeTask_Handler); 						//������ 	
	//�����ֶ�ģʽ����
	xTaskCreate((TaskFunction_t )ManualModTask,									//������ڵ�ַ           
				(const char*    )"ManualModTask",								//��������         
				(uint16_t       )512,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )2,												//�������ȼ�       
				(TaskHandle_t*  )&ManualModTask_Handler);		 				//������ 
	//����ͨ���������				
	xTaskCreate((TaskFunction_t )VentilationControlTask,						//������ڵ�ַ           
				(const char*    )"VentilationControlTask",						//��������         
				(uint16_t       )128,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )3,												//�������ȼ�       
				(TaskHandle_t*  )&VentilationControlTask_Handler); 				//������ 
	//���������������				
	xTaskCreate((TaskFunction_t )ServoControlTask,								//������ڵ�ַ           
				(const char*    )"ServoControlTask",							//��������         
				(uint16_t       )128,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )3,												//�������ȼ�       
				(TaskHandle_t*  )&ServoControlTask_Handler); 					//������ 	
	//������ʪ����������				
	xTaskCreate((TaskFunction_t )HumidifierControlTask,							//������ڵ�ַ           
				(const char*    )"HumidifierControlTask",						//��������         
				(uint16_t       )128,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )3,												//�������ȼ�       
				(TaskHandle_t*  )&HumidifierControlTask_Handler); 				//������ 
	//�������ȿ�������				
	xTaskCreate((TaskFunction_t )HeatingControlTask,							//������ڵ�ַ           
				(const char*    )"HeatingControlTask",							//��������         
				(uint16_t       )128,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )3,												//�������ȼ�       
				(TaskHandle_t*  )&HeatingControlTask_Handler); 					//������ 	
	//������ˮ��������				
	xTaskCreate((TaskFunction_t )WateringControlTask,							//������ڵ�ַ           
				(const char*    )"WateringControlTask",							//��������         
				(uint16_t       )128,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )3,												//�������ȼ�       
				(TaskHandle_t*  )&WateringControlTask_Handler); 				//������ 
	//���������������				
	xTaskCreate((TaskFunction_t )LightControlTask,								//������ڵ�ַ           
				(const char*    )"LightControlTask",							//��������         
				(uint16_t       )128,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )3,												//�������ȼ�       
				(TaskHandle_t*  )&LightControlTask_Handler); 						//������ 
	//����oled��ʾ����				
	xTaskCreate((TaskFunction_t )OledDisplayTask,								//������ڵ�ַ           
				(const char*    )"OledDisplayTask",								//��������         
				(uint16_t       )512,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )4,												//�������ȼ�       
				(TaskHandle_t*  )&OledDisplayTask_Handler); 					//������ 
//DataAcquisition_Handle = xTimerCreate( 		"DataAcquisition_timer", 
//											2000,
//											pdTRUE,
//											(void *)1,
//											DataAcquisition_Callback );
//	xTimerStart(DataAcquisition_Handle,0);  
	VentilationControl_Close();
	ServoControl_Close();
	HumidifierControl_Close();
	HeatingControl_Close();
	WateringControl_Close();
	LightControl_Close();
				
	Dev_status__Struct.Heating_flag		=0;
	Dev_status__Struct.Humidifier_flag	=0;
	Dev_status__Struct.Servo_flag		=0;
	Dev_status__Struct.Vent_flag		=0;
	Dev_status__Struct.Watering_flag	=0;
	Dev_status__Struct.Light_flag		=0;
				
	Env_Para_Range_Struct.MAX_CO2_Concentration =	1500 	;
	Env_Para_Range_Struct.MIN_CO2_Concentration =	750		;
	Env_Para_Range_Struct.MAX_humidity			=	70		;
	Env_Para_Range_Struct.MIN_humidity			=	60		;
	Env_Para_Range_Struct.MAX_Light_Intensity   =	100		;
	Env_Para_Range_Struct.MIN_Light_Intensity   =	80		;
	Env_Para_Range_Struct.MAX_Soil_Moisture		=	60		;
	Env_Para_Range_Struct.MIN_Soil_Moisture		=	30		;
	Env_Para_Range_Struct.MAX_temperature		=	27		;
	Env_Para_Range_Struct.MIN_temperature		=	23		;		


	vTaskSuspend(AutoModeTask_Handler);
	vTaskResume(ManualModTask_Handler);   			
    vTaskDelete(StartTask_Handler); 											//ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}
/*****************������������******************/
void UsartCommandTask(void *pvParameters)
{
//	BaseType_t xReturn = 0;
    while(1)
    {
		if(usart_idle_flag == 1)
		{
			
			if(strcmp((const char*)USART_RX_BUF, "switch auto") ==  0)
			{   
				vTaskSuspend(ManualModTask_Handler);
				vTaskResume(AutoModeTask_Handler);
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF));
				rx_cnt = 0; 
				usart_idle_flag = 0 ;
				xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 				
				printf("���л��Զ�ģʽ\n");
				xSemaphoreGive(UsartMuxSem_Handle);
			} 
			else if(strcmp((const char*)USART_RX_BUF, "switch manual") == 0)
			{   
				vTaskSuspend(AutoModeTask_Handler);
				vTaskResume(ManualModTask_Handler);
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0; 
				usart_idle_flag = 0 ;
				xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
				printf("���л��ֶ�ģʽ\n");
				xSemaphoreGive(UsartMuxSem_Handle);	
			}
			else if(strcmp((const char*)USART_RX_BUF, "clean") == 0)
			{   
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0; 
				xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
				printf("�����������\n");
				xSemaphoreGive(UsartMuxSem_Handle);
				usart_idle_flag = 0 ;
			}							
		}
		vTaskDelay(100);	
	}
}	
/*****************���ݲɼ�����******************/
void DataAcquisitionTask(void *pvParameters)
{
    while(1)
    {
		xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 	
		DHT11_Read_Data(&(Env_Data_Struct.temperature),&(Env_Data_Struct.humidity));//�ɼ��¶ȣ�ʪ��
		SGP30_Write(0x20,0x08);
		Env_Data_Struct.sgp30_dat = SGP30_Read();//�ɼ�������̼Ũ��
		Env_Data_Struct.CO2Data = (Env_Data_Struct.sgp30_dat & 0xffff0000) >> 16;	
		Env_Data_Struct.Light_Intensity =Get_Adc(5);//�ɼ�����ǿ��
		Env_Data_Struct.Soil_Moisture = Get_Adc2(4);//�ɼ�����ʪ��
				
		printf("T:%.2fC",Env_Data_Struct.temperature);
		printf(" H:%d%%RH\n",(int)Env_Data_Struct.humidity);
		printf("L:%d",Env_Data_Struct.Light_Intensity);
		printf("SM:%d\n",Env_Data_Struct.Soil_Moisture);
		printf("CO2:%dppm\n",Env_Data_Struct.CO2Data);
		printf("����Ƭ:");
		if(Dev_status__Struct.Heating_flag == 1)
		{
			printf("ON\n");
		}
		else
		{
			printf("OFF\n");
		}
		printf("ͨ���:");
		if(Dev_status__Struct.Servo_flag == 1)
		{
			printf("ON\n");
		}
		else
		{
			printf("OFF\n");
		}
		
		printf("���:");
		if(Dev_status__Struct.Vent_flag == 1)
		{
			printf("ON\n");
		}
		else
		{
			printf("OFF\n");
		}
		printf("��ʪ��:");
		if(Dev_status__Struct.Humidifier_flag == 1)
		{
			printf("ON\n");
		}
		else
		{
			printf("OFF\n");
		}
		
		printf("��ˮ��:");
		if(Dev_status__Struct.Watering_flag == 1)
		{
			printf("ON\n");
		}
		else
		{
			printf("OFF\n");
		}
					
		printf("LED:");
		if(Dev_status__Struct.Light_flag == 1)
		{
			printf("ON\n");
		}
		else
		{
			printf("OFF\n");
		}
		xSemaphoreGive(UsartMuxSem_Handle);		
		vTaskDelay(5000);	
    }
}

/*****************�Զ�ģʽ����******************/
void AutoModeTask(void *pvParameters)
{

    while(1)
    {	if(usart_idle_flag == 1)
		{
			if(strcmp((const char*)USART_RX_BUF, "checkout 1 ") == 0)
			{
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0;
				usart_idle_flag = 0 ;
				Env_Para_Range_Struct.MAX_CO2_Concentration =	1500 	;
				Env_Para_Range_Struct.MIN_CO2_Concentration =	750		;
				Env_Para_Range_Struct.MAX_humidity			=	70		;
				Env_Para_Range_Struct.MIN_humidity			=	60		;
				Env_Para_Range_Struct.MAX_Light_Intensity   =	100		;
				Env_Para_Range_Struct.MIN_Light_Intensity   =	80		;
				Env_Para_Range_Struct.MAX_Soil_Moisture		=	60		;
				Env_Para_Range_Struct.MIN_Soil_Moisture		=	30		;
				Env_Para_Range_Struct.MAX_temperature		=	27		;
				Env_Para_Range_Struct.MIN_temperature		=	23		;
				xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
				printf("�Զ�ģʽ�������л��� ���� ����\n");
				xSemaphoreGive(UsartMuxSem_Handle);			
			}
			else if(strcmp((const char*)USART_RX_BUF, "checkout 2 ") == 0)
			{
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0;
				usart_idle_flag = 0 ;
				Env_Para_Range_Struct.MAX_CO2_Concentration =	750 	;
				Env_Para_Range_Struct.MIN_CO2_Concentration =	550		;
				Env_Para_Range_Struct.MAX_humidity			=	80		;
				Env_Para_Range_Struct.MIN_humidity			=	60		;
				Env_Para_Range_Struct.MAX_Light_Intensity   =	70		;
				Env_Para_Range_Struct.MIN_Light_Intensity   =	50		;
				Env_Para_Range_Struct.MAX_Soil_Moisture		=	60		;
				Env_Para_Range_Struct.MIN_Soil_Moisture		=	30		;
				Env_Para_Range_Struct.MAX_temperature		=	22		;
				Env_Para_Range_Struct.MIN_temperature		=	17		;
				xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
				printf("�Զ�ģʽ�������л��� ��ݮ ����\n");
				xSemaphoreGive(UsartMuxSem_Handle);				
			}
			else if(strcmp((const char*)USART_RX_BUF, "checkout 3 ") == 0)
			{
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0;
				usart_idle_flag = 0 ;
				Env_Para_Range_Struct.MAX_CO2_Concentration =	1200 	;
				Env_Para_Range_Struct.MIN_CO2_Concentration =	800		;
				Env_Para_Range_Struct.MAX_humidity			=	70		;
				Env_Para_Range_Struct.MIN_humidity			=	60		;
				Env_Para_Range_Struct.MAX_Light_Intensity   =	80		;
				Env_Para_Range_Struct.MIN_Light_Intensity   =	60		;
				Env_Para_Range_Struct.MAX_Soil_Moisture		=	60		;
				Env_Para_Range_Struct.MIN_Soil_Moisture		=	30		;
				Env_Para_Range_Struct.MAX_temperature		=	20		;
				Env_Para_Range_Struct.MIN_temperature		=	15		;
				xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY)	; 
				printf("�Զ�ģʽ�������л��� ������ ����\n");
				xSemaphoreGive(UsartMuxSem_Handle);				
			}
			else if(strcmp((const char*)USART_RX_BUF, "checkout 4 ") == 0)
			{
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0;
				usart_idle_flag = 0 ;
				Env_Para_Range_Struct.MAX_CO2_Concentration =	2000 	;
				Env_Para_Range_Struct.MIN_CO2_Concentration =	1200	;
				Env_Para_Range_Struct.MAX_humidity			=	50		;
				Env_Para_Range_Struct.MIN_humidity			=	30		;
				Env_Para_Range_Struct.MAX_Light_Intensity   =	100		;
				Env_Para_Range_Struct.MIN_Light_Intensity   =	90		;
				Env_Para_Range_Struct.MAX_Soil_Moisture		=	60		;
				Env_Para_Range_Struct.MIN_Soil_Moisture		=	30		;
				Env_Para_Range_Struct.MAX_temperature		=	20		;
				Env_Para_Range_Struct.MIN_temperature		=	16		;
				xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
				printf("�Զ�ģʽ�������л��� �²� ����\n");
				xSemaphoreGive(UsartMuxSem_Handle);				
			}
		}
//		else 
//		{
//			memset(USART_RX_BUF,0,sizeof(USART_RX_BUF));
//			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
//			printf("��������ֶ�ģʽ���ɵ��ڴ�������ֵ");
//			xSemaphoreGive(UsartMuxSem_Handle);
//			rx_cnt = 0; 	
//		}
		if(Env_Data_Struct.CO2Data > Env_Para_Range_Struct.MAX_CO2_Concentration && (Dev_status__Struct.Servo_flag == 0 || Dev_status__Struct.Vent_flag == 0))
		{
			ServoControl_Open();//����ͨ���
			VentilationControl_Open();//����ͨ����
			Dev_status__Struct.Servo_flag = 1;
			Dev_status__Struct.Vent_flag  = 1;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�Զ�ģʽ��������̼Ũ�ȹ��ߣ��ѿ��� ͨ������ͨ���\n");
			xSemaphoreGive(UsartMuxSem_Handle);				
		}
		else if(Env_Data_Struct.CO2Data < Env_Para_Range_Struct.MIN_CO2_Concentration && (Dev_status__Struct.Servo_flag == 1 || Dev_status__Struct.Vent_flag == 1))
		{
			ServoControl_Close();//�ر�ͨ���
			VentilationControl_Close();//�ر�ͨ����
			Dev_status__Struct.Servo_flag = 0;
			Dev_status__Struct.Vent_flag  = 0;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�Զ�ģʽ��������̼Ũ�ȹ��ͣ��ѹر� ͨ������ͨ���\n");
			xSemaphoreGive(UsartMuxSem_Handle);	
		}
//		else if(Dev_status__Struct.Servo_flag == 0 || Dev_status__Struct.Vent_flag == 1)
//		{
//			ServoControl_Open();//����ͨ���
//			VentilationControl_Close();//�ر�ͨ����
//			Dev_status__Struct.Servo_flag = 1;
//			Dev_status__Struct.Vent_flag  = 0;
//			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
//			printf("�Զ�ģʽ��������̼Ũ�����ˣ��ѹر� ͨ������ͨ���\n");
//			xSemaphoreGive(UsartMuxSem_Handle);	
//		}
		
		if((Env_Data_Struct.humidity > Env_Para_Range_Struct.MAX_humidity) && (Dev_status__Struct.Humidifier_flag == 1))
		{
			HumidifierControl_Close();//�رռ�ʪ��
			Dev_status__Struct.Humidifier_flag  = 0;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�Զ�ģʽ������ʪ�ȹ��ߣ��ѹر� ��ʪ�� \n");
			xSemaphoreGive(UsartMuxSem_Handle);				
		}
		else if((Env_Data_Struct.humidity < Env_Para_Range_Struct.MIN_humidity) && (Dev_status__Struct.Humidifier_flag == 0))
		{
			HumidifierControl_Open();//������ʪ��
			Dev_status__Struct.Humidifier_flag  = 1;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�Զ�ģʽ������ʪ�ȹ��ͣ��ѿ��� ��ʪ�� \n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
//		else if(Dev_status__Struct.Humidifier_flag == 1)
//		{
//			HumidifierControl_Close();//�رռ�ʪ��
//			Dev_status__Struct.Humidifier_flag  = 0;
//			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
//			printf("�Զ�ģʽ������ʪ�����ˣ��ѹر� ��ʪ�� \n");
//			xSemaphoreGive(UsartMuxSem_Handle);	
//		}	
		if((Env_Data_Struct.Light_Intensity > Env_Para_Range_Struct.MAX_Light_Intensity) && (Dev_status__Struct.Light_flag == 1))
		{
			LightControl_Close();//�رղ����
			Dev_status__Struct.Light_flag  = 0;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�Զ�ģʽ������ǿ�ȹ��ߣ��ѹر� ����� \n");
			xSemaphoreGive(UsartMuxSem_Handle);				
		}
		else if((Env_Data_Struct.Light_Intensity < Env_Para_Range_Struct.MIN_Light_Intensity) && (Dev_status__Struct.Light_flag == 0))
		{
			LightControl_Open();//���������
			Dev_status__Struct.Light_flag  = 1;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�Զ�ģʽ������ǿ�ȹ��ͣ��ѿ��� ����� \n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
				
		if((Env_Data_Struct.Soil_Moisture > Env_Para_Range_Struct.MAX_Soil_Moisture) && (Dev_status__Struct.Watering_flag == 1))
		{
			WateringControl_Close();//�رս�ˮ��
			Dev_status__Struct.Watering_flag  = 0;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�Զ�ģʽ������ʪ�ȹ��ߣ��ѹر� ��ˮ�� \n");
			xSemaphoreGive(UsartMuxSem_Handle);				
		}
		else if((Env_Data_Struct.Soil_Moisture < Env_Para_Range_Struct.MIN_Soil_Moisture) && (Dev_status__Struct.Watering_flag == 0))
		{
			WateringControl_Open();//������ˮ��
			Dev_status__Struct.Watering_flag  = 1;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�Զ�ģʽ������ʪ�ȹ��ͣ��ѿ��� ��ˮ�� \n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
//		else if(Dev_status__Struct.Watering_flag == 1)
//		{
//			WateringControl_Close();//�رս�ˮ��
//			Dev_status__Struct.Watering_flag  = 0;
//			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
//			printf("�Զ�ģʽ������ʪ�����ˣ��ѹر� ��ˮ�� \n");
//			xSemaphoreGive(UsartMuxSem_Handle);	
//		}

		if((Env_Data_Struct.temperature > Env_Para_Range_Struct.MAX_temperature) && Dev_status__Struct.Servo_flag == 0 && Dev_status__Struct.Vent_flag == 1 && Dev_status__Struct.Heating_flag == 1)
		{
			ServoControl_Open();//����ͨ���
			VentilationControl_Open();//����ͨ����
			HeatingControl_Close();//�رռ�����
			Dev_status__Struct.Servo_flag 	= 1;
			Dev_status__Struct.Vent_flag  	= 1;
			Dev_status__Struct.Heating_flag	= 0;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�Զ�ģʽ�������¶ȹ��ߣ��ѿ��� ͨ����,ͨ��� �ѹر� ������\n");
			xSemaphoreGive(UsartMuxSem_Handle);				
		}
		else if((Env_Data_Struct.temperature < Env_Para_Range_Struct.MIN_temperature) && Dev_status__Struct.Heating_flag == 0)
		{
			HeatingControl_Open();//���������� 
			Dev_status__Struct.Heating_flag  = 1;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�Զ�ģʽ�������¶ȹ��ͣ��ѿ��� ������ \n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
//		else if(Dev_status__Struct.Heating_flag == 1)
//		{
//			HeatingControl_Close();//�رռ�����
//			Dev_status__Struct.Heating_flag  = 0;
//			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
//			printf("�Զ�ģʽ�������¶�����,�ѹر� ������ \n");
//			xSemaphoreGive(UsartMuxSem_Handle);
//		}

		
		vTaskDelay(1000);		
    }
}
/*****************�ֶ�ģʽ����******************/
void ManualModTask(void *pvParameters)
{
    while(1)
    {	if(usart_idle_flag == 1)
		{
			if(strcmp((const char*)USART_RX_BUF, "open Vent") == 0)
			{   
				xTaskNotify( VentilationControlTask_Handler,DEVICE_OPEN, eSetBits );
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0; 
				usart_idle_flag = 0 ;
			}
			else if(strcmp((const char*)USART_RX_BUF, "close Vent") == 0)
			{   
				xTaskNotify( VentilationControlTask_Handler,DEVICE_CLOSE, eSetBits );
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0; 
				usart_idle_flag = 0 ;
			}
			else if(strcmp((const char*)USART_RX_BUF, "open Servo") == 0)
			{   
				xTaskNotify( ServoControlTask_Handler,DEVICE_OPEN, eSetBits );
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0; 
				usart_idle_flag = 0 ;
			}
			else if(strcmp((const char*)USART_RX_BUF, "close Servo") == 0)
			{   
				xTaskNotify( ServoControlTask_Handler,DEVICE_CLOSE, eSetBits );
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0; 
				usart_idle_flag = 0 ;
			}
			else if(strcmp((const char*)USART_RX_BUF, "open Humid") == 0)
			{   
				xTaskNotify( HumidifierControlTask_Handler,DEVICE_OPEN, eSetBits );
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0;
				usart_idle_flag = 0 ;				
			}
			else if(strcmp((const char*)USART_RX_BUF, "close Humid") == 0)
			{   
				xTaskNotify( HumidifierControlTask_Handler,DEVICE_CLOSE, eSetBits );
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0; 
				usart_idle_flag = 0 ;
			}
			else if(strcmp((const char*)USART_RX_BUF, "open Heat") == 0)
			{   
				xTaskNotify( HeatingControlTask_Handler,DEVICE_OPEN, eSetBits );
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0; 
				usart_idle_flag = 0 ;
			}
			else if(strcmp((const char*)USART_RX_BUF, "close Heat") == 0)
			{   
				xTaskNotify( HeatingControlTask_Handler,DEVICE_CLOSE, eSetBits );
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0; 
				usart_idle_flag = 0 ;
			}
			else if(strcmp((const char*)USART_RX_BUF, "open Water") == 0)
			{   
				xTaskNotify( WateringControlTask_Handler,DEVICE_OPEN, eSetBits );
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0; 
				usart_idle_flag = 0 ;
			}
			else if(strcmp((const char*)USART_RX_BUF, "close Water") == 0)
			{   
				xTaskNotify( WateringControlTask_Handler,DEVICE_CLOSE, eSetBits );
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0; 
				usart_idle_flag = 0 ;
			}	
			else if(strcmp((const char*)USART_RX_BUF, "open Light") == 0)
			{   
				xTaskNotify( LightControlTask_Handler,DEVICE_OPEN, eSetBits );
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0; 
				usart_idle_flag = 0 ;
			}
			else if(strcmp((const char*)USART_RX_BUF, "close Light") == 0)
			{   
				xTaskNotify( LightControlTask_Handler,DEVICE_CLOSE, eSetBits );
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0; 
				usart_idle_flag = 0 ;
			}	
		}			
		vTaskDelay(100);	
    }
}
/*****************ͨ���������******************/
void VentilationControlTask(void *pvParameters)
{
	uint32_t VentilationControl_Notify = 0;
    while(1)
    {	
		xTaskNotifyWait(0,DEVICE_OPEN | DEVICE_CLOSE ,&VentilationControl_Notify,portMAX_DELAY);
		if(VentilationControl_Notify == DEVICE_OPEN)
		{
			VentilationControl_Open();
			Dev_status__Struct.Vent_flag = OPEN;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�ֶ����� ͨ�����ѿ�\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
		if(VentilationControl_Notify == DEVICE_CLOSE)
		{
			VentilationControl_Close();
			Dev_status__Struct.Vent_flag = CLOSE;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�ֶ����� ͨ�����ѹ�\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
    }
}

/*****************�����������******************/
void ServoControlTask(void *pvParameters)
{
	uint32_t ServoControl_Notify = 0;
    while(1)
    {	
		xTaskNotifyWait(0,DEVICE_OPEN | DEVICE_CLOSE ,&ServoControl_Notify,portMAX_DELAY);
		if(ServoControl_Notify == DEVICE_OPEN)
		{
			ServoControl_Open();
			Dev_status__Struct.Servo_flag = OPEN;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�ֶ����� ͨ����ѿ�\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
		if(ServoControl_Notify == DEVICE_CLOSE)
		{
			ServoControl_Close();
			Dev_status__Struct.Servo_flag = CLOSE;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�ֶ����� ͨ����ѹ�\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
    }
}
/*****************��ʪ����������****************/
void HumidifierControlTask(void *pvParameters)
{
	uint32_t HumidifierControl_Notify = 0;
    while(1)
    {	
		xTaskNotifyWait(0,DEVICE_OPEN | DEVICE_CLOSE ,&HumidifierControl_Notify,portMAX_DELAY);
		if(HumidifierControl_Notify == DEVICE_OPEN)
		{
			HumidifierControl_Open();
			Dev_status__Struct.Humidifier_flag = OPEN;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�ֶ����� ��ʪ���ѿ�\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
		if(HumidifierControl_Notify == DEVICE_CLOSE)
		{
			HumidifierControl_Close();
			Dev_status__Struct.Humidifier_flag = CLOSE;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�ֶ����� ��ʪ���ѹ�\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
    }
}

/*****************���ȿ�������****************/
void HeatingControlTask(void *pvParameters)
{	
	uint32_t HeatingControl_Notify = 0;
    while(1)
    {	
		xTaskNotifyWait(0,DEVICE_OPEN | DEVICE_CLOSE ,&HeatingControl_Notify,portMAX_DELAY);
		if(HeatingControl_Notify == DEVICE_OPEN)
		{
			HeatingControl_Open();
			Dev_status__Struct.Heating_flag = OPEN;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�ֶ����� �������ѿ�\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
		if(HeatingControl_Notify == DEVICE_CLOSE)
		{
			HeatingControl_Close();
			Dev_status__Struct.Heating_flag = CLOSE;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�ֶ����� �������ѹ�\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
    }
}
/*****************��ˮ��������****************/
void WateringControlTask(void *pvParameters)
{
	uint32_t WateringControl_Notify = 0;
    while(1)
    {	
		xTaskNotifyWait(0,DEVICE_OPEN | DEVICE_CLOSE ,&WateringControl_Notify,portMAX_DELAY);
		if(WateringControl_Notify == DEVICE_OPEN)
		{
			WateringControl_Open();
			Dev_status__Struct.Watering_flag = OPEN;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�ֶ����� ��ˮ���ѿ�\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
		if(WateringControl_Notify == DEVICE_CLOSE)
		{
			WateringControl_Close();
			Dev_status__Struct.Watering_flag = CLOSE;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�ֶ����� ��ˮ���ѹ�\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
    }
}
/****************�����������****************/
void LightControlTask(void *pvParameters)
{
	uint32_t LightControl_Notify = 0;
    while(1)
    {	
		xTaskNotifyWait(0,DEVICE_OPEN | DEVICE_CLOSE ,&LightControl_Notify,portMAX_DELAY);
		if(LightControl_Notify == DEVICE_OPEN)
		{
			LightControl_Open();
			Dev_status__Struct.Light_flag = OPEN;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�ֶ����� ������ѿ�\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
		if(LightControl_Notify == DEVICE_CLOSE)
		{
			LightControl_Close();
			Dev_status__Struct.Light_flag = CLOSE;
			xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
			printf("�ֶ����� ������ѹ�\n");
			xSemaphoreGive(UsartMuxSem_Handle);
		}
    }
}
/*****************OLED��ʾ����****************/
void OledDisplayTask(void *pvParameters)
{
	uint16_t cnt = 0;
    while(1)
    {	cnt ++ ;
		if(cnt < 11)
		{				
			OLED_ShowString(1, 1,"T:");
			OLED_ShowString(1, 5,".");
			OLED_ShowString(1, 7,"C  ");
			OLED_ShowString(1, 10,"H:");
			OLED_ShowString(1, 14,"%RH");
			OLED_ShowString(2, 1,"Light:");
			OLED_ShowString(2, 9,"        ");
			OLED_ShowString(3, 1,"CO2  :");
			OLED_ShowString(3, 11," ppm  ");
			OLED_ShowString(4, 1,"SM   :");
			OLED_ShowString(4, 9,"   %RH  ");
			OLED_ShowNum(1, 3, (int)Env_Data_Struct.temperature, 2);
			OLED_ShowNum(1, 6, (int)(Env_Data_Struct.temperature*100)%100, 1);
			OLED_ShowNum(1, 12, Env_Data_Struct.humidity, 2);
			OLED_ShowNum(2, 7, Env_Data_Struct.Light_Intensity, 2);
			OLED_ShowNum(3, 7, Env_Data_Struct.CO2Data, 4);
			OLED_ShowNum(4, 7, Env_Data_Struct.Soil_Moisture, 2);
			vTaskDelay(500);	
		}
		else
		{
			OLED_ShowString(1, 1,"Heat:");
			if(Dev_status__Struct.Heating_flag == 1)
			{
				OLED_ShowString(1, 6,"ON ");
			}
			else
			{
				OLED_ShowString(1, 6,"OF ");
			}
			OLED_ShowString(1, 9,"Servo:");
			if(Dev_status__Struct.Servo_flag == 1)
			{
				OLED_ShowString(1, 15,"ON");
			}
			else
			{
				OLED_ShowString(1, 15,"OF");
			}
			
			OLED_ShowString(2, 1,"Vent:");
			if(Dev_status__Struct.Vent_flag == 1)
			{
				OLED_ShowString(2, 6,"ON ");
			}
			else
			{
				OLED_ShowString(2, 6,"OF ");
			}
			OLED_ShowString(2, 9,"Humid:");
			if(Dev_status__Struct.Humidifier_flag == 1)
			{
				OLED_ShowString(2, 15,"ON");
			}
			else
			{
				OLED_ShowString(2, 15,"OF");
			}
			
			OLED_ShowString(3, 1,"Water:");
			if(Dev_status__Struct.Watering_flag == 1)
			{
				OLED_ShowString(3, 7,"ON        ");
			}
			else
			{
				OLED_ShowString(3, 7,"OF        ");
			}
						
			OLED_ShowString(4, 1,"Light:");
			if(Dev_status__Struct.Light_flag == 1)
			{
				OLED_ShowString(4, 7,"ON        ");
			}
			else
			{
				OLED_ShowString(4, 7,"OF        ");
			}
			vTaskDelay(500);	
		}
		
		if(cnt == 19)
		{
			cnt = 0 ;
		}

    }
}

//void DataAcquisition_Callback(void* parameter)
//{
//		    taskENTER_CRITICAL();           //�����ٽ���
//		Env_Data_Struct.Light_Intensity =Get_Adc(5);//�ɼ�����ǿ��
//		Env_Data_Struct.Soil_Moisture = Get_Adc2(4);//�ɼ�����ʪ��
////		xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
//		DHT11_Read_Data(&(Env_Data_Struct.temperature),&(Env_Data_Struct.humidity));//�ɼ��¶ȣ�ʪ��
//		SGP30_Write(0x20,0x08);
//		Env_Data_Struct.sgp30_dat = SGP30_Read();//�ɼ�������̼Ũ��
//		Env_Data_Struct.CO2Data = (Env_Data_Struct.sgp30_dat & 0xffff0000) >> 16;						
//		printf("temperature %f\n",Env_Data_Struct.temperature);
//		printf("humidity %f\n",Env_Data_Struct.humidity);
//		printf("Light_Intensity %d\n",Env_Data_Struct.Light_Intensity);
//		printf("Soil_Moisture %d\n",Env_Data_Struct.Soil_Moisture);
//		printf("co2 %d\n",Env_Data_Struct.CO2Data);
////		xSemaphoreGive(UsartMuxSem_Handle);	
//	 taskEXIT_CRITICAL();  
//	
//}

//int main()
//{	
////	Env_Data_t Environmental_Data_Struct;
//	delay_init(168);
//	TIM2_PWM_Init(19999,83);
////	SGP30_Init();
////	uart_init(115200);
////	DHT11_Init();
////	Adc_Init();//�������� ͨ��5 PA5
////	Adc2_Init();//����ʪ�� ͨ��4 PA4

//	while(1)
//	{	
//		ServoControl_Open();
////		delay_ms(2000);
////		ServoControl_Close();
//		delay_ms(2000);
////		Environmental_Data_Struct.Light_Intensity =Get_Adc(5);
////		Environmental_Data_Struct.Soil_Moisture = Get_Adc2(4);
////		DHT11_Read_Data(&(Environmental_Data_Struct.temperature),&(Environmental_Data_Struct.humidity));
////		SGP30_Write(0x20,0x08);
////		Environmental_Data_Struct.sgp30_dat = SGP30_Read();
////		Environmental_Data_Struct.CO2Data = (Environmental_Data_Struct.sgp30_dat & 0xffff0000) >> 16;
////		printf("temperature %f\n",Environmental_Data_Struct.temperature);
////		printf("humidity %f\n",Environmental_Data_Struct.humidity);
////		printf("Light_Intensity %d\n",Environmental_Data_Struct.Light_Intensity);
////		printf("Soil_Moisture %d\n",Environmental_Data_Struct.Soil_Moisture);
////		printf("co2 %d\n",Environmental_Data_Struct.CO2Data);
////		OLED_P8x16Str(1,1,"SDAWD");
////		delay_ms(2000);
//	}
   
//}

