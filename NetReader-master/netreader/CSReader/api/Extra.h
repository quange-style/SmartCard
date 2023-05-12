//　本类主要做一些卡操作前的准备工作或者善后工作

#pragma once

#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include "Declares.h"

typedef struct {
	int			sam_socket;
	uint8_t		data_send[64];
	uint8_t		len_data;

}SAM_FACTOR;

typedef struct {
	ETYTKOPER operType;
	uint8_t	passcode[32+1];
	uint8_t	p_eTicket_info[256];
	int lengthData;

}BLE_FACTOR;

class Extra
{
public:
	Extra(void);
	~Extra(void);

	static void sam_seq_add(int socket_id, unsigned char * p_data, unsigned char len);

	static bool watch_start();
	static void watch_end();

	static void ble_seq_add(ETYTKOPER operType,uint8_t * passcode, uint8_t * p_eTicket_info, int lengthData);

	static bool watch_ble_start();
	static void watch_ble_end();


protected:

	static SAM_FACTOR sam_factor;
	static pthread_t sam_handle;
	static sem_t sam_semaphore;

	static pthread_t search_handle;
	static sem_t search_semaphore;

	static void * sam_seq_proc(void * arg);
	static void * search_proc(void * arg);

	////////////////////////////
	///////////////BLE//////////
	static BLE_FACTOR ble_factor;
	static pthread_t ble_handle;
	static sem_t ble_semaphore;

	static pthread_t search_ble_handle;
	static sem_t search_ble_semaphore;

	static void * ble_seq_proc(void * arg);
	static void * search_ble_proc(void * arg);
};
