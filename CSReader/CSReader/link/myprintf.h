
#define _DEBUGPRINTF_

#ifdef _DEBUGPRINTF_
extern void dbg_dumpmemory(char *lpprompt, void *indata, int len);

extern void dbg_formatvar(char *fmt,...);


#else

#define dbg_dumpmemory(...)

#define dbg_formatvar(...)

#endif

