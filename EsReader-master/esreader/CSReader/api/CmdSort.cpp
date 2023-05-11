#include "CmdSort.h"
#include "Download.h"
#include "QFile.h"
#include "DataSecurity.h"
#include "InnerDebug.h"
#include "../link/linker.h"
#include "libserial.h"
#include "Records.h"
#include<sys/reboot.h>
#include <unistd.h>
#include "SysCmd.h"

bool CmdSort::m_flag_app_run		= true;
bool CmdSort::m_sys_reboot			= false;
uint8_t CmdSort::m_time_now[7]		= {0};
uint8_t CmdSort::m_antenna_mode		= 1;
uint8_t CmdSort::m_beep_counter		= 0;

CmdSort::CmdSort()
{

}

CmdSort::~CmdSort()
{

}


//#define _CURRENT_DEBUG
int CmdSort::Serial_Listening()
{
	int received					= 0;
	uint8_t readbuf[SIZE_MAX_RECV]  = {0};
	uint8_t databuf[SIZE_MAX_RECV]	= {0};
	uint16_t len_data				= 0;

	if ((libserial_init((char *)COM1, 115200, 8, 'N', 1, 0, 0)) > 0)
	{
		ubeep(100);
	}

	while(m_flag_app_run)
	{
		memset(readbuf, 0, SIZE_MAX_RECV);
		memset(databuf, 0, SIZE_MAX_RECV);
		received = libserial_recv_package(SIZE_MAX_RECV, 5, readbuf);

		if (received > 0)
		{
			// 如果设备每天都没有重新初始化读写器,那么读写器必须自行清理过期数据
			if (memcmp(m_time_now, readbuf + 7, 4) != 0)
			{
				memcpy(m_time_now, readbuf + 7, 7);
				g_Record.delete_file_overdue();
			}
			else
			{
				memcpy(m_time_now, readbuf + 7, 7);
			}

			m_antenna_mode = readbuf[6] & 0x03;

			cmd_classic(readbuf, databuf, len_data);
			if (len_data > 0)
			{
				cmd_send(readbuf, databuf, len_data);
			}

			if (m_sys_reboot)
			{
				sync(); // 同步磁盘数据,将缓存数据回写到硬盘,以防数据丢失[luther.gliethttp]
				reboot(RB_AUTOBOOT);
			}
		}
	}

	return 0;
}

void CmdSort::cmd_classic(uint8_t * p_cmd_recved, uint8_t * p_data_to_send, uint16_t& lentosend)
{
	bool app_valid = true;

	memcpy(CmdSort::m_time_now, p_cmd_recved + 7, 7);
	CmdSort::m_antenna_mode = (uint8_t)(p_cmd_recved[6] & 0x03);
	CmdSort::m_beep_counter = (uint8_t)((p_cmd_recved[6] >> 2) & 0x03);

	change_antenna(CmdSort::m_antenna_mode);

	switch (p_cmd_recved[3])
	{
	case COMMAND_A:
		{
			Api::api_calling(p_cmd_recved[4], p_cmd_recved + 16, p_data_to_send, lentosend, app_valid);
		}
		break;
	case COMMAND_B:
		{
			Download dl;
			dl.file_mgr(p_cmd_recved, p_data_to_send, lentosend);
		}
		break;
	case COMMAND_C:			// COMMAND_C的处理包含在download_file()中,此处调用应回复重叠
		{
			SysCmd::sys_calling(p_cmd_recved[4], p_cmd_recved + 16, p_data_to_send, lentosend, app_valid);
		}
		break;
	case COMMAND_D:
		break;
	case COMMAND_DEBUG:
		{
			InnerDebug::debug_calling(p_cmd_recved[4], p_cmd_recved + 16, p_data_to_send, lentosend, app_valid);
		}
	}

	Api::App_Addr_invalid(app_valid, p_data_to_send, lentosend);

	if (lentosend > 0)
	{
		RETINFO ret = {0};
		memcpy(&ret, p_data_to_send, sizeof(ret));
		if (ret.wErrCode == 0)
			Beep_Sleep(CmdSort::m_beep_counter);
	}
}

void CmdSort::cmd_send(uint8_t * p_cmd_recved, uint8_t * p_data_send, uint16_t len_data_send)
{
	uint8_t cmd_send[SIZE_MAX_RECV] = {0};

	cmd_send[0] = 0xBB;

	memcpy(cmd_send + 1, p_cmd_recved + 1, 4);
	memcpy(cmd_send + 5, &len_data_send, 2);
	memcpy(cmd_send + 7, p_data_send, len_data_send);

	libserial_send_package(cmd_send, len_data_send + 9);
}

void CmdSort::cmd_send_ok()
{
	uint8_t cmd_send[SIZE_MAX_RECV] = {0};

	cmd_send[0] = 0xBB;

	libserial_send_package(cmd_send, sizeof(RETINFO) + 9);
}
