#ifndef _OLED_H
#define _OLED_H
#include "sys.h"
 
//#define SCL PBout(6)
//#define SDA PBout(7) 
 
 
#define X_WIDTH 	128
#define Y_WIDTH 	64
//void OLED_GPIO_Init(void);
//void IIC_Start(void);
//void IIC_Stop(void);
void Write_IIC_Byte(unsigned char Byte);
void OLED_WriteData(unsigned char Data);
void OLED_WriteCommand(unsigned char Command);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char bmp_dat);
void OLED_CLS(void);
void OLED_Init(void);
void OLED_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[]);
void OLED_P8x16Str(unsigned char x,unsigned char y,unsigned char ch[]);
void OLED_P16x16Ch(unsigned char x,unsigned char y,unsigned int N);
void Draw_BMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
void OLED_P8x16char(unsigned char x,unsigned  char y,unsigned char ch);


void OLED_SetCursor(uint8_t Y, uint8_t X);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
uint32_t OLED_Pow(uint32_t X, uint32_t Y);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
 
#endif


//	printf("temperature %f\n",Env_Data_Struct.temperature);
//	printf("humidity %f\n",Env_Data_Struct.humidity);
//	printf("Light_Intensity %d\n",Env_Data_Struct.Light_Intensity);
//	printf("Soil_Moisture %d\n",Env_Data_Struct.Soil_Moisture);
//	printf("co2 %d\n",Env_Data_Struct.CO2Data);
 