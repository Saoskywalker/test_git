#ifndef _APP_LIB_H
#define _APP_LIB_H

#include "UserBaseLib.h"
#include "dwDriver.h"
#include "usart.h"
#include "adc.h"
#include "stmflash.h"

//Flash function
#define FLASH_SAVE_ADDR  0X0800F800 				//设置FLASH 保存地址(必须为偶数)

#define  MODEL_SELF      0x3C  // 本机型号码

//Hardware define
#define SOUND_PIN PAout(4)
#define HEAT2_PIN PBout(9)
#define COOL_ON_PIN PBout(4)
#define PUMP_PIN PBout(3)
#define BACK1_PIN PBout(1)
#define Valve_PIN PAout(6)
#define BIOS_PIN PBout(7)
#define BIOA_PIN PBout(6)
#define HEAT_PIN PBout(0)
#define RDorDE_PIN PBout(12)
#define BIO1or2_PIN PBout(5)
#define SW_PIN PBin(12)

//Picture Code
#define PIC_LANGUAGE 6
#define PIC_LANGUAGE_P 5
#define PIC_MENUS 11
#define PIC_MENUS_P 22
#define PIC_FACE_RF 12
#define PIC_FACE_RF_P 22
#define PIC_BODY_RF 14
#define PIC_BODY_RF_P 24
#define PIC_EYE_RF 17
#define PIC_O2 15
#define PIC_O2_BIG 16
#define PIC_BIO1 16
#define PIC_INSERT_CARD 18
#define PIC_BURN 80

extern u8 nextPage;
extern u8 requestPage;
extern const Button btnChinse;
extern const Button btnEnglish;
extern u8 WorkIntensity, WorkMode, muteFlag;
extern u16 WorkSuckTime, WorkReleaseTime;

//music code
#define	MSC_BUTTON  0
#define	MSC_START   2
#define	MSC_STOP    3
#define	MSC_ALERT   4
#define	MSC_ERROR   5

//key code
#define EYE_KEY 0X0015
#define FACE_KEY 0X0010
#define SUCK_MIDDLE_KEY 0X0011
#define BODY_KEY 0X0012
#define SUCK_BIG_KEY 0X0013
#define BIO1_KEY 0X0014
#define START_KEY 0X0021
#define STOP_KEY 0X0020
#define TIME_UP_KEY 0X0031
#define TIME_DOWN_KEY 0X0032
#define INT_UP_KEY 0X0033
#define INT_DOWN_KEY 0X0034
#define SUCK_UP_KEY 0X0035
#define SUCK_DOWN_KEY 0X0036
#define RELEASE_UP_KEY 0X0037
#define RELEASE_DOWN_KEY 0X0038
#define MODE_UP_KEY 0X0039
#define MODE_DOWN_KEY 0X003A
#define ENGLISH_KEY 0X0042
#define CHINESE_KEY 0X0041
#define ENTER_KEY 0X0043
#define SET_KEY 0X0044
#define BACK_KEY 0X0022
#define MUTE_KEY 0X0045
#define B1_KEY 0X00B1
#define B2_KEY 0X00B2
#define B3_KEY 0X00B3
#define B4_KEY 0X00B4

//FUNCTION NUM
#define FUNCTION_EXPLAIN 0
#define FUNCTION_FACE 1
#define FUNCTION_BODY 2
#define FUNCTION_O2 3
#define FUNCTION_O2_BIG 4
#define FUNCTION_SELECT 5
#define FUNCTION_LANGUAGE 6
#define FUNCTION_EYE 7
#define FUNCTION_RESTART 8
#define FUNCTION_BIO1 9
#define FUNCTION_CARD 10

//lll
typedef struct
{
  unsigned char menuExit     : 1;
  unsigned char rx1Flag      : 1;
  unsigned char Flag1s       : 1;
  unsigned char WorkFlag     : 1;
  unsigned char ms200        : 1;
  unsigned char WorkSecFlag  : 1;
  unsigned char ms1      	 : 1;
  unsigned char ms8       	 : 1;
}_BitAppCon;

extern _BitAppCon BitAppCon;

void PageLanguage(void);
void PageMenu(void);
void PageFaceRF(void);
void PageBodyRF(void);
void PageCool(void);
void PageO2(void);
void PageO2Big(void);
void PageBIO1(void);
void PageEyeRF(void);
void PageRestartAnimation(void);
void PageCard(void);

//Main board Communicate
#define RF_OFF 0x00
#define RF_ON 0x01
#define RF_BODY 0x02
#define RF_FACE 0x01
#define FRAME_HEAD    0x55
#define FRAME_END     0xAA
//#define CMD_TYPE	  0xCE
//#define CMD_STATUS	  0xC0
//#define CMD_LED  	  0xC1
//#define CMD_COUNTER	  0xC2
//#define CMD_INC 	  0xC3
//#define CMD_CLERA	  0xC4

typedef struct
{
	u8 head;
	u8 channel;
	u8 intensity;
	u8 onoff;
	u8 end;
}_Comm;
extern _Comm Comm;

extern u8 buff1[5];
void commRx1Handler(u8 byte);
void O2PWM(u8 i, u8 Work);
void HeatPWM(u8 i, u8 Work);
void Heat2PWM(u8 i, u8 Work);

/****BEE Module******/
extern u8 BeeMod, BeeTime;
void BeeFunction(void);

void BIO1PWM(u8 i, u8 Work);
void BIO1Power(u8 i, u8 Work);

/*485*/
extern u8 data485[4];
void Rec485(u8 i);
void Send485(u8 *i);

/* 
static __inline void dwPlayMusic(u8 id, u8 num)
{
	BeeMod = id;
	BeeTime = num;
}

static __inline void dwStopMusic(void)
{
	BeeTime = 0;
}
 */
#define INLINE_MUSIC_CANNEL()	{BeeTime = 0;}
#define INLINE_MUSIC_BUTTON() {BeeMod = 0; BeeTime = 1;}
#define INLINE_MUSIC_START() {BeeMod = 1; BeeTime = 1;}
#define INLINE_MUSIC_STOP() {BeeMod = 2; BeeTime = 3;}
#define INLINE_MUSIC_ERROR() {BeeMod = 0; BeeTime = 7;}
#define INLINE_MUSIC_SCR_ADJ() {BeeMod = 1; BeeTime = 2;}
//////////////////////////

/*****Display Intensity********/
static __inline void DisplayIntensity(u8 i, u16 x, u16 y)
{
	// dwCutPic(PIC_INTENSITY+i, 448, 143, 687, 384, 448, 143);
	dwDisNum(DW_SIZE_48, x, y, i / 10);
	dwDisNum(DW_SIZE_48, x + 24, y, i % 10);
}

/*****Display Mode********/
static __inline void DisplayMode(u8 i, u16 x, u16 y)
{
	dwDisChar(DW_SIZE_48, x, y, '@'+i);
}

/*****Display suck or release time********/
static __inline void DisplayO2Time(u16 j, u16 x, u16 y)
{
	dwDisNum(DW_SIZE_48, x, y, j / 10);
	dwDisNum(DW_SIZE_48, x + 24, y, j % 10);
}
///////////////////////////////

#endif
