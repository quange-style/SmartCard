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
//CPU����
typedef struct
{
    unsigned char       balance[4];                 // ���
    unsigned char       offlinedealserial[2];       // Ǯ���ѻ��������
    unsigned char       overdraft[3];               // ͸֧�޶�
    unsigned char       encryver;                   // ��Կ�汾��
    unsigned char       calcmarker;                 // �㷨��ʶ
    unsigned char       random[4];                  // α�����
} TINITCONSUME,*LPTINITCONSUME;

typedef struct
{
    unsigned char        balance[4];                 // ���
    unsigned char        onlinedealSerial[2]; 				/// �����������
    unsigned char        encryver;                   // ��Կ�汾��
    unsigned char        calcmarker;                 // �㷨��ʶ
    unsigned char        random[4];                  // α�����
    unsigned char        mac1[4];                      // mac1
} TINITRECHARGE,*LPTINITRECHARGE;

typedef struct
{
    unsigned char      random[4];                  // α�����
    unsigned char      offlinedealserial[2];       // Ǯ���ѻ��������
    unsigned char      money[4];                   // ���׽��
    unsigned char      trflag;                 // �������ͱ�ʶ
    unsigned char      tm[7];                 // ����ʱ��
    unsigned char      encryver;                   // ��Կ�汾��
    unsigned char      calcmarker;                 // �㷨��ʶ
    unsigned char      logicnumber[8];
//	    unsigned char      EncryptSeeds2[8];           //������ɢ����
//	    unsigned char      EncryptSeeds1[8];
} TPSAMGETMAC,*LPTPSAMGETMAC;

typedef struct
{
    char 	cMsgType[2];			// ��Ϣ����"51"
    unsigned char	dtTime[7];				// ����ʱ��
    char	cStation [4];			// վ��
    char	cDevType[2];			// �豸����
    char	cDevId[3];				// �豸����
    char	cSamId[16];				// SAM����
    long	lPosSeq;				// �ն˽������к�
    char	cNetPoint[16];			// �������
    char	cIssueCodeMain[4];		// ������������
    char 	cIssueCodeSub[4];		// �������ӱ���
    unsigned char	bTicketType[2];			// Ʊ������
    char	cLogicalId[20];			// Ʊ���߼�����
    char 	cPhysicalId[20];		// Ʊ��������
    unsigned char	bTestFlag;				// Ʊ�����ñ�ʶ
    long	lTradeSeqOnLine;		// ����������ˮ
    long	lTradeSeqOffLine;		// �ѻ�������ˮ
    char	cBusinessType[2];		// ҵ������"14"
    unsigned char	bWalletType;			// ֵ����
    long	lAmount;				// ��ֵ���
    long	lBalcance;				// ���
    char	cMac[8];				// ��ֵ��֤��
    char	cFreedom[8];			// �����
    char	cStationLast [4];		// �ϴ�վ��
    char	cDevTypeLast[2];		// �ϴ��豸����
    char	cDevIdLast[3];			// �ϴ��豸����
    unsigned char	dtTimeLast[7];			// �ϴ�ʱ��
    char	cOperatorId[6];			// ����Ա����
}TRECHARGE_PKG_INIT, * LPTRECHARGE_PKG_INIT;


//д���������
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
    unsigned char keyidx;				//��Կ����
	unsigned char len_terminal_id;		//�ն˺ų���
    unsigned char sam_terminal_id[8];	//�ն˺�

	unsigned char len_logical_id;		//�߼����ų���
	unsigned char offset_logical;		//ȡ�߼����ŵ�ƫ��
    unsigned char sam_logical_id[8];	//�߼�����
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

/* ������Ϣ */
struct ASSIST_DATA
{
    char sjt_key[6];															 /* ����Կ */
} ass;

/* ָ����Ϣ�ж� */
struct ST_CMD_IN_DATA														  /* �������ݽṹ */
{
    unsigned char ln; 														  /* �������ݳ��� */
} si;

struct ST_CMD_OT_DATA														  /* ������ݽṹ */
{
    char data[64];
    unsigned char ln; 														  /* �������ݳ��� */
} so;

//=================================================


//========================================================
//����:�Ե���SAM  ���г�ʼ��
//��ڲ���
//	 nsamsck=sam����Ӧ����
//���ڲ���
//       saminf=6�ֽڵ�sam  ����
//����ֵ
//       0=�ɹ�
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

		//modify by shiyulong 2013-11-05, ��ȡ��ˮ��
		if (nsamsck == 0)
		{
			for(int i=0; i<2; i++)
			{
				g_saminf[nsamsck].dsn = 0;//��ʼֵ
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
					//û���ļ�ֱ���˳�
					nresult = -1;
					break;
				}
			}
		}

	} while (0);

	return nresult;
}

//========================================================
//����:��Ѱ���߸�Ӧ��Ʊ��
//��ڲ���
//	 ��
//���ڲ���
//       lpcardno=����
//����ֵ
//       0=�޿�
//       1=UL
//       2=M1��
//       3=CPU��
//      99=�࿨
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
//					nresult = 3; //cpu��
//				}
//				else
//				{
//					nresult = 2; //m1��
//				}
//				CardNum++;
//				memcpy(firstsnr, snr, 4);
//				memcpy(lpcardno,firstsnr, 4);
//			};
//		}
//		else
//		{
//			//UL����Ʊ
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
//						nresult = 3; //cpu��
//					}
//					else
//					{
//						nresult = 2; //m1��
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
				//UL����Ʊ
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
				if (ISO14443A_Anticoll(snr) == 0)//����ͻ
				{
					if (ISO14443A_Select(snr, tmp) == 0)//ѡ��
					{
						card_type[i] = (tmp[0] & 0x24) == 0x20 ? 0x03 : 0x02;
						memcpy(ack[i], snr, 4);
					}
				}
			}
			else
			{
				//UL����Ʊ
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

	if (card_type[0] == 0)//����Ʊ
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
				//UL����Ʊ
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
				nresult = 3;				//����
			}
			else if (memcmp(sztmp, "\x59\x44",2)==0)
			{
				nresult = 4;				//����
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
				type = 3;				//����
			}
			else if (memcmp(sztmp, "\x59\x44",2)==0)
			{
				type = 4;				//����
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
//����: CPU ����λ,  ��ǰ��������searchcard����
//��ڲ���:  ��
//���ڲ���:
//                         lpinf: ��λ��Ϣ
//����ֵ:
//                =0�ɹ�
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
//����:������Ʊ����
//��ڲ���
//	 nsamsck=sam����Ӧ����
//���ڲ���
//       lpbuffer=������Ʊ�����ݣ��ӵ�4ҳ����15ҳ(48BYTE)
//����ֵ
//       0=�����ɹ�, ������������crc  ����ȷ
//       1=�����ɹ�, ��һ��������crc  ����
//       2=�����ɹ�, �ڶ���������crc  ����
//       3=�����ɹ�, ����������crc  ������
//      -1=�޿�
//      -2=������ʧ��
//      -5=����sam  ��ʧ��
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


	/* ���������� */
	// Read all data
	for (i=0; i<=15; i+=4)
	{
		status = ISO14443A_Read_UL(i, (uint8_t *)(sjt_card.data+(i<<2)));
		if (status != MI_OK)
		{
			//���Զ��һ��
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

	/* ������Կ */
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
		//����0  ��CRC  ��
		errcnt += 1;
	};

	if (generate_crc8((uint8_t *)(sjt_card.data+48),16,key_crypt[1])!=0)
	{
		//����1  ��CRC  ��
		errcnt += 2;
	};

	memcpy(lpbuffer, sjt_card.data,64);
	memcpy(lpbuffer+64,szphysical,7);

	return errcnt;
}

#define USERZONE_PAGER1 8
#define USERZONE_PAGER2 12
//========================================================
//����:д����Ʊ����
//��ڲ���
//        nsamsck=sam����Ӧ����
//        nzone=��������0-����������0��1-����������0
//        lpbuffer=16�ֽڴ��������ݣ�
//���ڲ���
//       ��
//����ֵ
//       0=�ɹ�
//      -3=д��ʧ��
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


	//ԭʼ�����ȼ��㲢��� crc8
	if ( idx>1 ) idx=1;
	crc8 = generate_crc8(lpbuffer, 15, key_crypt[idx]);
	lpbuffer[15] = crc8;

	memcpy(write_buf, lpbuffer, 16);

	//д����ǰ���м���

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

	//д����
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

	//д����ǰ���м���
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

	//д����
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
//����:�Ե���SAM  ���й��ܼ��ֻ�м������Ĳ��ܽ��з������ڽ���
//��ڲ���
//	 nsamsck=sam����Ӧ����
//    direct=���ݷ���, 0=��ʾȡ������Ϣ������Ϣ�跢�͸�������; 1=��ʾ�����������������������Ϣ���͵�SAM  ��
//    lpinf=��direct=1ʱΪ���� ����
//���ڲ���
//       ��
//����ֵ
//       0=�ɹ�
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
		//��ȡ�����
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

		//ִ���ⲿ��֤
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
//����:ѡ��Ӧ��Ŀ¼
//��ڲ���
//	 aid=Ӧ��Ŀ¼��ʶ, 0x3F00=��Ӧ��Ŀ¼,  0x1001=ADF1Ŀ¼, 0x1002=ADF2Ŀ¼
//���ڲ���
//       ��
//����ֵ
//       >=0�ɹ�
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
		//�޻�Ӧ
		nresult = -1;
	}
	else if (0x9000!=response_sw)
	{
		g_Record.log_out(0, level_error, "svt_selectfile[%d] = %04X",aid,response_sw);
		//��Ӧ����
		nresult = -2;
	}
	else
	{
		nresult = 0;
	}

	return nresult;

}

//========================================================
//����: ���������ļ�,  Ҫ�Ƚ�����Ӧ��Ӧ��Ŀ¼������ȷ��ȡ
//��ڲ���
//	 aid=���ļ���ʶ, 0x05=�����ļ�,  0x11=Ӧ�ÿ����ļ���0x15=������Ϣ�ļ�,  0x16=�ֿ��˸�����Ϣ�ļ�
//    start_offset:��ȡ�ļ�����ʼƫ�Ƶ�ַ
//    len:Ҫ��ȡ�ĳ���
//���ڲ���
//       lpdata: ��ȡ�����ݻ���
//����ֵ
//       >=0�ɹ�
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
		//�޻�Ӧ
		g_Record.log_out(0, level_error, "svt_readbinary[%d]= [%04X]",response_sw);
		nresult = -1;
	}
	else if (0x9000!=response_sw)
	{
		g_Record.log_out(0, level_error, "svt_readbinary[%d] = %04X",aid,response_sw);
		//��Ӧ����
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
//����: ����¼�ļ�,  Ҫ�Ƚ�����Ӧ��Ӧ��Ŀ¼������ȷ��ȡ
//��ڲ���
//	 aid=���ļ���ʶ, 0x17=�����ļ���0x18=��ʷ��ϸ
//    record_no:Ҫ��ȡ�ļ���¼��
//    len:Ҫ��ȡ�ĳ���
//���ڲ���
//       lpdata: ��ȡ�����ݻ���
//����ֵ
//       >=0�ɹ�
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
		//�޻�Ӧ
		//g_Record.log_out(response_len, level_error, "MifareProCom1(response_sw=%d)", response_sw);
		nresult = -1;
	}
	else
		if (0x9000!=response_sw)
		{
			//��Ӧ����
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
//����: ����ʷ��ϸ,  Ҫ�Ƚ�����Ӧ��Ӧ��Ŀ¼������ȷ��ȡ
//��ڲ���
//	 aid=���ļ���ʶ, 0x17=�����ļ���0x18=��ʷ��ϸ
//    record_start: Ҫ��ȡ��ʼ��¼��
//      record_cnt:Ҫ��ȡ�ļ�¼����
//���ڲ���
//       lpdata: ��ȡ�����ݻ���
//����ֵ
//       >=0�ɹ�
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
			//�޻�Ӧ
			nresult = -1;
			break;
		}
		else if (0x6a82 == response_sw || 0x6a83 == response_sw)
		{
			// ���ļ������޼�¼
			break;
		}
		else if (0x9000!=response_sw)
		{
			//��Ӧ����
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
//����: ��Ǯ��ֵ,  Ҫ�Ƚ�����Ӧ��Ӧ��Ŀ¼������ȷ��ȡ
//��ڲ���
//	 ��
//���ڲ���
//       lpdata: 4�ֽڵ�Ǯ��ֵ,  ���ֽ���ǰ
//����ֵ
//       >=0�ɹ�
//========================================================
int svt_getbalance(unsigned char *lpdata)
{
	unsigned char ulen;
	unsigned char sztmp[256];
	unsigned char response_len;
	unsigned short response_sw;

	//int i;
	int nresult =  -1;

	//Ǯ��
	ulen = 5;
	memcpy(sztmp, "\x80\x5C\x00\x02\x04", ulen);
	response_len = MifareProCom(ulen, sztmp, &response_sw);
	dbg_formatvar("805C |= %04X",response_sw);

	if (response_len==0)
	{
		//�޻�Ӧ
		nresult = -1;
	}
	else if (0x9000!=response_sw)
	{
		//��Ӧ����
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
//����: ȡ�������׼���
//��ڲ���
//	 nsamsck: ����SAM  ����Ӧ�Ŀ���,  ��0  ��ʼ
//���ڲ���
//       lpdata: 2�ֽڵ��������к�,  ���ֽ���ǰ
//����ֵ
//       >=0�ɹ�
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
//����: ȡ�ѻ����׼���
//��ڲ���
//	 nsamsck: ����SAM  ����Ӧ�Ŀ���,  ��0  ��ʼ
//���ڲ���
//       lpdata: 2�ֽڵĽ������к�,  ���ֽ���ǰ
//����ֵ
//       >=0�ɹ�
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
//�� �� ��: svt_read
//��    ��: ��ȡ����Ʊ����������
//��ڲ���:
//          nmode:������ʽ,=0��ͨ��ʽ, =1ȫ����ʽ����Ҫ��ȡ�ֿ�����Ϣ��������ʷ
//���ڲ���:
//          lpcardinf:���ص����ݻ�����
//          ��nmode=0ʱ
//            ���л�����Ϣ:40Byte
//            ������Ϣ:30Byte
//            ���׸�����Ϣ:48Byte
//            ������Ϣ:48Byte
//            Ӧ�ÿ�����Ϣ:32Byte
//            Ǯ��ֵ:4Byte
//            ���һ����ʷ��¼:23Byte
//          ��nmode=1ʱ
//            ���л�����Ϣ:40Byte
//            ������Ϣ:30Byte
//            ���׸�����Ϣ:48Byte
//            ������Ϣ:48Byte
//            Ӧ�ÿ�����Ϣ:32Byte
//            Ǯ��ֵ:4Byte
//            �ֿ��˸�����Ϣ:23Byte
//            10����ʷ��¼:230Byte
//�� �� ֵ:
//        >0: ���ص����ݳ���
//        <0: ��������
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


			//��������Ϣ
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


			//������Ϣ��
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

			//���׸�����Ϣ
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

			//������Ϣ��
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


			//Ӧ�ÿ�����Ϣ��
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
			//Ǯ��
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
				//���һ����ʷ
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

				//�ֿ�����Ϣ
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

				//����ʷ10��
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
//����:�Ե���svt �����г�ֵ
//��ڲ���
//	 nsamsck=sam����Ӧ����
//    nstep=��=0ʱ��ʾ��ȡ��ֵ��Ϣ��=1ʱ��ʾ��Ʊ�����г�ֵ����
//    value=Ҫ�䵽Ʊ���ڵĽ��(  ��)
//    lpinf =��nstep=1ʱΪ��������
//            �������ݽṹ:
//                       ����ʱ��7Byte
//                        MAC2   4Byte
//
//���ڲ���
//       pinf = �������
//          nstep=0   ������ݽṹ:
//                               ���4Byte,
//                       �����������2Byte,
//                           ��Կ�汾1Byte,
//                           �㷨��ʶ1Byte,
//                           α�����4Byte,
//                                       MAC1  4Byte
//                �ѻ��������2Byte,
//                      �ն˻����4Byte
//          nstep=1   ������ݽṹ:
//                                        TAC   4Byte
//����ֵ
//       0=�ɹ�
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


			//��ȡ��ֵ��Ϣ
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
			//ִ�г�ֵ, �����ڻ�ȡ��ֵ��Ϣ���м䲻��ִ����������
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
				//8052ʧ������һ�Σ���8050��ʼ
				//��ȡ��ֵ��Ϣ
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

				//ִ�г�ֵ, �����ڻ�ȡ��ֵ��Ϣ���м䲻��ִ����������
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
//����:�Ե���Ʊ�����ѻ�����
//��ڲ���
//	 nsamsck=sam����Ӧ����
//    value = ���ѽ��
//    trtype=��������, 06=��ͨ����,09=��������
//    lpmetrofile=��trtype=09  ʱ����Ϊ�����ļ����µ��������ݣ���ͨ����ʱ��ʹ��
//���ڲ���
//       respone_data=SAM�ն˺�(6BYTE) +  SAM�ն���ˮ( 4Byte) + TAC (4Byte)
//����ֵ
//       0=�ɹ�
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
			nresult = -9; //��������
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
//����:��SVTƱ��Ӧ�ÿ����ļ����и���
//��ڲ���
//	 nsamsck=sam����Ӧ����
//       file_id=���ļ���ʶ��Ŀǰֻ֧��0x11(Ӧ�ÿ����ļ�)��0x16(������Ϣ�ļ�)
//        offset=Ҫ�����ļ�����ʼ��ַ
//           nle=Ҫ���µ����ݳ���
//         lpinf=Ҫд�������

//
//���ڲ���
//	��
//����ֵ
//       0=�ɹ�
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
			paramter_p1=  (0x11 & 0x1F) | 0x80; //���ļ�0x11  ,  Ӧ�ÿ�����

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
//����:�Ե���Ʊָ���ļ�¼���и���
//��ڲ���
//    nsamsck=sam����Ӧ����
//    lplogicnumber=Ҫ����Ʊ����8�ֽ��߼�����(Ӧ�����к�)
//    record_id=�ĵ��ж�Ӧ�ļ�¼��
//    lock=������ʶ,=0������
//    lpfile=�����ļ����µ���������
//���ڲ���
//       respone_data=SAM�ն˺�(6BYTE) +  SAM�ն���ˮ( 4Byte) + TAC (4Byte)
//����ֵ
//       0=�ɹ�
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
//����:��SVTƱ�ĸ����ļ�ָ����¼���и���
//��ڲ���
//	 nsamsck=sam����Ӧ����
//  lplogicnumber=Ʊ���߼�����
//        record_id=��¼ID
//           lock=�������
//         lpinf=�����ļ���ӦҪд�������(Ҫ���ȡʱ��õ���ͬ)

//
//���ڲ���
//	��
//����ֵ
//       0=�ɹ�
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
//����:�Ե���Ʊָ���ļ�¼���и���
//��ڲ���
//    nsamsck=sam����Ӧ����
//    lplogicnumber=Ҫ����Ʊ����8�ֽ��߼�����(Ӧ�����к�)
//    recordcnt=Ҫ���µļ�¼����
//    dim_record_id=Ҫ���µĶ�����¼������
//    dim_lock=��Ӧ������¼��������ʶ����,=0������
//    dim_lpfile=�����ļ����µĶ�����¼���������ݣ���ֵʱ��Ҫע�ⲻͬ�ļ�¼�Ŷ�Ӧ�����ݳ���( ��������Ϊ������¼ǰ�����ض����ݵ�����)
//���ڲ���
//       respone_data=SAM�ն˺�(6BYTE) +  SAM�ն���ˮ( 4Byte) + TAC (4Byte)
//����ֵ
//       0=�ɹ�
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
		// ���ѳ�ʼ��
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

		// ���¸����ļ�
		offset=0;
		for (i=0;i<recordcnt;i++)
		{
			//Ŀǰֻ��0x11�ļ�������24, δ���п�����Ҫ�����¼�¼��ʶ
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

		// SAM�����ѳ�ʼ��������ȡSAM��ˮ
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

		// ���Ѳ�����TAC�루ǰ�����е���һ�����Ǹ����壬�����������ԣ�
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
		// ���ѳ�ʼ��
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

		// ���¸����ļ�
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

		// SAM�����ѳ�ʼ��������ȡSAM��ˮ
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

		// ���Ѳ�����TAC�루ǰ�����е���һ�����Ǹ����壩
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
//����:��һ��ͨƱ�����ѻ�����
//��ڲ���
//	 nsamsck=sam����Ӧ����
//    value = ���ѽ��
//    trtype=��������, 06=��ͨ����,09=��������
//    lpmetrofile=��trtype=09  ʱ����Ϊ�����ļ����µ��������ݣ���ͨ����ʱ��ʹ��
//���ڲ���
//       respone_data=SAM�ն˺�(6BYTE) +  SAM�ն���ˮ( 4Byte) + TAC (4Byte)
//����ֵ
//       0=�ɹ�
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
		// ��Բ�ͬ���ε�SAM��(������д��) 20140527��wxf

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
			nresult = -9; //��������
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
			memcpy(sztmp, "\x80\xDC\x03\xB8\x20", 5);//��ɳһ��ͨ
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
//����:��OCT�ӽ���
//��ڲ���
//   nsamsck=sam����Ӧ����
//    lplogicnumber = Ʊ��Ӧ�����к�
//    lpcitycode=���д���
//    optflag=��������(1-����,0-����)
//���ڲ���
//       ��
//����ֵ
//       0=�ɹ�
//ע:�ڿ��ж�
//��3F00Ӧ��Ŀ¼����00B203D400��ǰ���ֽ�Ϊ0x53,0x4B��Ϊ�������ڿ�
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
		//ȡ�����
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

		//PSAMѡ������ͨӦ��
		ulen = 11;
		memcpy(sztmp, "\x00\xA4\x04\x00\x06\xBD\xA8\xC9\xE8\xB2\xBF",11);
		response_len = sam_cmd(nsamsck, ulen, sztmp, sztmp, &response_sw);
		//dbg_dumpmemory("psam 00A4_R=",sztmp,response_len);
		if (( response_len==0 ) || (0x9000!=response_sw))
		{
			nresult = -1;
			break;
		}

		//PSAM���ⲿ��֤��Կ��ʼ��
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

		//PSAM���ⲿ��֤CPU�û���
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

		//CPU�û����ⲿ��֤
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

		//д���
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


		//PSAM�ָ���Ӧ��Ŀ¼
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
//�� �� ��: generate_tac
//��    ��: ���㵥��ƱTAC��
//��ڲ���:
//          nsamsck: sam����Ӧ����
//   *lplogicnumber: 8byte�߼�����
//            money: 4byte���׽��,long��
//   trade_datetime: 7byte����ʱ��
//���ڲ���:
//        lpresult:������(samid(6B)+dsn(4B)+tac(4B))��������ֵ=0ʱ��Ч
//����ֵ:
//       =0�ɹ�
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


	//des��ʼ��
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

	//����TAC
	ulen = 0;
	//    memcpy(sztmp, "\x80\xFA\x05\x00\x18", 5);
	memset(sztmp, 0x00, sizeof(sztmp));
	memcpy(sztmp, "\x80\xFA\x05\x00\x20", 5);
	ulen += 5;
	//vi,8byteȫ0
	ulen += 8;
	//4byte���׽��
	sztmp[ulen++] = ((money & 0xFF000000) >> 24);
	sztmp[ulen++] = ((money & 0x00FF0000) >> 16);
	sztmp[ulen++] = ((money & 0x0000FF00) >> 8);
	sztmp[ulen++] = (money & 0xFF);
	//1byte��������
	sztmp[ulen++] = 0x05;
	//6byte sam�ն˺�
	memcpy(sztmp+ulen, g_saminf[nsamsck].sam_terminal_id, g_saminf[nsamsck].len_terminal_id);
	ulen += 6;
	//4byte sam��ˮ
	sztmp[ulen++] = ((g_saminf[nsamsck].dsn & 0xFF000000) >> 24);
	sztmp[ulen++] = ((g_saminf[nsamsck].dsn & 0x00FF0000) >> 16);
	sztmp[ulen++] = ((g_saminf[nsamsck].dsn & 0x0000FF00) >> 8);
	sztmp[ulen++] = (g_saminf[nsamsck].dsn & 0xFF);
	//7byte����ʱ��
	memcpy(sztmp+ulen, trade_datetime, 7);
	ulen += 7;
	//2byte ������־
	memcpy(sztmp+ulen, "\x80\x00", 2);
	ulen += 2;

	response_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &response_sw);

	if (0x9000 == response_sw )
	{
		memcpy(tmpbuf+10,sztmp, 4);
	}

	//��дsam  ��ˮ
	g_saminf[nsamsck].dsn++;
	memcpy(sztmp, "\x00\xD6\x98\x00\x04\x00\x00\x00\x00", 9);
	ulen = 9;
	sztmp[5] = ((g_saminf[nsamsck].dsn >> 24) & 0xFF);
	sztmp[6] = ((g_saminf[nsamsck].dsn >> 16) & 0xFF);
	sztmp[7] = ((g_saminf[nsamsck].dsn >> 8) & 0xFF);
	sztmp[8] = (g_saminf[nsamsck].dsn  & 0xFF);

	memcpy(lpresult+6, sztmp+5, 4);//sam��ˮ

	//response_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &response_sw);
	Extra::sam_seq_add(nsamsck, sztmp, ulen);

	memcpy(lpresult, tmpbuf, 14);

	return nresult;
}

//========================================================
//����:ͨ��aid ѡ��Ӧ��Ŀ¼
//��ڲ���
//	 aid=Ӧ��Ŀ¼��ʶ,
//���ڲ���
//       ��
//����ֵ
//       >=0�ɹ�
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
		//�޻�Ӧ
		nresult = -1;
	}
	else if (0x9000!=response_sw)
	{
		//��Ӧ����
		nresult = -2;
	}
	else
	{
		nresult = 0;
	}

	return nresult;

}

//========================================================
//����:��ȡCPU���ϴν��׵�TAC��
//��ڲ���
//	 aid=Ӧ��Ŀ¼��ʶ,
//���ڲ���
//       ��
//����ֵ
//       >=0�ɹ�
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
//����:CPU��pinУ��
//���ڲ���
//       ��
//����ֵ
//       =0�ɹ�
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
//����:��ʼ��SAM��ȫ�������Ϣ
//���ڲ���
//       ��
//����ֵ
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
