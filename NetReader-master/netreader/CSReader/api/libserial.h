/*
 * =====================================================================================
 *
 *       Filename:  libserial.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年03月10日 09时23分25秒 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __LIBSERIAL_H__
#define __LIBSERIAL_H__

#ifdef __cplusplus
extern "C"
{

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  libserial_init
 *  Description:  初始化串行接口
 * 					dev：串口设备名
 * 					speed：串口波特率，可以是115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300, 0。
 * 					parity：奇偶校验。值为'N','E','O','S'。 
 * 					databits：数据位，值是5、6、7或者8, (扩展串口只设置8)。
 * 					stopbits：停止位，值是1或者2。 
 * 					hwf：硬件流控制。1为打开，0为关闭。
 * 					swf：软件流控制。1为打开，0为关闭。
 * =====================================================================================
 */
int libserial_init(char *dev, int speed, int parity, int databits, int stopbits, int hwf, int swf);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  libserial_close
 *  Description:  关闭串行接口
 *  Param:			fd - 打开的接口句柄
 * =====================================================================================
 */
int libserial_close(void);



//========================================================
//功能:发送数据报文
//入口参数
//	 buf=发送缓冲
//    buflen=数据长度
//出口参数
//       无
//返回值
//       0=发送完成
//       -1=发送失败
//========================================================
int libserial_send_package(unsigned char *buf, const int buflen);


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  
 *  Description:  
 *  Param:
 * =====================================================================================
 */
//extern int libserial_recv_package(int fd, const char begin, const char end, char *buf, int buflen, const int tmout);
//========================================================
//功能:接收数据报文
//入口参数
//	 maxrecv=接收缓冲大小
//    timeout=超时时间，单位为100ms
//出口参数
//       buf=接收缓冲
//返回值
//       0=执行无返回
//       -1=接收超时
//       -2=接收缓冲区满
//       -3=crc校验错
//       >0=有正确的数据报文件，返回整个报文长度
//========================================================
int libserial_recv_package(const int maxrecv, int timeout, unsigned char *buf);

};
#endif

#endif // #ifndef __SERIAL_H__
