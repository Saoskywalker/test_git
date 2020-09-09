#include "adc.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////
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

//��ʼ��ADC
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ͨ��0~3
void Adc_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
#ifdef ADC_INT_EN
	NVIC_InitTypeDef NVIC_InitStructure;
#endif

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE); //ʹ��ADC1ͨ��ʱ��

	RCC_ADCCLKConfig(RCC_PCLK2_Div6); //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	//PA7 ��Ϊģ��ͨ����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1); //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;					//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;						//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;					//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;								//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);									//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���

#ifdef ADC_TEMPERATURE_EN
	ADC_TempSensorVrefintCmd(ENABLE); //�����ڲ��¶ȴ�����
#endif

#ifdef ADC_INT_EN
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure); //����ָ���Ĳ�����ʼ��VIC�Ĵ���

	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE); //����ADCת������ж�
#endif

	ADC_Cmd(ADC1, ENABLE);		//ʹ��ָ����ADC1
	ADC_ResetCalibration(ADC1); //ʹ�ܸ�λУ׼
	while (ADC_GetResetCalibrationStatus(ADC1));		//�ȴ���λУ׼����
	ADC_StartCalibration(ADC1); //����ADУ׼
	while (ADC_GetCalibrationStatus(ADC1));   //�ȴ�У׼����
			//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������
}
//���ADCֵ
//ch:ͨ��ֵ 
u16 Get_Adc(u8 ch)
{
	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5); //ADC1,ADCͨ��,����ʱ��Ϊ239.5����
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);							  //ʹ��ָ����ADC1�����ת����������
	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
		;								 //�ȴ�ת������
	return ADC_GetConversionValue(ADC1); //�������һ��ADC1�������ת�����
}

/*��ѯ��ʽ��ȡ
ch:��ȡͨ����, 
times:ƽ������*/
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

/*�жϷ�ʽ��ȡ, ��ȡƽ����Ŷ�ʱ��*/
#ifdef ADC_INT_EN

static u8 ADC_Success_Flag = 1;
static u16 ADC_Result = 0;
u16 ADC_Average_Result = 0;
static u8 ADC_Average_Stop = 1;
void Start_Adc_WithInt(u8 Channel) //start once adc use int
{
	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����
	ADC_RegularChannelConfig(ADC1, Channel, 1, ADC_SampleTime_239Cycles5);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //ʹ��ָ����ADC1�����ת����������
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
	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����
	ADC_RegularChannelConfig(ADC1, Channel, 1, ADC_SampleTime_239Cycles5);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //ʹ��ָ����ADC1�����ת����������
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
				//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
				//ADC1,ADCͨ��,����ʱ��Ϊ239.5����
				ADC_RegularChannelConfig(ADC1, Channel, 1, ADC_SampleTime_239Cycles5);
				ADC_SoftwareStartConvCmd(ADC1, ENABLE); //ʹ��ָ����ADC1�����ת����������
			}
			else
			{
				ADC_Success_Flag = 0;
				temp_val += ADC_Result;
				ADC_Average_Result = temp_val / Atimes;
				temp_val = 0;
				//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
				//ADC1,ADCͨ��,����ʱ��Ϊ239.5����
				ADC_RegularChannelConfig(ADC1, Channel, 1, ADC_SampleTime_239Cycles5);
				ADC_SoftwareStartConvCmd(ADC1, ENABLE); //ʹ��ָ����ADC1�����ת����������
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
