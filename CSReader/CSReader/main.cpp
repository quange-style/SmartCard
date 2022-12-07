// main.cpp

#include "link/linker.h"
#include "api/QFile.h"
#include "api/TicketUl.h"
#include "api/TicketSvt.h"
#include "api/TicketBus.h"
#include "api/TicketBM.h"
#include "api/TicketJtb.h"
#include "api/TicketTct.h"
#include "api/TicketElecT.h"
#include "api/TicketPostPayElecT.h"
#include "api/TicketSJTElecT.h"
#include "api/TicketThirdPay.h"
#include "api/Records.h"
#include "api/Extra.h"
#include <stdlib.h>
#include <pthread.h>
#include "link/myprintf.h"
#include "api/serial.h"
#include "api/Api.h"
#include <unistd.h>

uint8_t * PTR_TICKET	= NULL;
size_t SIZE_TICKET		= 0;
int g_rfmain_status		= 0;
int g_rfsub_status		= 0;
CmdSort cmd_ble;

bool alloc_ticket_memory()
{
	size_t obiSize[] = {sizeof(TicketUl), sizeof(TicketSvt), sizeof(TicketBM), sizeof(TicketBus), sizeof(TicketTct), sizeof(TicketJtb), sizeof(TicketPostPayElecT),sizeof(TicketSJTElecT),sizeof(TicketThirdPay),sizeof(TicketElecT)};

	for (uint32_t i=0;i<sizeof(obiSize)/sizeof(obiSize[0]);i++)
	{
		if (obiSize[i] > SIZE_TICKET)
			SIZE_TICKET = obiSize[i];
	}

	SIZE_TICKET += 1024;
	PTR_TICKET = new(std::nothrow) uint8_t[SIZE_TICKET];

	return (PTR_TICKET != NULL);
}

void free_ticket_memory()
{
	delete []PTR_TICKET;
}

void clear_tmp_folder()
{
	char sys_cmd[512] = {0};
	sprintf(sys_cmd, "rm -rf %s/%s/*", QFile::running_directory(), NAME_TEMP_FOLDER);
	system(sys_cmd);
}

void *FunMonitorQR(void *ptr)
{
	//dbg_formatvar("FunMonitorQR");
	CmdSort cmd1;
	cmd1.Serial_ListeningQR();
	usleep(100);
    return NULL;
}

int main()
{
	int ret = 0;
	CmdSort cmd;
	//CmdSort cmdBle;

	int received					= 0;
	void *pdata;
	HS_LOG("runing\n");
	do
	{
	
		HS_LOG("init_running_directory \n");
		QFile::init_running_directory();

		clear_tmp_folder();

		g_Parameter.load_dat_config();
		g_Parameter.load_itp_dat_config();

		HS_LOG("rfdev_init \n");

		while (rfdev_init() < 0)
		{
		
			HS_LOG("Beep_Sleep \n");
			Beep_Sleep(1, 2000, 10000 * 1000);
		}

		g_rfmain_status = rfdev_get_rfstatus(1);
		g_rfsub_status |= rfdev_get_rfstatus(2);
		HS_LOG("samdev_init \n");

		if (samdev_init() < 0)
			break;

		HS_LOG("init_sam_inf \n");

        init_sam_inf();
		HS_LOG("init_sam_inf success \n");

		Beep_Sleep(1);
		HS_LOG("alloc_ticket_memory \n");

		if (!alloc_ticket_memory())
			break;

		g_Record.error_level_sort();
		g_Record.record_trade_start();
		Extra::watch_start();

		g_Record.log_out(ret, level_error, "reader running......");

		pthread_t thrd1;
		HS_LOG("FunMonitorQR \n");

		//pthread_create(&thrd1,NULL,FunMonitorQR,NULL);

		/*
		        long msec = 0;
		long time_use=0;
		struct timeval start;
		struct timeval end;
		gettimeofday(&start,NULL);*/
		HS_LOG("Serial_ListeningBLE \n");

		//cmd_ble.Serial_ListeningBLE(115200);//115200
		/*
				gettimeofday(&end,NULL);
		time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//Î¢Ãë
		msec = time_use / 1000;
		dbg_formatvar("msec:%ld",msec);
		*/

		cmd.Serial_Listening();

		g_Record.record_trade_end();
		Extra::watch_end();

		free_ticket_memory();

	} while (0);

	g_Record.log_out(ret, level_error, "reader exit......");

	return ret;
}
