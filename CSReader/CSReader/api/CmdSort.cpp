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
#include "../link/myprintf.h"
#include "SerialLocal.h"
#include "../json/writer.h"
#include "../json/JsonCPP.h"
#include "../ble/ble.h"
#include <time.h>


#define _TERMINAL_BOM
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
	int ret;

	
	HS_LOG("libserial_init_local COM1=%s   \n",COM1);
	if ((ret=libserial_init_local((char *)COM1, 115200, 8, 'N', 1, 0, 0)) > 0)
	{
		//dbg_formatvar("libserial_init_COM1");
		
		HS_LOG(" libserial_init_local %s ret=%d  success \n",COM1,ret);
		ubeep(100);
	}
	else{
		HS_LOG(" libserial_init_local %s error ret=%d  \n",COM1,ret);

	}

	//dbg_formatvar("libserial_init");

	while(m_flag_app_run)
	{
		memset(readbuf, 0, SIZE_MAX_RECV);
		memset(databuf, 0, SIZE_MAX_RECV);

		
		//HS_LOG(" begin libserial_recv_package_local \n");
		received = libserial_recv_package_local(SIZE_MAX_RECV, 5, readbuf);
		//HS_LOG(" received=%d \n",received);
		if (received > 0)
		{
			
			dbg_dumpmemory("rcv buf: ",readbuf,received);
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

			memcpy(Api::get_package_time, m_time_now, 7);//记录下被调用接口的时间

			m_antenna_mode = readbuf[6] & 0x03;

			cmd_classic(readbuf, databuf, len_data);
			if (len_data > 0)
			{
			
				dbg_dumpmemory("cmd_send: ",databuf,len_data);
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


//#define _CURRENT_DEBUG
int CmdSort::Serial_ListeningQR()
{
	int received					= 0;
	uint8_t readbuf[SIZE_MAX_RECV_QR]  = {0};
	uint8_t databuf[SIZE_MAX_RECV_QR]	= {0};
	uint16_t len_data				= 0;
	int ret;
	#ifdef _TERMINAL_BOM
	
	HS_LOG(" dev=%s   \n",COM4);
	if ((ret=libserial_init_local((char *)COM4, 115200, 8, 'N', 1, 0, 0)) > 0)
	{
		HS_LOG(" libserial_init_local %s ret=%d  success \n",COM4,ret);
		ubeep(100);
	}
	#else

	
	HS_LOG(" dev=%s   \n",COM3);
	if ((libserial_init_local((char *)COM3, 115200, 8, 'N', 1, 0, 0)) > 0)
	{
		//dbg_formatvar("libserial_init_local");
		ubeep(100);
	}
	#endif

	HS_LOG(" success  \n");

	while(m_flag_app_run)
	{
		memset(readbuf, 0, SIZE_MAX_RECV_QR);
		memset(databuf, 0, SIZE_MAX_RECV_QR);
		received = libserial_recv_package_local(SIZE_MAX_RECV_QR, 5, readbuf);
		dbg_formatvar("received:%d \n",received);
		if (received > 0)
		{
			Api::qr_readbuf[0] = 0x51;
			Api::qr_readbuf[1] = 0x52;

			//memcpy(&Api::qr_readbuf[2], readbuf, received+2);
			//加上时间戳用来去除掉无效的刷码
			memcpy(&Api::qr_readbuf[2], CmdSort::m_time_now, 7);
			memcpy(&Api::qr_readbuf[9], readbuf, received+2);
			Api::qr_readbuf_len = received;
			//dbg_dumpmemory("qr_readbuf:", Api::qr_readbuf, received+9);
			//g_Record.log_buffer("qr_readbuf:", Api::qr_readbuf, received+9);
		}
		usleep(50 * 1000);
	}

	return 0;
}


//#define _CURRENT_DEBUG
int CmdSort::Serial_ListeningBLE(int speed)
{
	int received					= 0;
	uint8_t readbuf[SIZE_MAX_RECV_QR]  = {0};
	uint8_t databuf[SIZE_MAX_RECV_QR]	= {0};
	uint16_t len_data				= 0;

	HS_LOG(" dev=%s   \n",COM2);

	if ((libserial_init_local_ble((char *)COM2, speed, 8, 'N', 1, 0, 0)) > 0)
	{
		dbg_formatvar("speed:%d",speed);
		ubeep(100);
	}

	return 0;
}

int CmdSort::Serial_ListeningTest(int speed)
{
	int received					= 0;
	uint8_t readbuf[SIZE_MAX_RECV_QR]  = {0};
	uint8_t databuf[SIZE_MAX_RECV_QR]	= {0};
	uint16_t len_data				= 0;


	if ((libserial_init_test((char *)COM2, speed, 8, 'N', 1, 0, 0)) > 0)
	{
		dbg_formatvar("speed:%d",speed);
		ubeep(100);
	}

	return 0;
}


int CmdSort::cmd_send_ble(ETBLEOPER operType,uint8_t * p_cmd_recved, uint8_t * p_data_send, uint16_t len_data_send,long * msec)
{
	uint8_t readbuf[SIZE_MAX_RECV] = {0};
	int length = 0;
	int timeout = 0;


	long time_use=0;
	struct timeval start;
	struct timeval end;

	if(!((operType == operBleDownloadReset)||(operType == operBleTest)||(operType == operBleTestOther)))
		libserial_null_read_local_ble();

	libserial_send_package_local_ble(p_data_send, len_data_send);

	gettimeofday(&start,NULL);

	if(operType!=operBleReset)
	{
		timeout = 15;
		length = libserial_recv_package_local_ble(operType,SIZE_MAX_RECV, timeout, readbuf);
	}

	gettimeofday(&end,NULL);

	time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
	*msec = time_use / 1000;
	//dbg_formatvar("recv-msec:%ld",msec);

	g_Record.log_out(0, level_error,"recv data msec:%ld",*msec);

	return length;

}


void CmdSort::cmd_classic(uint8_t * p_cmd_recved, uint8_t * p_data_to_send, uint16_t& lentosend)
{
	bool app_valid = true;

	memcpy(CmdSort::m_time_now, p_cmd_recved + 7, 7);
	CmdSort::m_antenna_mode = (uint8_t)(p_cmd_recved[6] & 0x03);
	CmdSort::m_beep_counter = (uint8_t)((p_cmd_recved[6] >> 2) & 0x03);


	memcpy(Api::testTmpData, p_cmd_recved, 14);

	g_Record.log_buffer("p_cmd_recved() = ", p_cmd_recved, 50);

	Api::antFlag = p_cmd_recved[6];

	change_antenna(CmdSort::m_antenna_mode);

	HS_LOG(" cmdtype=%d  \n",p_cmd_recved[3]);

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
			
			HS_LOG("COMMAND_B p_cmd_recved=%s\n",p_cmd_recved);
			dl.file_mgr(p_cmd_recved, p_data_to_send, lentosend);
		}
		break;
	case COMMAND_C:			// COMMAND_C的处理包含在download_file()中,此处调用应回复重叠
		{
			SysCmd::sys_calling(p_cmd_recved[4], p_cmd_recved + 16, p_data_to_send, lentosend, app_valid);
		}
		break;
	case COMMAND_D:
		{
			Download dl;
			dl.file_mgr_itp(p_cmd_recved, p_data_to_send, lentosend);
		}
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

	dbg_dumpmemory("cmd_send() =",cmd_send,len_data_send+7);

	//g_Record.log_buffer("cmd_send() = ", cmd_send,len_data_send+7);


	//if ((cmd_send[7] != 0x02)&&(cmd_send[4] != 0x02))//寻卡失败不打印，查询SAM卡状态不打印
	{
		g_Record.log_buffer("cmd_send_back() = ", cmd_send,len_data_send+7);
	}


	/*
	Json::Value json_object;
	string aa = JsonCPP::GetRespJson(json_object,json_object);

	int length = aa.length();
	dbg_formatvar("length:%d",length);

	length = aa.size();
	dbg_formatvar("size:%d",length);

	length = strlen(aa.c_str());
	dbg_formatvar("strlen:%d",length);

	dbg_dumpmemory("aa() =",(char *)aa.c_str(),length);*/

	//Json::FastWriter writer;
	//Json::Value json_object;
	//json_object.append


	libserial_send_package_local(cmd_send, len_data_send + 9);
}

void CmdSort::cmd_send_ok()
{
	uint8_t cmd_send[SIZE_MAX_RECV] = {0};

	cmd_send[0] = 0xBB;

	libserial_send_package_local(cmd_send, sizeof(RETINFO) + 9);
}
