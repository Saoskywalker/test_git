#include "adc.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////
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

//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3
void Adc_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
#ifdef ADC_INT_EN
	NVIC_InitTypeDef NVIC_InitStructure;
#endif

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE); //使能ADC1通道时钟

	RCC_ADCCLKConfig(RCC_PCLK2_Div6); //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA7 作为模拟通道输入引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1); //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;					//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;						//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;					//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;								//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);									//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器

#ifdef ADC_TEMPERATURE_EN
	ADC_TempSensorVrefintCmd(ENABLE); //开启内部温度传感器
#endif

#ifdef ADC_INT_EN
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器

	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE); //开启ADC转换完成中断
#endif

	ADC_Cmd(ADC1, ENABLE);		//使能指定的ADC1
	ADC_ResetCalibration(ADC1); //使能复位校准
	while (ADC_GetResetCalibrationStatus(ADC1));		//等待复位校准结束
	ADC_StartCalibration(ADC1); //开启AD校准
	while (ADC_GetCalibrationStatus(ADC1));   //等待校准结束
			//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能
}
//获得ADC值
//ch:通道值 
u16 Get_Adc(u8 ch)
{
	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5); //ADC1,ADC通道,采样时间为239.5周期
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);							  //使能指定的ADC1的软件转换启动功能
	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
		;								 //等待转换结束
	return ADC_GetConversionValue(ADC1); //返回最近一次ADC1规则组的转换结果
}

/*查询方式获取
ch:获取通道号, 
times:平均次数*/
u16 Get_Adc_Average(u8 ch, u8 times)
{
	u32 temp_val = 0;
	u8 t;
	for (t = 0; t < times; t++)
	{
		temp_val += Get_Adc(ch);
		delay_us(100);
	}
	return temp_val / times;
}

/*中断方式获取, 获取平均需放定时里*/
#ifdef ADC_INT_EN

static u8 ADC_Success_Flag = 1;
static u16 ADC_Result = 0;
u16 ADC_Average_Result = 0;
static u8 ADC_Average_Stop = 1;
void Start_Adc_WithInt(u8 Channel) //start once adc use int
{
	//设置指定ADC的规则组通道，一个序列，采样时间
	//ADC1,ADC通道,采样时间为239.5周期
	ADC_RegularChannelConfig(ADC1, Channel, 1, ADC_SampleTime_239Cycles5);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能指定的ADC1的软件转换启动功能
}
u8 Get_Adc_WithInt(u16 *Channel_ADC) //get once int ADC
{
	if (ADC_Success_Flag)
	{
		ADC_Success_Flag = 0;
		*Channel_ADC = ADC_Result;
		return 1; //success
	}
	return 0; //fail
}
void Start_Adc_Average_WithInt(u8 Channel, u8 i)	//start continous average convert
{
	ADC_Average_Stop = 0;
	Atimes = i;
	//设置指定ADC的规则组通道，一个序列，采样时间
	//ADC1,ADC通道,采样时间为239.5周期
	ADC_RegularChannelConfig(ADC1, Channel, 1, ADC_SampleTime_239Cycles5);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能指定的ADC1的软件转换启动功能
}
void Stop_Adc_Average_WithInt(void)	//stop continous average convert
{
	ADC_Average_Stop = 1;
}

void Get_Adc_Average_WithInt(void)	//timer process
{
	static u32 temp_val = 0;
	static u8 t = 0;
	if (ADC_Average_Stop == 0)
	{
		if (ADC_Success_Flag)
		{
			if (++t < Atimes)
			{
				ADC_Success_Flag = 0;
				temp_val += ADC_Result;
				//设置指定ADC的规则组通道，一个序列，采样时间
				//ADC1,ADC通道,采样时间为239.5周期
				ADC_RegularChannelConfig(ADC1, Channel, 1, ADC_SampleTime_239Cycles5);
				ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能指定的ADC1的软件转换启动功能
			}
			else
			{
				ADC_Success_Flag = 0;
				temp_val += ADC_Result;
				ADC_Average_Result = temp_val / Atimes;
				temp_val = 0;
				//设置指定ADC的规则组通道，一个序列，采样时间
				//ADC1,ADC通道,采样时间为239.5周期
				ADC_RegularChannelConfig(ADC1, Channel, 1, ADC_SampleTime_239Cycles5);
				ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能指定的ADC1的软件转换启动功能
			}
		}
	}
}

/*ADC IRQ*/
void ADC1_2_IRQHandler()
{
	if (ADC_GetITStatus(ADC1, ADC_IT_EOC) == SET)
	{
		ADC_Success_Flag = 1;
		ADC_Result = ADC_GetConversionValue(ADC1);
		//delay_ms(1);
	}
	ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
}

#endif
