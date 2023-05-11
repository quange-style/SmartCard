#include "Extra.h"
#include "../link/sam.h"
#include "CmdSort.h"
#include <string.h>
#include <unistd.h>
#include "../ble/ble.h"
#include "Records.h"

SAM_FACTOR Extra::sam_factor	= {0};

pthread_t Extra::sam_handle		= 0;
sem_t Extra::sam_semaphore		= {{0}};

pthread_t Extra::search_handle	= 0;
sem_t Extra::search_semaphore	= {{0}};

BLE_FACTOR Extra::ble_factor;

pthread_t Extra::ble_handle		= 0;
sem_t Extra::ble_semaphore		= {{0}};

pthread_t Extra::search_ble_handle	= 0;
sem_t Extra::search_ble_semaphore	= {{0}};

Extra::Extra(void)
{

}

Extra::~Extra(void)
{

}

bool Extra::watch_start()
{
	bool ret = false;

	do
	{
		if (sem_init(&sam_semaphore, 0, 0) != 0)
			break;

		if (pthread_create(&sam_handle, NULL, sam_seq_proc, NULL) != 0)
			break;

		//if (sem_init(&search_semaphore, 0, 0) != 0)
		//	break;

		//if (pthread_create(&search_handle, NULL, search_proc, NULL) != 0)
		//	break;

		ret = true;

	} while (0);

	return ret;
}

void Extra::watch_end()
{
	void * p_thread_result;

	sem_post(&sam_semaphore);
	pthread_join(sam_handle, &p_thread_result);
	sem_destroy(&sam_semaphore);

	//sem_post(&search_semaphore);
	//pthread_join(search_handle, &p_thread_result);
	//sem_destroy(&search_semaphore);
}

void Extra::sam_seq_add(int socket_id, unsigned char * p_data, unsigned char len)
{
	sam_factor.sam_socket = socket_id;
	sam_factor.len_data = len;
	memcpy(sam_factor.data_send, p_data, len);

	sem_post(&sam_semaphore);
}

void * Extra::sam_seq_proc(void * arg)
{
	unsigned char u_recv[64]	= {0};
	unsigned short response_sw	= 0;

	while (CmdSort::m_flag_app_run)
	{
		if (sem_wait(&sam_semaphore) == 0)
		{
			usleep(1000);
			sam_cmd(sam_factor.sam_socket,sam_factor.len_data, sam_factor.data_send, u_recv, &response_sw);
		}
	}

	return NULL;
}

void * Extra::search_proc(void * arg)
{
	while (CmdSort::m_flag_app_run)
	{
		if (sem_wait(&search_semaphore) == 0)
		{

		}
	}

	return NULL;
}


bool Extra::watch_ble_start()
{
	bool ret = false;

	do
	{
		if (sem_init(&ble_semaphore, 0, 0) != 0)
			break;

		if (pthread_create(&ble_handle, NULL, ble_seq_proc, NULL) != 0)
			break;

		//if (sem_init(&search_semaphore, 0, 0) != 0)
		//	break;

		//if (pthread_create(&search_handle, NULL, search_proc, NULL) != 0)
		//	break;

		ret = true;

	} while (0);

	return ret;
}

void Extra::watch_ble_end()
{
	void * p_thread_result;

	sem_post(&ble_semaphore);
	pthread_join(ble_handle, &p_thread_result);
	sem_destroy(&ble_semaphore);

	//sem_post(&search_semaphore);
	//pthread_join(search_handle, &p_thread_result);
	//sem_destroy(&search_semaphore);
}

void Extra::ble_seq_add(ETYTKOPER operType,uint8_t * passcode, uint8_t * p_eTicket_info, int lengthData)
{
	ble_factor.operType = operType;
	ble_factor.lengthData = lengthData;
	memcpy(ble_factor.passcode, passcode, 32);
	memcpy(ble_factor.p_eTicket_info, p_eTicket_info, lengthData);

	sem_post(&ble_semaphore);
}

void * Extra::ble_seq_proc(void * arg)
{
	unsigned char u_recv[64]	= {0};
	unsigned short response_sw	= 0;

	while (CmdSort::m_flag_app_run)
	{
		if (sem_wait(&ble_semaphore) == 0)
		{
			usleep(1000);
			//其他线程中不能使用日志打印，否则会导致内存泄露
			//g_Record.log_out(0, level_error, "ble_seq_proc start");
			ble::ble_all_oper(ble_factor.operType,ble_factor.passcode, ble_factor.p_eTicket_info, ble_factor.lengthData);
		}
	}

	return NULL;
}

void * Extra::search_ble_proc(void * arg)
{
	while (CmdSort::m_flag_app_run)
	{
		if (sem_wait(&search_ble_semaphore) == 0)
		{

		}
	}

	return NULL;
}
