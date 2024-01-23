
#ifndef _OLED_H
#define _OLED_H
#include "sys.h"
 
//#define SCL PBout(6)
//#define SDA PBout(7) 
 
#define high 1
#define low 0
 
#define	Brightness	0xCF 
#define X_WIDTH 	128
#define Y_WIDTH 	64
void OLED_GPIO_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void Write_IIC_Byte(unsigned char IIC_Byte);
void OLED_WrDat(unsigned char dat);
void OLED_WrCmd(unsigned char cmd);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char bmp_dat);
void OLED_CLS(void);
void OLED_Init(void);
void OLED_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[]);
void OLED_P8x16Str(unsigned char x,unsigned char y,unsigned char ch[]);
void OLED_P16x16Ch(unsigned char x,unsigned char y,unsigned int N);
void Draw_BMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
 
 
 
 
 
 
 
 
#endif
