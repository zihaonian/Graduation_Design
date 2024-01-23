#ifndef __SENSOR_H
#define __SENSOR_H	 
#include "sys.h"  
//黑 置1 下拉输入 
/*下面的方式是通过直接操作库函数方式读取IO*/
	 //PC1  Sensor1() 	
	//PC2   Sensor2() 


void Sensor_Init(void);	//IO初始化
u8 Sensor1(void);
u8 sensor2(void);
u8 OPENMV_OUT(void);
#endif
