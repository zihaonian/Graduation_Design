#ifndef __STE_H
#define __STE_H	 
#include "sys.h"  
#define STE_Median_PWM 1475
void ste_Init(void);
void STE_PWM(int PWM);//PWM [-325,325]
#endif
