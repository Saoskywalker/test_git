#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//ADC 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

//#define ADC_TEMPERATURE_EN		//使用内部温度传感器 
//#define ADC_INT_EN	//使用ADC中断
#define ADC_CH_TEMP  	ADC_Channel_16 //温度传感器通道

void Adc_Init(void);
u16  Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times); 

#ifdef ADC_INT_EN
extern u16 ADC_Average_Result;
void Start_Adc_WithInt(u8 Channel); //start once adc use int
u8 Get_Adc_WithInt(u16 *Channel_ADC); //get once int ADC
void Start_Adc_Average_WithInt(u8 Channel, u8 i);	//start continous average convert
void Stop_Adc_Average_WithInt(void);	//stop continous average convert
void Get_Adc_Average_WithInt(void); //timer process
#endif

#endif 
