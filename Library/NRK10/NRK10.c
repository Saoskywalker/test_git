#include "NRK10.h"
#include "usart.h"
#include "dwDriver.h"

u16 NRK10_Overtime_Cnt = NRK10_OVER_TIME;

const _NRK10_type NRK10_command = {
    {0XFA, 0X00, 0XFF},
    {0XFA, 0X01, 0XFE},
    {0XFA, 0X02, 0XFD},
    {0XFA, 0X03, 0XFC},
    {0XFA, 0X04, 0XFB},
    {0XFA, 0X05, 0XFA},
    {0XFA, 0X06, 0XF9},
    {0XFA, 0X07, 0XF8},
};

typedef struct{
	void(*pressHandle)(void);
	u8 sound;
	u8 *command;
} _NRK10_Listen;
static _NRK10_Listen NRK10_Listen_List[16];

//需被放入串口接收中断函数
u8 NRK10_dataBuff[] = {0, 0, 0};
void NRK10_ReceiveByte(u8 byte) 
{
	static u8 count = 0;

	switch(count){
		case 0:
			if(byte == 0XFA)
				count++;
			break;
		case 1:
			NRK10_dataBuff[1] = byte;
			count++;
			break;
		case 2:
			NRK10_dataBuff[2] = byte;
			count = 0;
            if(NRK10_dataBuff[1]+NRK10_dataBuff[2]==0XFF)
                NRK10_dataBuff[0] = 0XA1; //REC OK
			break;
		default:
			count = 0;
			break;
	}
}

static u8 commandCnt = 0;

//press: run function, i:sound, command:command
void NRK10_Listen(void(*press)(void), u8 i, const u8 *command)
{
    commandCnt++;
	if(commandCnt < 16)
    {
		NRK10_Listen_List[commandCnt].pressHandle = press;
		NRK10_Listen_List[commandCnt].sound = i;
		NRK10_Listen_List[commandCnt].command = (u8 *)command;
	}
}

void NRK10_Cancel(void)
{
	commandCnt = 0;
}

void NRK10_Handler(void)
{
	u8 i = 1;
    static u8 j = 0;

	if(NRK10_dataBuff[0]==0XA1)
    {
        NRK10_dataBuff[0] = 0X00;
        if(NRK10_dataBuff[1]==0x00)
        {
            NRK10_Overtime_Cnt = 0; //wake up
            dwPlayMusic(6, 1);
            return;
        }
        while(i<=commandCnt)
        {
            if(*(NRK10_Listen_List[i].command+1)==NRK10_dataBuff[1])
            {
                if(NRK10_Overtime_Cnt<NRK10_OVER_TIME) //10s
                {
                    NRK10_Overtime_Cnt = 0;
                    if(NRK10_Listen_List[i].pressHandle != 0)
                        NRK10_Listen_List[i].pressHandle();
                    if(NRK10_Listen_List[i].sound != 0)
                        dwPlayMusic(NRK10_Listen_List[i].sound, 1);
                }
                return;
            }
            i++;
        }
	}
    if(NRK10_Overtime_Cnt>=NRK10_OVER_TIME)
    {
        if(j)
        {
            j = 0;
            dwPlayMusic(30, 2);
        }
    }
    else
    {
        j = 1;
    }
}
