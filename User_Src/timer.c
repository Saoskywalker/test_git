#include "timer.h"
#include "AppLib.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 

//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM3, ENABLE);  //使能TIMx					 
}
//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断 100us
{
	static u16 Flag1sCnt = 0, Flag200msCnt = 0, Flag1msCnt = 0;
	//static u8 KeyCnt = 0;
	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx更新中断标志 
//			#ifdef DEBUG
		// DEBUG_TIME_PIN = ~DEBUG_TIME_PIN;
//			#endif

		if (nextPage == FUNCTION_BIO1)
		{
			BIO1PWM(WorkMode, BitAppCon.WorkFlag);
			BIO1Power(WorkIntensity, BitAppCon.WorkFlag);
		}
		else
		{
			BIO1PWM(0, 0);
			BIO1Power(0, 0);
		}
			
		if(++Flag200msCnt>=2000)	//200ms, use for uart1 sending
		{
			Flag200msCnt = 0;
			BitAppCon.ms200 = 1;
		}
		
		if(++Flag1msCnt>=10)
		{
			Flag1msCnt = 0;
			BitAppCon.ms1 = 1;
			if (nextPage == FUNCTION_O2 || nextPage == FUNCTION_O2_BIG)
				O2PWM(WorkMode, BitAppCon.WorkFlag);
			else
				O2PWM(0, 0);
			if(nextPage == FUNCTION_O2_BIG)
				Heat2PWM(WorkIntensity, BitAppCon.WorkFlag);
			else
				Heat2PWM(0, 0);
			if(nextPage == FUNCTION_O2)
				HeatPWM(WorkIntensity, BitAppCon.WorkFlag);
			else
				HeatPWM(0, 0);
			BeeFunction();
/* 			if (nextPage != FUNCTION_CARD) //卡检测, 拔出就退出
			{
				if (SW_PIN == 1)
				{
					if (++KeyCnt >= 100)
					{
						KeyCnt = 0;
						nextPage = FUNCTION_CARD;
					}
				}
				else
				{
					KeyCnt = 0;
				}
			} */
		}

		if(++Flag1sCnt>=10000) //1s
		{
			if ((BitAppCon.WorkSecFlag == 1))
			{
				Flag1sCnt = 0;
				BitAppCon.Flag1s = 1;
			}
		}
			
	}
}

//TIM4 CH3 PWM输出设置 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM4_PWM_Init(u16 arr,u16 psc)
{		 					 
	
 	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //使能TIMx外设
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIOB外设时钟使能
	
 
   //设置该引脚为复用输出功能,输出TIM4 CH3的PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM_CH3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用功能输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化GPIO
 
	TIM_TimeBaseStructure.TIM_Period = arr; //设置自动重装载周期值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置预分频值 不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx
	
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //CH3 PWM2模式	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //OC3 低电平有效 
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  //根据指定的参数初始化外设TIMx

	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH3 预装载使能
	
	TIM_ARRPreloadConfig(TIM4, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	TIM_Cmd(TIM4, ENABLE);  //使能TIMx
	   										  
} 












