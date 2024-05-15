/***************************飞音云电子****************************
**  工程名称：YS-V0.7语音识别模块驱动程序
**	CPU: STC11L08XE
**	晶振：22.1184MHZ
**	波特率：9600 bit/S
**	配套产品信息：YS-V0.7语音识别开发板
**                http://yuesheng001.taobao.com
**  作者：zdings
**  联系：751956552@qq.com
**  修改日期：2013.9.13
**  说明：普通模式：直接发语音识别
/***************************飞音云电子******************************/
#include "config.h"
/************************************************************************************/
//	nAsrStatus 用来在main主程序中表示程序运行的状态，不是LD3320芯片内部的状态寄存器
//	LD_ASR_NONE:		表示没有在作ASR识别
//	LD_ASR_RUNING：		表示LD3320正在作ASR识别中
//	LD_ASR_FOUNDOK:		表示一次识别流程结束后，有一个识别结果
//	LD_ASR_FOUNDZERO:	表示一次识别流程结束后，没有识别结果
//	LD_ASR_ERROR:		表示一次识别流程中LD3320芯片内部出现不正确的状态
/***********************************************************************************/
uint8 idata nAsrStatus=0;	
void MCU_init(); 
void ProcessInt0(); //识别处理函数
void  delay(unsigned long uldata);
void 	User_handle(uint8 dat);//用户执行操作函数
void Delay200ms();
void Led_test(void);//单片机工作指示
uint8_t G0_flag=DISABLE;//运行标志，ENABLE:运行。DISABLE:禁止运行 
sbit LED=P4^2;//信号指示灯

/***********************************************************
* 名    称： void  main(void)
* 功    能： 主函数	程序入口
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void  main(void)
{
	uint8 idata nAsrRes;
	uint8 i=0;
	Led_test();
	MCU_init();
	LD_Reset();
	UartIni(); /*串口初始化*/
	nAsrStatus = LD_ASR_NONE;		//	初始状态：没有在作ASR
	
	#ifdef TEST	
    PrintCom("一级口令：小杰\r\n"); /*text.....*/
	PrintCom("二级口令：1、代码测试\r\n"); /*text.....*/
	PrintCom("	2、开发板验证\r\n"); /*text.....*/
	PrintCom("	3、开灯\r\n"); /*text.....*/
	PrintCom("	4、关灯\r\n"); /*text.....*/
	PrintCom("  5、北京\r\n"); /*text.....*/
	PrintCom("	6、上海\r\n"); /*text.....*/
	PrintCom("	7、广州\r\n"); /*text.....*/
	#endif

	while(1)
	{
		switch(nAsrStatus)
		{
			case LD_ASR_RUNING:
			case LD_ASR_ERROR:		
				break;
			case LD_ASR_NONE:
			{
				nAsrStatus=LD_ASR_RUNING;
				if (RunASR()==0)	/*	启动一次ASR识别流程：ASR初始化，ASR添加关键词语，启动ASR运算*/
				{
					nAsrStatus = LD_ASR_ERROR;
				}
				break;
			}
			case LD_ASR_FOUNDOK: /*	一次ASR识别流程结束，去取ASR识别结果*/
			{				
				nAsrRes = LD_GetResult();		/*获取结果*/
				User_handle(nAsrRes);//用户执行函数 
				nAsrStatus = LD_ASR_NONE;
				break;
			}
			case LD_ASR_FOUNDZERO:
			default:
			{
				nAsrStatus = LD_ASR_NONE;
				break;
			}
		}// switch	 			
	}// while

}
/***********************************************************
* 名    称： 	 LED灯测试
* 功    能： 单片机是否工作指示
* 入口参数： 无 
* 出口参数：无
* 说    明： 					 
**********************************************************/
void Led_test(void)
{
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
}
/***********************************************************
* 名    称： void MCU_init()
* 功    能： 单片机初始化
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void MCU_init()
{
	P0 = 0xff;
	P1 = 0xff;
	P2 = 0xff;
	P3 = 0xff;
	P4 = 0xff;

	P1M0=0XFF;	//P1端口设置为推挽输出功能，即提高IO口驱动能力，从驱动继电器模块工作
	P1M1=0X00;

	LD_MODE = 0;		//	设置MD管脚为低，并行模式读写
	IE0=1;
	EX0=1;
	EA=1;
}
/***********************************************************
* 名    称：	延时函数
* 功    能：
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void Delay200us()		//@22.1184MHz
{
	unsigned char i, j;
	_nop_();
	_nop_();
	i = 5;
	j = 73;
	do
	{
		while (--j);
	} while (--i);
}

void  delay(unsigned long uldata)
{
	unsigned int j  =  0;
	unsigned int g  =  0;
	while(uldata--)
	Delay200us();
}

void Delay200ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	i = 17;
	j = 208;
	k = 27;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
/***********************************************************
* 名    称： 中断处理函数
* 功    能：
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void ExtInt0Handler(void) interrupt 0  
{ 	
	ProcessInt0();				/*	LD3320 送出中断信号，包括ASR和播放MP3的中断，需要在中断处理函数中分别处理*/
}
/***********************************************************
* 名    称：用户执行函数 
* 功    能：识别成功后，执行动作可在此进行修改 
* 入口参数： 无 
* 出口参数：无
* 说    明： 					 
**********************************************************/
void 	User_handle(uint8 dat)
{
     //UARTSendByte(dat);//串口识别码（十六进制）
			 LED=0;
			 switch(dat)		   /*对结果执行相关操作,客户修改*/
			  {															//串口发送1 对应干垃圾
				    case CODE_g1:			/*命令“测试”*/
						PrintCom("switch manual"); /*text.....*/
//						PrintCom("1\r\n"); /*text.....*/
//						PrintCom("1\r\n"); /*text.....*/	
												 break;
					case CODE_g2:	 /*命令“全开”*/
						PrintCom("switch auto"); /*text.....*/
//						PrintCom("2\r\n"); /*text.....*/
//						PrintCom("2\r\n"); /*text.....*/	
												 break;
					case CODE_g3:		/*命令“复位”*/				
						PrintCom("open Vent"); /*text.....*/
//						PrintCom("3\r\n"); /*text.....*/
//						PrintCom("3\r\n"); /*text.....*/
													break;
					case CODE_g4:		/*命令“复位”*/				
						PrintCom("close Vent"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/	
												 break;
					case CODE_g5:		/*命令“复位”*/				
						PrintCom("open Servo"); /*text.....*/	  
//						PrintCom("5\r\n"); /*text.....*/
//						PrintCom("5\r\n"); /*text.....*/	
												 break;
					case CODE_g6:		/*命令“复位”*/				
						PrintCom("close Servo"); /*text.....*/		   
//						PrintCom("6\r\n"); /*text.....*/
//						PrintCom("6\r\n"); /*text.....*/	
												 break;
					case CODE_g7:		/*命令“复位”*/				
						PrintCom("open Humid"); /*text.....*/	 
//						PrintCom("1\r\n"); /*text.....*/
//						PrintCom("1\r\n"); /*text.....*/	
												 break;
					case CODE_g8:		/*命令“复位”*/				
						PrintCom("close Humid"); /*text.....*/
//						PrintCom("1\r\n"); /*text.....*/
//						PrintCom("1\r\n"); /*text.....*/	
												 break;
					case CODE_g9:		/*命令“复位”*/				
						PrintCom("open Heat"); /*text.....*/	  
//						PrintCom("1\r\n"); /*text.....*/
//						PrintCom("1\r\n"); /*text.....*/	
												 break;
					case CODE_g10:		/*命令“复位”*/				
						PrintCom("close Heat"); /*text.....*/		  
//						PrintCom("1\r\n"); /*text.....*/	
//						PrintCom("1\r\n"); /*text.....*/	
												 break;
					case CODE_g11:		/*命令“复位”*/				
						PrintCom("open Water"); /*text.....*/		  
//						PrintCom("1\r\n"); /*text.....*/	
//						PrintCom("1\r\n"); /*text.....*/	
												 break;					

											
													
					 case CODE_s1:			/*命令“测试”*/		   	//串口发送2 对应湿垃圾
						PrintCom("close Water"); /*text.....*/
//						PrintCom("2\r\n"); /*text.....*/	
//						PrintCom("2\r\n"); /*text.....*/
											 break;
					case CODE_s2:	 /*命令“全开”*/
						PrintCom("open Light"); /*text.....*/
//						PrintCom("2\r\n"); /*text.....*/			
//						PrintCom("2\r\n"); /*text.....*/
											 break;
					case CODE_s3:		/*命令“复位”*/				
						PrintCom("close Light"); /*text.....*/
//						PrintCom("2\r\n"); /*text.....*/		
//						PrintCom("2\r\n"); /*text.....*/
//											 break;
//					case CODE_s4:		/*命令“复位”*/				
//						PrintCom("2\r\n"); /*text.....*/
//						PrintCom("2\r\n"); /*text.....*/		
//						PrintCom("2\r\n"); /*text.....*/
//											 break;
//					case CODE_s5:		/*命令“复位”*/				
//						PrintCom("2\r\n"); /*text.....*/	 
//						PrintCom("2\r\n"); /*text.....*/			
//						PrintCom("2\r\n"); /*text.....*/
//											 break;
//					case CODE_s6:		/*命令“复位”*/				
//						PrintCom("2\r\n"); /*text.....*/		   
//						PrintCom("2\r\n"); /*text.....*/				
//						PrintCom("2\r\n"); /*text.....*/
//											 break;
//					case CODE_s7:		/*命令“复位”*/				
//						PrintCom("2\r\n"); /*text.....*/	  
//						PrintCom("2\r\n"); /*text.....*/			
//						PrintCom("2\r\n"); /*text.....*/
//											 break;
//					case CODE_s8:		/*命令“复位”*/				
//						PrintCom("2\r\n"); /*text.....*/
//						PrintCom("2\r\n"); /*text.....*/					
//						PrintCom("2\r\n"); /*text.....*/
//											 break;
//					case CODE_s9:		/*命令“复位”*/				
//						PrintCom("2\r\n"); /*text.....*/	
//						PrintCom("2\r\n"); /*text.....*/				
//						PrintCom("2\r\n"); /*text.....*/
//											 break;
//					case CODE_s10:		/*命令“复位”*/				
//						PrintCom("2\r\n"); /*text.....*/		   
//						PrintCom("2\r\n"); /*text.....*/				
//						PrintCom("2\r\n"); /*text.....*/
//											 break;
//	             	case CODE_s11:		/*命令“复位”*/				
//						PrintCom("2\r\n"); /*text.....*/		   
//						PrintCom("2\r\n"); /*text.....*/				
//						PrintCom("2\r\n"); /*text.....*/
//											 break;

//					
//					 case CODE_k1:			/*命令“测试”*/		    //串口发送3 对应有害垃圾
//						PrintCom("3\r\n"); /*text.....*/
//						PrintCom("3\r\n"); /*text.....*/		
//						PrintCom("3\r\n"); /*text.....*/
//												 break;
//					case CODE_k2:	 /*命令“全开”*/
//						PrintCom("3\r\n"); /*text.....*/
//								PrintCom("3\r\n"); /*text.....*/		
//							PrintCom("3\r\n"); /*text.....*/
//												 break;
//					case CODE_k3:		/*命令“复位”*/				
//						PrintCom("3\r\n"); /*text.....*/
//				    	PrintCom("3\r\n"); /*text.....*/		
//						PrintCom("3\r\n"); /*text.....*/
//												 break;
//					case CODE_k4:		/*命令“复位”*/				
//						PrintCom("3\r\n"); /*text.....*/
//						PrintCom("3\r\n"); /*text.....*/				
//						PrintCom("3\r\n"); /*text.....*/
//												 break;
//					case CODE_k5:		/*命令“复位”*/				
//						PrintCom("3\r\n"); /*text.....*/	  
//						PrintCom("3\r\n"); /*text.....*/		
//						PrintCom("3\r\n"); /*text.....*/
//												 break;
//					case CODE_k6:		/*命令“复位”*/				
//						PrintCom("3\r\n"); /*text.....*/		   
//						PrintCom("3\r\n"); /*text.....*/				
//						PrintCom("3\r\n"); /*text.....*/
//												 break;
//					case CODE_k7:		/*命令“复位”*/				
//						PrintCom("3\r\n"); /*text.....*/	  
//						PrintCom("3\r\n"); /*text.....*/			
//						PrintCom("3\r\n"); /*text.....*/
//												 break;
//					case CODE_k8:		/*命令“复位”*/				
//						PrintCom("3\r\n"); /*text.....*/
//						PrintCom("3\r\n"); /*text.....*/					
//						PrintCom("3\r\n"); /*text.....*/
//												 break;
//					case CODE_k9:		/*命令“复位”*/				
//						PrintCom("3\r\n"); /*text.....*/	 
//						PrintCom("3\r\n"); /*text.....*/				
//						PrintCom("3\r\n"); /*text.....*/
//												 break;
//					case CODE_k10:		/*命令“复位”*/				
//						PrintCom("3\r\n"); /*text.....*/		  
//						PrintCom("3\r\n"); /*text.....*/       			
//					    PrintCom("3\r\n"); /*text.....*/
//					case CODE_k11:		/*命令“复位”*/				
//						PrintCom("3\r\n"); /*text.....*/		  
//						PrintCom("3\r\n"); /*text.....*/       			
//						PrintCom("3\r\n"); /*text.....*/
//												 break;							




//							
//					case CODE_y1:			/*命令“测试”*/		   //串口发送4 对应可回收物
//						PrintCom("4\r\n"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/
//					    PrintCom("4\r\n"); /*text.....*/
//								break;
//					case CODE_y2:	 /*命令“全开”*/
//						PrintCom("4\r\n"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/
//					    PrintCom("4\r\n"); /*text.....*/
//								break;
//					case CODE_y3:		/*命令“复位”*/				
//						PrintCom("4\r\n"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/
//								break;
//					case CODE_y4:		/*命令“复位”*/				
//						PrintCom("4\r\n"); /*text.....*/
//					    PrintCom("4\r\n"); /*text.....*/
//					    PrintCom("4\r\n"); /*text.....*/
//								break;
//					case CODE_y5:		/*命令“复位”*/				
//						PrintCom("4\r\n"); /*text.....*/	 
//						PrintCom("4\r\n"); /*text.....*/	
//						PrintCom("4\r\n"); /*text.....*/
//								break;
//					case CODE_y6:		/*命令“复位”*/				
//						PrintCom("4\r\n"); /*text.....*/		   
//						PrintCom("4\r\n"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/
//								break;
//					case CODE_y7:		/*命令“复位”*/				
//						PrintCom("4\r\n"); /*text.....*/	 
//						PrintCom("4\r\n"); /*text.....*/	
//						PrintCom("4\r\n"); /*text.....*/
//								break;
//					case CODE_y8:		/*命令“复位”*/				
//						PrintCom("4\r\n"); /*text.....*/
//					    PrintCom("4\r\n"); /*text.....*/
//					    PrintCom("4\r\n"); /*text.....*/
//								break;
//					case CODE_y9:		/*命令“复位”*/				
//						PrintCom("4\r\n"); /*text.....*/	  
//						PrintCom("4\r\n"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/
//								break;
//					case CODE_y10:		/*命令“复位”*/				
//						PrintCom("4\r\n"); /*text.....*/	  
//						PrintCom("4\r\n"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/
//								break;
//					
//	                case CODE_y11:		/*命令“复位”*/				
//						PrintCom("4\r\n"); /*text.....*/	  
//						PrintCom("4\r\n"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/
//								break;

//																															
//					default:PrintCom("请重新识别发口令\r\n"); /*text.....*/break;
				   	
				}	
		  Delay200ms();
		LED=1;
}	 
