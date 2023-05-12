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


//#define _TERMINAL_BOM
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
		//dbg_formatvar("libserial_init_COM1");
		ubeep(100);
	}

	//dbg_formatvar("libserial_init");

	while(m_flag_app_run)
	{
		memset(readbuf, 0, SIZE_MAX_RECV);
		memset(databuf, 0, SIZE_MAX_RECV);
		received = libserial_recv_package(SIZE_MAX_RECV, 5, readbuf);

		if (received > 0)
		{
			// 每跨一个运营日，检查日志是否大于50M
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

void CmdSort::Serial_Listening_USB_SCAN(){
	TYPE_QR_COM beforeCom = TYPE_QR_COM4;
	TYPE_QR_COM currentCom = TYPE_QR_COM4;
	//监听闸机扫码头以及usb扫码枪串口是否存在，以解决同一个版本可同时在BOM、AGM上使用
	while(m_flag_app_run)
	{
		if (access((char *)COM4, 0) == 0)
		{
			currentCom=TYPE_QR_COM4;
		}
		else{
			currentCom=TYPE_QR_COM_OTHER;
		}

		if(beforeCom!=currentCom)
		{
			libserial_close_local();

			switch (currentCom)
			{
			case TYPE_QR_COM4:
				if ((libserial_init_local((char *)COM4, 115200, 8, 'N', 1, 0, 0)) > 0)
				{
					g_Record.log_out(0, level_normal,"COM4 init suc");
					//ubeep(100);
				}
				break;

			case TYPE_QR_COM_OTHER:
				if ((libserial_init_local((char *)COM3, 115200, 8, 'N', 1, 0, 0)) > 0)
				{
					g_Record.log_out(0, level_normal,"COM3 init suc");
					//ubeep(100);
				}
				break;
			}

		}
		beforeCom = currentCom;
		usleep(1000 * 1000 * 2);//TODO:2s监听一次
	}
}

//#define _CURRENT_DEBUG
int CmdSort::Serial_ListeningQR()
{
	int received					= 0;
	uint8_t readbuf[SIZE_MAX_RECV_QR]  = {0};
	uint8_t databuf[SIZE_MAX_RECV_QR]	= {0};
	uint16_t len_data				= 0;

	if (access((char *)COM4, 0) == 0)
	{
		if ((libserial_init_local((char *)COM4, 115200, 8, 'N', 1, 0, 0)) > 0)
		{
			ubeep(100);
		}
	}
	else{
		if ((libserial_init_local((char *)COM3, 115200, 8, 'N', 1, 0, 0)) > 0)
		{
			ubeep(100);
		}
	}

	while(m_flag_app_run)
	{

		memset(readbuf, 0, SIZE_MAX_RECV_QR);
		memset(databuf, 0, SIZE_MAX_RECV_QR);
		received = libserial_recv_package_local(SIZE_MAX_RECV_QR, 5, readbuf);
		if (received > 0)
		{
			Api::qr_readbuf[0] = 0x51;//QR
			Api::qr_readbuf[1] = 0x52;//QR

			//memcpy(&Api::qr_readbuf[2], readbuf, received+2);
			//加上时间戳用来去除掉无效的刷码
			memcpy(&Api::qr_readbuf[2], CmdSort::m_time_now, 7);
			memcpy(&Api::qr_readbuf[9], readbuf, received+2);
			Api::qr_readbuf_len = received;
			//g_Record.log_buffer("qr_read:", Api::qr_readbuf, received+9, level_normal);
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

	if(!((operType == operBleDownloadReset)||(operType == operBleTest)||(operType == operBleTestOther))){
		libserial_null_read_local_ble();
	}

	libserial_send_package_local_ble(p_data_send, len_data_send);

	gettimeofday(&start,NULL);
	if(operType!=operBleReset)
	{
		timeout = 15;//TODO:蓝牙超时时间
		length = libserial_recv_package_local_ble(operType,SIZE_MAX_RECV, timeout, readbuf);
	}
	gettimeofday(&end,NULL);

	time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
	*msec = time_use / 1000;

	return length;

}


void CmdSort::cmd_classic(uint8_t * p_cmd_recved, uint8_t * p_data_to_send, uint16_t& lentosend)
{
	bool app_valid = true;
	uint16_t dataLen=0;
	uint8_t datalenBuff[2]={0};

	memcpy(CmdSort::m_time_now, p_cmd_recved + 7, 7);
	CmdSort::m_antenna_mode = (uint8_t)(p_cmd_recved[6] & 0x03);
	CmdSort::m_beep_counter = (uint8_t)((p_cmd_recved[6] >> 2) & 0x03);

	memcpy(Api::testTmpData, p_cmd_recved, 14);
	//FIXME:根据实际长度返回打印收到的数据
	memcpy(datalenBuff, p_cmd_recved + 14, 2);//获取收到的报文长度
	dataLen=datalenBuff[1]<<8|datalenBuff[0];
	g_Record.log_buffer("recved:", p_cmd_recved, 16+dataLen,level_error);

	Api::antFlag = p_cmd_recved[6];

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

	//g_Record.log_buffer("cmd_send() = ", cmd_send,len_data_send+7);
	//FIXME:返回无卡可以不打印
	if ((cmd_send[7] != 0x02))//寻卡失败不打印，查询SAM卡状态不打印
	{
		g_Record.log_buffer("sendback:", cmd_send,len_data_send+7,level_error);
	}else{
		g_Record.log_out(0, level_normal,"no card");
	}

	libserial_send_package(cmd_send, len_data_send + 9);
}

void CmdSort::cmd_send_ok()
{
	uint8_t cmd_send[SIZE_MAX_RECV] = {0};

	cmd_send[0] = 0xBB;

	libserial_send_package(cmd_send, sizeof(RETINFO) + 9);
}
