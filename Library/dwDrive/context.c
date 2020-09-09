
#include "context.h"
#include "myDefine.h"

Page nextPage;
Page requestPage;

void setNextPage(Page next){
	nextPage = next;	
}

Page getNextPage(void){
	return nextPage;
}

void setRequestPage(Page request){
	requestPage = request;
}

Page getRequestPage(void){
	return requestPage;
}

