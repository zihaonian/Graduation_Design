#ifndef _TMER_H
#define _TMER_H
#include "sys.h"	
extern u8 Flag_back;
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM5_Int_Init(u16 arr,u16 psc);
#endif
