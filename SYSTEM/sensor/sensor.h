#ifndef __SENSOR_H
#define __SENSOR_H	 
#include "sys.h"  
//黑 置1 下拉输入 
/*下面的方式是通过直接操作库函数方式读取IO*/
#define Sensor1() 		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1) //PC1
#define sensor2() 		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)	//PC2


void Sensor_Init(void);	//IO初始化

#endif
