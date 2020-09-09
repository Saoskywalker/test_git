#ifndef __DWDRIVER_H
#define __DWDRIVER_H

#include "myDefine.h"

#define LANGUAGE_CHINESE     0
#define LANGUAGE_ENGLISH     20
#define LANGUAGE_ESPANOL     70

#define DW_COL_RED		0xF800
#define DW_COL_GREEN	0x07E0
#define DW_COL_BLUE 	0x001F
#define DW_COL_WHITE	0xFFFF
#define DW_COL_BLACK	0x0000

#define DW_KEY_PRESSED	0x79   // ���ذ���
#define DW_KEY_RELEASE	0x78   // ��������
#define DW_CMD_PRESSED	0x73   // ����
#define DW_CMD_RELEASE	0x72   // ����
#define DW_CMD_DONE     0xE4   // У�����

#define DW_KEY_NONE     0xFF   // �ް���

typedef enum TextSize{
	DW_SIZE_8,
	DW_SIZE_12,
	DW_SIZE_16,
	DW_SIZE_24,
	DW_SIZE_32,
	DW_SIZE_48,
}TextSize;

typedef struct Button{
	u8 picNum;
	u16  xs;
	u16  ys;
	u16  xe;
	u16  ye;
}Button;

typedef struct Image{
	u8 picNum;
	u16  xs;
	u16  ys;
	u16  xe;
	u16  ye;
}Image;

typedef struct{
	void(*pressHandle)(void);
	void(*freeHandle)(void);
	Button* button;
	u16 command;
}DwKeyListen;

extern u16  locaX, locaY;
extern u8   dataBuff[10];
extern u8 language; 
extern DwKeyListen dwKeyListen[30];

void dwListenCoord(void(*press)(void), void(*free)(void), const Button* btn);
void dwHandlerCoord(void);
void dwListenButton(void(*press)(void), void(*free)(void), u16 Command);
void dwHandlerButton(void);

// param lang: LANGUAGE_CHINESE or LANGUAGE_ENGLISH
void dwSetLanguage(u8 lang);

/***����Ļ����һ���ֽ�***/
void dwSendByte(u8 byte);

/*******֡������*********/
void dwSendOver(void);

// ���õ�ɫ��
void dwSetColor(u16 f,u16 b);


/***ȡָ��λ����Ϊ����ɫ***/
void dwSetBackColor(u16 x,u16 y);

/***ȡָ��λ����Ϊǰ��ɫ***/
void dwSetForeColor(u16 x,u16 y);

// L = 1�������ԣ�l= 0 ����������
void dwDisButton(const Button* button,u8 l,u16 x,u16 y);

// L = 1�������ԣ�l= 0 ����������
void dwDisImage(const Image* image,u8 l,u16 x,u16 y);


/*****ȫ����ʾָ��λ��ͼƬ����������*****/
//para: Num Ϊλ��������
void dwDisPicNoL(u8 picNum);

/*****ȫ����ʾָ��λ��ͼƬ��������*****/
//para: Num Ϊλ��������
void dwDisPicWithL(u8 picNum);

/**** ��Ļ�Դ���������һ�� *******/
void dwSound(u8 duration);  // duration*10ms


/****����У׼***/
void dwCalibration(void);


/******������Ļ����ģʽ******/
void dwSetMode(void);

/****����ͼƬ��ʾ��ָ��λ����������****/
void dwCutPic(u8 picNum,u16 xs,u16 ys,u16 xe,u16 ye,u16 x,u16 y);

 /****����ͼƬ��ʾ��ָ��λ�ò���������****/
void dwCutPicL(u8 picNum,u16 xs,u16 ys,u16 xe,u16 ye,u16 x,u16 y);


 /*****ָʾ��Сָ��λ����ʾһ���ַ�*****/
 //prama: size=8 8*8; 16 16*16; 24 24*24; 32 32*32
 void dwDisChar(TextSize size, u16 x, u16 y, u8 c);


 /*****ָʾ��Сָ��λ����ʾһ������*****/
 //prama: size=8 8*8; 16 16*16; 24 24*24; 32 32*32
 void dwDisNum(TextSize size, u16 x, u16 y, u8 c);


 /*****ָʾ��Сָ��λ����ʾ����*****/
 //prama: size=8 8*8; 16 16*16; 24 24*24; 32 32*32
 void dwDisString(TextSize size, u16 x, u16 y, u8 string[]);


 /*****�ȴ�����*****/
void dwWaitRelease(void);

 /*****�ȴ�����*****/
void dwWaitPress(void);

 /***��ѯ����״̬****/
 // 0x73  ����
 // 0x72  ����
 // 0xE4  ����У�����
 u8 dwQueryCmd(void);

void dwCancelKey(void);

//��������
void dwPlayMusic(u8 id, u8 num);

// ֹͣ����
void dwStopMusic(void);

//��������
void dwPlayVol(u8 i);

//�豻���봮�ڽ����жϺ���
/*******�յ���Ļ����������********/
void dwReceiveByte(u8 byte);

void funDisTime(u16 timing, u16 x, u16 y);

#endif
