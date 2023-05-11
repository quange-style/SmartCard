/*
 * =====================================================================================
 *
 *       Filename:  libserial.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011��03��10�� 09ʱ23��25�� UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:
 *        Company:
 *
 * =====================================================================================
 */
#include "SerialLocal.h"
//#include "typedefine.h"

#pragma pack(1)

//���Ľ��սṹ
typedef struct{
	unsigned char	stx;
	unsigned char	node;
	unsigned char	serialno;
	unsigned char	cla;
	unsigned char	ins;
	unsigned char	p1;
	unsigned char	p2;
	unsigned char	tm[7];
	unsigned short	len;//Hi
} TPKGHEADER_RECV, *LPTPKGHEADER_RECV;

//���ķ��ͽṹ
typedef struct{
	unsigned char	stx;
	unsigned char	node;
	unsigned char	serialno;
	unsigned char	cla;
	unsigned char	ins;
	unsigned short	len;//Hi
} TPKGHEADER_SEND, *LPTPKGHEADER_SEND;

#pragma pack()

#define ERR_RECV_TIMEOUT -1 //���ճ�ʱ
#define ERR_RECV_FULL -2 //���ݽ��ջ�������
#define ERR_RECV_PKGCRC -3 //����У���
#define ERR_RECV_CONTINUE -6 //����δ��ɣ�����

#define CLA_FILE_COUNT 3 //�ļ�����ú�������
#define CLA_BUSSINE_COUNT 3 //�ļ�����ú�������

#ifdef _WIN32
//const int speed_arr[] = {CBR_128000, CBR_115200, CBR_57600, CBR_38400, CBR_19200, CBR_9600, CBR_4800, CBR_2400, CBR_1200, CBR_300, 0};
#else
const int speed_arr[] = {B230400, B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300,B0};
#endif
const int name_arr[] = {230400, 115200, 57600, 38400,  19200,  9600, 4800, 2400, 1200,  300, 0};

static int g_fd=-1;

unsigned short int pkg_crc16(const unsigned char *ptr, unsigned int len)
{
	static unsigned short int const crc_ta[16]=
	{
		0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
		0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef
	};

    unsigned short crc;
    unsigned char da;


    crc=0;
    while (len--!=0)
    {
        da=crc>>12;
        crc<<=4;
        crc^=crc_ta[da^(*ptr>>4)];

        da=crc>>12;
        crc<<=4;
        crc^=crc_ta[da^(*ptr&0x0f)];
        ptr++;
    }
    return(crc);
}

//===================================================
//����:listenpackchk
//����:����λ���·������ݽ���У�飬�����ݰ��Ϸ�ʱ�����������ݽ��д����ش�
//����:
//     ��ڲ���:len-���յ������ܳ���, data-���յ���������
//     ���ڲ���:������ֵ=0ʱ��������Ч��len-��ǰ���������ݰ�����(����У����), data-��������
//����ֵ:
//     0-������Ч
//     1-���ݰ�ͷ��
//     -2 - ���Ȳ���
//     -3 - У���
//====================================================
int listenpackchk_local(unsigned int nlen, unsigned char *buffer)
{
//	    uchar *pbuf=(uchar *)buffer;
//	    if (pbuf[nlen-1]==0x0d)
//	        return(0);
//	    else
//	        return(1);
    LPTPKGHEADER_RECV lpbuf=(LPTPKGHEADER_RECV)buffer;
    //uint headerlen_rx=sizeof(TPKGHEADER_RECV);
    //uchar *pcontent_rx=(uchar *)(lpbuf+headerlen_rx);

    unsigned int pkglen;
    unsigned short crc1, crc2;
    int nret;


    do
    {

        if (nlen<sizeof(TPKGHEADER_RECV))
        {
            //�����ǰ���յĳ���С�ڱ��ĳ��������ݰ���δ��������
            nret = ERR_RECV_CONTINUE;
            break;
        }


        pkglen = lpbuf->len;

        if (nlen>=(pkglen+18))
        {

            crc2 = pkg_crc16(buffer,pkglen+16); //У�����ݴӳ���(2byte) ��ʼ��У����(1byte) ǰ(pklen+2-1)
            crc1 = buffer[pkglen+17];
            crc1<<=8;
            crc1 += buffer[pkglen+16];

            if (crc1==crc2)
            {
                nret = 0;
            }
            else
            {
                nret = ERR_RECV_PKGCRC;
            }
        }
        else
        {
            nret = ERR_RECV_CONTINUE;
        }


    }
    while (0);

    return(nret);

}


//===================================================
//����:listenpackchk
//����:����λ���·������ݽ���У�飬�����ݰ��Ϸ�ʱ�����������ݽ��д����ش�
//����:
//     ��ڲ���:len-���յ������ܳ���, data-���յ���������
//     ���ڲ���:������ֵ=0ʱ��������Ч��len-��ǰ���������ݰ�����(����У����), data-��������
//����ֵ:
//     0-������Ч
//     1-���ݰ�ͷ��
//     -2 - ���Ȳ���
//     -3 - У���
//====================================================
int listenpackchk_local_ble(unsigned int nlen, unsigned char *buffer)
{
//	    uchar *pbuf=(uchar *)buffer;
//	    if (pbuf[nlen-1]==0x0d)
//	        return(0);
//	    else
//	        return(1);
    LPTPKGHEADER_RECV lpbuf=(LPTPKGHEADER_RECV)buffer;
    //uint headerlen_rx=sizeof(TPKGHEADER_RECV);
    //uchar *pcontent_rx=(uchar *)(lpbuf+headerlen_rx);

    unsigned int pkglen;
    unsigned short crc1, crc2;
    int nret;


    do
    {

        if (nlen<sizeof(TPKGHEADER_RECV))
        {
            //�����ǰ���յĳ���С�ڱ��ĳ��������ݰ���δ��������
            nret = ERR_RECV_CONTINUE;
            break;
        }


        pkglen = lpbuf->len;

        if (nlen>=(pkglen+18))
        {

            crc2 = pkg_crc16(buffer,pkglen+16); //У�����ݴӳ���(2byte) ��ʼ��У����(1byte) ǰ(pklen+2-1)
            crc1 = buffer[pkglen+17];
            crc1<<=8;
            crc1 += buffer[pkglen+16];

            if (crc1==crc2)
            {
                nret = 0;
            }
            else
            {
                nret = ERR_RECV_PKGCRC;
            }
        }
        else
        {
            nret = ERR_RECV_CONTINUE;
        }


    }
    while (0);

    return(nret);

}


int libserial_save_set_local(struct termios *save_tios)
{
#ifdef _WIN32

#else
	if (tcgetattr(g_fd, save_tios) < 0)
        return -1;
#endif
    return 0;
}

int libserial_save_set_local_ble(struct termios *save_tios)
{
#ifdef _WIN32

#else
	if (tcgetattr(g_fd, save_tios) < 0)
        return -1;
#endif
    return 0;
}

int libserial_re_set_local(struct termios *save_tios)
{
#ifdef _WIN32

#else
	if (tcsetattr(g_fd, TCSANOW, save_tios) < 0)
        return -1;
#endif
    return 0;
}

int libserial_re_set_local_ble(struct termios *save_tios)
{
#ifdef _WIN32

#else
	if (tcsetattr(g_fd_ble, TCSANOW, save_tios) < 0)
        return -1;
#endif
    return 0;
}

//=================================================================================================
// ���ڳ�ʼ��������
// dev�������豸��������1��"/dev/ttyS0"������2��"/dev/ttyS1"���������ơ�
// speed�����ڲ����ʣ�������230400, 115200, 57600, 38400,  19200,  9600, 4800, 2400, 1200, 300, 0��
// parity����żУ�顣ֵΪ'N','E','O','S'��
// databits������λ��ֵ��5��6��7����8��
// stopbits��ֹͣλ��ֵ��1����2��
// hwf��Ӳ�������ơ�1Ϊ�򿪣�0Ϊ�رա�
// swf����������ơ�1Ϊ�򿪣�0Ϊ�رա�
//==================================================================================================
int libserial_init_local(char *dev, int speed, int parity, int databits, int stopbits, int hwf, int swf)
{

    int i	= 0;
	int ret = 0;

#ifdef _WIN32

#else
    struct termios tty;

    g_fd = open(dev, O_RDWR | O_NOCTTY);

    if (g_fd == -1)
    {
        return -1;
    }

    ret = tcgetattr(g_fd, &tty);

    if (ret<0)
    {
        close(g_fd);
        g_fd = -1;
        return -1;
    }
    //���ò�����
    for (i=0; i<(int)(sizeof(speed_arr)/sizeof(int)); i++)
    {
        if (speed == name_arr[i])
        {
            cfsetispeed(&tty, speed_arr[i]);
            cfsetospeed(&tty, speed_arr[i]);
            break;
        }
        if (name_arr[i] == 0)
        {
//            printf("speed %d is not support,set to 9600\n",speed);
            cfsetispeed(&tty, B9600);
            cfsetospeed(&tty, B9600);
        }
    }

    //��������λ
    switch (databits)
    {
    case 5:
        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS5;
        break;
    case 6:
        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS6;
        break;
    case 7:
        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS7;
        break;
    case 8:
    default:
        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
        break;
    }

    //����ֹͣλ
    if (stopbits == 2)
        tty.c_cflag |= CSTOPB;
    else
        tty.c_cflag &= ~CSTOPB;

    //������żУ��
    switch (parity)
    {
        //����żУ��
    case 'n':
    case 'N':
        tty.c_cflag &= ~PARENB;   /* Clear parity enable */
        tty.c_iflag &= ~INPCK;    /* Enable parity checking */
        break;
        //��У��
    case 'o':
    case 'O':
        tty.c_cflag |= (PARODD | PARENB); /* ����Ϊ��Ч��*/
        tty.c_iflag |= INPCK;             /* Disable parity checking */
        break;
        //żУ��
    case 'e':
    case 'E':
        tty.c_cflag |= PARENB;    /* Enable parity */
        tty.c_cflag &= ~PARODD;   /* ת��ΪżЧ��*/
        tty.c_iflag |= INPCK;     /* Disable parity checking */
        break;
        //��Ч�ڡ�����żУ�顱
    case 'S':
    case 's':  /*as no parity*/
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        break;
    default:
        tty.c_cflag &= ~PARENB;   /* Clear parity enable */
        tty.c_iflag &= ~INPCK;    /* Enable parity checking */
        break;
    }

    //����Ӳ��������
    if (hwf)
        tty.c_cflag |= CRTSCTS;
    else
        tty.c_cflag &= ~CRTSCTS;

    /*if (hwf) */
    /*tty.c_cflag |= CIREN;*/

    //�������������
    if (swf)
        tty.c_iflag |= IXON | IXOFF;
    else
        tty.c_iflag &= ~(IXON|IXOFF|IXANY);

    //����ΪRAWģʽ
    tty.c_iflag &= ~(IGNBRK | IGNCR | INLCR | ICRNL | IUCLC |
                     IXANY | IXON | IXOFF | INPCK | ISTRIP);
    tty.c_iflag |= (BRKINT | IGNPAR);
    tty.c_oflag &= ~OPOST;
    tty.c_lflag &= ~(XCASE|ECHONL|NOFLSH);
    tty.c_lflag &= ~(ICANON | ISIG | ECHO);
    tty.c_cflag |= (CLOCAL | CREAD);

    //tty.c_cc[VTIME] = 1;   /*��������ģʽ��������ʱ����λΪ0.1��*/
    //tty.c_cc[VMIN] = 1;   /* 1Ϊ����ģʽ��0Ϊ������ģʽ*/
    tty.c_cc[VTIME] = 2;   /*��������ģʽ��������ʱ����λΪ0.1��*/
    tty.c_cc[VMIN] = 0;   /* 1Ϊ����ģʽ��0Ϊ������ģʽ*/

    //���ô�������
    ret = tcsetattr(g_fd, TCSANOW, &tty);

    if (ret<0)
    {
        close(g_fd);
        g_fd = -1;
        return -1;
    }

    tcflush(g_fd, TCIOFLUSH);
#endif

    return g_fd;
}


//=================================================================================================
// ���ڳ�ʼ��������
// dev�������豸��������1��"/dev/ttyS0"������2��"/dev/ttyS1"���������ơ�
// speed�����ڲ����ʣ�������230400, 115200, 57600, 38400,  19200,  9600, 4800, 2400, 1200, 300, 0��
// parity����żУ�顣ֵΪ'N','E','O','S'��
// databits������λ��ֵ��5��6��7����8��
// stopbits��ֹͣλ��ֵ��1����2��
// hwf��Ӳ�������ơ�1Ϊ�򿪣�0Ϊ�رա�
// swf����������ơ�1Ϊ�򿪣�0Ϊ�رա�
//==================================================================================================
int libserial_init_local_ble(char *dev, int speed, int parity, int databits, int stopbits, int hwf, int swf)
{

    int i	= 0;
	int ret = 0;

#ifdef _WIN32

#else
    struct termios tty;

    g_fd_ble = open(dev, O_RDWR | O_NOCTTY);

    if (g_fd_ble == -1)
    {
        return -1;
    }

    ret = tcgetattr(g_fd_ble, &tty);

    if (ret<0)
    {
        close(g_fd_ble);
        g_fd_ble = -1;
        return -1;
    }
    //���ò�����
    for (i=0; i<(int)(sizeof(speed_arr)/sizeof(int)); i++)
    {
        if (speed == name_arr[i])
        {
            cfsetispeed(&tty, speed_arr[i]);
            cfsetospeed(&tty, speed_arr[i]);
            break;
        }
        if (name_arr[i] == 0)
        {
//            printf("speed %d is not support,set to 9600\n",speed);
            cfsetispeed(&tty, B9600);
            cfsetospeed(&tty, B9600);
        }
    }

    //��������λ
    switch (databits)
    {
    case 5:
        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS5;
        break;
    case 6:
        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS6;
        break;
    case 7:
        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS7;
        break;
    case 8:
    default:
        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
        break;
    }

    //����ֹͣλ
    if (stopbits == 2)
        tty.c_cflag |= CSTOPB;
    else
        tty.c_cflag &= ~CSTOPB;

    //������żУ��
    switch (parity)
    {
        //����żУ��
    case 'n':
    case 'N':
        tty.c_cflag &= ~PARENB;   /* Clear parity enable */
        tty.c_iflag &= ~INPCK;    /* Enable parity checking */
        break;
        //��У��
    case 'o':
    case 'O':
        tty.c_cflag |= (PARODD | PARENB); /* ����Ϊ��Ч��*/
        tty.c_iflag |= INPCK;             /* Disable parity checking */
        break;
        //żУ��
    case 'e':
    case 'E':
        tty.c_cflag |= PARENB;    /* Enable parity */
        tty.c_cflag &= ~PARODD;   /* ת��ΪżЧ��*/
        tty.c_iflag |= INPCK;     /* Disable parity checking */
        break;
        //��Ч�ڡ�����żУ�顱
    case 'S':
    case 's':  /*as no parity*/
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        break;
    default:
        tty.c_cflag &= ~PARENB;   /* Clear parity enable */
        tty.c_iflag &= ~INPCK;    /* Enable parity checking */
        break;
    }

    //����Ӳ��������
    if (hwf)
        tty.c_cflag |= CRTSCTS;
    else
        tty.c_cflag &= ~CRTSCTS;

    /*if (hwf) */
    /*tty.c_cflag |= CIREN;*/

    //�������������
    if (swf)
        tty.c_iflag |= IXON | IXOFF;
    else
        tty.c_iflag &= ~(IXON|IXOFF|IXANY);

    //����ΪRAWģʽ
    tty.c_iflag &= ~(IGNBRK | IGNCR | INLCR | ICRNL | IUCLC |
                     IXANY | IXON | IXOFF | INPCK | ISTRIP);
    tty.c_iflag |= (BRKINT | IGNPAR);
    tty.c_oflag &= ~OPOST;
    tty.c_lflag &= ~(XCASE|ECHONL|NOFLSH);
    tty.c_lflag &= ~(ICANON | ISIG | ECHO);
    tty.c_cflag |= (CLOCAL | CREAD);

    //tty.c_cc[VTIME] = 1;   /*��������ģʽ��������ʱ����λΪ0.1��*/
    //tty.c_cc[VMIN] = 1;   /* 1Ϊ����ģʽ��0Ϊ������ģʽ*/
    tty.c_cc[VTIME] = 2;   /*��������ģʽ��������ʱ����λΪ0.1��*/
    tty.c_cc[VMIN] = 0;   /* 1Ϊ����ģʽ��0Ϊ������ģʽ*/

    //���ô�������
    ret = tcsetattr(g_fd_ble, TCSANOW, &tty);

    if (ret<0)
    {
        close(g_fd_ble);
        g_fd_ble = -1;
        return -1;
    }

    tcflush(g_fd_ble, TCIOFLUSH);
#endif

    return g_fd_ble;
}

int libserial_close_local(void)
{
    int status;

    status = close(g_fd);
    g_fd = -1;

    return(status);
}

int libserial_close_local_ble(void)
{
    int status;

    status = close(g_fd_ble);
    g_fd_ble = -1;

    return(status);
}

void libserial_chang_baud_local(int speed)
{
    int i	= 0;

#ifdef _WIN32

#else
    struct termios tty;

    // ȡ�õ�ǰ���ڲ���
    tcgetattr(g_fd, &tty);

    //���ò�����
    for (i= 0;  i < (int)(sizeof(speed_arr) / sizeof(int));  i++)
    {
        if (speed == name_arr[i])
        {
            cfsetispeed(&tty, speed_arr[i]); // ���벨����
            cfsetospeed(&tty, speed_arr[i]); // ���������
            break;
        }
        if (name_arr[i] == 0)
        {
            printf("speed %d is not support,set to 115200\n",speed);
            cfsetispeed(&tty, B115200);
            cfsetospeed(&tty, B115200);
        }
    }

    // �����µĴ��ڲ���
    tcsetattr(g_fd,TCSANOW,&tty);
#endif
}

void libserial_chang_baud_local_ble(int speed)
{
    int i	= 0;

#ifdef _WIN32

#else
    struct termios tty;

    // ȡ�õ�ǰ���ڲ���
    tcgetattr(g_fd_ble, &tty);

    //���ò�����
    for (i= 0;  i < (int)(sizeof(speed_arr) / sizeof(int));  i++)
    {
        if (speed == name_arr[i])
        {
            cfsetispeed(&tty, speed_arr[i]); // ���벨����
            cfsetospeed(&tty, speed_arr[i]); // ���������
            break;
        }
        if (name_arr[i] == 0)
        {
            printf("speed %d is not support,set to 115200\n",speed);
            cfsetispeed(&tty, B115200);
            cfsetospeed(&tty, B115200);
        }
    }

    // �����µĴ��ڲ���
    tcsetattr(g_fd_ble,TCSANOW,&tty);
#endif
}

void libserial_null_read_local(void)
{
	int rc			= 0;
	char tmp[128]	= {0};

#ifdef _WIN32

#else
    struct termios options;
    struct termios old;

    libserial_save_set_local(&old);

    // ���ó�ʱʱ��
    tcgetattr(g_fd, &options);
    options.c_cc[VTIME]=1;
    tcsetattr(g_fd, TCSANOW, &options);

    while (1)
    {
        rc = read(g_fd, tmp, sizeof(tmp));
        //if (rc != 1) break;
        if (rc <= 0) break;
    }

    libserial_re_set(&old);
#endif
}

void libserial_null_read_local_ble(void)
{
	int rc			= 0;
	char tmp[128]	= {0};

#ifdef _WIN32

#else
    struct termios options;
    struct termios old;

    libserial_save_set_local_ble(&old);

    // ���ó�ʱʱ��
    tcgetattr(g_fd_ble, &options);
    options.c_cc[VTIME]=1;
    tcsetattr(g_fd_ble, TCSANOW, &options);

    while (1)
    {
        rc = read(g_fd_ble, tmp, sizeof(tmp));
        //if (rc != 1) break;
        if (rc <= 0) break;
    }

    libserial_re_set_local_ble(&old);
#endif
}

void libserial_set_rts_local(int st)
{
    int linedata	= 0;

#ifdef _WIN32

#else
	ioctl(g_fd, TIOCMGET, &linedata);

    if (st)
    {
        linedata |= TIOCM_RTS;
    }
    else
    {
        linedata &= ~TIOCM_RTS;
    }

    ioctl(g_fd, TIOCMSET, &linedata);
    usleep(2);

#endif
}

void libserial_set_rts_local_ble(int st)
{
    int linedata	= 0;

#ifdef _WIN32

#else
	ioctl(g_fd_ble, TIOCMGET, &linedata);

    if (st)
    {
        linedata |= TIOCM_RTS;
    }
    else
    {
        linedata &= ~TIOCM_RTS;
    }

    ioctl(g_fd_ble, TIOCMSET, &linedata);
    usleep(2);

#endif
}


int libserial_send_package_local(uchar *buf, const int buflen)
{

    unsigned short crc16;

    // ˢ�¶�д����
    crc16 = pkg_crc16(buf, buflen-2);
    memcpy((char *)(buf+(buflen-2)), (char *)&crc16, 2);
    /*tcflush(fd, TCIOFLUSH);*/
    int realwrite=write(g_fd, buf, buflen);
    if (realwrite==buflen)
    	{
    		return 0;
    	}
    	else
		    return -1;
}

int libserial_send_package_local_ble(uchar *buf, const int buflen)
{

    unsigned short crc16;

    // ˢ�¶�д����
    //crc16 = pkg_crc16(buf, buflen-2);
    //memcpy((char *)(buf+(buflen-2)), (char *)&crc16, 2);
    /*tcflush(fd, TCIOFLUSH);*/
    int realwrite=write(g_fd_ble, buf, buflen);
    if (realwrite==buflen)
    	{
    		return 0;
    	}
    	else
		    return -1;
}

int libserial_recv_package_local(int maxrecv, int timeout, uchar *buf)
{
    int rc = 0/*,record = 0*/;
    //int i = 0;
    int sumlen=0, pos=0/*, len=0*/;
    int maxlimite=maxrecv;//�����ճ��ȣ���ֹ�������
    //uchar c = 0;
    int result = -1;

#ifdef _WIN32

#else
	struct termios old, curr;
    libserial_save_set_local(&old);

    tcgetattr(g_fd, &curr);
    if (timeout<=0)
    	curr.c_cc[VTIME] = 1; //100ms
    else
    	curr.c_cc[VTIME] = timeout; //100ms
    tcsetattr(g_fd, TCSANOW, &curr);

    while (1)
    {

        rc = read(g_fd, buf+pos, maxlimite); // �����ַ�

        if (rc < 1)
        {
            // ��ʱ
            result = ERR_RECV_TIMEOUT;
            break;
        }

        sumlen += rc;
        pos += rc;
        maxlimite -= rc;

        //if (pfun)
        {

            //if (sumlen > 0) result = pfun(sumlen, buf);
            if (sumlen > (int)(sizeof(TPKGHEADER_RECV)))
            {
            	 result = listenpackchk_local(sumlen, buf);


            if ( 0 == result)
            {
                result = sumlen;
                break;
            }
            else if(ERR_RECV_CONTINUE==result)
            {
            	continue;
            }
            else if(ERR_RECV_PKGCRC==result)
            {
            	//����У�����
            	break;
            }
          }
        }

        if ( maxlimite <= 0 )
        {
            result = ERR_RECV_FULL;
            break;
        }


        result = -1;

    }

    libserial_re_set_local(&old);
#endif

    return result;
}

int libserial_recv_package_local_ble(int maxrecv, int timeout, uchar *buf)
{
    int rc = 0/*,record = 0*/;
    //int i = 0;
    int sumlen=0, pos=0/*, len=0*/;
    int maxlimite=maxrecv;//�����ճ��ȣ���ֹ�������
    //uchar c = 0;
    int result = -1;

#ifdef _WIN32

#else
	struct termios old, curr;
    libserial_save_set_local_ble(&old);

    tcgetattr(g_fd_ble, &curr);
    if (timeout<=0)
    	curr.c_cc[VTIME] = 1; //100ms
    else
    	curr.c_cc[VTIME] = timeout; //100ms
    tcsetattr(g_fd_ble, TCSANOW, &curr);

    while (1)
    {

        rc = read(g_fd_ble, buf+pos, maxlimite); // �����ַ�

        if (rc < 1)
        {
            // ��ʱ
            result = ERR_RECV_TIMEOUT;
            break;
        }

        sumlen += rc;
        pos += rc;
        maxlimite -= rc;

        //if (pfun)
        {

            //if (sumlen > 0) result = pfun(sumlen, buf);
            if (sumlen > (int)(sizeof(TPKGHEADER_RECV)))
            {
            	 result = listenpackchk_local_ble(sumlen, buf);


            if ( 0 == result)
            {
                result = sumlen;
                break;
            }
            else if(ERR_RECV_CONTINUE==result)
            {
            	continue;
            }
            else if(ERR_RECV_PKGCRC==result)
            {
            	//����У�����
            	break;
            }
          }
        }

        if ( maxlimite <= 0 )
        {
            result = ERR_RECV_FULL;
            break;
        }


        result = -1;

    }

    libserial_re_set_local_ble(&old);
#endif

    return result;
}