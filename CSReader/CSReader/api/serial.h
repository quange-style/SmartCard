#ifndef _SERIAL_H
#define _SERIAL_H 1
//#pragma once
#include "../link/vardef.h"             /* INT32, INT16, INT8, UINT32, UINT16, UINT8 */

/* serial.c */
//OpenComPort(1, 115200, 8, "1", 'N');
INT32 OpenComPort (INT32 ComPort, INT32 baudrate, INT32 databit, char *stopbit, char parity);
//void CloseComPort (INT32 ComPort);
//INT32 ReadComPort (INT32 ComPort, void *data, INT32 datalength);
//INT32 ReadComPort(INT32 ComPort, void *data, INT32 datalength, INT32 (*pfun)(UINT8,void *));
//INT32 ReadComPort(INT32 ComPort, void *data, INT32 (*pfun)(INT32 *,void *));
INT32 ReadComPort(INT32 ComPort, INT32 maxrecv, UINT32 timeout, void *data, INT32 (*pfun)(INT32 *,void *));
//INT32 WriteComPort (INT32 ComPort, UINT8 * data, INT32 datalength);
//INT32 SendFile (const char *pathname, INT32 echo);

//#endif /* serial.c */

/*class serial
{
public:
    serial(void);
    ~serial(void);

    static INT32 OpenComPort (INT32 ComPort, INT32 baudrate, INT32 databit, const char *stopbit, char parity);
	static void CloseComPort (INT32 ComPort);
	//INT32 ReadComPort (INT32 ComPort, void *data, INT32 datalength);
	//INT32 ReadComPort(INT32 ComPort, void *data, INT32 datalength, INT32 (*pfun)(UINT8,void *));
	//INT32 ReadComPort(INT32 ComPort, void *data, INT32 (*pfun)(INT32 *,void *));
	static INT32 ReadComPort(INT32 ComPort, INT32 maxrecv, UINT32 timeout, void *data, INT32 (*pfun)(INT32 *,void *));
	static INT32 WriteComPort (INT32 ComPort, UINT8 * data, INT32 datalength);
};*/
#endif
