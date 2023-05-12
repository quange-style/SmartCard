// linker.cpp

#include "linker.h"
#include "../api/Errors.h"
#include "../api/Publics.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../api/Records.h"
#include "myprintf.h"

extern TMETROINF g_metroinf;
unsigned char g_ant_type = 0;


void ubeep(int beep_time)
{
    Beep(beep_time);
}

uint16_t change_antenna(uint8_t mode)
{
    return antenna_switch(mode);
}

uint16_t sam_init(int sock_id, char * p_sam_id, char * p_tml_id)
{
	uint16_t ret			= 0;
    uint8_t sam_id[8]       = {0};
	uint8_t tml_id[8]		= {0};
    char    sz_sam_id[17]   = {0};
	char	sz_tml_id[13]	= {0};
	uint8_t len_sam, len_tml, pos;

	memset(sz_sam_id, '0', 16);
	memset(sz_tml_id, '0', 12);

	if (sock_id == SAM_SOCK_1)
	{
		ret = sam_metro_active(sock_id, &len_sam, sam_id, &len_tml, tml_id);
		if (ret == 0)
		{
			pos = 16 - len_sam * 2;
			memset(sz_sam_id, '0', pos);
			for (uint8_t i=0;i<len_sam;i++,pos+=2)
			{
				sprintf(sz_sam_id + pos, "%02X", sam_id[i]);
			}

			pos = 12 - len_tml * 2;
			memset(sz_tml_id, '0', pos);
			for (uint8_t i=0;i<len_tml;i++,pos+=2)
			{
				sprintf(sz_tml_id + pos, "%02X", tml_id[i]);
			}
		}
	}

	memcpy(p_sam_id, sz_sam_id, 16);
	memcpy(p_tml_id, sz_tml_id, 16);
	dbg_formatvar("p_sam_id:%s",p_sam_id);

    return ret;
}

//=================================================================================
//函 数 名: metro_svt_read
//功    能: 读取地铁票的数据内容
//出口参数:
// p_base包括
//            发行基本信息:40Byte
//            公共信息:30Byte
//            交易辅助信息:48Byte
//            地铁信息:48Byte
//            应用控制信息:32Byte
//            钱包值:4Byte
//p_owner     持卡人个人信息:23Byte
//p_history_last
//			  最近一次历史记录:23Byte
//            10条历史记录:230Byte
//返 回 值:
//        >0: 返回的数据长度
//        <0: 操作出错
//=================================================================================
uint16_t metro_svt_read(uint8_t * p_base, uint8_t * p_owner, uint8_t * p_history_last)
{
	//unsigned char CardNum = 0;
	unsigned char status=0, snr[12];//, snr1[5];
	unsigned char sztmp[256];
	//unsigned char ucTokenCnt=0;
	//unsigned char atqa,firstsnr[7];
	unsigned char ulen = 0;
	unsigned int i;
	unsigned int data_len;
	unsigned short sam_sw;
	int nresult = 0;

	do
	{
		for (i=0; i<3; i++)
		{
			status=ISO14443A_Request( 0x52, sztmp );
			if (status == 0) break;
		}
		if (status)
		{
			nresult = ERR_CARD_NONE;
			break;
		}

		status=ISO14443A_Anticoll(snr);
		if (status)
		{
			nresult = ERR_CARD_NONE;
			break;
		}

		status=ISO14443A_Select(snr, sztmp);
		if (status)
		{
			nresult = ERR_CARD_NONE;
			break;
		}

		status =  MifareProRst(0x40,ulen,sztmp);
		if (status == 0)
		{
			nresult = ERR_CARD_NONE;
			break;
		}

		//add test 2016-02-25
// 		ulen = 19;
// 		memcpy(sztmp, "\x00\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", ulen);
// 		ulen = MifareProCom(ulen, sztmp, &sam_sw);

		ulen = 7;
		memcpy(sztmp, "\x00\xA4\x00\x00\x02\x3F\x00", ulen);
		status = MifareProCom(ulen, sztmp, &sam_sw);
		if ((status==0) || (0x9000!=sam_sw))
		{
			nresult = ERR_CARD_NONE;
			break;
		}

		data_len = 0;

		//
		//发行区信息
		//
		ulen = 5;
		memcpy(sztmp, "\x00\xB0\x85\x00\x28", ulen);
		status = MifareProCom(ulen, sztmp, &sam_sw);
		if ((status==0) || (0x9000!=sam_sw))
		{
			g_Record.log_out(0, level_error, "00B0850028");
			nresult = ERR_CARD_READ;
			break;
		}
		memcpy(g_metroinf.physicalnumber+4, snr, 4);
		memcpy(g_metroinf.logicnumber, sztmp+8, 8);
		g_metroinf.testflag = sztmp[6];
		memcpy(p_base + data_len, sztmp, 40);
		data_len += 40;

		//
		//公共信息区
		//
		ulen = 14;
		memcpy(sztmp, "\x00\xA4\x04\x00\x09\xF0\x00\x00\x41\x00\x53\x20\xAD\xF1", ulen);
		status = MifareProCom(ulen, sztmp, &sam_sw);
		if ((status==0) || (0x9000!=sam_sw))
		{
			g_Record.log_out(0, level_error, "00A4040009F0000041005320ADF1");
			nresult = ERR_CARD_READ;
			break;
		}

		ulen = 5;
		memcpy(sztmp, "\x00\xB0\x95\x00\x1E", ulen);
		status = MifareProCom(ulen, sztmp, &sam_sw);
		if ((status==0) || (0x9000!=sam_sw))
		{
			g_Record.log_out(0, level_error, "00B095001E");
			nresult = ERR_CARD_READ;
			break;
		}
		memcpy(p_base + data_len, sztmp, 30);
		data_len += 30;

		//
		//交易辅助信息
		//
		ulen = 5;
		memcpy(sztmp, "\x00\xB2\x01\xBC\x00", ulen);
		status = MifareProCom(ulen, sztmp, &sam_sw);
		if ((status==0) || (0x9000!=sam_sw))
		{
			g_Record.log_out(0, level_error, "00B201BC00");
			nresult = ERR_CARD_READ;
			break;
		}
		memcpy(p_base + data_len, sztmp, 48);
		data_len += 48;

		//地铁信息区
		ulen = 5;
		memcpy(sztmp, "\x00\xB2\x02\xBC\x00", ulen);
		status = MifareProCom(ulen, sztmp, &sam_sw);
		if ((status==0) || (0x9000!=sam_sw))
		{
			g_Record.log_out(0, level_error, "00B202BC00");
			nresult = ERR_CARD_READ;
			break;
		}
		memcpy(g_metroinf.mtrinf, sztmp, 48);
		memcpy(p_base + data_len, sztmp, 48);
		data_len += 48;


		//应用控制信息区
		ulen = 5;
		memcpy(sztmp, "\x00\xB0\x91\x00\x1E", ulen);
		status = MifareProCom(ulen, sztmp, &sam_sw);
		if ((status==0) || (0x9000!=sam_sw))
		{
			g_Record.log_out(0, level_error, "00B091001E");
			nresult = ERR_CARD_READ;
			break;
		}
		memcpy(p_base + data_len, sztmp, 32);
		data_len += 32;

		//805C000204;
		//钱包
		ulen = 5;
		memcpy(sztmp, "\x80\x5C\x00\x02\x04", ulen);
		status = MifareProCom(ulen, sztmp, &sam_sw);
		if ((status==0) || (0x9000!=sam_sw))
		{
			g_Record.log_out(0, level_error, "805C000204");
			nresult = ERR_CARD_READ;
			break;
		}
		memcpy(p_base + data_len, sztmp, 4);
		data_len += 4;

		//持卡人信息
		if (p_owner != NULL)
		{
			ulen = 5;
			memcpy(sztmp, "\x00\xB0\x96\x00\x37", ulen);
			status = MifareProCom(ulen, sztmp, &sam_sw);
			if ((status==0) || (0x9000!=sam_sw))
			{
				g_Record.log_out(0, level_error, "00B0960037");
				nresult = ERR_CARD_READ;
				break;
			}

			memcpy(p_owner, sztmp, 55);
		}

		if (p_history_last != NULL)
		{
			//最后一次历史
			ulen = 5;
			memcpy(sztmp, "\x00\xB2\x01\xC4\x00", ulen);
			status = MifareProCom(ulen, sztmp, &sam_sw);
			if (status==0)
			{
				g_Record.log_out(0, level_error, "00B201C400");
				nresult = ERR_CARD_READ;
				break;
			}
			memcpy(p_history_last, sztmp, 23);
		}

	} while (0);

	return nresult;
}

//=================================================================================
//函 数 名: metro_svt_read
//功    能: 读取地铁票的数据内容
//出口参数:
//p_history   10条历史记录:230Byte
//返 回 值:
//        >0: 返回的数据长度
//        <0: 操作出错
//=================================================================================
uint16_t metro_svt_history(uint8_t * p_history)
{
	//unsigned char CardNum = 0;
	unsigned char status=0;//, snr[12], snr1[5];
	unsigned char sztmp[256];
	//unsigned char ucTokenCnt=0;
	//unsigned char atqa,firstsnr[7];
	unsigned char ulen;
	unsigned int i;
	unsigned int data_len = 0;
	unsigned short sam_sw;
	int nresult = 0;


	//读历史10条
	for (i=1;i<11;i++)
	{
		ulen = 5;
		memcpy(sztmp, "\x00\xB2\x01\xC4\x00", ulen);
		sztmp[2]=i;
		status = MifareProCom(ulen, sztmp, &sam_sw);
		if ((status==0) || (0x9000!=sam_sw))
		{
			g_Record.log_out(0, level_error, "00B201C400");
			nresult = ERR_CARD_READ;
			break;
		}
		memcpy(p_history + data_len, sztmp, 23);
		data_len += 23;
	}

	return(nresult);

}

int active_mtr_svt(unsigned char *lpinf)
{
	unsigned char status,i;
	unsigned char sztmp[255];
	unsigned char ulen = 0;
	unsigned char snr[10];

	for (i=0; i<3; i++)
	{
		status=ISO14443A_Request( 0x52, sztmp );
		if (status == 0) break;
	}
	if (status)
	{
		return ERR_CARD_NONE;
	}

	status=ISO14443A_Anticoll(snr);
	if (status)
	{
		return ERR_CARD_NONE;
	}

	status=ISO14443A_Select(snr, sztmp);
	if (status)
	{
		return ERR_CARD_NONE;
	}

	status =  MifareProRst(0x40,ulen,sztmp);
	if (status == 0)
	{
		return ERR_CARD_NONE;
	}

	//add test 2016-02-25
	//ulen = 19;
	//memcpy(sztmp, "\x00\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", ulen);
	//ulen = MifareProCom(ulen, sztmp, &response_sw);

	return 0;
}

uint16_t rf_modify(uint8_t dev_type, uint8_t ant_mode, ETYTKOPER oper_type, int * p_rfmain_Status, int * p_rfsub_Status)
{
	uint16_t ret = 0;

	/*
	if (*p_rfmain_Status == 0 && ant_mode == 0x00)
	{
		*p_rfmain_Status = rfdev_get_rfstatus(1);
		if (*p_rfmain_Status == 0)
			ret = ERR_RF_NONE;
	}
	else if (*p_rfsub_Status == 0 && ant_mode == 0x01)
	{
		*p_rfsub_Status = rfdev_get_rfstatus(2);
		if (*p_rfsub_Status == 0)
			ret = ERR_RF_NONE;
	}*/
	/*
	if (ret == 0)
	{
		switch(dev_type)
		{
		case 3:		// BOM
		case 4:		// 进闸机
		case 7:		// TCM
			g_ant_type = 0;
			break;
		case 2:		// TVM
			g_ant_type = 1;
			break;
		case 5:		// 出闸机
			if (ant_mode == 0)
				g_ant_type = 0;
			else
				g_ant_type = 1;
			break;
		case 6:		// 双向闸机
			if (oper_type == operEntry)
			{
				g_ant_type = 0;
			}
			else if (oper_type == operExit)
			{
				if (ant_mode == 0)
					g_ant_type = 0;
				else
					g_ant_type = 1;
			}
			break;
		case 9:		// ES
			g_ant_type = 1;
			break;
		}*/

		g_ant_type = 0x01;

		change_antenna(ant_mode);
		if (ISO14443A_Init(g_ant_type) != 0)
			ret = ERR_CARD_NONE;
	//}

	return ret;
}

void Beep_Sleep(uint8_t beep_times, int MSTimeBeep/* = 30*/, int USTimeNoBeep/* = 30000*/)
{
	if (beep_times > 0)
	{
		Beep(MSTimeBeep);
		for (uint8_t n=0;n<beep_times-1;n++)
		{
			usleep(USTimeNoBeep);
			Beep(MSTimeBeep);
		}
	}
}

bool Check_Issued()
{
	unsigned char ulen;
	unsigned char respone_len;
	unsigned char szcmd[256];
	unsigned short sam_sw;

	ulen = 19;
	memcpy(szcmd, "\x00\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", ulen);

	respone_len = MifareProCom(ulen, szcmd, &sam_sw);

	if ((respone_len==0) || (0x9000!=sam_sw))
	{
		//未发行
		return false;
	}

	return true;
}
