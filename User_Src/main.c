/**********************************************************************
Name: Internal Lipolysis Instrument(display contral)
Version: v2.3 Released
Editor: Aysi
Function: 3 channel strength RF
History:2018.8.3: Debug completed
		2018.12.26: change version to vertical RF
		add NTC(PA4 AIN4), sound pin became pump pin(delay_us 500)
		 develop success
		 2019.2.25: NTC error dithering elimination
		 2019.3.13; NTC changed PA7, PA4 use to sound pin
		 2019.4.9: Add WorkSecFlag use for RF(2 WorkFlag)
		 2019.4.15: RF work way changed
		 2019.5.23: NTC current changed(Changed AD value)
**********************************************************************/

//#define DEBUG
#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "timer.h"
#include "AppLib.h"
#include "24cxx.h"

void screenCheck(void);

int main()
{
	u16 i = 0, j = 0;

	LED_Init();
	NVIC_Configuration(); //nvic set 2:2
	delay_init();
	uart_init(9600);			 //Main Board
	uart2_init(115200);			 //DW Display
	TIM4_PWM_Init(7200, 0);		 //10Khz.
	TIM_SetCompare3(TIM4, 3000); //4.6v
	screenCheck();
	delay_ms(1000);
	dwStopMusic();
	TIM3_Int_Init(99, 70); //100us
	Adc_Init();
	AT24CXX_Init();
	nextPage = FUNCTION_SELECT;
	dwSetColor(DW_COL_RED, DW_COL_WHITE);
	INLINE_MUSIC_START();
	delay_ms(100);
	delay_ms(100

	STMFLASH_Read(FLASH_SAVE_ADDR+2, &j, 1); //mute state
	muteFlag = j;
	STMFLASH_Read(FLASH_SAVE_ADDR, &i, 1); //read language config ago
	if (i)
		dwSetLanguage(LANGUAGE_CHINESE);
	else
		dwSetLanguage(LANGUAGE_ENGLISH);
		

	//#ifndef DEBUG
	//	IWDG_Init(4,625);    //psc 64,reload 625,voertime 1s	
	//#endif
	while (1)
	{
		switch (nextPage)
		{
		case FUNCTION_RESTART:
		{
			PageRestartAnimation();
			break;
		}
		case FUNCTION_LANGUAGE:
		{
			PageLanguage();
			break;
		}
		case FUNCTION_FACE:
		{
			PageFaceRF();
			break;
		}
		case FUNCTION_BODY:
		{
			PageBodyRF();
			break;
		}
		case FUNCTION_BIO1:
		{
			PageBIO1();
			break;
		}
		case FUNCTION_O2:
		{
			PageO2();
			break;
		}
		case FUNCTION_O2_BIG:
		{
			PageO2Big();
			break;
		}
		case FUNCTION_SELECT:
		{
			PageMenu();
			break;
		}
		case FUNCTION_EYE:
		{
			PageEyeRF();
			break;
		}
		case FUNCTION_CARD:
		{
			PageCard();
			break;
		}
		default:
		{
			break;
		}
		}
	}
}

void screenCheck(void)
{
	u8 i;
	//Check communicate with DW Display, continue 5s
	for (i = 0; i < 25; i++)
	{
		dwSendByte(0xAA);
		dwSendByte(0x00);  // 发送握手指令
		dwSendOver();
		delay_ms(5);
		if (dwQueryCmd() == 0x74)
		{ // 握手成功
			i = 0xFF;
			break;
		}
		delay_ms(200);
	}
	if (i != 0xFF)
	{ // 失败
		while (1)
		{
			SOUND_PIN = ~SOUND_PIN;
			delay_ms(500);
		}
	}
}
