#include "Encoder.h" 
#include "sys.h"

void TIM_ETR_Init(void)
{
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
GPIO_InitTypeDef GPIO_InitStructure;

RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
GPIO_Init(GPIOE,&GPIO_InitStructure);

GPIO_PinAFConfig(GPIOE,GPIO_PinSource0,GPIO_AF_TIM4);   //开启第二功能

TIM_TimeBaseStructure.TIM_Period = 0xFFFF; 
TIM_TimeBaseStructure.TIM_Prescaler = 0; 
TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

TIM_ETRClockMode2Config(TIM4,TIM_ExtTRGPSC_OFF,TIM_ExtTRGPolarity_NonInverted,0);  //外部时钟源模式2
TIM_Cmd(TIM4, ENABLE);
TIM_GetCounter(TIM4);
TIM_SetCounter(TIM4,0);
}

      //    TIM_GetCounter(TIM4)    //调用直接获取计数值
      //    TIM_SetCounter(TIM4,0)  //调用清零计数值
