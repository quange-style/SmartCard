/*
 * Program: 	serial.c
 * Author:  	Paul Dean
 * Version:     0.0.3
 * Date:    	2002-02-19
 * Description: To provide underlying serial port function,
 *              for high level applications.
 *
*/

#include <termios.h>            /* tcgetattr, tcsetattr */
#include <stdio.h>              /* perror, printf, puts, fprintf, fputs */
#include <unistd.h>             /* read, write, close */
#include <fcntl.h>              /* open */
#include <sys/signal.h>
#include <sys/types.h>
#include <string.h>             /* bzero, memcpy */
#include <limits.h>             /* CHAR_MAX */

#include "../link/vardef.h"             /* INT32, INT16, INT8, UINT32, UINT16, UINT8 */
#include "serial.h"


typedef struct _SERIALVAR{
	INT32 fd;
	struct termios termios_old, termios_new;
	fd_set   fs_read, fs_write;
	struct timeval tv_timeout;
	struct sigaction sigaction_io;   /* definition of signal action */
} SERIALVAR;

/*
 * Decription for TIMEOUT_SEC(buflen,baud);
 * baud bits per second, buflen bytes to send.
 * buflen*20 (20 means sending an octect-bit data by use of the maxim bits 20)
 * eg. 9600bps baudrate, buflen=1024B, then TIMEOUT_SEC = 1024*20/9600+1 = 3
 * don't change the two lines below unless you do know what you are doing.
*/
#define TIMEOUT_SEC(buflen,baud) (buflen*20/baud+2)
#define TIMEOUT_USEC 20000

#define MAXPORT 5 //支持的串口数量


SERIALVAR hCOM[MAXPORT];

//	static int      SetPortSig ();
//	static void     signal_handler_IO (int status);
static void     SetBaudrate(INT32 ComPort, INT32 baudrate);
static INT32    GetBaudrate(INT32 ComPort);
static void     SetDataBit (INT32 ComPort, INT32 databit);
static INT32    BAUDRATE (INT32 baudrate);
static INT32    _BAUDRATE (INT32 baudrate);
static INT32    SetPortAttr (INT32 ComPort, INT32 baudrate, INT32 databit,
                             const char *stopbit, char parity);
static void     SetStopBit (INT32 ComPort, const char *stopbit);
static void     SetParityCheck (INT32 ComPort, char parity);
static INT32    PortIsValid(INT32 ComPort);

INT32 PortIsValid(INT32 ComPort)
{
		if (ComPort > 3) return(-1);
		if (hCOM[ComPort].fd>0)
		{
			return 0;
		}
		else
		{
			return(-1);
		}

}

/* Open serial port ComPort at baudrate baud rate. */
INT32 OpenComPort(INT32 ComPort, INT32 baudrate, INT32 databit,
                   char *stopbit, char parity)
{
		INT32    fd;             //File descriptor for the port
    char           *pComPort;
    INT32           retval;

		if (ComPort > 3) return(-1);

    switch (ComPort) {
    case 0:
        pComPort = "/dev/ttyS0";
        break;
    case 1:
        pComPort = "/dev/ttyS1";
        break;
   case 2:
        pComPort = "/dev/ttyS2";
        break;
   /*  case 3:
        pComPort = "/dev/ttySAC3";
        break;
	case 4:
		pComPort = "/dev/ttyGS0";
		break;*/
    default:
        pComPort = "/dev/ttyS0";
        break;
    }

    hCOM[ComPort].fd = open(pComPort, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (-1 == fd) {
        fprintf(stderr, "cannot open port %s\n", pComPort);
        return (-1);
    }
    tcgetattr(hCOM[ComPort].fd, &hCOM[ComPort].termios_old);       /* save old termios value */
    /* 0 on success, -1 on failure */
    retval = SetPortAttr (ComPort, baudrate, databit, stopbit, parity);
    if (-1 == retval) {
        fprintf(stderr, "\nport %s cannot set baudrate at %d\n", pComPort,
                 baudrate);
    }
    return (retval);
}

/* close serial port by use of file descriptor fd */
void CloseComPort (INT32 ComPort)
{

    /* flush output data before close and restore old attribute */
    if (0 == PortIsValid(ComPort))
    {
    	tcsetattr(hCOM[ComPort].fd, TCSADRAIN, &hCOM[ComPort].termios_old);
    	close(hCOM[ComPort].fd);
    	hCOM[ComPort].fd = -1;
    }
}

//======================================================================================================
//名称:ReadComPort
//功能:从指定的端口接收数据
//入口参数:
//         ComPort-接收数据的端口号,
//         maxrecv-接收缓冲最大容量,
//         timeout-接收超时时间设定(微秒，=0时表示无限等待),
//         data-缓冲指针,
//         pfun-回调函数(用于对接收的数据进行判断)
//出口参数:
//         data-接收的数据(当返回值=0时有效)
//返回值: 0-成功接收数据
//======================================================================================================
INT32 ReadComPort(INT32 ComPort, INT32 maxrecv, UINT32 timeout, void *data, INT32 (*pfun)(INT32 *,void *))
{
    INT32 retval=0;
    INT32 sumlen=0, pos=0, len=0;
    INT32 maxlimite=maxrecv;//最大接收长度，防止缓冲溢出
    UINT32 tmo=timeout;
    if (0 == PortIsValid(ComPort))//判断端口是否用效
    {
    	while(1)
    	{
		    FD_ZERO(&hCOM[ComPort].fs_read);
		    FD_SET(hCOM[ComPort].fd, &hCOM[ComPort].fs_read);

				if (tmo != 0)
				{
			    hCOM[ComPort].tv_timeout.tv_sec =tmo/1000000;
			    hCOM[ComPort].tv_timeout.tv_usec = tmo%1000000;//

			    retval = select(hCOM[ComPort].fd + 1, &hCOM[ComPort].fs_read, NULL, NULL, &hCOM[ComPort].tv_timeout);
		    }
		    else
		    {
			    retval = select(hCOM[ComPort].fd + 1, &hCOM[ComPort].fs_read, NULL, NULL, NULL);
		    }


		    if (retval>0)
		    {
		    	len = read(hCOM[ComPort].fd, data+pos, maxlimite);

					sumlen += len;
		    	pos += len;
		    	maxlimite -= len;

			    if (pfun)
			    {
			    	if (sumlen > 0)
			    		if ( 0 == pfun(&sumlen, data)) return(sumlen);
			    }

			    if (0 == maxlimite)
			    {
			    	return(sumlen);//缓冲区满
			    }

			    tmo = 50000;//当收到第一次数据时，有可能数据太长需要接收多次，所以进入接收状态后调整超时时间


		    }
		    else
		    {
//		    	if (sumlen > 0) return(sumlen);
//		    	else
		    	return(-1);

		    }
	    }

    }
    else
    	return(-1);
}

//	INT32 ReadComPort(INT32 ComPort, void *data, INT32 datalength, INT32 (*pfun)(UINT8,void *))
//	{
//	    INT32 retval = 0;
//	    INT32 revlen = datalength, sumlen = 0,pos=0,len=0;
//	    if (0 == PortIsValid(ComPort))
//	    {
//	    	while(1)
//	    	{
//			    FD_ZERO(&hCOM[ComPort].fs_read);
//			    FD_SET(hCOM[ComPort].fd, &hCOM[ComPort].fs_read);
//			    hCOM[ComPort].tv_timeout.tv_sec = 0;//500;//TIMEOUT_SEC(datalength, GetBaudrate(ComPort));
//			    hCOM[ComPort].tv_timeout.tv_usec = 10000;//TIMEOUT_USEC;
//			    retval = select(hCOM[ComPort].fd + 1, &hCOM[ComPort].fs_read, NULL, NULL, &hCOM[ComPort].tv_timeout);
//			    if (retval>0)
//			    {
//			    	sumlen += read(hCOM[ComPort].fd, data+pos, revlen);
//				    pos += len;
//				    if (pfun)
//				    {
//				    	if (sumlen > 0)
//				    		if ( 0 == pfun(sumlen, data)) return(sumlen);
//				    }
//
//			    	revlen = datalength - sumlen;
//			    	if (revlen == 0) return(sumlen);
//
//			    }
//			    else
//			    {
//			    	if (sumlen > 0) return(sumlen);
//			    	else
//			    	return(-1);
//
//			    }
//		    }
//
//	    }
//	    else
//	    	return(-1);
//	}

/*
 * Write datalength bytes in buffer given by UINT8 *data,
 * return value: bytes written
 * Nonblock mode
*/
INT32 WriteComPort (INT32 ComPort, UINT8 * data, INT32 datalength)
{
    INT32 retval, len = 0, total_len = 0;
    if (0 == PortIsValid(ComPort))
    {
	    FD_ZERO(&hCOM[ComPort].fs_write);
	    FD_SET(hCOM[ComPort].fd, &hCOM[ComPort].fs_write);
	    hCOM[ComPort].tv_timeout.tv_sec = TIMEOUT_SEC(datalength, GetBaudrate(ComPort));
	    hCOM[ComPort].tv_timeout.tv_usec = TIMEOUT_USEC;

	    for (total_len = 0, len = 0; total_len < datalength;) {
	        retval = select(hCOM[ComPort].fd + 1, NULL, &hCOM[ComPort].fs_write, NULL, &hCOM[ComPort].tv_timeout);
	        if (retval) {
	            len = write(hCOM[ComPort].fd, &data[total_len], datalength - total_len);
	            if (len > 0) {
	                total_len += len;
	            }
	        }
	        else {
	            tcflush(hCOM[ComPort].fd, TCOFLUSH);     /* flush all output data */
	            break;
	        }
	    }
	    return (total_len);
	  }
	  else
	  	return(0);
}

/* get serial port baudrate */
static INT32 GetBaudrate (INT32 ComPort)
{
    return (_BAUDRATE(cfgetospeed(&hCOM[ComPort].termios_new)));
}

/* set serial port baudrate by use of file descriptor fd */
static void SetBaudrate(INT32 ComPort, INT32 baudrate)
{
    hCOM[ComPort].termios_new.c_cflag = BAUDRATE(baudrate);  /* set baudrate */
}

static void SetDataBit(INT32 ComPort, INT32 databit)
{
    hCOM[ComPort].termios_new.c_cflag &= ~CSIZE;
    switch (databit) {
    case 8:
        hCOM[ComPort].termios_new.c_cflag |= CS8;
        break;
    case 7:
       hCOM[ComPort].termios_new.c_cflag |= CS7;
        break;
    case 6:
        hCOM[ComPort].termios_new.c_cflag |= CS6;
        break;
    case 5:
        hCOM[ComPort].termios_new.c_cflag |= CS5;
        break;
    default:
        hCOM[ComPort].termios_new.c_cflag |= CS8;
        break;
    }
}

static void SetStopBit(INT32 ComPort, const char *stopbit)
{
    if (0 == strcmp(stopbit, "1")) {
        hCOM[ComPort].termios_new.c_cflag &= ~CSTOPB; /* 1 stop bit */
    }
    else if (0 == strcmp(stopbit, "1.5")) {
        hCOM[ComPort].termios_new.c_cflag &= ~CSTOPB; /* 1.5 stop bits */
    }
    else if (0 == strcmp(stopbit, "2")) {
        hCOM[ComPort].termios_new.c_cflag |= CSTOPB;  /* 2 stop bits */
    }
    else {
        hCOM[ComPort].termios_new.c_cflag &= ~CSTOPB; /* 1 stop bit */
    }
}

static void SetParityCheck(INT32 ComPort, char parity)
{
    switch (parity) {
    case 'N':                  /* no parity check */
        hCOM[ComPort].termios_new.c_cflag &= ~PARENB;
        break;
    case 'E':                  /* even */
        hCOM[ComPort].termios_new.c_cflag |= PARENB;
        hCOM[ComPort].termios_new.c_cflag &= ~PARODD;
        break;
    case 'O':                  /* odd */
        hCOM[ComPort].termios_new.c_cflag |= PARENB;
        hCOM[ComPort].termios_new.c_cflag |= ~PARODD;
        break;
    default:                   /* no parity check */
        hCOM[ComPort].termios_new.c_cflag &= ~PARENB;
        break;
    }
}

static INT32 SetPortAttr(INT32 ComPort, INT32 baudrate,
                          INT32 databit, const char *stopbit, char parity)
{
    bzero(&hCOM[ComPort].termios_new, sizeof (hCOM[ComPort].termios_new));
    cfmakeraw(&hCOM[ComPort].termios_new);
    SetBaudrate(ComPort, baudrate);
    hCOM[ComPort].termios_new.c_cflag |= CLOCAL | CREAD;      /* | CRTSCTS */
    SetDataBit(ComPort, databit);
    SetParityCheck(ComPort, parity);
    SetStopBit (ComPort, stopbit);
    hCOM[ComPort].termios_new.c_oflag = 0;
    hCOM[ComPort].termios_new.c_lflag |= 0;
    hCOM[ComPort].termios_new.c_oflag &= ~OPOST;
    hCOM[ComPort].termios_new.c_cc[VTIME] = 1;        /* unit: 1/10 second. */
    hCOM[ComPort].termios_new.c_cc[VMIN] = 1; /* minimal characters for reading */
    tcflush(hCOM[ComPort].fd, TCIFLUSH);
    return (tcsetattr(hCOM[ComPort].fd, TCSANOW, &hCOM[ComPort].termios_new));
}

static INT32 BAUDRATE (INT32 baudrate)
{
    switch (baudrate) {
    case 0:
        return (B0);
    case 50:
        return (B50);
    case 75:
        return (B75);
    case 110:
        return (B110);
    case 134:
        return (B134);
    case 150:
        return (B150);
    case 200:
        return (B200);
    case 300:
        return (B300);
    case 600:
        return (B600);
    case 1200:
        return (B1200);
    case 2400:
        return (B2400);
    case 9600:
        return (B9600);
    case 19200:
        return (B19200);
    case 38400:
        return (B38400);
    case 57600:
        return (B57600);
    case 115200:
        return (B115200);
    default:
        return (B9600);
    }
}

static INT32 _BAUDRATE (INT32 baudrate)
{
/* reverse baudrate */
    switch (baudrate) {
    case B0:
        return (0);
    case B50:
        return (50);
    case B75:
        return (75);
    case B110:
        return (110);
    case B134:
        return (134);
    case B150:
        return (150);
    case B200:
        return (200);
    case B300:
        return (300);
    case B600:
        return (600);
    case B1200:
        return (1200);
    case B2400:
        return (2400);
    case B9600:
        return (9600);
    case B19200:
        return (19200);
    case B38400:
        return (38400);
    case B57600:
        return (57600);
    case B115200:
        return (115200);
    default:
        return (9600);
    }
}


