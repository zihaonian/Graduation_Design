#include "stm32f4xx.h"                
#include "ADC.h"
#include "delay.h"
 
//初始化ADC
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
  