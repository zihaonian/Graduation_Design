#include "usart.h"
#include "beer.h"
void Beer_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOC时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_DOWN;//下拉 
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
}			
void Beer_Open(void)
{
	GPIO_SetBits(GPIOC,GPIO_Pin_3);
}

void Beer_Close(void)
{
	GPIO_ResetBits(GPIOC,GPIO_Pin_3);
}































