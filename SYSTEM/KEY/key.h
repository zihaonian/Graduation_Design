#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//STM32F4工程模板-库函数版本
//淘宝店铺：http://mcudev.taobao.com									  
////////////////////////////////////////////////////////////////////////////////// 	 

/*下面的方式是通过直接操作库函数方式读取IO*/
#define KEY1 		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) //PA1
#define KEY2 		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)	//PA2
#define KEY3 		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)	//PA3 
#define KEY4 		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)	//PA4 


/*下面方式是通过位带操作方式读取IO*/
/*
#define KEY1 		PAin(1)   //PA1
#define KEY2 		PAin(2)		//PA2
#define KEY3 		PAin(3)		//PA3
#define KEY4  	PAin(4)		//PA4
*/


#define KEY1_PRES 1
#define KEY2_PRES	2
#define KEY3_PRES	3
#define KEY4_PRES	4

void KEY_Init(void);	//IO初始化
u8 KEY_Scan(u8);  		//按键扫描函数	 1:连续扫描 0：不连续扫描//  返回值1 2 3 4分别表示对应数字按键

#endif
