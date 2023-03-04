#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/reboot.h>
#include <sys/ioctl.h>

int g_uart_fd = -1;

#define SOC_UART0 		"/dev/ttymxc0"
#define SOC_UART1 		"/dev/ttymxc1"
//#define SOC_UART1 		"/dev/ttyS1"

#define SOC_UART2 		"/dev/ttymxc2"
#define SOC_UART3 		"/dev/ttymxc3"
#define SOC_UART4 		"/dev/ttymxc4"
#define SOC_UART5 		"/dev/ttymxc5"
#define SOC_UART6 		"/dev/ttymxc6"
#define SOC_UART7 		"/dev/ttymxc7"
#define SOC_UART8 		"/dev/ttymxc8"

/***************************************日志打印定义********************************************/
//日志等级定义
#define ERROR     3
#define WARN 	  4
#define INFO	  6
#define DEBUG     7

#define DEBUG_LEVEL 7

#define DRV_DBG(level, fmt, args...)	do{ if(level < DEBUG_LEVEL)\
	fprintf(stderr, "Driver Api [%s, %d]: " fmt, __FUNCTION__, __LINE__, ##args);\
}while(0)
/**********************************************************************************************/
int  get_your_choice(void)
{
	int choice=-1;
	char cmd[1024];

	scanf("%s",cmd);
	choice=atoi(cmd);

	return choice;	
}

void uart_usage_tip(void)
{
	printf("==========================\n");
	printf("0:SOC ttymxc0 test \n");		
	printf("1:SOC ttymxc1 test \n");	
	printf("2:SOC ttymxc2 test \n");
	printf("3:SOC ttymxc3 test \n");	
	printf("4:AVT ttymxc4 test \n");
	printf("5:AVT ttymxc5 test \n");	
	printf("6:AVT ttymxc6 test \n");
	printf("7:AVT ttymxc7 test \n");
	printf("8:AVT ttymxc8 test \n");
	
			
	printf("==========================\n");

}

int drvapi_uart_open(const char *dev_name)
{
	int fd_serial = -1;
	
	fd_serial = open(dev_name, O_RDWR  |O_NOCTTY | O_NDELAY);   
	if (fd_serial < 0)
	{     
		DRV_DBG(ERROR, "open Serial Port %s error\n", dev_name);
		return -1;		  
	}    
	
	if(fcntl(fd_serial, F_SETFL, 0) < 0) 
	{
		DRV_DBG(ERROR, "fcntl Serial Port %s failed\n", dev_name); 
		return -1;	
	}	


	if(isatty(STDIN_FILENO)==0)    
	{
		DRV_DBG(ERROR, "Serial Port: standard input is not a terminal device\n"); 
		return -1;	
	}

	return fd_serial; 
}


int drvapi_uart_close(int fd)
{
	if(fd != -1)
	{
		close(fd);
		
		return 0;
	}
	else
	{
		return -1;
	}
}


int drvapi_set_uart(int fd, int baud_rate, int nbits, char nevent, int nstop, int block)
{
	struct termios newtio, oldtio;
	
	if(tcgetattr(fd, &oldtio) != 0) 
	{     
		DRV_DBG(ERROR, "Get Serial Port info error, fd=%d\n", fd);  
		return -1;  
	}  
	
	bzero(&newtio, sizeof(newtio));  
	newtio.c_cflag |= CLOCAL | CREAD;  
	newtio.c_cflag &= ~CSIZE;

	switch(nbits)   
	{  
		case 7:     
			newtio.c_cflag |= CS7;    
		break;  
		
		case 8:    
			newtio.c_cflag |= CS8;     
		break; 
		
		default:
			DRV_DBG(ERROR, "incorrect nbits:%d\n", nbits);
			return -1;
		break;
	}    
	
	switch(nevent) 
	{
		case 'O':   
				newtio.c_cflag |= PARENB;     
				newtio.c_cflag |= PARODD;    
				newtio.c_iflag |= (INPCK | ISTRIP);  
		 break;   
		 
		case 'E':     
				newtio.c_iflag |= (INPCK | ISTRIP);   
				newtio.c_cflag |= PARENB;    
				newtio.c_cflag &= ~PARODD;  
		break;  
		
		case 'N':     
				newtio.c_cflag &= ~PARENB;   
		break;  
		
		case 's':
				newtio.c_cflag &= ~PARENB;
				newtio.c_cflag &= ~CSTOPB;
		break;
		default:
			DRV_DBG(ERROR, "incorrect nevent:%x\n", nevent);
			return -1;
		break;
	}

	switch(baud_rate)  
	{  
		case 2400:   
			cfsetispeed(&newtio, B2400);  
			cfsetospeed(&newtio, B2400);    
		break; 
		
		case 4800:   
			cfsetispeed(&newtio, B4800); 
			cfsetospeed(&newtio, B4800);    
		break;   
		
		case 9600:    
			cfsetispeed(&newtio, B9600);   
			cfsetospeed(&newtio, B9600);    
		break;  
		
		case 115200:     
			cfsetispeed(&newtio, B115200); 
			cfsetospeed(&newtio, B115200);  
		break;  
		
		default:   
			DRV_DBG(ERROR, "incorrect baud_rate:%d\n", baud_rate);
			return -1; 
		break; 
	} 

	switch(nstop)  
	{
		case 1:    
			newtio.c_cflag &= ~CSTOPB;   
		break;
		
		case 2:    
			newtio.c_cflag |= CSTOPB;   
		break;	
		
		default:   
			DRV_DBG(ERROR, "incorrect nstop:%d\n", nstop);
			return -1; 
		break; 		
	}

	newtio.c_cc[VTIME] = 1;
	newtio.c_cc[VMIN] = block;
	tcflush(fd,TCIFLUSH);  
	
	if((tcsetattr(fd, TCSANOW, &newtio)) !=0)  
	{     
		DRV_DBG(ERROR, "Set Serial Port info error, fd=%d\n", fd);  
		return -1;  
	}   

	return 0;
}



int drvapi_uart_read(int fd, unsigned char *rd_buf, unsigned int rd_len)
{
	return read(fd, rd_buf, rd_len);
}


int drvapi_uart_write(int fd, unsigned char *wr_buf, unsigned int wr_len)
{
	return write(fd, wr_buf, wr_len);
}


int uart_send_ctl(void)
{
	//RS232
	unsigned char wbuf0[9] = {0x81,0x1,0x6,0x1,0x10,0xd,0x3,0x1,0xff}; //up
    unsigned char wbuf1[9] = {0x81,0x1,0x6,0x1,0x10,0xd,0x3,0x2,0xff}; //down
    unsigned char wbuf2[9] = {0x81,0x1,0x6,0x1,0x10,0xd,0x1,0x3,0xff}; //left
    unsigned char wbuf3[9] = {0x81,0x1,0x6,0x1,0x10,0xd,0x2,0x3,0xff}; //right

	//RS485
	unsigned char wbuf4[7] = {0xff,0x1,0x0,0x08,0x1f,0x1f,0x00}; //up
    unsigned char wbuf5[7] = {0xff,0x1,0x0,0x10,0x1f,0x1f,0x00}; //down
    unsigned char wbuf6[7] = {0xff,0x1,0x0,0x04,0x1f,0x1f,0x00}; //left
    unsigned char wbuf7[7] = {0xff,0x1,0x0,0x02,0x1f,0x1f,0x00}; //right


	unsigned char *sendmsg=NULL;
	int sendlen;
	
	int i=0;
	int wr_cnt=0;
	
	int choice = 0xff;	

	printf("==========================\n");
	printf("0:rs232 up\n");		
	printf("1:rs232 down\n");	
	printf("2:rs232 left\n");
	printf("3:rs232 right\n");
	printf("4:rs485 up\n");		
	printf("5:rs485 down\n");	
	printf("6:rs485 left\n");
	printf("7:rs485 right\n");
	printf("==========================\n");
	
	printf("please input ctl cmd:\n");
	choice=get_your_choice();
	switch(choice)
	{
		case 0:
			sendmsg = wbuf0;
			sendlen = sizeof(wbuf0);
			break;
		case 1:
			sendmsg = wbuf1;
			sendlen = sizeof(wbuf1);
			break;
		case 2:
			sendmsg = wbuf2;
			sendlen = sizeof(wbuf2);
			break;
		case 3:
			sendmsg = wbuf3;
			sendlen = sizeof(wbuf3);
			break;	

		case 4:
			sendmsg = wbuf4;
			sendlen = sizeof(wbuf4);
			break;
		case 5:
			sendmsg = wbuf5;
			sendlen = sizeof(wbuf5);
			break;
		case 6:
			sendmsg = wbuf6;
			sendlen = sizeof(wbuf6);
			break;
		case 7:
			sendmsg = wbuf7;
			sendlen = sizeof(wbuf7);
			break;	
			
		default:
			printf("***************************\n");
			printf("***** incorrect numuber !!!******\n");
			printf("***************************\n");
			break;
	}

	if(sendmsg==NULL || sendlen<0)
	{
		return -1;	
	}

	for(i=0; i<sendlen; i++)
	{
		printf("0x%x ",sendmsg[i]);
	}	
	printf("sendlen %d \n",sendlen);

	printf("Writing to uart...\n");
	if((wr_cnt = drvapi_uart_write(g_uart_fd, &sendmsg[0], sendlen)) > 0)
	{
		printf("write %d bytes \n",wr_cnt);
		usleep(10000);
		return wr_cnt;
	}
	else
	{	
		printf("write error\n");
		return -1;
	}
	
	return 0;
}


int uart_recv_ctl(void)
{
	unsigned   char rcv_buff[1024]="";
	int i=0;
	int rd_cnt=0;
	
	printf("Reading from ptz...\n");
	memset(rcv_buff, 0, 1024);
	if((rd_cnt=drvapi_uart_read(g_uart_fd, rcv_buff, 1024)) < 0)
	{
		printf("read error\n");
		return -1;		
	}
	printf("Read bytes %d\n",rd_cnt);
	
	for(i=0; i<rd_cnt; i++)
	{
		printf("recv buff[%x]:0x%x\n", i, rcv_buff[i]);
	}

	return rd_cnt;
}


int uart_test(unsigned char channel)
{
	char *uart_port=NULL;
	int cmd = 0;

	switch(channel)
	{
		case 0:
			uart_port = SOC_UART0;
			break;
		case 1:
			uart_port = SOC_UART1; 		
			break;
		case 2:
			uart_port = SOC_UART2; 		
			break;
		case 3:
			uart_port = SOC_UART3; 		
			break;			
		case 4:
			uart_port = SOC_UART4;
			break;
		case 5:
			uart_port = SOC_UART5; 		
			break;
		case 6:
			uart_port = SOC_UART6; 		
			break;
		case 7:
			uart_port = SOC_UART7; 		
			break;	
		case 8:
			uart_port = SOC_UART8; 		
			break;	
		default:
			printf("Invalid channel\n");
			break;	
	}

	g_uart_fd=drvapi_uart_open(uart_port);
	if(g_uart_fd < 0)
	{
		printf("open %s error\n", uart_port);
		return -1;		    
	}	
	
	if(drvapi_set_uart(g_uart_fd, 9600, 8, 'N', 1, 5) < 0)
	{    
		printf("set serial error"); 
		drvapi_uart_close(g_uart_fd); 
		return -1;	
	}

			
//	printf("please input cmd:(0:read uart 1:write uart)\n");
//	scanf("%d", &cmd);

//	else if(cmd == 1)
	{
		if(uart_send_ctl() < 0)
		{
			printf("uart_send_ctl error\n");
			return -1;		
		}
	}
//	if(cmd == 0)
	{
		if(uart_recv_ctl() < 0)
		{
			printf("uart_recv_ctl error\n");
			return -1;		
		}
	}

	
	return 0;
}


int main(int argc , char* argv[])
{
	int choice = 0xff;

	while(1)
	{
		uart_usage_tip();
		printf("please input your choice:\n");
		choice = get_your_choice(); 	
		if(uart_test(choice) < 0)
		{
			printf("uart test error\n");	
		}		
		drvapi_uart_close(g_uart_fd);
	}

	return 0;
}

