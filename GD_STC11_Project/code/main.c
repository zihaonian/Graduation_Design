/***************************�����Ƶ���****************************
**  �������ƣ�YS-V0.7����ʶ��ģ����������
**	CPU: STC11L08XE
**	����22.1184MHZ
**	�����ʣ�9600 bit/S
**	���ײ�Ʒ��Ϣ��YS-V0.7����ʶ�𿪷���
**                http://yuesheng001.taobao.com
**  ���ߣ�zdings
**  ��ϵ��751956552@qq.com
**  �޸����ڣ�2013.9.13
**  ˵������ͨģʽ��ֱ�ӷ�����ʶ��
/***************************�����Ƶ���******************************/
#include "config.h"
/************************************************************************************/
//	nAsrStatus ������main�������б�ʾ�������е�״̬������LD3320оƬ�ڲ���״̬�Ĵ���
//	LD_ASR_NONE:		��ʾû������ASRʶ��
//	LD_ASR_RUNING��		��ʾLD3320������ASRʶ����
//	LD_ASR_FOUNDOK:		��ʾһ��ʶ�����̽�������һ��ʶ����
//	LD_ASR_FOUNDZERO:	��ʾһ��ʶ�����̽�����û��ʶ����
//	LD_ASR_ERROR:		��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬
/***********************************************************************************/
uint8 idata nAsrStatus=0;	
void MCU_init(); 
void ProcessInt0(); //ʶ������
void  delay(unsigned long uldata);
void 	User_handle(uint8 dat);//�û�ִ�в�������
void Delay200ms();
void Led_test(void);//��Ƭ������ָʾ
uint8_t G0_flag=DISABLE;//���б�־��ENABLE:���С�DISABLE:��ֹ���� 
sbit LED=P4^2;//�ź�ָʾ��

/***********************************************************
* ��    �ƣ� void  main(void)
* ��    �ܣ� ������	�������
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
**********************************************************/ 
void  main(void)
{
	uint8 idata nAsrRes;
	uint8 i=0;
	Led_test();
	MCU_init();
	LD_Reset();
	UartIni(); /*���ڳ�ʼ��*/
	nAsrStatus = LD_ASR_NONE;		//	��ʼ״̬��û������ASR
	
	#ifdef TEST	
    PrintCom("һ�����С��\r\n"); /*text.....*/
	PrintCom("�������1���������\r\n"); /*text.....*/
	PrintCom("	2����������֤\r\n"); /*text.....*/
	PrintCom("	3������\r\n"); /*text.....*/
	PrintCom("	4���ص�\r\n"); /*text.....*/
	PrintCom("  5������\r\n"); /*text.....*/
	PrintCom("	6���Ϻ�\r\n"); /*text.....*/
	PrintCom("	7������\r\n"); /*text.....*/
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
				if (RunASR()==0)	/*	����һ��ASRʶ�����̣�ASR��ʼ����ASR��ӹؼ��������ASR����*/
				{
					nAsrStatus = LD_ASR_ERROR;
				}
				break;
			}
			case LD_ASR_FOUNDOK: /*	һ��ASRʶ�����̽�����ȥȡASRʶ����*/
			{				
				nAsrRes = LD_GetResult();		/*��ȡ���*/
				User_handle(nAsrRes);//�û�ִ�к��� 
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
* ��    �ƣ� 	 LED�Ʋ���
* ��    �ܣ� ��Ƭ���Ƿ���ָʾ
* ��ڲ����� �� 
* ���ڲ�������
* ˵    ���� 					 
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
* ��    �ƣ� void MCU_init()
* ��    �ܣ� ��Ƭ����ʼ��
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
**********************************************************/ 
void MCU_init()
{
	P0 = 0xff;
	P1 = 0xff;
	P2 = 0xff;
	P3 = 0xff;
	P4 = 0xff;

	P1M0=0XFF;	//P1�˿�����Ϊ����������ܣ������IO�������������������̵���ģ�鹤��
	P1M1=0X00;

	LD_MODE = 0;		//	����MD�ܽ�Ϊ�ͣ�����ģʽ��д
	IE0=1;
	EX0=1;
	EA=1;
}
/***********************************************************
* ��    �ƣ�	��ʱ����
* ��    �ܣ�
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
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
* ��    �ƣ� �жϴ�����
* ��    �ܣ�
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
**********************************************************/ 
void ExtInt0Handler(void) interrupt 0  
{ 	
	ProcessInt0();				/*	LD3320 �ͳ��ж��źţ�����ASR�Ͳ���MP3���жϣ���Ҫ���жϴ������зֱ���*/
}
/***********************************************************
* ��    �ƣ��û�ִ�к��� 
* ��    �ܣ�ʶ��ɹ���ִ�ж������ڴ˽����޸� 
* ��ڲ����� �� 
* ���ڲ�������
* ˵    ���� 					 
**********************************************************/
void 	User_handle(uint8 dat)
{
     //UARTSendByte(dat);//����ʶ���루ʮ�����ƣ�
			 LED=0;
			 switch(dat)		   /*�Խ��ִ����ز���,�ͻ��޸�*/
			  {															//���ڷ���1 ��Ӧ������
				    case CODE_g1:			/*������ԡ�*/
						PrintCom("switch manual"); /*text.....*/
//						PrintCom("1\r\n"); /*text.....*/
//						PrintCom("1\r\n"); /*text.....*/	
												 break;
					case CODE_g2:	 /*���ȫ����*/
						PrintCom("switch auto"); /*text.....*/
//						PrintCom("2\r\n"); /*text.....*/
//						PrintCom("2\r\n"); /*text.....*/	
												 break;
					case CODE_g3:		/*�����λ��*/				
						PrintCom("open Vent"); /*text.....*/
//						PrintCom("3\r\n"); /*text.....*/
//						PrintCom("3\r\n"); /*text.....*/
													break;
					case CODE_g4:		/*�����λ��*/				
						PrintCom("close Vent"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/	
												 break;
					case CODE_g5:		/*�����λ��*/				
						PrintCom("open Servo"); /*text.....*/	  
//						PrintCom("5\r\n"); /*text.....*/
//						PrintCom("5\r\n"); /*text.....*/	
												 break;
					case CODE_g6:		/*�����λ��*/				
						PrintCom("close Servo"); /*text.....*/		   
//						PrintCom("6\r\n"); /*text.....*/
//						PrintCom("6\r\n"); /*text.....*/	
												 break;
					case CODE_g7:		/*�����λ��*/				
						PrintCom("open Humid"); /*text.....*/	 
//						PrintCom("1\r\n"); /*text.....*/
//						PrintCom("1\r\n"); /*text.....*/	
												 break;
					case CODE_g8:		/*�����λ��*/				
						PrintCom("close Humid"); /*text.....*/
//						PrintCom("1\r\n"); /*text.....*/
//						PrintCom("1\r\n"); /*text.....*/	
												 break;
					case CODE_g9:		/*�����λ��*/				
						PrintCom("open Heat"); /*text.....*/	  
//						PrintCom("1\r\n"); /*text.....*/
//						PrintCom("1\r\n"); /*text.....*/	
												 break;
					case CODE_g10:		/*�����λ��*/				
						PrintCom("close Heat"); /*text.....*/		  
//						PrintCom("1\r\n"); /*text.....*/	
//						PrintCom("1\r\n"); /*text.....*/	
												 break;
					case CODE_g11:		/*�����λ��*/				
						PrintCom("open Water"); /*text.....*/		  
//						PrintCom("1\r\n"); /*text.....*/	
//						PrintCom("1\r\n"); /*text.....*/	
												 break;					

											
													
					 case CODE_s1:			/*������ԡ�*/		   	//���ڷ���2 ��Ӧʪ����
						PrintCom("close Water"); /*text.....*/
//						PrintCom("2\r\n"); /*text.....*/	
//						PrintCom("2\r\n"); /*text.....*/
											 break;
					case CODE_s2:	 /*���ȫ����*/
						PrintCom("open Light"); /*text.....*/
//						PrintCom("2\r\n"); /*text.....*/			
//						PrintCom("2\r\n"); /*text.....*/
											 break;
					case CODE_s3:		/*�����λ��*/				
						PrintCom("close Light"); /*text.....*/
//						PrintCom("2\r\n"); /*text.....*/		
//						PrintCom("2\r\n"); /*text.....*/
//											 break;
//					case CODE_s4:		/*�����λ��*/				
//						PrintCom("2\r\n"); /*text.....*/
//						PrintCom("2\r\n"); /*text.....*/		
//						PrintCom("2\r\n"); /*text.....*/
//											 break;
//					case CODE_s5:		/*�����λ��*/				
//						PrintCom("2\r\n"); /*text.....*/	 
//						PrintCom("2\r\n"); /*text.....*/			
//						PrintCom("2\r\n"); /*text.....*/
//											 break;
//					case CODE_s6:		/*�����λ��*/				
//						PrintCom("2\r\n"); /*text.....*/		   
//						PrintCom("2\r\n"); /*text.....*/				
//						PrintCom("2\r\n"); /*text.....*/
//											 break;
//					case CODE_s7:		/*�����λ��*/				
//						PrintCom("2\r\n"); /*text.....*/	  
//						PrintCom("2\r\n"); /*text.....*/			
//						PrintCom("2\r\n"); /*text.....*/
//											 break;
//					case CODE_s8:		/*�����λ��*/				
//						PrintCom("2\r\n"); /*text.....*/
//						PrintCom("2\r\n"); /*text.....*/					
//						PrintCom("2\r\n"); /*text.....*/
//											 break;
//					case CODE_s9:		/*�����λ��*/				
//						PrintCom("2\r\n"); /*text.....*/	
//						PrintCom("2\r\n"); /*text.....*/				
//						PrintCom("2\r\n"); /*text.....*/
//											 break;
//					case CODE_s10:		/*�����λ��*/				
//						PrintCom("2\r\n"); /*text.....*/		   
//						PrintCom("2\r\n"); /*text.....*/				
//						PrintCom("2\r\n"); /*text.....*/
//											 break;
//	             	case CODE_s11:		/*�����λ��*/				
//						PrintCom("2\r\n"); /*text.....*/		   
//						PrintCom("2\r\n"); /*text.....*/				
//						PrintCom("2\r\n"); /*text.....*/
//											 break;

//					
//					 case CODE_k1:			/*������ԡ�*/		    //���ڷ���3 ��Ӧ�к�����
//						PrintCom("3\r\n"); /*text.....*/
//						PrintCom("3\r\n"); /*text.....*/		
//						PrintCom("3\r\n"); /*text.....*/
//												 break;
//					case CODE_k2:	 /*���ȫ����*/
//						PrintCom("3\r\n"); /*text.....*/
//								PrintCom("3\r\n"); /*text.....*/		
//							PrintCom("3\r\n"); /*text.....*/
//												 break;
//					case CODE_k3:		/*�����λ��*/				
//						PrintCom("3\r\n"); /*text.....*/
//				    	PrintCom("3\r\n"); /*text.....*/		
//						PrintCom("3\r\n"); /*text.....*/
//												 break;
//					case CODE_k4:		/*�����λ��*/				
//						PrintCom("3\r\n"); /*text.....*/
//						PrintCom("3\r\n"); /*text.....*/				
//						PrintCom("3\r\n"); /*text.....*/
//												 break;
//					case CODE_k5:		/*�����λ��*/				
//						PrintCom("3\r\n"); /*text.....*/	  
//						PrintCom("3\r\n"); /*text.....*/		
//						PrintCom("3\r\n"); /*text.....*/
//												 break;
//					case CODE_k6:		/*�����λ��*/				
//						PrintCom("3\r\n"); /*text.....*/		   
//						PrintCom("3\r\n"); /*text.....*/				
//						PrintCom("3\r\n"); /*text.....*/
//												 break;
//					case CODE_k7:		/*�����λ��*/				
//						PrintCom("3\r\n"); /*text.....*/	  
//						PrintCom("3\r\n"); /*text.....*/			
//						PrintCom("3\r\n"); /*text.....*/
//												 break;
//					case CODE_k8:		/*�����λ��*/				
//						PrintCom("3\r\n"); /*text.....*/
//						PrintCom("3\r\n"); /*text.....*/					
//						PrintCom("3\r\n"); /*text.....*/
//												 break;
//					case CODE_k9:		/*�����λ��*/				
//						PrintCom("3\r\n"); /*text.....*/	 
//						PrintCom("3\r\n"); /*text.....*/				
//						PrintCom("3\r\n"); /*text.....*/
//												 break;
//					case CODE_k10:		/*�����λ��*/				
//						PrintCom("3\r\n"); /*text.....*/		  
//						PrintCom("3\r\n"); /*text.....*/       			
//					    PrintCom("3\r\n"); /*text.....*/
//					case CODE_k11:		/*�����λ��*/				
//						PrintCom("3\r\n"); /*text.....*/		  
//						PrintCom("3\r\n"); /*text.....*/       			
//						PrintCom("3\r\n"); /*text.....*/
//												 break;							




//							
//					case CODE_y1:			/*������ԡ�*/		   //���ڷ���4 ��Ӧ�ɻ�����
//						PrintCom("4\r\n"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/
//					    PrintCom("4\r\n"); /*text.....*/
//								break;
//					case CODE_y2:	 /*���ȫ����*/
//						PrintCom("4\r\n"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/
//					    PrintCom("4\r\n"); /*text.....*/
//								break;
//					case CODE_y3:		/*�����λ��*/				
//						PrintCom("4\r\n"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/
//								break;
//					case CODE_y4:		/*�����λ��*/				
//						PrintCom("4\r\n"); /*text.....*/
//					    PrintCom("4\r\n"); /*text.....*/
//					    PrintCom("4\r\n"); /*text.....*/
//								break;
//					case CODE_y5:		/*�����λ��*/				
//						PrintCom("4\r\n"); /*text.....*/	 
//						PrintCom("4\r\n"); /*text.....*/	
//						PrintCom("4\r\n"); /*text.....*/
//								break;
//					case CODE_y6:		/*�����λ��*/				
//						PrintCom("4\r\n"); /*text.....*/		   
//						PrintCom("4\r\n"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/
//								break;
//					case CODE_y7:		/*�����λ��*/				
//						PrintCom("4\r\n"); /*text.....*/	 
//						PrintCom("4\r\n"); /*text.....*/	
//						PrintCom("4\r\n"); /*text.....*/
//								break;
//					case CODE_y8:		/*�����λ��*/				
//						PrintCom("4\r\n"); /*text.....*/
//					    PrintCom("4\r\n"); /*text.....*/
//					    PrintCom("4\r\n"); /*text.....*/
//								break;
//					case CODE_y9:		/*�����λ��*/				
//						PrintCom("4\r\n"); /*text.....*/	  
//						PrintCom("4\r\n"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/
//								break;
//					case CODE_y10:		/*�����λ��*/				
//						PrintCom("4\r\n"); /*text.....*/	  
//						PrintCom("4\r\n"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/
//								break;
//					
//	                case CODE_y11:		/*�����λ��*/				
//						PrintCom("4\r\n"); /*text.....*/	  
//						PrintCom("4\r\n"); /*text.....*/
//						PrintCom("4\r\n"); /*text.....*/
//								break;

//																															
//					default:PrintCom("������ʶ�𷢿���\r\n"); /*text.....*/break;
				   	
				}	
		  Delay200ms();
		LED=1;
}	 
