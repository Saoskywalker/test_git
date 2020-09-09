
#ifndef __CONTEXT_H
#define __CONTEXT_H

#include "myDefine.h"

typedef enum{
	Page_Context,                     // 上下文
	Page_Function,                    // 功能页	
	Page_Menus,						  // 功能菜单页
	Page_Password,                    // 密码页
	Page_Set,                         // 设置页
	Page_Define,					  // 工作头标定
}Page;

//extern Page nextPage;
//extern Page requestPage;

void setNextPage(Page next);

Page getNextPage(void);

void setRequestPage(Page request);

Page getRequestPage(void);
	

#endif



