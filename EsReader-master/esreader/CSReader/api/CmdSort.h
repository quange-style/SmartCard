#ifndef CMDSORT_H
#define CMDSORT_H

#pragma once
#include <stdint.h>
#include "libserial.h"
#include "Structs.h"
#include "Api.h"

#define COM1				(char *)"/dev/ttyS0"
#define COM2				(char *)"/dev/ttyS1"

#define SIZE_MAX_RECV		10240

#define COMMAND_A			(uint8_t)1
#define COMMAND_B			(uint8_t)2
#define COMMAND_C			(uint8_t)3
#define COMMAND_D			(uint8_t)4

#define COMMAND_DEBUG		(uint8_t)0x10

class CmdSort
{
public:

    CmdSort();

    virtual ~CmdSort();

	int Serial_Listening();

public:
	static bool m_flag_app_run;

	static bool m_sys_reboot;

	static uint8_t m_time_now[7];

	static uint8_t m_antenna_mode;

	static uint8_t m_beep_counter;

	static void cmd_send_ok();

	static void cmd_send(uint8_t * p_cmd_recved, uint8_t * p_data_send, uint16_t len_data_send);

protected:

	void cmd_classic(uint8_t * p_cmd_recved, uint8_t * p_data_to_send, uint16_t& lentosend);


};

#endif // CMDSORT_H
