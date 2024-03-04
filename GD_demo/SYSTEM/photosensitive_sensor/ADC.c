#include "stm32f4xx.h"                
#include "ADC.h"
#include "delay.h"
uint16_t Adc_Value[2] = {0};
void adc_DMA_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //ʹ��ADC1ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |  RCC_AHB1Periph_DMA2, ENABLE);//ʹ�����ż�DMAʱ��

    //���ų�ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;  //PA1 2 ��ʼ��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��

    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������ӳ����ʱ��
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1; //ʹ��
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//Ԥ��ƵΪ4��Ƶ;ADCCLK=PCLK2/4=84/4=21Mhz,��ò�Ҫ����36Mhz 
    ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��


    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 2;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_480Cycles );    
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SampleTime_480Cycles );
	
    ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE); 
    ADC_DMACmd(ADC1, ENABLE); 

	
    DMA_DeInit(DMA2_Stream0);

    DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(ADC1->DR);
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)Adc_Value;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;

    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = 2;


    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;    
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                    //�洢��ͻ�����δ���
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);

    DMA_Cmd(DMA2_Stream0, ENABLE); 


}


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