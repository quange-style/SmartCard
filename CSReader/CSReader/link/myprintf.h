#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


#include <string.h> //strrchr()函数所需头文件
//linux :
#define FILENAME(x) strrchr(x,'/')?strrchr(x,'/')+1:x


#define HS_LOG_EN
#ifdef HS_LOG_EN

//#define HS_LOG(_fmt, arg...) \
//	printf(" %s(%d): " _fmt, __func__, __LINE__, ##arg);

#define HS_LOG(fmt, args...) printf("[%s::%s:%d] " fmt ,FILENAME(__FILE__),__FUNCTION__,__LINE__, ##args)

#else
#define HS_LOG(fmt, args...)


#endif



//#define _DEBUGPRINTF_

#ifdef _DEBUGPRINTF_
extern void dbg_dumpmemory(char *lpprompt, void *indata, int len);

extern void dbg_formatvar(char *fmt,...);


#else

void hs_dumpmemory(char *lpprompt, void *indata, int len,const void *file,const void *fun, int line);


#define dbg_dumpmemory(lpprompt,indata,len) hs_dumpmemory(lpprompt,indata,len,FILENAME(__FILE__),__FUNCTION__,__LINE__)
#define dbg_formatvar(fmt,args...) printf("\n[%s::%s:%d] " fmt ,FILENAME(__FILE__),__FUNCTION__,__LINE__, ##args)


#endif




