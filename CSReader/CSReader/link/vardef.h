#ifndef _VARDEF_H_
#define _VARDEF_H_

#define _DEBUG_DISPLAY
#ifdef _DEBUG_DISPLAY
#define db_printf(...)   printf(__VA_ARGS__)
#else
#define db_printf(...)
#endif


/* 
 * sizeof (int) == 4;
 * sizeof (short) == 2;
 * sizeof (char) == 1;
 */

/* for windows data type compatible */
#define _(text) text
#define  TEXT(text)	text

#define MASTER_ANTENNA
#define SUB_ANTENNA

typedef int     LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned char BYTE;
typedef char    TCHAR;

typedef int     INT32;
typedef short   INT16;
typedef char    INT8;
typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;


typedef struct {
    const char     *program_name;
    const char     *package;
    const char     *version;
    const char     *authors;
    INT32           baudrate;
    char           *device;
    INT32           debug;
    INT32           echo;
    INT32           port;
    INT32           databit;
    char           *stopbit;
    char            parity;
} Arguments;

extern Arguments args;


/* some useful macros */
#ifndef MAX
# define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
# define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

//=================================
//上位机下发数据包解析结构
//fm340ctrl和listenprocess文件会用到
//=================================
typedef struct _RFUBIT{
	UINT8 other:7;//天线，0=主，1=副
	UINT8 rf:1;
} RFBIT;
	
typedef struct _LISTENPK{
	UINT8 node;
	UINT8 cmd;
	RFBIT rfu;
	UINT8 datalength;
	UINT8 data[2048];
} LISTENPK, *PLISTENPK;



#endif                          /*vardef.h */
