## 项目概述

​		本项目基于毕业设计（基于STM32的智能温室大棚控制系统设计），以STM32F103C8T6单片机为主控制器，通过温湿度、土壤湿度、光照强度、CO2浓度等传感器和舵机、加热片、风扇、按键等模块实现对温室大棚内环境的检测和控制，OLED显示各种控制参数。熟练掌握STM32单片机相关软件和传感器工作原理，实现基于STM32的智能温室大棚控制系统，并装配成为一个完整的硬件产品。可采集并智能控制温室大棚温湿度、土壤湿度、光照强度、CO2浓度；可实现各按键功能，并实现各种控制参数显示。

## 文件结构

```
graduation_design/
├── GD_demo/         # 毕业设计工程文件		
│   ├── CORE/   # 存放STM32内核寄存器定义映射，启动文件等文件	
│   ├── FWLIB/  # 固件库文件
│	│   ├── inc		# 固件库头文件
│	│   └── src		# 固件库源文件
│   ├── OBJ		# 存放目标文件
│   ├── SYSTEM	# 模块驱动文件
│	│   ├── delay# 延时模块驱动文件
│	│   ├── dht11# 温湿度模块驱动文件
│	│   ├── KEY# 独立按键模块驱动文件
│	│   ├── LED# LED控制模块驱动文件
│	│   ├── oled# oled显示器模块驱动文件
│	│   ├── PWM# 电机(风扇)输出控制驱动文件
│	│   ├── photosensitive_sensor# 光敏传感器模块驱动文件，ADC
│	│   ├── TIMER# 定时器驱动文件
│	│   ├── usart# 串口驱动文件
│	│   └── sys	# 位带操作文件
│   │── USER			# debug	keil相关文件
│   └── keilkilll.bat	# 编译输出文件删除脚本
│
├── SYSTEM/           	# 参考模块驱动文件
│
├── Template3.0/		# 参考工程文件
│
├── task				# 毕业设计(论文)任务书
│
├── main.c              # 参考main文件
│   
└── README.md           # 本文档，提供有关项目概述、主要内容、时间安排以及工作进度等信息
```

## 主要内容

1. 明确课题目标，搜集、查阅相关资料
2. 学习软件编程
3. 学习单片机硬件相关参数和完成硬件选型
4. 完成STM32的智能温室大棚控制系统软件编程
5. 完成STM32的智能温室大棚控制系统硬件设计
6. 完成和搭建硬件系统和整体功能测试；完成论文撰写
7. 完成论文撰写

## 时间安排

| 序 号 | 设计（论文）各阶段名称                                   | 日 期     |
| ----- | -------------------------------------------------------- | --------- |
| 1     | 查阅相关参考资料，熟悉毕业设计任务                       | 第1周     |
| 2     | 毕业实习，了解成像系统和软件平台，方案设计，撰写开题报告 | 第2-4周   |
| 3     | 根据课题要求编写算法                                     | 第5-9周   |
| 4     | 算法改进，测试和实验结果分析                             | 第10-11周 |
| 5     | 实验结果分析，毕业论文撰写                               | 第12-14周 |
| 6     | 准备答辩资料，毕业答辩                                   | 第15-16周 |

## 工作进度

|   工作内容   |        具体内容        | 完成状态 |
| :----------: | :--------------------: | :------: |
|   模块调试   |     温湿度模块驱动     |  已完成  |
|   模块调试   |       OLED显示器       |  已完成  |
|   模块调试   | 电机(风扇)输出控制驱动 |  已完成  |
|   模块调试   |        串口驱动        |  已完成  |
|   模块调试   |   光敏传感器模块驱动   |  已完成  |
|   模块调试   |    土壤湿度模块驱动    |  已完成  |
|   模块调试   |     加湿器模块驱动     |  未完成  |
|   模块调试   |     加热器模块驱动     |  未完成  |
|   模块调试   | 舵机(通风控制)模块驱动 |  已完成  |
|   模块调试   |  二氧化碳浓度模块驱动  |  未完成  |
|   算法调试   |   时间片任务调度算法   |  未完成  |
|   模块调试   |                        |          |
| 算法逻辑编写 |      模块控制逻辑      |  未完成  |
|              |                        |          |
|              |                        |          |
|              |                        |          |
|              |                        |          |
|              |                        |          |
|              |                        |          |

