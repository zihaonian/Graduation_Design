#include "usart.h"
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
}			
void LED_Open(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
}

void LED_Close(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
}

void LED_Toggle(void)
{
	GPIO_ToggleBits(GPIOB,GPIO_Pin_0);
}





























