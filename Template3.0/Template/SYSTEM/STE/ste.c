#include "ste.h"
#include "usart.h" 
 #include "pwm.h"

//按键初始化函数
void ste_Init(void)
{
	TIM2_PWM_Init(19999,83);
	TIM_SetCompare1(TIM2,STE_Median_PWM);
}
void STE_PWM(int PWM)
{
	if(PWM<0)
	{
		PWM=PWM*1.62;
	}
	if(PWM>400)
	{
		PWM=400;
	}
	else if(PWM<-648)
	{
		PWM=-648;
	}
	TIM_SetCompare1(TIM2,PWM+STE_Median_PWM);
}




















