#include "sensor.h"
#include "usart.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//STM32F4工程模板-库函数版本
//淘宝店铺：http://mcudev.taobao.com							  
////////////////////////////////////////////////////////////////////////////////// 	 

//按键初始化函数
void Sensor_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOC时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_4; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_DOWN;//下拉 
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
} 


u8 Sensor1(void)
{
	return GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1);
	
}
u8 sensor2(void)
{
	return GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2);
}


u8 OPENMV_OUT(void)
{
	return GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4);
}
	


















