#include <stdio.h>
#include <string.h>
#include <unistd.h> //usleep
#include <sys/io.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <stdarg.h>
#include "../api/Records.h"
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
#define SAMIO				_IO(SAM_IOC_MAGIC, 3)
#define SAMVERSION			_IO(SAM_IOC_MAGIC, 4)

//========================================================
//����:��sam  �豸���г�ʼ��,  ��Ӧ������ʱ����Ҫ��
//��ڲ���
//	                ��
//���ڲ���
//                   ��
//����ֵ
//       0=�ɹ�
//      -1=���豸�������
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
//����:�豸sam  ����������
//��ڲ���
//	 nsamsck=sam����Ӧ����
//    baudrate=sam���������0=9600,1=19200,2=38400,3=57600,4=115200
//���ڲ���
//       ��
//����ֵ
//       ��
//========================================================

void sam_setbaud(unsigned char nsamsck,unsigned char baudrate)
{
    unsigned char tmp[2];


    tmp[0] = nsamsck;
    tmp[1] = baudrate;
    ioctl(g_hostsam,SOCKETSET,tmp);

}


//========================================================
//����:��ָ���ۺŵ�sam  �����и�λ
//��ڲ���
//	 nsamsck=sam����Ӧ����
//���ڲ���
//       saminf=6�ֽڵ�sam  ����
//       lpdata=��λ������Ϣ
//����ֵ
//       0=��λʧ��
//       >0��λ�ɹ��󷵻ص�����
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
//����:����ָ����sam  ��ִ��
//��ڲ���
//	 nsamsck=sam����Ӧ����
//    nlen=Ҫ���͵�ָ���(����ָ��ṹ������)
//    indata=���͵�ָ��������
//���ڲ���
//       outdata=sam��ִ����ɺ󷵻ص�����
//       sam_sw=samִ��״̬��,  һ��0x9000  �ǳɹ�
//����ֵ
//       0=ִ���޷���
//       >0ִ���з��أ�����sam_sw  �ж��Ƿ�ɹ�ִ��
//========================================================
unsigned char sam_cmd(unsigned char nsamsck, unsigned char nlen,unsigned char *indata,unsigned char *outdata, unsigned short int *sam_sw)
{
    unsigned char len;
    unsigned char ApduMesg[256];
	unsigned short int sw;
	int i;


	/*g_Record.log_out(0, level_error, "sam_cmd(indata)");
	for(i=0;i<nlen;i++)
    {
        g_Record.log_out(0, level_error, "sam_cmd(indata=%02x)",indata[i]);
    }*/

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

    /*
	g_Record.log_out(0, level_error, "sam_cmd(outdata)");
    for(i=0;i<len;i++)
    {
        g_Record.log_out(0, level_error, "sam_cmd(outdata=%02x)",outdata[i]);
    }
	*/

    return len;

}

char sam_version(unsigned char *version)
{
	char status= 0 ;
	//uchar data[5];
	//rc531_switch(0);
	status = ioctl(g_hostsam, SAMVERSION, version);
	//memcpy(version,data,sizeof(data));

	return status;

}

void samdev_destory(void)
{
	if (g_hostsam>0)
	{
		close(g_hostsam);
		g_hostsam = -1;

	}
}
