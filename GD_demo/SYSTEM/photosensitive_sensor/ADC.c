#include "stm32f4xx.h"                
#include "ADC.h"
#include "delay.h"
 
//��ʼ��ADC
void Adc_Init(void)
{	
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);// ʹ��ADC1ʱ��
	
	//��ʼ��ADC1ͨ��5 IO��

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AN;  //ģʽΪģ������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;  //PA5 ͨ��5
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL; //����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);   //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);    //��λ����
	
	//��ʼ��CCRͨ�ÿ��ƼĴ�������

	ADC_CommonInitStructure.ADC_DMAAccessMode=ADC_DMAAccessMode_Disabled; //DMAʧ��
	ADC_CommonInitStructure.ADC_Mode=ADC_Mode_Independent;  //����ģʽ
	ADC_CommonInitStructure.ADC_Prescaler=ADC_Prescaler_Div4;  //Ԥ��Ƶֵ����
	ADC_CommonInitStructure.ADC_TwoSamplingDelay=ADC_TwoSamplingDelay_5Cycles;//���������׶�֮���ӳ�5��ʱ��
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	//��ʼ��ADC1��ز���
	
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;  //�ر�����ת��
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;  //�Ҷ���
	ADC_InitStructure.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None; //��ֹ������⣬ʹ���������
	ADC_InitStructure.ADC_NbrOfConversion=1;  //1��ת���ڹ���������
	ADC_InitStructure.ADC_Resolution=ADC_Resolution_8b ;  //8λģʽ
	ADC_InitStructure.ADC_ScanConvMode=DISABLE; //��ɨ��ģʽ
	ADC_Init(ADC1,&ADC_InitStructure); //ADC��ʼ��
	
	ADC_Cmd(ADC1,ENABLE);   //����ADת����
}
void Adc2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2,ENABLE);// ʹ��ADC1ʱ��
	
	//��ʼ��ADC1ͨ��5 IO��

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AN;  //ģʽΪģ������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;  //PA4 ͨ��4
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL; //����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,ENABLE);   //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,DISABLE);    //��λ����
	
	//��ʼ��CCRͨ�ÿ��ƼĴ�������

	ADC_CommonInitStructure.ADC_DMAAccessMode=ADC_DMAAccessMode_Disabled; //DMAʧ��
	ADC_CommonInitStructure.ADC_Mode=ADC_Mode_Independent;  //����ģʽ
	ADC_CommonInitStructure.ADC_Prescaler=ADC_Prescaler_Div4;  //Ԥ��Ƶֵ����
	ADC_CommonInitStructure.ADC_TwoSamplingDelay=ADC_TwoSamplingDelay_5Cycles;//���������׶�֮���ӳ�5��ʱ��
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	//��ʼ��ADC1��ز���

	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;  //�ر�����ת��
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;  //�Ҷ���
	ADC_InitStructure.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None; //��ֹ������⣬ʹ���������
	ADC_InitStructure.ADC_NbrOfConversion=1;  //1��ת���ڹ���������
	ADC_InitStructure.ADC_Resolution=ADC_Resolution_8b ;  //8λģʽ
	ADC_InitStructure.ADC_ScanConvMode=DISABLE; //��ɨ��ģʽ
	ADC_Init(ADC2,&ADC_InitStructure); //ADC��ʼ��
	
	ADC_Cmd(ADC2,ENABLE);   //����ADת����
}

u16 Get_Adc2(u8 ch)
{
	ADC_RegularChannelConfig(ADC2,ch,1,ADC_SampleTime_480Cycles);//����ADC������ͨ����һ�����У�����ʱ��480
	ADC_SoftwareStartConv(ADC2);//ʹ��ָ����ADC1�����ת����������
	while(!ADC_GetFlagStatus(ADC2,ADC_FLAG_EOC));//��ȡ״̬�Ĵ�����״̬λEOC���ȴ�ת������
	return (256-ADC_GetConversionValue(ADC2))/2.55;//�������һ�ε�ADC1�������ת�����
}
//��ȡADCֵ
//ch��ͨ��ֵ 0~16
//����ֵ��ת�����
u16 Get_Adc(u8 ch)
{
	ADC_RegularChannelConfig(ADC1,ch,1,ADC_SampleTime_480Cycles);//����ADC������ͨ����һ�����У�����ʱ��480
	ADC_SoftwareStartConv(ADC1);//ʹ��ָ����ADC1�����ת����������
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));//��ȡ״̬�Ĵ�����״̬λEOC���ȴ�ת������
	return (256-ADC_GetConversionValue(ADC1))/2.55;//�������һ�ε�ADC1�������ת�����
}
//��ȡͨ��ch��ת��ֵ��ȡtimes�Σ�Ȼ��ƽ��
//ch��ͨ�����  times����ȡ����
//����ֵ��ͨ��ch��times��ת�����ƽ��ֵ
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t,max=0,min=255,value;
	for(t=0;t<times;t++)
	{ value = Get_Adc(ch);
		if(value>max) 
		{
			max = value;
		}
		if(value<min) 
		{
			min = value;
		}
		temp_val=temp_val+value; //ȡtimes��ͨ��ֵ�������
		delay_ms(5);
	}
	
	return (temp_val-min-max)/(times-2); //����ƽ��ֵ
}
  