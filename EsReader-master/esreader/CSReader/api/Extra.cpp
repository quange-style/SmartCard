#include "Extra.h"
#include "../link/sam.h"
#include "CmdSort.h"
#include <string.h>
#include <unistd.h>

SAM_FACTOR Extra::sam_factor	= {0};

pthread_t Extra::sam_handle		= 0;
sem_t Extra::sam_semaphore		= {{0}};

pthread_t Extra::search_handle	= 0;
sem_t Extra::search_semaphore	= {{0}};

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
