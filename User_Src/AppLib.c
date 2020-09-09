#include "AppLib.h"
#include "delay.h"
#include "24cxx.h"

_BitAppCon BitAppCon = {0, 0, 0, 0, 0, 0, 0, 0};
u8 nextPage = 0;
u8 requestPage = 0;

//Botton press effect
static const Button btnStart = {PIC_BODY_RF_P, 239, 465, 239+64, 465+64};
static const Button btnPause = {PIC_BODY_RF_P, 372, 465, 372+64, 465+64};
static const Button btnMute = {1, 714, 10, 773, 69};

//Botton original effect
static const Button btnStart_O = {PIC_BODY_RF, 239, 465, 239+64, 465+64};
static const Button btnPause_O = {PIC_BODY_RF, 372, 465, 372+64, 465+64};
static const Button btnMute_O = {2, 714, 10, 773, 69};

u8 CommSendFlag = 0;									  //Send Main Board Buffer or not
_Comm Comm = {FRAME_HEAD, RF_FACE, 1, RF_OFF, FRAME_END}; //Send Main Board Buffer
u16 WorkTime = 0;
u8 WorkIntensity = 0, WorkMode = 0, muteFlag = 0;
u16 WorkSuckTime = 0, WorkReleaseTime = 0;
u16 TimeX, TimeY, IntensityX, IntensityY, ModeX, ModeY, SuckX, SuckY, ReleaseX, ReleaseY;

/*language select*/
static void menuChinsePres(void)
{
	u16 i = 123;

	BitAppCon.menuExit = 1;
	nextPage = FUNCTION_SELECT;
	INLINE_MUSIC_BUTTON();
	dwSetLanguage(LANGUAGE_CHINESE);
	STMFLASH_Write(FLASH_SAVE_ADDR, &i, 1); //Write language config
}

static void menuEnglishPres(void)
{
	u16 i = 0;

	BitAppCon.menuExit = 1;
	nextPage = FUNCTION_SELECT;
	INLINE_MUSIC_BUTTON();
	dwSetLanguage(LANGUAGE_ENGLISH);
	STMFLASH_Write(FLASH_SAVE_ADDR, &i, 1); //Write language config
}

/*Menu select*/
static void menuEyeRFPres(void)
{
	// if (BitAppCon.WorkFlag)
	// {
	// }
	// else
	{
		nextPage = FUNCTION_EYE;
		INLINE_MUSIC_BUTTON();
	}
}

static void menuFaceRFPres(void)
{
	// if (BitAppCon.WorkFlag)
	// {
	// }
	// else
	{
		nextPage = FUNCTION_FACE;
		INLINE_MUSIC_BUTTON();
	}
}

static void menuBodyRFPres(void)
{
	// if (BitAppCon.WorkFlag)
	// {
	// }
	// else
	{
		nextPage = FUNCTION_BODY;
		INLINE_MUSIC_BUTTON();
	}
}

static void menuO2Pres(void)
{
	// if (BitAppCon.WorkFlag)
	// {
	// }
	// else
	{
		nextPage = FUNCTION_O2;
		INLINE_MUSIC_BUTTON();
	}
}

static void menuO2BigPres(void)
{
	// if (BitAppCon.WorkFlag)
	// {
	// }
	// else
	{
		nextPage = FUNCTION_O2_BIG;
		INLINE_MUSIC_BUTTON();
	}
}

static void menuBIO1Pres(void)
{
	// if (BitAppCon.WorkFlag)
	// {
	// }
	// else
	{
		nextPage = FUNCTION_BIO1;
		INLINE_MUSIC_BUTTON();
	}
}

/*Back button*/
static void BackPres(void)
{
	INLINE_MUSIC_BUTTON();
	nextPage = FUNCTION_SELECT;
}

/*SET button*/
static void SetPres(void)
{
	INLINE_MUSIC_BUTTON();
	nextPage = FUNCTION_LANGUAGE;
}

/*Start button*/
static void StartPres(void)
{
	INLINE_MUSIC_START();
	if (nextPage == FUNCTION_BIO1)
	{
		BitAppCon.WorkFlag = 1;
		BitAppCon.WorkSecFlag = 1;
		dwDisButton(&btnStart, 1, btnStart.xs, btnStart.ys);
		dwDisButton(&btnPause_O, 1, btnPause_O.xs, btnPause_O.ys);
	}
	else if (nextPage == FUNCTION_O2 || nextPage == FUNCTION_O2_BIG)
	{
		BitAppCon.WorkFlag = 1;
		BitAppCon.WorkSecFlag = 1;
		dwDisButton(&btnStart, 1, btnStart.xs, btnStart.ys);
		dwDisButton(&btnPause_O, 1, btnPause_O.xs, btnPause_O.ys);
	}
	else if (nextPage == FUNCTION_FACE || nextPage == FUNCTION_BODY || nextPage == FUNCTION_EYE)
	{
		CommSendFlag = 1;
		Comm.onoff = RF_ON;
		BitAppCon.WorkFlag = 1;
		dwDisButton(&btnStart, 1, btnStart.xs, btnStart.ys);
		dwDisButton(&btnPause_O, 1, btnPause_O.xs, btnPause_O.ys);
	}
}

/*Pause button*/
static void PausePres(void)
{
	INLINE_MUSIC_STOP();
	if (nextPage == FUNCTION_BIO1)
	{
		BitAppCon.WorkFlag = 0;
		BitAppCon.WorkSecFlag = 0;
		dwDisButton(&btnStart_O, 1, btnStart_O.xs, btnStart_O.ys);
		dwDisButton(&btnPause, 1, btnPause.xs, btnPause.ys);
	}
	else if (nextPage == FUNCTION_O2 || nextPage == FUNCTION_O2_BIG)
	{
		BitAppCon.WorkFlag = 0;
		BitAppCon.WorkSecFlag = 0;
		dwDisButton(&btnStart_O, 1, btnStart_O.xs, btnStart_O.ys);
		dwDisButton(&btnPause, 1, btnPause.xs, btnPause.ys);
	}
	else if (nextPage == FUNCTION_FACE || nextPage == FUNCTION_BODY || nextPage == FUNCTION_EYE)
	{
		Comm.onoff = RF_OFF;
		BitAppCon.WorkFlag = 0;
		BitAppCon.WorkSecFlag = 0;
		CommSendFlag = 1;
		dwDisButton(&btnStart_O, 1, btnStart_O.xs, btnStart_O.ys);
		dwDisButton(&btnPause, 1, btnPause.xs, btnPause.ys);
	}
}

/*Time button*/
static void TimeUpPres(void)
{
	INLINE_MUSIC_BUTTON();
	if (WorkTime < 3600)
	{
		WorkTime += 60;
	}
	funDisTime(WorkTime, TimeX, TimeY);
}

static void TimeDownPres(void)
{
	INLINE_MUSIC_BUTTON();
	if (WorkTime > 60)
	{
		WorkTime -= 60;
	}
	funDisTime(WorkTime, TimeX, TimeY);
}

/*Intensity button*/
static void IntensityUpPres(void)
{
	INLINE_MUSIC_BUTTON();
	if (nextPage == FUNCTION_O2 || nextPage == FUNCTION_O2_BIG)
	{
		if (WorkIntensity < 8)
		{
			WorkIntensity++;
		}
	}
	else if (nextPage == FUNCTION_BIO1)
	{
		if (WorkIntensity < 15)
		{
			WorkIntensity++;
		}
	}
	else
	{
		if (WorkIntensity < 15)
		{
			WorkIntensity++;
			if (WorkIntensity <= 10)
				Comm.intensity = WorkIntensity;
			else
				Comm.intensity = WorkIntensity * 2 - 10;
			CommSendFlag = 1;
		}
	}
	DisplayIntensity(WorkIntensity, IntensityX, IntensityY);
}

static void IntensityDownPres(void)
{
	INLINE_MUSIC_BUTTON();
	if (nextPage == FUNCTION_O2 || nextPage == FUNCTION_O2_BIG)
	{
		if (WorkIntensity > 1)
			WorkIntensity--;
	}
	else if (nextPage == FUNCTION_BIO1)
	{
		if (WorkIntensity > 1)
		{
			WorkIntensity--;
		}
	}
	else
	{
		if (WorkIntensity > 1)
		{
			WorkIntensity--;
			if (WorkIntensity <= 10)
				Comm.intensity = WorkIntensity;
			else
				Comm.intensity = WorkIntensity * 2 - 10;
			CommSendFlag = 1;
		}
	}
	DisplayIntensity(WorkIntensity, IntensityX, IntensityY);
}

/*SUCK RELEASE TIME BUTTON*/
static void SuckTimeUpPres(void)
{
	INLINE_MUSIC_BUTTON();
	if (WorkSuckTime < 10)
		WorkSuckTime+=1;
	DisplayO2Time(WorkSuckTime, SuckX, SuckY);
}

static void SuckTimeDownPres(void)
{
	INLINE_MUSIC_BUTTON();
	if (WorkSuckTime > 0)
		WorkSuckTime-=1;
	DisplayO2Time(WorkSuckTime, SuckX, SuckY);
}

static void ReleaseTimeUpPres(void)
{
	INLINE_MUSIC_BUTTON();
	if (WorkReleaseTime < 10)
		WorkReleaseTime+=1;
	DisplayO2Time(WorkReleaseTime, ReleaseX, ReleaseY);
}

static void ReleaseTimeDownPres(void)
{
	INLINE_MUSIC_BUTTON();
	if (WorkReleaseTime > 0)
		WorkReleaseTime-=1;
	DisplayO2Time(WorkReleaseTime, ReleaseX, ReleaseY);
}

/*mode button*/
static void ModeUpPres(void)
{
	INLINE_MUSIC_BUTTON();

	if (WorkMode < 4)
	{
		WorkMode++;
	}
	DisplayMode(WorkMode, ModeX, ModeY);
}

static void ModeDownPres(void)
{
	INLINE_MUSIC_BUTTON();

	if (WorkMode > 1)
	{
		WorkMode--;
	}
	DisplayMode(WorkMode, ModeX, ModeY);
}

/*Other button*/
static void EnterPres(void)
{
	BitAppCon.menuExit = 1;
	nextPage = FUNCTION_SELECT;
	INLINE_MUSIC_BUTTON();
}

static void mutePres(void)
{
	u16 i = 0;
	INLINE_MUSIC_BUTTON();
	muteFlag = !muteFlag;
	i = muteFlag;
	STMFLASH_Write(FLASH_SAVE_ADDR+2, &i, 1);
	if(muteFlag)
		dwDisButton(&btnMute_O, 0, btnMute_O.xs, btnMute_O.ys);
	else
		dwDisButton(&btnMute, 0, btnMute.xs, btnMute.ys);
}

/*uart1 send to main board*/
void MainBoardSend(void)
{
	if (CommSendFlag == 1)
	{
		CommSendFlag = 0;
		uasrt1SendByte(Comm.head);
		uasrt1SendByte(Comm.channel);
		uasrt1SendByte(Comm.intensity);
		uasrt1SendByte(Comm.onoff);
		uasrt1SendByte(Comm.end);
	}
}

void LoopSend(void)
{
	//UART1 times send datas to main board
	if (BitAppCon.ms200)
	{
		BitAppCon.ms200 = 0;
		CommSendFlag = 1;
		MainBoardSend();
	}
}

//Power up 
void PageRestartAnimation(void)
{		
	dwDisPicNoL(0);
	BitAppCon.menuExit = 0;

	dwCancelKey();
	dwListenButton(EnterPres, 0, ENTER_KEY);
	while (!BitAppCon.menuExit)
	{
		dwHandlerButton();
		delay_ms(10);
	}
	dwWaitRelease();
	dwHandlerButton();
}

#define  TYPE_ADMIN      0xAA  // 管理卡
#define  TYPE_FOREVER    0xA1  // 永久卡
#define  TYPE_USER       0x55  // 用户卡即次数卡
#define  ADD_MODEL       10    // MODEL_F1
#define  ADD_TYPE        11    // TYPE_ADMIN, TYPE_USER, TYPE_FOREVER
#define  ADD_NUMS        12    // 00-99 工作次数
#define  ADD_TIME        13    // 00-99 工作时间

static u8 PIN[] = {0, 0, 0, 0, 0, 0};
static const u8 PINset[] = {3, 3, 4, 4, 1, 1};
static u8 PINcnt = 0;
static void B1Pres(void)
{
	PIN[PINcnt] = 1;
	PINcnt++;
}
static void B2Pres(void)
{
	PIN[PINcnt] = 2;
	PINcnt++;
}
static void B3Pres(void)
{
	PIN[PINcnt] = 3;
	PINcnt++;
}
static void B4Pres(void)
{
	PIN[PINcnt] = 4;
	PINcnt++;
}

void PageBurn(void);
void PageCard(void)
{
	u8 cardFlag = 0;
	u8 cardModel = 0;
	u8 cardType = 0;
	u8 cardNums = 0;
	u8 cardTime = 0;
	u8 i = 0;

	dwDisPicWithL(PIC_INSERT_CARD);
	dwCancelKey();
	dwListenButton(B1Pres, 0, B1_KEY);
	dwListenButton(B2Pres, 0, B2_KEY);
	dwListenButton(B3Pres, 0, B3_KEY);
	dwListenButton(B4Pres, 0, B4_KEY);
	while (1)
	{
		if((SW_PIN == 0) && (cardFlag == 0)){      // 插卡
			delay_ms(50); 
			if((SW_PIN == 0) && (cardFlag == 0)){  // 插卡
				cardFlag = 1;
				cardModel = AT24CXX_ReadOneByte((u16)ADD_MODEL);
				cardType  = AT24CXX_ReadOneByte((u16)ADD_TYPE);
				cardNums  = AT24CXX_ReadOneByte((u16)ADD_NUMS);
				cardTime  = AT24CXX_ReadOneByte((u16)ADD_TIME);
	//				uasrt1SendByte(cardModel);
	//				uasrt1SendByte(cardType);
	//				uasrt1SendByte(cardNums);
	//				uasrt1SendByte(cardTime);
				if(cardModel == MODEL_SELF){
					if(cardType == TYPE_USER){
						INLINE_MUSIC_BUTTON();
						WorkTime = cardTime*60;
						break;
					}else if(cardType == TYPE_FOREVER){
						INLINE_MUSIC_BUTTON();
						WorkTime = cardTime*60;
						cardNums = 99;
						break;
					}else{
						INLINE_MUSIC_STOP();	//坏卡
					}
				}else{
					INLINE_MUSIC_STOP();	//白卡或坏卡,不对机型
				}
			}
		}
		if((SW_PIN == 1) && (cardFlag != 0)){      // 拔卡
			delay_ms(50);
			if((SW_PIN == 1) && (cardFlag != 0)){  // 确定拔卡
				cardFlag = 0;
				dwDisPicWithL(PIC_INSERT_CARD);
				INLINE_MUSIC_BUTTON();
			}
		}
		if(PINcnt>=6)
		{
			PINcnt = 0;
			for (i = 0; i < 6; i++)
			{
				if(PINset[i]!=PIN[i])
					break;
			}
			if(i>=6)
				PageBurn();
		}
		dwHandlerButton();
		LoopSend();
	}

	dwDisPicNoL(1);
	dwDisNum(DW_SIZE_32, 168, 26, cardNums/10); //次数
	dwDisNum(DW_SIZE_32, 168 + 16, 26, cardNums%10);
	dwDisNum(DW_SIZE_32, 606, 26, WorkTime / 600); //时间
	dwDisNum(DW_SIZE_32, 606 + 16, 26, WorkTime / 60 % 10);
	dwDisChar(DW_SIZE_32, 606 + 32, 26, ':');
	dwDisNum(DW_SIZE_32, 606 + 48, 26, WorkTime % 60 / 10);
	dwDisNum(DW_SIZE_32, 606 + 64, 26, WorkTime % 10);
	BitAppCon.menuExit = 0;
	dwCancelKey();
	dwListenButton(EnterPres, 0, ENTER_KEY);

	while (!BitAppCon.menuExit)
	{
		dwHandlerButton();
		delay_ms(10);
		LoopSend();
	}

	if(AT24CXX_ReadOneByte((u16)ADD_MODEL)==MODEL_SELF)
	{ 
		if(cardType != TYPE_FOREVER)
		{
			if(cardNums==0)
			{		
				nextPage = FUNCTION_CARD;	//没次数
			}
			else
			{
				cardNums--;
				AT24CXX_WriteOneByte((u16)ADD_NUMS, cardNums);	
			}
		}
	}
	else
	{
		nextPage = FUNCTION_CARD;
	}	
	
	dwWaitRelease();
	dwHandlerButton();
}

void PageLanguage(void)
{
	BitAppCon.menuExit = 0;
	dwDisPicWithL(PIC_LANGUAGE);

	//display version
	dwDisChar(DW_SIZE_12, 2, 585, 'V');
	dwDisChar(DW_SIZE_12, 8, 585, '2');
	dwDisChar(DW_SIZE_12, 14, 585, '.');
	dwDisChar(DW_SIZE_12, 20, 585, '3');

	if(muteFlag)
		dwDisButton(&btnMute_O, 0, btnMute_O.xs, btnMute_O.ys);
	else
		dwDisButton(&btnMute, 0, btnMute.xs, btnMute.ys);

	dwCancelKey();
	dwListenButton(menuChinsePres, 0, CHINESE_KEY);
	dwListenButton(menuEnglishPres, 0, ENGLISH_KEY);
	dwListenButton(mutePres, 0, MUTE_KEY);
	while (!BitAppCon.menuExit)
	{
		dwHandlerButton();
		LoopSend();
	}
	dwWaitRelease();
	dwHandlerButton();
}

void PageMenu(void)
{
	BitAppCon.menuExit = 0;
	dwDisPicWithL(PIC_MENUS);

	dwCancelKey();
	dwListenButton(menuEyeRFPres, 0, EYE_KEY);
	dwListenButton(menuBodyRFPres, 0, BODY_KEY);
	dwListenButton(menuFaceRFPres, 0, FACE_KEY);
	dwListenButton(menuO2Pres, 0, SUCK_MIDDLE_KEY);
	dwListenButton(menuO2BigPres, 0, SUCK_BIG_KEY);
	dwListenButton(menuBIO1Pres, 0, BIO1_KEY);
	dwListenButton(SetPres, 0, SET_KEY);

	while (!BitAppCon.menuExit)
	{
		dwHandlerButton();

		if (nextPage != FUNCTION_SELECT)
		{
			BitAppCon.menuExit = 1;
		}
		LoopSend();
	}
	dwWaitRelease();
	dwHandlerButton();
}

void WorkTimeDeal(void)
{
	if (BitAppCon.WorkSecFlag)
	{
		if (BitAppCon.Flag1s)
		{
			BitAppCon.Flag1s = 0;
			if (WorkTime > 0)
			{
				WorkTime--;
			}
			else
			{
				INLINE_MUSIC_STOP();
				WorkTime = 1800;
				PausePres();
			}
			funDisTime(WorkTime, TimeX, TimeY);
		}
	}
}

void PageEyeRF(void)
{
	CommSendFlag = 0;
	WorkTime = 1800;
	WorkIntensity = 1;
	Comm.channel = RF_FACE;
	Comm.intensity = WorkIntensity;
	Comm.onoff = RF_OFF;
	BitAppCon.menuExit = 0;
	TimeX = 180; 
	TimeY = 345;
	IntensityX = 545;
	IntensityY = 345;

	dwDisPicWithL(PIC_EYE_RF);
	dwDisButton(&btnPause, 1, btnPause.xs, btnPause.ys);
	funDisTime(WorkTime, TimeX, TimeY);
	DisplayIntensity(WorkIntensity, IntensityX, IntensityY);

	dwCancelKey();
	dwListenButton(StartPres, 0, START_KEY);
	dwListenButton(PausePres, 0, STOP_KEY);
	dwListenButton(BackPres, 0, BACK_KEY);
	dwListenButton(TimeUpPres, 0, TIME_UP_KEY);
	dwListenButton(TimeDownPres, 0, TIME_DOWN_KEY);
	dwListenButton(IntensityUpPres, 0, INT_UP_KEY);
	dwListenButton(IntensityDownPres, 0, INT_DOWN_KEY);

	while (!BitAppCon.menuExit)
	{
		dwHandlerButton();

		if (nextPage != FUNCTION_EYE)
		{
			CommSendFlag = 1;
			BitAppCon.menuExit = 1;
			Comm.onoff = RF_OFF;
			BitAppCon.WorkFlag = 0;
			BitAppCon.WorkSecFlag = 0;
		}

		WorkTimeDeal();
		LoopSend();

		//current is over high warning, and pauses
		if (BitAppCon.rx1Flag)
		{
			BitAppCon.rx1Flag = 0;
			if (buff1[0] == 0xEE)
			{
				INLINE_MUSIC_ERROR();
				dwDisButton(&btnStart_O, 1, btnStart_O.xs, btnStart_O.ys);
				dwDisButton(&btnPause, 1, btnPause.xs, btnPause.ys);
				Comm.onoff = RF_OFF;
				BitAppCon.WorkFlag = 0;
				BitAppCon.WorkSecFlag = 0;
				CommSendFlag = 1;
			}
			else if((buff1[0] == 0xA0)&&BitAppCon.WorkFlag) //PRESS
			{
				BitAppCon.WorkSecFlag = 1;
				INLINE_MUSIC_BUTTON();
			}
			else if((buff1[0] == 0xA1)&&BitAppCon.WorkFlag) //RELEASE
			{
				if(BitAppCon.WorkSecFlag)
				{
					BitAppCon.WorkSecFlag = 0;
					INLINE_MUSIC_BUTTON();
				}
			}
		}
	}
	dwWaitRelease();
	dwHandlerButton();
}

void PageFaceRF(void)
{
	CommSendFlag = 0;
	WorkTime = 1800;
	WorkIntensity = 1;
	Comm.channel = RF_FACE;
	Comm.intensity = WorkIntensity;
	Comm.onoff = RF_OFF;
	BitAppCon.menuExit = 0;
	TimeX = 180; 
	TimeY = 345;
	IntensityX = 545;
	IntensityY = 345;

	dwDisPicWithL(PIC_FACE_RF);
	dwDisButton(&btnPause, 1, btnPause.xs, btnPause.ys);
	funDisTime(WorkTime, TimeX, TimeY);
	DisplayIntensity(WorkIntensity, IntensityX, IntensityY);

	dwCancelKey();
	dwListenButton(StartPres, 0, START_KEY);
	dwListenButton(PausePres, 0, STOP_KEY);
	dwListenButton(BackPres, 0, BACK_KEY);
	dwListenButton(TimeUpPres, 0, TIME_UP_KEY);
	dwListenButton(TimeDownPres, 0, TIME_DOWN_KEY);
	dwListenButton(IntensityUpPres, 0, INT_UP_KEY);
	dwListenButton(IntensityDownPres, 0, INT_DOWN_KEY);

	while (!BitAppCon.menuExit)
	{
		dwHandlerButton();

		if (nextPage != FUNCTION_FACE)
		{
			CommSendFlag = 1;
			BitAppCon.menuExit = 1;
			Comm.onoff = RF_OFF;
			BitAppCon.WorkFlag = 0;
			BitAppCon.WorkSecFlag = 0;
		}

		WorkTimeDeal();
		LoopSend();

		//current is over high warning, and pauses
		if (BitAppCon.rx1Flag)
		{
			BitAppCon.rx1Flag = 0;
			if (buff1[0] == 0xEE)
			{
				INLINE_MUSIC_ERROR();
				dwDisButton(&btnStart_O, 1, btnStart_O.xs, btnStart_O.ys);
				dwDisButton(&btnPause, 1, btnPause.xs, btnPause.ys);
				Comm.onoff = RF_OFF;
				BitAppCon.WorkFlag = 0;
				BitAppCon.WorkSecFlag = 0;
				CommSendFlag = 1;
			}
			else if((buff1[0] == 0xA0)&&BitAppCon.WorkFlag) //PRESS
			{
				BitAppCon.WorkSecFlag = 1;
				INLINE_MUSIC_BUTTON();
			}
			else if((buff1[0] == 0xA1)&&BitAppCon.WorkFlag) //RELEASE
			{
				if(BitAppCon.WorkSecFlag)
				{
					BitAppCon.WorkSecFlag = 0;
					INLINE_MUSIC_BUTTON();
				}
			}
		}
	}
	dwWaitRelease();
	dwHandlerButton();
}

void PageBodyRF(void)
{
	CommSendFlag = 0;
	WorkTime = 1800;
	WorkIntensity = 1;
	Comm.channel = RF_BODY;
	Comm.intensity = WorkIntensity;
	Comm.onoff = RF_OFF;
	BitAppCon.menuExit = 0;
	TimeX = 180; 
	TimeY = 345;
	IntensityX = 545;
	IntensityY = 345;

	dwDisPicWithL(PIC_BODY_RF);
	dwDisButton(&btnPause, 1, btnPause.xs, btnPause.ys);
	funDisTime(WorkTime, TimeX, TimeY);
	DisplayIntensity(WorkIntensity, IntensityX, IntensityY);

	dwCancelKey();
	dwListenButton(StartPres, 0, START_KEY);
	dwListenButton(PausePres, 0, STOP_KEY);
	dwListenButton(BackPres, 0, BACK_KEY);
	dwListenButton(TimeUpPres, 0, TIME_UP_KEY);
	dwListenButton(TimeDownPres, 0, TIME_DOWN_KEY);
	dwListenButton(IntensityUpPres, 0, INT_UP_KEY);
	dwListenButton(IntensityDownPres, 0, INT_DOWN_KEY);
	
	while (!BitAppCon.menuExit)
	{
		dwHandlerButton();

		if (nextPage != FUNCTION_BODY)
		{
			CommSendFlag = 1;
			BitAppCon.menuExit = 1;
			Comm.onoff = RF_OFF;
			BitAppCon.WorkFlag = 0;
			BitAppCon.WorkSecFlag = 0;
		}

		WorkTimeDeal();
		LoopSend();

		//current is over high warning, and pauses
		if (BitAppCon.rx1Flag)
		{
			BitAppCon.rx1Flag = 0;
			if (buff1[0] == 0xEE)
			{
				INLINE_MUSIC_ERROR();
				dwDisButton(&btnStart_O, 1, btnStart_O.xs, btnStart_O.ys);
				dwDisButton(&btnPause, 1, btnPause.xs, btnPause.ys);
				Comm.onoff = RF_OFF;
				BitAppCon.WorkFlag = 0;
				BitAppCon.WorkSecFlag = 0;
				CommSendFlag = 1;
			}
			else if((buff1[0] == 0xA0)&&BitAppCon.WorkFlag) //PRESS
			{
				BitAppCon.WorkSecFlag = 1;
				INLINE_MUSIC_BUTTON();
			}
			else if((buff1[0] == 0xA1)&&BitAppCon.WorkFlag) //RELEASE
			{
				if(BitAppCon.WorkSecFlag)
				{
					BitAppCon.WorkSecFlag = 0;
					INLINE_MUSIC_BUTTON();
				}
			}
		}
	}
	dwWaitRelease();
	dwHandlerButton();
}

void PageBIO1(void)
{
	// WorkTime = 1800;
	WorkIntensity = 1;
	WorkMode = 1;
	BitAppCon.menuExit = 0;
	TimeX = 392; 
	TimeY = 252;
	IntensityX = 600;
	IntensityY = 354;
	ModeX = 290;
	ModeY = 354;

	dwDisPicWithL(PIC_BIO1);
	dwDisButton(&btnPause, 1, btnPause.xs, btnPause.ys);
	funDisTime(WorkTime, TimeX, TimeY);
	DisplayIntensity(WorkIntensity, IntensityX, IntensityY);
	DisplayMode(WorkMode, ModeX, ModeY);

	dwCancelKey();
	dwListenButton(StartPres, 0, START_KEY);
	dwListenButton(PausePres, 0, STOP_KEY);
	dwListenButton(BackPres, 0, BACK_KEY);
	dwListenButton(TimeUpPres, 0, TIME_UP_KEY);
	dwListenButton(TimeDownPres, 0, TIME_DOWN_KEY);
	dwListenButton(IntensityUpPres, 0, INT_UP_KEY);
	dwListenButton(IntensityDownPres, 0, INT_DOWN_KEY);
	dwListenButton(ModeUpPres, 0, MODE_UP_KEY);
	dwListenButton(ModeDownPres, 0, MODE_DOWN_KEY);

	while (!BitAppCon.menuExit)
	{
		dwHandlerButton();
		if (nextPage != FUNCTION_BIO1)
		{
			BitAppCon.menuExit = 1;
			BitAppCon.WorkFlag = 0;
			BitAppCon.WorkSecFlag = 0;
		}
		WorkTimeDeal();
		if (BitAppCon.WorkFlag)
		{
			BACK1_PIN = 1; //here contral LED
		}
		else
		{
			BACK1_PIN = 0; //here contral LED
		}

		LoopSend();
	}
	dwWaitRelease();
	dwHandlerButton();
}

//Temperature Process
const u16 TemperatureTable[] = {4095, 1396, 1296, 1196, 1096,
								996, 796, 596, 498};
u16 IceTemperature = 0;
static u8 NtcErrorFlag = 0;
static void TemperatureProcess(void)
{
	static u16 i = 0;
	
	//NTC ERROR
	if(IceTemperature>=4000||IceTemperature<=100)
	{
		if(++i>=10000)
		{
			i = 10000;
			if(NtcErrorFlag==0)
			{
				NtcErrorFlag = 1;
				HEAT_PIN = 0;
				INLINE_MUSIC_ERROR();
			}			
		}
	}
	else
	{
		if (i>=25)
			i -= 25;
		else
			i = 0;
		if(i==0)
			NtcErrorFlag = 0;

		if(IceTemperature>=TemperatureTable[WorkIntensity]+16)
			HEAT_PIN = 1;
		if(IceTemperature<=TemperatureTable[WorkIntensity])
			HEAT_PIN = 0;
	}
}

void PageO2(void)
{
	BACK1_PIN = 0; //change channel
	WorkTime = 1800;
	WorkIntensity = 1;
	WorkMode = 1;
	BitAppCon.menuExit = 0;
	TimeX = 339; 
	TimeY = 247;
	IntensityX = 540;
	IntensityY = 365;
	ModeX = 225;
	ModeY = 365;

	dwDisPicWithL(PIC_O2);
	dwDisButton(&btnPause, 1, btnPause.xs, btnPause.ys);
	funDisTime(WorkTime, TimeX, TimeY);
	DisplayIntensity(WorkIntensity, IntensityX, IntensityY);
	DisplayMode(WorkMode, ModeX, ModeY);

	dwCancelKey();
	dwListenButton(StartPres, 0, START_KEY);
	dwListenButton(PausePres, 0, STOP_KEY);
	dwListenButton(BackPres, 0, BACK_KEY);
	dwListenButton(TimeUpPres, 0, TIME_UP_KEY);
	dwListenButton(TimeDownPres, 0, TIME_DOWN_KEY);
	dwListenButton(IntensityUpPres, 0, INT_UP_KEY);
	dwListenButton(IntensityDownPres, 0, INT_DOWN_KEY);
	dwListenButton(ModeUpPres, 0, MODE_UP_KEY);
	dwListenButton(ModeDownPres, 0, MODE_DOWN_KEY);

	while (!BitAppCon.menuExit)
	{
		dwHandlerButton();
		if (nextPage != FUNCTION_O2)
		{
			BitAppCon.menuExit = 1;
			BitAppCon.WorkFlag = 0;
			BitAppCon.WorkSecFlag = 0;
		}
		WorkTimeDeal();
		if (BitAppCon.WorkFlag)
		{
			PUMP_PIN = 1;
		}
		else
		{
			PUMP_PIN = 0;
			NtcErrorFlag = 0;
		}

		LoopSend();
	}
	BACK1_PIN = 0;
	dwWaitRelease();
	dwHandlerButton();
}

void PageO2Big(void)
{
	BACK1_PIN = 1; //change channel
	WorkTime = 1800;
	WorkIntensity = 1;
	WorkMode = 1;
	BitAppCon.menuExit = 0;
	TimeX = 339; 
	TimeY = 247;
	IntensityX = 540;
	IntensityY = 365;
	ModeX = 225;
	ModeY = 365;

	dwDisPicWithL(PIC_O2_BIG);
	dwDisButton(&btnPause, 1, btnPause.xs, btnPause.ys);
	funDisTime(WorkTime, TimeX, TimeY);
	DisplayIntensity(WorkIntensity, IntensityX, IntensityY);
	DisplayMode(WorkMode, ModeX, ModeY);

	dwCancelKey();
	dwListenButton(StartPres, 0, START_KEY);
	dwListenButton(PausePres, 0, STOP_KEY);
	dwListenButton(BackPres, 0, BACK_KEY);
	dwListenButton(TimeUpPres, 0, TIME_UP_KEY);
	dwListenButton(TimeDownPres, 0, TIME_DOWN_KEY);
	dwListenButton(IntensityUpPres, 0, INT_UP_KEY);
	dwListenButton(IntensityDownPres, 0, INT_DOWN_KEY);
	dwListenButton(ModeUpPres, 0, MODE_UP_KEY);
	dwListenButton(ModeDownPres, 0, MODE_DOWN_KEY);

	while (!BitAppCon.menuExit)
	{
		dwHandlerButton();
		if (nextPage != FUNCTION_O2_BIG)
		{
			BitAppCon.menuExit = 1;
			BitAppCon.WorkFlag = 0;
			BitAppCon.WorkSecFlag = 0;
		}
		WorkTimeDeal();
		if (BitAppCon.WorkFlag)
		{
			PUMP_PIN = 1;
		}
		else
		{
			PUMP_PIN = 0;
			NtcErrorFlag = 0;
		}

		LoopSend();
	}
	BACK1_PIN = 0;
	dwWaitRelease();
	dwHandlerButton();
}

//O2 PWM
const u16 O2ModPeriod[] = {0, 2000, 4000, 4000, 1000, 5000};
const u16 O2ModCompare[] = {0, 1000, 2000, 3000, 1000, 250};

void O2PWM(u8 i, u8 Work)
{
	static u16 O2TimeCnt = 0;
	static u8 O2ModRenew = 0;

	if (Work)
	{
		O2ModRenew = i;
		if (++O2TimeCnt >= O2ModPeriod[O2ModRenew])
		{
			O2TimeCnt = 0;
		}
		if ((O2TimeCnt < O2ModCompare[O2ModRenew]))
			Valve_PIN = 0;
		else
			Valve_PIN = 1;
	}
	else
	{
		O2TimeCnt = 0;
		Valve_PIN = 0;
	}
}

/* void O2PWM(u16 suck, u16 release, u8 Work)
{
	static u16 O2TimeCnt = 0;

	if (Work)
	{
		if (++O2TimeCnt >= suck+release)
		{
			O2TimeCnt = 0;
		}
		if (O2TimeCnt < suck)
			Valve_PIN = 0;
		else
			Valve_PIN = 1;
	}
	else
	{
		O2TimeCnt = 0;
		Valve_PIN = 0;
	}
} */

const u16 HeatIntensityTable[] = {0, 1, 2, 3, 4,
								  5, 5, 6, 6};
void HeatPWM(u8 i, u8 Work)
{
	static u16 HeatTimeCnt = 0;
	static u8 HeatModRenew = 0;

	if (Work)
	{
		if (++HeatTimeCnt >= 8)
		{
			HeatTimeCnt = 0;
			HeatModRenew = i;
		}
		if ((HeatTimeCnt < HeatIntensityTable[HeatModRenew]))
			HEAT_PIN = 1; //ON
		else
			HEAT_PIN = 0; //OFF
	}
	else
	{
		HeatTimeCnt = 0;
		HEAT_PIN = 0; //ON
	}
}

void Heat2PWM(u8 i, u8 Work)
{
	static u16 HeatTimeCnt = 0;
	static u8 HeatModRenew = 0;

	if (Work)
	{
		if (++HeatTimeCnt >= 8)
		{
			HeatTimeCnt = 0;
			HeatModRenew = i;
		}
		if ((HeatTimeCnt < HeatIntensityTable[HeatModRenew]))
			HEAT2_PIN = 1; //ON
		else
			HEAT2_PIN = 0; //OFF
	}
	else
	{
		HeatTimeCnt = 0;
		HEAT2_PIN = 0; //ON
	}
}

//BIO1 PWM
//const u16 BIO1IntensityTable[] = {0, 26, 28, 30, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 44, 45, 46, 47, 47, 48, 48};
const u16 BIO1IntensityTable[] = {0, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 44, 45, 46, 46, 47, 47, 48, 48};
// const u16 BIO1ModPeriod[] = {0, 10000, 9900, 2640, 4950, 4950, 4950}; 
// const u16 BIO1ModCompare[] = {0, 10000, 4950, 1320, 2640, 2640, 4950};
const u16 BIO1ModPeriod[] = {1000, 500, 400, 300, 180, 330, 190}; 
const u16 BIO1ModCompare[] = {0, 10000, 4950, 1320, 2640, 2640, 4950};
void BIO1PWM(u8 i, u8 Work)
{
	static u16 BIO1TimeCnt = 0, BIO1TimeCnt2 = 0;
	static u8 BIO1ModRenew = 0;

	if (Work)
	{
		// if (++BIO1TimeCnt2 >= BIO1ModPeriod[BIO1ModRenew])
		// {
		// 	BIO1TimeCnt2 = 0;
		// 	BIO1ModRenew = i;
		// }
		// if (BIO1TimeCnt2 < BIO1ModCompare[BIO1ModRenew])
		// {
		// 	if (++BIO1TimeCnt >= 330) //BIO1ModPeriod[BIO1ModRenew])
		// 		BIO1TimeCnt = 0;
		// 	if (BIO1TimeCnt < 2) //BIO1ModCompare[BIO1ModRenew]))
		// 		BIOS_PIN = 0;	//open
		// 	else
		// 		BIOS_PIN = 1; //close
		// }
		// else
		// {
		// 	BIOS_PIN = 1; //close
		// 	BIO1TimeCnt = 0;
		// }
		if (++BIO1TimeCnt >= BIO1ModPeriod[BIO1ModRenew]) //BIO1ModPeriod[BIO1ModRenew])
		{
			BIO1TimeCnt = 0;
			BIO1ModRenew = i;
		}
		if (BIO1TimeCnt < 5)
			BIOS_PIN = 0;	 //open
		else
			BIOS_PIN = 1; //close
	}
	else
	{
		BIO1TimeCnt = 0;
		BIO1TimeCnt2 = 0;
		BIOS_PIN = 1; //close
	}
}

void BIO1Power(u8 i, u8 Work)
{
	static u16 BIO1TimeCnt = 0;
	static u8 BIO1ModRenew = 0;

	if (Work)
	{
		if (++BIO1TimeCnt >= 75)
		{
			BIO1TimeCnt = 0;
			BIO1ModRenew = i;
		}
		if ((BIO1TimeCnt < BIO1IntensityTable[BIO1ModRenew]))
			BIOA_PIN = 1;
		else
			BIOA_PIN = 0;
	}
	else
	{
		BIO1TimeCnt = 0;
		BIOA_PIN = 0;
	}
}

//Rec and process frame
u8 buff1[5];
void commRx1Handler(u8 byte)
{
	static u8 count = 0;

	if (count == 0)
	{
		if (byte == 0xAA)
		{
			count++;
		}
	}
	else if (count < 2)
	{
		buff1[count - 1] = byte;
		count++;
	}
	else
	{
		count = 0;
		if (byte == 0x55)
		{
			BitAppCon.rx1Flag = 1;
		}
	}
}

/*485*/
void Send485(u8 *i)
{
	u8 verify = 0XAA+0X01+0XC0+i[0]+i[1];

	RDorDE_PIN = 1;
	uasrt3SendByte(0XAA);
	uasrt3SendByte(0X01);
	uasrt3SendByte(0XC0);
	uasrt3SendByte(i[0]);
	uasrt3SendByte(i[1]);
	uasrt3SendByte(verify);
	RDorDE_PIN = 0;
}

u8 data485[4] = {0};
void Rec485(u8 i)
{
	static u8 cnt = 0;
	u8 tt = 0;

	switch (cnt)
	{
	case 0:
	{
		if (i == 0XAA)
			cnt++;
		break;
	}
	case 1:
	{
		if (i == 0X01)
			cnt++;
		else
			cnt = 0;
		break;
	}
	case 2:
	{
		cnt++;
		if (i == 0XA0)
			data485[1] = i;
		else if (i == 0XA1)
			data485[1] = i;
		else
			cnt = 0;
		break;
	}
	case 3:
	{
		data485[2] = i;
		cnt++;
		break;
	}
	case 4:
	{
		data485[3] = i;
		cnt++;
		break;
	}
	case 5:
	{
		tt = 0XAA + 0X01 + (u16)data485[1] + data485[2] + data485[3];
		if (tt == i)
			data485[0] = 1; //success flag
		else
			data485[0] = 0;
		cnt = 0;
		break;
	}
	default:
		cnt = 0;
		break;
	}
}

/*Bee function*/
const u16 BeeModPeriod[] = {110, 410, 1000, 1010, 3010};
const u16 BeeModCompare[] = {100, 400, 500, 1000, 3000};
u8 BeeMod = 0, BeeTime = 0;
void BeeFunction(void)
{
	static u16 BeeTimeCnt = 0;

	if (BeeTime > 0)
	{
		if (++BeeTimeCnt >= BeeModPeriod[BeeMod])
		{
			BeeTimeCnt = 0;
			BeeTime--;
		}
		else
		{
			if (BeeTimeCnt >= BeeModCompare[BeeMod])
				SOUND_PIN = 0;
			else
				SOUND_PIN = 1;
		}
	}
	else
	{
		BeeTimeCnt = 0;
		SOUND_PIN = 0;
	}
}
////////////////////////////////////////
