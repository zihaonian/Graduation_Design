#ifndef __SENSOR_H
#define __SENSOR_H	 
#include "sys.h"  
//�� ��1 �������� 
/*����ķ�ʽ��ͨ��ֱ�Ӳ����⺯����ʽ��ȡIO*/
#define Sensor1() 		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1) //PC1
#define sensor2() 		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)	//PC2


void Sensor_Init(void);	//IO��ʼ��

#endif
