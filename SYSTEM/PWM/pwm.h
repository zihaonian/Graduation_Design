

#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
void TIM3_PWM_Init(u32 arr,u32 psc);//初始化PA6
void TIM2_PWM_Init(u32 arr,u32 psc);
#endif

//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
//	delay_init(168);  //初始化延时函数

//  if(CAMERA_TRIGGER_25HZ)
//	{
//		PWM_PSC = 84;//分频系数.----计数频率为84M/PWM_PSC = 1M hz
//		PWM_ARR = 40000;//重装载值----PWM频率  (84M/PWM_PSC)/PWM_ARR = 25 hz
//		PWM_a = 0.1;//占空比	
//	}else if(CAMERA_TRIGGER_20HZ)
//	{
//		PWM_PSC = 84;//分频系数.----计数频率为84M/PWM_PSC = 1M hz
//		PWM_ARR = 50000;//重装载值----PWM频率  (84M/PWM_PSC)/PWM_ARR = 25 hz
//		PWM_a = 0.1;//占空比	
//	}
//	
//	PWM_CCR = (uint32_t)PWM_ARR*PWM_a;	     
// 	TIM14_PWM_Init(PWM_ARR,PWM_PSC);	//设置PWM周期
//	TIM_SetCompare1(TIM14,PWM_CCR);	//修改比较值，修改占空比
