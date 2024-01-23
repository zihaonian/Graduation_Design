#ifndef _DHT11__H_
#define _DHT11__H_
#include "sys.h"
 
 
//IO��������
//IO������ͨ��λ�����õģ�����GPIOģʽ�Ĵ��������´����Ѿ����ʹ�ã����в���⣬�ɻ�ͷ��ϰDS18B20��ʵ�����
#define DHT11_IO_IN() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	//PB9����ģʽ
#define DHT11_IO_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} 	//PB9���ģʽ 
 
//IO��������
#define DHT11_DQ_OUT PBout(9)  //PB9���
#define DHT11_DQ_IN PBin(9)    //PB9����
 
 
void DHT11_Reset(void);
u8 DHT11_CheckExist(void);
u8 DHT11_Read_Bit(void);
u8 DHT11_Read_Byte(void);
u8 DHT11_Read_Data(u8 *temperature,u8 *humidity);
u8 DHT11_Init(void);
 
#endif

