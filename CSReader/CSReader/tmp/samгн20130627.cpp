#include <stdio.h>
#include <string.h>
#include <unistd.h> //usleep
#include <sys/io.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <stdarg.h>
#include "sam.h"
//#include <time.h>
#include <sys/ioctl.h>


#define uchar unsigned char
#define uint unsigned short


//static int g_currentsocket=-1;
static int g_hostsam=-1;


#define SAM_IOC_MAGIC 'S'
#define SOCKETSET			_IO(SAM_IOC_MAGIC, 0)
#define SAMRST				_IO(SAM_IOC_MAGIC, 1)
#define SAMCMD				_IO(SAM_IOC_MAGIC, 2)
#define SAMIO					_IO(SAM_IOC_MAGIC, 3)

//========================================================
//功能:对sam  设备进行初始化,  在应用启动时必须要做
//入口参数
//	                无
//出口参数
//                   无
//返回值
//       0=成功
//      -1=打开设备句柄出错
//========================================================
int samdev_init(void)
{
    int nresult=-1;

    g_hostsam = open("/dev/samdev0", O_RDWR);
    if (g_hostsam > 0)
    {
    	nresult = 0;
    }
    return nresult;
}

//========================================================
//功能:设备sam  卡工作速率
//入口参数
//	 nsamsck=sam卡对应卡槽
//    baudrate=sam卡速率序号0=9600,1=19200,2=38400,3=57600,4=115200
//出口参数
//       无
//返回值
//       无
//========================================================

void sam_setbaud(unsigned char nsamsck,unsigned char baudrate)
{
    unsigned char tmp[2];


    tmp[0] = nsamsck;
    tmp[1] = baudrate;
    ioctl(g_hostsam,SOCKETSET,tmp);

}


//========================================================
//功能:对指定槽号的sam  卡进行复位
//入口参数
//	 nsamsck=sam卡对应卡槽
//出口参数
//       saminf=6字节的sam  卡号
//       lpdata=复位返回信息
//返回值
//       0=复位失败
//       >0复位成功后返回的数据
//========================================================
unsigned char sam_rst(unsigned char nsamsck,unsigned char *lpdata)
{
    unsigned char len;
    unsigned char ApduMesg[256];

    ApduMesg[0] = nsamsck;
    ioctl(g_hostsam,SAMRST,ApduMesg);
    len = ApduMesg[0] ;
    if (len > 0)
        memcpy(lpdata,ApduMesg+1,len);

    return len;


}

//========================================================
//功能:发送指令让sam  卡执行
//入口参数
//	 nsamsck=sam卡对应卡槽
//    nlen=要发送的指令长度(包括指令结构和数据)
//    indata=发送的指令数据流
//出口参数
//       outdata=sam卡执行完成后返回的数据
//       sam_sw=sam执行状态码,  一般0x9000  是成功
//返回值
//       0=执行无返回
//       >0执行有返回，需结合sam_sw  判断是否成功执行
//========================================================
unsigned char sam_cmd(unsigned char nsamsck, unsigned char nlen,unsigned char *indata,unsigned char *outdata, unsigned short int *sam_sw)
{
    unsigned char len;
    unsigned char ApduMesg[256];
	unsigned short int sw;

    ApduMesg[0] =nsamsck;  //socket
    ApduMesg[1] = nlen;//
    memcpy(ApduMesg+2,indata,nlen);
    ioctl(g_hostsam,SAMCMD,ApduMesg);
    len = ApduMesg[0] ;
    if (len >= 2)
    {
        memcpy(outdata,ApduMesg+1,len);
        sw = outdata[len-2];
        sw <<= 8;
        sw += outdata[len-1];
        *sam_sw = sw;
//        dbg_formatvar("sam_len=%d",len);
    }
    else
    {
    	sw = 0xFFFF;
    	len = 0;
        *sam_sw = sw;
    }

    return len;

}

char sam_version(unsigned char *version)
{
	char status= 0 ;
	uchar data[5];
	//rc531_switch(0);
	status = ioctl(g_hostsam, SAMVERSION, version);
	//memcpy(version,data,sizeof(data));

	return status;
	
}

