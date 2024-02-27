#include "sys.h"
#include "usart.h"	
#include "string.h"	
#include "led.h"


#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					  
#endif

  uint8_t rx_cnt = 0;   
  uint8_t usart_idle_flag = 0; 


#if 1
#pragma import(__use_no_semihosting)             
              
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
  
_sys_exit(int x) 
{ 
	x = x; 
} 

int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 
#if EN_USART1_RX   	
uint8_t USART_RX_BUF[USART_REC_LEN];    
void uart_init(u32 bound){
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
	//PA9接蓝牙RXD  PA10接蓝牙TXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);	// 开启空闲中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

#endif
	
}
#endif	

 

void USART1_IRQHandler(void)
	{
	uint32_t data;	
	if(USART_GetITStatus(USART1,USART_IT_RXNE))
	{               
		USART_RX_BUF[rx_cnt] = USART_ReceiveData(USART1);  
		rx_cnt++;
	}
	
	if(USART_GetITStatus(USART1,USART_IT_IDLE) != RESET)
	{ 
		data = USART1->SR;		 //清空闲中断
		data = USART1->DR;        
		usart_idle_flag = 1;	 
//	if(strcmp(USART_RX_BUF, "led on") ==  0)
//		{      //判断两个字符串相等
//		 LED_Open();  //置低位，LED灯亮
//		 memset(USART_RX_BUF,0,sizeof(USART_RX_BUF));   //清空数组
//		 rx_cnt = 0;                          //清空索引置为0
//		} 
//	if(strcmp(USART_RX_BUF, "led off") == 0)
//		{    //判断两个字符串相等
//		 LED_Close();   //置高位，LED灯灭
//		 memset(USART_RX_BUF,0,sizeof(USART_RX_BUF)); //清空数组
//		 rx_cnt = 0;                        //清空索引置为0
//		}
	}
}












