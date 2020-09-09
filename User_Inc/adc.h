#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//ADC ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

//#define ADC_TEMPERATURE_EN		//ʹ���ڲ��¶ȴ����� 
//#define ADC_INT_EN	//ʹ��ADC�ж�
#define ADC_CH_TEMP  	ADC_Channel_16 //�¶ȴ�����ͨ��

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
