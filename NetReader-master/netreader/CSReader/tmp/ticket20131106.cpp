#include <string.h>
#include <stdint.h>
#include "sam.h"
#include "algorithm.h"
//#include "public.h"
#include "iso14443.h"
#include "ticket.h"


#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

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
    unsigned char        onlinedealSerial[2];               /// �����������
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
//      unsigned char      EncryptSeeds2[8];           //������ɢ����
//      unsigned char      EncryptSeeds1[8];
} TPSAMGETMAC,*LPTPSAMGETMAC;

typedef struct
{
    char    cMsgType[2];            // ��Ϣ����"51"
    unsigned char   dtTime[7];              // ����ʱ��
    char    cStation [4];           // վ��
    char    cDevType[2];            // �豸����
    char    cDevId[3];              // �豸����
    char    cSamId[16];             // SAM����
    long    lPosSeq;                // �ն˽������к�
    char    cNetPoint[16];          // �������
    char    cIssueCodeMain[4];      // ������������
    char    cIssueCodeSub[4];       // �������ӱ���
    unsigned char   bTicketType[2];         // Ʊ������
    char    cLogicalId[20];         // Ʊ���߼�����
    char    cPhysicalId[20];        // Ʊ��������
    unsigned char   bTestFlag;              // Ʊ�����ñ�ʶ
    long    lTradeSeqOnLine;        // ����������ˮ
    long    lTradeSeqOffLine;       // �ѻ�������ˮ
    char    cBusinessType[2];       // ҵ������"14"
    unsigned char   bWalletType;            // ֵ����
    long    lAmount;                // ��ֵ���
    long    lBalcance;              // ���
    char    cMac[8];                // ��ֵ��֤��
    char    cFreedom[8];            // �����
    char    cStationLast [4];       // �ϴ�վ��
    char    cDevTypeLast[2];        // �ϴ��豸����
    char    cDevIdLast[3];          // �ϴ��豸����
    unsigned char   dtTimeLast[7];          // �ϴ�ʱ��
    char    cOperatorId[6];         // ����Ա����
} TRECHARGE_PKG_INIT, * LPTRECHARGE_PKG_INIT;


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

TSJT_CARD   sjt_card;

typedef struct
{
    unsigned char keyidx; //��Կ����
    unsigned char terminalnumber[6]; //�ն˺�
    unsigned char logicnumber[6];    //�߼�����
    uint32_t dsn;
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
    char sjt_key[6];                                                             /* ����Կ */
} ass;

/* ָ����Ϣ�ж� */
struct ST_CMD_IN_DATA                                                         /* �������ݽṹ */
{
    unsigned char ln;                                                         /* �������ݳ��� */
} si;

struct ST_CMD_OT_DATA                                                         /* ������ݽṹ */
{
    char data[64];
    unsigned char ln;                                                         /* �������ݳ��� */
} so;

//=================================================


//========================================================
//����:�Ե���SAM  ���г�ʼ��
//��ڲ���
//   nsamsck=sam����Ӧ����
//���ڲ���
//       saminf=6�ֽڵ�sam  ����
//����ֵ
//       0=�ɹ�
//========================================================
int sam_metro_active(int nsamsck, unsigned char *saminf)
{
    unsigned char status;
    unsigned char sztmp[256];
    unsigned short int sam_sw;
    unsigned char sam_len;
    int i;
    int nresult = -1;

    do
    {
        sam_setbaud(nsamsck, SAM_BAUDRATE_38400);

        status = sam_rst(nsamsck,sztmp);
        if ( status <= 0 )  break;

        sam_len = 7;
        memcpy(sztmp, "\x00\xA4\x00\x00\x02\x3f\x00",sam_len);
        status = sam_cmd(nsamsck,sam_len,sztmp, sztmp, &sam_sw);
        if ( status <= 0 )  break;

        sam_len = 5;
        memcpy(sztmp, "\x00\xB0\x95\x00\x0A",sam_len);
        status = sam_cmd(nsamsck,sam_len,sztmp, sztmp, &sam_sw);
        if (0x9000 != sam_sw || status <= 0)    break;

        memcpy(g_saminf[nsamsck].logicnumber, sztmp+4, 6);
        memcpy(saminf, sztmp+4, 6);

        sam_len = 5;
        memcpy(sztmp, "\x00\xB0\x96\x00\x06",sam_len);
        status = sam_cmd(nsamsck,sam_len,sztmp, sztmp, &sam_sw);
        if (0x9000 != sam_sw || status <= 0)    break;

        memcpy(g_saminf[nsamsck].terminalnumber, sztmp, 6);

        sam_len = 7;
        memcpy(sztmp, "\x00\xA4\x00\x00\x02\x10\x01",sam_len);
        status = sam_cmd(nsamsck,sam_len,sztmp, sztmp, &sam_sw);
        if ( status <= 0)   break;

        sam_len = 5;
        memcpy(sztmp, "\x00\xB0\x97\x00\x01",sam_len);
        status = sam_cmd(nsamsck,sam_len,sztmp, sztmp, &sam_sw);
        if (0x9000 != sam_sw || status <= 0)    break;

        //modify by shiyulong 2013-11-05, ��ȡ��ˮ��
        for(int i=0; i<2; i++)
        {
            g_saminf[nsamsck].dsn = 0;//��ʼֵ
            sam_len = 5;
            memcpy(sztmp, "\x00\xB0\x98\x00\x04",sam_len);
            status = sam_cmd(nsamsck,sam_len,sztmp, sztmp, &sam_sw);
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
                break;
            }
        }

        nresult = 0;
        g_saminf[nsamsck].keyidx = sztmp[0];

    }
    while (0);


    return nresult;
}

//========================================================
//����:��Ѱ���߸�Ӧ��Ʊ��
//��ڲ���
//   ��
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
//  unsigned char CardNum = 0;
//  unsigned char status=0, snr[12], snr1[5];
//  unsigned char tmp[20]; // added by zj
//  unsigned char ucTokenCnt=0;
//  unsigned char atqa,firstsnr[7];;
//  int nresult=0;
//
//  memset(firstsnr, 0x00, sizeof(firstsnr));
//
//  //ISO14443A_Init();
//
//  status=ISO14443A_Request( 0x52, tmp );
//
//  if (status==0)
//  {
//
//      atqa=tmp[0];
//      if (atqa!=0x44)
//      {
//          status=ISO14443A_Anticoll(snr);
//
//          if (status==0)
//          {
//              status=ISO14443A_Select(snr, tmp);
//
//              if ((tmp[0]&0x24)==0x20)
//              {
//                  nresult = 3; //cpu��
//              }
//              else
//              {
//                  nresult = 2; //m1��
//              }
//              CardNum++;
//              memcpy(firstsnr, snr, 4);
//              memcpy(lpcardno,firstsnr, 4);
//          };
//      }
//      else
//      {
//          //UL����Ʊ
//          if ((status=ISO14443A_Anticoll_UL(0x93, snr)) == 0)
//          {
//
//              if ((status=ISO14443A_Select_UL(0x93, snr, tmp)) == 0)
//              {
//
//                  if ((status=ISO14443A_Anticoll_UL(0x95, snr1)) == 0)
//                  {
//                      if ((status=ISO14443A_Select_UL(0x95, snr1, tmp)) == 0)
//                      {
//                          memcpy(lpcardno, &snr[1], 3);
//                          memcpy(lpcardno+3, snr1, 4);
//                          memcpy(firstsnr, lpcardno, 7);
//                          ucTokenCnt++;
//                          CardNum++;
//                          nresult = 1;
//
//                      }
//                  }
//              }
//          }
//      }
//  }
//
//  if ((status=ISO14443A_Request( 0x52, tmp ))==0)
//  {
//
//      atqa=tmp[0];
//
//      if (atqa!=0x44)
//      {
//          if ((status=ISO14443A_Anticoll(snr))==0)
//          {
//              if ((status=ISO14443A_Select(snr, tmp))==0)
//              {
//
//                  if (memcmp(snr, firstsnr, 4)!=0)
//                  {
//                      CardNum++;
//                  }
//                  if ((tmp[0]&0x24)==0x20)
//                  {
//                      nresult = 3; //cpu��
//                  }
//                  else
//                  {
//                      nresult = 2; //m1��
//                  }
//                  memcpy(lpcardno, snr, 4);
//
//              }
//
//          }
//      }
//      else
//      {
//          if ((status=ISO14443A_Anticoll_UL(0x93, snr)) == 0)
//          {
//              if ((status=ISO14443A_Select_UL(0x93, snr, tmp)) == 0)
//              {
//                  if ((status=ISO14443A_Anticoll_UL(0x95, snr1)) == 0)
//                  {
//                      if ((status=ISO14443A_Select_UL(0x95, snr1, tmp)) == 0)
//                      {
//                          memcpy(lpcardno, &snr[1], 3);
//                          memcpy(lpcardno+3, snr1, 4);
//                          if (memcmp(firstsnr, lpcardno, 7)!=0)
//                          {
//                              ucTokenCnt++;
//                              CardNum++;
//                          }
//                          nresult = 1;
//                      }
//                  }
//              }
//          }
//      }
//
//
//  }
//
//  if (CardNum>1)
//  {
//      nresult = 99;
//  }
//  else
//  {
//      memcpy(g_lstphysical,lpcardno, 8);
//  }
//
//  return nresult;
//
//}
int searchcard(unsigned char *lpcardno)
{
    unsigned char snr[12]       = {0};
    unsigned char snr1[5]       = {0};
    unsigned char tmp[100]      = {0};
    unsigned char ack[2][8]     = {{0},{0}};
    int card_type[2]            = {0};
    int ret                     = 0;

    unsigned char ulen, response_len;
    unsigned short response_sw;

    for (int i=0; i<2; i++)
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

    //add by shiyulong 2013-11-05; ��cpu  ���ٽ���ϸ��
    if (ret==3)
    {
        ret = 0;
        for (int i=0; i<2; i++)
        {
            if (ISO14443A_Request( 0x52, tmp )==0)
            {
                if (ISO14443A_Select(g_lstphysical, tmp)==0)
                {
                    if (MifareProRst(0x40,ulen,tmp)>0)
                    {
                        ret = 9;//Ĭ��Ϊ��֧�ֵĿ���
                        ulen = 5;
                        memcpy(tmp, "\x00\xB0\x85\x00\x02", ulen);
                        response_len = MifareProCom(ulen, tmp, &response_sw);
                        if (response_sw==0x9000)
                        {
                            if (memcmp(tmp, "\x53\x20",2)==0)
                            {
                                //����
                                ret = 3;
                            }
                            else if (memcmp(tmp, "\x59\x44",2)==0)
                            {
                                //����
                                ret = 4;
                            }

                        }


                    }
                }

                break;//ֻҪ����һ�μ��˳�ѭ��
            }
        }


    }

    //0=�޿�; 1=����Ʊ; 2=M1 ; 3=����Ʊ; 4=����; 5-8=Ԥ��; 9=��֧�ֿ��ࣻ99=�࿨
    return ret;

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

    return 0;//ǿ�Ʒ��سɹ�, add by shiyulong 2013-11-05

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

    return nresult;
}


//========================================================
//����:������Ʊ����
//��ڲ���
//   nsamsck=sam����Ӧ����
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

    cmd_ln = 18;
    memcpy(&cmd[0], "\x80\xFC\x01\x01\x0D", 5);
    memcpy(&cmd[5], sjt_card.detail.fix.fix_no_1, 4);
    memcpy(&cmd[9], sjt_card.detail.iss.log_no, 4);
    memcpy(&cmd[13],sjt_card.detail.iss.log_mac, 4);
    memcpy(&cmd[17], "\xA5", 1);

    status = sam_cmd(nsamsck,cmd_ln,cmd, cmd, &sam_sw);

    /* ������Կ */
    if (0x9000!=sam_sw)
    {

        //dbg_formatvar("sam_sw=%04X",sam_sw);
        return (-5);
    }

    memcpy(key_crypt, cmd, 6);

    memcpy(en_tmp, key_crypt+2, 4);
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
    //    memcpy(&en_tmp[0], &ass.sjt_key[2], 4);
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
	char status;
	int nresult = 0;
	int offset_pager;


	//ԭʼ�����ȼ��㲢��� crc8
	if ( idx>1 ) idx=1;
	crc8 = generate_crc8(lpbuffer, 15, key_crypt[idx]);
	lpbuffer[15] = crc8;


	//д����ǰ���м���

	memcpy(en_tmp, key_crypt+2, 4);

	for (i = 0; i < 4; i ++)
	{
		en_next[0] = lpbuffer[i*4];
		en_next[1] = lpbuffer[i*4 +1];
		en_next[2] = lpbuffer[i*4 + 2];
		en_next[3] = lpbuffer[i*4 + 3];

		lpbuffer[i*4] = lpbuffer[i*4] ^ en_tmp[0];
		lpbuffer[i*4 + 1] = lpbuffer[i*4 + 1] ^ en_tmp[1];
		lpbuffer[i*4 + 2] = lpbuffer[i*4 + 2] ^ en_tmp[2];
		lpbuffer[i*4 + 3] = lpbuffer[i*4 + 3] ^ en_tmp[3];

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
		status = ISO14443A_Write_UL(offset_pager+i,lpbuffer+j);
		if (status)
		{
			nresult = i;
			break;
		}
	}

	return nresult;
}


//=====================================================================

//======================================================================================
//����:���ɵ���Ʊtac ����
//��ڲ���
//        nsamsck=sam����Ӧ����
//        lplogicnumber=��������0-����������0��1-����������0
//        lptacdata=���������tac ����(�ݶ�Ϊ24  �ֽ�)
//���ڲ���
//       lpresult=samid(6B)+dsn(4B)+tac(4B)
//����ֵ
//       0=�ɹ�
//      -1=����sam  ����(�˴�����Ϊ���׳ɹ������ݣ�ֻ��Ϊ��־��Ϣ�����Ʊ������ɹ��������׳ɹ�)
//======================================================================================
int  ul_generate_tac(int nsamsck, unsigned char *lplogicnumber, unsigned char *lptacdata, unsigned char *lpresult)
{
    unsigned char sztmp[50];
    unsigned char ulen;
    unsigned char response_len;
    unsigned short response_sw;
    unsigned char tmpbuf[20];//samid(6B)+dsn(4B)+tac(4B)
    int nresult=0;


	memset(tmpbuf, 0, sizeof(tmpbuf));
	memcpy(tmpbuf,g_saminf[nsamsck].logicnumber, 6);
	tmpbuf[6] = ((g_saminf[nsamsck].dsn >> 24) & 0xff);
	tmpbuf[7] = ((g_saminf[nsamsck].dsn >> 16) & 0xff);
	tmpbuf[8] = ((g_saminf[nsamsck].dsn >> 8) & 0xff);
	tmpbuf[9] = (g_saminf[nsamsck].dsn & 0xff);
	

	//des��ʼ��
	ulen = 5;
    memcpy(sztmp, "\x80\x1A\x26\x01\x08", 5);
    memcpy(sztmp+ulen, lplogicnumber, 8);
    ulen += 8;
    response_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &response_sw);
    if (0x9000 != response_sw )
    {
		ulen = 5;
		memcpy(sztmp, "\x80\x1A\x26\x01\x08", 5);
		memcpy(sztmp+ulen, lplogicnumber, 8);
		ulen += 8;
		sam_cmd(nsamsck,ulen,sztmp, sztmp, &response_sw);
		if (response_sw != 0x9000)
		{
			nresult = -1;
		}
    }


	//����TAC
	ulen = 5;
    memcpy(sztmp, "\x80\xFA\x05\x00\x18", 5);
    memcpy(sztmp+ulen, lptacdata, 24);
    ulen += 24;
    response_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &response_sw);
    
    if (0x9000 == response_sw )
    {
		memcpy(tmpbuf+10,sztmp, 4);
    }
    else
    {
		nresult = -1;
    }

	//��дsam  ��ˮ
	g_saminf[nsamsck].dsn++;
    memcpy(sztmp, "\x00\xD6\x98\x00\x04\x00\x00\x00\x00", 9);
    ulen = 9;
    sztmp[5] = ((g_saminf[nsamsck].dsn >> 24) & 0xFF);
    sztmp[6] = ((g_saminf[nsamsck].dsn >> 16) & 0xFF);
    sztmp[7] = ((g_saminf[nsamsck].dsn >> 8) & 0xFF);
    sztmp[8] = (g_saminf[nsamsck].dsn  & 0xFF);

    response_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &response_sw);
	  
    if (0x9000 == response_sw )
    {
		g_saminf[nsamsck].dsn++;
    }
    else
    {
		nresult = -1;
    }

	memcpy(lpresult, tmpbuf, 14);
    

}


//========================================================
//����:�Ե���SAM  ���й��ܼ��ֻ�м������Ĳ��ܽ��з������ڽ���
//��ڲ���
//   nsamsck=sam����Ӧ����
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
//   aid=Ӧ��Ŀ¼��ʶ, 0x3F00=��Ӧ��Ŀ¼,  0x1001=ADF1Ŀ¼, 0x1002=ADF2Ŀ¼
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
        response_len = MifareProCom(ulen, sztmp, &response_sw);

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
//����: ���������ļ�,  Ҫ�Ƚ�����Ӧ��Ӧ��Ŀ¼������ȷ��ȡ
//��ڲ���
//   aid=���ļ���ʶ, 0x05=�����ļ�,  0x11=Ӧ�ÿ����ļ���0x15=������Ϣ�ļ�,  0x16=�ֿ��˸�����Ϣ�ļ�
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

        response_len = MifareProCom(ulen, sztmp, &response_sw);
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
            //        nresult = 0;
            memcpy(lpdata, sztmp, response_len-2);
            nresult=response_len-2;
        }

        return nresult;

    }

//========================================================
//����: ����¼�ļ�,  Ҫ�Ƚ�����Ӧ��Ӧ��Ŀ¼������ȷ��ȡ
//��ڲ���
//   aid=���ļ���ʶ, 0x17=�����ļ���0x18=��ʷ��ϸ
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
            nresult=response_len-2;
        }

        return nresult;

    }


//========================================================
//����: ����ʷ��ϸ,  Ҫ�Ƚ�����Ӧ��Ӧ��Ŀ¼������ȷ��ȡ
//��ڲ���
//   aid=���ļ���ʶ, 0x17=�����ļ���0x18=��ʷ��ϸ
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
        int nresult =  -1;


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
//   ��
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
//   nsamsck: ����SAM  ����Ӧ�Ŀ���,  ��0  ��ʼ
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

            memcpy(sztmp+ulen, g_saminf[nsamsck].terminalnumber, 6);
            ulen += 6;
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
//   nsamsck: ����SAM  ����Ӧ�Ŀ���,  ��0  ��ʼ
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

        memcpy(sztmp+ulen, g_saminf[nsamsck].terminalnumber, 6);
        ulen += 6;
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
        //      struct timeval end_tv;
        //      unsigned long bt,et;
        //
        //      gettimeofday(&end_tv, NULL);
        //      bt=end_tv.tv_sec*1000000L+end_tv.tv_usec;
        //  //  dbg_formatvar("begin=%d:%d",end_tv.tv_sec, end_tv.tv_usec);

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
                    for (i=1; i<11; i++)
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

        //      gettimeofday(&end_tv, NULL);
        //      et=end_tv.tv_sec*1000000L+end_tv.tv_usec;
        //  //  dbg_formatvar("end=%d:%d",end_tv.tv_sec, end_tv.tv_usec);
        //
        //      dbg_formatvar("read sum=%d",(et-bt)/1000);

        return(nresult);

    }


//========================================================
//����:�Ե���svt �����г�ֵ
//��ڲ���
//   nsamsck=sam����Ӧ����
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
    int svt_recharge(unsigned char * p_timenow, int nsamsck, int nstep, unsigned long value, unsigned char *lpinf)
    {

        unsigned char respone_len = 0;
        unsigned char sam_len = 0;
        unsigned char sztmp[256] = {0};
        unsigned short sam_sw = 0;
        unsigned char offline_terminal[6] = {0};
        TPSAMGETMAC PsamGetMac1;
        LPTINITCONSUME lpinitconsume = NULL;
        int nresult=-1;

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
                memcpy(sztmp+sam_len, g_saminf[nsamsck].terminalnumber, 6);
                sam_len += 6;

                respone_len = MifareProCom(sam_len,sztmp, &sam_sw);

                if (( respone_len==0 ) || (0x9000!=sam_sw))
                {
                    nresult = -1;
                    break;
                }

                memcpy(offline_terminal, sztmp+4, 2);
                lpinitconsume = (LPTINITCONSUME)sztmp;

                PsamGetMac1.encryver = lpinitconsume->encryver;
                PsamGetMac1.calcmarker = lpinitconsume->calcmarker;
                memcpy(PsamGetMac1.offlinedealserial, lpinitconsume->offlinedealserial, 2);
                memcpy(PsamGetMac1.random, lpinitconsume->random, 4);
                memcpy(PsamGetMac1.tm, p_timenow, 7);


                PsamGetMac1.trflag = 06;



                memcpy(PsamGetMac1.logicnumber, g_metroinf.logicnumber, 8);

                PsamGetMac1.money[0] = 0;

                PsamGetMac1.money[1] = 0;

                PsamGetMac1.money[2] = 0;

                PsamGetMac1.money[3] = 0;

                memcpy(sztmp,"\x80\x70\x00\x00\x1C",5);
                sam_len = 5;
                memcpy(sztmp+5, (char *)&PsamGetMac1, sizeof(PsamGetMac1));
                sam_len += sizeof(PsamGetMac1);
                respone_len = sam_cmd(nsamsck,sam_len,sztmp, sztmp, &sam_sw);
                if ((respone_len==0)||(0x9000!=sam_sw))
                {
                    nresult = -2;
                    break;
                }
                memcpy(offline_terminal+2, sztmp, 4);



                sam_len = 8;
                memcpy(sztmp, "\x00\x20\x00\x00\x03\x12\x34\x56",8);
                respone_len = MifareProCom(sam_len,sztmp, &sam_sw);
                if ((respone_len==0 ) || (0x9000!=sam_sw))
                {
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
                memcpy(sztmp+sam_len, g_saminf[nsamsck].terminalnumber, 6);
                sam_len += 6;


                respone_len = MifareProCom(sam_len,sztmp, &sam_sw);

                if ((respone_len==0 ) || (0x9000!=sam_sw))
                {
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
                respone_len = MifareProCom(sam_len,sztmp, &sam_sw);

                if ((respone_len==0 ) || (0x9000!=sam_sw))
                {
                    nresult = -4;
                    break;
                }

                memcpy(lpinf, sztmp, 4);
                nresult = 0;
            }

        }
        while (0);

        return(nresult);

    }

//========================================================
//����:�Ե���Ʊ�����ѻ�����
//��ڲ���
//   nsamsck=sam����Ӧ����
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

        //      struct timeval end_tv;
        //      unsigned long bt,et;

        //      gettimeofday(&end_tv, NULL);
        //      bt=end_tv.tv_sec*1000000L+end_tv.tv_usec;
        //  dbg_formatvar("begin=%d:%d",end_tv.tv_sec, end_tv.tv_usec);

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

            memcpy(sztmp+ulen, g_saminf[nsamsck].terminalnumber, 6);
            ulen += 6;
            //      dbg_dumpmemory("8050=",sztmp,ulen);
            respone_len = MifareProCom(ulen, sztmp, &sam_sw);
            //      dbg_dumpmemory("8050-",sztmp,respone_len);
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
                    //                  dbg_formatvar("80dc=%04X",sam_sw);
                    nresult = -4;
                    break;
                }



            }


            memcpy(sztmp,"\x80\x70\x00\x00\x1C",5);
            ulen = 5;
            memcpy(sztmp+5, (char *)&PsamGetMac1, sizeof(PsamGetMac1));
            ulen += sizeof(PsamGetMac1);
            //      dbg_dumpmemory("8070=",sztmp,ulen);
            respone_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &sam_sw);
            //      dbg_dumpmemory("8070-",sztmp,respone_len);


            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                //if (0x9000==sam_sw)
                nresult = -5;
                break;
            }

            memcpy(lpout->sam_terminalserial, sztmp, 4);
            memcpy(lpout->sam_terminalnumber, g_saminf[nsamsck].terminalnumber,6);

            memcpy(szmac, sztmp, 8);
            memcpy(sztmp,"\x80\x54\x01\x00\x0F", 5);
            ulen = 5;
            memcpy(sztmp+ulen, szmac, 4);
            ulen += 4;
            memcpy(sztmp+ulen, PsamGetMac1.tm, 7);
            ulen += 7;
            memcpy(sztmp+ulen, szmac+4, 4);
            ulen += 4;

            //      dbg_dumpmemory("8054=",sztmp,ulen);
            respone_len = MifareProCom(ulen, sztmp, &sam_sw);
            //      dbg_dumpmemory("8054-",sztmp,respone_len);

            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = -6;
                break;
            }
            memcpy(lpout->tac, sztmp, 4);
            memcpy(szmac, sztmp, 8);

            //memcpy(sztmp,"\x80\x72\x00\x00\x04",5);
            //ulen = 5;
            //memcpy(sztmp+5, szmac+4, 4);
            //ulen += 4;
            ////        dbg_dumpmemory("8072=",sztmp,ulen);
            //respone_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &sam_sw);
            ////        dbg_dumpmemory("8072-",sztmp,respone_len);
            //if ((respone_len==0) || (0x9000!=sam_sw))
            //{
            //  nresult = -7;
            //  break;
            //}

            nresult = 0;
        }
        while (0);

        //      gettimeofday(&end_tv, NULL);
        //      et=end_tv.tv_sec*1000000L+end_tv.tv_usec;
        //  dbg_formatvar("end=%d:%d",end_tv.tv_sec, end_tv.tv_usec);

        //  dbg_formatvar("consum sum=%d",(et-bt)/1000);

        return (nresult);

    }





//===============================================================
//����:��SVTƱ��Ӧ�ÿ����ļ����и���
//��ڲ���
//   nsamsck=sam����Ӧ����
//       file_id=���ļ���ʶ��Ŀǰֻ֧��0x11(Ӧ�ÿ����ļ�)��0x16(������Ϣ�ļ�)
//        offset=Ҫ�����ļ�����ʼ��ַ
//           nle=Ҫ���µ����ݳ���
//         lpinf=Ҫд�������

//
//���ڲ���
//  ��
//����ֵ
//       0=�ɹ�
//===============================================================
    int svt_update_binfile(int nsamsck,unsigned char *lplogicnumber,int file_id, int offset, int nlen, unsigned char *lpinf)
    {

        unsigned char respone_len;
        unsigned char sam_len;
        unsigned char sztmp[256];
        unsigned char random[8];
        unsigned char fulldata[8]= {0x80,0x00,0x00,0x00,0x00,0x00,0x00, 0x00};
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
            memcpy(sztmp, "\x04\xD6\x96\x00\x06",5);
            sztmp[2]=paramter_p1;
            sztmp[3]=offset;
            sztmp[4]=nlen+4;//+mac
            sam_len = 5;
            memcpy(sztmp+sam_len,lpinf,nlen);
            sam_len += nlen;
            memcpy(sztmp+sam_len,random,4);
            sam_len += 4;

            if (full_len)
            {
                memcpy(sztmp+sam_len, fulldata, full_len);
                sam_len += full_len;
            }



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

            memcpy(sztmp+ulen, g_saminf[nsamsck].terminalnumber, 6);
            ulen += 6;
            //      dbg_dumpmemory("8050=",sztmp,ulen);
            response_len = MifareProCom(ulen, sztmp, &sam_sw);
            //      dbg_dumpmemory("8050-",sztmp,response_len);
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
            memcpy(lpout->sam_terminalnumber, g_saminf[nsamsck].terminalnumber,6);

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
            response_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &sam_sw);
            if ((response_len==0) || (0x9000!=sam_sw))
            {
                nresult = -7;
                break;
            }

            nresult = 0;
        }
        while (0);


        return (nresult);

    }

//===============================================================
//����:��SVTƱ�ĸ����ļ�ָ����¼���и���
//��ڲ���
//   nsamsck=sam����Ӧ����
//  lplogicnumber=Ʊ���߼�����
//        record_id=��¼ID
//           lock=�������
//         lpinf=�����ļ���ӦҪд�������(Ҫ���ȡʱ��õ���ͬ)

//
//���ڲ���
//  ��
//����ֵ
//       0=�ɹ�
//===============================================================
    int svt_unlock_recordfile(int nsamsck,unsigned char *lplogicnumber,int record_id, unsigned char lock, unsigned char *lpinf)
    {

        unsigned char respone_len;
        unsigned char sam_len;
        unsigned char sztmp[256];
        unsigned char random[8];
        unsigned char fulldata[8]= {0x80,0x00,0x00,0x00,0x00,0x00,0x00, 0x00};
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
                nresult = -1;
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
            sztmp[sam_len+3]=0xB8; //(((fild_id << 3) & 0xF8) | 0x04);;
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
                nresult = -1;
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
//    recordcnt=Ҫ���µļ�¼����
//    dim_record_id=Ҫ���µĶ�����¼������
//    dim_lock=��Ӧ������¼��������ʶ����,=0������
//    dim_lpfile=�����ļ����µĶ�����¼���������ݣ���ֵʱ��Ҫע�ⲻͬ�ļ�¼�Ŷ�Ӧ�����ݳ���( ��������Ϊ������¼ǰ�����ض����ݵ�����)
//���ڲ���
//       respone_data=SAM�ն˺�(6BYTE) +  SAM�ն���ˮ( 4Byte) + TAC (4Byte)
//����ֵ
//       0=�ɹ�
//========================================================
    uint16_t svt_updata_more_complex(int nsamsck, unsigned char *lplogicnumber, long lAmount, unsigned char recordcnt,
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

            memcpy(sztmp+ulen, g_saminf[nsamsck].terminalnumber, 6);
            ulen += 6;
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
            memcpy(PsamGetMac1.tm, "\x20\x13\x04\x23\x20\x00\x20", 7);

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
            offset=0;
            for (i=0; i<recordcnt; i++)
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
            memcpy(lpout->sam_terminalnumber, g_saminf[nsamsck].terminalnumber,6);

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
            response_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &sam_sw);

            nresult = 0;

        }
        while (0);

        return (nresult);
    }

    uint16_t updata_more_complex(int nsamsck, unsigned char *lplogicnumber, long lAmount, unsigned char recordcnt, PCOMPLEX_FILE complex_arr, unsigned char *respone_data)
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

            memcpy(sztmp+ulen, g_saminf[nsamsck].terminalnumber, 6);
            ulen += 6;
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
            memcpy(PsamGetMac1.tm, "\x20\x13\x04\x23\x20\x00\x20", 7);

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
            for (i=0; i<recordcnt; i++)
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
            memcpy(lpout->sam_terminalnumber, g_saminf[nsamsck].terminalnumber,6);

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
            response_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &sam_sw);

            nresult = 0;

        }
        while (0);

        return (nresult);
    }

//========================================================
//����:��һ��ͨƱ�����ѻ�����
//��ڲ���
//   nsamsck=sam����Ӧ����
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

            memcpy(sztmp+ulen, g_saminf[nsamsck].terminalnumber, 6);
            ulen += 6;
            //      dbg_dumpmemory("8050=",sztmp,ulen);
            respone_len = MifareProCom(ulen, sztmp, &sam_sw);
            //      dbg_dumpmemory("8050-",sztmp,respone_len);
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

                //          memcpy(sztmp, "\x80\xDC\x02\xBC\x30", 5);     //����Ʊ
                //          memcpy(sztmp+ulen, "\x02\x2E\x00", 3);
                memcpy(sztmp, "\x80\xDC\x03\xB8\x20", 5);//��ɳһ��ͨ
                ulen = 5;
                memcpy(sztmp+ulen, "\x03\x1E\x00", 3);
                ulen += 3;
                memcpy(sztmp+ulen, lpmetrofile, 29);
                ulen += 29;

                respone_len = MifareProCom(ulen, sztmp, &sam_sw);
                if ((respone_len==0) || (0x9000!=sam_sw))
                {
                    //                  dbg_formatvar("80dc=%04X",sam_sw);
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

            //      dbg_dumpmemory("8070=",sztmp,ulen);
            respone_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &sam_sw);
            //      dbg_dumpmemory("8070-",sztmp,respone_len);


            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                //if (0x9000==sam_sw)
                nresult = -5;
                break;
            }

            memcpy(lpout->sam_terminalserial, sztmp, 4);
            memcpy(lpout->sam_terminalnumber, g_saminf[nsamsck].terminalnumber,6);

            memcpy(szmac, sztmp, 8);
            memcpy(sztmp,"\x80\x54\x01\x00\x0F", 5);
            ulen = 5;
            memcpy(sztmp+ulen, szmac, 4);
            ulen += 4;
            memcpy(sztmp+ulen, PsamGetMac1.tm, 7);
            ulen += 7;
            memcpy(sztmp+ulen, szmac+4, 4);
            ulen += 4;

            //      dbg_dumpmemory("8054=",sztmp,ulen);
            respone_len = MifareProCom(ulen, sztmp, &sam_sw);
            //      dbg_dumpmemory("8054-",sztmp,respone_len);

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
            //      dbg_dumpmemory("8072=",sztmp,ulen);
            respone_len = sam_cmd(nsamsck,ulen,sztmp, sztmp, &sam_sw);
            //      dbg_dumpmemory("8072-",sztmp,respone_len);
            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                //Ʊ���ۿ�ɹ�����SAM  ��������ζ����ɹ������ó˿�ͨ��
                //nresult = -7;
                //break;
            }

            nresult = 0;
        }
        while (0);

        //      gettimeofday(&end_tv, NULL);
        //      et=end_tv.tv_sec*1000000L+end_tv.tv_usec;
        //  dbg_formatvar("end=%d:%d",end_tv.tv_sec, end_tv.tv_usec);

        //  dbg_formatvar("consum sum=%d",(et-bt)/1000);

        return (nresult);

    }
