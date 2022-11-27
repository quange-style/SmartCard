#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "myprintf.h"

#ifdef _DEBUGPRINTF_
void dbg_dumpmemory(char *lpprompt, void *indata, int len)
{
	//return;
    int i;
    unsigned char *lpdata=(unsigned char *)indata;

    fprintf(stdout,"%s",lpprompt);

    for (i=0;i<len;i++)
    {
        fprintf(stdout,"%02X ",lpdata[i]);
    }
    fprintf(stdout,"\n");
}


void dbg_formatvar(char *fmt,...)
{
	//return;
    int cnt;
    va_list argptr;
    char szbuf[256];

//	fprintf(stdout,"%s",lpprompt);

    va_start(argptr,fmt);
    cnt = vsnprintf( szbuf, sizeof(szbuf) - 1, fmt, argptr);

    fprintf(stdout,"%s ",szbuf);
    fprintf(stdout,"\n");
}
#endif

