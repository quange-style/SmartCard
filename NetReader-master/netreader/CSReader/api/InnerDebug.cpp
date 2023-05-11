#include "InnerDebug.h"
#include "../link/linker.h"
#include <string.h>
#include "CmdSort.h"
#include "Api.h"
#include "Errors.h"
#include "Records.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdarg.h>
#include<unistd.h>

unsigned char WriteData[36] =
{
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
	0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
	0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
	0xff,0xee,0xdd,0xcc
};

InnerDebug::InnerDebug(void)
{
}


InnerDebug::~InnerDebug(void)
{
}

void InnerDebug::debug_calling(uint8_t debug_func_addr, uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data, bool& valid_app_addr)
{
	static DEBUG_FUNC_LIST fun_list[] =
	{
		{0x0000, debug_test},
		{0x0001, debug_test_kk},
		{0x0002, debug_rf_version},
		//
		//iso14443A
		//
		{0x0003, debug_14443a_init},
		{0x0004, debug_14443a_request},
		{0x0005, debug_14443a_anticoll},
		{0x0006, debug_14443a_select},
		{0x0007, debug_14443a_read},
		{0x0008, debug_14443a_write},
		{0x0009, debug_14443a_auth},
		//
		// ul
		//
		{0x000a, debug_ul_request},
		{0x000b, debug_ul_anticoll},
		{0x000c, debug_ul_select},
		{0x000d, debug_ul_read},
		{0x000e, debug_ul_write},
		{0x000f, debug_ul_cWrite},
		//
		// cpu
		//
		{0x0010, debug_MifareProRst},
		{0x0011, debug_MifareProCom},
		//
		// sam
		//
		{0x0012, debug_sam_setbaud},
		{0x0013, debug_sam_rst},
		{0x0014, debug_sam_cmd},
		//
		//
		//
		{0x0015, debug_test_once},
		{0x00ff, debug_put_reader_data}

	};

	for (uint16_t i=0; i<sizeof(fun_list)/sizeof(fun_list[0]); i++)
	{
		if (debug_func_addr == fun_list[i].addr_func)
		{
			fun_list[i].debug_fun_ptr(param_stack, data_to_send, len_data);
			valid_app_addr = true;
			return;
		}
	}

	valid_app_addr = false;
}

void InnerDebug::debug_test(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	char status;
	unsigned char sztmp[50];
	unsigned char snr[20];

	len_data = 0;
	unsigned char ant_type = param_stack[0];

	Beep(100);

	while(1)
	{

		ISO14443A_Init(ant_type);
		status = ISO14443A_Request_UL(0x52,sztmp);
		if (sztmp[0] == 0x44)
		{
			status = ISO14443A_Anticoll_UL(0x93,sztmp);
			if (status == 0)
			{
				memcpy(snr,sztmp,5);
				status = ISO14443A_Select_UL(0x93,snr,sztmp);
				if (status == 0)
				{
					status = ISO14443A_Anticoll_UL(0x95,sztmp);
					if (status == 0)
					{
						memcpy(snr,sztmp,5);
						status = ISO14443A_Select_UL(0x95,snr,sztmp);
						if (status == 0)
						{
							status = ISO14443A_Read_UL(0x08,sztmp);
							if (status == 0)
								Beep(10);
						}
					}
				}
			}
		}
		else
		{

			status=ISO14443A_Anticoll(snr);

			if (status==0)
			{
				status=ISO14443A_Select(snr, sztmp);
				if (status == 0)
					Beep(10);
			}

		}
	}

}

void InnerDebug::debug_test_kk(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	char status;
	unsigned char sztmp[256];
	unsigned char data[256];
	unsigned char snr[5];
	unsigned long pageUL = 0;
	unsigned char len = 0;
	//unsigned char ulen			= 0;
	unsigned char key[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	unsigned short SWRpd = 0;

	unsigned char ant_type = param_stack[0];

	Beep(100);

	while(1){

		//UL

		ISO14443A_Init(ant_type);
		memset(sztmp,0,sizeof(sztmp));

		status = ISO14443A_Request_UL(0x52,sztmp);
		if (status != 0)
			continue;

		if(sztmp[0] == 0x44)
		{
			pageUL %= 8;
			status = ISO14443A_Anticoll_UL(0x93,sztmp);
			if (status == 0)
			{
				memcpy(snr,sztmp,5);
				status = ISO14443A_Select_UL(0x93,snr,sztmp);
				if (status == 0)
				{
					status = ISO14443A_Anticoll_UL(0x95,sztmp);
					if (status == 0)
					{
						memcpy(snr,sztmp,5);
						status = ISO14443A_Select_UL(0x95,snr,sztmp);
						if (status == 0)
						{
							status = ISO14443A_Write_UL(8+pageUL,&WriteData[4 * pageUL]);
							ISO14443A_Read_UL(8+pageUL,sztmp);

							if(memcmp(sztmp,&WriteData[4 * pageUL],4)==0)
								Beep(30);
							else
								Beep(200);

							pageUL++;
						}
					}
				}
			}
		}
		else if ((sztmp[0] == 0x04)||(sztmp[0] == 0x02)||(sztmp[0] == 0x08))
		{
			status = ISO14443A_Anticoll(sztmp);
			if (status == 0)
			{
				memcpy(snr,sztmp,4);
				status = ISO14443A_Select(snr,sztmp);
				if((status==0)&&((sztmp[0]&0x20)==0x20))
				{
					len = MifareProRst(0x40,len,sztmp);
					if(len > 0)
					{
						len = 7;
						memcpy(data,"\x00\xA4\x00\x00\x02\x3F\x00",7);
						len = MifareProCom(len,data,&SWRpd);
						if((len > 0)&&(SWRpd == 0x9000))
							Beep(30);
						else Beep(200);
					}
					else Beep(200);


				}
				else
				{
					status = ISO14443A_Authentication(0X60,snr,key,0x09);
					if(status==0)
					{
						status = ISO14443A_WriteBlock(0x09,WriteData);
						status = ISO14443A_ReadBlock(0x09,data);
						if(memcmp(WriteData,data,16)==0)
							Beep(30);
						else Beep(200);

					}
				}
			}
		}
	}
}

void InnerDebug::debug_test_once(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	char status;
	unsigned char sztmp[256];
	unsigned char data[256];
	unsigned char snr[5];
	unsigned long pageUL = 0;
	unsigned char len = 0;
	unsigned char key[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	unsigned short SWRpd = 0;

	unsigned char ant_type = param_stack[0];
	RETINFO ret = {0};

	ISO14443A_Init(ant_type);
	memset(sztmp,0,sizeof(sztmp));

	do{
		ret.wErrCode = ERR_CARD_NONE;

		//UL
		status = ISO14443A_Request_UL(0x52,sztmp);
		if (status != 0)	break;

		if(sztmp[0] == 0x44)
		{
			pageUL %= 8;
			status = ISO14443A_Anticoll_UL(0x93,sztmp);
			if (status != 0)	break;

			memcpy(snr,sztmp,5);
			status = ISO14443A_Select_UL(0x93,snr,sztmp);
			if (status != 0)	break;

			status = ISO14443A_Anticoll_UL(0x95,sztmp);
			if (status != 0)	break;

			memcpy(snr,sztmp,5);
			status = ISO14443A_Select_UL(0x95,snr,sztmp);
			if (status != 0)	break;

			status = ISO14443A_Write_UL(8+pageUL,&WriteData[4 * pageUL]);
			if (status != 0)	break;

			status = ISO14443A_Read_UL(8+pageUL,sztmp);
			if (status != 0)	break;

			if(memcmp(sztmp,&WriteData[4 * pageUL],4) != 0)
			{
				Beep(200);
				break;
			}

			pageUL++;
			ret.wErrCode = 0;
			break;
		}

		if ((sztmp[0] == 0x04)||(sztmp[0] == 0x02)||(sztmp[0] == 0x08))
		{
			status = ISO14443A_Anticoll(sztmp);
			if (status != 0)	break;

			memcpy(snr,sztmp,4);
			status = ISO14443A_Select(snr,sztmp);
			if (status != 0)	break;

			if((sztmp[0] & 0x20) == 0x20)	//CPU
			{
				len = MifareProRst(0x40,len,sztmp);
				if(len <= 0)
				{
					Beep(200);
					break;
				}

				len = 7;
				memcpy(data,"\x00\xA4\x00\x00\x02\x3F\x00",7);
				len = MifareProCom(len,data,&SWRpd);
				if(len <= 0 || SWRpd != 0x9000)
				{
					Beep(200);
					break;
				}

				ret.wErrCode = 0;
				break;
			}

			// MF
			status = ISO14443A_Authentication(0X60,snr,key,0x09);
			if(status != 0)	break;

			status = ISO14443A_WriteBlock(0x09,WriteData);
			if(status != 0)	break;

			status = ISO14443A_ReadBlock(0x09,data);
			if(status != 0)	break;

			if(memcmp(WriteData, data, 16) != 0)
			{
				Beep(200);
				break;
			}
			ret.wErrCode = 0;
			break;
		}

	} while(0);

	len_data = Api::organize_data_region(data_to_send, &ret);

}

void InnerDebug::debug_rf_version(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	unsigned char uc_version[8] = {0};
	uint16_t len_version = 0;

	ret.wErrCode = rf_version(uc_version);
	if (ret.wErrCode == 0)
		len_version = 1;

	len_data = Api::organize_data_region(data_to_send, &ret, len_version, uc_version);
}

//
//iso14443A
//
void InnerDebug::debug_14443a_init(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};

	unsigned char ant_type = param_stack[0];

	ret.wErrCode = ISO14443A_Init(ant_type);

	len_data = Api::organize_data_region(data_to_send, &ret);
}

void InnerDebug::debug_14443a_request(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	unsigned char req_result[8] = {0};
	uint16_t len_result = 0;

	ret.wErrCode = ISO14443A_Request(param_stack[0], req_result);
	if (ret.wErrCode == 0)
		len_result = 2;

	len_data = Api::organize_data_region(data_to_send, &ret, len_result, &req_result);
}

void InnerDebug::debug_14443a_anticoll(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	unsigned char atc_result[32] = {0};
	uint16_t len_result = 0;

	ret.wErrCode = ISO14443A_Anticoll(atc_result);
	if (ret.wErrCode == 0)
		len_result = 4;

	len_data = Api::organize_data_region(data_to_send, &ret, len_result, &atc_result);
}

void InnerDebug::debug_14443a_select(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	unsigned char size_select = 0;

	ret.wErrCode = ISO14443A_Select(param_stack, &size_select);

	len_data = Api::organize_data_region(data_to_send, &ret, 1, &size_select);
}

void InnerDebug::debug_14443a_read(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	unsigned char bock_data[16] = {0};
	uint16_t len_bock = 0;

	ret.wErrCode = ISO14443A_ReadBlock(param_stack[0], bock_data);
	if (ret.wErrCode == 0)
		len_bock = 16;

	len_data = Api::organize_data_region(data_to_send, &ret, len_bock, bock_data);
}

void InnerDebug::debug_14443a_write(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};

	ret.wErrCode = ISO14443A_WriteBlock(param_stack[0], param_stack + 1);

	len_data = Api::organize_data_region(data_to_send, &ret);
}

void InnerDebug::debug_14443a_auth(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};

	ret.wErrCode = ISO14443A_Authentication(param_stack[0], param_stack + 2, param_stack + 6, param_stack[1]);

	len_data = Api::organize_data_region(data_to_send, &ret);
}
//
// ul
//
void InnerDebug::debug_ul_request(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	unsigned char req_result[8] = {0};
	uint16_t len_result = 0;

	ret.wErrCode = ISO14443A_Request_UL(param_stack[0], req_result);
	if (ret.wErrCode == 0)
		len_result = 2;

	len_data = Api::organize_data_region(data_to_send, &ret, len_result, &req_result);
}

void InnerDebug::debug_ul_anticoll(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	unsigned char atc_result[8] = {0};
	uint16_t len_result = 0;

	ret.wErrCode = ISO14443A_Anticoll_UL(param_stack[0], atc_result);
	if (ret.wErrCode == 0)
		len_result = 5;

	len_data = Api::organize_data_region(data_to_send, &ret, len_result, &atc_result);
}

void InnerDebug::debug_ul_select(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	unsigned char size_select = 0;

	ret.wErrCode = ISO14443A_Select_UL(param_stack[0], param_stack + 1, &size_select);

	len_data = Api::organize_data_region(data_to_send, &ret);
}

void InnerDebug::debug_ul_read(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	unsigned char uc_read[16] = {0};
	uint16_t len_read = 0;

	ret.wErrCode = ISO14443A_Read_UL(param_stack[0], uc_read);
	if (ret.wErrCode == 0)
		len_read = 16;

	len_data = Api::organize_data_region(data_to_send, &ret, len_read, &uc_read);
}

void InnerDebug::debug_ul_write(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};

	ret.wErrCode = ISO14443A_Write_UL(param_stack[0], param_stack + 1);

	len_data = Api::organize_data_region(data_to_send, &ret);
}

void InnerDebug::debug_ul_cWrite(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};

	ret.wErrCode = ISO14443A_CWrite_UL(param_stack[0], param_stack + 1);

	len_data = Api::organize_data_region(data_to_send, &ret);
}
//
// cpu
//
void InnerDebug::debug_MifareProRst(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	unsigned char uc_cmd[512] = {0};
	uint16_t len_cmd = 0;

	len_cmd = MifareProRst(param_stack[0], 0, uc_cmd);
	if (len_cmd > 0)
		ret.wErrCode = 0;
	else
		ret.wErrCode = 0xFE;

	len_data = Api::organize_data_region(data_to_send, &ret, len_cmd, uc_cmd);
}

void InnerDebug::debug_MifareProCom(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	unsigned char uc_send_recv[512] = {0};
	unsigned short us_state = 0;
	uint16_t len_recv = 0;

	memcpy(uc_send_recv, param_stack + 1, param_stack[0]);
	len_recv = MifareProCom(param_stack[0], uc_send_recv, &us_state);
	if (len_recv > 0)
	{
		ret.wErrCode = 0;
	}

	len_data = Api::organize_data_region(data_to_send, &ret, len_recv, uc_send_recv);
}

void InnerDebug::debug_sam_setbaud(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};

	/*ret.wErrCode = */sam_setbaud(param_stack[0], param_stack[1]);

	len_data = Api::organize_data_region(data_to_send, &ret);
}

void InnerDebug::debug_sam_rst(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	unsigned char uc_recv[512] = {0};
	uint16_t len_recv = 0;

	len_recv = sam_rst(param_stack[0], uc_recv);
	if (len_recv > 0)
		ret.wErrCode = 0;
	else
		ret.wErrCode = 0xFF;

	len_data = Api::organize_data_region(data_to_send, &ret, len_recv, uc_recv);
}

void InnerDebug::debug_sam_cmd(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	unsigned char uc_recv[512] = {0};
	uint16_t len_recv = 0;
	unsigned short us_state = 0;

	len_recv = sam_cmd(param_stack[0], param_stack[1], param_stack + 2, uc_recv, &us_state);
	if (len_recv > 0)
		ret.wErrCode = 0;
	else
		ret.wErrCode = 0xFF;

	len_data = Api::organize_data_region(data_to_send, &ret, len_recv, uc_recv);
}

void InnerDebug::debug_put_reader_data(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret							= {0};
	char sz_share_name[_POSIX_PATH_MAX]	= {0};
	char sz_save_name[_POSIX_PATH_MAX]	= {0};
	char sz_cmd[512]					= {0};
	char sz_mount[_POSIX_PATH_MAX]		= {0};
	char sz_save[_POSIX_PATH_MAX]		= {0};


	do
	{
		memcpy(sz_share_name, param_stack, 16);
		memcpy(sz_save_name, param_stack + 16, 16);

		// 检查共享目录下的挂载目录是否存在，如果不存在就挂载
		sprintf(sz_mount, "/mnt/share");
		if (access(sz_mount, 0) != 0 && mkdir(sz_mount, 0755) != 0)
		{
			ret.wErrCode = ERR_UNDEFINED;
			g_Record.log_out(ret.wErrCode, level_error, "directory %s access denied", sz_mount);
			break;
		}

		// 如果目录不为空，说明已经mount了，否则需要重新mount
		if (Records::folder_null(sz_mount))
		{
			sprintf(sz_cmd, "mount 192.168.1.101:/%s %s", sz_share_name, sz_mount);
			if (system(sz_cmd) != 0)
			{
				ret.wErrCode = ERR_UNDEFINED;
				g_Record.log_out(ret.wErrCode, level_error, "%s failed", sz_cmd);
				break;
			}
		}

		// 目标目录不存在则创建
		sprintf(sz_save, "%s/%s", sz_mount, sz_save_name);
		if (access(sz_save, 0) != 0 && mkdir(sz_save, 0755) != 0)
		{
			ret.wErrCode = ERR_UNDEFINED;
			g_Record.log_out(ret.wErrCode, level_error, "directory %s access denied", sz_save);
			break;
		}

		// 复制文件到nfs文件夹下面
		sprintf(sz_cmd, "cp -a -r -f /data/* %s", sz_save);
		if (system(sz_cmd) != 0)
		{
			ret.wErrCode = ERR_UNDEFINED;
			g_Record.log_out(ret.wErrCode, level_error, "%s failed", sz_cmd);
			break;
		}

	} while (0);

	len_data = Api::organize_data_region(data_to_send, &ret);
}
