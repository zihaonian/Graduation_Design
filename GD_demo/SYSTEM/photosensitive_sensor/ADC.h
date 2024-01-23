#ifndef _ADC__H_
#define _ADC__H_
#include "stm32f4xx.h"  
void Adc_Init(void);
u16 Get_Adc(u8 ch);
u16 Get_Adc_Average(u8 ch,u8 times);
void Adc2_Init(void);
u16 Get_Adc2(u8 ch);
#endif

