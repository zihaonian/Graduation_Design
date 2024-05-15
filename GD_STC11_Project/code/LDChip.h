
#ifndef LD_CHIP_H
#define LD_CHIP_H

#define uint8 unsigned char
#define uint16 unsigned int
#define uint32 unsigned long

//	以下五个状态定义用来记录程序是在运行ASR识别过程中的哪个状态
#define LD_ASR_NONE				0x00	 /*	表示没有在作ASR识别*/
#define LD_ASR_RUNING			0x01	/*	表示LD3320正在作ASR识别中*/
#define LD_ASR_FOUNDOK			0x10	/*表示一次识别流程结束后，有一个识别结果*/
#define LD_ASR_FOUNDZERO 		0x11	/*表示一次识别流程结束后，没有识别结果*/
#define LD_ASR_ERROR	 		0x31	/*表示一次识别流程中LD3320芯片内部出现不正确的状态*/


#define CLK_IN   		22.1184	/* 用户注意修改输入的晶振时钟大小 */
#define LD_PLL_11			(uint8)((CLK_IN/2.0)-1)
#define LD_PLL_MP3_19		0x0f
#define LD_PLL_MP3_1B		0x18
#define LD_PLL_MP3_1D   	(uint8)(((90.0*((LD_PLL_11)+1))/(CLK_IN))-1)

#define LD_PLL_ASR_19 		(uint8)(CLK_IN*32.0/(LD_PLL_11+1) - 0.51)
#define LD_PLL_ASR_1B 		0x48
#define LD_PLL_ASR_1D 		0x1f

//函数声明
void LD_Reset();
void LD_Init_Common();
void LD_Init_ASR();
uint8 RunASR(void);
void LD_AsrStart();
uint8 LD_AsrRun();
uint8 LD_AsrAddFixed();
uint8 LD_GetResult();


//识别码客户修改处 
#define CODE_CMD  0x00   //该命令码0x00用户不可进行修改。
#define CODE_DMCS	0x01		//代码测试
#define CODE_KFBYZ	0x02	//开发板验证
#define CODE_KD 0x04			//开灯
#define CODE_GD 0x05			//关灯
#define CODE_BJ 0x16			//关灯
#define CODE_SH 0x17			//上海
#define CODE_GZ	0x2f			//广州

#define	CODE_g1  0x22
#define	CODE_g2  0x23														
#define	CODE_g3  0x24
#define	CODE_g4  0x25														
#define	CODE_g5  0x26
#define	CODE_g6  0x27															
#define	CODE_g7  0x28
#define	CODE_g8  0x29															
#define	CODE_g9  0x2a															
#define	CODE_g10  0x2b
#define	CODE_g11  0x2c	

#define	CODE_s1  0x31
#define	CODE_s2  0x32														
#define	CODE_s3  0x33
#define	CODE_s4  0x34														
#define	CODE_s5  0x35
#define	CODE_s6  0x36															
#define	CODE_s7  0x37
#define	CODE_s8  0x38															
#define	CODE_s9  0x39
#define	CODE_s10 0x3a															
#define	CODE_s11 0x3b														
#define	CODE_k1  0x41
#define	CODE_k2  0x42														
#define	CODE_k3  0x43
#define	CODE_k4  0x44														
#define	CODE_k5  0x45
#define	CODE_k6  0x46															
#define	CODE_k7  0x47
#define	CODE_k8  0x48															
#define	CODE_k9  0x49														
#define	CODE_k10  0x4a															
#define	CODE_k11  0x4b
#define	CODE_y1  0x51
#define	CODE_y2  0x52														
#define	CODE_y3  0x53
#define	CODE_y4  0x54														
#define	CODE_y5  0x55
#define	CODE_y6  0x56															
#define	CODE_y7  0x57
#define	CODE_y8  0x58															
#define	CODE_y9  0x59														
#define	CODE_y10  0x69							   		
#define	CODE_y11  0x6a														

#define MIC_VOL 0x55	 //咪头增益（灵敏度调节）
#endif
