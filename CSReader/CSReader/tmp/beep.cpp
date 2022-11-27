#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/ioctl.h>
#include<sys/ioctl.h>
#include <fcntl.h>
#include "beep.h"


#define BEEP_IOC_MAGIC 'B'
#define BEEP		_IO(BEEP_IOC_MAGIC, 0)

static int fd = -1;



static void open_buzzer(void)
{
	if(fd<0)
	{
		fd = open("/dev/beep0", 0);
		if (fd < 0)
		{
			//perror("open pwm_buzzer device");
			return;
		}
	}
}

	// any function exit call will stop the buzzer
	//atexit(close_buzzer);
}



static void set_buzzer_freq(unsigned char freq)
{
	// this IOCTL command is the key to set frequency
	 unsigned char temp;

	temp = freq;
	int ret = ioctl(fd, BEEP, &freq);
	//printf("beep open is ok%02x\n",ret);
	if(ret < 0)
	{
		//perror("set the frequency of the buzzer");
		exit(1);
	}
}


static void Beep(int freq)
{
	//return ;
	if (open_buzzer()>0)
	{
		set_buzzer_freq(freq);
	}
	//usleep(50000);
	//stop_buzzer();
	//close_buzzer();

}
