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
#include "usart.h"
#include "pwm.h" 
#include "timer.h"//OPENMV_OUT() C4
#include "led.h"
#include "key.h"//A1 A2 A3 A4
#include "sensor.h"////PC1  Sensor1()  PC2   Sensor2()  
#include "beer.h"//C3
#include "speed.h"//DIR A7  PWM A6
#include "ste.h"//A0       
#include "Encoder.h" //E0
int Flag_finish=0;
int count_j=0;
int main(void)
{ 
	LED_Init();
	LED_Close();
  delay_init(168);
	delay_ms(7000);//等待openmv完成初始化
	TIM5_Int_Init(2000,8399);
	uart_init(115200);
	Beer_Init();
	speed_Init();
	Sensor_Init();
	ste_Init();
	KEY_Init();
	TIM_ETR_Init();
	LED_Open();//各初始化完成标志	
	while(1)
	{		
		if(KEY_Num==0)
		{
			KEY_Num=KEY_Scan(0);
			count_j=0;
		}
		if(KEY_Num!=0&&count_j==0)//有输入时延时2s
		{
			if(KEY_Num==1)
			{
				LED_Open();
				Beer_Open();
				delay_ms(500);
				Beer_Close();
				delay_ms(1500);
				count_j++;
			}
			else if(KEY_Num==2)
			{
				Beer_Close();
				Beer_Open();
				delay_ms(500);
				Beer_Close();
				delay_ms(1500);
				count_j++;
			}
			
		}
		//前进识别
		if((KEY_Num==1||KEY_Num==2)&&Flag_back==0)
		{
			Forward_PWM(3000);
		}
		

		if(KEY_Num==1&&Flag_back==1)//倒车入库
		{
			
			Backward_PWM(0);
			delay_ms(100);
			STE_PWM(0);
			TIM_SetCounter(TIM4,0);
			Backward_PWM(3000);
			while(TIM_GetCounter(TIM4)<150);
			TIM_SetCounter(TIM4,0);//向后
			
			STE_PWM(-300);
			Backward_PWM(0);
			delay_ms(100);
      Forward_PWM(4000);
			
			
			while(TIM_GetCounter(TIM4)<400);
			Forward_PWM(0);
			STE_PWM(400);
			delay_ms(100);
			Backward_PWM(4000);
			Beer_Open();
			delay_ms(500);
			Beer_Close();
			while(Sensor1()==0);
			while(sensor2()==0);
			delay_ms(200);
			STE_PWM(0);
			TIM_SetCounter(TIM4,0);
			while(TIM_GetCounter(TIM4)<280);
			Forward_PWM(0);
			TIM_SetCounter(TIM4,0);//已入库
			
			Beer_Open();
			delay_ms(500);
			Beer_Close();
			delay_ms(4500);
			
			Forward_PWM(4000);//出库
//			while(Sensor1()==0&&sensor2()==0);
			TIM_SetCounter(TIM4,0);
			while(TIM_GetCounter(TIM4)<250);
			STE_PWM(400);
			TIM_SetCounter(TIM4,0);
			while(TIM_GetCounter(TIM4)<1050);
			//进入完成阶段标志
			Flag_finish=1;
		}	
		
		else if(KEY_Num==2&&Flag_back==1)//侧方位
		{
			TIM_SetCounter(TIM4,0);
			Forward_PWM(3000);
			while(TIM_GetCounter(TIM4)<400);
			TIM_SetCounter(TIM4,0);
			Forward_PWM(0);
			Beer_Open();
			STE_PWM(200);
			Backward_PWM(4000);
			delay_ms(500);
			Beer_Close();
			while(Sensor1()==0);
			STE_PWM(-325);
			TIM_SetCounter(TIM4,0);
			while(TIM_GetCounter(TIM4)<600);//已入库
			Backward_PWM(0);
			Beer_Open();
			delay_ms(500);
			Beer_Close();
			delay_ms(4500);
			
			Forward_PWM(4000);//出库
			while(sensor2()==0);
			delay_ms(500);
			STE_PWM(325);
			TIM_SetCounter(TIM4,0);
			while(TIM_GetCounter(TIM4)<770);

			//进入完成阶段标志
			Flag_finish=1;
		}
		
		if(Flag_finish==1&&Flag_back==1)//完成阶段标志
		{
			STE_PWM(0);
			TIM5_Int_Init(2000,8399);
			TIM_SetCounter(TIM4,0);
			Forward_PWM(3000);
			while(TIM_GetCounter(TIM4)<500);
			Forward_PWM(0);
			TIM_SetCounter(TIM4,0);
			Flag_finish=0;
			Flag_back=0;
			KEY_Num=0;
			
		}
	}
}



//			printf("%d",TIM_GetCounter(TIM4));
//		else if(KEY_Num==KEY3_PRES)
//		{
//			LED_Open();
//		}
//		else if(KEY_Num==KEY4_PRES)
//		{
//			LED_Close();
//		}	
//		if(Sensor1()==1)
//			LED_Open();
//		else 
//			LED_Close();
//		Forward_PWM(2000);
//		delay_ms(2000);
//		Forward_PWM(0);
//		delay_ms(2000);
//		Backward_PWM(2000);
//		delay_ms(2000);
//		Forward_PWM(0);
//		delay_ms(2000);
//		STE_PWM(200);
//		delay_ms(2000);
//		STE_PWM(-200);
//		delay_ms(2000);
//		printf("%d",TIM_GetCounter(TIM4));




