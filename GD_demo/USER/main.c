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
#include "oled.h"//PB12��SCL��  PB13��SDA�� 
#include "ADC.h"//PA5(sun_sonsor)  PA4(soil_moisture)
#include "DHT11.h"//PB9
#include "pwm.h"
#include "sgp30.h"
//#include "myiic.h"
#include "usart.h"//PA9������RXD  PA10������TXD
#include "pwm.h" 
#include "timer.h"
#include "led.h"
#include "key.h"//A1 A2 A3 A4
#include "FreeRTOS.h"
#include "task.h" 
#include "semphr.h"

SemaphoreHandle_t UsartMuxSem_Handle;						//���崮�ڻ���������

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
	Adc_Init();//�������� ͨ��5
	Adc2_Init();//����ʪ�� ͨ��4
//	adc_DMA_init();
	TIM2_PWM_Init(19999,83);//��� PA0 500-2000
	SGP30_Init();
	LED_Open();
	
	//������ʼ����
	xTaskCreate((TaskFunction_t )startTask,										//������ڵ�ַ           
            (const char*    )"startTask",										//��������         
            (uint16_t       )128,  												//�����ջ��С      
            (void*          )NULL,  											//�����������               
            (UBaseType_t    )5,													//�������ȼ�       
            (TaskHandle_t*  )&StartTask_Handler); 								//������    
    vTaskStartScheduler();          
}


void startTask(void *pvParameters) 
{
    taskENTER_CRITICAL();           //�����ٽ���
	UsartMuxSem_Handle = xSemaphoreCreateMutex(); //����������
	//����������������
	xTaskCreate((TaskFunction_t )UsartCommandTask,								//������ڵ�ַ           
				(const char*    )"UsartCommandTask",							//��������         
				(uint16_t       )128,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )8,												//�������ȼ�       
				(TaskHandle_t*  )&UsartCommandTask_Handler); 					//������ 
    //�������ݲɼ�����
	xTaskCreate((TaskFunction_t )DataAcquisitionTask,								//������ڵ�ַ           
				(const char*    )"DataAcquisitionTask",								//��������         
				(uint16_t       )128,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )7,												//�������ȼ�       
				(TaskHandle_t*  )&DataAcquisitionTask_Handler); 					//������ 
	//�����Զ�ģʽ����			
	xTaskCreate((TaskFunction_t )AutoModeTask,									//������ڵ�ַ           
				(const char*    )"AutoModeTask",								//��������         
				(uint16_t       )128,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )4,												//�������ȼ�       
				(TaskHandle_t*  )&AutoModeTask_Handler); 						//������ 	
	//�����ֶ�ģʽ����
	xTaskCreate((TaskFunction_t )ManualModTask,									//������ڵ�ַ           
				(const char*    )"ManualModTask",								//��������         
				(uint16_t       )128,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )4,												//�������ȼ�       
				(TaskHandle_t*  )&ManualModTask_Handler);		 				//������ 
	//����ͨ���������				
	xTaskCreate((TaskFunction_t )VentilationControlTask,						//������ڵ�ַ           
				(const char*    )"VentilationControlTask",						//��������         
				(uint16_t       )128,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )6,												//�������ȼ�       
				(TaskHandle_t*  )&VentilationControlTask_Handler); 				//������ 
	//���������������				
	xTaskCreate((TaskFunction_t )ServoControlTask,								//������ڵ�ַ           
				(const char*    )"ServoControlTask",							//��������         
				(uint16_t       )128,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )6,												//�������ȼ�       
				(TaskHandle_t*  )&ServoControlTask_Handler); 					//������ 	
	//������ʪ����������				
	xTaskCreate((TaskFunction_t )HumidifierControlTask,							//������ڵ�ַ           
				(const char*    )"HumidifierControlTask",						//��������         
				(uint16_t       )128,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )6,												//�������ȼ�       
				(TaskHandle_t*  )&HumidifierControlTask_Handler); 				//������ 
	//�������ȿ�������				
	xTaskCreate((TaskFunction_t )HeatingControlTask,							//������ڵ�ַ           
				(const char*    )"HeatingControlTask",							//��������         
				(uint16_t       )128,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )6,												//�������ȼ�       
				(TaskHandle_t*  )&HeatingControlTask_Handler); 					//������ 	
	//������ˮ��������				
	xTaskCreate((TaskFunction_t )WateringControlTask,							//������ڵ�ַ           
				(const char*    )"WateringControlTask",							//��������         
				(uint16_t       )128,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )6,												//�������ȼ�       
				(TaskHandle_t*  )&WateringControlTask_Handler); 				//������ 
	//����oled��ʾ����				
	xTaskCreate((TaskFunction_t )OledDisplayTask,								//������ڵ�ַ           
				(const char*    )"OledDisplayTask",								//��������         
				(uint16_t       )128,  											//�����ջ��С      
				(void*          )NULL,  										//�����������               
				(UBaseType_t    )3,												//�������ȼ�       
				(TaskHandle_t*  )&OledDisplayTask_Handler); 					//������ 

	vTaskSuspend(ManualModTask_Handler);
	vTaskResume(AutoModeTask_Handler);   			
    vTaskDelete(StartTask_Handler); 											//ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}
/*****************������������******************/
void UsartCommandTask(void *pvParameters)
{
	
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
				xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 				
				printf("�Զ�ģʽ\n");
				xSemaphoreGive(UsartMuxSem_Handle);
				rx_cnt = 0; 				
			} 
			else if(strcmp((const char*)USART_RX_BUF, "switch manual") == 0)
			{   
				vTaskSuspend(AutoModeTask_Handler);
				vTaskResume(ManualModTask_Handler);
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); 
				rx_cnt = 0; 
				xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
				printf("�ֶ�ģʽ\n");
				xSemaphoreGive(UsartMuxSem_Handle);
			} 
			else
			{
				memset(USART_RX_BUF,0,sizeof(USART_RX_BUF));
				xSemaphoreTake(UsartMuxSem_Handle,portMAX_DELAY); 
				printf("��������");
				xSemaphoreGive(UsartMuxSem_Handle);
				rx_cnt = 0; 
			}		
		}
		vTaskDelay(100);	
	}
}	
/*****************���ݲɼ�����******************/
void DataAcquisitionTask(void *pvParameters)
{
	uint32_t sgp30_dat = 0;
    while(1)
    {
		SGP30_Write(0x20,0x08);
		sgp30_dat = SGP30_Read();
		CO2Data = (sgp30_dat & 0xffff0000) >> 16;                                                   
		printf("%d\n",CO2Data);
		vTaskDelay(1000);		
    }
}
/*****************�Զ�ģʽ����******************/
void AutoModeTask(void *pvParameters)
{
	LED_Close();
    while(1)
    {	
		
		
		vTaskDelay(200);		
    }
}
/*****************�ֶ�ģʽ����******************/
void ManualModTask(void *pvParameters)
{


	LED_Open(); 
    while(1)
    {	

		
		
    }
}
/*****************ͨ���������******************/
void VentilationControlTask(void *pvParameters)
{

    while(1)
    {	

		LED_Toggle();
		vTaskDelay(1000);
    }
}
/*****************�����������******************/
void ServoControlTask(void *pvParameters)
{

    while(1)
    {	


		vTaskDelay(100);
    }
}
/*****************��ʪ����������****************/
void HumidifierControlTask(void *pvParameters)
{

    while(1)
    {	


		vTaskDelay(100);
    }
}
/*****************���ȿ�������****************/
void HeatingControlTask(void *pvParameters)
{

    while(1)
    {	


		vTaskDelay(100);
    }
}
/*****************��ˮ��������****************/
void WateringControlTask(void *pvParameters)
{

    while(1)
    {	


		vTaskDelay(100);
    }
}
/*****************OLED��ʾ����****************/
void OledDisplayTask(void *pvParameters)
{

    while(1)
    {	


		vTaskDelay(100);
    }
}