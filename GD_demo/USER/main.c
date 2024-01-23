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
#include "usart.h"
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
	Adc2_Init();//土壤湿度 通道4
	TIM2_PWM_Init(19999,83);//舵机 PA0 500-2000
	
	while(1)
	{
			printf("sdwasd");
			delay_ms(1500);
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

