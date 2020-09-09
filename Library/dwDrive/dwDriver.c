#include "dwDriver.h"
#include "usart.h"

u8 dataBuff[10];
u16  locaX, locaY;
u8 language; 
u8 keyNum = 0;
u16 fColor = DW_COL_WHITE;
u16 bcolor = DW_COL_BLACK;

DwKeyListen dwKeyListen[30];

void(*dwSend)(u8);

union CharInt{
	u8 b[2];
	u16 a;
};

/***向屏幕发送一个字节***/
void dwSendByte(u8 byte){
	uasrt2SendByte(byte);
}

// param lang: LANGUAGE_CHINESE,LANGUAGE_ENGLISH
void dwSetLanguage(u8 lang){
	if(lang == LANGUAGE_CHINESE){
		language = LANGUAGE_CHINESE;
	}else if(lang == LANGUAGE_ENGLISH){
		language = LANGUAGE_ENGLISH;
	}else if(lang == LANGUAGE_ESPANOL){
		language = LANGUAGE_ESPANOL;
	}	
}

/*******************about key********************/
void dwCancelKey(void){
	keyNum = 0;
}

//****************坐标模式*****************
void dwListenCoord(void(*press)(void), void(*free)(void), const Button* btn){
	if(keyNum < 30){
		dwKeyListen[keyNum].pressHandle = press;
		dwKeyListen[keyNum].freeHandle = free;
		dwKeyListen[keyNum].button = (Button*)btn;
		keyNum++;
	}
}

/**将坐标数据转换成int 型**/
static void dwConvertLocation(void){
	locaX = 0;
	locaX = dataBuff[1];
	locaX <<= 8;
	locaX += dataBuff[2];
	
	locaY = 0;
	locaY = dataBuff[3];
	locaY <<= 8;
	locaY += dataBuff[4];
}

static u8 dwGetKey(void){
	u8 i;
	dwConvertLocation();
	for(i=0; i<keyNum; i++){	
		if((locaX >= dwKeyListen[i].button->xs) && (locaX <= dwKeyListen[i].button->xe)
			&& (locaY >= dwKeyListen[i].button->ys) && (locaY <= dwKeyListen[i].button->ye)){
			break;
		}
	}
	return i;
}

void dwHandlerCoord(void){
	static u8 keyFlag = 0;
	static u8 i;

	// 按下处理
	if((dwQueryCmd() == DW_CMD_PRESSED)&&(keyFlag == 0)){
		keyFlag = 1;
		i = dwGetKey();
		if(i < keyNum){ // 有效按键
			if(dwKeyListen[i].pressHandle != 0){  // 不为空
				dwKeyListen[i].pressHandle();
			}
		}
	}
	
	// 松手处理
	if((dwQueryCmd() == DW_CMD_RELEASE)&&(keyFlag == 1)){
		keyFlag = 0;
//		if((i < keyNum)&&(i==dwGetKey())){ // 有效按键
				if((i < keyNum)){ // 有效按键
			if(dwKeyListen[i].freeHandle != 0){  // 不为空
				dwKeyListen[i].freeHandle();
			}
		}
	}
}

//***************触控模式***********************
void dwListenButton(void(*press)(void), void(*free)(void), u16 Command){
	if(keyNum < 30){
		dwKeyListen[keyNum].pressHandle = press;
		dwKeyListen[keyNum].freeHandle = free;
		dwKeyListen[keyNum].command = Command;
		keyNum++;
	}
}

void dwHandlerButton(void){
	static u8 keyFlag = 0;
	static u8 i;
	u16 cmd = 0;

	// 按下处理
	if ((dwQueryCmd() == DW_KEY_PRESSED) && (keyFlag == 0))
	{
		keyFlag = 1;

		cmd = ((u16)dataBuff[3]<<8)+dataBuff[4];
		for (i = 0; i < keyNum; i++)
		{
			if (cmd == dwKeyListen[i].command)
				break;
		}

		if (i < keyNum)
		{ // 有效按键
			if (dwKeyListen[i].pressHandle != 0)
			{ // 不为空
				dwKeyListen[i].pressHandle();
			}
		}
	}

	// 松手处理
	if ((dwQueryCmd() == DW_KEY_RELEASE) && (keyFlag == 1))
	{
		keyFlag = 0;
		//		if((i < keyNum)&&(i==dwGetKey())){ // 有效按键
		if ((i < keyNum))
		{ // 有效按键
			if (dwKeyListen[i].freeHandle != 0)
			{ // 不为空
				dwKeyListen[i].freeHandle();
			}
		}
	}
}

/*******帧结束符*********/
void dwSendOver(void){
	dwSendByte(0xCC);
	dwSendByte(0x33);
	dwSendByte(0xC3);
	dwSendByte(0x3C);
}

// 设置调色板
void dwSetColor(u16 f,u16 b){
	fColor = f;
	bcolor = b;
	dwSendByte(0xAA);
	dwSendByte(0x40);
	dwSendByte((u8)(f>>8));
	dwSendByte((u8)f);
	dwSendByte((u8)(b>>8));
	dwSendByte((u8)b);
	dwSendOver();
}

/***取指定位置作为背景色***/
void dwSetBackColor(u16 x,u16 y){
	dwSendByte(0xAA);
	dwSendByte(0x42);
	dwSendByte((u8)(x>>8));
	dwSendByte((u8)x);
	dwSendByte((u8)(y>>8));
	dwSendByte((u8)y);
	dwSendOver();
}

/***取指定位置作为前景色***/
void dwSetForeColor(u16 x,u16 y){
	dwSendByte(0xAA);
	dwSendByte(0x43);
	dwSendByte((u8)(x>>8));
	dwSendByte((u8)x);
	dwSendByte((u8)(y>>8));
	dwSendByte((u8)y);
	dwSendOver();
}

/*****全屏显示指定位置图片不区分语言*****/
//para: Num 为位置索引号
void dwDisPicNoL(u8 picNum){
	dwSendByte(0xAA);
	dwSendByte(0x70); //显示图片命令
	dwSendByte(picNum);
	dwSendOver();
}

/*****全屏显示指定位置图片区分语言*****/
//para: Num 为位置索引号
void dwDisPicWithL(u8 picNum){
	dwDisPicNoL(picNum + language);
}

// L = 1区分语言，l= 0 不区分语言
void dwDisButton(const Button* button,u8 l,u16 x,u16 y){
	if(l){
		dwCutPicL(button->picNum,button->xs,button->ys,button->xe,button->ye,x,y);
	}else{
		dwCutPic(button->picNum,button->xs,button->ys,button->xe,button->ye,x,y);
	}
}

// L = 1区分语言，l= 0 不区分语言
void dwDisImage(const Image* image,u8 l,u16 x,u16 y){
	if(l){
		dwCutPicL(image->picNum,image->xs,image->ys,image->xe,image->ye,x,y);
	}else{
		dwCutPic(image->picNum,image->xs,image->ys,image->xe,image->ye,x,y);
	}
}


/****剪切图片显示到指定位置不区分语言****/
void dwCutPic(u8 picNum,u16 xs,u16 ys,u16 xe,u16 ye,u16 x,u16 y){
	dwSendByte(0xAA);
	dwSendByte(0x71);//剪切图片命令
	dwSendByte(picNum);
	
	dwSendByte((u8)(xs>>8));
	dwSendByte((u8)xs);
	dwSendByte((u8)(ys>>8));
	dwSendByte((u8)ys);
	
	dwSendByte((u8)(xe>>8));
	dwSendByte((u8)xe);
	dwSendByte((u8)(ye>>8));
	dwSendByte((u8)ye);

	dwSendByte((u8)(x>>8));
	dwSendByte((u8)x);
	dwSendByte((u8)(y>>8));
	dwSendByte((u8)y);

	dwSendOver();
}

/****剪切图片显示到指定位置区分语言****/
void dwCutPicL(u8 picNum,u16 xs,u16 ys,u16 xe,u16 ye,u16 x,u16 y){
	dwCutPic(picNum+language,xs,ys,xe,ye,x,y);
}


/*****指示大小指定位置显示一个字符*****/
//prama: size=8 8*8; 16 16*16; 24 24*24; 32 32*32
void dwDisChar(TextSize size, u16 x, u16 y, u8 c){
	u8 temp[2]= {0,0};
	temp[0] = c;
	dwDisString(size,x,y,temp);
}

/*****指示大小指定位置显示一个数字*****/
//prama: size=8 8*8; 16 16*16; 24 24*24; 32 32*32
void dwDisNum(TextSize size, u16 x, u16 y, u8 c){
	if(c < 10){
		u8 temp[2]= {0,0};
		temp[0] = c + 0x30;
		dwDisString(size,x,y,temp);
	}	
}

/*****指示大小指定位置显示文字*****/
//prama: size=8 8*8; 16 16*16; 24 24*24; 32 32*32
void dwDisString(TextSize size, u16 x, u16 y, u8 string[]){
	u8 i = 0;
	dwSendByte(0xAA);
	switch(size){
		case DW_SIZE_8:
			dwSendByte(0x53);
			break;
		case DW_SIZE_12:
			dwSendByte(0x6E);
			break;
		case DW_SIZE_16:
			dwSendByte(0x54);
			break;
		case DW_SIZE_24:
			dwSendByte(0x6F);
			break;
		case DW_SIZE_32:
			dwSendByte(0x55);
			break;	
		case DW_SIZE_48:
			dwSendByte(0x98);
			break;
	}
	dwSendByte((u8)(x>>8));
	dwSendByte((u8)x);
	dwSendByte((u8)(y>>8));
	dwSendByte((u8)y);
	if(size == DW_SIZE_48){
		dwSendByte(0x24);    // lib_ID
		dwSendByte(0x81);    // C_Mode
		dwSendByte(0x05);    // C_Dots
		dwSendByte((u8)(fColor>>8));
		dwSendByte((u8)fColor);
		dwSendByte((u8)(bcolor>>8));
		dwSendByte((u8)bcolor);
	}
	while(string[i]){
		dwSendByte(string[i]);
		i++;
	}
	dwSendOver();
}

/****** 配置屏幕工作模式 ******/
void dwSetMode(void){
	dwSendByte(0xAA);
	dwSendByte(0xE0);
	dwSendByte(0x55);
	dwSendByte(0xAA);
	dwSendByte(0x5A);
	dwSendByte(0xA5);
	dwSendByte(0x00);  // 显示模式
	dwSendByte(0x07);  // 波特率115200
	dwSendByte(0x48);  // 触控模式 只上传一次0x73
	dwSendByte(0xEF);  // 文本显示模式滤除背景
	dwSendOver();
}

/**** 屏幕自带蜂鸣器响一声 *******/
void dwSound(u8 duration){  // duration*10ms
	dwSendByte(0xAA);
	dwSendByte(0x79);
	dwSendByte(duration);
	dwSendOver();
}

 /*****等待松手*****/
void dwWaitRelease(void){
	while(dataBuff[0] != 0x72 && dataBuff[0] != 0x78);
}

 /*****等待按下*****/
void dwWaitPress(void){
	while(dataBuff[0] != 0x73 && dataBuff[0] != 0x79);
}

/***查询按键状态****/
// DW_CMD_PRESSED= 0x73  按下
// DW_CMD_RELEASE= 0x72  松手
// DW_CMD_DONE   = 0xE4  触控校正完成
u8 dwQueryCmd(void){
	return dataBuff[0];
}

/****触屏校准***/
void dwCalibration(void){ 
	dwSendByte(0xAA);
	dwSendByte(0xE4); 
	dwSendByte(0x55);
	dwSendByte(0xAA);
	dwSendByte(0x5A);
	dwSendByte(0xA5);
	dwSendOver();
	while(dataBuff[0] != 0xE4);  // 等待收到校准成功指令
}

// 最大音量播放音乐
// param id:  0--120;
// param num: 1--120;
extern u8 muteFlag;
void dwPlayMusic(u8 id, u8 num){
	if(muteFlag) return; //静音
	dwSendByte(0xAA);
	dwSendByte(0x30);
	dwSendByte(id);
	dwSendByte(num);
	dwSendByte(0xA0); 	//音量0x00~0xff
	dwSendOver();
}

// 停止音乐
void dwStopMusic(void){
	dwSendByte(0xAA);
	dwSendByte(0x30);
	dwSendByte(0);
	dwSendByte(0);
	dwSendByte(0);   
	dwSendOver();
}

//调节音量
void dwPlayVol(u8 i){
	dwSendByte(0xAA);
	dwSendByte(0x32);
	dwSendByte(i);
	dwSendOver();
}

//需被放入串口接收中断函数
/*******收到屏幕的数据外理********/
void dwReceiveByte(u8 byte){
	static u8 count = 0;
	static u8 tep=0;
	switch(count){
		case 0:
			if(byte == 0xAA){
				count++;
			}
			break;
		case 1:			
			if(byte == 0x73){ // 按下
				count++;
				tep = byte;
			}else if(byte == 0x72){ // 松手
//				count++;
//				tep = byte;
				count = 0;
				dataBuff[0] = 0x72;
			}else if(byte == 0xE4){  //触屏校准成功
				count = 4;
				tep = byte;
			}else if(byte == 0x00){  // 握手成功
				count = 0;
				dataBuff[0] = 0x74; 
			}else if(byte == 0X79){  //触控模式按下
				count = 4;
				tep = byte;
			}else if(byte == 0X78){  //触控模式松开
				count = 4;
				tep = byte;
			}
			else{
				count = 0;
			}
			break;
		case 2:
			dataBuff[1] = byte;
			count++;
			break;
		case 3:
			dataBuff[2] = byte;
			count++;
			break;
		case 4:
			dataBuff[3] = byte;
			count++;
			break;
		case 5:
			dataBuff[4] = byte;
			count++;
			break;
		case 6:
			if(byte == 0xCC){
				count++;
			}else{
				count = 0;
			}
			break;
		case 7:
			if(byte == 0x33){
				count++;
			}else{
				count = 0;
			}
			break;
		case 8:
			if(byte == 0xC3){
				count++;
			}else{
				count = 0;
			}
			break;
		case 9:  
			if(byte == 0x3C){
				count = 0;
				dataBuff[0] = tep;		
			}
			break;
		default:
			count = 0;
			break;
	}
}

//Display time: 00:00
void funDisTime(u16 timing, u16 x, u16 y){
	dwDisNum(DW_SIZE_48, x, y, timing / 600);
	dwDisNum(DW_SIZE_48, x + 24, y, timing / 60 % 10);
	dwDisChar(DW_SIZE_48, x + 48, y, ':');
	dwDisNum(DW_SIZE_48, x + 72, y, timing % 60 / 10);
	dwDisNum(DW_SIZE_48, x + 96, y, timing % 10);
}
