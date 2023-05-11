#include <string.h>
#include <stdint.h>
#include "sam.h"
#include "algorithm.h"
#include "iso14443.h"
#include "ticket.h"
#include "octcard.h"


#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "../api/Extra.h"
#include "../api/Records.h"
#include "../api/Api.h"
#include "../api/Errors.h"

#include "../link/myprintf.h"

#define MI_OK 0

//=================================================================
//CPU卡用
typedef struct
{
    unsigned char       balance[4];                 // 余额
    unsigned char       offlinedealserial[2];       // 钱包脱机交易序号
    unsigned char       overdraft[3];               // 透支限额
    unsigned char       encryver;                   // 密钥版本号
    unsigned char       calcmarker;                 // 算法标识
    unsigned char       random[4];                  // 伪随机数
} TINITCONSUME,*LPTINITCONSUME;

typedef struct
{
    unsigned char        balance[4];                 // 余额
    unsigned char        onlinedealSerial[2]; 				/// 联机交易序号
    unsigned char        encryver;                   // 密钥版本号
    unsigned char        calcmarker;                 // 算法标识
    unsigned char        random[4];                  // 伪随机数
    unsigned char        mac1[4];                      // mac1
} TINITRECHARGE,*LPTINITRECHARGE;

typedef struct
{
    unsigned char      random[4];                  // 伪随机数
    unsigned char      offlinedealserial[2];       // 钱包脱机交易序号
    unsigned char      money[4];                   // 交易金额
    unsigned char      trflag;                 // 交易类型标识
    unsigned char      tm[7];                 // 交易时间
    unsigned char      encryver;                   // 密钥版本号
    unsigned char      calcmarker;                 // 算法标识
    unsigned char      logicnumber[8];
//	    unsigned char      EncryptSeeds2[8];           //二级分散因子
//	    unsigned char      EncryptSeeds1[8];
} TPSAMGETMAC,*LPTPSAMGETMAC;

typedef struct
{
    char 	cMsgType[2];			// 消息类型"51"
    unsigned char	dtTime[7];				// 申请时间
    char	cStation [4];			// 站点
    char	cDevType[2];			// 设备类型
    char	cDevId[3];				// 设备编码
    char	cSamId[16];				// SAM卡号
    long	lPosSeq;				// 终端交易序列号
    char	cNetPoint[16];			// 网点编码
    char	cIssueCodeMain[4];		// 发行商主编码
    char 	cIssueCodeSub[4];		// 发行商子编码
    unsigned char	bTicketType[2];			// 票卡类型
    char	cLogicalId[20];			// 票卡逻辑卡号
    char 	cPhysicalId[20];		// 票卡物理卡号
    unsigned char	bTestFlag;				// 票卡引用标识
    long	lTradeSeqOnLine;		// 联机交易流水
    long	lTradeSeqOffLine;		// 脱机交易流水
    char	cBusinessType[2];		// 业务类型"14"
    unsigned char	bWalletType;			// 值类型
    long	lAmount;				// 充值金额
    long	lBalcance;				// 余额
    char	cMac[8];				// 充值认证码
    char	cFreedom[8];			// 随机数
    char	cStationLast [4];		// 上次站点
    char	cDevTypeLast[2];		// 上次设备类型
    char	cDevIdLast[3];			// 上次设备编码
    unsigned char	dtTimeLast[7];			// 上次时间
    char	cOperatorId[6];			// 操作员编码
}TRECHARGE_PKG_INIT, * LPTRECHARGE_PKG_INIT;


//写卡输出参数
typedef struct
{
    unsigned char sam_terminalnumber[6];
    unsigned char sam_terminalserial[4];
    unsigned char tac[4];
} TOUTMTRW, *LPTOUTMTRW;


TMETROINF g_metroinf;


//=======================================
typedef struct
{
    char fix_no_0[3];
    char bcc_0[1];
    char fix_no_1[4];
    char bcc_1[1];
    char rfu[1];
    char lock[2];
    char otp[4];
} TSJT_FIX_INFO, *LPTSJT_FIX_INFO;

typedef struct
{
    char log_no[4];
    char log_mac[4];
    char iss_time[8];
    char test_flag[1];
    char main_type[1];
    char sub_type[1];
} TSJT_ISSUE_INFO, *LPTSJT_ISSUE_INFO;

typedef struct
{
    char data[4];
} TSJT_PUBLIC_INFO, *LPSJT_PUBLIC_INFO;

typedef struct
{
    char dul_time[14];
    unsigned char line;
    unsigned char station;
    unsigned char dev;
    unsigned char dev_no;
    unsigned char state;
    unsigned char special;
    unsigned short balance;
    unsigned char in_line;
    unsigned char in_station;
    unsigned char trade_no;
} TSJT_STATE_INFO, *LPTSJT_STATE_INFO;


typedef struct
{
    TSJT_FIX_INFO fix;
    TSJT_ISSUE_INFO iss;
    TSJT_PUBLIC_INFO pub;
    TSJT_STATE_INFO sta[2];
} TSJT_INFO, *LPTSJT_INFO;


typedef union
{
    char data[64];
    TSJT_INFO detail;
} TSJT_CARD;

TSJT_CARD	sjt_card;

typedef struct
{
    unsigned char keyidx;				//密钥索引
	unsigned char len_terminal_id;		//终端号长度
    unsigned char sam_terminal_id[8];	//终端号

	unsigned char len_logical_id;		//逻辑卡号长度
	unsigned char offset_logical;		//取逻辑卡号的偏移
    unsigned char sam_logical_id[8];	//逻辑卡号
	unsigned int  dsn;
} TSAMINF, *LPTSAMINF;

unsigned char key_crypt[6];

#define MAX_SOCKET 8
#define _METROFILE_SIZE_ 0X2E
#define _METROFILE_SIZE1_ 0X2E
#define _METROFILE_SIZE2_ 0X16

TSAMINF g_saminf[MAX_SOCKET];
unsigned char g_lstphysical[8];

const unsigned char key_ul[2][6]=
{
    {0x16,0x3B,0x54,0x74,0x55,0xB4},
    {0x20,0x02,0x2B,0x8B,0xEF,0xD1}
};

const unsigned char id_ul[2][4]=
{
    {0x04,0x29,0xD7,0x72},
    {0x04,0x2A,0xCA,0x6C}
};


//=================================================

//=================================================

/* 辅助信息 */
struct ASSIST_DATA
{
    char sjt_key[6];															 /* 卡密钥 */
} ass;

/* 指令信息判断 */
struct ST_CMD_IN_DATA														  /* 输入数据结构 */
{
    unsigned char ln; 														  /* 输入数据长度 */
} si;

struct ST_CMD_OT_DATA														  /* 输出数据结构 */
{
    char data[64];
    unsigned char ln; 														  /* 输入数据长度 */
} so;

//=================================================


//========================================================
//功能:对地铁SAM  进行初始化
//入口参数
//	 nsamsck=sam卡对应卡槽
//出口参数
//       saminf=6字节的sam  卡号
//返回值
//       0=成功
//========================================================
int sam_metro_active(int nsamsck, unsigned char * p_len_sam_id, unsigned char * p_sam_id, unsigned char * p_len_terminal_id, unsigned char * p_terminal_id)
{
	unsigned char status;
	unsigned char sztmp[256];
	unsigned short int sam_sw;
	unsigned char sam_len;
	int nresult = -1;

	do
	{
		sam_setbaud(nsamsck, SAM_BAUDRATE_9600);

		status = sam_rst(nsamsck,sztmp);
		dbg_formatvar("status:%d",status);
		if ( status <= 0 )	break;

		sam_len = 7;
		memcpy(sztmp, "\x00\xA4\x00\x00\x02\x3f\x00",sam_len);
		status = sam_cmd(nsamsck,sam_len,sztmp, sztmp, &sam_sw);
		dbg_formatvar("status2:%d   sam_sw:%04X",status,sam_sw);
		if ( status <= 0 )	break;

		sam_len = 5;
		memcpy(sztmp, "\x00\xB0\x95\x00\x0A",sam_len);
		status = sam_cmd(nsamsck,sam_len,sztmp, sztmp, &sam_sw);
		dbg_formatvar("status3:%d   sam_sw:%04X",status,sam_sw);
		if (0x9000 != sam_sw || status <= 0)	break;

		memcpy(g_saminf[nsamsck].sam_logical_id, sztmp+g_saminf[nsamsck].offset_logical, g_saminf[nsamsck].len_logical_id);
		*p_len_sam_id = g_saminf[nsamsck].len_logical_id;
		memcpy(p_sam_id, g_saminf[nsamsck].sam_logical_id, g_saminf[nsamsck].len_logical_id);

		sam_len = 5;
		memcpy(sztmp, "\x00\xB0\x96\x00\x06",sam_len);
		status = sam_cmd(nsamsck,sam_len,sztmp, sztmp, &sam_sw);
		dbg_formatvar("status4:%d   sam_sw:%04X",status,sam_sw);
		if (0x9000 != sam_sw || status <= 0)	break;

		memcpy(g_saminf[nsamsck].sam_terminal_id, sztmp, g_saminf[nsamsck].len_terminal_id);
		*p_len_terminal_id = g_saminf[nsamsck].len_terminal_id;
		memcpy(p_terminal_id, g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);

		sam_len = 7;
		memcpy(sztmp, "\x00\xA4\x00\x00\x02\x10\x01",sam_len);
		status = sam_cmd(nsamsck,sam_len,sztmp, sztmp, &sam_sw);
		dbg_formatvar("status5:%d   sam_sw:%04X",status,sam_sw);
		if ( status <= 0)	break;

		sam_len = 5;
		memcpy(sztmp, "\x00\xB0\x97\x00\x01",sam_len);
		status = sam_cmd(nsamsck,sam_len,sztmp, sztmp, &sam_sw);
		dbg_formatvar("status6:%d   sam_sw:%04X",status,sam_sw);
		if (0x9000 != sam_sw || status <= 0)	break;

		g_saminf[nsamsck].keyidx=sztmp[0];
		nresult = 0;

		//modify by shiyulong 2013-11-05, 读取流水号
		if (nsamsck == 0)
		{
			for(int i=0; i<2; i++)
			{
				g_saminf[nsamsck].dsn = 0;//初始值
				sam_len = 5;
				memcpy(sztmp, "\x00\xB0\x98\x00\x04",sam_len);
				status = sam_cmd(nsamsck,sam_len,sztmp, sztmp, &sam_sw);
				dbg_formatvar("status7:%d   sam_sw:%04X",status,sam_sw);
				if (0x9000 == sam_sw )
				{
					g_saminf[nsamsck].dsn = sztmp[0];
					g_saminf[nsamsck].dsn <<= 8;
					g_saminf[nsamsck].dsn += sztmp[1];
					g_saminf[nsamsck].dsn <<= 8;
					g_saminf[nsamsck].dsn += sztmp[2];
					g_saminf[nsamsck].dsn <<= 8;
					g_saminf[nsamsck].dsn += sztmp[3];
					break;
				}
				else if (0x6A82 == sam_sw)
				{
					//没有文件直接退出
					nresult = -1;
					break;
				}
			}
		}

	} while (0);

	return nresult;
}

//========================================================
//功能:搜寻天线感应区票卡
//入口参数
//	 无
//出口参数
//       lpcardno=卡号
//返回值
//       0=无卡
//       1=UL
//       2=M1卡
//       3=CPU卡
//      99=多卡
//========================================================
//int searchcard(unsigned char *lpcardno)
//{
//
//	unsigned char CardNum = 0;
//	unsigned char status=0, snr[12], snr1[5];
//	unsigned char tmp[20]; // added by zj
//	unsigned char ucTokenCnt=0;
//	unsigned char atqa,firstsnr[7];
//	int nresult=0;
//
//	memset(firstsnr, 0x00, sizeof(firstsnr));
//
//	//ISO14443A_Init();
//
//	status=ISO14443A_Request( 0x52, tmp );
//
//	if (status==0)
//	{
//
//		atqa=tmp[0];
//		if (atqa!=0x44)
//		{
//			status=ISO14443A_Anticoll(snr);
//
//			if (status==0)
//			{
//				status=ISO14443A_Select(snr, tmp);
//
//				if ((tmp[0]&0x24)==0x20)
//				{
//					nresult = 3; //cpu卡
//				}
//				else
//				{
//					nresult = 2; //m1卡
//				}
//				CardNum++;
//				memcpy(firstsnr, snr, 4);
//				memcpy(lpcardno,firstsnr, 4);
//			};
//		}
//		else
//		{
//			//UL单程票
//			if ((status=ISO14443A_Anticoll_UL(0x93, snr)) == 0)
//			{
//
//				if ((status=ISO14443A_Select_UL(0x93, snr, tmp)) == 0)
//				{
//
//					if ((status=ISO14443A_Anticoll_UL(0x95, snr1)) == 0)
//					{
//						if ((status=ISO14443A_Select_UL(0x95, snr1, tmp)) == 0)
//						{
//							memcpy(lpcardno, &snr[1], 3);
//							memcpy(lpcardno+3, snr1, 4);
//							memcpy(firstsnr, lpcardno, 7);
//							ucTokenCnt++;
//							CardNum++;
//							nresult = 1;
//
//						}
//					}
//				}
//			}
//		}
//	}
//
//	if ((status=ISO14443A_Request( 0x52, tmp ))==0)
//	{
//
//		atqa=tmp[0];
//
//		if (atqa!=0x44)
//		{
//			if ((status=ISO14443A_Anticoll(snr))==0)
//			{
//				if ((status=ISO14443A_Select(snr, tmp))==0)
//				{
//
//					if (memcmp(snr, firstsnr, 4)!=0)
//					{
//						CardNum++;
//					}
//					if ((tmp[0]&0x24)==0x20)
//					{
//						nresult = 3; //cpu卡
//					}
//					else
//					{
//						nresult = 2; //m1卡
//					}
//					memcpy(lpcardno, snr, 4);
//
//				}
//
//			}
//		}
//		else
//		{
//			if ((status=ISO14443A_Anticoll_UL(0x93, snr)) == 0)
//			{
//				if ((status=ISO14443A_Select_UL(0x93, snr, tmp)) == 0)
//				{
//					if ((status=ISO14443A_Anticoll_UL(0x95, snr1)) == 0)
//					{
//						if ((status=ISO14443A_Select_UL(0x95, snr1, tmp)) == 0)
//						{
//							memcpy(lpcardno, &snr[1], 3);
//							memcpy(lpcardno+3, snr1, 4);
//							if (memcmp(firstsnr, lpcardno, 7)!=0)
//							{
//								ucTokenCnt++;
//								CardNum++;
//							}
//							nresult = 1;
//						}
//					}
//				}
//			}
//		}
//
//
//	}
//
//	if (CardNum>1)
//	{
//		nresult = 99;
//	}
//	else
//	{
//		memcpy(g_lstphysical,lpcardno, 8);
//	}
//
//	return nresult;
//
//}
int searchcard(unsigned char *lpcardno)
{
	unsigned char snr[12]		= {0};
	unsigned char snr1[5]		= {0};
	unsigned char tmp[20]		= {0};
	unsigned char ack[2][8]		= {{0},{0}};
	int card_type[2]			= {0};
	int ret						= 0;

	for (int i=0;i<2;i++)
	{
		if (ISO14443A_Request(0x52, tmp) == 0)
		{
			if (tmp[0] != 0x44)
			{
				if (ISO14443A_Anticoll(snr) == 0)
				{
					if (ISO14443A_Select(snr, tmp) == 0)
					{
						card_type[i] = (tmp[0] & 0x24) == 0x20 ? 0x03 : 0x02;
						memcpy(ack[i], snr, 4);
					}
				}
			}
			else
			{
				//UL单程票
				if (ISO14443A_Anticoll_UL(0x93, snr) == 0)
				{
					if (ISO14443A_Select_UL(0x93, snr, tmp) == 0)
					{
						if (ISO14443A_Anticoll_UL(0x95, snr1) == 0)
						{
							if (ISO14443A_Select_UL(0x95, snr1, tmp) == 0)
							{
								card_type[i] = 1;
								memcpy(ack[i], snr + 1, 3);
								memcpy(ack[i] + 3, snr1, 4);
							}
						}
					}
				}
			}
		}
	}

	if (card_type[0] == 0)
	{
		memcpy(lpcardno, ack[1], 8);
		ret = card_type[1];
	}
	else if (card_type[1] == 0)
	{
		memcpy(lpcardno, ack[0], 8);
		ret = card_type[0];
	}
	else if (memcmp(ack[0], ack[1], 8) == 0)
	{
		memcpy(lpcardno, ack[0], 8);
		ret = card_type[0];
	}
	else
	{
		ret = 99;
	}

	memcpy(g_lstphysical, lpcardno, 8);

	if (ret == 3)
	{
		ret = distinguish_cpu(lpcardno);
	}

	return ret;
}

unsigned short search_card_ex(unsigned char *lpcardno, unsigned char& type)
{
	unsigned char snr[12]		= {0};
	unsigned char snr1[5]		= {0};
	unsigned char tmp[20]		= {0};
	unsigned char ack[2][8]		= {{0},{0}};
	int card_type[2]			= {0};
	unsigned short ret			= 0;

	for (int i=0;i<2;i++)
	{
		if (ISO14443A_Request(0x52, tmp) == 0)//xunka
		{
			if (tmp[0] != 0x44)
			{
				if (ISO14443A_Anticoll(snr) == 0)//防冲突
				{
					if (ISO14443A_Select(snr, tmp) == 0)//选卡
					{
						card_type[i] = (tmp[0] & 0x24) == 0x20 ? 0x03 : 0x02;
						memcpy(ack[i], snr, 4);
					}
				}
			}
			else
			{
				//UL单程票
				if (ISO14443A_Anticoll_UL(0x93, snr) == 0)
				{
					if (ISO14443A_Select_UL(0x93, snr, tmp) == 0)
					{
						if (ISO14443A_Anticoll_UL(0x95, snr1) == 0)
						{
							if (ISO14443A_Select_UL(0x95, snr1, tmp) == 0)
							{
								card_type[i] = 1;
								memcpy(ack[i], snr + 1, 3);
								memcpy(ack[i] + 3, snr1, 4);
							}
						}
					}
				}
			}
		}
	}

	if (card_type[0] == 0)//单程票
	{
		memcpy(lpcardno, ack[1], 8);
		type = card_type[1];
	}
	else if (card_type[1] == 0)
	{
		memcpy(lpcardno, ack[0], 8);
		type = card_type[0];
	}
	else if (memcmp(ack[0], ack[1], 8) == 0)
	{
		memcpy(lpcardno, ack[0], 8);
		type = card_type[0];
	}
	else
	{
		ret = ERR_CARD_MORE;
	}

	memcpy(g_lstphysical, lpcardno, 8);

	if (type == 3)
	{
		g_Record.log_out(0, level_error, "distinguish_cpu_ex");
		ret = distinguish_cpu_ex(lpcardno, type);
	}

	return ret;
}

int searchcard_es(unsigned char *lpcardno)
{
	unsigned char snr[12]		= {0};
	unsigned char snr1[5]		= {0};
	unsigned char tmp[20]		= {0};
	unsigned char ack[2][8]		= {{0},{0}};
	int card_type[2]			= {0};
	int ret						= 0;

	for (int i=0;i<2;i++)
	{
		if (ISO14443A_Request(0x52, tmp) == 0)
		{
			if (tmp[0] != 0x44)
			{
				if (ISO14443A_Anticoll(snr) == 0)
				{
					if (ISO14443A_Select(snr, tmp) == 0)
					{
						card_type[i] = (tmp[0] & 0x24) == 0x20 ? 0x03 : 0x02;
						memcpy(ack[i], snr, 4);
					}
				}
			}
			else
			{
				//UL单程票
				if (ISO14443A_Anticoll_UL(0x93, snr) == 0)
				{
					if (ISO14443A_Select_UL(0x93, snr, tmp) == 0)
					{
						if (ISO14443A_Anticoll_UL(0x95, snr1) == 0)
						{
							if (ISO14443A_Select_UL(0x95, snr1, tmp) == 0)
							{
								card_type[i] = 1;
								memcpy(ack[i], snr + 1, 3);
								memcpy(ack[i] + 3, snr1, 4);
							}
						}
					}
				}
			}
		}
	}

	if (card_type[0] == 0)
	{
		memcpy(lpcardno, ack[1], 8);
		ret = card_type[1];
	}
	else if (card_type[1] == 0)
	{
		memcpy(lpcardno, ack[0], 8);
		ret = card_type[0];
	}
	else if (memcmp(ack[0], ack[1], 8) == 0)
	{
		memcpy(lpcardno, ack[0], 8);
		ret = card_type[0];
	}
	else
	{
		ret = 99;
	}

	memcpy(g_lstphysical, lpcardno, 8);


	return ret;
}

int distinguish_cpu(unsigned char * p_pid)
{
	int nresult					= 99;
	unsigned short response_sw	= 0;
	unsigned char ulen			= 0;
	unsigned char sztmp[256]	= {0};

	do
	{
		if (ISO14443A_Request(0x52, sztmp) != 0)
		{
			if (ISO14443A_Request(0x52, sztmp) != 0)	break;
		}

		if (ISO14443A_Select(g_lstphysical, sztmp) != 0)	break;

		nresult = octm1_judge(p_pid);
		if (nresult == 0x02)	break;

		if (MifareProRst(0x40,ulen,sztmp) == 0)
		{
			if (ISO14443A_Init(g_ant_type) != 0)	break;

			usleep(5000);
			if (ISO14443A_Request(0x52, sztmp) != 0)	break;

			usleep(5000);
			if (ISO14443A_Select(g_lstphysical, sztmp) != 0)	break;

			usleep(5000);
			if (MifareProRst(0x40,ulen,sztmp) == 0)	break;
		}

		//add test 2016-02-25
// 		ulen = 19;
// 		memcpy(sztmp, "\x00\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", ulen);
// 		ulen = MifareProCom(ulen, sztmp, &response_sw);

		ulen = 5;
		memcpy(sztmp, "\x00\xB0\x85\x00\x02", ulen);

		ulen = MifareProCom(ulen, sztmp, &response_sw);

		if (response_sw==0x9000)
		{
			if (memcmp(sztmp, "\x53\x20",2)==0 || memcmp(sztmp, "\x07\x31",2)==0)//add test 2016-02-25
			{
				nresult = 3;				//地铁
			}
			else if (memcmp(sztmp, "\x59\x44",2)==0)
			{
				nresult = 4;				//公交
			}
			else
			{
				nresult = 9;
			}
		}
		else
		{
			nresult = 9;
		}

	} while (0);


	return nresult;
}

unsigned short distinguish_cpu_ex(unsigned char * p_pid, unsigned char& type)
{
	unsigned short nresult		= 0;
	unsigned short response_sw	= 0;
	unsigned char ulen			= 0;
	unsigned char sztmp[256]	= {0};

	do
	{
		if (ISO14443A_Request(0x52, sztmp) != 0)
		{
			if (ISO14443A_Request(0x52, sztmp) != 0)
			{
				nresult = ERR_CARD_NONE;
				break;
			}
		}

		if (ISO14443A_Select(g_lstphysical, sztmp) != 0)
		{
			nresult = ERR_CARD_NONE;
			break;
		}

		nresult = octm1_judge_ex(p_pid, type);
		if (nresult != 0 || type == 2)	break;

		if (MifareProRst(0x40,ulen,sztmp) == 0)
		{
			if (ISO14443A_Init(g_ant_type) != 0)
			{
				nresult = ERR_CARD_NONE;
				break;
			}

			//usleep(5000);
			if (ISO14443A_Request(0x52, sztmp) != 0)
			{
				if (ISO14443A_Request(0x52, sztmp) != 0)
				{
					nresult = ERR_CARD_NONE;
					break;
				}
			}

			//usleep(5000);
			if (ISO14443A_Select(g_lstphysical, sztmp) != 0)
			{
				nresult = ERR_CARD_NONE;
				break;
			}

			//usleep(5000);
			if (MifareProRst(0x40,ulen,sztmp) == 0)
			{
				nresult = ERR_CARD_NONE;
				break;
			}
		}


		//add test 2016-02-25
// 		ulen = 19;
// 		memcpy(sztmp, "\x00\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", ulen);
// 		ulen = MifareProCom(ulen, sztmp, &response_sw);



		ulen = 5;
		memcpy(sztmp, "\x00\xB0\x85\x00\x02", ulen);

		ulen = MifareProCom(ulen, sztmp, &response_sw);

		if (response_sw==0x9000)
		{
			if (memcmp(sztmp, "\x53\x20",2)==0 || memcmp(sztmp, "\x07\x31",2)==0|| memcmp(sztmp, "\x09\x91",2)==0)
			{
				type = 3;				//地铁
			}
			else if (memcmp(sztmp, "\x59\x44",2)==0)
			{
				type = 4;				//公交
			}
			else
			{
				g_Record.log_out(0, level_error, "distinguish_cpu_ex:%02X%02X",sztmp[0],sztmp[1]);
				nresult = ERR_OVER_SYSTEM;
				break;
			}
		}
		else
		{
			nresult = ERR_CARD_NONE;
			break;
		}

	} while (0);


	return nresult;
}

//=================================
//功能: CPU 卡复位,  此前必须做过searchcard函数
//入口参数:  无
//出口参数:
//                         lpinf: 复位信息
//返回值:
//                =0成功
//=================================
int svt_active(unsigned char *lpinf)
{
	unsigned char status,i;
	unsigned char sztmp[255];
	unsigned char ulen = 0;
	//unsigned char snr[10];
	int nresult = -1;


	for (i=0; i<3; i++)
	{
		status=ISO14443A_Request( 0x52, sztmp );
		if (status == 0) break;
	}

	if (status == 0)
	{
		status=ISO14443A_Select(g_lstphysical, sztmp);
	}


	status =  MifareProRst(0x40,ulen,sztmp);
	if (status)
	{
		memcpy(lpinf, sztmp, status);
		nresult = 0;
	}

	//add test 2016-02-25
	//ulen = 19;
	//memcpy(sztmp, "\x00\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", ulen);
	//ulen = MifareProCom(ulen, sztmp, &response_sw);

	return nresult;
}


//========================================================
//功能:读单程票数据
//入口参数
//	 nsamsck=sam卡对应卡槽
//出口参数
//       lpbuffer=解码后的票卡数据，从第4页到第15页(48BYTE)
//返回值
//       0=读卡成功, 且两个处理区crc  都正确
//       1=读卡成功, 第一个处理区crc  错误
//       2=读卡成功, 第二个处理区crc  错误
//       3=读卡成功, 两个处理区crc  都错误
//      -1=无卡
//      -2=读数据失败
//      -5=操作sam  卡失败
//========================================================
int readul(int nsamsck, unsigned char *lpbuffer)
{
	//unsigned char buffer[64];
	unsigned char atqa[2], sak;
	unsigned char szphysical[7];
	unsigned char sztemp[12];
	unsigned char cmd[256];
	//unsigned char tmp_cmd[18] = {0};
	unsigned char en_tmp[4];//,en_next[4];
	int errcnt=0;

	unsigned char cmd_ln;
	unsigned short int sam_sw;
	//unsigned char sam_len;

	char status;
	unsigned char i;


	status= ISO14443A_Request(0x26, sztemp);
	if (status != 0)
	{

		for (i=0; i<3; i++)
		{
			status = ISO14443A_Request_UL(0x26, atqa);
			if (status == MI_OK) break;
		}
	}

	if (status != MI_OK) return(-1);
	status = ISO14443A_Anticoll_UL(0x93, sztemp); // Anticoll 1
	if (status != MI_OK) return(-1);
	memcpy(szphysical, sztemp+1, 3);

	status = ISO14443A_Select_UL(0x93, sztemp, &sak);
	if (status != MI_OK) return (-1);

	status = ISO14443A_Anticoll_UL(0x95, sztemp); // Anticoll 2
	if (status != MI_OK) return(-1);
	memcpy(szphysical+3, sztemp, 4);

	status = ISO14443A_Select_UL(0x95, sztemp, &sak);
	if (status != MI_OK) return (-1);


	/* 读所有数据 */
	// Read all data
	for (i=0; i<=15; i+=4)
	{
		status = ISO14443A_Read_UL(i, (uint8_t *)(sjt_card.data+(i<<2)));
		if (status != MI_OK)
		{
			//尝试多读一次
			status = ISO14443A_Read_UL(i, (uint8_t *)(sjt_card.data+(i<<2)));
			if (status != MI_OK)
			{
				return (-2);
			}
		}
	}

	g_Record.log_out(0, level_error, "fix_no_1=(%02X%02X%02X%02X)",sjt_card.detail.fix.fix_no_1[0], sjt_card.detail.fix.fix_no_1[1],sjt_card.detail.fix.fix_no_1[2],sjt_card.detail.fix.fix_no_1[3]);
	g_Record.log_out(0, level_error, "log_no=(%02X%02X%02X%02X)",sjt_card.detail.iss.log_no[0], sjt_card.detail.iss.log_no[1],sjt_card.detail.iss.log_no[2],sjt_card.detail.iss.log_no[3]);
	g_Record.log_out(0, level_error, "log_mac=(%02X%02X%02X%02X)",sjt_card.detail.iss.log_mac[0], sjt_card.detail.iss.log_mac[1],sjt_card.detail.iss.log_mac[2],sjt_card.detail.iss.log_mac[3]);
	dbg_dumpmemory("fix_no_1=",sjt_card.detail.fix.fix_no_1,4);
	dbg_dumpmemory("log_no=",sjt_card.detail.iss.log_no,4);
	dbg_dumpmemory("log_mac=",sjt_card.detail.iss.log_mac,4);

	cmd_ln = 18;
	memcpy(&cmd[0], "\x80\xFC\x01\x01\x0D", 5);
	memcpy(&cmd[5], sjt_card.detail.fix.fix_no_1, 4);
	memcpy(&cmd[9], sjt_card.detail.iss.log_no, 4);
	memcpy(&cmd[13],sjt_card.detail.iss.log_mac, 4);
	memcpy(&cmd[17], "\xA5", 1);

	dbg_dumpmemory("cmd=",cmd,18);

	status = sam_cmd(nsamsck,cmd_ln,cmd, cmd, &sam_sw);
	dbg_formatvar("status=%d",status);
	dbg_dumpmemory("cmd=",cmd,6);
	dbg_formatvar("sam_sw=%04X",sam_sw);

	g_Record.log_out(0, level_error, "sam_sw=(%04X)|cmd=(%02X%02X%02X%02X%02X%02X)",sam_sw,cmd[0], cmd[1],cmd[2],cmd[3],cmd[4],cmd[5]);

	/* 计算密钥 */
	if (0x9000!=sam_sw)
	{
		return (-5);
	}

	memcpy(key_crypt, cmd, 6);

	memcpy(en_tmp, key_crypt+2, 4);
	dbg_dumpmemory("key_crypt=",key_crypt,6);
	for (i = 0; i < 4; i ++)
	{
		sjt_card.data[32 + i*4] = sjt_card.data[32 + i*4] ^ en_tmp[0];
		sjt_card.data[32 + i*4 + 1] = sjt_card.data[32 + i*4 + 1] ^ en_tmp[1];
		sjt_card.data[32 + i*4 + 2] = sjt_card.data[32 + i*4 + 2] ^ en_tmp[2];
		sjt_card.data[32 + i*4 + 3] = sjt_card.data[32 + i*4 + 3] ^ en_tmp[3];

		en_tmp[0] = sjt_card.data[32 + i*4];
		en_tmp[1] =sjt_card.data[32 + i*4 + 1];
		en_tmp[2] = sjt_card.data[32 + i*4 + 2];
		en_tmp[3] = sjt_card.data[32 + i*4 + 3];
	}

	memcpy(en_tmp, key_crypt+2, 4);
	//	  memcpy(&en_tmp[0], &ass.sjt_key[2], 4);
	for (i = 0; i < 4; i ++)
	{
		sjt_card.data[48 + i*4] = sjt_card.data[48 + i*4] ^ en_tmp[0];
		sjt_card.data[48 + i*4 + 1] = sjt_card.data[48 + i*4 + 1] ^ en_tmp[1];
		sjt_card.data[48 + i*4 + 2] = sjt_card.data[48 + i*4 + 2] ^ en_tmp[2];
		sjt_card.data[48 + i*4 + 3] = sjt_card.data[48 + i*4 + 3] ^ en_tmp[3];

		en_tmp[0] = sjt_card.data[48 + i*4];
		en_tmp[1] =sjt_card.data[48 + i*4 + 1];
		en_tmp[2] = sjt_card.data[48 + i*4 + 2];
		en_tmp[3] = sjt_card.data[48 + i*4 + 3];
	}

	if (generate_crc8((uint8_t *)(sjt_card.data+32),16,key_crypt[0])!=0)
	{
		//处理0  区CRC  错
		errcnt += 1;
	};

	if (generate_crc8((uint8_t *)(sjt_card.data+48),16,key_crypt[1])!=0)
	{
		//处理1  区CRC  错
		errcnt += 2;
	};

	memcpy(lpbuffer, sjt_card.data,64);
	memcpy(lpbuffer+64,szphysical,7);

	return errcnt;
}

#define USERZONE_PAGER1 8
#define USERZONE_PAGER2 12
//========================================================
//功能:写单程票数据
//入口参数
//        nsamsck=sam卡对应卡槽
//        nzone=数据区域，0-处理数据区0，1-处理数据区0
//        lpbuffer=16字节处理区数据，
//出口参数
//       无
//返回值
//       0=成功
//      -3=写卡失败
//========================================================
int writeul(int nsamsck, int nzone, unsigned char *lpbuffer)
{
	int i,j;
	unsigned char idx = nzone;
	unsigned char crc8;
	unsigned char en_tmp[4],en_next[4];
	unsigned char write_buf[16];
	char status;
	int nresult = 0;
	int offset_pager;


	//原始数据先计算并填充 crc8
	if ( idx>1 ) idx=1;
	crc8 = generate_crc8(lpbuffer, 15, key_crypt[idx]);
	lpbuffer[15] = crc8;

	memcpy(write_buf, lpbuffer, 16);

	//写数据前进行加密

	memcpy(en_tmp, key_crypt+2, 4);

	for (i = 0; i < 4; i ++)
	{
		en_next[0] = write_buf[i*4];
		en_next[1] = write_buf[i*4 + 1];
		en_next[2] = write_buf[i*4 + 2];
		en_next[3] = write_buf[i*4 + 3];

		write_buf[i*4] = write_buf[i*4] ^ en_tmp[0];
		write_buf[i*4 + 1] = write_buf[i*4 + 1] ^ en_tmp[1];
		write_buf[i*4 + 2] = write_buf[i*4 + 2] ^ en_tmp[2];
		write_buf[i*4 + 3] = write_buf[i*4 + 3] ^ en_tmp[3];

		en_tmp[0] = en_next[0];
		en_tmp[1] = en_next[1];
		en_tmp[2] = en_next[2];
		en_tmp[3] = en_next[3];
	}

	//写数据
	if ( nzone == 0)
		offset_pager = USERZONE_PAGER1;
	else
		offset_pager = USERZONE_PAGER2;

	for (i=0,j=0;i<4;i++,j+=4)
	{
		status = ISO14443A_Write_UL(offset_pager+i,write_buf+j);
		if (status)
		{
			nresult = i;
			break;
		}
	}

	return nresult;
}
uint16_t writeul_new(int nsamsck, int nzone, unsigned char *lpbuffer)
{
	int i,j;
	unsigned char en_tmp[4],en_next[4];
	unsigned char write_buf[16];
	char status;
	uint16_t ret = 0;
	int offset_pager = 0;

	//#####################
	g_Record.log_out(0, level_disaster, "6 in writeul_new:%d", nzone);

	memcpy(write_buf, lpbuffer, 16);

	//写数据前进行加密
	memcpy(en_tmp, key_crypt+2, 4);

	for (i = 0; i < 4; i ++)
	{
		en_next[0] = write_buf[i*4];
		en_next[1] = write_buf[i*4 + 1];
		en_next[2] = write_buf[i*4 + 2];
		en_next[3] = write_buf[i*4 + 3];

		write_buf[i*4] = write_buf[i*4] ^ en_tmp[0];
		write_buf[i*4 + 1] = write_buf[i*4 + 1] ^ en_tmp[1];
		write_buf[i*4 + 2] = write_buf[i*4 + 2] ^ en_tmp[2];
		write_buf[i*4 + 3] = write_buf[i*4 + 3] ^ en_tmp[3];

		en_tmp[0] = en_next[0];
		en_tmp[1] = en_next[1];
		en_tmp[2] = en_next[2];
		en_tmp[3] = en_next[3];
	}

	//写数据
	if ( nzone == 0)
		offset_pager = USERZONE_PAGER1;
	else if (nzone == 1)
		offset_pager = USERZONE_PAGER2;
	else
		ret = ERR_INPUT_INVALID;

	//#####################
	g_Record.log_out(0, level_disaster, "offset_pager in writeul_new:%d", offset_pager);

	if (ret == 0)
	{
		for (i=0,j=0;i<4;i++,j+=4)
		{
			status = ISO14443A_Write_UL(offset_pager+i,write_buf+j);
			if (status)
			{
				ret = i;
				break;
			}
		}
	}

	return ret;
}
uint16_t add_ul_crc8(unsigned char data_point, unsigned char * p_area_data)
{
	//#####################
	g_Record.log_out(0, level_disaster, "5 in add_ul_crc8:%d", data_point);

	if (data_point != 0 && data_point != 1)
		return ERR_INPUT_INVALID;

	p_area_data[15] = generate_crc8(p_area_data, 15, key_crypt[data_point]);

	return 0;
}

//=====================================================================

//========================================================
//功能:对地铁SAM  进行功能激活，只有激活过后的才能进行发售延期解锁
//入口参数
//	 nsamsck=sam卡对应卡槽
//    direct=数据方向, 0=表示取激活信息，此信息需发送给服务器; 1=表示激活操作，将服务器返回信息发送到SAM  卡
//    lpinf=当direct=1时为输入 参数
//出口参数
//       无
//返回值
//       0=成功
//========================================================
int sam_metro_function_active(int nsamsck, int direct, unsigned char *lpinf)
{
	unsigned char status;
	unsigned char sam_len;
	unsigned char sztmp[256];
	unsigned short sam_sw;
	int nresult=-1;


	if (direct==0)
	{
		//获取随机数
		sam_len = 5;
		memcpy(sztmp, "\x00\x84\x00\x00\x08",sam_len);
		status = sam_cmd(nsamsck,sam_len,sztmp, sztmp, &sam_sw);
		if ( status>0 )
		{
			if (0x9000==sam_sw)
			{
				memcpy(lpinf, sztmp, 8);
				nresult = 0;
			}
		}
	}
	else
	{

		//执行外部认证
		nresult=-2;

		sam_len = 5;
		memcpy(sztmp, "\x00\x82\x00\x01\x08",sam_len);

		memcpy(sztmp+5,lpinf, 8);
		sam_len = 5+8;

		status = sam_cmd(nsamsck,sam_len,sztmp, sztmp, &sam_sw);


		if ( status>0 )
		{
			if (0x9000==sam_sw)
			{
				nresult = 0;
			}
		}

	}

	return nresult;
}

//========================================================
//功能:选择应用目录
//入口参数
//	 aid=应用目录标识, 0x3F00=主应用目录,  0x1001=ADF1目录, 0x1002=ADF2目录
//出口参数
//       无
//返回值
//       >=0成功
//========================================================
int svt_selectfile(unsigned short aid)
{
	unsigned char ulen;
	unsigned char sztmp[256];
	unsigned char response_len;
	unsigned short response_sw;
	int nresult =  -1;
	ulen = 7;
	memcpy(sztmp, "\x00\xA4\x00\x00\x02\x3F\x00", ulen);
	sztmp[5]= ((aid>>8) & 0x00FF);
	sztmp[6]= (aid & 0x00FF);
	dbg_dumpmemory("svt_selectfile=",sztmp,7);
	response_len = MifareProCom(ulen, sztmp, &response_sw);
	dbg_dumpmemory("svt_selectfile |=",sztmp,response_len);
	dbg_formatvar("svt_selectfile |= %04X",response_sw);

	if (response_len==0)
	{
		//无回应
		nresult = -1;
	}
	else if (0x9000!=response_sw)
	{
		g_Record.log_out(0, level_error, "svt_selectfile[%d] = %04X",aid,response_sw);
		//响应错误
		nresult = -2;
	}
	else
	{
		nresult = 0;
	}

	return nresult;

}

//========================================================
//功能: 读二进制文件,  要先进入相应的应用目录才能正确读取
//入口参数
//	 aid=短文件标识, 0x05=发行文件,  0x11=应用控制文件，0x15=公共信息文件,  0x16=持卡人个人信息文件
//    start_offset:读取文件的起始偏移地址
//    len:要读取的长度
//出口参数
//       lpdata: 读取的数据缓冲
//返回值
//       >=0成功
//========================================================
int svt_readbinary(unsigned char aid, unsigned char start_offset, unsigned char len, unsigned char *lpdata)
{
	unsigned char ulen;
	unsigned char sztmp[256];
	unsigned char response_len;
	unsigned short response_sw;

	int nresult =  -1;
	ulen = 5;
	memcpy(sztmp, "\x00\xB0\x00\x00\x02", ulen);
	sztmp[2]= ((aid & 0x1F) | 0x80);
	sztmp[3]= start_offset;
	sztmp[4]= len;
	dbg_dumpmemory("svt_readbinary=",sztmp,ulen);
	response_len = MifareProCom(ulen, sztmp, &response_sw);
	dbg_dumpmemory("svt_readbinary=",sztmp,response_len);
	dbg_formatvar("response_sw = %04x | response_len = %d",response_sw,response_len);
	if (response_len==0)
	{
		//无回应
		g_Record.log_out(0, level_error, "svt_readbinary[%d]= [%04X]",response_sw);
		nresult = -1;
	}
	else if (0x9000!=response_sw)
	{
		g_Record.log_out(0, level_error, "svt_readbinary[%d] = %04X",aid,response_sw);
		//响应错误
		nresult = -2;
	}
	else
	{
		//        nresult = 0;
		memcpy(lpdata, sztmp, response_len-2);
		nresult=response_len-2;
	}

	return nresult;

}

//========================================================
//功能: 读记录文件,  要先进入相应的应用目录才能正确读取
//入口参数
//	 aid=短文件标识, 0x17=复合文件，0x18=历史明细
//    record_no:要读取文件记录号
//    len:要读取的长度
//出口参数
//       lpdata: 读取的数据缓冲
//返回值
//       >=0成功
//========================================================
int svt_readrecord(unsigned char aid, unsigned char record_no, unsigned char len, unsigned char *lpdata)
{
	unsigned char ulen = 0;
	unsigned char sztmp[256];
	unsigned char response_len = 0;
	unsigned short response_sw = 0;

	int nresult =  -1;
	ulen = 5;
	memcpy(sztmp, "\x00\xB2\x00\x00\x02", ulen);
	sztmp[2]= record_no;
	sztmp[3]= ((aid << 3) & 0xF8);
	sztmp[4]= len;

	response_len = MifareProCom(ulen, sztmp, &response_sw);

	if (response_len==0)
	{
		//无回应
		//g_Record.log_out(response_len, level_error, "MifareProCom1(response_sw=%d)", response_sw);
		nresult = -1;
	}
	else
		if (0x9000!=response_sw)
		{
			//响应错误
			g_Record.log_out(0, level_error, "svt_readrecord[%d][%d] = %04X",aid,record_no,response_sw);
			nresult = -2;
		}
		else
		{
			//nresult = 0;
			memcpy(lpdata, sztmp, response_len-2);
			nresult=response_len-2;
		}

		return nresult;

}


//========================================================
//功能: 读历史明细,  要先进入相应的应用目录才能正确读取
//入口参数
//	 aid=短文件标识, 0x17=复合文件，0x18=历史明细
//    record_start: 要读取起始记录号
//      record_cnt:要读取的记录条数
//出口参数
//       lpdata: 读取的数据缓冲
//返回值
//       >=0成功
//========================================================
int svt_readhistory(unsigned char aid, unsigned char record_start, unsigned char record_cnt, unsigned char *lpdata)
{
	unsigned char ulen;
	unsigned char sztmp[256];
	unsigned char response_len;
	unsigned short response_sw;
	unsigned char read_idx=record_start;

	int offset;
	int i;
	int nresult =  0;


	i=0;
	offset = 0;

	do
	{
		ulen = 5;
		memcpy(sztmp, "\x00\xB2\x00\x00\x02", ulen);
		sztmp[2]= read_idx;
		sztmp[3]= (((aid << 3) & 0xF8) | 0x04);
		sztmp[4]= 00;

		response_len = MifareProCom(ulen, sztmp, &response_sw);
		if (response_len==0)
		{
			//无回应
			nresult = -1;
			break;
		}
		else if (0x6a82 == response_sw || 0x6a83 == response_sw)
		{
			// 无文件或者无记录
			break;
		}
		else if (0x9000!=response_sw)
		{
			//响应错误
			//nresult = -2;
			break;
		}
		else
		{
			//nresult = 0;
			memcpy(lpdata+offset, sztmp, response_len-2);
			offset += response_len-2;
			nresult = offset;
		}
		read_idx++;
		i++;
	}
	while (i<record_cnt);

	return nresult;
}

//========================================================
//功能: 读钱包值,  要先进入相应的应用目录才能正确读取
//入口参数
//	 无
//出口参数
//       lpdata: 4字节的钱包值,  高字节在前
//返回值
//       >=0成功
//========================================================
int svt_getbalance(unsigned char *lpdata)
{
	unsigned char ulen;
	unsigned char sztmp[256];
	unsigned char response_len;
	unsigned short response_sw;

	//int i;
	int nresult =  -1;

	//钱包
	ulen = 5;
	memcpy(sztmp, "\x80\x5C\x00\x02\x04", ulen);
	response_len = MifareProCom(ulen, sztmp, &response_sw);
	dbg_formatvar("805C |= %04X",response_sw);

	if (response_len==0)
	{
		//无回应
		nresult = -1;
	}
	else if (0x9000!=response_sw)
	{
		//响应错误
		nresult = -2;
	}
	else
	{
		//nresult = 0;
		memcpy(lpdata, sztmp, response_len-2);
		nresult = response_len-2;
	}

	return nresult;

}


//========================================================
//功能: 取联机交易计数
//入口参数
//	 nsamsck: 地铁SAM  卡对应的卡座,  从0  开始
//出口参数
//       lpdata: 2字节的联机序列号,  高字节在前
//返回值
//       >=0成功
//========================================================
int svt_getonlineserial(int nsamsck,unsigned char *lpserial)
{
	unsigned char ulen;
	unsigned char sztmp[256];
	unsigned char response_len;
	unsigned short response_sw;
	LPTINITRECHARGE lpinitrecharge;
	int nresult=-1;

	do
	{
		ulen = 8;
		memcpy(sztmp, "\x00\x20\x00\x00\x03\x12\x34\x56",8);
		response_len = MifareProCom(ulen,sztmp, &response_sw);
		if ((response_len==0 ) || (0x9000!=response_sw))
		{
			nresult = -3;
			break;

		}

		ulen=0;

		memcpy(sztmp, "\x80\x50\x00\x02\x0B", 5);

		ulen += 5;

		sztmp[5]=g_saminf[nsamsck].keyidx;
		ulen += 1;
		sztmp[ulen] = 0;
		ulen += 1;
		sztmp[ulen] = 0;
		ulen += 1;
		sztmp[ulen] = 0;
		ulen += 1;
		sztmp[ulen] = 0;
		ulen += 1;

		memcpy(sztmp+ulen, g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);
		ulen += g_saminf[nsamsck].len_terminal_id;
		response_len = MifareProCom(ulen, sztmp, &response_sw);
		if (response_sw==0)
		{
			nresult = -1;
		}
		else if (0x9000!=response_sw)
		{
			nresult = -1;
		}
		else
		{
			lpinitrecharge = (LPTINITRECHARGE)sztmp;
			memcpy(lpserial, lpinitrecharge->onlinedealSerial, 2);
			nresult = 2;
		}
	}
	while (0);

	return nresult;

}

//========================================================
//功能: 取脱机交易计数
//入口参数
//	 nsamsck: 地铁SAM  卡对应的卡座,  从0  开始
//出口参数
//       lpdata: 2字节的胶机序列号,  高字节在前
//返回值
//       >=0成功
//========================================================
int svt_getofflineserial(int nsamsck,unsigned char *lpserial)
{
	unsigned char ulen;
	unsigned char sztmp[256];
	unsigned char response_len;
	unsigned short response_sw;
	LPTINITCONSUME lpinitconsume;
	int nresult=-1;


	ulen=0;

	memcpy(sztmp, "\x80\x50\x01\x02\x0B", 5);

	ulen += 5;

	sztmp[5]=g_saminf[nsamsck].keyidx;
	ulen += 1;
	sztmp[ulen] = 0;
	ulen += 1;
	sztmp[ulen] = 0;
	ulen += 1;
	sztmp[ulen] = 0;
	ulen += 1;
	sztmp[ulen] = 0;
	ulen += 1;

	memcpy(sztmp+ulen, g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);
	ulen += g_saminf[nsamsck].len_terminal_id;
	response_len = MifareProCom(ulen, sztmp, &response_sw);
	if (response_sw==0)
	{
		nresult = -1;
	}
	else if (0x9000!=response_sw)
	{
		nresult = -1;
	}
	else
	{
		lpinitconsume = (LPTINITCONSUME)sztmp;
		memcpy(lpserial, lpinitconsume->offlinedealserial, 2);
		nresult = 2;
	}

	return nresult;


}


//=================================================================================
//函 数 名: svt_read
//功    能: 读取地铁票的数据内容
//入口参数:
//          nmode:读卡方式,=0普通方式, =1全读方式，需要读取持卡人信息和所有历史
//出口参数:
//          lpcardinf:返回的数据缓冲区
//          当nmode=0时
//            发行基本信息:40Byte
//            公共信息:30Byte
//            交易辅助信息:48Byte
//            地铁信息:48Byte
//            应用控制信息:32Byte
//            钱包值:4Byte
//            最近一次历史记录:23Byte
//          当nmode=1时
//            发行基本信息:40Byte
//            公共信息:30Byte
//            交易辅助信息:48Byte
//            地铁信息:48Byte
//            应用控制信息:32Byte
//            钱包值:4Byte
//            持卡人个人信息:23Byte
//            10条历史记录:230Byte
//返 回 值:
//        >0: 返回的数据长度
//        <0: 操作出错
//=================================================================================
int svt_read(int opertion, unsigned char *lpcardinf)
{
	//unsigned char CardNum = 0;
	unsigned char status=0, snr[12];//, snr1[5];
	unsigned char sztmp[256];
	//unsigned char ucTokenCnt=0;
	//unsigned char atqa, firstsnr[7];
	unsigned char ulen = 0;
	unsigned int i;
	unsigned int data_len;
	unsigned short sam_sw;
	int nresult=-1;
	//		struct timeval end_tv;
	//		unsigned long bt,et;
	//
	//		gettimeofday(&end_tv, NULL);
	//		bt=end_tv.tv_sec*1000000L+end_tv.tv_usec;
	//	//	dbg_formatvar("begin=%d:%d",end_tv.tv_sec, end_tv.tv_usec);

	for (i=0; i<3; i++)
	{
		status=ISO14443A_Request( 0x52, sztmp );
		if (status == 0) break;
	}

	if (status == 0)
	{

		status=ISO14443A_Anticoll(snr);

		if (status==0)
		{
			status=ISO14443A_Select(snr, sztmp);

		}
	}

	if (status == 0)
	{

		do
		{
			status =  MifareProRst(0x40,ulen,sztmp);
			if (status==0)
			{
				nresult = -1;
				break;
			}

			//add test 2016-02-25
// 			ulen = 19;
// 			memcpy(sztmp, "\x00\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", ulen);
// 			ulen = MifareProCom(ulen, sztmp, &sam_sw);


			ulen = 7;
			memcpy(sztmp, "\x00\xA4\x00\x00\x02\x3F\x00", ulen);
			status = MifareProCom(ulen, sztmp, &sam_sw);
			if ((status==0) || (0x9000!=sam_sw))
			{

				nresult = -2;
				break;
			}


			//发行区信息
			ulen = 5;
			memcpy(sztmp, "\x00\xB0\x85\x00\x28", ulen);
			status = MifareProCom(ulen, sztmp, &sam_sw);
			if ((status==0) || (0x9000!=sam_sw))
			{
				nresult = -2;
				break;
			}
			memcpy(g_metroinf.physicalnumber+4, snr, 4);
			g_metroinf.testflag = sztmp[6];
			data_len = 0;
			memcpy(lpcardinf+data_len, sztmp, 40);
			data_len += 40;


			memcpy(g_metroinf.logicnumber, sztmp+8, 8);


			ulen = 14;
			memcpy(sztmp, "\x00\xA4\x04\x00\x09\xF0\x00\x00\x41\x00\x53\x20\xAD\xF1", ulen);
			status = MifareProCom(ulen, sztmp, &sam_sw);
			if ((status==0) || (0x9000!=sam_sw))
			{
				nresult = -2;
				break;
			}


			//公共信息区
			ulen = 5;
			memcpy(sztmp, "\x00\xB0\x95\x00\x1E", ulen);
			status = MifareProCom(ulen, sztmp, &sam_sw);
			if ((status==0) || (0x9000!=sam_sw))
			{
				nresult = -2;
				break;
			}
			memcpy(lpcardinf+data_len, sztmp, 30);
			data_len += 30;

			//交易辅助信息
			ulen = 5;
			memcpy(sztmp, "\x00\xB2\x01\xBC\x00", ulen);
			status = MifareProCom(ulen, sztmp, &sam_sw);
			if ((status==0) || (0x9000!=sam_sw))
			{
				nresult = -2;
				break;
			}
			memcpy(lpcardinf+data_len, sztmp, 48);
			data_len += 48;

			//地铁信息区
			ulen = 5;
			memcpy(sztmp, "\x00\xB2\x02\xBC\x00", ulen);
			status = MifareProCom(ulen, sztmp, &sam_sw);
			if ((status==0) || (0x9000!=sam_sw))
			{
				nresult = -2;
				break;
			}
			memcpy(g_metroinf.mtrinf, sztmp, 48);
			memcpy(lpcardinf+data_len, sztmp, 48);
			data_len += 48;


			//应用控制信息区
			ulen = 5;
			memcpy(sztmp, "\x00\xB0\x91\x00\x1E", ulen);
			status = MifareProCom(ulen, sztmp, &sam_sw);
			if ((status==0) || (0x9000!=sam_sw))
			{
				nresult = -2;
				break;
			}
			memcpy(lpcardinf+data_len, sztmp, 32);
			data_len += 32;

			//805C000204;
			//钱包
			ulen = 5;
			memcpy(sztmp, "\x80\x5C\x00\x02\x04", ulen);
			status = MifareProCom(ulen, sztmp, &sam_sw);
			if ((status==0) || (0x9000!=sam_sw))
			{
				nresult = -2;
				break;
			}
			memcpy(lpcardinf+data_len, sztmp, 4);
			data_len += 4;

			if (opertion==0)
			{
				//最后一次历史
				ulen = 5;
				memcpy(sztmp, "\x00\xB2\x01\xC4\x00", ulen);
				status = MifareProCom(ulen, sztmp, &sam_sw);
				if (status==0)
				{
					//nresult = -2;
					break;
				}
				memcpy(lpcardinf+data_len, sztmp, 23);
				data_len += 23;

			}
			else
			{

				//持卡人信息
				ulen = 5;
				memcpy(sztmp, "\x00\xB0\x96\x00\x37", ulen);
				status = MifareProCom(ulen, sztmp, &sam_sw);
				if ((status==0) || (0x9000!=sam_sw))
				{
					nresult = -2;
					break;
				}

				memcpy(lpcardinf+data_len, sztmp, 55);
				data_len += 55;

				//读历史10条
				for (i=1;i<11;i++)
				{
					ulen = 5;
					memcpy(sztmp, "\x00\xB2\x01\xC4\x00", ulen);
					sztmp[2]=i;
					status = MifareProCom(ulen, sztmp, &sam_sw);
					if ((status==0) || (0x9000!=sam_sw))
					{
						//nresult = -2;
						break;
					}
					memcpy(lpcardinf+data_len, sztmp, 23);
					data_len += 23;
				}

			}



			nresult = data_len;


		}
		while (0);

	}

	//		gettimeofday(&end_tv, NULL);
	//		et=end_tv.tv_sec*1000000L+end_tv.tv_usec;
	//	//	dbg_formatvar("end=%d:%d",end_tv.tv_sec, end_tv.tv_usec);
	//
	//		dbg_formatvar("read sum=%d",(et-bt)/1000);

	return(nresult);

}


//========================================================
//功能:对地铁svt 卡进行充值
//入口参数
//	 nsamsck=sam卡对应卡槽
//    nstep=当=0时表示获取充值信息，=1时表示对票卡进行充值操作
//    value=要充到票卡内的金额(  分)
//    lpinf =当nstep=1时为输入数据
//            输入数据结构:
//                       交易时间7Byte
//                        MAC2   4Byte
//
//出口参数
//       pinf = 输出数据
//          nstep=0   输出数据结构:
//                               余额4Byte,
//                       联机交易序号2Byte,
//                           密钥版本1Byte,
//                           算法标识1Byte,
//                           伪随机数4Byte,
//                                       MAC1  4Byte
//                脱机交易序号2Byte,
//                      终端机序号4Byte
//          nstep=1   输出数据结构:
//                                        TAC   4Byte
//返回值
//       0=成功
//========================================================
int svt_recharge(unsigned char trType, unsigned char * p_timenow, int nsamsck, int nstep, unsigned long value, unsigned char *lpinf)
{

	unsigned char respone_len = 0;
	unsigned char sam_len = 0;
	unsigned char sztmp[256] = {0};
	unsigned short sam_sw = 0;
	unsigned char offline_terminal[6] = {0};
	TPSAMGETMAC PsamGetMac1;
	LPTINITCONSUME lpinitconsume = NULL;
	int nresult=-1;
	int i;

	do
	{
		memset(&PsamGetMac1, 0, sizeof(PsamGetMac1));

		if (nstep==0)
		{

			sam_len = 0;
			memcpy(sztmp, "\x80\x50\x01\x02\x0B",5);
			sam_len += 5;
			memcpy(sztmp+sam_len, &g_saminf[nsamsck].keyidx,1);
			sam_len += 1;
			sztmp[sam_len] = 0;
			sam_len += 1;
			sztmp[sam_len] = 0;
			sam_len += 1;
			sztmp[sam_len] = 0;
			sam_len += 1;
			sztmp[sam_len] = 0;
			sam_len += 1;
			memcpy(sztmp+sam_len, g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);
			sam_len += g_saminf[nsamsck].len_terminal_id;

			//g_Record.log_out(respone_len, level_error, "MifareProCom1(sztmp=%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x)",sztmp[0],sztmp[1],sztmp[2],sztmp[3],sztmp[4],sztmp[5],sztmp[6],sztmp[7],sztmp[8],sztmp[9],sztmp[10],sztmp[11],sztmp[12],sztmp[13],sztmp[14],sztmp[15],sztmp[16],sztmp[17]);
			//g_Record.log_out(respone_len, level_error, "MifareProCom1(sztmp=%02x)",sztmp[0],sztmp[0],sztmp[0],sztmp[0],sztmp[0],);

			respone_len = MifareProCom(sam_len,sztmp, &sam_sw);
			if (( respone_len==0 ) || (0x9000!=sam_sw))
			{
			    g_Record.log_out(respone_len, level_error, "svt_recharge.MifareProCom(sam_sw=%d)",sam_sw);
				nresult = -1;
				break;
			}

			//g_Record.log_out(0, level_error, "MifareProCom(outdata)");
			//for(i=0;i<respone_len;i++)
            //{
                //g_Record.log_out(0, level_error, "MifareProCom(sztmp=%02x)",sztmp[i]);
            //}

			memcpy(offline_terminal, sztmp+4, 2);
			lpinitconsume = (LPTINITCONSUME)sztmp;

     		PsamGetMac1.encryver = lpinitconsume->encryver;
			PsamGetMac1.calcmarker = lpinitconsume->calcmarker;
			memcpy(PsamGetMac1.offlinedealserial, lpinitconsume->offlinedealserial, 2);
			memcpy(PsamGetMac1.random, lpinitconsume->random, 4);
			memcpy(PsamGetMac1.tm, p_timenow, 7);

			PsamGetMac1.trflag = trType;

			memcpy(PsamGetMac1.logicnumber, g_metroinf.logicnumber, 8);

			PsamGetMac1.money[0] = 0;

			PsamGetMac1.money[1] = 0;

			PsamGetMac1.money[2] = 0;

			PsamGetMac1.money[3] = 0;

			memcpy(sztmp,"\x80\x70\x00\x00\x1C",5);
			sam_len = 5;
			memcpy(sztmp+5, (char *)&PsamGetMac1, sizeof(PsamGetMac1));
			sam_len += sizeof(PsamGetMac1);

			g_Record.log_out(respone_len, level_error, "sam_cmd1(sztmp=%02x%02x%02x%02x%02x%02x%02x%02x%02x)",sztmp[0],sztmp[1],sztmp[2],sztmp[3],sztmp[4],sztmp[5],sztmp[6],sztmp[7],sztmp[8]);


			respone_len = sam_cmd(nsamsck,sam_len,sztmp, sztmp, &sam_sw);

			//g_Record.log_out(0, level_error, "sam_cmd(outdata)");
			//for(i=0;i<sam_len;i++)
            //{
                //g_Record.log_out(0, level_error, "sam_cmd(sztmp=%02x)",sztmp[i]);
            //}

			if ((respone_len==0)||(0x9000!=sam_sw))
			{
			    g_Record.log_out(respone_len, level_error, "svt_recharge.sam_cmd(sam_sw=%d)",sam_sw);
				nresult = -2;
				break;
			}
			memcpy(offline_terminal+2, sztmp, 4);

			sam_len = 8;
			memcpy(sztmp, "\x00\x20\x00\x00\x03\x12\x34\x56",8);
			//g_Record.log_out(respone_len, level_error, "MifareProCom11(sztmp=%02x%02x%02x%02x%02x%02x%02x%02x)",sztmp[0],sztmp[1],sztmp[2],sztmp[3],sztmp[4],sztmp[5],sztmp[6],sztmp[7]);

			respone_len = MifareProCom(sam_len,sztmp, &sam_sw);

			//g_Record.log_out(0, level_error, "MifareProCom2(outdata)");
			//for(i=0;i<respone_len;i++)
            //{
                //g_Record.log_out(0, level_error, "MifareProCom2(sztmp=%02x)",sztmp[i]);
            //}

			if ((respone_len==0 ) || (0x9000!=sam_sw))
			{
			    g_Record.log_out(respone_len, level_error, "svt_recharge.MifareProCom11(sam_sw=%d)",sam_sw);
				nresult = -3;
				break;

			}


			//获取充值信息
			sam_len = 0;
			memcpy(sztmp, "\x80\x50\x00\x02\x0B",5);
			sam_len += 5;
			memcpy(sztmp+sam_len, &g_saminf[nsamsck].keyidx,1);
			sam_len += 1;
			sztmp[sam_len] = ((value >> 24) & 0xFF);
			sam_len += 1;
			sztmp[sam_len] = ((value >> 16) & 0xFF);
			sam_len += 1;
			sztmp[sam_len] = ((value >> 8) & 0xFF);
			sam_len += 1;
			sztmp[sam_len] = (value & 0xFF);
			sam_len += 1;
			memcpy(sztmp+sam_len, g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);
			sam_len += g_saminf[nsamsck].len_terminal_id;

			//g_Record.log_out(respone_len, level_error, "MifareProCom2(sztmp=%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x)",sztmp[0],sztmp[1],sztmp[2],sztmp[3],sztmp[4],sztmp[5],sztmp[6],sztmp[7],sztmp[8],sztmp[9],sztmp[10],sztmp[11],sztmp[12],sztmp[13],sztmp[14],sztmp[15],sztmp[16],sztmp[17]);



			respone_len = MifareProCom(sam_len,sztmp, &sam_sw);


			//g_Record.log_out(0, level_error, "MifareProCom3(outdata)");
			//for(i=0;i<respone_len;i++)
            //{
                //g_Record.log_out(0, level_error, "MifareProCom3(sztmp=%02x)",sztmp[i]);
            //}

			if ((respone_len==0 ) || (0x9000!=sam_sw))
			{
			    g_Record.log_out(respone_len, level_error, "svt_recharge.MifareProCom22(sam_sw=%d)",sam_sw);
				nresult = -3;
				break;

			}


			memcpy(lpinf, sztmp, 16);
			memcpy(lpinf+16, offline_terminal, 6);

			nresult = 0;

		}
		else
		{
			//执行充值, 必须在获取充值信息后，中间不能执行其它操作
			sam_len = 0;
			memcpy(sztmp, "\x80\x52\x00\x00\x0B",5);
			sam_len += 5;
			memcpy(sztmp+sam_len, lpinf,11);
			sam_len += 11;

			//g_Record.log_out(0, level_error, "MifareProCom33(sztmp=%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x)",sztmp[0],sztmp[1],sztmp[2],sztmp[3],sztmp[4],sztmp[5],sztmp[6],sztmp[7],sztmp[8],sztmp[9],sztmp[10],sztmp[11],sztmp[12],sztmp[13],sztmp[14],sztmp[15]);

			respone_len = MifareProCom(sam_len,sztmp, &sam_sw);

			//g_Record.log_out(0, level_error, "MifareProCom4(outdata)");
			//for(i=0;i<respone_len;i++)
            //{
                //g_Record.log_out(0, level_error, "MifareProCom4(sztmp=%02x)",sztmp[i]);
            //}

			if ((respone_len==0 ) || (0x9000!=sam_sw))
			{
			    g_Record.log_out(respone_len, level_error, "svt_recharge.MifareProCom33(sam_sw=%d)",sam_sw);
				nresult = -4;
				break;
			}

			/*
			if ((respone_len==0 ) || (0x9000!=sam_sw))
			{
				g_Record.log_out(respone_len, level_error, "svt_recharge.MifareProCom33(sam_sw=%d)",sam_sw);
				//8052失败重做一次，从8050开始
				//获取充值信息
				sam_len = 0;
				memcpy(sztmp, "\x80\x50\x00\x02\x0B",5);
				sam_len += 5;
				memcpy(sztmp+sam_len, &g_saminf[nsamsck].keyidx,1);
				sam_len += 1;
				sztmp[sam_len] = ((value >> 24) & 0xFF);
				sam_len += 1;
				sztmp[sam_len] = ((value >> 16) & 0xFF);
				sam_len += 1;
				sztmp[sam_len] = ((value >> 8) & 0xFF);
				sam_len += 1;
				sztmp[sam_len] = (value & 0xFF);
				sam_len += 1;
				memcpy(sztmp+sam_len, g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);
				sam_len += g_saminf[nsamsck].len_terminal_id;

				//g_Record.log_out(respone_len, level_error, "MifareProCom2(sztmp=%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x)",sztmp[0],sztmp[1],sztmp[2],sztmp[3],sztmp[4],sztmp[5],sztmp[6],sztmp[7],sztmp[8],sztmp[9],sztmp[10],sztmp[11],sztmp[12],sztmp[13],sztmp[14],sztmp[15],sztmp[16],sztmp[17]);

				respone_len = MifareProCom(sam_len,sztmp, &sam_sw);

				//g_Record.log_out(0, level_error, "MifareProCom3(outdata)");
				//for(i=0;i<respone_len;i++)
				//{
					//g_Record.log_out(0, level_error, "MifareProCom3(sztmp=%02x)",sztmp[i]);
				//}

				if ((respone_len==0 ) || (0x9000!=sam_sw))
				{
					g_Record.log_out(respone_len, level_error, "svt_recharge.MifareProCom44(sam_sw=%d)",sam_sw);
					nresult = -4;
					break;

				}

				//执行充值, 必须在获取充值信息后，中间不能执行其它操作
				sam_len = 0;
				memcpy(sztmp, "\x80\x52\x00\x00\x0B",5);
				sam_len += 5;
				memcpy(sztmp+sam_len, lpinf,11);
				sam_len += 11;

				respone_len = MifareProCom(sam_len,sztmp, &sam_sw);

				if ((respone_len==0 ) || (0x9000!=sam_sw))
				{

					g_Record.log_out(respone_len, level_error, "svt_recharge.MifareProCom55(sam_sw=%d)",sam_sw);
					nresult = -5;
					break;
				}
			}
			*/



			memcpy(lpinf, sztmp, 4);
			nresult = 0;
		}

	} while (0);

	return(nresult);

}

//========================================================
//功能:对地铁票进行脱机消费
//入口参数
//	 nsamsck=sam卡对应卡槽
//    value = 消费金额
//    trtype=交易类型, 06=普通消费,09=复合消费
//    lpmetrofile=当trtype=09  时，作为复合文件更新的数据内容，普通消费时不使用
//出口参数
//       respone_data=SAM终端号(6BYTE) +  SAM终端流水( 4Byte) + TAC (4Byte)
//返回值
//       0=成功
//========================================================
int svt_consum(unsigned char * p_timenow, int nsamsck, unsigned char *lplogicnumber, unsigned long value,unsigned char trtype, unsigned char *lpmetrofile, unsigned char *respone_data)
{
	unsigned char sztmp[512];
	unsigned char szmac[8];
	unsigned char ulen;
	unsigned char respone_len;
	unsigned short sam_sw;
	TPSAMGETMAC PsamGetMac1;
	LPTINITCONSUME lpinitconsume;
	LPTOUTMTRW lpout=(LPTOUTMTRW)respone_data;
	int nresult=-1;

	do
	{
		ulen=0;
		if (0x06==trtype)
		{
			memcpy(sztmp, "\x80\x50\x01\x02\x0B", 5);
		}
		else if (0x09==trtype)
		{
			memcpy(sztmp, "\x80\x50\x03\x02\x0B", 5);
		}
		else
		{
			nresult = -9; //参数错误
			break;
		}
		ulen += 5;

		sztmp[5]=g_saminf[nsamsck].keyidx;
		ulen += 1;
		sztmp[ulen] = ((value >> 24) & 0xFF);
		ulen += 1;
		sztmp[ulen] = ((value >> 16) & 0xFF);
		ulen += 1;
		sztmp[ulen] = ((value >> 8) & 0xFF);
		ulen += 1;
		sztmp[ulen] = (value & 0xFF);
		ulen += 1;

		memcpy(sztmp+ulen, g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);
		ulen += g_saminf[nsamsck].len_terminal_id;
		//		dbg_dumpmemory("8050=",sztmp,ulen);
		respone_len = MifareProCom(ulen, sztmp, &sam_sw);
		//		dbg_dumpmemory("8050-",sztmp,respone_len);
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -3;
			break;
		}

		lpinitconsume = (LPTINITCONSUME)sztmp;

		PsamGetMac1.encryver = lpinitconsume->encryver;
		PsamGetMac1.calcmarker = lpinitconsume->calcmarker;
		memcpy(PsamGetMac1.offlinedealserial, lpinitconsume->offlinedealserial, 2);
		memcpy(PsamGetMac1.random, lpinitconsume->random, 4);
		memcpy(PsamGetMac1.tm, p_timenow, 7);


		PsamGetMac1.trflag = trtype;



		memcpy(PsamGetMac1.logicnumber, lplogicnumber, 8);
		PsamGetMac1.money[0] = ((value >> 24) & 0xFF);

		PsamGetMac1.money[1] = ((value >> 16) & 0xFF);

		PsamGetMac1.money[2] = ((value >> 8) & 0xFF);

		PsamGetMac1.money[3] = (value & 0xFF);

		if (0x09==trtype)
		{

			memcpy(sztmp, "\x80\xDC\x02\xBC\x30", 5);
			ulen = 5;
			memcpy(sztmp+ulen, g_metroinf.mtrinf, 3);
			memcpy(sztmp+ulen, "\x02\x2E\x00", 3);
			ulen += 3;
			memcpy(sztmp+ulen, lpmetrofile, 45);
			ulen += 45;

			respone_len = MifareProCom(ulen, sztmp, &sam_sw);
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				//					dbg_formatvar("80dc=%04X",sam_sw);
				nresult = -4;
				break;
			}



		}


		memcpy(sztmp,"\x80\x70\x00\x00\x1C",5);
		ulen = 5;
		memcpy(sztmp+5, (char *)&PsamGetMac1, sizeof(PsamGetMac1));
		ulen += sizeof(PsamGetMac1);
		//		dbg_dumpmemory("8070=",sztmp,ulen);
		respone_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &sam_sw);
		//		dbg_dumpmemory("8070-",sztmp,respone_len);


		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			//if (0x9000==sam_sw)
			nresult = -5;
			break;
		}

		memcpy(lpout->sam_terminalserial, sztmp, 4);
		memcpy(lpout->sam_terminalnumber, g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);

		memcpy(szmac, sztmp, 8);
		memcpy(sztmp,"\x80\x54\x01\x00\x0F", 5);
		ulen = 5;
		memcpy(sztmp+ulen, szmac, 4);
		ulen += 4;
		memcpy(sztmp+ulen, PsamGetMac1.tm, 7);
		ulen += 7;
		memcpy(sztmp+ulen, szmac+4, 4);
		ulen += 4;

		//		dbg_dumpmemory("8054=",sztmp,ulen);
		respone_len = MifareProCom(ulen, sztmp, &sam_sw);
		//		dbg_dumpmemory("8054-",sztmp,respone_len);

		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -6;
			break;
		}
		memcpy(lpout->tac, sztmp, 4);
		memcpy(szmac, sztmp, 8);

		memcpy(sztmp,"\x80\x72\x00\x00\x04",5);
		ulen = 5;
		memcpy(sztmp+5, szmac+4, 4);
		ulen += 4;
		//respone_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &sam_sw);
		Extra::sam_seq_add(nsamsck, sztmp, ulen);

		nresult = 0;
	}
	while (0);

	return (nresult);

}





//===============================================================
//功能:对SVT票的应用控制文件进行更新
//入口参数
//	 nsamsck=sam卡对应卡槽
//       file_id=短文件标识，目前只支持0x11(应用控制文件)和0x16(个人信息文件)
//        offset=要更新文件的起始地址
//           nle=要更新的数据长度
//         lpinf=要写入的数据

//
//出口参数
//	无
//返回值
//       0=成功
//===============================================================
int svt_update_binfile(int nsamsck,unsigned char *lplogicnumber,int file_id, int offset, int nlen, unsigned char *lpinf)
{

	unsigned char respone_len;
	unsigned char sam_len;
	unsigned char sztmp[256];
	unsigned char random[8];
	unsigned char fulldata[8]={0x80,0x00,0x00,0x00,0x00,0x00,0x00, 0x00};
	unsigned char data_len;
	unsigned char full_len;
	unsigned short sam_sw;
	unsigned char paramter_p1;
	//unsigned char offline_terminal[6];
	//TPSAMGETMAC PsamGetMac1;
	//LPTINITCONSUME lpinitconsume;
	int nresult=-1;


	do
	{

		if ((file_id==0x11) ||((file_id==0x16)))
		{
			paramter_p1=  (0x11 & 0x1F) | 0x80; //短文件0x11  ,  应用控制区

		}
		else
		{
			nresult = -2;
			break;
		}


		sam_len = 5;
		memcpy(sztmp, "\x00\x84\x00\x00\x04",5);


		respone_len = MifareProCom(sam_len,sztmp, &sam_sw);


		if (( respone_len==0 ) || (0x9000!=sam_sw))
		{
			nresult = -1;
			break;
		}
		memset(random, 0, sizeof(random));
		memcpy(random, sztmp, 4);

		sam_len = 5;
		memcpy(sztmp, "\x80\x1A\x26\x03\x08",5);
		memcpy(sztmp+sam_len, lplogicnumber, 8);//g_metroinf.logicnumber, 8);
		sam_len += 8;

		respone_len = sam_cmd(nsamsck, sam_len, sztmp, sztmp, &sam_sw);

		if (( respone_len==0 ) || (0x9000!=sam_sw))
		{
			nresult = -1;
			break;
		}


		data_len = 8+5+nlen;//random+ins_cmd+data

		if (data_len%8)
		{
			full_len = 8 - (data_len%8);
		}
		else
		{
			full_len = 0;
		}


		sam_len = 0;
		memcpy(sztmp, "\x80\xFA\x05\x00\x10",5);
		sztmp[4] = data_len + full_len;
		sam_len += 5;
		memcpy(sztmp+sam_len, random, 8);
		sam_len += 8;
		memcpy(sztmp+sam_len, "\x04\xD6\x96\x00\x06", 5);
		sztmp[sam_len+2]=paramter_p1;
		sztmp[sam_len+3]=offset;
		sztmp[sam_len+4]=nlen+4;//+mac
		sam_len += 5;
		memcpy(sztmp+sam_len, lpinf, nlen);
		sam_len += nlen;
		if (full_len)
		{
			memcpy(sztmp+sam_len, fulldata, full_len);
			sam_len += full_len;
		}



		respone_len = sam_cmd(nsamsck, sam_len, sztmp, sztmp, &sam_sw);

		if (( respone_len==0 ) || (0x9000!=sam_sw))
		{
			nresult = -1;
			break;
		}

		memcpy(random, sztmp, 4);
		memset(sztmp,0,sizeof(sztmp));
		memcpy(sztmp, "\x04\xD6\x96\x00\x06",5);
		sztmp[2]=paramter_p1;
		sztmp[3]=offset;
		sztmp[4]=nlen+4;//+mac
		sam_len = 5;
		memcpy(sztmp+sam_len,lpinf,nlen);
		sam_len += nlen;
		memcpy(sztmp+sam_len,random,4);
		sam_len += 4;

// 		if (full_len)
// 		{
// 			memcpy(sztmp+sam_len, fulldata, full_len);
// 			sam_len += full_len;
// 		}



		respone_len = MifareProCom(sam_len,sztmp, &sam_sw);


		if (( respone_len==0 ) || (0x9000!=sam_sw))
		{
			nresult = -1;
			break;
		}

		nresult = 0;



	}
	while (0);

	return(nresult);

}

//========================================================
//功能:对地铁票指定的记录进行更新
//入口参数
//    nsamsck=sam卡对应卡槽
//    lplogicnumber=要更新票卡的8字节逻辑卡号(应用序列号)
//    record_id=文档中对应的记录号
//    lock=锁定标识,=0不锁定
//    lpfile=复合文件更新的数据内容
//出口参数
//       respone_data=SAM终端号(6BYTE) +  SAM终端流水( 4Byte) + TAC (4Byte)
//返回值
//       0=成功
//========================================================
int svt_updata_complex(unsigned char * p_timenow, int nsamsck, unsigned char *lplogicnumber, unsigned char record_id, unsigned char lock, unsigned char *lpfile, unsigned char *respone_data)
{
	unsigned char sztmp[256];
	unsigned char szmac[8];
	unsigned char ulen;
	unsigned char response_len;
	unsigned short sam_sw;
	unsigned char real_len;
	const unsigned long value=0;
	TPSAMGETMAC PsamGetMac1;
	LPTINITCONSUME lpinitconsume;
	LPTOUTMTRW lpout=(LPTOUTMTRW)respone_data;
	int nresult=-1;

	do
	{
		ulen=0;
		memcpy(sztmp, "\x80\x50\x03\x02\x0B", 5);
		ulen += 5;

		sztmp[5]=g_saminf[nsamsck].keyidx;
		ulen += 1;
		sztmp[ulen] = ((value >> 24) & 0xFF);
		ulen += 1;
		sztmp[ulen] = ((value >> 16) & 0xFF);
		ulen += 1;
		sztmp[ulen] = ((value >> 8) & 0xFF);
		ulen += 1;
		sztmp[ulen] = (value & 0xFF);
		ulen += 1;

		memcpy(sztmp+ulen, g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);
		ulen += g_saminf[nsamsck].len_terminal_id;
		//		dbg_dumpmemory("8050=",sztmp,ulen);
		response_len = MifareProCom(ulen, sztmp, &sam_sw);
		//		dbg_dumpmemory("8050-",sztmp,response_len);
		if ((response_len==0) || (0x9000!=sam_sw))
		{
			nresult = -3;
			break;
		}

		lpinitconsume = (LPTINITCONSUME)sztmp;

		PsamGetMac1.encryver = lpinitconsume->encryver;
		PsamGetMac1.calcmarker = lpinitconsume->calcmarker;
		memcpy(PsamGetMac1.offlinedealserial, lpinitconsume->offlinedealserial, 2);
		memcpy(PsamGetMac1.random, lpinitconsume->random, 4);
		memcpy(PsamGetMac1.tm, p_timenow, 7);


		PsamGetMac1.trflag = 0x09;



		memcpy(PsamGetMac1.logicnumber, lplogicnumber, 8);//g_metroinf.logicnumber
		PsamGetMac1.money[0] = ((value >> 24) & 0xFF);

		PsamGetMac1.money[1] = ((value >> 16) & 0xFF);

		PsamGetMac1.money[2] = ((value >> 8) & 0xFF);

		PsamGetMac1.money[3] = (value & 0xFF);

		if (record_id==0x11)
		{
			real_len = _METROFILE_SIZE2_;
		}
		else
		{
			real_len = _METROFILE_SIZE1_;
		}

		memcpy(sztmp, "\x80\xDC\x02\xB8\x30", 5);
		sztmp[2] = record_id;
		sztmp[4] = real_len+2;
		ulen = 5;
		sztmp[ulen++] = record_id;
		sztmp[ulen++] = real_len;
		sztmp[ulen++] = lock;
		memcpy(sztmp+ulen, lpfile, real_len-1);//45);

		//memcpy(sztmp+ulen, g_metroinf.mtrinf, 3);
		//memcpy(sztmp+ulen, lpfile, 48);
		ulen += real_len-1;//45;

		response_len = MifareProCom(ulen, sztmp, &sam_sw);

		if ((response_len==0) || (0x9000!=sam_sw))
		{
			nresult = -4;
			break;
		}


		memcpy(sztmp,"\x80\x70\x00\x00\x1C",5);
		ulen = 5;
		memcpy(sztmp+5, (char *)&PsamGetMac1, sizeof(PsamGetMac1));
		ulen += sizeof(PsamGetMac1);
		response_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &sam_sw);


		if ((response_len==0) || (0x9000!=sam_sw))
		{
			nresult = -5;
			break;
		}

		memcpy(lpout->sam_terminalserial, sztmp, 4);
		memcpy(lpout->sam_terminalnumber, g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);

		memcpy(szmac, sztmp, 8);
		memcpy(sztmp,"\x80\x54\x01\x00\x0F", 5);
		ulen = 5;
		memcpy(sztmp+ulen, szmac, 4);
		ulen += 4;
		memcpy(sztmp+ulen, PsamGetMac1.tm, 7);
		ulen += 7;
		memcpy(sztmp+ulen, szmac+4, 4);
		ulen += 4;

		response_len = MifareProCom(ulen, sztmp, &sam_sw);

		if ((response_len==0) || (0x9000!=sam_sw))
		{
			nresult = -6;
			break;
		}
		memcpy(lpout->tac, sztmp, 4);
		memcpy(szmac, sztmp, 8);

		memcpy(sztmp,"\x80\x72\x00\x00\x04",5);
		ulen = 5;
		memcpy(sztmp+5, szmac+4, 4);
		ulen += 4;
		//response_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &sam_sw);
		Extra::sam_seq_add(nsamsck, sztmp, ulen);

		nresult = 0;
	}
	while (0);


	return (nresult);

}

//===============================================================
//功能:对SVT票的复合文件指定记录进行更新
//入口参数
//	 nsamsck=sam卡对应卡槽
//  lplogicnumber=票卡逻辑卡号
//        record_id=记录ID
//           lock=锁定标记
//         lpinf=复合文件对应要写入的数据(要与读取时获得的相同)

//
//出口参数
//	无
//返回值
//       0=成功
//===============================================================
int svt_unlock_recordfile(int nsamsck,unsigned char *lplogicnumber,int record_id, unsigned char lock, unsigned char *lpinf)
{

	unsigned char respone_len;
	unsigned char sam_len;
	unsigned char sztmp[256];
	unsigned char random[8];
	unsigned char fulldata[8]={0x80,0x00,0x00,0x00,0x00,0x00,0x00, 0x00};
	unsigned char data_len;
	unsigned char full_len;
	unsigned short sam_sw;
	unsigned char real_len;
	int nresult=-1;


	do
	{
		sam_len = 5;
		memcpy(sztmp, "\x00\x84\x00\x00\x04",5);

		respone_len = MifareProCom(sam_len,sztmp, &sam_sw);

		if (( respone_len==0 ) || (0x9000!=sam_sw))
		{
			nresult = -1;
			break;
		}
		memset(random, 0, sizeof(random));
		memcpy(random, sztmp, 4);

		sam_len = 5;
		memcpy(sztmp, "\x80\x1A\x26\x03\x08",5);
		memcpy(sztmp+sam_len, lplogicnumber, 8);
		sam_len += 8;
		respone_len = sam_cmd(nsamsck, sam_len, sztmp, sztmp, &sam_sw);
		if (( respone_len==0 ) || (0x9000!=sam_sw))
		{
			nresult = -2;
			break;
		}

		if (record_id==0x11)
		{
			real_len = _METROFILE_SIZE2_;
		}
		else
		{
			real_len = _METROFILE_SIZE1_;
		}


		data_len = 8+5+real_len+2;//random+ins_cmd+data

		if (data_len%8)
		{
			full_len = 8 - (data_len%8);
		}
		else
		{
			full_len = 0;
		}



		sam_len = 0;
		memcpy(sztmp, "\x80\xFA\x05\x00\x10",5);
		sztmp[4] = data_len + full_len;
		sam_len += 5;
		memcpy(sztmp+sam_len, random, 8);
		sam_len += 8;
		memcpy(sztmp+sam_len, "\x04\xDC\x00\x00\x06", 5);
		sztmp[sam_len+2]=record_id;
		sztmp[sam_len+3]=0xB8; //(((fild_id << 3) & 0xF8) | 0x04);
		sztmp[sam_len+4]=real_len+2+4;//+mac
		sam_len += 5;
		sztmp[sam_len++]=record_id;
		sztmp[sam_len++]=real_len;
		sztmp[sam_len++]=lock;

		memcpy(sztmp+sam_len, lpinf, real_len-1);

		sam_len += real_len-1;

		if (full_len)
		{
			memcpy(sztmp+sam_len, fulldata, full_len);
			sam_len += full_len;
		}


		respone_len = sam_cmd(nsamsck, sam_len, sztmp, sztmp, &sam_sw);
		if (( respone_len==0 ) || (0x9000!=sam_sw))
		{
			nresult = -3;
			break;
		}

		memcpy(random, sztmp, 4);
		memcpy(sztmp, "\x04\xDC\x96\x00\x06",5);
		sztmp[2]=record_id;
		sztmp[3]=0xB8;
		sztmp[4]=real_len+2+4;//+mac
		sam_len = 5;
		sztmp[sam_len++]=record_id;
		sztmp[sam_len++]=real_len;
		sztmp[sam_len++]=lock;


		memcpy(sztmp+sam_len,lpinf,real_len-1);
		sam_len += real_len-1;
		memcpy(sztmp+sam_len,random,4);
		sam_len += 4;

		respone_len = MifareProCom(sam_len,sztmp, &sam_sw);

		if (( respone_len==0 ) || (0x9000!=sam_sw))
		{
			nresult = -4;
			break;
		}

		nresult = 0;



	}
	while (0);

	g_Record.log_out(0, level_disaster, "svt_unlock_recordfile:%d\r\n", nresult);

	return(nresult);

}

//========================================================
//功能:对地铁票指定的记录进行更新
//入口参数
//    nsamsck=sam卡对应卡槽
//    lplogicnumber=要更新票卡的8字节逻辑卡号(应用序列号)
//    recordcnt=要更新的记录总数
//    dim_record_id=要更新的多条记录号数组
//    dim_lock=对应多条记录的锁定标识数组,=0不锁定
//    dim_lpfile=复合文件更新的多条记录的数据内容，赋值时需要注意不同的记录号对应的数据长度( 数据内容为不含记录前三个特定数据的内容)
//出口参数
//       respone_data=SAM终端号(6BYTE) +  SAM终端流水( 4Byte) + TAC (4Byte)
//返回值
//       0=成功
//========================================================
uint16_t svt_updata_more_complex(unsigned char tac_type, unsigned char * p_time_now, int nsamsck,
								 unsigned char *lplogicnumber, long lAmount, unsigned char recordcnt,
								 unsigned char *dim_record_id, unsigned char *dim_lock, unsigned char *dim_lpfile, unsigned char *respone_data)
{
	unsigned char sztmp[256];
	unsigned char szmac[8];
	unsigned char ulen;
	unsigned char i;
	unsigned char response_len;
	unsigned short sam_sw;
	unsigned char real_len;
	int offset=0;
	TPSAMGETMAC PsamGetMac1;
	LPTINITCONSUME lpinitconsume;
	LPTOUTMTRW lpout=(LPTOUTMTRW)respone_data;

	uint16_t nresult=0;

	do
	{
		// 消费初始化
		ulen=0;
		memcpy(sztmp, "\x80\x50\x03\x02\x0B", 5);
		if (recordcnt == 0)
			sztmp[2] = 0x01;
		ulen += 5;

		sztmp[5]=g_saminf[nsamsck].keyidx;
		ulen += 1;
		sztmp[ulen] = ((lAmount >> 24) & 0xFF);
		ulen += 1;
		sztmp[ulen] = ((lAmount >> 16) & 0xFF);
		ulen += 1;
		sztmp[ulen] = ((lAmount >> 8) & 0xFF);
		ulen += 1;
		sztmp[ulen] = (lAmount & 0xFF);
		ulen += 1;

		memcpy(sztmp+ulen, g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);
		ulen += g_saminf[nsamsck].len_terminal_id;
		response_len = MifareProCom(ulen, sztmp, &sam_sw);
		if ((response_len==0) || (0x9000!=sam_sw))
		{
			nresult = 2;
			break;
		}

		lpinitconsume = (LPTINITCONSUME)sztmp;

		PsamGetMac1.encryver = lpinitconsume->encryver;
		PsamGetMac1.calcmarker = lpinitconsume->calcmarker;
		memcpy(PsamGetMac1.offlinedealserial, lpinitconsume->offlinedealserial, 2);
		memcpy(PsamGetMac1.random, lpinitconsume->random, 4);
		memcpy(PsamGetMac1.tm, p_time_now, 7);

		PsamGetMac1.trflag = tac_type;

		memcpy(PsamGetMac1.logicnumber, lplogicnumber, 8);//g_metroinf.logicnumber
		PsamGetMac1.money[0] = ((lAmount >> 24) & 0xFF);
		PsamGetMac1.money[1] = ((lAmount >> 16) & 0xFF);
		PsamGetMac1.money[2] = ((lAmount >> 8) & 0xFF);
		PsamGetMac1.money[3] = (lAmount & 0xFF);

		// 更新复合文件
		offset=0;
		for (i=0;i<recordcnt;i++)
		{
			//目前只有0x11文件长度是24, 未来有可能需要加入新记录标识
			if (dim_record_id[i]==0x11)
			{
				real_len = _METROFILE_SIZE2_;
			}
			else
			{
				real_len = _METROFILE_SIZE1_;
			}

			memcpy(sztmp, "\x80\xDC\x02\xB8\x30", 5);
			sztmp[2] = dim_record_id[i];
			sztmp[4] = real_len+2;
			ulen = 5;
			sztmp[ulen++] = dim_record_id[i];
			sztmp[ulen++] = real_len;
			sztmp[ulen++] = dim_lock[i];
			memcpy(sztmp+ulen, dim_lpfile+offset, real_len-1);


			ulen += real_len-1;

			response_len = MifareProCom(ulen, sztmp, &sam_sw);
			if ((response_len==0) || (0x9000!=sam_sw))
			{
				nresult = 2;
				break;
			}
			offset += real_len-1;

		}

		// SAM卡消费初始化，并获取SAM流水
		memcpy(sztmp,"\x80\x70\x00\x00\x1C",5);
		ulen = 5;
		memcpy(sztmp+5, (char *)&PsamGetMac1, sizeof(PsamGetMac1));
		ulen += sizeof(PsamGetMac1);
		response_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &sam_sw);
		if ((response_len==0) || (0x9000!=sam_sw))
		{
			nresult = 3;
			break;
		}

		memcpy(lpout->sam_terminalserial, sztmp, 4);
		memcpy(lpout->sam_terminalnumber, g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);

		// 消费并产生TAC码（前面所有到这一步都是个整体，具有事务特性）
		memcpy(szmac, sztmp, 8);
		memcpy(sztmp,"\x80\x54\x01\x00\x0F", 5);
		ulen = 5;
		memcpy(sztmp+ulen, szmac, 4);
		ulen += 4;
		memcpy(sztmp+ulen, PsamGetMac1.tm, 7);
		ulen += 7;
		memcpy(sztmp+ulen, szmac+4, 4);
		ulen += 4;

		response_len = MifareProCom(ulen, sztmp, &sam_sw);

		if ((response_len==0) || (0x9000!=sam_sw))
		{
			nresult = 4;
			break;
		}
		memcpy(lpout->tac, sztmp, 4);
		memcpy(szmac, sztmp, 8);

		memcpy(sztmp,"\x80\x72\x00\x00\x04",5);
		ulen = 5;
		memcpy(sztmp+5, szmac+4, 4);
		ulen += 4;
		//response_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &sam_sw);
		Extra::sam_seq_add(nsamsck, sztmp, ulen);

		nresult = 0;

	} while (0);

	return (nresult);
}

uint16_t updata_more_complex(unsigned char * p_time_now, int nsamsck, unsigned char *lplogicnumber, long lAmount, unsigned char recordcnt, PCOMPLEX_FILE complex_arr, unsigned char *respone_data)
{
	unsigned char sztmp[256];
	unsigned char szmac[8];
	unsigned char ulen;
	unsigned char i;
	unsigned char response_len;
	unsigned short sam_sw;
	TPSAMGETMAC PsamGetMac1;
	LPTINITCONSUME lpinitconsume;
	LPTOUTMTRW lpout=(LPTOUTMTRW)respone_data;

	uint16_t nresult=0;

	do
	{
		// 消费初始化
		ulen=0;
		memcpy(sztmp, "\x80\x50\x03\x02\x0B", 5);
		if (recordcnt == 0)
			sztmp[2] = 0x01;
		ulen += 5;

		sztmp[5]=g_saminf[nsamsck].keyidx;
		ulen += 1;
		sztmp[ulen] = ((lAmount >> 24) & 0xFF);
		ulen += 1;
		sztmp[ulen] = ((lAmount >> 16) & 0xFF);
		ulen += 1;
		sztmp[ulen] = ((lAmount >> 8) & 0xFF);
		ulen += 1;
		sztmp[ulen] = (lAmount & 0xFF);
		ulen += 1;

		memcpy(sztmp+ulen, g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);
		ulen += g_saminf[nsamsck].len_terminal_id;
		response_len = MifareProCom(ulen, sztmp, &sam_sw);
		if ((response_len==0) || (0x9000!=sam_sw))
		{
			nresult = 2;
			break;
		}

		lpinitconsume = (LPTINITCONSUME)sztmp;

		PsamGetMac1.encryver = lpinitconsume->encryver;
		PsamGetMac1.calcmarker = lpinitconsume->calcmarker;
		memcpy(PsamGetMac1.offlinedealserial, lpinitconsume->offlinedealserial, 2);
		memcpy(PsamGetMac1.random, lpinitconsume->random, 4);
		memcpy(PsamGetMac1.tm, p_time_now, 7);

		if (recordcnt == 0)
			PsamGetMac1.trflag = 0x06;
		else
			PsamGetMac1.trflag = 0x09;

		memcpy(PsamGetMac1.logicnumber, lplogicnumber, 8);//g_metroinf.logicnumber
		PsamGetMac1.money[0] = ((lAmount >> 24) & 0xFF);
		PsamGetMac1.money[1] = ((lAmount >> 16) & 0xFF);
		PsamGetMac1.money[2] = ((lAmount >> 8) & 0xFF);
		PsamGetMac1.money[3] = (lAmount & 0xFF);

		// 更新复合文件
		for (i=0;i<recordcnt;i++)
		{
			memcpy(sztmp, "\x80\xDC\x02\xB8\x30", 5);
			sztmp[2] = complex_arr[i].id;
			sztmp[4] = complex_arr[i].len + 2;
			ulen = 5;
			sztmp[ulen++] = complex_arr[i].id;
			sztmp[ulen++] = complex_arr[i].len;
			sztmp[ulen++] = complex_arr[i].lock_flag;
			memcpy(sztmp+ulen, complex_arr[i].data, complex_arr[i].len - 1);


			ulen += complex_arr[i].len - 1;

			response_len = MifareProCom(ulen, sztmp, &sam_sw);
			if ((response_len==0) || (0x9000!=sam_sw))
			{
				nresult = 2;
				break;
			}
		}

		// SAM卡消费初始化，并获取SAM流水
		memcpy(sztmp,"\x80\x70\x00\x00\x1C",5);
		ulen = 5;
		memcpy(sztmp+5, (char *)&PsamGetMac1, sizeof(PsamGetMac1));
		ulen += sizeof(PsamGetMac1);
		response_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &sam_sw);
		if ((response_len==0) || (0x9000!=sam_sw))
		{
			nresult = 3;
			break;
		}

		memcpy(lpout->sam_terminalserial, sztmp, 4);
		memcpy(lpout->sam_terminalnumber, g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);

		// 消费并产生TAC码（前面所有到这一步都是个整体）
		memcpy(szmac, sztmp, 8);
		memcpy(sztmp,"\x80\x54\x01\x00\x0F", 5);
		ulen = 5;
		memcpy(sztmp+ulen, szmac, 4);
		ulen += 4;
		memcpy(sztmp+ulen, PsamGetMac1.tm, 7);
		ulen += 7;
		memcpy(sztmp+ulen, szmac+4, 4);
		ulen += 4;

		response_len = MifareProCom(ulen, sztmp, &sam_sw);

		if ((response_len==0) || (0x9000!=sam_sw))
		{
			nresult = 4;
			break;
		}
		memcpy(lpout->tac, sztmp, 4);
		memcpy(szmac, sztmp, 8);

		memcpy(sztmp,"\x80\x72\x00\x00\x04",5);
		ulen = 5;
		memcpy(sztmp+5, szmac+4, 4);
		ulen += 4;
		//response_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &sam_sw);
		Extra::sam_seq_add(nsamsck, sztmp, ulen);

		nresult = 0;

	} while (0);

	return (nresult);
}

//========================================================
//功能:对一卡通票进行脱机消费
//入口参数
//	 nsamsck=sam卡对应卡槽
//    value = 消费金额
//    trtype=交易类型, 06=普通消费,09=复合消费
//    lpmetrofile=当trtype=09  时，作为复合文件更新的数据内容，普通消费时不使用
//出口参数
//       respone_data=SAM终端号(6BYTE) +  SAM终端流水( 4Byte) + TAC (4Byte)
//返回值
//       0=成功
//========================================================
int oct_consum(unsigned char * p_timenow, int nsamsck, unsigned char *lplogicnumber, unsigned long value,unsigned char trtype, unsigned char *lpmetrofile, unsigned char *respone_data)
{
	unsigned char sztmp[512];
	unsigned char szmac[8];
	unsigned char cainf[10];
	unsigned char ulen;
	unsigned char respone_len;
	unsigned short sam_sw;
	TPSAMGETMAC PsamGetMac1;
	LPTINITCONSUME lpinitconsume;
	LPTOUTMTRW lpout=(LPTOUTMTRW)respone_data;
	int nresult=-1;


	do
	{
		//######################################################
		// 针对不同批次的SAM卡(包括读写器) 20140527－wxf

		if (sam_rst(nsamsck,sztmp) <= 0)	break;

		ulen = 7;
		memcpy(sztmp, "\x00\xA4\x00\x00\x02\x10\x01", ulen);
		if ( sam_cmd(nsamsck, ulen, sztmp, sztmp, &sam_sw) <= 0)	break;
		if (0x9000 != sam_sw)	break;

		//######################################################

		ulen  = 0;
		memcpy(sztmp, "\x80\xCA\x00\x00\x09", 5);
		ulen = 5;
		respone_len = MifareProCom(ulen, sztmp, &sam_sw);
		if ((respone_len==0) ||(sam_sw!=0x9000))
		{
			nresult = -3;
			break;
		}

		memcpy(cainf, sztmp, 9);

		ulen=0;
		memcpy(sztmp+ulen, "\x80\xCA\x00\x00\x09",5);
		ulen += 5;
		memcpy(sztmp+ulen, cainf,9);
		ulen += 9;

		respone_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &sam_sw);
		if ((respone_len==0) ||(sam_sw!=0x9000))
		{
			nresult = -5;
			break;
		}


		ulen=0;
		if (0x06==trtype)
		{
			memcpy(sztmp, "\x80\x50\x01\x02\x0B", 5);
		}
		else if (0x09==trtype)
		{
			memcpy(sztmp, "\x80\x50\x03\x02\x0B", 5);
		}
		else
		{
			nresult = -9; //参数错误
			break;
		}
		ulen += 5;

		sztmp[5]=g_saminf[nsamsck].keyidx;
		ulen += 1;
		sztmp[ulen] = ((value >> 24) & 0xFF);
		ulen += 1;
		sztmp[ulen] = ((value >> 16) & 0xFF);
		ulen += 1;
		sztmp[ulen] = ((value >> 8) & 0xFF);
		ulen += 1;
		sztmp[ulen] = (value & 0xFF);
		ulen += 1;

		memcpy(sztmp+ulen, g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);
		ulen += g_saminf[nsamsck].len_terminal_id;
		respone_len = MifareProCom(ulen, sztmp, &sam_sw);
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -3;
			break;
		}

		lpinitconsume = (LPTINITCONSUME)sztmp;

		PsamGetMac1.encryver = lpinitconsume->encryver;
		PsamGetMac1.calcmarker = lpinitconsume->calcmarker;
		memcpy(PsamGetMac1.offlinedealserial, lpinitconsume->offlinedealserial, 2);
		memcpy(PsamGetMac1.random, lpinitconsume->random, 4);
		memcpy(PsamGetMac1.tm, p_timenow, 7);


		PsamGetMac1.trflag = trtype;



		memcpy(PsamGetMac1.logicnumber, lplogicnumber, 8);
		PsamGetMac1.money[0] = ((value >> 24) & 0xFF);

		PsamGetMac1.money[1] = ((value >> 16) & 0xFF);

		PsamGetMac1.money[2] = ((value >> 8) & 0xFF);

		PsamGetMac1.money[3] = (value & 0xFF);

		if (0x09==trtype)
		{
			memcpy(sztmp, "\x80\xDC\x03\xB8\x20", 5);//长沙一卡通
			ulen = 5;
			memcpy(sztmp+ulen, "\x03\x1E\x00", 3);
			ulen += 3;
			memcpy(sztmp+ulen, lpmetrofile, 29);
			ulen += 29;

			respone_len = MifareProCom(ulen, sztmp, &sam_sw);
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -4;
				break;
			}
		}

		memcpy(sztmp,"\x80\x70\x00\x00\x24",5);
		ulen = 5;
		memcpy(sztmp+5, (char *)&PsamGetMac1, sizeof(PsamGetMac1));
		ulen += sizeof(PsamGetMac1);
		memcpy(sztmp+ulen, "\x41\x00",2);
		ulen += 2;
		memcpy(sztmp+ulen, "\xFF\x00\x00\x00\x00\x00",6);
		ulen += 6;

		respone_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &sam_sw);
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -5;
			break;
		}

		memcpy(lpout->sam_terminalserial, sztmp, 4);
		memcpy(lpout->sam_terminalnumber, g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);

		memcpy(szmac, sztmp, 8);
		memcpy(sztmp,"\x80\x54\x01\x00\x0F", 5);
		ulen = 5;
		memcpy(sztmp+ulen, szmac, 4);
		ulen += 4;
		memcpy(sztmp+ulen, PsamGetMac1.tm, 7);
		ulen += 7;
		memcpy(sztmp+ulen, szmac+4, 4);
		ulen += 4;

		respone_len = MifareProCom(ulen, sztmp, &sam_sw);
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -6;
			break;
		}
		memcpy(lpout->tac, sztmp, 4);
		memcpy(szmac, sztmp, 8);

		memcpy(sztmp,"\x80\x72\x00\x00\x04",5);
		ulen = 5;
		memcpy(sztmp+5, szmac+4, 4);
		ulen += 4;
		//respone_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &sam_sw);
		Extra::sam_seq_add(nsamsck, sztmp, ulen);

		nresult = 0;
	}
	while (0);

	return (nresult);
}

//========================================================
//功能:对OCT加解锁
//入口参数
//   nsamsck=sam卡对应卡槽
//    lplogicnumber = 票卡应用序列号
//    lpcitycode=城市代码
//    optflag=操作类型(1-加锁,0-解锁)
//出口参数
//       无
//返回值
//       0=成功
//注:黑卡判断
//在3F00应用目录发送00B203D400，前两字节为0x53,0x4B则为已锁定黑卡
//========================================================
int oct_cpu_lock(int nsamsck, unsigned char *lplogicnumber, unsigned char *lpcitycode, unsigned char optflag)
{
	unsigned char ulen, response_len;
	unsigned short response_sw;
	unsigned char sztmp[256];
	unsigned char random[9],mac[9];
	int nresult=-1;


	do
	{
		//取随机数
		ulen = 5;
		memcpy(sztmp, "\x00\x84\x00\x00\x08",5);

		//dbg_dumpmemory("0084_S=",sztmp,ulen);
		response_len = MifareProCom(ulen,sztmp, &response_sw);
		//dbg_dumpmemory("0084_R=",sztmp,response_len);

		if (( response_len==0 ) || (0x9000!=response_sw))
		{
			nresult = -1;
			break;
		}
		memset(random, 0, sizeof(random));
		memcpy(random, sztmp, 8);

		//PSAM选择互联互通应用
		ulen = 11;
		memcpy(sztmp, "\x00\xA4\x04\x00\x06\xBD\xA8\xC9\xE8\xB2\xBF",11);
		response_len = sam_cmd(nsamsck, ulen, sztmp, sztmp, &response_sw);
		//dbg_dumpmemory("psam 00A4_R=",sztmp,response_len);
		if (( response_len==0 ) || (0x9000!=response_sw))
		{
			nresult = -1;
			break;
		}

		//PSAM卡外部认证密钥初始化
		ulen = 0;
		memcpy(sztmp, "\x80\x1A\x45\x01\x10",5);
		ulen = 5;
		memcpy(sztmp+ulen, lplogicnumber, 8);//g_metroinf.logicnumber, 8);
		ulen += 8;
		memcpy(sztmp+ulen, lpcitycode,2);
		ulen += 2;
		memcpy(sztmp+ulen, "\xFF\x00\x00\x00\x00\x00",6);
		ulen += 6;
		//dbg_dumpmemory("801A_S=",sztmp,ulen);
		response_len = sam_cmd(nsamsck, ulen, sztmp, sztmp, &response_sw);
		//dbg_dumpmemory("801A_R=",sztmp,response_len);
		if (( response_len==0 ) || (0x9000!=response_sw))
		{
			nresult = -1;
			break;
		}

		//PSAM卡外部认证CPU用户卡
		ulen = 0;
		memcpy(sztmp, "\x80\xFA\x00\x00\x08",5);
		ulen += 5;
		memcpy(sztmp+ulen, random, 8);
		ulen +=8;
		//dbg_dumpmemory("80FA_S=",sztmp,ulen);
		response_len = sam_cmd(nsamsck, ulen, sztmp, sztmp, &response_sw);
		//dbg_dumpmemory("80FA_R=",sztmp,response_len);
		if (( response_len==0 ) || (0x9000!=response_sw))
		{
			nresult = -1;
			break;
		}

		memcpy(mac, sztmp, 8);

		//CPU用户卡外部认证
		ulen = 0;
		memcpy(sztmp, "\x00\x82\x00\x03\x08", 5);
		ulen += 5;
		memcpy(sztmp+ulen, mac, 8);
		ulen += 8;

		//dbg_dumpmemory("0082_S=",sztmp,ulen);
		response_len = MifareProCom(ulen,sztmp, &response_sw);
		//dbg_dumpmemory("0082=",sztmp,response_len);

		if (( response_len==0 ) || (0x9000!=response_sw))
		{
			nresult = -1;
			break;
		}

		//写标记
		ulen = 0;
		memcpy(sztmp, "\x00\xDC\x03\xD4\x10",5);
		ulen += 5;
		if (optflag)
			memcpy(sztmp+ulen,"\x53\x4B\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
		else
			memcpy(sztmp+ulen,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);

		ulen += 16;
		//dbg_dumpmemory("00DC_S=",sztmp,ulen);
		response_len = MifareProCom(ulen,sztmp, &response_sw);
		//dbg_dumpmemory("00DC=",sztmp,response_len);

		if (( response_len==0 ) || (0x9000!=response_sw))
		{
			nresult = -1;
			break;
		}


		//PSAM恢复到应用目录
		ulen = 7;
		memcpy(sztmp, "\x00\xA4\x00\x00\x02\x10\x01",7);
		response_len = sam_cmd(nsamsck, ulen, sztmp, sztmp, &response_sw);
		//dbg_dumpmemory("psam 00A4_R=",sztmp,response_len);
		if (( response_len==0 ) || (0x9000!=response_sw))
		{
			nresult = -1;
			break;
		}

		nresult = 0;
	}
	while(0);

	return nresult;
}

//=============================================================================================================
//函 数 名: generate_tac
//功    能: 计算单程票TAC码
//入口参数:
//          nsamsck: sam卡对应卡槽
//   *lplogicnumber: 8byte逻辑卡号
//            money: 4byte交易金额,long型
//   trade_datetime: 7byte交易时间
//出口参数:
//        lpresult:计算结果(samid(6B)+dsn(4B)+tac(4B))，当返回值=0时有效
//返回值:
//       =0成功
//=============================================================================================================
int  generate_tac(int nsamsck, unsigned char *lplogicnumber, unsigned long money, unsigned char *trade_datetime, unsigned char *lpresult)
{
	unsigned char sztmp[50];
	unsigned char ulen;
	unsigned char response_len;
	unsigned short response_sw;
	unsigned char tmpbuf[20];//samid(6B)+dsn(4B)+tac(4B)
	int nresult = 0;


	memset(tmpbuf, 0, sizeof(tmpbuf));
	memcpy(tmpbuf,g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);
	tmpbuf[6] = ((g_saminf[nsamsck].dsn >> 24) & 0xff);
	tmpbuf[7] = ((g_saminf[nsamsck].dsn >> 16) & 0xff);
	tmpbuf[8] = ((g_saminf[nsamsck].dsn >> 8) & 0xff);
	tmpbuf[9] = (g_saminf[nsamsck].dsn & 0xff);


	//des初始化
	ulen = 5;
	memcpy(sztmp, "\x80\x1A\x24\x01\x08", 5);
	memcpy(sztmp+ulen, lplogicnumber, 8);
	ulen += 8;
	response_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &response_sw);
	if (0x9000 != response_sw )
	{
		ulen = 5;
		memcpy(sztmp, "\x80\x1A\x24\x01\x08", 5);
		memcpy(sztmp+ulen, lplogicnumber, 8);
		ulen += 8;


		sam_cmd(nsamsck,ulen,sztmp, sztmp, &response_sw);
	}

	//计算TAC
	ulen = 0;
	//    memcpy(sztmp, "\x80\xFA\x05\x00\x18", 5);
	memset(sztmp, 0x00, sizeof(sztmp));
	memcpy(sztmp, "\x80\xFA\x05\x00\x20", 5);
	ulen += 5;
	//vi,8byte全0
	ulen += 8;
	//4byte交易金额
	sztmp[ulen++] = ((money & 0xFF000000) >> 24);
	sztmp[ulen++] = ((money & 0x00FF0000) >> 16);
	sztmp[ulen++] = ((money & 0x0000FF00) >> 8);
	sztmp[ulen++] = (money & 0xFF);
	//1byte交易类型
	sztmp[ulen++] = 0x05;
	//6byte sam终端号
	memcpy(sztmp+ulen, g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);
	ulen += 6;
	//4byte sam流水
	sztmp[ulen++] = ((g_saminf[nsamsck].dsn & 0xFF000000) >> 24);
	sztmp[ulen++] = ((g_saminf[nsamsck].dsn & 0x00FF0000) >> 16);
	sztmp[ulen++] = ((g_saminf[nsamsck].dsn & 0x0000FF00) >> 8);
	sztmp[ulen++] = (g_saminf[nsamsck].dsn & 0xFF);
	//7byte交易时间
	memcpy(sztmp+ulen, trade_datetime, 7);
	ulen += 7;
	//2byte 结束标志
	memcpy(sztmp+ulen, "\x80\x00", 2);
	ulen += 2;

	response_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &response_sw);

	if (0x9000 == response_sw )
	{
		memcpy(tmpbuf+10,sztmp, 4);
	}

	//回写sam  流水
	g_saminf[nsamsck].dsn++;
	memcpy(sztmp, "\x00\xD6\x98\x00\x04\x00\x00\x00\x00", 9);
	ulen = 9;
	sztmp[5] = ((g_saminf[nsamsck].dsn >> 24) & 0xFF);
	sztmp[6] = ((g_saminf[nsamsck].dsn >> 16) & 0xFF);
	sztmp[7] = ((g_saminf[nsamsck].dsn >> 8) & 0xFF);
	sztmp[8] = (g_saminf[nsamsck].dsn  & 0xFF);

	memcpy(lpresult+6, sztmp+5, 4);//sam流水

	//response_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &response_sw);
	Extra::sam_seq_add(nsamsck, sztmp, ulen);

	memcpy(lpresult, tmpbuf, 14);

	return nresult;
}

//========================================================
//功能:通过aid 选择应用目录
//入口参数
//	 aid=应用目录标识,
//出口参数
//       无
//返回值
//       >=0成功
//========================================================
int svt_selecfileaid(unsigned char ulen, unsigned char *lpaid)
{
	unsigned char ucmdlen;
	unsigned char sztmp[256];
	unsigned char response_len;
	unsigned short response_sw;
	int nresult =  -1;
	ucmdlen = 0;
	memcpy(sztmp, "\x00\xA4\x04\x00",4);
	ucmdlen += 4;
	sztmp[ucmdlen] = ulen;
	ucmdlen++;
	memcpy(sztmp + ucmdlen, lpaid, ulen);
	ucmdlen += ulen;
	dbg_dumpmemory("svt_selecfileaid=",sztmp,ucmdlen);
	response_len = MifareProCom(ucmdlen, sztmp, &response_sw);
	dbg_dumpmemory("svt_selecfileaid |=",sztmp,response_len);
	dbg_formatvar("svt_selecfileaid |= %04X",response_sw);

	if (response_len==0)
	{
		//无回应
		nresult = -1;
	}
	else if (0x9000!=response_sw)
	{
		//响应错误
		nresult = -2;
	}
	else
	{
		nresult = 0;
	}

	return nresult;

}

//========================================================
//功能:获取CPU卡上次交易的TAC码
//入口参数
//	 aid=应用目录标识,
//出口参数
//       无
//返回值
//       >=0成功
//========================================================
int get_cpu_last_tac(uint16_t off_line_counter, uint8_t tac_type, unsigned char * p_recv)
{
	int ret						= 0;
	unsigned char data_cmd[256]	= {0};
	unsigned char len_cmd		= 0;
	unsigned char len_recv		= 0;
	unsigned short sw_result	= 0;

	do
	{
		if (svt_selectfile(0x3F01) < 0)
		{
			ret = -1;
			g_Record.log_out(0, level_disaster, "svt_selectfile(3F01) failed in function get_cpu_last_tac");
			break;
		}

		memcpy(data_cmd + len_cmd, "\x80\x5A\x00\x02\x02", 5);
		data_cmd[3] = tac_type;
		len_cmd += 5;
		data_cmd[len_cmd] = (unsigned char)((off_line_counter >> 8) & 0xFF);
		data_cmd[len_cmd + 1] = (unsigned char)(off_line_counter & 0xFF);
		len_cmd += 2;

		//g_Record.log_buffer((char *)("get tac send:"), data_cmd, 7);
		len_recv = MifareProCom(len_cmd, data_cmd, &sw_result);
		if (len_recv == 0 || sw_result != 0x9000)
		{
			g_Record.log_out(0, level_disaster, "MifareProCom failed in function get_cpu_last_tac, len_recv=%d,sw_result=%04X", len_recv, sw_result);
			ret = -2;
			break;
		}

		memcpy(p_recv, data_cmd + 4, 4);

	} while (0);

	return ret;
}

//========================================================
//功能:CPU卡pin校验
//出口参数
//       无
//返回值
//       =0成功
//========================================================
int mtr_cpu_pin_check()
{
	unsigned char sam_len		= 8;
	unsigned char sz_data[256]	= {0};
	unsigned short sam_sw		= 0;

	memcpy(sz_data, "\x00\x20\x00\x00\x03\x12\x34\x56",8);
	unsigned char respone_len = MifareProCom(sam_len, sz_data, &sam_sw);
	if ((respone_len==0 ) || (0x9000!=sam_sw))
	{
		return -1;
	}

	return 0;
}

//========================================================
//功能:初始化SAM卡全局相关信息
//出口参数
//       无
//返回值
//========================================================
void init_sam_inf()
{
	memset(g_saminf, 0, sizeof(g_saminf));

	g_saminf[0].len_logical_id = 8;
	g_saminf[0].len_terminal_id = 6;
	g_saminf[0].offset_logical = 0;

	g_saminf[1].len_logical_id = 6;
	g_saminf[1].len_terminal_id = 6;
	g_saminf[1].offset_logical = 2;
}
