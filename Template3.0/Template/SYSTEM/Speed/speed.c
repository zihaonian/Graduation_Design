#include "speed.h"
#include "usart.h" 
 #include "pwm.h"

//按键初始化函数
void speed_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOC时钟
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_NOPULL;// 
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	TIM3_PWM_Init(9999,83);
	Forward_PWM(0);
}
void Forward_PWM(u16 PWM)
{

	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	TIM_SetCompare1(TIM3,PWM);
	
}


void Backward_PWM(u16 PWM)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_7);
	TIM_SetCompare1(TIM3,PWM);
	
}




















