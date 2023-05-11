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
#include "api/TicketElecTOffline.h"
#include "api/Records.h"
#include "api/Extra.h"
#include <stdlib.h>
#include <pthread.h>
#include "link/myprintf.h"
#include "api/serial.h"
#include "api/Api.h"

uint8_t * PTR_TICKET	= NULL;
size_t SIZE_TICKET		= 0;
int g_rfmain_status		= 0;
int g_rfsub_status		= 0;
CmdSort cmd_ble;

bool alloc_ticket_memory()
{
	size_t obiSize[] = {sizeof(TicketUl), sizeof(TicketSvt), sizeof(TicketBM), sizeof(TicketBus), 
	sizeof(TicketTct), sizeof(TicketJtb), 
	sizeof(TicketPostPayElecT),sizeof(TicketSJTElecT),sizeof(TicketElecT),
	sizeof(TicketElecTOffline),sizeof(TicketThirdPay)};

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

void *FunMonitor_USB_SCAN(void *ptr)
{
	CmdSort cmd2;
	cmd2.Serial_Listening_USB_SCAN();
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

	do
	{
		QFile::init_running_directory();

		clear_tmp_folder();

		g_Parameter.load_dat_config();
		g_Parameter.load_itp_dat_config();

		while (rfdev_init() < 0)
		{
			Beep_Sleep(1, 2000, 10000 * 1000);
		}

		g_rfmain_status = rfdev_get_rfstatus(1);
		g_rfsub_status |= rfdev_get_rfstatus(2);

		if (samdev_init() < 0)
			break;

        init_sam_inf();

		Beep_Sleep(1);

		if (!alloc_ticket_memory())
			break;

		g_Record.error_level_sort();
		g_Record.record_trade_start();
		Extra::watch_start();
		Extra::watch_ble_start();

		g_Record.log_out(ret, level_error, "reader running......");

		pthread_t thrd1;
		pthread_t thrd2;

		pthread_create(&thrd1,NULL,FunMonitorQR,NULL);
		pthread_create(&thrd2,NULL,FunMonitor_USB_SCAN,NULL);

		cmd_ble.Serial_ListeningBLE(115200);//115200

		cmd.Serial_Listening();

		g_Record.record_trade_end();
		Extra::watch_end();
		Extra::watch_ble_end();

		free_ticket_memory();

	} while (0);

	g_Record.log_out(ret, level_error, "reader exit......");

	return ret;
}
