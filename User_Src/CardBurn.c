#include "AppLib.h"
#include "delay.h"
#include "24cxx.h" 

#define  TYPE_ADMIN      0xAA  // 管理卡
#define  TYPE_FOREVER      0xA1  // 永久卡
#define  TYPE_USER       0x55  // 用户卡即次数卡

#define  ADD_MODEL       10    // MODEL_F1
#define  ADD_TYPE        11    // TYPE_ADMIN, TYPE_USER
#define  ADD_NUMS        12    // 00-99 工作次数
#define  ADD_TIME        13    // 00-99 工作时间

#define CARD_TIME_UP 0X00C1
#define CARD_TIME_DOWN 0X00C2
#define CARD_COUNT_UP 0X00C3
#define CARD_COUNT_DOWN 0X00C4
#define CARD_MAN 0X00C5
#define CARD_FOREVER 0X00C6
#define CARD_USER 0X00C7

static u8 ICnums = 50, ICtime = 40, Model = MODEL_SELF;

/*显示多位数值*/
static void __dwDisDig(u16 x, u16 y, u16 number)
{
	u8 c = 0;
	u8 temp[2]= {0,0};
	
	c = number%100/10;	
	if(c < 10){		
		temp[0] = c + 0x30;
		dwDisString(DW_SIZE_48,x,y,temp);
	}	
	
	c = number%10;	
	if(c < 10){
		temp[0] = c + 0x30;
		dwDisString(DW_SIZE_48,x+24,y,temp);
	}	
}

static void ManagerCardFree(void)
{
	u8 temp = 0, error = 0;

	AT24CXX_WriteOneByte((u16)ADD_MODEL, Model);
	delay_ms(5);
	AT24CXX_WriteOneByte((u16)ADD_TYPE, TYPE_ADMIN);
	delay_ms(5);
	INLINE_MUSIC_BUTTON();
	temp = AT24CXX_ReadOneByte((u16)ADD_MODEL);
	if(temp != Model){
		error++;
	}
	temp = AT24CXX_ReadOneByte((u16)ADD_TYPE);
	if(temp != TYPE_ADMIN){
		error++;
	}
	if(error == 0){   // 校验正确
		dwDisString(DW_SIZE_48, 314, 531,"OK ");
	}else{           // 校验错误
		delay_ms(200);
		dwDisString(DW_SIZE_48, 314, 531,"ERR");
	}
	dwDisString(DW_SIZE_48,470, 467,"   ");
	dwDisString(DW_SIZE_48,470, 531,"   ");
}

static void UserCardFree(void)
{
	u8 temp = 0, error = 0;

	AT24CXX_WriteOneByte((u16)ADD_MODEL, Model);
	delay_ms(5);
	AT24CXX_WriteOneByte((u16)ADD_TYPE, TYPE_USER);
	delay_ms(5);
	AT24CXX_WriteOneByte((u16)ADD_NUMS, ICnums);
	delay_ms(5);
	AT24CXX_WriteOneByte((u16)ADD_TIME, ICtime);
	delay_ms(5);
	INLINE_MUSIC_BUTTON();
	temp = AT24CXX_ReadOneByte((u16)ADD_MODEL);
	if(temp != Model){
		error++;
	}
	temp = AT24CXX_ReadOneByte((u16)ADD_TYPE);
	if(temp != TYPE_USER){
		error++;
	} 
	temp = AT24CXX_ReadOneByte((u16)ADD_NUMS);
	if(temp != ICnums){
		error++;
	}
	temp = AT24CXX_ReadOneByte((u16)ADD_TIME);
	if(temp != ICtime){
		error++;
	}
	if(error == 0){  // 校验正确
		dwDisString(DW_SIZE_48, 470, 531,"OK ");
	}else{           // 校验错误
		delay_ms(200);
		dwDisString(DW_SIZE_48, 470, 531,"ERR");
	}
	dwDisString(DW_SIZE_48, 470, 467,"   ");
	dwDisString(DW_SIZE_48,314, 531,"   ");
}

static void ForeverCardFree(void)
{
	u8 temp = 0, error = 0;

	AT24CXX_WriteOneByte((u16)ADD_MODEL, Model);
	delay_ms(5);
	AT24CXX_WriteOneByte((u16)ADD_TYPE, TYPE_FOREVER);
	delay_ms(5);
	AT24CXX_WriteOneByte((u16)ADD_NUMS, 99);
	delay_ms(5);
	AT24CXX_WriteOneByte((u16)ADD_TIME, ICtime);
	delay_ms(5);
	INLINE_MUSIC_BUTTON();
	temp = AT24CXX_ReadOneByte((u16)ADD_MODEL);
	if(temp != Model){
		error++;
	}
	temp = AT24CXX_ReadOneByte((u16)ADD_TYPE);
	if(temp != TYPE_FOREVER){
		error++;
	} 
	temp = AT24CXX_ReadOneByte((u16)ADD_NUMS);
	if(temp != 99){
		error++;
	}
	temp = AT24CXX_ReadOneByte((u16)ADD_TIME);
	if(temp != ICtime){
		error++;
	}
	if(error == 0){   // 校验正确
		dwDisString(DW_SIZE_48, 470, 467,"OK ");
	}else{           // 校验错误
		delay_ms(200);
		dwDisString(DW_SIZE_48, 470, 467,"ERR");
	}
	dwDisString(DW_SIZE_48, 314, 531,"   ");
	dwDisString(DW_SIZE_48,470, 531,"   ");
}

static void TimeUpFree_Manager(void)
{
	if(ICtime < 99){
		ICtime++;
	__dwDisDig(420, 337, ICtime);
	INLINE_MUSIC_BUTTON();
	}
}

static void TimeDownFree_Manager(void)
{
	if(ICtime > 1){
		ICtime--;
	__dwDisDig(420, 337, ICtime);
	INLINE_MUSIC_BUTTON();
	}
}

static void CountUpFree(void)
{
	if(ICnums < 99){
	ICnums++;
	__dwDisDig(420, 408, ICnums);
	INLINE_MUSIC_BUTTON();
	}
}

static void CountDownFree(void)
{
	if(ICnums > 1){
	ICnums--;
	__dwDisDig(420, 408, ICnums);
	INLINE_MUSIC_BUTTON();
	}
}

void PageBurn(void)
{
	u8 cardFlag = 0;
	
	dwSetColor(DW_COL_GREEN, DW_COL_WHITE);
	dwDisPicNoL(PIC_BURN);
	__dwDisDig(420, 337, ICtime);
	__dwDisDig(420, 408, ICnums);

	if(SW_PIN == 0)
		cardFlag = 1;
	
	dwCancelKey();	
	dwListenButton(0,  ManagerCardFree, CARD_MAN);
	dwListenButton(0, UserCardFree, CARD_USER);	
	dwListenButton(0, ForeverCardFree, CARD_FOREVER);	
	dwListenButton(0, TimeUpFree_Manager, CARD_TIME_UP);
	dwListenButton(0, TimeDownFree_Manager, CARD_TIME_DOWN);
	dwListenButton(0,  CountUpFree, CARD_COUNT_UP);
	dwListenButton(0,  CountDownFree, CARD_COUNT_DOWN);
	
	while(1)
    {				
		if((SW_PIN == 0) && (cardFlag == 0))
        {      // 插卡
			delay_ms(50);
			if((SW_PIN == 0) && (cardFlag == 0))
            {  // 插卡
				cardFlag = 1;
				INLINE_MUSIC_BUTTON();
				dwDisString(DW_SIZE_48, 314, 531,"   ");
				dwDisString(DW_SIZE_48,470, 467,"   ");
				dwDisString(DW_SIZE_48,470, 531,"   ");
			}
		}
		if((SW_PIN == 1) && (cardFlag != 0))
        {      // 拔卡
			delay_ms(50);
			if((SW_PIN == 1) && (cardFlag != 0))
            {  // 确定拔卡
				cardFlag = 0;
				INLINE_MUSIC_BUTTON();
			}
		}		
		dwHandlerButton();
	}
}
