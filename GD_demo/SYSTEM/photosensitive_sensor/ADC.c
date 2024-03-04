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

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |  RCC_AHB1Periph_DMA2, ENABLE);//使能引脚及DMA时钟

    //引脚初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;  //PA1 2 初始化
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不上下拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化

    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两采样间延迟五个时钟
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1; //使能
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频为4分频;ADCCLK=PCLK2/4=84/4=21Mhz,最好不要超过36Mhz 
    ADC_CommonInit(&ADC_CommonInitStructure);//初始化


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
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                    //存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);

    DMA_Cmd(DMA2_Stream0, ENABLE); 


}


void Adc_Init(void)
{	
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);// 使能ADC1时钟
	
	//初始化ADC1通道5 IO口

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AN;  //模式为模拟输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;  //PA5 通道5
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL; //不带上下拉
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);   //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);    //复位结束
	
	//初始化CCR通用控制寄存器配置

	ADC_CommonInitStructure.ADC_DMAAccessMode=ADC_DMAAccessMode_Disabled; //DMA失能
	ADC_CommonInitStructure.ADC_Mode=ADC_Mode_Independent;  //独立模式
	ADC_CommonInitStructure.ADC_Prescaler=ADC_Prescaler_Div4;  //预分频值设置
	ADC_CommonInitStructure.ADC_TwoSamplingDelay=ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间延迟5个时钟
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	//初始化ADC1相关参数
	
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;  //关闭连续转换
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;  //右对齐
	ADC_InitStructure.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None; //禁止触发检测，使用软件触发
	ADC_InitStructure.ADC_NbrOfConversion=1;  //1个转换在规则序列中
	ADC_InitStructure.ADC_Resolution=ADC_Resolution_8b ;  //8位模式
	ADC_InitStructure.ADC_ScanConvMode=DISABLE; //非扫描模式
	ADC_Init(ADC1,&ADC_InitStructure); //ADC初始化
	
	ADC_Cmd(ADC1,ENABLE);   //开启AD转换器
}
void Adc2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2,ENABLE);// 使能ADC1时钟
	
	//初始化ADC1通道5 IO口

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AN;  //模式为模拟输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;  //PA4 通道4
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL; //不带上下拉
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,ENABLE);   //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,DISABLE);    //复位结束
	
	//初始化CCR通用控制寄存器配置

	ADC_CommonInitStructure.ADC_DMAAccessMode=ADC_DMAAccessMode_Disabled; //DMA失能
	ADC_CommonInitStructure.ADC_Mode=ADC_Mode_Independent;  //独立模式
	ADC_CommonInitStructure.ADC_Prescaler=ADC_Prescaler_Div4;  //预分频值设置
	ADC_CommonInitStructure.ADC_TwoSamplingDelay=ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间延迟5个时钟
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	//初始化ADC1相关参数

	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;  //关闭连续转换
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;  //右对齐
	ADC_InitStructure.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None; //禁止触发检测，使用软件触发
	ADC_InitStructure.ADC_NbrOfConversion=1;  //1个转换在规则序列中
	ADC_InitStructure.ADC_Resolution=ADC_Resolution_8b ;  //8位模式
	ADC_InitStructure.ADC_ScanConvMode=DISABLE; //非扫描模式
	ADC_Init(ADC2,&ADC_InitStructure); //ADC初始化
	
	ADC_Cmd(ADC2,ENABLE);   //开启AD转换器
}

u16 Get_Adc2(u8 ch)
{
	ADC_RegularChannelConfig(ADC2,ch,1,ADC_SampleTime_480Cycles);//设置ADC规则组通道，一个序列，采样时间480
	ADC_SoftwareStartConv(ADC2);//使能指定的ADC1的软件转换启动功能
	while(!ADC_GetFlagStatus(ADC2,ADC_FLAG_EOC));//读取状态寄存器的状态位EOC，等待转换结束
	return (256-ADC_GetConversionValue(ADC2))/2.55;//返回最近一次的ADC1规则组的转换结果
}
//获取ADC值
//ch：通道值 0~16
//返回值：转换结果
u16 Get_Adc(u8 ch)
{
	ADC_RegularChannelConfig(ADC1,ch,1,ADC_SampleTime_480Cycles);//设置ADC规则组通道，一个序列，采样时间480
	ADC_SoftwareStartConv(ADC1);//使能指定的ADC1的软件转换启动功能
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));//读取状态寄存器的状态位EOC，等待转换结束
	return (256-ADC_GetConversionValue(ADC1))/2.55;//返回最近一次的ADC1规则组的转换结果
}
//获取通道ch的转换值，取times次，然后平均
//ch：通道编号  times：获取次数
//返回值：通道ch的times次转换结果平均值
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
		temp_val=temp_val+value; //取times次通道值进行求和
		delay_ms(5);
	}
	
	return (temp_val-min-max)/(times-2); //返回平均值
}  