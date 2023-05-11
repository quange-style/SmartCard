#include "string.h"
#include "stdio.h"
#include "sam.h"
#include "iso14443.h"
#include "octcard.h"
#include "../api/Errors.h"
#include "../api/Records.h"
#include "../link/myprintf.h"

#define MI_OK 0
#define PICC_AUTHENT1A 0x60
#define PICC_AUTHENT1B 0x61


#define CRC_AFFIX  "CS-METRO-2013@GDMH"



#define OCTM1SAM_SOCKET 0x02 //OCTSAM�������( ��0 ��ʼ)


#define OCT_SECTOR_APP 0       //Ӧ�ñ�ʶ������
#define OCT_SECTOR_ISSUE 1     //����������
#define OCT_SECTOR_PURSE 2     //Ǯ��������
#define OCT_SECTOR_PUBLIC 9    //������Ϣ������
#define OCT_SECTOR_METRO 11    //������Ϣ������

#define OCT_BLK_ISSUESUE_IDX 1  //������Ϣ��
#define OCT_BLK_ADDTIONALISSUE_IDX 6  //Ӧ���������ݿ�

#define OCT_BLK_PURSE_ORG_IDX 8  //����Ǯ������
#define OCT_BLK_PURSE_BAK_IDX 9  //����Ǯ������

#define OCT_BLK_PURSE_OTHER_ORG_IDX 16  //ר��Ǯ������
#define OCT_BLK_PURSE_OTHER_BAK_IDX 17  //ר��Ǯ������

#define OCT_BLK_PUBLIC_ORG_IDX 36  //������Ϣ������
#define OCT_BLK_PUBLIC_BAK_IDX 37  //������Ϣ������
#define OCT_BLK_HISTORY_IDX 38     //������Ϣ�����̽�����ʷ

#define OCT_BLK_METRO_ORG_IDX 44   //�������ݿ�����
#define OCT_BLK_METRO_BAK_IDX 45   //�������ݿ鸱��



//���ļ���ʶ
#define OCTSAM_FILE_MF 0x3F00
#define OCTSAM_FILE_ADF1 0x1001
#define OCTSAM_FILE_ADF2 0x1002
#define OCTSAM_FILE_ADF3 0x1003


#define PSAM_OCTMF_SCK 0
#define PSAM_OCTCPU_SCK 1


// typedef struct
// {
//     unsigned char logic_id[8];//�߼�����
//     unsigned char key_history[6];//key04
//     unsigned char key_purse[6];//key20_1
//     unsigned char key_public[6];//key20_3
//     unsigned char public_blk[16];//����ʱʹ��
// } TOCT_KEY, *PTOCT_KEY;
typedef struct
{
    unsigned char key_avalid; //��Կ�Ƿ���Ч����, 00=��Ч��>0��Ч
    unsigned char keya[6];//key04
    unsigned char keyb[6];//key20_1
    unsigned char public_blk[16];//����ʱʹ��
} TOCT_KEY, *PTOCT_KEY;


// 0 ��Ƭ������ HEX  50�������ÿ���
// 1 Ӧ�������� HEX  "01����ͨǮ���� A3�����˿� A4��ѧ����     A5��������Ʊ�� A6���м���Ʊ�� A7������ԱƱ�� A8������ѧ����"
// 2��5  Ӧ�ÿ���� BCD  ����TAC��������Ҫ�õ�
// 6��8  �ۿ�����  BCD  �ꡢ�¡��գ�yymmdd��
// 9��10  �ۿ�ʱ��  BCD  ʱ���֣�hhmm��
// 11��12 �ۿ��ص�  BCD  �ۿ��ص���
// 13 �ۿ�Ѻ��  HEX  Ԫ��0.00��
// 14 ���ñ�־  HEX  "AA��δ���ã�BB�����ã���CPU��M1ʱ=02δ������Ҫ��CPU������"
// 15 У����  HEX  CRC8

typedef struct
{
//==================================================
    unsigned char szbalance[4];
//==================================================
    unsigned char type_major;//��Ƭ������ 50�����ÿ�
    unsigned char type_minjor;//Ӧ��������
    // 01����ͨǮ����
    // A3�����˿�
    // A4��ѧ����
    // A5��������Ʊ��
    // A6���м���Ʊ��
    // A7������ԱƱ��
    // A8������ѧ����
    unsigned char appserial[4]; //Ӧ�ÿ���� bcd
    unsigned char sale_dt[5];//����ʱ�� bcd, yymmddhhnn
    unsigned char sale_addr[2]; //���۵ص��� bcd,
    unsigned char deposit;//Ѻ��,Ԫ
    unsigned char appflag;//Ӧ�ñ�ʶ,AA��δ���ã�BB�����ã�=02��CPU������
//===================
    unsigned char verify_dt[3]; //������ yymmdd
    unsigned char pursemode; //���ڴ�ֵ���
    //0x00������ר��Ǯ��������Ǯ����δ���ã�
    //0x01��ר��Ǯ�������ã�
    //0x02������Ǯ�������ã�
    //0x03��ר��Ǯ��������Ǯ����������
    unsigned char city[2];  //���д���
    unsigned char industry[2]; //��ҵ����
//=================================================
    unsigned char common_count[2]; //����Ǯ�����״���,��λ��ǰ
    unsigned char other_count[2];  //ר��Ǯ�����״���,��λ��ǰ
    unsigned char dt_trdate[4]; //��������yyyy-mm-dd
    unsigned char lock_flag; //04��������������
//=================================================
    unsigned char tr_type;//��������
    unsigned char servicer_code[2];//�����̴���
    unsigned char terminal_no[4];   //POS��� 4BYTE;
    unsigned char dt_trdate2[3]; //������ʷ��������yy-mm-dd
    unsigned char banlance[3];  //����ǰ���
    unsigned char tr_amount[2];    //���׽��
//==================================================
    unsigned char metro_inf[14]; //����ҪCRC16;



}TOUTPUT, *LPTOUTPUT;

typedef struct
{
//==================================================
    unsigned char szbalance[4];
	unsigned char szbalance2[4];
//==================================================
    unsigned char type_major;//��Ƭ������ 50�����ÿ�
    unsigned char type_minjor;//Ӧ��������
    // 01����ͨǮ����
    // A3�����˿�
    // A4��ѧ����
    // A5��������Ʊ��
    // A6���м���Ʊ��
    // A7������ԱƱ��
    // A8������ѧ����
    unsigned char appserial[4]; //Ӧ�ÿ���� bcd
    unsigned char sale_dt[5];//����ʱ�� bcd, yymmddhhnn
    unsigned char sale_addr[2]; //���۵ص��� bcd,
    unsigned char deposit;//Ѻ��,Ԫ
    unsigned char appflag;//Ӧ�ñ�ʶ,AA��δ���ã�BB�����ã�=02��CPU������
//===================
    unsigned char verify_dt[3]; //������ yymmdd
    unsigned char pursemode; //���ڴ�ֵ���
    //0x00������ר��Ǯ��������Ǯ����δ���ã�
    //0x01��ר��Ǯ�������ã�
    //0x02������Ǯ�������ã�
    //0x03��ר��Ǯ��������Ǯ����������
    unsigned char city[2];  //���д���
    unsigned char industry[2]; //��ҵ����
//=================================================
    unsigned char common_count[2]; //����Ǯ�����״���,��λ��ǰ
    unsigned char other_count[2];  //ר��Ǯ�����״���,��λ��ǰ
    unsigned char dt_trdate[4]; //��������yyyy-mm-dd
    unsigned char lock_flag; //04��������������
//=================================================
    unsigned char tr_type;//��������
    unsigned char servicer_code[2];//�����̴���
    unsigned char terminal_no[4];   //POS��� 4BYTE;
    unsigned char dt_trdate2[3]; //������ʷ��������yy-mm-dd
    unsigned char banlance[3];  //����ǰ���
    unsigned char tr_amount[2];    //���׽��
//==================================================
    unsigned char metro_inf[14]; //����ҪCRC16;



}TOUTPUT2, *LPTOUTPUT2;


//���ݿ�ṹ����
//������
typedef struct
{
    unsigned char type_major;//��Ƭ������ 50�����ÿ�
    unsigned char type_minjor;//Ӧ��������
    // 01����ͨǮ����
    // A3�����˿�
    // A4��ѧ����
    // A5��������Ʊ��
    // A6���м���Ʊ��
    // A7������ԱƱ��
    // A8������ѧ����
    unsigned char appserial[4]; //Ӧ�ÿ���� bcd
    unsigned char sale_dt[5];//����ʱ�� bcd, yymmddhhnn
    unsigned char sale_addr[2]; //���۵ص��� bcd,
    unsigned char deposit;//Ѻ��,Ԫ
    unsigned char appflag;//Ӧ�ñ�ʶ,AA��δ���ã�BB�����ã�=02��CPU������
    unsigned char crc8;
} TOCT_BLK_ISSUE_DETAIL, *PTOCT_BLK_ISSUE_DETAIL;

//Ӧ��������
typedef struct
{
    unsigned char verify_dt[3]; //������ yymmdd
    unsigned char pursemode;    //���ڴ�ֵ���
    //0x00������ר��Ǯ��������Ǯ����δ���ã�
    //0x01��ר��Ǯ�������ã�
    //0x02������Ǯ�������ã�
    //0x03��ר��Ǯ��������Ǯ����������
    unsigned char city[2];      //���д���
    unsigned char industry[2];  //��ҵ����
    unsigned char cardmac[4];//����֤��
    unsigned char rfu[3];
    unsigned char crc8;
} TOCT_BLK_ADDTIONALISSUE_DETAIL, *PTOCT_BLK_ADDTIONALISSUE_DETAIL;



//����Ǯ��
typedef struct
{
    unsigned char purse1[4];
    unsigned char purse2[4];
    unsigned char purse3[4];
    unsigned char addr[4];
} TOCT_BLK_COMMON_PURSE_DETAIL, *PTOCT_BLK_COMMON_PURSE_DETAIL;

//������Ϣ��
typedef struct
{
    unsigned char proc_flag;//���̱�ʶ
    //0x11������Ǯ�����׿�ʼ��
    //0x12��ר��Ǯ�����׿�ʼ��
    //0x13������Ǯ�����׽�����
    //0x14��ר��Ǯ�����׽�����
    unsigned char tr_type;  //��������
    //0x01������Ǯ����ֵ���ף�
    //0x02��ר��Ǯ����ֵ���ף�
    //0x03������Ǯ�����ѽ��ף�
    //0x04��ר��Ǯ�����ѽ��ף�
    unsigned char tr_pt;    //����ָ��
    unsigned char common_count[2]; //����Ǯ�����״���,��λ��ǰ
    unsigned char other_count[2];  //ר��Ǯ�����״���,��λ��ǰ
    unsigned char dt_trdate[4]; //��������yyyy-mm-dd
    unsigned char lock_flag;    //04��������������
    unsigned char rfu[3];
    unsigned char crc8;         //У��
} TOCT_BLK_PUBLIC_DETAIL, *PTOCT_BLK_PUBLIC_DETAIL;

//��ʷ���ݿ�
typedef struct
{
    unsigned char tr_type;//��������
    unsigned char servicer_code[2];//�����̴���
    unsigned char terminal_no[4];     //POS��� 4BYTE;
    unsigned char dt_trdate[3]; //��������yy-mm-dd
    unsigned char banlance[3];   //����ǰ���
    unsigned char tr_amount[2];    //���׽��
    unsigned char crc8;         //У��
} TOCT_BLK_HISTORY_DETAIL, *PTOCT_BLK_HISTORY_DETAIL;


//�������ݿ�
typedef struct
{
unsigned char verison:
    4;//���汾
unsigned char device_type:
    4;//�豸����
unsigned char status:
    4; //��״̬
unsigned char device_code_hi:
    4;//�豸��Ÿ�λ
    unsigned char device_code_lo;  //�豸��ŵ�λ
    unsigned char lst_station[2];//���һ�β�����·վ��
    unsigned char tm4[4];//���һ�β���ʱ��
    unsigned char lst_entry[2];//���һ�ν�վ��·վ��
    unsigned char rfu[3];
    unsigned char crc16[2];
} TOCT_METRO_DETAIL, *PTOCT_METRO_DETAIL;


typedef union
{
    unsigned char buf[16];
    TOCT_BLK_ISSUE_DETAIL detail;
} OCT_BLK_ISSUESUE;

typedef union
{
    unsigned char buf[16];
    TOCT_BLK_ADDTIONALISSUE_DETAIL detail;
} OCT_BLK_ADDTIONALISSUE;


typedef union
{
    unsigned char buf[16];
    TOCT_BLK_COMMON_PURSE_DETAIL detail;
} OCT_BLK_PURSE;

typedef union
{
    unsigned char buf[16];
    TOCT_BLK_PUBLIC_DETAIL detail;
} OCT_BLK_PUBLIC;

typedef union
{
    unsigned char buf[16];
    TOCT_BLK_HISTORY_DETAIL detail;
} OCT_BLK_HISTORY;

typedef union
{
    unsigned char buf[16];
    TOCT_METRO_DETAIL detail;
} OCT_BLK_METRO;



typedef struct
{
//  Ǯ���й���Ǯ����ר��Ǯ�������ṹ�ж���2Ϊר��Ǯ��
    unsigned long lpurse_common_org;//Ǯ������ֵ
    unsigned long lpurse_common_bak;//Ǯ������ֵ

    unsigned long lpurse_other_org;//Ǯ������ֵ
    unsigned long lpurse_other_bak;//Ǯ������ֵ

//
//     unsigned short ntr_count_org;//������Ϣ������Ǯ������Ʊ�����׼���ֵ
//     unsigned short ntr_count_bak;//������Ϣ������Ǯ������Ʊ�����׼���ֵ
//
//
//     unsigned short ntr_count_other_org;//������Ϣ��ר��Ǯ������Ʊ�����׼���ֵ
//     unsigned short ntr_count_other_bak;//������Ϣ��ר��Ǯ������Ʊ�����׼���ֵ

    unsigned char lst_tr_type;//�ϴν�������


//�����������ڴ��������ֻ��Ҫ�������ݣ�Ϊ�˽�ʡ�ռ䣬��������й̶�����

    OCT_BLK_ISSUESUE blk_issue;
    OCT_BLK_ADDTIONALISSUE blk_addtional;

    OCT_BLK_PURSE blk_purse_common_org;  //����Ǯ��������
    OCT_BLK_PURSE blk_purse_common_bak;  //����Ǯ���鸱��

    OCT_BLK_PURSE blk_purse_other_org;  //ר��Ǯ��������
    OCT_BLK_PURSE blk_purse_other_bak;  //ר��Ǯ���鸱��


//����Ǯ����ר��Ǯ�����ù�����Ϣ����ʷ��
    OCT_BLK_PUBLIC blk_public_org;    //������Ϣ����
    OCT_BLK_PUBLIC blk_public_bak;    //������Ϣ����
    OCT_BLK_HISTORY blk_history;      //��ʷ���ݿ�
    OCT_BLK_METRO blk_metro_org; //������Ϣ������
    OCT_BLK_METRO blk_metro_bak; //������Ϣ������

//    OCT_BLK_HISTORY blk_newhistory;      //����ʷ���ݿ飬��Ҫд�������
}  TOCT_INF, *PTOCT_INF;

typedef struct
{
    unsigned char tr_type;//��������
    unsigned char tr_datetime[7];//����ʱ��
    unsigned char before_balance[4];//����ǰ���
    unsigned char tr_money[2];//���׽��
    unsigned char tr_count[2];//Ʊ������
//    unsigned char padding[7];//�̶�Ϊ80 00 00 00 00 00 00
} TOCT_TAC, *PTOCT_TAC; //des��ʼ������ȡ����˳��



typedef struct
{
    unsigned char physicalnumber[4];//������
    unsigned char logiccardnumber[8];

    unsigned char currsector;//��ǰ����֤������
} TOCT_CURRINF, *PTOCT_CURRINF;


struct Err_Info
{
    unsigned char szSnr[4];
    unsigned long ulRemain;
};


//M1���ϵ���Ϣ
typedef struct
{
    unsigned char Logicno[8];//�߼�����
    unsigned char npursetype;//Ǯ������0=����Ǯ��, 1=ר��Ǯ��
    unsigned long balance_before;//����ǰ���
    unsigned long balance_after;//���Ѻ����
} BREAKINF;


TOCT_KEY g_octkeyinf; //��ǰOCT  ��key �������Ϣ

TOCT_CURRINF g_octcurrinf;//��ǰ����Ʊ���Ķ�̬��Ϣ

BREAKINF g_m1recover;

static unsigned short CRC16_TAB[] =
{
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

unsigned short crc16(unsigned char * lpData, unsigned short nLen)
{
    unsigned short wCrc = 0xFFFF;
    unsigned short n;
    for (n=0;n<nLen;n++)
    {
        wCrc = (wCrc >> 8) ^ CRC16_TAB[(wCrc ^ lpData[n]) & 0xFF];
    }

    return (unsigned short)(wCrc^0xFFFF);
}


//========================================================================
//�� �� ��: crc_ck
//��    ��: У�����ݿ��crc8��ֵ�Ƿ���ȷ
//�������:
//          string_cu:��Ҫ��������ݿ�ָ��
//             length:�����ܳ��ȣ�����Ҫ���CRC8��һ���ֽ�
//���ڲ���:
//   string_ck:����crc8ֵ������
//�� �� ֵ:
//          =0��ȷ
//         !=0����
//========================================================================
unsigned char crc16_ck(unsigned char *string_ck,unsigned char nlength)
{
    unsigned char data_src[256] = {0};
    unsigned short crc16_value;
    unsigned short crc16_cmp;
    size_t pos = strlen(CRC_AFFIX);

    memcpy((unsigned char *)&crc16_cmp, string_ck+(nlength-2),2);

    memcpy(data_src, CRC_AFFIX, pos);
    memcpy(data_src + pos, string_ck, nlength);
    crc16_value=crc16(data_src, pos + nlength - 2);

    if (crc16_value==crc16_cmp)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

//========================================================================
//�� �� ��: crc_cu
//��    ��: �������ݿ��crc8��ֵ���������
//�������:
//          string_cu:��Ҫ��������ݿ�ָ��
//             length:�����ܳ��ȣ�����Ҫ���CRC8��һ���ֽ�
//���ڲ���:
//   string_cu:�����crc8ֵ����������
//�� �� ֵ:
//          ��
//========================================================================
void crc16_cu(unsigned char *string_cu,unsigned short nlength)
{

    unsigned char data_src[256] = {0};
    unsigned short crc16_value;
    size_t pos = strlen(CRC_AFFIX);

    memcpy(data_src, CRC_AFFIX, pos);
    memcpy(data_src + pos, string_cu, nlength);
    crc16_value=crc16(data_src, pos + nlength - 2);
    memcpy(string_cu + nlength - 2, &crc16_value, 2);
}





//========================================================================
//�� �� ��: crc_c
//��    ��: crc8�ļ���
//�������:
//          x:��Ҫ����crc8���ֽ�����
//       temp:����Ԥ��ֵ
//���ڲ���:
//   ��
//�� �� ֵ:
//          �����CRC8ֵ
//========================================================================

unsigned char crc_c(unsigned char x,unsigned char temp)
{
    unsigned char i,y;
    for ( i=0;i<8;i++)
    {
        y=x^temp;
        if (y&=0x01 )temp^=0x18;
        temp>>=1;
        if (y)temp|=0x80;
        x>>=1;
    }
    return(~temp);
}

//========================================================================
//�� �� ��: crc_ck
//��    ��: У�����ݿ��crc8��ֵ�Ƿ���ȷ
//�������:
//          string_cu:��Ҫ��������ݿ�ָ��
//             length:�����ܳ��ȣ�����Ҫ���CRC8��һ���ֽ�
//���ڲ���:
//   string_ck:����crc8ֵ������
//�� �� ֵ:
//          =0��ȷ
//         !=0����
//========================================================================
unsigned char crc_ck(unsigned char *string_ck,unsigned char length)
{
    unsigned char temp=0;
    unsigned char j;
    for ( j=0;j<length;j++)temp=crc_c(string_ck[j],temp);
    temp=~temp;
    if ( temp)return(1);
    return(0);
}


//========================================================================
//�� �� ��: crc_cu
//��    ��: �������ݿ��crc8��ֵ���������
//�������:
//          string_cu:��Ҫ��������ݿ�ָ��
//             length:�����ܳ��ȣ�����Ҫ���CRC8��һ���ֽ�
//���ڲ���:
//   string_cu:�����crc8ֵ����������
//�� �� ֵ:
//          ��
//========================================================================
void crc_cu(unsigned char *string_cu,unsigned char length)
{
    unsigned char temp=0;
    unsigned char j;
    for ( j=0;j<(length-1);j++)temp=crc_c(string_cu[j],temp);
    string_cu[j]=temp;
}

//==========================================================================================
//�� �� ��: octsam_chk
//��    ��: ���SAM���Ƿ���ڣ��Է�������ʼ���󱻰ε����M1���ɿ۷ѵ����ܼ���TAC�����
//��ڲ���:
//          idx_sam:��ǰӦ��SAM����Ӧ�Ŀ������к�(��0��ʼ)
//���ڲ���:
//             ��
//�� �� ֵ:
//          0=sam������
//==========================================================================================
unsigned char octsam_chk(unsigned char idx_sam)
{
    const unsigned char cmd_selectfile[]={0x00, 0xA4, 0x04, 0x00, 0x09, 0xA3, 0x00, 0x00, 0x57, 0x64, 0x62, 0x69, 0x61, 0x6F};


    unsigned char ret=0;
    unsigned char tmpbuf[64];
    unsigned char uclen;
    unsigned short int response_sw;


	do{

        uclen = sizeof(cmd_selectfile);
        memcpy(tmpbuf, cmd_selectfile, uclen);

        sam_cmd(idx_sam, uclen, tmpbuf, tmpbuf, &response_sw);
        if (response_sw==0xFFFF)
        {
            ret = 0x05;
            break;
        }
        else if (response_sw != 0x9000)
        {
            ret = 0x05;
            //break;
        }

        ret = 0;


    }
    while (0);

    return ret;


}


unsigned char octsam_exportkey(unsigned char idx_sam)
{
    const unsigned char cmd_selectfile[]={0x00, 0xA4, 0x04, 0x00, 0x09, 0xA3, 0x00, 0x00, 0x57, 0x64, 0x62, 0x69, 0x61, 0x6F};
    const unsigned char cmd_initdes[2][13]=
    {
        {0x80, 0x1A, 0x28, 0x01, 0x08, 0x07, 0x56, 0x71, 0x68, 0x90, 0x72, 0x89, 0x68},
        {0x80, 0x1A, 0x28, 0x02, 0x08, 0x07, 0x56, 0x71, 0x68, 0x90, 0x72, 0x89, 0x68}
    };

    const unsigned char cmd_descrypt[]={0x80, 0xFA, 0x00, 0x00, 0x08, 0x07, 0x56, 0x08, 0x56, 0x05, 0x51, 0x07, 0x73};

    unsigned char ret=0;
    unsigned char tmpbuf[64];
    unsigned char uclen;
    unsigned short int response_sw;

    g_octkeyinf.key_avalid = 0x00;

    do
    {
//         sam_setbaud(idx_sam, SAM_BAUDRATE_38400);
//
//         ret = sam_rst(idx_sam,tmpbuf);
//         if ( ret <= 0 )  {ret = 6;break;}


        uclen = sizeof(cmd_selectfile);
        memcpy(tmpbuf, cmd_selectfile, uclen);

        sam_cmd(idx_sam, uclen, tmpbuf, tmpbuf, &response_sw);
        if (response_sw==0xFFFF)
        {
            ret = 0x05;
            break;
        }
        else if (response_sw != 0x9000)
        {
            ret = 0x05;
            //break;
        }

        //����KEYA===============================
        uclen = 13;
        memcpy(tmpbuf, cmd_initdes[0], uclen);

        sam_cmd(idx_sam, uclen, tmpbuf, tmpbuf, &response_sw);
        if (response_sw==0xFFFF)
        {
            break;
        }
        else  if (response_sw != 0x9000)
        {
            break;
        }


        uclen = sizeof(cmd_descrypt);
        memcpy(tmpbuf, cmd_descrypt, uclen);

        sam_cmd(idx_sam, uclen, tmpbuf, tmpbuf, &response_sw);
        if (response_sw==0xFFFF)
        {
            break;
        }
        else  if (response_sw != 0x9000)
        {
            break;
        }
        memcpy(g_octkeyinf.keya, tmpbuf+2, 6);

        //����KEYB=========================================
        uclen = 13;
        memcpy(tmpbuf, cmd_initdes[1], uclen);

        sam_cmd(idx_sam, uclen, tmpbuf, tmpbuf, &response_sw);
        if (response_sw==0xFFFF)
        {
            break;
        }
        else  if (response_sw != 0x9000)
        {
            break;
        }


        uclen = sizeof(cmd_descrypt);
        memcpy(tmpbuf, cmd_descrypt, uclen);

        sam_cmd(idx_sam, uclen, tmpbuf, tmpbuf, &response_sw);
        if (response_sw==0xFFFF)
        {
            break;
        }
        else  if (response_sw != 0x9000)
        {
            break;
        }
        memcpy(g_octkeyinf.keyb, tmpbuf+2, 6);

        g_octkeyinf.key_avalid = 0xFF;
        ret = 0;


    }
    while (0);

    return ret;


}


char octm1_readblock(unsigned char blk_idx, unsigned char *szdata)
{
    char ret=0;
    unsigned char currsector = (blk_idx >> 2);

    if (currsector != g_octcurrinf.currsector)
    {
        if (currsector == OCT_SECTOR_PUBLIC)
        {
            ret = ISO14443A_Authentication(PICC_AUTHENT1B, g_octcurrinf.physicalnumber, g_octkeyinf.keyb, blk_idx);
        }
        else if (currsector == OCT_SECTOR_METRO)
        {
            ret = ISO14443A_Authentication(PICC_AUTHENT1B, g_octcurrinf.physicalnumber, g_octkeyinf.keyb, blk_idx);
        }
        else
        {
            ret = ISO14443A_Authentication(PICC_AUTHENT1A, g_octcurrinf.physicalnumber, g_octkeyinf.keya, blk_idx);
        }
    }

    if (ret==MI_OK)
    {
        ret=ISO14443A_ReadBlock(blk_idx, szdata);

    }

    return ret;


}

char octm1_writeblock(unsigned char blk_idx, unsigned char *szdata)
{

    char ret=0;
    unsigned char currsector = (blk_idx >> 2);

    if (currsector != g_octcurrinf.currsector)
    {
        if (currsector == OCT_SECTOR_PUBLIC)
        {
            ret = ISO14443A_Authentication(PICC_AUTHENT1B, g_octcurrinf.physicalnumber, g_octkeyinf.keyb, blk_idx);
        }
        else if (currsector == OCT_SECTOR_METRO)
        {
            ret = ISO14443A_Authentication(PICC_AUTHENT1B, g_octcurrinf.physicalnumber, g_octkeyinf.keyb, blk_idx);
        }
        else
        {
            ret = ISO14443A_Authentication(PICC_AUTHENT1A, g_octcurrinf.physicalnumber, g_octkeyinf.keya, blk_idx);
        }
    }



    if (ret==MI_OK)
    {
        ret=ISO14443A_WriteBlock(blk_idx, szdata);
    }

    if (ret==MI_OK)
    {
        //�����������֤���´�ֱ�ӽ��ж�д����
        g_octcurrinf.currsector = currsector;
    }

    return ret;


}

//ndest��nsrc������һ����������
char octm1_copy_purse(unsigned char ndest, unsigned char nsrc)
{

    char ret = -1;
    unsigned char currsector = (ndest >> 2);

    if (currsector != g_octcurrinf.currsector)
    {
        ret = ISO14443A_Authentication(PICC_AUTHENT1A, g_octcurrinf.physicalnumber, g_octkeyinf.keya, ndest);
    }

    if (ret==MI_OK)
    {
        ret=ISO14443A_Restore(nsrc);
    }

    if (ret==MI_OK)
    {

        ret = ISO14443A_Transfer(ndest); //Ǯ����������
    }

    if (ret==MI_OK)
    {
        //�����������֤���´�ֱ�ӽ��ж�д����
        g_octcurrinf.currsector = currsector;
    }

    return ret;

}

char octm1_write_purse(unsigned char blk_idx, unsigned char optmode, unsigned long lvalue )
{

    char ret=0;
    unsigned char tmpbuf[16];
    unsigned char currsector = (blk_idx >> 2);

    if (currsector != g_octcurrinf.currsector)
    {
        ret = ISO14443A_Authentication(PICC_AUTHENT1A, g_octcurrinf.physicalnumber, g_octkeyinf.keya, blk_idx);
    }

    if (ret==MI_OK)
    {
        ret = ISO14443A_Restore(blk_idx); //Ǯ����������
    }

    if (ret==MI_OK)
    {
        //tmpbuf[0] = (lvalue & 0x000000FF);
        //tmpbuf[1] = (lvalue & 0x0000FF00);
        //tmpbuf[2] = (lvalue & 0x00FF0000);
        //tmpbuf[3] = (lvalue & 0xFF000000);

		tmpbuf[0] = (lvalue & 0x000000FF);
		tmpbuf[1] = (lvalue & 0x0000FF00) >> 8;
		tmpbuf[2] = (lvalue & 0x00FF0000) >> 16;
		tmpbuf[3] = (lvalue & 0xFF000000) >> 24;

        ret = ISO14443A_Value(optmode, blk_idx, tmpbuf); //PICC_INCREMENT//PICC_DECREMENT
    }

    if (ret==MI_OK)
    {
        ret = ISO14443A_Transfer(blk_idx); //Ǯ��������ֵ
    }

    if (ret==MI_OK)
    {
        //�����������֤���´�ֱ�ӽ��ж�д����
        g_octcurrinf.currsector = currsector;
    }

    return ret;

}

//===========================================
//���ݿ�У�鴦����

//�ж����ݿ��Ƿ���Ч����Ч���ط�0ֵ
unsigned char octm1_verify_block(unsigned char *lpdata)
{
    unsigned char ret=1;
    if (crc_ck(lpdata,16)==0)
    {
        ret = 0;//=0��ȷ
    }

    return ret;
}



//�ж�Ǯ���Ƿ���Ч����Ч���ط�0ֵ
unsigned char octm1_verify_purse(OCT_BLK_PURSE *pblk)
{
    unsigned char i;
    unsigned char ret=0;

    for (i=0; i<4; i++)
    {
        // �ж�����Ǯ���Ϸ�
        if (pblk->buf[i] != 0xff - pblk->buf[i+4])
        {
            //�Ƿ�
            ret = 1;
            //break;
        }
        if (pblk->buf[i] != pblk->buf[i+8])
        {
            //�Ƿ�
            ret = 1;
            //break;
        }

    }
    if ((pblk->buf[12]+pblk->buf[13])!=(pblk->buf[14]+pblk->buf[15]))
        //if (memcmp(pblk->buf+12, "\x00\xFF\x00\xFF", 4))
    {
        //�Ƿ�
        ret = 1;
    }

    return ret;
}


//�з��������Ƿ���Ч����Ч���ط�0ֵ
unsigned char octm1_verify_issue(OCT_BLK_ISSUESUE *pblk)
{
    unsigned char ret=1;
    if (crc_ck(pblk->buf,16)==0)
    {
        ret = 0;//=0��ȷ
    }
    return ret;
}

//�з��������Ƿ���Ч����Ч���ط�0ֵ
unsigned char octm1_verify_addtionalissue(OCT_BLK_ADDTIONALISSUE *pblk)
{
    unsigned char ret=1;
    if (crc_ck(pblk->buf,16)==0)
    {
        ret = 0;//=0��ȷ
    }
    return ret;
}



//�й�����Ϣ���Ƿ���Ч����Ч���ط�0ֵ
unsigned char octm1_verify_public(OCT_BLK_PUBLIC *pblk)
{
    unsigned char ret=1;
    if (crc_ck(pblk->buf,16)==0)
    {
        ret = 0;//=0��ȷ
    }
    return ret;
}


//�й�����Ϣ���Ƿ���Ч����Ч���ط�0ֵ
unsigned char octm1_verify_history(OCT_BLK_HISTORY *pblk)
{
    unsigned char ret=1;
    if (crc_ck(pblk->buf,16)==0)
    {
        ret = 0;//=0��ȷ
    }
    return ret;
}

//�е�����Ϣ���Ƿ���Ч����Ч���ط�0ֵ
unsigned char octm1_verify_metro(OCT_BLK_METRO *pblk)
{
    unsigned char ret=1;
    if (crc16_ck(pblk->buf,16)==0)
    {
        ret = 0;//=0��ȷ
    }
    return ret;
}


//============����У�鲿�ֽ���==================
//===================================================



//============================================================================
//�Թ�����Ϣ���ж�Ӧ��Ǯ�����������ۼƣ�=0Ϊ����Ǯ����=1Ϊר��Ǯ��
//============================================================================

void octm1_inc_public_tr_count(unsigned char purse_type, PTOCT_BLK_PUBLIC_DETAIL pblk)
{
    unsigned short wtmp;
    if (purse_type==0)
    {
        wtmp = pblk->common_count[0];
        wtmp <<= 8;
        wtmp += pblk->common_count[1];

        wtmp++;

        pblk->common_count[0] = ((wtmp>>8) & 0x00FF);
        pblk->common_count[1] = (wtmp& 0x00FF);
    }
    else
    {
        wtmp = pblk->other_count[0];
        wtmp <<= 8;
        wtmp += pblk->other_count[1];

        wtmp++;

        pblk->other_count[0] = ((wtmp>>8) & 0x00FF);
        pblk->other_count[1] = (wtmp& 0x00FF);

    }
}


//==========================================================================================
//�� �� ��: octm1_metro_recover
//��    ��: ������Ϣ���ָ����̣�Ƕ����Ǯ���ָ�������
//��ڲ���:
//          direction:�ָ�����, ֻ���������������Ҷ���Ч������Ż��õ�,һ��һ���������öԵĸ��Ǵ��
//                    0=�Զ��ָ�����
//                    1=���ָ�, ���������Ǹ���
//                    2=��ǰ�ָ�, �ø�����������
//            poctinf:�����е�����Ϣ������ݽṹ
//���ڲ���:
//             ��
//�� �� ֵ:
//          0=�ָ��ɹ�
//==========================================================================================

unsigned char octm1_metro_recover(unsigned char direction, PTOCT_INF poctinf, char metro_recover_flag)
{
    unsigned char metro_org_invalid = 0; //Ǯ�������Ƿ���Ч, ��0ֵΪ��Ч
    unsigned char metro_bak_invalid = 0; //Ǯ�������Ƿ���Ч, ��0ֵΪ��Ч
    unsigned char tmpbuf[16];
    unsigned char ret=0;

    do
    {
		if (metro_recover_flag != 1)
			break;

        if (octm1_verify_metro(&poctinf->blk_metro_org))
        {
            metro_org_invalid = 1;
        }

        if (octm1_verify_metro(&poctinf->blk_metro_bak))
        {
            metro_bak_invalid = 1;
        }


        if ((metro_org_invalid)&&(metro_bak_invalid))
        {
            //������Ϣ����������������ȫ0״̬
            memset(tmpbuf, 0x00, sizeof(tmpbuf));
            ret = octm1_writeblock(OCT_BLK_METRO_ORG_IDX, tmpbuf);

            if (ret)
            {
                ret = 0xA8;
                break;
            }

            ret = octm1_writeblock(OCT_BLK_METRO_BAK_IDX, tmpbuf);
            if (ret)
            {
                ret = 0xA8;
                break;
            }

            memcpy(poctinf->blk_metro_org.buf, tmpbuf, 16);
            memcpy(poctinf->blk_metro_bak.buf, tmpbuf, 16);


        }
        else if (metro_org_invalid)
        {
            //������Ч, �ø�����������
            memcpy(poctinf->blk_metro_org.buf, poctinf->blk_metro_bak.buf, 16);
            ret = octm1_writeblock(OCT_BLK_METRO_ORG_IDX, poctinf->blk_metro_org.buf);
            if (ret)
            {
                ret = 0xA8;
                break;
            }
        }
        else if (metro_bak_invalid)
        {
            //������Ч, ���������Ǹ���
            memcpy(poctinf->blk_metro_bak.buf, poctinf->blk_metro_org.buf, 16);
            ret = octm1_writeblock(OCT_BLK_METRO_BAK_IDX, poctinf->blk_metro_bak.buf);
            if (ret)
            {
                ret = 0xA8;
                break;
            }

        }
        else
        {
            //����������Ч,ֻ�������������ȵ����
            if (memcmp(poctinf->blk_metro_bak.buf, poctinf->blk_metro_bak.buf, 16))
            {
                if ((direction==0) || (direction==1))
                {
                    //���ָ�, �������Ǹ���
                    memcpy(poctinf->blk_metro_bak.buf, poctinf->blk_metro_org.buf, 16);
                    ret = octm1_writeblock(OCT_BLK_METRO_BAK_IDX, poctinf->blk_metro_bak.buf);
                    if (ret)
                    {
                        ret = 0xA8;
                        break;
                    }
                }
                else
                {
                    //��ǰ�ָ�, ������������
                    memcpy(poctinf->blk_metro_org.buf, poctinf->blk_metro_bak.buf, 16);
                    ret = octm1_writeblock(OCT_BLK_METRO_ORG_IDX, poctinf->blk_metro_org.buf);
                    if (ret)
                    {
                        ret = 0xA8;
                        break;
                    }
                }
            }
        }

        ret = 0x00;
    }
    while (0);

    return(ret);

}

//==============================================================
//ר��Ǯ���ָ�
unsigned char octm1_other_purse_recover_a0(PTOCT_INF poctinf)
{
    //����B0
// dbg_formatvar("PROCESS==B0\n");
    memcpy(poctinf->blk_public_bak.buf, poctinf->blk_public_org.buf, 16);
    if (octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX, poctinf->blk_public_bak.buf))
    {
        return 0xA8;
    }


    return 0;

}


unsigned char octm1_other_purse_recover_a1(PTOCT_INF poctinf)
{
    //����B1
// dbg_formatvar("PROCESS==B1\n");
    if (poctinf->lpurse_other_org != poctinf->lpurse_other_bak)
    {
        if (octm1_copy_purse(OCT_BLK_PURSE_OTHER_BAK_IDX, OCT_BLK_PURSE_OTHER_ORG_IDX))
        {
            return 0xA8;
        }

        poctinf->lpurse_other_bak = poctinf->lpurse_other_org;
        memcpy(poctinf->blk_purse_other_bak.buf, poctinf->blk_purse_other_org.buf, 16);
    }

    memcpy(poctinf->blk_public_bak.buf, poctinf->blk_public_org.buf, 16);
    if (octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX , poctinf->blk_public_bak.buf))
    {
        return 0xA8;
    }

    return 0;

}

unsigned char octm1_other_purse_recover_a2(PTOCT_INF poctinf)
{
    //����B2
// dbg_formatvar("PROCESS==B2\n");
// dbg_formatvar("%ld, %ld\n",poctinf->lpurse_other_org,poctinf->lpurse_other_bak);

    if (poctinf->lpurse_other_org != poctinf->lpurse_other_bak)
    {
//  dbg_formatvar("PROCESS==B21\n");
        //Ǯ��������������, ��ʾ�۷ѳɹ�, ���ָ�
        //Ǯ�������Ǹ�����������Ϣ���������Ǹ���
        //����ָ�����ۼӹ�������ֻ�轫���̱�ʶ��Ϊ2  ����
        poctinf->blk_public_org.detail.proc_flag = 0x14;
        memcpy(poctinf->blk_public_bak.buf, poctinf->blk_public_org.buf, 16);

        if (octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX , poctinf->blk_public_org.buf))
        {
            return 0xA8;
        }


        if (octm1_copy_purse(OCT_BLK_PURSE_OTHER_BAK_IDX, OCT_BLK_PURSE_OTHER_ORG_IDX))
        {
            return 0xA8;
        }

        poctinf->lpurse_common_bak = poctinf->lpurse_common_org;
        memcpy(poctinf->blk_purse_common_bak.buf, poctinf->blk_purse_common_org.buf, 16);

        if (octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX , poctinf->blk_public_bak.buf))
        {
            return 0xA8;
        }

    }
    else
    {
        //Ǯ�������������, ��ʾ�۷�ʧ��, ��ǰ�ָ�
        //������Ϣ��������������
        memcpy(poctinf->blk_public_org.buf, poctinf->blk_public_bak.buf, 16);
        if (octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX , poctinf->blk_public_org.buf))
        {
            return 0xA8;
        }

    }


    return 0;

}

unsigned char octm1_other_purse_recover_a3(PTOCT_INF poctinf)
{
    //����B3 , ��b0�з������
// dbg_formatvar("PROCESS==B3\n");

    if ((poctinf->lst_tr_type==0x01) || (poctinf->lst_tr_type==0x03))
    {
        //�ϴ�Ϊ����Ǯ��
        if (poctinf->lpurse_common_org != poctinf->lpurse_common_bak)
        {
            poctinf->blk_public_bak.detail.proc_flag = 0x13;
            octm1_inc_public_tr_count(0,&poctinf->blk_public_bak.detail);
            memcpy(poctinf->blk_public_org.buf, poctinf->blk_public_bak.buf, 16);
            if (octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX , poctinf->blk_public_org.buf))
            {
                return 0xA8;
            }

            if (octm1_copy_purse(OCT_BLK_PURSE_BAK_IDX, OCT_BLK_PURSE_ORG_IDX))
            {
                return 0xA8;
            }

            if (octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX , poctinf->blk_public_bak.buf))
            {
                return 0xA8;
            }

            poctinf->lpurse_common_bak = poctinf->lpurse_common_org;
            memcpy(poctinf->blk_purse_common_bak.buf, poctinf->blk_purse_common_org.buf, 16);
        }
        else
        {
            memcpy(poctinf->blk_public_org.buf, poctinf->blk_public_bak.buf, 16);
            if (octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX , poctinf->blk_public_org.buf))
            {
                return 0xA8;
            }

        }
    }
    else
    {
        //�ϴ�Ϊר��Ǯ��
        if (poctinf->lpurse_other_org != poctinf->lpurse_other_bak)
        {
            poctinf->blk_public_bak.detail.proc_flag = 0x14;
            octm1_inc_public_tr_count(1,&poctinf->blk_public_bak.detail);
            memcpy(poctinf->blk_public_org.buf, poctinf->blk_public_bak.buf, 16);
            if (octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX , poctinf->blk_public_org.buf))
            {
                return 0xA8;
            }

            if (octm1_copy_purse(OCT_BLK_PURSE_OTHER_BAK_IDX, OCT_BLK_PURSE_OTHER_ORG_IDX))
            {
                return 0xA8;
            }
            poctinf->lpurse_other_bak = poctinf->lpurse_other_org;
            memcpy(poctinf->blk_purse_other_bak.buf, poctinf->blk_purse_other_org.buf, 16);

            if (octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX , poctinf->blk_public_bak.buf))
            {
                return 0xA8;
            }

        }
        else
        {
            memcpy(poctinf->blk_public_org.buf, poctinf->blk_public_bak.buf, 16);
            if (octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX , poctinf->blk_public_org.buf))
            {
                return 0xA8;
            }

        }
    }


    return 0;

}


unsigned char octm1_other_purse_recover_a4(PTOCT_INF poctinf)
{
    //����B4,ר��Ǯ���������
// dbg_formatvar("PROCESS==B4\n");
    if (poctinf->lpurse_common_org != poctinf->lpurse_common_bak)
    {
        if (octm1_copy_purse(OCT_BLK_PURSE_BAK_IDX, OCT_BLK_PURSE_ORG_IDX))
        {
            return 0xA8;
        }

        poctinf->lpurse_common_bak = poctinf->lpurse_common_org;
        memcpy(poctinf->blk_purse_common_bak.buf, poctinf->blk_purse_common_org.buf, 16);

    }
    memcpy(poctinf->blk_public_bak.buf, poctinf->blk_public_org.buf, 16);
    if (octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX , poctinf->blk_public_bak.buf))
    {
        return 0xA8;
    }


    return 0;

}

unsigned char octm1_other_purse_recover_a5(PTOCT_INF poctinf)
{
    //����B5
// dbg_formatvar("PROCESS==B5\n");
// printf("%ld, %ld\n",poctinf->lpurse_other_org,poctinf->lpurse_other_bak);
    if (poctinf->lpurse_common_org != poctinf->lpurse_common_bak)
    {
//   dbg_formatvar("PROCESS==B51\n");
        //Ǯ���������,���ָ�
        poctinf->blk_public_org.detail.proc_flag = 0x13;
        memcpy(poctinf->blk_public_bak.buf, poctinf->blk_public_org.buf, 16);
        if (octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX , poctinf->blk_public_org.buf))

            if (octm1_copy_purse(OCT_BLK_PURSE_BAK_IDX, OCT_BLK_PURSE_ORG_IDX))
            {
                return 0xA8;
            }

        if (octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX , poctinf->blk_public_bak.buf))

            poctinf->lpurse_common_bak = poctinf->lpurse_common_org;
        memcpy(poctinf->blk_purse_common_bak.buf, poctinf->blk_purse_common_org.buf, 16);

    }
    else
    {
        //����Ǯ���۷�δ���,��ǰ�ָ�
// dbg_formatvar("PROCESS==B52\n");
        memcpy(poctinf->blk_public_org.buf, poctinf->blk_public_bak.buf, 16);
        if (octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX , poctinf->blk_public_bak.buf))
        {
            return 0xA8;
        }

    }


    return 0;

}

unsigned char octm1_other_purse_recover(PTOCT_INF poctinf, char metro_recover_flag)
{
    unsigned char status;

    unsigned char purse_org_invalid = 0; //Ǯ�������Ƿ���Ч, ��0ֵΪ��Ч
    unsigned char purse_bak_invalid = 0; //Ǯ�������Ƿ���Ч, ��0ֵΪ��Ч

    unsigned char purse_other_org_invalid = 0; //Ǯ�������Ƿ���Ч, ��0ֵΪ��Ч
    unsigned char purse_other_bak_invalid = 0; //Ǯ�������Ƿ���Ч, ��0ֵΪ��Ч


    unsigned char public_org_invalid = 0;//������Ϣ���Ƿ���Ч,��0ֵΪ��Ч
    unsigned char public_bak_invalid = 0;//������Ϣ���Ƿ���Ч,��0ֵΪ��Ч



    if (octm1_verify_purse(&poctinf->blk_purse_common_org))
    {
        purse_org_invalid = 1;
    };

    if (octm1_verify_purse(&poctinf->blk_purse_common_bak))
    {
        purse_bak_invalid = 1;
    };

    if (octm1_verify_purse(&poctinf->blk_purse_other_org))
    {
        purse_other_org_invalid = 1;
    }

    if (octm1_verify_purse(&poctinf->blk_purse_other_bak))
    {
        purse_other_bak_invalid = 1;
    };

    if (octm1_verify_public(&poctinf->blk_public_org))
    {
        public_org_invalid = 1;
    };

    if (octm1_verify_public(&poctinf->blk_public_bak))
    {
        public_bak_invalid = 1;
    };

    if (purse_org_invalid)
    {
        poctinf->lpurse_common_org = 0;
    }
    else
    {
        poctinf->lpurse_common_org = poctinf->blk_purse_common_org.buf[3];
        poctinf->lpurse_common_org <<= 8;
        poctinf->lpurse_common_org += poctinf->blk_purse_common_org.buf[2];
        poctinf->lpurse_common_org <<= 8;
        poctinf->lpurse_common_org += poctinf->blk_purse_common_org.buf[1];
        poctinf->lpurse_common_org <<= 8;
        poctinf->lpurse_common_org += poctinf->blk_purse_common_org.buf[0];
    }


    if (purse_bak_invalid)
    {
        poctinf->lpurse_common_bak = 0;
    }
    else
    {
        poctinf->lpurse_common_bak = poctinf->blk_purse_common_bak.buf[3];
        poctinf->lpurse_common_bak <<= 8;
        poctinf->lpurse_common_bak += poctinf->blk_purse_common_bak.buf[2];
        poctinf->lpurse_common_bak <<= 8;
        poctinf->lpurse_common_bak += poctinf->blk_purse_common_bak.buf[1];
        poctinf->lpurse_common_bak <<= 8;
        poctinf->lpurse_common_bak += poctinf->blk_purse_common_bak.buf[0];
    }

    if (purse_other_org_invalid)
    {
        poctinf->lpurse_other_org = 0;
    }
    else
    {
        poctinf->lpurse_other_org = poctinf->blk_purse_other_org.buf[3];
        poctinf->lpurse_other_org <<= 8;
        poctinf->lpurse_other_org += poctinf->blk_purse_other_org.buf[2];
        poctinf->lpurse_other_org <<= 8;
        poctinf->lpurse_other_org += poctinf->blk_purse_other_org.buf[1];
        poctinf->lpurse_other_org <<= 8;
        poctinf->lpurse_other_org += poctinf->blk_purse_other_org.buf[0];
    }


    if (purse_other_bak_invalid)
    {
        poctinf->lpurse_other_bak = 0;
    }
    else
    {
        poctinf->lpurse_other_bak = poctinf->blk_purse_other_bak.buf[3];
        poctinf->lpurse_other_bak <<= 8;
        poctinf->lpurse_other_bak += poctinf->blk_purse_other_bak.buf[2];
        poctinf->lpurse_other_bak <<= 8;
        poctinf->lpurse_other_bak += poctinf->blk_purse_other_bak.buf[1];
        poctinf->lpurse_other_bak <<= 8;
        poctinf->lpurse_other_bak += poctinf->blk_purse_other_bak.buf[0];
    }

    //Ǯ������������򹫹���Ϣ�������򱨴�
    if ((purse_other_org_invalid && purse_other_bak_invalid))// || (public_org_invalid && public_bak_invalid))
    {
//  dbg_formatvar("E5 = %02X %02X %02X %02X\n",purse_org_invalid,purse_bak_invalid,public_org_invalid,public_bak_invalid);
        return 0xE5; //Ʊ�����ݴ��󣬲��ָܻ�
    }

    //Ǯ������������򹫹���Ϣ�������򱨴�
    if (public_org_invalid && public_bak_invalid)
    {
		// ���ۺ���������ָ���ע��ifģ�飬20140820��wxf
        //if (memcmp(poctinf->blk_public_org.buf, "\x00\x00\x00\x00", 4))
        //{
        //    return 0xE5; //Ʊ�����ݴ��󣬲��ָܻ�
        //}
        //else
        {
            //�޸�����Ϊ������Ϣ�п��ܱ�����Ǯ���ṹ
            public_org_invalid = 0;
            public_bak_invalid = 0;
            memset(poctinf->blk_public_org.buf, 0x00, 16);
            memset(poctinf->blk_public_bak.buf, 0x00, 16);
            crc_cu(poctinf->blk_public_org.buf, 16);
            crc_cu(poctinf->blk_public_bak.buf, 16);
        }
    }


    //���ר��Ǯ����һ��һ���򽫴�Ľ��лָ������ں����������ж�һ������
    if (purse_other_org_invalid)
    {
		if (octm1_metro_recover(2, poctinf, metro_recover_flag))
		{
			return 0xA8;
		}

        //Ǯ������������
        if (octm1_copy_purse(OCT_BLK_PURSE_OTHER_ORG_IDX, OCT_BLK_PURSE_OTHER_BAK_IDX))
        {
            return 0xA8;
        }
        poctinf->lpurse_other_org = poctinf->lpurse_other_bak;
        memcpy(poctinf->blk_purse_other_org.buf, poctinf->blk_purse_other_bak.buf, 16);

    }
    else if (purse_other_bak_invalid)
    {

		if (octm1_metro_recover(1, poctinf, metro_recover_flag))
		{
			return 0xA8;
		}
        //Ǯ������������
        if (octm1_copy_purse(OCT_BLK_PURSE_OTHER_BAK_IDX, OCT_BLK_PURSE_OTHER_ORG_IDX))
        {
            return 0xA8;
        }
        poctinf->lpurse_other_bak = poctinf->lpurse_other_org;
        memcpy(poctinf->blk_purse_other_bak.buf, poctinf->blk_purse_other_org.buf, 16);

    }
    else
    {
        //Ǯ�����������Ϸ������ȵ����ֻ��Ҫ�����������
        if (poctinf->lpurse_other_org != poctinf->lpurse_other_bak)
        {
            if (memcmp(g_octcurrinf.logiccardnumber, g_m1recover.Logicno, 4)==0)
            {


				if (g_m1recover.npursetype)
				{
					//�ϴ��ǹ���Ǯ������
	                if (poctinf->lpurse_common_org == g_m1recover.balance_after)
	                {
						if (octm1_metro_recover(2, poctinf, metro_recover_flag))
						{
							return 0xA8;
						}

	                    //�ϴη������������ı䣬�����ָ���ԭʼ״̬
	                    if (octm1_copy_purse(OCT_BLK_PURSE_OTHER_ORG_IDX, OCT_BLK_PURSE_OTHER_BAK_IDX))
	                    {
	                        return 0xA8;
	                    }
	                    poctinf->lpurse_other_org = poctinf->lpurse_other_bak;
	                    memcpy(poctinf->blk_purse_other_org.buf, poctinf->blk_purse_other_bak.buf, 16);
	                    memset(&g_m1recover, 0x00, sizeof(g_m1recover));

	                }
                }

            }

        }
    }



    //���Ǯ����һ��һ���򽫴�Ľ��лָ������ں����������ж�һ������
    if (purse_org_invalid)
    {

        //Ǯ������������
        if (octm1_copy_purse(OCT_BLK_PURSE_ORG_IDX, OCT_BLK_PURSE_BAK_IDX))
        {
            return 0xA8;
        }
        poctinf->lpurse_common_org = poctinf->lpurse_common_bak;
        memcpy(poctinf->blk_purse_common_org.buf, poctinf->blk_purse_common_bak.buf, 16);

    }
    else if (purse_bak_invalid)
    {

        //Ǯ������������
        if (octm1_copy_purse(OCT_BLK_PURSE_BAK_IDX, OCT_BLK_PURSE_ORG_IDX))
        {
            return 0xA8;
        }
        poctinf->lpurse_common_bak = poctinf->lpurse_common_org;
        memcpy(poctinf->blk_purse_common_bak.buf, poctinf->blk_purse_common_org.buf, 16);

    }
    else
    {
        //Ǯ�����������Ϸ������ȵ����ֻ��Ҫ�����������
        if (poctinf->lpurse_common_org != poctinf->lpurse_common_bak)
        {
            if (memcmp(g_octcurrinf.logiccardnumber, g_m1recover.Logicno, 4)==0)
            {


				if (g_m1recover.npursetype==0)
				{
					//�ϴ��ǹ���Ǯ������
	                if (poctinf->lpurse_common_org == g_m1recover.balance_after)
	                {
						if (octm1_metro_recover(2, poctinf, metro_recover_flag))
						{
							return 0xA8;
						}

	                    //�ϴη������������ı䣬�����ָ���ԭʼ״̬
	                    if (octm1_copy_purse(OCT_BLK_PURSE_ORG_IDX, OCT_BLK_PURSE_BAK_IDX))
	                    {
	                        return 0xA8;
	                    }
	                    poctinf->lpurse_common_org = poctinf->lpurse_common_bak;
	                    memcpy(poctinf->blk_purse_common_org.buf, poctinf->blk_purse_common_bak.buf, 16);
	                    memset(&g_m1recover, 0x00, sizeof(g_m1recover));

	                }
                }

            }

        }
    }


    if (public_org_invalid)
    {
        //������Ϣ��������Ч
        //�����ֿ��ܣ�
        //1(��ǰ�ָ�)�ǻ�û����Ǯ��ʱд��, ��ʱǮ������������ȵ�,ֻ�轫������Ϣ���������Ƶ�����
        //2(���ָ�)�ǲ�����Ǯ��������д��, ��ʱǮ�������Ѹ���, �轫������Ϣ�������ۼƺ��Ƶ�������д��
        status = octm1_other_purse_recover_a3(poctinf);
        if (status) return(status);
    }
    else if (public_bak_invalid)
    {
        //����A0
        status = octm1_other_purse_recover_a0(poctinf);
        if (status) return(status);
    }
    else
    {
        //������Ϣ����������ȷ�����������
        if (memcmp(poctinf->blk_public_org.buf, poctinf->blk_public_bak.buf, 16))
        {
            // 1������Ϣ��������
            //�жϹ�����Ϣ���������̱�ʶ=1 ��=2
            if (poctinf->blk_public_org.detail.proc_flag == 0x11)
            {
                //����B2
                //dbg_formatvar("B2\n");
                status = octm1_other_purse_recover_a5(poctinf);
                if (status) return(status);
            }
            else if (poctinf->blk_public_org.detail.proc_flag == 0x13)
            {
                //����B1
                //dbg_formatvar("B1\n");
                status = octm1_other_purse_recover_a4(poctinf);
                if (status) return(status);

            }
            else if (poctinf->blk_public_org.detail.proc_flag == 0x14)
            {
                //����B4
                //dbg_formatvar("B4\n");
                status = octm1_other_purse_recover_a1(poctinf);
                if (status) return(status);

            }
            else if (poctinf->blk_public_org.detail.proc_flag == 0x12)
            {
                //����B5
                //dbg_formatvar("B5\n");
                status = octm1_other_purse_recover_a2(poctinf);
                if (status) return(status);

            }

        }
        else
        {
            // 2������Ϣ���������
            if (poctinf->lpurse_other_org != poctinf->lpurse_other_bak)
            {
                if (octm1_copy_purse(OCT_BLK_PURSE_OTHER_BAK_IDX, OCT_BLK_PURSE_OTHER_ORG_IDX))
                {
                    return 0xA8;
                }

                poctinf->lpurse_other_bak = poctinf->lpurse_other_org;
                memcpy(poctinf->blk_purse_other_bak.buf, poctinf->blk_purse_other_org.buf, 16);
            }

			if (poctinf->lpurse_common_org != poctinf->lpurse_common_bak)
			{
				if (octm1_copy_purse(OCT_BLK_PURSE_BAK_IDX, OCT_BLK_PURSE_ORG_IDX))
				{
					return 0xA8;
				}

				poctinf->lpurse_common_bak = poctinf->lpurse_common_org;
				memcpy(poctinf->blk_purse_common_bak.buf, poctinf->blk_purse_common_org.buf, 16);
			}


        }

    }

	octm1_metro_recover(0, poctinf, metro_recover_flag);



    return 0x00;

}


//==============================================================
unsigned char octm1_common_purse_recover_b0(PTOCT_INF poctinf)
{
    //����B0
// dbg_formatvar("PROCESS==B0\n");
    memcpy(poctinf->blk_public_bak.buf, poctinf->blk_public_org.buf, 16);
    if (octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX, poctinf->blk_public_bak.buf))
    {
        return 0xA8;
    }


    return 0;

}


unsigned char octm1_common_purse_recover_b1(PTOCT_INF poctinf)
{
    //����B1
// dbg_formatvar("PROCESS==B1\n");
    if (poctinf->lpurse_common_org != poctinf->lpurse_common_bak)
    {
        if (octm1_copy_purse(OCT_BLK_PURSE_BAK_IDX, OCT_BLK_PURSE_ORG_IDX))
        {
            return 0xA8;
        }

        poctinf->lpurse_common_bak = poctinf->lpurse_common_org;
        memcpy(poctinf->blk_purse_common_bak.buf, poctinf->blk_purse_common_org.buf, 16);
    }

    memcpy(poctinf->blk_public_bak.buf, poctinf->blk_public_org.buf, 16);
    if (octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX , poctinf->blk_public_bak.buf))
    {
        return 0xA8;
    }

    return 0;

}

unsigned char octm1_common_purse_recover_b2(PTOCT_INF poctinf)
{
    //����B2
// dbg_formatvar("PROCESS==B2\n");
// dbg_formatvar("%ld, %ld\n",poctinf->lpurse_other_org,poctinf->lpurse_other_bak);

    if (poctinf->lpurse_common_org != poctinf->lpurse_common_bak)
    {
//  dbg_formatvar("PROCESS==B21\n");
        //Ǯ��������������, ��ʾ�۷ѳɹ�, ���ָ�
        //Ǯ�������Ǹ�����������Ϣ���������Ǹ���
        //����ָ�����ۼӹ�������ֻ�轫���̱�ʶ��Ϊ2  ����
        poctinf->blk_public_org.detail.proc_flag = 0x13;
        memcpy(poctinf->blk_public_bak.buf, poctinf->blk_public_org.buf, 16);

        if (octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX , poctinf->blk_public_org.buf))
        {
            return 0xA8;
        }


        if (octm1_copy_purse(OCT_BLK_PURSE_BAK_IDX, OCT_BLK_PURSE_ORG_IDX))
        {
            return 0xA8;
        }

        poctinf->lpurse_common_bak = poctinf->lpurse_common_org;
        memcpy(poctinf->blk_purse_common_bak.buf, poctinf->blk_purse_common_org.buf, 16);

        if (octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX , poctinf->blk_public_bak.buf))
        {
            return 0xA8;
        }

    }
    else
    {
        //Ǯ�������������, ��ʾ�۷�ʧ��, ��ǰ�ָ�
        //������Ϣ��������������
        memcpy(poctinf->blk_public_org.buf, poctinf->blk_public_bak.buf, 16);
        if (octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX , poctinf->blk_public_org.buf))
        {
            return 0xA8;
        }

    }


    return 0;

}

unsigned char octm1_common_purse_recover_b3(PTOCT_INF poctinf)
{
    //����B3 , ��b0�з������
// dbg_formatvar("PROCESS==B3\n");

    if ((poctinf->lst_tr_type==0x01) || (poctinf->lst_tr_type==0x03))
    {
        //�ϴ�Ϊ����Ǯ��
        if (poctinf->lpurse_common_org != poctinf->lpurse_common_bak)
        {
            poctinf->blk_public_bak.detail.proc_flag = 0x13;
            octm1_inc_public_tr_count(0,&poctinf->blk_public_bak.detail);
            memcpy(poctinf->blk_public_org.buf, poctinf->blk_public_bak.buf, 16);
            if (octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX , poctinf->blk_public_org.buf))
            {
                return 0xA8;
            }

            if (octm1_copy_purse(OCT_BLK_PURSE_BAK_IDX, OCT_BLK_PURSE_ORG_IDX))
            {
                return 0xA8;
            }

            if (octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX , poctinf->blk_public_bak.buf))
            {
                return 0xA8;
            }

            poctinf->lpurse_common_bak = poctinf->lpurse_common_org;
            memcpy(poctinf->blk_purse_common_bak.buf, poctinf->blk_purse_common_org.buf, 16);
        }
        else
        {
            memcpy(poctinf->blk_public_org.buf, poctinf->blk_public_bak.buf, 16);
            if (octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX , poctinf->blk_public_org.buf))
            {
                return 0xA8;
            }

        }
    }
    else
    {
        //�ϴ�Ϊר��Ǯ��
        if (poctinf->lpurse_other_org != poctinf->lpurse_other_bak)
        {
            poctinf->blk_public_bak.detail.proc_flag = 0x14;
            octm1_inc_public_tr_count(1,&poctinf->blk_public_bak.detail);
            memcpy(poctinf->blk_public_org.buf, poctinf->blk_public_bak.buf, 16);
            if (octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX , poctinf->blk_public_org.buf))
            {
                return 0xA8;
            }

            if (octm1_copy_purse(OCT_BLK_PURSE_OTHER_BAK_IDX, OCT_BLK_PURSE_OTHER_ORG_IDX))
            {
                return 0xA8;
            }
            poctinf->lpurse_other_bak = poctinf->lpurse_other_org;
            memcpy(poctinf->blk_purse_other_bak.buf, poctinf->blk_purse_other_org.buf, 16);

            if (octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX , poctinf->blk_public_bak.buf))
            {
                return 0xA8;
            }

        }
        else
        {
            memcpy(poctinf->blk_public_org.buf, poctinf->blk_public_bak.buf, 16);
            if (octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX , poctinf->blk_public_org.buf))
            {
                return 0xA8;
            }

        }
    }


    return 0;

}


unsigned char octm1_common_purse_recover_b4(PTOCT_INF poctinf)
{
    //����B4,ר��Ǯ���������
// dbg_formatvar("PROCESS==B4\n");
    if (poctinf->lpurse_other_org != poctinf->lpurse_other_bak)
    {
        if (octm1_copy_purse(OCT_BLK_PURSE_OTHER_BAK_IDX, OCT_BLK_PURSE_OTHER_ORG_IDX))
        {
            return 0xA8;
        }

        poctinf->lpurse_other_bak = poctinf->lpurse_other_org;
        memcpy(poctinf->blk_purse_other_bak.buf, poctinf->blk_purse_other_org.buf, 16);

    }
    memcpy(poctinf->blk_public_bak.buf, poctinf->blk_public_org.buf, 16);
    if (octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX , poctinf->blk_public_bak.buf))
    {
        return 0xA8;
    }


    return 0;

}

unsigned char octm1_common_purse_recover_b5(PTOCT_INF poctinf)
{
    //����B5
// dbg_formatvar("PROCESS==B5\n");
// printf("%ld, %ld\n",poctinf->lpurse_other_org,poctinf->lpurse_other_bak);
    if (poctinf->lpurse_other_org != poctinf->lpurse_other_bak)
    {
//   dbg_formatvar("PROCESS==B51\n");
        //Ǯ���������,���ָ�
        poctinf->blk_public_org.detail.proc_flag = 0x14;
        memcpy(poctinf->blk_public_bak.buf, poctinf->blk_public_org.buf, 16);
        if (octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX , poctinf->blk_public_org.buf))

            if (octm1_copy_purse(OCT_BLK_PURSE_OTHER_BAK_IDX, OCT_BLK_PURSE_OTHER_ORG_IDX))
            {
                return 0xA8;
            }

        if (octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX , poctinf->blk_public_bak.buf))

            poctinf->lpurse_other_bak = poctinf->lpurse_other_org;
        memcpy(poctinf->blk_purse_other_bak.buf, poctinf->blk_purse_other_org.buf, 16);

    }
    else
    {
        //ר��Ǯ���۷�δ���,��ǰ�ָ�
// dbg_formatvar("PROCESS==B52\n");
        memcpy(poctinf->blk_public_org.buf, poctinf->blk_public_bak.buf, 16);
        if (octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX , poctinf->blk_public_bak.buf))
        {
            return 0xA8;
        }



    }


    return 0;

}

unsigned char octm1_common_purse_recover(PTOCT_INF poctinf, char metro_recover_flag)
{
    unsigned char status;

    unsigned char purse_org_invalid = 0; //Ǯ�������Ƿ���Ч, ��0ֵΪ��Ч
    unsigned char purse_bak_invalid = 0; //Ǯ�������Ƿ���Ч, ��0ֵΪ��Ч

    unsigned char purse_other_org_invalid = 0; //Ǯ�������Ƿ���Ч, ��0ֵΪ��Ч
    unsigned char purse_other_bak_invalid = 0; //Ǯ�������Ƿ���Ч, ��0ֵΪ��Ч


    unsigned char public_org_invalid = 0;//������Ϣ���Ƿ���Ч,��0ֵΪ��Ч
    unsigned char public_bak_invalid = 0;//������Ϣ���Ƿ���Ч,��0ֵΪ��Ч



    if (octm1_verify_purse(&poctinf->blk_purse_common_org))
    {
        purse_org_invalid = 1;
    };

    if (octm1_verify_purse(&poctinf->blk_purse_common_bak))
    {
        purse_bak_invalid = 1;
    };

    if (octm1_verify_purse(&poctinf->blk_purse_other_org))
    {
        purse_other_org_invalid = 1;
    }

    if (octm1_verify_purse(&poctinf->blk_purse_other_bak))
    {
        purse_other_bak_invalid = 1;
    };

    if (octm1_verify_public(&poctinf->blk_public_org))
    {
        public_org_invalid = 1;
    };

    if (octm1_verify_public(&poctinf->blk_public_bak))
    {
        public_bak_invalid = 1;
    };

    if (purse_org_invalid)
    {
        poctinf->lpurse_common_org = 0;
    }
    else
    {
        poctinf->lpurse_common_org = poctinf->blk_purse_common_org.buf[3];
        poctinf->lpurse_common_org <<= 8;
        poctinf->lpurse_common_org += poctinf->blk_purse_common_org.buf[2];
        poctinf->lpurse_common_org <<= 8;
        poctinf->lpurse_common_org += poctinf->blk_purse_common_org.buf[1];
        poctinf->lpurse_common_org <<= 8;
        poctinf->lpurse_common_org += poctinf->blk_purse_common_org.buf[0];
    }


    if (purse_bak_invalid)
    {
        poctinf->lpurse_common_bak = 0;
    }
    else
    {
        poctinf->lpurse_common_bak = poctinf->blk_purse_common_bak.buf[3];
        poctinf->lpurse_common_bak <<= 8;
        poctinf->lpurse_common_bak += poctinf->blk_purse_common_bak.buf[2];
        poctinf->lpurse_common_bak <<= 8;
        poctinf->lpurse_common_bak += poctinf->blk_purse_common_bak.buf[1];
        poctinf->lpurse_common_bak <<= 8;
        poctinf->lpurse_common_bak += poctinf->blk_purse_common_bak.buf[0];
    }

    if (purse_other_org_invalid)
    {
        poctinf->lpurse_other_org = 0;
    }
    else
    {
        poctinf->lpurse_other_org = poctinf->blk_purse_other_org.buf[3];
        poctinf->lpurse_other_org <<= 8;
        poctinf->lpurse_other_org += poctinf->blk_purse_other_org.buf[2];
        poctinf->lpurse_other_org <<= 8;
        poctinf->lpurse_other_org += poctinf->blk_purse_other_org.buf[1];
        poctinf->lpurse_other_org <<= 8;
        poctinf->lpurse_other_org += poctinf->blk_purse_other_org.buf[0];
    }


    if (purse_other_bak_invalid)
    {
        poctinf->lpurse_other_bak = 0;
    }
    else
    {
        poctinf->lpurse_other_bak = poctinf->blk_purse_other_bak.buf[3];
        poctinf->lpurse_other_bak <<= 8;
        poctinf->lpurse_other_bak += poctinf->blk_purse_other_bak.buf[2];
        poctinf->lpurse_other_bak <<= 8;
        poctinf->lpurse_other_bak += poctinf->blk_purse_other_bak.buf[1];
        poctinf->lpurse_other_bak <<= 8;
        poctinf->lpurse_other_bak += poctinf->blk_purse_other_bak.buf[0];
    }

    //Ǯ������������򹫹���Ϣ�������򱨴�
    if ((purse_org_invalid && purse_bak_invalid))// || (public_org_invalid && public_bak_invalid))
    {
//  dbg_formatvar("E5 = %02X %02X %02X %02X\n",purse_org_invalid,purse_bak_invalid,public_org_invalid,public_bak_invalid);
        return 0xE5; //Ʊ�����ݴ��󣬲��ָܻ�
    }

    //Ǯ������������򹫹���Ϣ�������򱨴�
    if (public_org_invalid && public_bak_invalid)
    {
		// ���ۺ���������ָ���ע��ifģ�飬20140820��wxf
        //if (memcmp(poctinf->blk_public_org.buf, "\x00\x00\x00\x00", 4))
        //{
        //    return 0xE5; //Ʊ�����ݴ��󣬲��ָܻ�
        //}
        //else
        {
            //�޸�����Ϊ������Ϣ�п��ܱ�����Ǯ���ṹ
            public_org_invalid = 0;
            public_bak_invalid = 0;
            memset(poctinf->blk_public_org.buf, 0x00, 16);
            memset(poctinf->blk_public_bak.buf, 0x00, 16);
            crc_cu(poctinf->blk_public_org.buf, 16);
            crc_cu(poctinf->blk_public_bak.buf, 16);
        }
    }


    //���Ǯ����һ��һ���򽫴�Ľ��лָ������ں����������ж�һ������
    if (purse_org_invalid)
    {
		if (octm1_metro_recover(2, poctinf, metro_recover_flag))
		{
			return 0xA8;
		}

        //Ǯ������������
        if (octm1_copy_purse(OCT_BLK_PURSE_ORG_IDX, OCT_BLK_PURSE_BAK_IDX))
        {
            return 0xA8;
        }
        poctinf->lpurse_common_org = poctinf->lpurse_common_bak;
        memcpy(poctinf->blk_purse_common_org.buf, poctinf->blk_purse_common_bak.buf, 16);

    }
    else if (purse_bak_invalid)
    {
		if (octm1_metro_recover(1, poctinf, metro_recover_flag))
		{
			return 0xA8;
		}


        //Ǯ������������
        if (octm1_copy_purse(OCT_BLK_PURSE_BAK_IDX, OCT_BLK_PURSE_ORG_IDX))
        {
            return 0xA8;
        }
        poctinf->lpurse_common_bak = poctinf->lpurse_common_org;
        memcpy(poctinf->blk_purse_common_bak.buf, poctinf->blk_purse_common_org.buf, 16);

    }
    else
    {
        //Ǯ�����������Ϸ������ȵ����ֻ��Ҫ�����������
        if (poctinf->lpurse_common_org != poctinf->lpurse_common_bak)
        {
            if (memcmp(g_octcurrinf.logiccardnumber, g_m1recover.Logicno, 4)==0)
            {


				if (g_m1recover.npursetype==0)
				{
					//�ϴ��ǹ���Ǯ������
	                if (poctinf->lpurse_common_org == g_m1recover.balance_after)
	                {
						if (octm1_metro_recover(2, poctinf, metro_recover_flag))
						{
							return 0xA8;
						}

	                    //�ϴη������������ı䣬�����ָ���ԭʼ״̬
	                    if (octm1_copy_purse(OCT_BLK_PURSE_ORG_IDX, OCT_BLK_PURSE_BAK_IDX))
	                    {
	                        return 0xA8;
	                    }
	                    poctinf->lpurse_common_org = poctinf->lpurse_common_bak;
	                    memcpy(poctinf->blk_purse_common_org.buf, poctinf->blk_purse_common_bak.buf, 16);
	                    memset(&g_m1recover, 0x00, sizeof(g_m1recover));

	                }
                }

            }

        }
    }


    //���ר��Ǯ����һ��һ���򽫴�Ľ��лָ������ں����������ж�һ������
    if (purse_other_org_invalid)
    {
        //Ǯ������������
        if (octm1_copy_purse(OCT_BLK_PURSE_OTHER_ORG_IDX, OCT_BLK_PURSE_OTHER_BAK_IDX))
        {
            return 0xA8;
        }
        poctinf->lpurse_other_org = poctinf->lpurse_other_bak;
        memcpy(poctinf->blk_purse_other_org.buf, poctinf->blk_purse_other_bak.buf, 16);

    }
    else if (purse_other_bak_invalid)
    {

        //Ǯ������������
        if (octm1_copy_purse(OCT_BLK_PURSE_OTHER_BAK_IDX, OCT_BLK_PURSE_OTHER_ORG_IDX))
        {
            return 0xA8;
        }
        poctinf->lpurse_other_bak = poctinf->lpurse_other_org;
        memcpy(poctinf->blk_purse_other_bak.buf, poctinf->blk_purse_other_org.buf, 16);

    }
    else
    {
        //Ǯ�����������Ϸ������ȵ����ֻ��Ҫ�����������
        if (poctinf->lpurse_other_org != poctinf->lpurse_other_bak)
        {
            if (memcmp(g_octcurrinf.logiccardnumber, g_m1recover.Logicno, 4)==0)
            {


				if (g_m1recover.npursetype)
				{
					//�ϴ��ǹ���Ǯ������
	                if (poctinf->lpurse_other_org == g_m1recover.balance_after)
	                {
						if (octm1_metro_recover(2, poctinf, metro_recover_flag))
						{
							return 0xA8;
						}

	                    //�ϴη������������ı䣬�����ָ���ԭʼ״̬
	                    if (octm1_copy_purse(OCT_BLK_PURSE_OTHER_ORG_IDX, OCT_BLK_PURSE_OTHER_BAK_IDX))
	                    {
	                        return 0xA8;
	                    }
	                    poctinf->lpurse_other_org = poctinf->lpurse_other_bak;
	                    memcpy(poctinf->blk_purse_other_org.buf, poctinf->blk_purse_other_bak.buf, 16);
	                    memset(&g_m1recover, 0x00, sizeof(g_m1recover));

	                }
                }

            }

        }
    }



    if (public_org_invalid)
    {
        //������Ϣ��������Ч
        //�����ֿ��ܣ�
        //1(��ǰ�ָ�)�ǻ�û����Ǯ��ʱд��, ��ʱǮ������������ȵ�,ֻ�轫������Ϣ���������Ƶ�����
        //2(���ָ�)�ǲ�����Ǯ��������д��, ��ʱǮ�������Ѹ���, �轫������Ϣ�������ۼƺ��Ƶ�������д��
        status = octm1_common_purse_recover_b3(poctinf);
        if (status) return(status);
    }
    else if (public_bak_invalid)
    {
        //����B0
        status = octm1_common_purse_recover_b0(poctinf);
        if (status) return(status);
    }
    else
    {
        //������Ϣ����������ȷ�����������
        if (memcmp(poctinf->blk_public_org.buf, poctinf->blk_public_bak.buf, 16))
        {
            // 1������Ϣ��������
            //�жϹ�����Ϣ���������̱�ʶ=1 ��=2
            if (poctinf->blk_public_org.detail.proc_flag == 0x11)
            {
                //����B2
                //dbg_formatvar("B2\n");
                status = octm1_common_purse_recover_b2(poctinf);
                if (status) return(status);
            }
            else if (poctinf->blk_public_org.detail.proc_flag == 0x13)
            {
                //����B1
                //dbg_formatvar("B1\n");
                status = octm1_common_purse_recover_b1(poctinf);
                if (status) return(status);

            }
            else if (poctinf->blk_public_org.detail.proc_flag == 0x14)
            {
                //����B4
                //dbg_formatvar("B4\n");
                status = octm1_common_purse_recover_b4(poctinf);
                if (status) return(status);

            }
            else if (poctinf->blk_public_org.detail.proc_flag == 0x12)
            {
                //����B5
                //dbg_formatvar("B5\n");
                status = octm1_common_purse_recover_b5(poctinf);
                if (status) return(status);

            }

        }
        else
        {
            // 2������Ϣ���������
            if (poctinf->lpurse_common_org != poctinf->lpurse_common_bak)
            {
                if (octm1_copy_purse(OCT_BLK_PURSE_BAK_IDX, OCT_BLK_PURSE_ORG_IDX))
                {
                    return 0xA8;
                }

                poctinf->lpurse_common_bak = poctinf->lpurse_common_org;
                memcpy(poctinf->blk_purse_common_bak.buf, poctinf->blk_purse_common_org.buf, 16);
            }

			if (poctinf->lpurse_other_org != poctinf->lpurse_other_bak)
			{
				if (octm1_copy_purse(OCT_BLK_PURSE_OTHER_BAK_IDX, OCT_BLK_PURSE_OTHER_ORG_IDX))
				{
					return 0xA8;
				}

				poctinf->lpurse_other_bak = poctinf->lpurse_other_org;
				memcpy(poctinf->blk_purse_other_bak.buf, poctinf->blk_purse_other_org.buf, 16);
			}


        }

    }

	octm1_metro_recover(0, poctinf, metro_recover_flag);



    return 0x00;

}

unsigned char octm1_saminit(unsigned char idx_sam, unsigned char *lpsamcode)
{
    unsigned char ret=5;
    unsigned char tmpbuf[50];
    unsigned char uclen;
    unsigned short int response_sw;

    do
    {
        sam_setbaud(idx_sam, SAM_BAUDRATE_38400);

        ret = sam_rst(idx_sam,tmpbuf);
        if ( ret == 0 )
        {
            ret = 1;
            break;
        }

        uclen = 5;
        memcpy(tmpbuf, "\x00\xB0\x96\x00\x06",uclen);
        sam_cmd(idx_sam,uclen, tmpbuf, tmpbuf, &response_sw);
        if ( response_sw != 0x9000 )
        {
        	ret = 3;
            break;
        }

        memcpy(lpsamcode, tmpbuf, 6);
		if (octsam_exportkey(idx_sam))
		{
			ret = 4;
			break;
		}
		ret = 0;

    }
    while (0);

    return (ret);
}


//�ڶ�����Ϣʱ���������������������
unsigned char g_public_inf[16];//д���õ�
//unsigned char g_history_inf[16];//д���õ�
unsigned long g_nbalance;//����ǰ����Ǯ����д���õ�
unsigned long g_nbalance2;//����ǰר��Ǯ����д���õ�

//==========================================================================================
//�� �� ��: octm1_getdata
//��    ��: ��ȡm1���ؼ�����
//��ڲ���:
//          idx_sam:��ǰӦ��SAM����Ӧ�Ŀ������к�(��0��ʼ)
//     lpcardnumber:Ѱ�������л�ȡ��4�ֽڵ�������
//���ڲ���:
//          lprev:ȡ�õ���������,��������
//					//==================================================
//					    unsigned char szbalance[4];
//					//==================================================
//					    unsigned char type_major;//��Ƭ������ 50�����ÿ�
//					    unsigned char type_minjor;//Ӧ��������
//					                              // 01����ͨǮ����
//					                              // A3�����˿�
//					                              // A4��ѧ����
//					                              // A5��������Ʊ��
//					                              // A6���м���Ʊ��
//					                              // A7������ԱƱ��
//					                              // A8������ѧ����
//					    unsigned char appserial[4]; //Ӧ�ÿ���� bcd
//					    unsigned char sale_dt[5];//����ʱ�� bcd, yymmddhhnn
//					    unsigned char sale_addr[2]; //���۵ص��� bcd,
//					    unsigned char deposit;//Ѻ��,Ԫ
//					    unsigned char appflag;//Ӧ�ñ�ʶ,AA��δ���ã�BB�����ã�=02��CPU������
//					//===================
//					    unsigned char verify_dt[3]; //������ yymmdd
//					    unsigned char pursemode; //���ڴ�ֵ���
//					                             //0x00������ר��Ǯ��������Ǯ����δ���ã�
//					                             //0x01��ר��Ǯ�������ã�
//					                             //0x02������Ǯ�������ã�
//					                             //0x03��ר��Ǯ��������Ǯ����������
//					    unsigned char city[2];  //���д���
//					    unsigned char industry[2]; //��ҵ����
//					//=================================================
//					    unsigned char common_count[2]; //����Ǯ�����״���,��λ��ǰ
//					    unsigned char other_count[2];  //ר��Ǯ�����״���,��λ��ǰ
//					    unsigned char dt_trdate[4]; //��������yyyy-mm-dd
//					    unsigned char lock_flag; //04��������������
//					//=================================================
//					    unsigned char tr_type;//��������
//					    unsigned char servicer_code[2];//�����̴���
//					    unsigned char terminal_no[4];   //POS��� 4BYTE;
//					    unsigned char dt_trdate2[3]; //������ʷ��������yy-mm-dd
//					    unsigned char banlance[3];  //����ǰ���
//					    unsigned char tr_amount[2];    //���׽��
//					//==================================================
//						unsigned char metro_inf[14]; //����ҪCRC16;


//�� �� ֵ:
//          0=�ɹ�
//==========================================================================================
//unsigned char octm1_getdata(unsigned char idx_sam, unsigned char *lpcardnumber, unsigned char *lprev)
//{
//    unsigned char ret=0;
//    unsigned char tmpbuf[16];
//    TOCT_INF octinf;
//    LPTOUTPUT lpdata = (LPTOUTPUT)lprev;
//
//    do
//    {
//        ret=ISO14443A_Request( 0x52, tmpbuf );
//        if (ret)
//        {
//            ret=ISO14443A_Request( 0x52, tmpbuf );
//        }
//        if (ret) break;
//
//        ret=ISO14443A_Select(lpcardnumber, tmpbuf);
//        if (ret) break;
//
//        memcpy(g_octcurrinf.physicalnumber, lpcardnumber, 4);
//
//        g_octcurrinf.currsector = -1;
//        //������Կ
//        //��Կ�ڳ�ʼ��SAM��ʱ�ѵ���
////        ret = octsam_exportkey(OCTM1SAM_SOCKET);
////        if (ret) break;
//		if (octsam_chk(idx_sam))
//		{
//			ret = 0xE5;
//			break;
//		}
//
//        //�������ݿ�
//        ret = octm1_readblock(OCT_BLK_ISSUESUE_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_issue.buf, tmpbuf, 16);
//
//
//        //Ӧ���������ݿ�
//        ret = octm1_readblock(OCT_BLK_ADDTIONALISSUE_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_addtional.buf, tmpbuf, 16);
//
//        //����Ǯ������
//        ret = octm1_readblock(OCT_BLK_PURSE_ORG_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_purse_common_org.buf, tmpbuf, 16);
//
//
//        //����Ǯ������
//        ret = octm1_readblock(OCT_BLK_PURSE_BAK_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_purse_common_bak.buf, tmpbuf, 16);
//
//        //ר��Ǯ������
//        ret = octm1_readblock(OCT_BLK_PURSE_OTHER_ORG_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_purse_other_org.buf, tmpbuf, 16);
//
//        //ר��Ǯ������
//        ret = octm1_readblock(OCT_BLK_PURSE_OTHER_BAK_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_purse_other_bak.buf, tmpbuf, 16);
//
//        //������Ϣ������
//        ret = octm1_readblock(OCT_BLK_PUBLIC_ORG_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_public_org.buf, tmpbuf, 16);
//
//        //������Ϣ������
//        ret = octm1_readblock(OCT_BLK_PUBLIC_BAK_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_public_bak.buf, tmpbuf, 16);
//
//        //���ѹ�����ʷ
//        ret = octm1_readblock(OCT_BLK_HISTORY_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_history.buf, tmpbuf, 16);
//
//        //������Ϣ������
//        ret = octm1_readblock(OCT_BLK_METRO_ORG_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_metro_org.buf, tmpbuf, 16);
//
//        //������Ϣ������
//        ret = octm1_readblock(OCT_BLK_METRO_BAK_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_metro_org.buf, tmpbuf, 16);
//
////	        dbg_dumpmemory("blk_purse_common_org=",octinf.blk_purse_common_org.buf,16);
////	        dbg_dumpmemory("blk_purse_common_bak=",octinf.blk_purse_common_bak.buf,16);
////	        dbg_dumpmemory("blk_purse_other_org=",octinf.blk_purse_other_org.buf,16);
////	        dbg_dumpmemory("blk_purse_other_org=",octinf.blk_purse_other_bak.buf,16);
////	        dbg_dumpmemory("blk_public_org=",octinf.blk_public_org.buf,16);
////	        dbg_dumpmemory("blk_public_bak=",octinf.blk_public_bak.buf,16);
////	        dbg_dumpmemory("blk_history_org=",octinf.blk_history.buf,16);
////	        dbg_dumpmemory("blk_metro_org=",octinf.blk_metro_org.buf,16);
////	        dbg_dumpmemory("blk_metro_bak=",octinf.blk_metro_bak.buf,16);
//
//        //���лָ����̲������Ա�֤Ǯ������������Ϣ������������ȷ��
//        ret = octm1_common_purse_recover(&octinf);
//        if (ret) break;
//
////	        dbg_formatvar("recover\n");
////	        dbg_dumpmemory("blk_issue_bak=",octinf.blk_issue.buf,16);
////	        dbg_dumpmemory("blk_addtional_bak=",octinf.blk_addtional.buf,16);
////	        dbg_dumpmemory("blk_purse_common_org=",octinf.blk_purse_common_org.buf,16);
////	        dbg_dumpmemory("blk_purse_common_bak=",octinf.blk_purse_common_bak.buf,16);
////	        dbg_dumpmemory("blk_purse_other_org=",octinf.blk_purse_other_org.buf,16);
////	        dbg_dumpmemory("blk_purse_other_org=",octinf.blk_purse_other_bak.buf,16);
////	        dbg_dumpmemory("blk_public_org=",octinf.blk_public_org.buf,16);
////	        dbg_dumpmemory("blk_public_bak=",octinf.blk_public_bak.buf,16);
////	        dbg_dumpmemory("blk_history_org=",octinf.blk_history.buf,16);
////	        dbg_dumpmemory("blk_metro_org=",octinf.blk_metro_org.buf,16);
////	        dbg_dumpmemory("blk_metro_bak=",octinf.blk_metro_bak.buf,16);
//
//
//        //========================================================
//        memcpy(g_public_inf, octinf.blk_public_org.buf, 16);
//        g_nbalance = octinf.blk_purse_common_org.buf[3];
//        g_nbalance <<= 8;
//        g_nbalance += octinf.blk_purse_common_org.buf[2];
//        g_nbalance <<= 8;
//        g_nbalance += octinf.blk_purse_common_org.buf[1];
//        g_nbalance <<= 8;
//        g_nbalance += octinf.blk_purse_common_org.buf[0];
//
//
//        //�����ֵ
//        //Ǯ��ֵ
//        lpdata->szbalance[0] = octinf.blk_purse_common_org.buf[3];
//        lpdata->szbalance[1] = octinf.blk_purse_common_org.buf[2];
//        lpdata->szbalance[2] = octinf.blk_purse_common_org.buf[1];
//        lpdata->szbalance[3] = octinf.blk_purse_common_org.buf[0];
////=============================================================================
//        //������Ϣ��������
//        lpdata->type_major = octinf.blk_issue.detail.type_major;//��Ƭ������ 50�����ÿ�
//        lpdata->type_minjor = octinf.blk_issue.detail.type_minjor;//Ӧ��������
//        lpdata->type_minjor = octinf.blk_issue.detail.type_minjor;//Ӧ��������
//        memcpy(lpdata->appserial, octinf.blk_issue.detail.appserial, 4); //��Ӧ�ñ��
//        memcpy(lpdata->sale_dt, octinf.blk_issue.detail.sale_dt, 5);//�ۿ�ʱ�� yymmddhhnn
//        memcpy(lpdata->sale_addr, octinf.blk_issue.detail.sale_addr, 2);//�ۿ��ص��� bcd
//        lpdata->deposit = octinf.blk_issue.detail.deposit;//Ѻ��,Ԫ
//        lpdata->appflag = octinf.blk_issue.detail.appflag;//Ӧ�ñ�ʶ
//
//		memcpy(g_octcurrinf.logiccardnumber, lpdata->appserial, 4);
//
//
////==========================================================================================
//        //Ӧ��������������
//        memcpy(lpdata->verify_dt, octinf.blk_addtional.detail.verify_dt, 3); //������ yymmdd
//        lpdata->pursemode = octinf.blk_addtional.detail.pursemode; //���ڴ�ֵ���
//        memcpy(lpdata->city, octinf.blk_addtional.detail.city, 2);  //���д���
//        memcpy(lpdata->industry, octinf.blk_addtional.detail.industry,2); //��ҵ����
////=================================================
//        //������Ϣ��������
//        memcpy(lpdata->common_count, octinf.blk_public_org.detail.common_count, 2); //����Ǯ�����״���,��λ��ǰ
//        memcpy(lpdata->other_count, octinf.blk_public_org.detail.other_count, 2);//[2];  //ר��Ǯ�����״���,��λ��ǰ
//        memcpy(lpdata->dt_trdate, octinf.blk_public_org.detail.dt_trdate, 4); //[4]; //��������yyyy-mm-dd
//        lpdata->lock_flag = octinf.blk_public_org.detail.lock_flag; //04��������������
////=================================================
//        //������Ϣ��ʷ��������
//        lpdata->tr_type = octinf.blk_history.detail.tr_type;//��������
//        memcpy(lpdata->servicer_code, octinf.blk_history.detail.servicer_code, 2);//�����̴���
//        memcpy(lpdata->terminal_no, octinf.blk_history.detail.terminal_no, 4);//[4];   //POS��� 4BYTE;
//        memcpy(lpdata->dt_trdate2, octinf.blk_history.detail.dt_trdate, 3);//[3]; //��������yy-mm-dd
//        memcpy(lpdata->banlance, octinf.blk_history.detail.banlance, 3);  //����ǰ���
//        memcpy(lpdata->tr_amount, octinf.blk_history.detail.tr_amount, 2);    //���׽��
////==================================================
//        //������Ϣ��������
//        memcpy(lpdata->metro_inf, octinf.blk_metro_org.buf, 14);
//        return(0);
//    }
//    while (0);
//
//    return(ret);
//
//}

//==========================================================================================
//�� �� ��: octm1_getdata2
//��    ��: ��ȡm1���ؼ�����
//��ڲ���:
//          idx_sam:��ǰӦ��SAM����Ӧ�Ŀ������к�(��0��ʼ)
//     lpcardnumber:Ѱ�������л�ȡ��4�ֽڵ�������
//���ڲ���:
//          lprev:ȡ�õ���������,��������
//					//==================================================
//					    unsigned char szbalance[4];   // ����Ǯ��
//					    unsigned char szbalance2[4]; // ר��Ǯ��
//					//==================================================
//					    unsigned char type_major;//��Ƭ������ 50�����ÿ�
//					    unsigned char type_minjor;//Ӧ��������
//					                              // 01����ͨǮ����
//					                              // A3�����˿�
//					                              // A4��ѧ����
//					                              // A5��������Ʊ��
//					                              // A6���м���Ʊ��
//					                              // A7������ԱƱ��
//					                              // A8������ѧ����
//					    unsigned char appserial[4]; //Ӧ�ÿ���� bcd
//					    unsigned char sale_dt[5];//����ʱ�� bcd, yymmddhhnn
//					    unsigned char sale_addr[2]; //���۵ص��� bcd,
//					    unsigned char deposit;//Ѻ��,Ԫ
//					    unsigned char appflag;//Ӧ�ñ�ʶ,AA��δ���ã�BB�����ã�=02��CPU������
//					//===================
//					    unsigned char verify_dt[3]; //������ yymmdd
//					    unsigned char pursemode; //���ڴ�ֵ���
//					                             //0x00������ר��Ǯ��������Ǯ����δ���ã�
//					                             //0x01��ר��Ǯ�������ã�
//					                             //0x02������Ǯ�������ã�
//					                             //0x03��ר��Ǯ��������Ǯ����������
//					    unsigned char city[2];  //���д���
//					    unsigned char industry[2]; //��ҵ����
//					//=================================================
//					    unsigned char common_count[2]; //����Ǯ�����״���,��λ��ǰ
//					    unsigned char other_count[2];  //ר��Ǯ�����״���,��λ��ǰ
//					    unsigned char dt_trdate[4]; //��������yyyy-mm-dd
//					    unsigned char lock_flag; //04��������������
//					//=================================================
//					    unsigned char tr_type;//��������
//					    unsigned char servicer_code[2];//�����̴���
//					    unsigned char terminal_no[4];   //POS��� 4BYTE;
//					    unsigned char dt_trdate2[3]; //������ʷ��������yy-mm-dd
//					    unsigned char banlance[3];  //����ǰ���
//					    unsigned char tr_amount[2];    //���׽��
//					//==================================================
//						unsigned char metro_inf[14]; //����ҪCRC16;


//�� �� ֵ:
//          0=�ɹ�
//==========================================================================================
unsigned char octm1_getdata2(unsigned char idx_sam, unsigned char *lpcardnumber, unsigned char *lprev)
{
    unsigned char ret=0;
    unsigned char tmpbuf[16];
    TOCT_INF octinf;
    LPTOUTPUT2 lpdata = (LPTOUTPUT2)lprev;

    do
    {
        ret=ISO14443A_Request( 0x52, tmpbuf );
        if (ret)
        {
            ret=ISO14443A_Request( 0x52, tmpbuf );
        }
        if (ret) break;

		//g_Record.log_out(0, level_disaster,"octm1_getdata2-ISO14443A_Anticoll");
		ret=ISO14443A_Anticoll(lpcardnumber);
		if (ret) break;

        ret=ISO14443A_Select(lpcardnumber, tmpbuf);
        if (ret) break;

        memcpy(g_octcurrinf.physicalnumber, lpcardnumber, 4);

        g_octcurrinf.currsector = -1;
        //������Կ
        //��Կ�ڳ�ʼ��SAM��ʱ�ѵ���
//        ret = octsam_exportkey(OCTM1SAM_SOCKET);
//        if (ret) break;
		if (octsam_chk(idx_sam))
		{
			ret = 0xE5;
			break;
		}

        //�������ݿ�
        ret = octm1_readblock(OCT_BLK_ISSUESUE_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_issue.buf, tmpbuf, 16);


        //Ӧ���������ݿ�
        ret = octm1_readblock(OCT_BLK_ADDTIONALISSUE_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_addtional.buf, tmpbuf, 16);

        //����Ǯ������
        ret = octm1_readblock(OCT_BLK_PURSE_ORG_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_purse_common_org.buf, tmpbuf, 16);


        //����Ǯ������
        ret = octm1_readblock(OCT_BLK_PURSE_BAK_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_purse_common_bak.buf, tmpbuf, 16);

        //ר��Ǯ������
        ret = octm1_readblock(OCT_BLK_PURSE_OTHER_ORG_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_purse_other_org.buf, tmpbuf, 16);

        //ר��Ǯ������
        ret = octm1_readblock(OCT_BLK_PURSE_OTHER_BAK_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_purse_other_bak.buf, tmpbuf, 16);

        //������Ϣ������
        ret = octm1_readblock(OCT_BLK_PUBLIC_ORG_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_public_org.buf, tmpbuf, 16);

        //������Ϣ������
        ret = octm1_readblock(OCT_BLK_PUBLIC_BAK_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_public_bak.buf, tmpbuf, 16);

        //���ѹ�����ʷ
        ret = octm1_readblock(OCT_BLK_HISTORY_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_history.buf, tmpbuf, 16);

        //������Ϣ������
        ret = octm1_readblock(OCT_BLK_METRO_ORG_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_metro_org.buf, tmpbuf, 16);

        //������Ϣ������
        ret = octm1_readblock(OCT_BLK_METRO_BAK_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_metro_bak.buf, tmpbuf, 16);

//	        dbg_dumpmemory("blk_purse_common_org=",octinf.blk_purse_common_org.buf,16);
//	        dbg_dumpmemory("blk_purse_common_bak=",octinf.blk_purse_common_bak.buf,16);
//	        dbg_dumpmemory("blk_purse_other_org=",octinf.blk_purse_other_org.buf,16);
//	        dbg_dumpmemory("blk_purse_other_org=",octinf.blk_purse_other_bak.buf,16);
//	        dbg_dumpmemory("blk_public_org=",octinf.blk_public_org.buf,16);
//	        dbg_dumpmemory("blk_public_bak=",octinf.blk_public_bak.buf,16);
//	        dbg_dumpmemory("blk_history_org=",octinf.blk_history.buf,16);
//	        dbg_dumpmemory("blk_metro_org=",octinf.blk_metro_org.buf,16);
//	        dbg_dumpmemory("blk_metro_bak=",octinf.blk_metro_bak.buf,16);

        //���лָ����̲������Ա�֤Ǯ������������Ϣ������������ȷ��
        if (octinf.blk_issue.detail.type_minjor==0xA4 || octinf.blk_issue.detail.type_minjor==0xB4 || octinf.blk_issue.detail.type_minjor==0xA7)
        {
			//ѧ������ר��Ǯ���ָ�Ϊ��������Ǯ���ָ������ж�ִ��״̬
			ret = octm1_other_purse_recover(&octinf, 1);
			if (ret) break;
			octm1_common_purse_recover(&octinf, 0);
        }
        else
        {
			//��ѧ�����Թ���Ǯ���ָ�Ϊ����ר��Ǯ���ָ������ж�ִ��״̬
			ret = octm1_common_purse_recover(&octinf, 1);
			if (ret) break;
			octm1_other_purse_recover(&octinf, 0);
        }

//	        dbg_formatvar("recover\n");
//	        dbg_dumpmemory("blk_issue_bak=",octinf.blk_issue.buf,16);
//	        dbg_dumpmemory("blk_addtional_bak=",octinf.blk_addtional.buf,16);
//	        dbg_dumpmemory("blk_purse_common_org=",octinf.blk_purse_common_org.buf,16);
//	        dbg_dumpmemory("blk_purse_common_bak=",octinf.blk_purse_common_bak.buf,16);
//	        dbg_dumpmemory("blk_purse_other_org=",octinf.blk_purse_other_org.buf,16);
//	        dbg_dumpmemory("blk_purse_other_org=",octinf.blk_purse_other_bak.buf,16);
//	        dbg_dumpmemory("blk_public_org=",octinf.blk_public_org.buf,16);
//	        dbg_dumpmemory("blk_public_bak=",octinf.blk_public_bak.buf,16);
//	        dbg_dumpmemory("blk_history_org=",octinf.blk_history.buf,16);
//	        dbg_dumpmemory("blk_metro_org=",octinf.blk_metro_org.buf,16);
//	        dbg_dumpmemory("blk_metro_bak=",octinf.blk_metro_bak.buf,16);


        //========================================================
        memcpy(g_public_inf, octinf.blk_public_org.buf, 16);
        g_nbalance = octinf.blk_purse_common_org.buf[3];
        g_nbalance <<= 8;
        g_nbalance += octinf.blk_purse_common_org.buf[2];
        g_nbalance <<= 8;
        g_nbalance += octinf.blk_purse_common_org.buf[1];
        g_nbalance <<= 8;
        g_nbalance += octinf.blk_purse_common_org.buf[0];


        g_nbalance2 = octinf.blk_purse_other_org.buf[3];
        g_nbalance2 <<= 8;
        g_nbalance2 += octinf.blk_purse_other_org.buf[2];
        g_nbalance2 <<= 8;
        g_nbalance2 += octinf.blk_purse_other_org.buf[1];
        g_nbalance2 <<= 8;
        g_nbalance2 += octinf.blk_purse_other_org.buf[0];


        //�����ֵ
        //Ǯ��ֵ
        lpdata->szbalance[0] = octinf.blk_purse_common_org.buf[3];
        lpdata->szbalance[1] = octinf.blk_purse_common_org.buf[2];
        lpdata->szbalance[2] = octinf.blk_purse_common_org.buf[1];
        lpdata->szbalance[3] = octinf.blk_purse_common_org.buf[0];

		lpdata->szbalance2[0] = octinf.blk_purse_other_org.buf[3];
		lpdata->szbalance2[1] = octinf.blk_purse_other_org.buf[2];
		lpdata->szbalance2[2] = octinf.blk_purse_other_org.buf[1];
		lpdata->szbalance2[3] = octinf.blk_purse_other_org.buf[0];

//=============================================================================
        //������Ϣ��������
        lpdata->type_major = octinf.blk_issue.detail.type_major;//��Ƭ������ 50�����ÿ�
        lpdata->type_minjor = octinf.blk_issue.detail.type_minjor;//Ӧ��������
        lpdata->type_minjor = octinf.blk_issue.detail.type_minjor;//Ӧ��������
        memcpy(lpdata->appserial, octinf.blk_issue.detail.appserial, 4); //��Ӧ�ñ��
        memcpy(lpdata->sale_dt, octinf.blk_issue.detail.sale_dt, 5);//�ۿ�ʱ�� yymmddhhnn
        memcpy(lpdata->sale_addr, octinf.blk_issue.detail.sale_addr, 2);//�ۿ��ص��� bcd
        lpdata->deposit = octinf.blk_issue.detail.deposit;//Ѻ��,Ԫ
        lpdata->appflag = octinf.blk_issue.detail.appflag;//Ӧ�ñ�ʶ

		memcpy(g_octcurrinf.logiccardnumber, lpdata->appserial, 4);


//==========================================================================================
        //Ӧ��������������
        memcpy(lpdata->verify_dt, octinf.blk_addtional.detail.verify_dt, 3); //������ yymmdd
        lpdata->pursemode = octinf.blk_addtional.detail.pursemode; //���ڴ�ֵ���
        memcpy(lpdata->city, octinf.blk_addtional.detail.city, 2);  //���д���
        memcpy(lpdata->industry, octinf.blk_addtional.detail.industry,2); //��ҵ����
//=================================================
        //������Ϣ��������
        memcpy(lpdata->common_count, octinf.blk_public_org.detail.common_count, 2); //����Ǯ�����״���,��λ��ǰ
        memcpy(lpdata->other_count, octinf.blk_public_org.detail.other_count, 2);//[2];  //ר��Ǯ�����״���,��λ��ǰ
        memcpy(lpdata->dt_trdate, octinf.blk_public_org.detail.dt_trdate, 4); //[4]; //��������yyyy-mm-dd
        lpdata->lock_flag = octinf.blk_public_org.detail.lock_flag; //04��������������
//=================================================
        //������Ϣ��ʷ��������
        lpdata->tr_type = octinf.blk_history.detail.tr_type;//��������
        memcpy(lpdata->servicer_code, octinf.blk_history.detail.servicer_code, 2);//�����̴���
        memcpy(lpdata->terminal_no, octinf.blk_history.detail.terminal_no, 4);//[4];   //POS��� 4BYTE;
        memcpy(lpdata->dt_trdate2, octinf.blk_history.detail.dt_trdate, 3);//[3]; //��������yy-mm-dd
        memcpy(lpdata->banlance, octinf.blk_history.detail.banlance, 3);  //����ǰ���
        memcpy(lpdata->tr_amount, octinf.blk_history.detail.tr_amount, 2);    //���׽��
//==================================================
        //������Ϣ��������
        memcpy(lpdata->metro_inf, octinf.blk_metro_org.buf, 14);
        return(0);
    }
    while (0);

    return(ret);

}


//==========================================================================================
//�� �� ��: octm1_writedata
//��    ��: ��M1������д����
//��ڲ���:
//            idx_sam:��ǰӦ��SAM����Ӧ�Ŀ������к�(��0��ʼ)
//          nopertion:������ʽ, 0=����������,1=����������(��д������Ϣ��Ҳ���۷ѣ�ֻд���������), 2=ֻ���µ�����Ϣ����=3ֻ�۷Ѳ�����������Ϣ��, =4�۷ѵ�ͬʱд������Ϣ��,
//          tr_dttime:����ʱ�䣬ֻ����Ҫ��ֵʱ��ʹ��, bcd��, yyyymmddhhnnss
//             lvalue:Ҫ���ѵ�ֵ
//          metro_inf:������Ϣ�� 16byte
//
//���ڲ���:
//          ��
//�� �� ֵ:
//          0=�ɹ�
//       0xEE=д��ʧ�ܣ���Ҫʱ�н���ȷ��
//       ����=д��ʧ�ܣ����账��
//==========================================================================================
//unsigned char octm1_writedata(unsigned char idx_sam, unsigned char nopertion, unsigned char *tr_dttime, unsigned long lvalue, unsigned char *metro_inf)
//{
//
//    const unsigned char tr_type=0x03;//�̶�Ϊ0x03��ʾ����Ǯ������
//    const unsigned char service_code[2]={0x00,0x00};//����δ���壬��ǿ��0x00
//    const unsigned char pos_code[4]={0x00,0x00};//����δ�������壬��ǿ��0x00
//
//    unsigned char tmpbuf[16];
//    unsigned char ret = -1;
//
//
//    PTOCT_BLK_PUBLIC_DETAIL lppublic = (PTOCT_BLK_PUBLIC_DETAIL)g_public_inf;
//    PTOCT_BLK_HISTORY_DETAIL lphistory = (PTOCT_BLK_HISTORY_DETAIL)tmpbuf;
//
//    do
//    {
//
//        memset(tmpbuf, 0x00, sizeof(tmpbuf));
//
//        if ((nopertion==0)||(nopertion==1))
//        {
//            //=================================================
//            //����������(��д������Ϣ��Ҳ���۷ѣ�ֻд���������)
//            if (nopertion==1)
//            {
//                lppublic->lock_flag = 0x04;
//            }
//            else
//            {
//                lppublic->lock_flag = 0x00;
//            }
//            crc_cu(g_public_inf, 16);//����CRC8
//            ret = octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX, g_public_inf);
//            if (ret)
//            {
//                ret = 0xEE;
//            };
//
//            //����д�ɹ��󣬸�����ʹ����ʧ��Ҳ�����ɹ�
//            octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX, g_public_inf);
//        }
//        else if (nopertion==2)
//        {
//            //========================================================
//            //ֻ���µ�����Ϣ��
//            ret = octm1_writeblock(OCT_BLK_METRO_ORG_IDX, metro_inf);
//            if (ret)
//            {
//                ret = 0xEE;
//            };
//
//            //����д�ɹ��󣬸�����ʹ����ʧ��Ҳ�����ɹ�
//            octm1_writeblock(OCT_BLK_METRO_BAK_IDX, metro_inf);
//
//        }
//        else if ((nopertion==3) ||(nopertion==4))
//        {
//            //============================================================
//            //3=ֻ�۷Ѳ�����������Ϣ��
//            //4=//�۷ѵ�ͬʱд������Ϣ��
//            if (lvalue > g_nbalance)
//            {
//                return 0x25;
//            }
//
//
//            //д���׹�����ʷ
//            lphistory->tr_type = tr_type;
//            memcpy(lphistory->servicer_code, service_code, 2);
//            memcpy(lphistory->terminal_no, pos_code, 4);
//            memcpy(lphistory->dt_trdate, tr_dttime+2, 3);//yymmdd
//            lphistory->banlance[0] = ((g_nbalance & 0x00FF0000) >> 16);
//            lphistory->banlance[1] = ((g_nbalance & 0x0000FF00) >> 8);
//            lphistory->banlance[2] = (g_nbalance & 0x000000FF);
//            lphistory->tr_amount[0] = ((lvalue & 0x0000FF00) >> 8);
//            lphistory->tr_amount[1] = (lvalue & 0x000000FF);
//
//            lphistory->tr_type = tr_type;
//            crc_cu(tmpbuf, 16);
//            ret = octm1_writeblock(OCT_BLK_HISTORY_IDX, tmpbuf);
//            if (ret)
//            {
//                ret = 0xA1;
//                break;
//            }
//
//
//            //д������Ϣ��, ���̱�ʶ=0x11,��������+1, ��������=0x03
//            octm1_inc_public_tr_count(0, lppublic);
//            lppublic->proc_flag = 0x11;
//            lppublic->tr_type = tr_type;
//            memcpy(lppublic->dt_trdate, tr_dttime, 4);
//            crc_cu(g_public_inf, 16);
//            ret = octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX, g_public_inf);
//            if (ret)
//            {
//                ret = 0xA2;
//                break;
//            }
//
//
//            if (nopertion==4)
//            {
//                //��д�������Ϣ��
//                ret = octm1_writeblock(OCT_BLK_METRO_ORG_IDX, metro_inf);
//                if (ret)
//                {
//                    ret = 0xEE;
//                    break;
//                }
//
//            }
//
//            //Ǯ�������ۿ�
//            //0xC0=���ѣ�
//            ret = octm1_write_purse(OCT_BLK_PURSE_ORG_IDX, 0xC0,lvalue);
//            if (ret)
//            {
//                ret = 0xEE;
//                g_m1recover.npursetype = 0;
//				memcpy(g_m1recover.Logicno, g_octcurrinf.logiccardnumber, 4);
//				g_m1recover.balance_after = g_nbalance - lvalue;
//                break;
//            }
//
//			if (memcmp(g_m1recover.Logicno, g_octcurrinf.logiccardnumber, 4)==0)
//			{
//				memset(&g_m1recover, 0x00, sizeof(g_m1recover));
//			}
//
//            //Ǯ�������ۿ�ɹ���Ĳ���ʧ�ܶ��������׳ɹ�
//            //д������Ϣ��, ���̱�ʶ=0x13
//            lppublic->proc_flag = 0x13;
//            crc_cu(g_public_inf, 16);
//            ret = octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX, g_public_inf);
//            if (ret)
//            {
//                ret = 0;
//                break;
//            }
//
//            //Ǯ������������
//            ret = octm1_copy_purse(OCT_BLK_PURSE_BAK_IDX, OCT_BLK_PURSE_ORG_IDX);
//            if (ret)
//            {
//                ret = 0x0;
//                break;
//            }
//
//
//            //д������Ϣ��, ���̱�ʶ=0x13
//            ret = octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX, g_public_inf);
//            if (ret)
//            {
//                ret = 0;
//                break;
//            }
//            ret = octm1_writeblock(OCT_BLK_METRO_BAK_IDX, metro_inf);
//            if (ret)
//            {
//                ret = 0;
//                break;
//            }
//
//        }
//        else
//        {
//            //������ʽ�������󣬲���ʶ��
//            ret = 0xE1;
//
//        }
//
//
//    }
//    while (0);
//
//    return(ret);
//
//
//}

//==========================================================================================
//�� �� ��: octm1_writedata2
//��    ��: ��M1������д����
//��ڲ���:
//            idx_sam:��ǰӦ��SAM����Ӧ�Ŀ������к�(��0��ʼ)
//          nopertion:������ʽ, 0=����������,1=����������(��д������Ϣ��Ҳ���۷ѣ�ֻд���������), 2=ֻ���µ�����Ϣ����=3ֻ�۷Ѳ�����������Ϣ��, =4�۷ѵ�ͬʱд������Ϣ��,
//          tr_dttime:����ʱ�䣬ֻ����Ҫ��ֵʱ��ʹ��, bcd��, yyyymmddhhnnss
//       npursetype:Ҫ�����Ǯ�����, 0=����Ǯ��, 1���0ֵ=ר��Ǯ��
//             lvalue:Ҫ���ѵ�ֵ
//          metro_inf:������Ϣ�� 16byte
//
//���ڲ���:
//          ��
//�� �� ֵ:
//          0=�ɹ�
//       0xEE=д��ʧ�ܣ���Ҫʱ�н���ȷ��
//       ����=д��ʧ�ܣ����账��
//==========================================================================================
unsigned char octm1_writedata2(unsigned char idx_sam, unsigned char nopertion, unsigned char *tr_dttime, unsigned char npursetype, unsigned long lvalue, unsigned char *metro_inf)
{

    unsigned char tr_type=0x03;//�̶�Ϊ0x03��ʾ����Ǯ������
    const unsigned char service_code[2]={0x00,0x00};//����δ���壬��ǿ��0x00
    const unsigned char pos_code[4]={0x00,0x00};//����δ�������壬��ǿ��0x00

    unsigned char tmpbuf[16];
    unsigned char ret = -1;


    PTOCT_BLK_PUBLIC_DETAIL lppublic = (PTOCT_BLK_PUBLIC_DETAIL)g_public_inf;
    PTOCT_BLK_HISTORY_DETAIL lphistory = (PTOCT_BLK_HISTORY_DETAIL)tmpbuf;

    do
    {

        memset(tmpbuf, 0x00, sizeof(tmpbuf));

        if ((nopertion==0)||(nopertion==1))
        {
            //=================================================
            //����������(��д������Ϣ��Ҳ���۷ѣ�ֻд���������)
            if (nopertion==1)
            {
                lppublic->lock_flag = 0x04;
            }
            else
            {
                lppublic->lock_flag = 0x00;
            }
            crc_cu(g_public_inf, 16);//����CRC8
            ret = octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX, g_public_inf);
            if (ret)
            {
                ret = 0xEE;
            };

            //����д�ɹ��󣬸�����ʹ����ʧ��Ҳ�����ɹ�
            octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX, g_public_inf);
        }
        else if (nopertion==2)
        {
            //========================================================
            //ֻ���µ�����Ϣ��
            ret = octm1_writeblock(OCT_BLK_METRO_ORG_IDX, metro_inf);
            if (ret)
            {
                ret = 0xEE;
            };

            //����д�ɹ��󣬸�����ʹ����ʧ��Ҳ�����ɹ�
            octm1_writeblock(OCT_BLK_METRO_BAK_IDX, metro_inf);
        }
        else if ((nopertion==3) ||(nopertion==4))
        {
            //============================================================
            //3=ֻ�۷Ѳ�����������Ϣ��
            //4=//�۷ѵ�ͬʱд������Ϣ��
			if (npursetype == 0)
			{
				//����Ǯ������ж�
	            if (lvalue > g_nbalance)
	            {
	                return 0x25;
	            }
				tr_type=0x03;
            }
            else
            {
				//ר��Ǯ������ж�
	            if (lvalue > g_nbalance2)
	            {
	                return 0x25;
	            }
	            tr_type=0x04;
            }


            //д���׹�����ʷ
            lphistory->tr_type = tr_type;
            memcpy(lphistory->servicer_code, service_code, 2);
            memcpy(lphistory->terminal_no, pos_code, 4);
            memcpy(lphistory->dt_trdate, tr_dttime+1, 3);//yymmdd
			if (npursetype == 0)
			{
				//����Ǯ������
	            lphistory->banlance[0] = ((g_nbalance & 0x00FF0000) >> 16);
	            lphistory->banlance[1] = ((g_nbalance & 0x0000FF00) >> 8);
	            lphistory->banlance[2] = (g_nbalance & 0x000000FF);
	            lphistory->tr_amount[0] = ((lvalue & 0x0000FF00) >> 8);
	            lphistory->tr_amount[1] = (lvalue & 0x000000FF);
			}
			else
			{
				//ר��Ǯ������
				lphistory->banlance[0] = ((g_nbalance2 & 0x00FF0000) >> 16);
				lphistory->banlance[1] = ((g_nbalance2 & 0x0000FF00) >> 8);
				lphistory->banlance[2] = (g_nbalance2 & 0x000000FF);
				lphistory->tr_amount[0] = ((lvalue & 0x0000FF00) >> 8);
				lphistory->tr_amount[1] = (lvalue & 0x000000FF);
			}

            lphistory->tr_type = tr_type;
            crc_cu(tmpbuf, 16);
            ret = octm1_writeblock(OCT_BLK_HISTORY_IDX, tmpbuf);
            if (ret)
            {
                ret = 0xA1;
                break;
            }


            //д������Ϣ��, ���̱�ʶ=0x11,��������+1, ��������=0x03
            octm1_inc_public_tr_count(0, lppublic);
			if (npursetype == 0)
			{
				//����Ǯ������
            	lppublic->proc_flag = 0x11;
			}
			else
			{
				//ר��Ǯ������
            	lppublic->proc_flag = 0x12;
			}
            lppublic->tr_type = tr_type;
            memcpy(lppublic->dt_trdate, tr_dttime, 4);
            crc_cu(g_public_inf, 16);
            ret = octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX, g_public_inf);
            if (ret)
            {
                ret = 0xA2;
                break;
            }


            if (nopertion==4)
            {
                //��д�������Ϣ��
                ret = octm1_writeblock(OCT_BLK_METRO_ORG_IDX, metro_inf);
                if (ret)
                {
                    ret = 0xEE;
                    break;
                }

            }

            //Ǯ�������ۿ�
            //0xC0=���ѣ�
			if (npursetype == 0)
			{
				//����Ǯ������
	            ret = octm1_write_purse(OCT_BLK_PURSE_ORG_IDX, 0xC0,lvalue);
			}
			else
			{
				//ר��Ǯ������
				ret = octm1_write_purse(OCT_BLK_PURSE_OTHER_ORG_IDX, 0xC0,lvalue);
			}

            if (ret)
            {
                ret = 0xEE;
				if (npursetype == 0)
				{
					g_m1recover.npursetype = 0;
				}
				else
				{
					g_m1recover.npursetype = 1;
				}

				memcpy(g_m1recover.Logicno, g_octcurrinf.logiccardnumber, 4);
				g_m1recover.balance_after = g_nbalance - lvalue;
                break;
            }

			if (memcmp(g_m1recover.Logicno, g_octcurrinf.logiccardnumber, 4)==0)
			{
				memset(&g_m1recover, 0x00, sizeof(g_m1recover));
			}

            //Ǯ�������ۿ�ɹ���Ĳ���ʧ�ܶ��������׳ɹ�
            //д������Ϣ��, ���̱�ʶ=0x13
			if (npursetype == 0)
			{
				//����Ǯ������
            	lppublic->proc_flag = 0x13;
			}
			else
			{
				//ר��Ǯ������
				lppublic->proc_flag = 0x14;
			}
            crc_cu(g_public_inf, 16);
            ret = octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX, g_public_inf);
            if (ret)
            {
                ret = 0;
                break;
            }

            //Ǯ������������
			if (npursetype == 0)
			{
				//����Ǯ������
            	ret = octm1_copy_purse(OCT_BLK_PURSE_BAK_IDX, OCT_BLK_PURSE_ORG_IDX);
			}
			else
			{
				//ר��Ǯ������
            	ret = octm1_copy_purse(OCT_BLK_PURSE_OTHER_BAK_IDX, OCT_BLK_PURSE_OTHER_ORG_IDX);
			}

            if (ret)
            {
                ret = 0x0;
                break;
            }


            //д������Ϣ��, ���̱�ʶ=0x13
            ret = octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX, g_public_inf);
            if (ret)
            {
                ret = 0;
                break;
            }

			if (nopertion == 2 || nopertion == 4)
				ret = octm1_writeblock(OCT_BLK_METRO_BAK_IDX, metro_inf);
            if (ret)
            {
                ret = 0;
                break;
            }

        }
        else
        {
            //������ʽ�������󣬲���ʶ��
            ret = 0xE1;
        }
    }
    while (0);

    return(ret);
}



//==========================================================================================
//�� �� ��: octm1_gettac
//��    ��: ����TAC��
//��ڲ���:
//          idx_sam:��ǰӦ��SAM����Ӧ�Ŀ������к�(��0��ʼ)
//      lpappnumber:��ǰ��Ƭ��Ӧ�ÿ����, ��octm1_getdata�������ѻ�ȡ
//           lpdata:Ҫ����TAC������������� 16byte
//
//���ڲ���:
//             lptac:Ҫд��ĵ�����Ϣ�� 4byte
//�� �� ֵ:
//          0=�ɹ�
//==========================================================================================
unsigned char octm1_gettac(unsigned char idx_sam, unsigned char *lpappnumber, unsigned char *lpdata, unsigned char *lptac)
{
    const unsigned char cmd_initdes[13]={0x80, 0x1A, 0x28, 0x01, 0x08, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00};
    const unsigned char cmd_descrypt[]={0x80, 0xFA, 0x01, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	unsigned char ulen;
	unsigned short response_sw;
	unsigned char cmdbuf[50];

	ulen = 13;
	memcpy(cmdbuf, cmd_initdes, ulen);
	memcpy(cmdbuf+5, lpappnumber, 4);

	sam_cmd(idx_sam, ulen, cmdbuf, cmdbuf, &response_sw);
	if (response_sw!=0x9000)
	{
		return 0xE5;
	}


	ulen = 21;
	memcpy(cmdbuf, cmd_descrypt, ulen);
	memcpy(cmdbuf+5, lpdata, 16);

	sam_cmd(idx_sam, ulen, cmdbuf, cmdbuf, &response_sw);
	if (response_sw!=0x9000)
	{
		return 0xE5;
	}

	memcpy(lptac, cmdbuf, 4);

	return 0x00;
}


//==========================================================================================
//�� �� ��: octm1_judge
//��    ��: ��ɳ�������ж�,��m1����ѡ��select����ֵsakΪCPU��ʱ��Ҫ�Դ˿��ж��Ƿ�ɳ��CPU��M1��,
//          ����ܲ���ȡM1���ݿ�����Ϊ�Ǵ�CPU�����������أ����ں������ú�����ж���ʲô��ҵ��CPU��;
//          ����ܶ�ȡM1���ݿ���Ϊ����cpu��m1,�ٶ�m1���ķ��������ñ�־�����ж�,=0x01/0xBB�����Թ���M1����
//          ���̽��д���,���=2��ֱ���߹���CPU�����̣���Ϊ������־�������أ����ں������ú�����ж���ʲô��ҵ��CPU��;
//��ڲ���:
//      lpcardnumber:Ʊ����������, ��searchcard�������ѻ�ȡ
//
//���ڲ���:
//             ��
//�� �� ֵ:
//          0=�޿�
//		    2=����������m1���̴���
//          4=����������cpu�����̴���
//          3=������Ҫ�����ж����ĸ���ҵ��CPU��
//===========================================================================
unsigned char octm1_judge(unsigned char *lpcardnumber)
{
	unsigned char ret;
	unsigned char tmpbuf[16];
	PTOCT_BLK_ISSUE_DETAIL lpissue=(PTOCT_BLK_ISSUE_DETAIL)tmpbuf;

	g_octcurrinf.currsector = -1;
	memcpy(g_octcurrinf.physicalnumber, lpcardnumber, 4);

	if (octm1_readblock(OCT_BLK_ISSUESUE_IDX, tmpbuf))
	{

		if (ISO14443A_Init(0x01) != 0)
		{
			dbg_formatvar("ISO14443A_Init");
			g_Record.log_out(0, level_error,"ISO14443A_Init error");
			return 0x00;
		}

		//���ǹ���cpu��M1�� , ����������ж�CPU����
		ret=ISO14443A_Request( 0x52, tmpbuf );
		if (ret)
		{
			ret=ISO14443A_Request( 0x52, tmpbuf );
		}
		if (ret)
		{
			//Ѱ�������򷵻��޿�
			return 0x00;
		}
		ret=ISO14443A_Select(lpcardnumber, tmpbuf);
		if (ret)
		{
			//�Ҳ����ղ��жϵĿ����ж�Ϊ�޿�;
			return 0x00;
		}

		//����CPU�����������ú���Ҫ�����ж����ĸ���ҵ��CPU��
		return 0x03;
	}

	//����ܶ�cpu�����ж�ȡm1���ݿ飬˵���ǹ�����cpu����m1

	//if ((lpissue->appflag == 0x01)||(lpissue->appflag == 0xBB))
	//{
	//	//����M1�� , �����߹���m1��������
	//	return 0x02;
	//}
	//else if (lpissue->appflag  == 0x02)
	//{
	//	//����cpu�� , �����߹���CPU��������
	//	return 0x04;

	//}
	//else if (lpissue->appflag == 0xaa)
	//{
	//	return 98;	// ��Ʊδ����
	//}
	//else
	//{

 //       //����CPU�����������ú���Ҫ�����ж����ĸ���ҵ��CPU��
	//	return 0x03;
	//}

	if (lpissue->appflag  == 0x02)
	{
		//����cpu�� , �����߹���CPU��������
		return 0x04;
	}
	else
	{
		return 0x02;	// ��ת��CPU���̵�ȫ������M1,����M1�������ж����ñ�ʶ
	}
}

unsigned short octm1_judge_ex(unsigned char *lpcardnumber, unsigned char& type)
{
	unsigned short ret				= 0;
	unsigned char tmpbuf[16]		= {0};
	PTOCT_BLK_ISSUE_DETAIL lpissue	= (PTOCT_BLK_ISSUE_DETAIL)tmpbuf;

	do
	{
		g_octcurrinf.currsector = -1;
		memcpy(g_octcurrinf.physicalnumber, lpcardnumber, 4);

		if (octm1_readblock(OCT_BLK_ISSUESUE_IDX, tmpbuf))
		{

			if (ISO14443A_Init(0x01) != 0)
			{
				dbg_formatvar("ISO14443A_Init");
				g_Record.log_out(0, level_error,"ISO14443A_Init error");
				ret = ERR_CARD_NONE;
				break;
			}

			//���ǹ���cpu��M1�� , ����������ж�CPU����
			if (ISO14443A_Request( 0x52, tmpbuf ) != 0)
			{
				//Ѱ�������򷵻��޿�
				dbg_formatvar("octm1_readblock:ISO14443A_Request");
				ret = ERR_CARD_NONE;
				break;
			}

			if (ISO14443A_Anticoll(lpcardnumber) != 0)
			{
				ret = ERR_CARD_NONE;
				break;
			}

			if (ISO14443A_Select(lpcardnumber, tmpbuf) != 0)
			{
				//�Ҳ����ղ��жϵĿ����ж�Ϊ�޿�;
				dbg_formatvar("octm1_readblock:ISO14443A_Select");
				ret = ERR_CARD_NONE;
				break;
			}

			//����CPU�����������ú���Ҫ�����ж����ĸ���ҵ��CPU��
			type = 0x03;
			break;
		}

		//����ܶ�cpu�����ж�ȡm1���ݿ飬˵���ǹ�����cpu����m1

		if (lpissue->appflag  == 0x02)
		{
			//����cpu�� , �����߹���CPU��������
			type = 0x04;
		}
		else
		{
			type = 0x02;	// ��ת��CPU���̵�ȫ������M1,����M1�������ж����ñ�ʶ
		}

	} while (0);

	return ret;
}
