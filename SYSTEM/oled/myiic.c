
#include "myiic.h"
#include "delay.h"
	
 
//IIC的初始化
//SDA PB9
//SCL PB8
void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
  //GPIOB10,B11³õÊ¼»¯ÉèÖÃ
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 |GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  
  
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	IIC_SCL=1;
	IIC_SDA=1;
}
 
 
void IIC_Start(void)
{
	SDA_OUT();     
	IIC_SDA=1;	  	  
	IIC_SCL=1;  
	delay_us(2);
 	IIC_SDA=0;     
	delay_us(2);
	IIC_SCL=0;     //钳住I2C，准备接受或者发送数据
}	  
 
 
 
void IIC_Stop(void)
{
	SDA_OUT();//sdaÏßÊä³ö
	IIC_SCL=0;
	IIC_SDA=0;
 	delay_us(2);
	IIC_SCL=1; 
	IIC_SDA=1;
	delay_us(2);							   					   	
}
 
 
//返回值：1：成功 0：失败
 
 
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      
	IIC_SDA=1;delay_us(1);   
	IIC_SCL=1;delay_us(1);	
  
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;  
	return 0;  
} 
 
 
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(4);
	IIC_SCL=1;
	delay_us(4);
	IIC_SCL=0;
}
 
	    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(4);
	IIC_SCL=1;
	delay_us(4);
	IIC_SCL=0;
}					 				     
		
u8 test=0;
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	  SDA_OUT(); 	    
    IIC_SCL=0;
	 
    for(t=0;t<8;t++)
    {             
			if((txd&0x80)>>7)
        IIC_SDA=1;
			else 
				IIC_SDA=0;
			
        txd<<=1; 	 
			  delay_us(5);
				IIC_SCL=1;
				delay_us(5); 
				IIC_SCL=0;	
				delay_us(5);
    }	 
} 	    
 
 
 
   
 
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	 SDA_IN();
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(5);
		    IIC_SCL=1;
        receive<<=1;
		     
        if(READ_SDA)receive++;   
		   delay_us(5); 
    }					 
    if (!ack)
        IIC_NAck();
    else
        IIC_Ack();   
    return receive;
}
