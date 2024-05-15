
#include "stm32f4xx.h"                 
#include "MyI2C.h"
#include "delay.h"
#include "MyI2C.h"
//��ʼ��IIC
void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);  //��ʼ��GPIOBʱ��
	

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	IIC_SCL=1;
	IIC_SDA=1;
}
//������ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();  //SDA�����
	IIC_SDA=1;
	IIC_SCL=1;
	DWT_DelayUS(4);
	IIC_SDA=0;//ʱ����Ϊ�ߵ�ƽʱ�����������ɸߵ�ƽ��Ϊ�͵�ƽ��ʾ��ʼ�ź�
	DWT_DelayUS(4);
	IIC_SCL=0;  //׼�����ͺͽ������ݣ���Ϊ������������SCL��ƽ�ڼ�ִ�е�
}
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT(); //SDA�����
	IIC_SCL=0;
	IIC_SDA=0; //SCLΪ�͵�ƽʱ��SDA�ӵ͵�ƽת��Ϊ�ߵ�ƽ��ʾֹͣ�ź�
	DWT_DelayUS(4);
	IIC_SCL=1;
	IIC_SDA=1;
	DWT_DelayUS(4);
}
//�ȴ�Ӧ���źŵĵ���
//����ֵ��1������Ӧ��ʧ��
//		  0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();  //SDA����Ϊ����
	IIC_SDA=1;
	DWT_DelayUS(1);
	IIC_SCL=1;
	DWT_DelayUS(1);
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)//��ֹӦ��ʱ��������������
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;
	return 0;
}
//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	DWT_DelayUS(2);
	IIC_SCL=1;
	DWT_DelayUS(2);
	IIC_SCL=0;  //SCL�ɸߵ�ƽת���͵�ƽ��ʾһ��Ӧ��
}
//������ACKӦ��
void IIC_NACK(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;  //SDA�ߵ�ƽ�ڼ��ǲ�����Ӧ���
	DWT_DelayUS(2);
	IIC_SCL=1;
	DWT_DelayUS(2);
	IIC_SCL=0; 
}
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void IIC_Send_Byte(u8 Byte)
{
	u8 i;
	SDA_OUT();
	IIC_SCL=0;  //����ʱ�ӿ�ʼ���ݴ���
	for(i=0;i<8;i++)
	{
		IIC_SDA=(Byte&0x80)>>7;
		Byte<<=1;
		DWT_DelayUS(2);
		IIC_SCL=1;
		DWT_DelayUS(2);
		IIC_SCL=0;
		DWT_DelayUS(2);  //ʱ���ɸߵ�ƽ���͵�ƽ��ʾ����һ���ֽ�
	}
}
//���ֽ�
//Ack=1������ACK
//Ack=0������nACK
u8 IIC_Read_Byte(u8 Ack)
{
	u8 i,Data;
	SDA_IN();
	for(i=0;i<8;i++)
	{
		IIC_SCL=0;
		DWT_DelayUS(2);
		IIC_SCL=1;  //��һ���ֽ�֮ǰ�����ͷ�����	
		Data<<=1;
		if(READ_SDA)
			Data++;
		DWT_DelayUS(1);
	}
	if(!Ack)
		IIC_NACK();
	else
		IIC_Ack();
	return Data;
}
