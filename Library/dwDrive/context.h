
#ifndef __CONTEXT_H
#define __CONTEXT_H

#include "myDefine.h"

typedef enum{
	Page_Context,                     // ������
	Page_Function,                    // ����ҳ	
	Page_Menus,						  // ���ܲ˵�ҳ
	Page_Password,                    // ����ҳ
	Page_Set,                         // ����ҳ
	Page_Define,					  // ����ͷ�궨
}Page;

//extern Page nextPage;
//extern Page requestPage;

void setNextPage(Page next);

Page getNextPage(void);

void setRequestPage(Page request);

Page getRequestPage(void);
	

#endif



