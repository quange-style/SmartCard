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



#define OCTM1SAM_SOCKET 0x02 //OCTSAM卡座序号( 从0 开始)


#define OCT_SECTOR_APP 0       //应用标识扇区号
#define OCT_SECTOR_ISSUE 1     //发行扇区号
#define OCT_SECTOR_PURSE 2     //钱包扇区号
#define OCT_SECTOR_PUBLIC 9    //公共信息扇区号
#define OCT_SECTOR_METRO 11    //公共信息扇区号

#define OCT_BLK_ISSUESUE_IDX 1  //发行信息块
#define OCT_BLK_ADDTIONALISSUE_IDX 6  //应用索引数据块

#define OCT_BLK_PURSE_ORG_IDX 8  //公共钱包正本
#define OCT_BLK_PURSE_BAK_IDX 9  //公共钱包副本

#define OCT_BLK_PURSE_OTHER_ORG_IDX 16  //专用钱包正本
#define OCT_BLK_PURSE_OTHER_BAK_IDX 17  //专用钱包副本

#define OCT_BLK_PUBLIC_ORG_IDX 36  //公共信息区正本
#define OCT_BLK_PUBLIC_BAK_IDX 37  //公共信息区副本
#define OCT_BLK_HISTORY_IDX 38     //公共信息区过程交易历史

#define OCT_BLK_METRO_ORG_IDX 44   //地铁数据块正本
#define OCT_BLK_METRO_BAK_IDX 45   //地铁数据块副本



//短文件标识
#define OCTSAM_FILE_MF 0x3F00
#define OCTSAM_FILE_ADF1 0x1001
#define OCTSAM_FILE_ADF2 0x1002
#define OCTSAM_FILE_ADF3 0x1003


#define PSAM_OCTMF_SCK 0
#define PSAM_OCTCPU_SCK 1


// typedef struct
// {
//     unsigned char logic_id[8];//逻辑卡号
//     unsigned char key_history[6];//key04
//     unsigned char key_purse[6];//key20_1
//     unsigned char key_public[6];//key20_3
//     unsigned char public_blk[16];//锁卡时使用
// } TOCT_KEY, *PTOCT_KEY;
typedef struct
{
    unsigned char key_avalid; //密钥是否用效可用, 00=无效，>0有效
    unsigned char keya[6];//key04
    unsigned char keyb[6];//key20_1
    unsigned char public_blk[16];//锁卡时使用
} TOCT_KEY, *PTOCT_KEY;


// 0 卡片主类型 HEX  50：消费用卡；
// 1 应用子类型 HEX  "01：普通钱包卡 A3：成人卡 A4：学生卡     A5：老年人票卡 A6：残疾人票卡 A7：公务员票卡 A8：家属学生卡"
// 2～5  应用卡编号 BCD  计算TAC过程中需要用到
// 6～8  售卡日期  BCD  年、月、日（yymmdd）
// 9～10  售卡时间  BCD  时、分（hhmm）
// 11～12 售卡地点  BCD  售卡地点编号
// 13 售卡押金  HEX  元（0.00）
// 14 启用标志  HEX  "AA：未启用；BB：启用；当CPU仿M1时=02未启用需要到CPU卡流程"
// 15 校验码  HEX  CRC8

typedef struct
{
//==================================================
    unsigned char szbalance[4];
//==================================================
    unsigned char type_major;//卡片主类型 50消费用卡
    unsigned char type_minjor;//应用子类型
    // 01：普通钱包卡
    // A3：成人卡
    // A4：学生卡
    // A5：老年人票卡
    // A6：残疾人票卡
    // A7：公务员票卡
    // A8：家属学生卡
    unsigned char appserial[4]; //应用卡编号 bcd
    unsigned char sale_dt[5];//发售时间 bcd, yymmddhhnn
    unsigned char sale_addr[2]; //发售地点编号 bcd,
    unsigned char deposit;//押金,元
    unsigned char appflag;//应用标识,AA：未启用；BB：启用；=02走CPU卡流程
//===================
    unsigned char verify_dt[3]; //审核情况 yymmdd
    unsigned char pursemode; //卡内储值情况
    //0x00：卡内专用钱包、公共钱包均未启用；
    //0x01：专用钱包已启用；
    //0x02：公共钱包已启用；
    //0x03：专用钱包、公共钱包均已启用
    unsigned char city[2];  //城市代码
    unsigned char industry[2]; //行业代码
//=================================================
    unsigned char common_count[2]; //公共钱包交易次数,高位在前
    unsigned char other_count[2];  //专用钱包交易次数,高位在前
    unsigned char dt_trdate[4]; //交易日期yyyy-mm-dd
    unsigned char lock_flag; //04锁卡，其它正常
//=================================================
    unsigned char tr_type;//交易类型
    unsigned char servicer_code[2];//服务商代码
    unsigned char terminal_no[4];   //POS编号 4BYTE;
    unsigned char dt_trdate2[3]; //过程历史交易日期yy-mm-dd
    unsigned char banlance[3];  //交易前余额
    unsigned char tr_amount[2];    //交易金额
//==================================================
    unsigned char metro_inf[14]; //不需要CRC16;



}TOUTPUT, *LPTOUTPUT;

typedef struct
{
//==================================================
    unsigned char szbalance[4];
	unsigned char szbalance2[4];
//==================================================
    unsigned char type_major;//卡片主类型 50消费用卡
    unsigned char type_minjor;//应用子类型
    // 01：普通钱包卡
    // A3：成人卡
    // A4：学生卡
    // A5：老年人票卡
    // A6：残疾人票卡
    // A7：公务员票卡
    // A8：家属学生卡
    unsigned char appserial[4]; //应用卡编号 bcd
    unsigned char sale_dt[5];//发售时间 bcd, yymmddhhnn
    unsigned char sale_addr[2]; //发售地点编号 bcd,
    unsigned char deposit;//押金,元
    unsigned char appflag;//应用标识,AA：未启用；BB：启用；=02走CPU卡流程
//===================
    unsigned char verify_dt[3]; //审核情况 yymmdd
    unsigned char pursemode; //卡内储值情况
    //0x00：卡内专用钱包、公共钱包均未启用；
    //0x01：专用钱包已启用；
    //0x02：公共钱包已启用；
    //0x03：专用钱包、公共钱包均已启用
    unsigned char city[2];  //城市代码
    unsigned char industry[2]; //行业代码
//=================================================
    unsigned char common_count[2]; //公共钱包交易次数,高位在前
    unsigned char other_count[2];  //专用钱包交易次数,高位在前
    unsigned char dt_trdate[4]; //交易日期yyyy-mm-dd
    unsigned char lock_flag; //04锁卡，其它正常
//=================================================
    unsigned char tr_type;//交易类型
    unsigned char servicer_code[2];//服务商代码
    unsigned char terminal_no[4];   //POS编号 4BYTE;
    unsigned char dt_trdate2[3]; //过程历史交易日期yy-mm-dd
    unsigned char banlance[3];  //交易前余额
    unsigned char tr_amount[2];    //交易金额
//==================================================
    unsigned char metro_inf[14]; //不需要CRC16;



}TOUTPUT2, *LPTOUTPUT2;


//数据块结构定义
//发行区
typedef struct
{
    unsigned char type_major;//卡片主类型 50消费用卡
    unsigned char type_minjor;//应用子类型
    // 01：普通钱包卡
    // A3：成人卡
    // A4：学生卡
    // A5：老年人票卡
    // A6：残疾人票卡
    // A7：公务员票卡
    // A8：家属学生卡
    unsigned char appserial[4]; //应用卡编号 bcd
    unsigned char sale_dt[5];//发售时间 bcd, yymmddhhnn
    unsigned char sale_addr[2]; //发售地点编号 bcd,
    unsigned char deposit;//押金,元
    unsigned char appflag;//应用标识,AA：未启用；BB：启用；=02走CPU卡流程
    unsigned char crc8;
} TOCT_BLK_ISSUE_DETAIL, *PTOCT_BLK_ISSUE_DETAIL;

//应用索引区
typedef struct
{
    unsigned char verify_dt[3]; //审核情况 yymmdd
    unsigned char pursemode;    //卡内储值情况
    //0x00：卡内专用钱包、公共钱包均未启用；
    //0x01：专用钱包已启用；
    //0x02：公共钱包已启用；
    //0x03：专用钱包、公共钱包均已启用
    unsigned char city[2];      //城市代码
    unsigned char industry[2];  //行业代码
    unsigned char cardmac[4];//卡认证码
    unsigned char rfu[3];
    unsigned char crc8;
} TOCT_BLK_ADDTIONALISSUE_DETAIL, *PTOCT_BLK_ADDTIONALISSUE_DETAIL;



//公共钱包
typedef struct
{
    unsigned char purse1[4];
    unsigned char purse2[4];
    unsigned char purse3[4];
    unsigned char addr[4];
} TOCT_BLK_COMMON_PURSE_DETAIL, *PTOCT_BLK_COMMON_PURSE_DETAIL;

//公共信息块
typedef struct
{
    unsigned char proc_flag;//过程标识
    //0x11：公共钱包交易开始；
    //0x12：专用钱包交易开始；
    //0x13：公共钱包交易结束；
    //0x14：专用钱包交易结束；
    unsigned char tr_type;  //交易类型
    //0x01：公共钱包充值交易；
    //0x02：专用钱包充值交易；
    //0x03：公共钱包消费交易；
    //0x04：专用钱包消费交易；
    unsigned char tr_pt;    //交易指针
    unsigned char common_count[2]; //公共钱包交易次数,高位在前
    unsigned char other_count[2];  //专用钱包交易次数,高位在前
    unsigned char dt_trdate[4]; //交易日期yyyy-mm-dd
    unsigned char lock_flag;    //04锁卡，其它正常
    unsigned char rfu[3];
    unsigned char crc8;         //校验
} TOCT_BLK_PUBLIC_DETAIL, *PTOCT_BLK_PUBLIC_DETAIL;

//历史数据块
typedef struct
{
    unsigned char tr_type;//交易类型
    unsigned char servicer_code[2];//服务商代码
    unsigned char terminal_no[4];     //POS编号 4BYTE;
    unsigned char dt_trdate[3]; //交易日期yy-mm-dd
    unsigned char banlance[3];   //交易前余额
    unsigned char tr_amount[2];    //交易金额
    unsigned char crc8;         //校验
} TOCT_BLK_HISTORY_DETAIL, *PTOCT_BLK_HISTORY_DETAIL;


//地铁数据块
typedef struct
{
unsigned char verison:
    4;//卡版本
unsigned char device_type:
    4;//设备类型
unsigned char status:
    4; //卡状态
unsigned char device_code_hi:
    4;//设备编号高位
    unsigned char device_code_lo;  //设备编号低位
    unsigned char lst_station[2];//最后一次操作线路站点
    unsigned char tm4[4];//最后一次操作时间
    unsigned char lst_entry[2];//最后一次进站线路站点
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
//  钱包有公共钱包和专用钱包，本结构中定义2为专用钱包
    unsigned long lpurse_common_org;//钱包正本值
    unsigned long lpurse_common_bak;//钱包副本值

    unsigned long lpurse_other_org;//钱包正本值
    unsigned long lpurse_other_bak;//钱包副本值

//
//     unsigned short ntr_count_org;//公共信息区公共钱包正本票卡交易计数值
//     unsigned short ntr_count_bak;//公共信息区公共钱包副本票卡交易计数值
//
//
//     unsigned short ntr_count_other_org;//公共信息区专用钱包正本票卡交易计数值
//     unsigned short ntr_count_other_bak;//公共信息区专用钱包副本票卡交易计数值

    unsigned char lst_tr_type;//上次交易类型


//发行区数据在处理过程中只需要部分数据，为了节省空间，不对其进行固定分配

    OCT_BLK_ISSUESUE blk_issue;
    OCT_BLK_ADDTIONALISSUE blk_addtional;

    OCT_BLK_PURSE blk_purse_common_org;  //公共钱包块正本
    OCT_BLK_PURSE blk_purse_common_bak;  //公共钱包块副本

    OCT_BLK_PURSE blk_purse_other_org;  //专用钱包块正本
    OCT_BLK_PURSE blk_purse_other_bak;  //专用钱包块副本


//公共钱包和专用钱包共用公共信息和历史区
    OCT_BLK_PUBLIC blk_public_org;    //公共信息正本
    OCT_BLK_PUBLIC blk_public_bak;    //公共信息副本
    OCT_BLK_HISTORY blk_history;      //历史数据块
    OCT_BLK_METRO blk_metro_org; //地铁信息区正本
    OCT_BLK_METRO blk_metro_bak; //地铁信息区副本

//    OCT_BLK_HISTORY blk_newhistory;      //新历史数据块，将要写入的数据
}  TOCT_INF, *PTOCT_INF;

typedef struct
{
    unsigned char tr_type;//交易类型
    unsigned char tr_datetime[7];//交易时间
    unsigned char before_balance[4];//交易前余额
    unsigned char tr_money[2];//交易金额
    unsigned char tr_count[2];//票卡计数
//    unsigned char padding[7];//固定为80 00 00 00 00 00 00
} TOCT_TAC, *PTOCT_TAC; //des初始化卡号取正常顺列



typedef struct
{
    unsigned char physicalnumber[4];//物理卡号
    unsigned char logiccardnumber[8];

    unsigned char currsector;//当前已认证扇区号
} TOCT_CURRINF, *PTOCT_CURRINF;


struct Err_Info
{
    unsigned char szSnr[4];
    unsigned long ulRemain;
};


//M1卡断点信息
typedef struct
{
    unsigned char Logicno[8];//逻辑卡号
    unsigned char npursetype;//钱包类型0=公用钱包, 1=专用钱包
    unsigned long balance_before;//消费前余额
    unsigned long balance_after;//消费后余额
} BREAKINF;


TOCT_KEY g_octkeyinf; //当前OCT  的key 及相关信息

TOCT_CURRINF g_octcurrinf;//当前操作票卡的动态信息

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
//函 数 名: crc_ck
//功    能: 校验数据块的crc8的值是否正确
//输入参数:
//          string_cu:需要计算的数据块指针
//             length:数据总长度，包括要填充CRC8的一个字节
//出口参数:
//   string_ck:带有crc8值的数组
//返 回 值:
//          =0正确
//         !=0错误
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
//函 数 名: crc_cu
//功    能: 计算数据块的crc8的值并进行填充
//输入参数:
//          string_cu:需要计算的数据块指针
//             length:数据总长度，包括要填充CRC8的一个字节
//出口参数:
//   string_cu:填充了crc8值的数据数组
//返 回 值:
//          无
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
//函 数 名: crc_c
//功    能: crc8的计算
//输入参数:
//          x:需要计算crc8的字节数据
//       temp:公共预置值
//出口参数:
//   无
//返 回 值:
//          计算的CRC8值
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
//函 数 名: crc_ck
//功    能: 校验数据块的crc8的值是否正确
//输入参数:
//          string_cu:需要计算的数据块指针
//             length:数据总长度，包括要填充CRC8的一个字节
//出口参数:
//   string_ck:带有crc8值的数组
//返 回 值:
//          =0正确
//         !=0错误
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
//函 数 名: crc_cu
//功    能: 计算数据块的crc8的值并进行填充
//输入参数:
//          string_cu:需要计算的数据块指针
//             length:数据总长度，包括要填充CRC8的一个字节
//出口参数:
//   string_cu:填充了crc8值的数据数组
//返 回 值:
//          无
//========================================================================
void crc_cu(unsigned char *string_cu,unsigned char length)
{
    unsigned char temp=0;
    unsigned char j;
    for ( j=0;j<(length-1);j++)temp=crc_c(string_cu[j],temp);
    string_cu[j]=temp;
}

//==========================================================================================
//函 数 名: octsam_chk
//功    能: 检查SAM卡是否存在，以放正常初始华后被拔掉造成M1卡可扣费但不能计算TAC的情况
//入口参数:
//          idx_sam:当前应用SAM卡对应的卡槽序列号(从0开始)
//出口参数:
//             无
//返 回 值:
//          0=sam卡存在
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

        //计算KEYA===============================
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

        //计算KEYB=========================================
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
        //如果区块已认证，下次直接进行读写操作
        g_octcurrinf.currsector = currsector;
    }

    return ret;


}

//ndest和nsrc必须在一个扇区号内
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

        ret = ISO14443A_Transfer(ndest); //钱包副本调用
    }

    if (ret==MI_OK)
    {
        //如果区块已认证，下次直接进行读写操作
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
        ret = ISO14443A_Restore(blk_idx); //钱包正本调用
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
        ret = ISO14443A_Transfer(blk_idx); //钱包正本传值
    }

    if (ret==MI_OK)
    {
        //如果区块已认证，下次直接进行读写操作
        g_octcurrinf.currsector = currsector;
    }

    return ret;

}

//===========================================
//数据块校验处理部分

//判断数据块是否无效，无效返回非0值
unsigned char octm1_verify_block(unsigned char *lpdata)
{
    unsigned char ret=1;
    if (crc_ck(lpdata,16)==0)
    {
        ret = 0;//=0正确
    }

    return ret;
}



//判断钱包是否无效，无效返回非0值
unsigned char octm1_verify_purse(OCT_BLK_PURSE *pblk)
{
    unsigned char i;
    unsigned char ret=0;

    for (i=0; i<4; i++)
    {
        // 判断正本钱包合法
        if (pblk->buf[i] != 0xff - pblk->buf[i+4])
        {
            //非法
            ret = 1;
            //break;
        }
        if (pblk->buf[i] != pblk->buf[i+8])
        {
            //非法
            ret = 1;
            //break;
        }

    }
    if ((pblk->buf[12]+pblk->buf[13])!=(pblk->buf[14]+pblk->buf[15]))
        //if (memcmp(pblk->buf+12, "\x00\xFF\x00\xFF", 4))
    {
        //非法
        ret = 1;
    }

    return ret;
}


//判发行区块是否无效，无效返回非0值
unsigned char octm1_verify_issue(OCT_BLK_ISSUESUE *pblk)
{
    unsigned char ret=1;
    if (crc_ck(pblk->buf,16)==0)
    {
        ret = 0;//=0正确
    }
    return ret;
}

//判发行区块是否无效，无效返回非0值
unsigned char octm1_verify_addtionalissue(OCT_BLK_ADDTIONALISSUE *pblk)
{
    unsigned char ret=1;
    if (crc_ck(pblk->buf,16)==0)
    {
        ret = 0;//=0正确
    }
    return ret;
}



//判公共信息块是否无效，无效返回非0值
unsigned char octm1_verify_public(OCT_BLK_PUBLIC *pblk)
{
    unsigned char ret=1;
    if (crc_ck(pblk->buf,16)==0)
    {
        ret = 0;//=0正确
    }
    return ret;
}


//判公共信息块是否无效，无效返回非0值
unsigned char octm1_verify_history(OCT_BLK_HISTORY *pblk)
{
    unsigned char ret=1;
    if (crc_ck(pblk->buf,16)==0)
    {
        ret = 0;//=0正确
    }
    return ret;
}

//判地铁信息块是否无效，无效返回非0值
unsigned char octm1_verify_metro(OCT_BLK_METRO *pblk)
{
    unsigned char ret=1;
    if (crc16_ck(pblk->buf,16)==0)
    {
        ret = 0;//=0正确
    }
    return ret;
}


//============数据校验部分结束==================
//===================================================



//============================================================================
//对公共信息区中对应的钱包计数进行累计，=0为公共钱包，=1为专用钱包
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
//函 数 名: octm1_metro_recover
//功    能: 地铁信息区恢复流程，嵌套在钱包恢复流程中
//入口参数:
//          direction:恢复方向, 只有在正副本不等且都有效的情况才会用到,一对一错的情况是用对的覆盖错的
//                    0=自动恢复方向
//                    1=向后恢复, 用正本覆盖副本
//                    2=向前恢复, 用副本覆盖正本
//            poctinf:包含有地铁信息块的数据结构
//出口参数:
//             无
//返 回 值:
//          0=恢复成功
//==========================================================================================

unsigned char octm1_metro_recover(unsigned char direction, PTOCT_INF poctinf, char metro_recover_flag)
{
    unsigned char metro_org_invalid = 0; //钱包正本是否无效, 非0值为无效
    unsigned char metro_bak_invalid = 0; //钱包副本是否无效, 非0值为无效
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
            //地铁信息区正副本都错，赋成全0状态
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
            //正本无效, 用副本覆盖正本
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
            //副本无效, 用正本覆盖副本
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
            //正副本都有效,只处理正副本不等的情况
            if (memcmp(poctinf->blk_metro_bak.buf, poctinf->blk_metro_bak.buf, 16))
            {
                if ((direction==0) || (direction==1))
                {
                    //向后恢复, 正本覆盖副本
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
                    //向前恢复, 副本覆盖正本
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
//专用钱包恢复
unsigned char octm1_other_purse_recover_a0(PTOCT_INF poctinf)
{
    //过程B0
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
    //过程B1
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
    //过程B2
// dbg_formatvar("PROCESS==B2\n");
// dbg_formatvar("%ld, %ld\n",poctinf->lpurse_other_org,poctinf->lpurse_other_bak);

    if (poctinf->lpurse_other_org != poctinf->lpurse_other_bak)
    {
//  dbg_formatvar("PROCESS==B21\n");
        //钱包正正副本不等, 表示扣费成功, 向后恢复
        //钱包本覆盖副本，公共信息区正本覆盖副本
        //正本指针已累加过，所以只需将进程标识置为2  即可
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
        //钱包正正副本相等, 表示扣费失败, 向前恢复
        //公共信息区副本覆盖正本
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
    //过程B3 , 从b0中分离出来
// dbg_formatvar("PROCESS==B3\n");

    if ((poctinf->lst_tr_type==0x01) || (poctinf->lst_tr_type==0x03))
    {
        //上次为公用钱包
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
        //上次为专用钱包
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
    //过程B4,专用钱包完成消费
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
    //过程B5
// dbg_formatvar("PROCESS==B5\n");
// printf("%ld, %ld\n",poctinf->lpurse_other_org,poctinf->lpurse_other_bak);
    if (poctinf->lpurse_common_org != poctinf->lpurse_common_bak)
    {
//   dbg_formatvar("PROCESS==B51\n");
        //钱包消费完成,向后恢复
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
        //公用钱包扣费未完成,向前恢复
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

    unsigned char purse_org_invalid = 0; //钱包正本是否无效, 非0值为无效
    unsigned char purse_bak_invalid = 0; //钱包副本是否无效, 非0值为无效

    unsigned char purse_other_org_invalid = 0; //钱包正本是否无效, 非0值为无效
    unsigned char purse_other_bak_invalid = 0; //钱包副本是否无效, 非0值为无效


    unsigned char public_org_invalid = 0;//公共信息区是否无效,非0值为无效
    unsigned char public_bak_invalid = 0;//公共信息区是否无效,非0值为无效



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

    //钱包正副本都错或公共信息区都错则报错
    if ((purse_other_org_invalid && purse_other_bak_invalid))// || (public_org_invalid && public_bak_invalid))
    {
//  dbg_formatvar("E5 = %02X %02X %02X %02X\n",purse_org_invalid,purse_bak_invalid,public_org_invalid,public_bak_invalid);
        return 0xE5; //票卡数据错误，不能恢复
    }

    //钱包正副本都错或公共信息区都错则报错
    if (public_org_invalid && public_bak_invalid)
    {
		// 不论何种情况都恢复，注释if模块，20140820－wxf
        //if (memcmp(poctinf->blk_public_org.buf, "\x00\x00\x00\x00", 4))
        //{
        //    return 0xE5; //票卡数据错误，不能恢复
        //}
        //else
        {
            //修复，因为公共信息有可能被发成钱包结构
            public_org_invalid = 0;
            public_bak_invalid = 0;
            memset(poctinf->blk_public_org.buf, 0x00, 16);
            memset(poctinf->blk_public_bak.buf, 0x00, 16);
            crc_cu(poctinf->blk_public_org.buf, 16);
            crc_cu(poctinf->blk_public_bak.buf, 16);
        }
    }


    //如果专用钱包是一对一错，则将错的进行恢复，这在后续可以少判断一种条件
    if (purse_other_org_invalid)
    {
		if (octm1_metro_recover(2, poctinf, metro_recover_flag))
		{
			return 0xA8;
		}

        //钱包副本到正本
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
        //钱包正本到副本
        if (octm1_copy_purse(OCT_BLK_PURSE_OTHER_BAK_IDX, OCT_BLK_PURSE_OTHER_ORG_IDX))
        {
            return 0xA8;
        }
        poctinf->lpurse_other_bak = poctinf->lpurse_other_org;
        memcpy(poctinf->blk_purse_other_bak.buf, poctinf->blk_purse_other_org.buf, 16);

    }
    else
    {
        //钱包正副本都合法但不等的情况只需要处理闪卡情况
        if (poctinf->lpurse_other_org != poctinf->lpurse_other_bak)
        {
            if (memcmp(g_octcurrinf.logiccardnumber, g_m1recover.Logicno, 4)==0)
            {


				if (g_m1recover.npursetype)
				{
					//上次是公用钱包闪卡
	                if (poctinf->lpurse_common_org == g_m1recover.balance_after)
	                {
						if (octm1_metro_recover(2, poctinf, metro_recover_flag))
						{
							return 0xA8;
						}

	                    //上次发生闪卡，金额改变，将金额恢复到原始状态
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



    //如果钱包是一对一错，则将错的进行恢复，这在后续可以少判断一种条件
    if (purse_org_invalid)
    {

        //钱包副本到正本
        if (octm1_copy_purse(OCT_BLK_PURSE_ORG_IDX, OCT_BLK_PURSE_BAK_IDX))
        {
            return 0xA8;
        }
        poctinf->lpurse_common_org = poctinf->lpurse_common_bak;
        memcpy(poctinf->blk_purse_common_org.buf, poctinf->blk_purse_common_bak.buf, 16);

    }
    else if (purse_bak_invalid)
    {

        //钱包正本到副本
        if (octm1_copy_purse(OCT_BLK_PURSE_BAK_IDX, OCT_BLK_PURSE_ORG_IDX))
        {
            return 0xA8;
        }
        poctinf->lpurse_common_bak = poctinf->lpurse_common_org;
        memcpy(poctinf->blk_purse_common_bak.buf, poctinf->blk_purse_common_org.buf, 16);

    }
    else
    {
        //钱包正副本都合法但不等的情况只需要处理闪卡情况
        if (poctinf->lpurse_common_org != poctinf->lpurse_common_bak)
        {
            if (memcmp(g_octcurrinf.logiccardnumber, g_m1recover.Logicno, 4)==0)
            {


				if (g_m1recover.npursetype==0)
				{
					//上次是公用钱包闪卡
	                if (poctinf->lpurse_common_org == g_m1recover.balance_after)
	                {
						if (octm1_metro_recover(2, poctinf, metro_recover_flag))
						{
							return 0xA8;
						}

	                    //上次发生闪卡，金额改变，将金额恢复到原始状态
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
        //公共信息区正本无效
        //有两种可能，
        //1(向前恢复)是还没操作钱包时写错, 这时钱包正副本是相等的,只需将公共信息区副本复制到正本
        //2(向后恢复)是操作完钱包正本后写错, 这时钱包正本已改完, 需将公共信息区副本累计后复制到正本并写入
        status = octm1_other_purse_recover_a3(poctinf);
        if (status) return(status);
    }
    else if (public_bak_invalid)
    {
        //过程A0
        status = octm1_other_purse_recover_a0(poctinf);
        if (status) return(status);
    }
    else
    {
        //公共信息正副本都正确的有两种情况
        if (memcmp(poctinf->blk_public_org.buf, poctinf->blk_public_bak.buf, 16))
        {
            // 1公共信息正副不等
            //判断公共信息区正本进程标识=1 或=2
            if (poctinf->blk_public_org.detail.proc_flag == 0x11)
            {
                //过程B2
                //dbg_formatvar("B2\n");
                status = octm1_other_purse_recover_a5(poctinf);
                if (status) return(status);
            }
            else if (poctinf->blk_public_org.detail.proc_flag == 0x13)
            {
                //过程B1
                //dbg_formatvar("B1\n");
                status = octm1_other_purse_recover_a4(poctinf);
                if (status) return(status);

            }
            else if (poctinf->blk_public_org.detail.proc_flag == 0x14)
            {
                //过程B4
                //dbg_formatvar("B4\n");
                status = octm1_other_purse_recover_a1(poctinf);
                if (status) return(status);

            }
            else if (poctinf->blk_public_org.detail.proc_flag == 0x12)
            {
                //过程B5
                //dbg_formatvar("B5\n");
                status = octm1_other_purse_recover_a2(poctinf);
                if (status) return(status);

            }

        }
        else
        {
            // 2公共信息正副本相等
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
    //过程B0
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
    //过程B1
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
    //过程B2
// dbg_formatvar("PROCESS==B2\n");
// dbg_formatvar("%ld, %ld\n",poctinf->lpurse_other_org,poctinf->lpurse_other_bak);

    if (poctinf->lpurse_common_org != poctinf->lpurse_common_bak)
    {
//  dbg_formatvar("PROCESS==B21\n");
        //钱包正正副本不等, 表示扣费成功, 向后恢复
        //钱包本覆盖副本，公共信息区正本覆盖副本
        //正本指针已累加过，所以只需将进程标识置为2  即可
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
        //钱包正正副本相等, 表示扣费失败, 向前恢复
        //公共信息区副本覆盖正本
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
    //过程B3 , 从b0中分离出来
// dbg_formatvar("PROCESS==B3\n");

    if ((poctinf->lst_tr_type==0x01) || (poctinf->lst_tr_type==0x03))
    {
        //上次为公用钱包
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
        //上次为专用钱包
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
    //过程B4,专用钱包完成消费
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
    //过程B5
// dbg_formatvar("PROCESS==B5\n");
// printf("%ld, %ld\n",poctinf->lpurse_other_org,poctinf->lpurse_other_bak);
    if (poctinf->lpurse_other_org != poctinf->lpurse_other_bak)
    {
//   dbg_formatvar("PROCESS==B51\n");
        //钱包消费完成,向后恢复
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
        //专用钱包扣费未完成,向前恢复
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

    unsigned char purse_org_invalid = 0; //钱包正本是否无效, 非0值为无效
    unsigned char purse_bak_invalid = 0; //钱包副本是否无效, 非0值为无效

    unsigned char purse_other_org_invalid = 0; //钱包正本是否无效, 非0值为无效
    unsigned char purse_other_bak_invalid = 0; //钱包副本是否无效, 非0值为无效


    unsigned char public_org_invalid = 0;//公共信息区是否无效,非0值为无效
    unsigned char public_bak_invalid = 0;//公共信息区是否无效,非0值为无效



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

    //钱包正副本都错或公共信息区都错则报错
    if ((purse_org_invalid && purse_bak_invalid))// || (public_org_invalid && public_bak_invalid))
    {
//  dbg_formatvar("E5 = %02X %02X %02X %02X\n",purse_org_invalid,purse_bak_invalid,public_org_invalid,public_bak_invalid);
        return 0xE5; //票卡数据错误，不能恢复
    }

    //钱包正副本都错或公共信息区都错则报错
    if (public_org_invalid && public_bak_invalid)
    {
		// 不论何种情况都恢复，注释if模块，20140820－wxf
        //if (memcmp(poctinf->blk_public_org.buf, "\x00\x00\x00\x00", 4))
        //{
        //    return 0xE5; //票卡数据错误，不能恢复
        //}
        //else
        {
            //修复，因为公共信息有可能被发成钱包结构
            public_org_invalid = 0;
            public_bak_invalid = 0;
            memset(poctinf->blk_public_org.buf, 0x00, 16);
            memset(poctinf->blk_public_bak.buf, 0x00, 16);
            crc_cu(poctinf->blk_public_org.buf, 16);
            crc_cu(poctinf->blk_public_bak.buf, 16);
        }
    }


    //如果钱包是一对一错，则将错的进行恢复，这在后续可以少判断一种条件
    if (purse_org_invalid)
    {
		if (octm1_metro_recover(2, poctinf, metro_recover_flag))
		{
			return 0xA8;
		}

        //钱包副本到正本
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


        //钱包正本到副本
        if (octm1_copy_purse(OCT_BLK_PURSE_BAK_IDX, OCT_BLK_PURSE_ORG_IDX))
        {
            return 0xA8;
        }
        poctinf->lpurse_common_bak = poctinf->lpurse_common_org;
        memcpy(poctinf->blk_purse_common_bak.buf, poctinf->blk_purse_common_org.buf, 16);

    }
    else
    {
        //钱包正副本都合法但不等的情况只需要处理闪卡情况
        if (poctinf->lpurse_common_org != poctinf->lpurse_common_bak)
        {
            if (memcmp(g_octcurrinf.logiccardnumber, g_m1recover.Logicno, 4)==0)
            {


				if (g_m1recover.npursetype==0)
				{
					//上次是公用钱包闪卡
	                if (poctinf->lpurse_common_org == g_m1recover.balance_after)
	                {
						if (octm1_metro_recover(2, poctinf, metro_recover_flag))
						{
							return 0xA8;
						}

	                    //上次发生闪卡，金额改变，将金额恢复到原始状态
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


    //如果专用钱包是一对一错，则将错的进行恢复，这在后续可以少判断一种条件
    if (purse_other_org_invalid)
    {
        //钱包副本到正本
        if (octm1_copy_purse(OCT_BLK_PURSE_OTHER_ORG_IDX, OCT_BLK_PURSE_OTHER_BAK_IDX))
        {
            return 0xA8;
        }
        poctinf->lpurse_other_org = poctinf->lpurse_other_bak;
        memcpy(poctinf->blk_purse_other_org.buf, poctinf->blk_purse_other_bak.buf, 16);

    }
    else if (purse_other_bak_invalid)
    {

        //钱包正本到副本
        if (octm1_copy_purse(OCT_BLK_PURSE_OTHER_BAK_IDX, OCT_BLK_PURSE_OTHER_ORG_IDX))
        {
            return 0xA8;
        }
        poctinf->lpurse_other_bak = poctinf->lpurse_other_org;
        memcpy(poctinf->blk_purse_other_bak.buf, poctinf->blk_purse_other_org.buf, 16);

    }
    else
    {
        //钱包正副本都合法但不等的情况只需要处理闪卡情况
        if (poctinf->lpurse_other_org != poctinf->lpurse_other_bak)
        {
            if (memcmp(g_octcurrinf.logiccardnumber, g_m1recover.Logicno, 4)==0)
            {


				if (g_m1recover.npursetype)
				{
					//上次是公用钱包闪卡
	                if (poctinf->lpurse_other_org == g_m1recover.balance_after)
	                {
						if (octm1_metro_recover(2, poctinf, metro_recover_flag))
						{
							return 0xA8;
						}

	                    //上次发生闪卡，金额改变，将金额恢复到原始状态
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
        //公共信息区正本无效
        //有两种可能，
        //1(向前恢复)是还没操作钱包时写错, 这时钱包正副本是相等的,只需将公共信息区副本复制到正本
        //2(向后恢复)是操作完钱包正本后写错, 这时钱包正本已改完, 需将公共信息区副本累计后复制到正本并写入
        status = octm1_common_purse_recover_b3(poctinf);
        if (status) return(status);
    }
    else if (public_bak_invalid)
    {
        //过程B0
        status = octm1_common_purse_recover_b0(poctinf);
        if (status) return(status);
    }
    else
    {
        //公共信息正副本都正确的有两种情况
        if (memcmp(poctinf->blk_public_org.buf, poctinf->blk_public_bak.buf, 16))
        {
            // 1公共信息正副不等
            //判断公共信息区正本进程标识=1 或=2
            if (poctinf->blk_public_org.detail.proc_flag == 0x11)
            {
                //过程B2
                //dbg_formatvar("B2\n");
                status = octm1_common_purse_recover_b2(poctinf);
                if (status) return(status);
            }
            else if (poctinf->blk_public_org.detail.proc_flag == 0x13)
            {
                //过程B1
                //dbg_formatvar("B1\n");
                status = octm1_common_purse_recover_b1(poctinf);
                if (status) return(status);

            }
            else if (poctinf->blk_public_org.detail.proc_flag == 0x14)
            {
                //过程B4
                //dbg_formatvar("B4\n");
                status = octm1_common_purse_recover_b4(poctinf);
                if (status) return(status);

            }
            else if (poctinf->blk_public_org.detail.proc_flag == 0x12)
            {
                //过程B5
                //dbg_formatvar("B5\n");
                status = octm1_common_purse_recover_b5(poctinf);
                if (status) return(status);

            }

        }
        else
        {
            // 2公共信息正副本相等
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


//在读卡信息时对以下三个变量进行填充
unsigned char g_public_inf[16];//写卡用到
//unsigned char g_history_inf[16];//写卡用到
unsigned long g_nbalance;//消费前公用钱包余额，写卡用到
unsigned long g_nbalance2;//消费前专用钱包余额，写卡用到

//==========================================================================================
//函 数 名: octm1_getdata
//功    能: 获取m1卡关键数据
//入口参数:
//          idx_sam:当前应用SAM卡对应的卡槽序列号(从0开始)
//     lpcardnumber:寻卡过程中获取的4字节的物理卡号
//出口参数:
//          lprev:取得的数据内容,内容如下
//					//==================================================
//					    unsigned char szbalance[4];
//					//==================================================
//					    unsigned char type_major;//卡片主类型 50消费用卡
//					    unsigned char type_minjor;//应用子类型
//					                              // 01：普通钱包卡
//					                              // A3：成人卡
//					                              // A4：学生卡
//					                              // A5：老年人票卡
//					                              // A6：残疾人票卡
//					                              // A7：公务员票卡
//					                              // A8：家属学生卡
//					    unsigned char appserial[4]; //应用卡编号 bcd
//					    unsigned char sale_dt[5];//发售时间 bcd, yymmddhhnn
//					    unsigned char sale_addr[2]; //发售地点编号 bcd,
//					    unsigned char deposit;//押金,元
//					    unsigned char appflag;//应用标识,AA：未启用；BB：启用；=02走CPU卡流程
//					//===================
//					    unsigned char verify_dt[3]; //审核情况 yymmdd
//					    unsigned char pursemode; //卡内储值情况
//					                             //0x00：卡内专用钱包、公共钱包均未启用；
//					                             //0x01：专用钱包已启用；
//					                             //0x02：公共钱包已启用；
//					                             //0x03：专用钱包、公共钱包均已启用
//					    unsigned char city[2];  //城市代码
//					    unsigned char industry[2]; //行业代码
//					//=================================================
//					    unsigned char common_count[2]; //公共钱包交易次数,高位在前
//					    unsigned char other_count[2];  //专用钱包交易次数,高位在前
//					    unsigned char dt_trdate[4]; //交易日期yyyy-mm-dd
//					    unsigned char lock_flag; //04锁卡，其它正常
//					//=================================================
//					    unsigned char tr_type;//交易类型
//					    unsigned char servicer_code[2];//服务商代码
//					    unsigned char terminal_no[4];   //POS编号 4BYTE;
//					    unsigned char dt_trdate2[3]; //过程历史交易日期yy-mm-dd
//					    unsigned char banlance[3];  //交易前余额
//					    unsigned char tr_amount[2];    //交易金额
//					//==================================================
//						unsigned char metro_inf[14]; //不需要CRC16;


//返 回 值:
//          0=成功
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
//        //导出密钥
//        //密钥在初始化SAM卡时已导出
////        ret = octsam_exportkey(OCTM1SAM_SOCKET);
////        if (ret) break;
//		if (octsam_chk(idx_sam))
//		{
//			ret = 0xE5;
//			break;
//		}
//
//        //发行数据块
//        ret = octm1_readblock(OCT_BLK_ISSUESUE_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_issue.buf, tmpbuf, 16);
//
//
//        //应用索引数据块
//        ret = octm1_readblock(OCT_BLK_ADDTIONALISSUE_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_addtional.buf, tmpbuf, 16);
//
//        //公共钱包正本
//        ret = octm1_readblock(OCT_BLK_PURSE_ORG_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_purse_common_org.buf, tmpbuf, 16);
//
//
//        //公共钱包副本
//        ret = octm1_readblock(OCT_BLK_PURSE_BAK_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_purse_common_bak.buf, tmpbuf, 16);
//
//        //专用钱包正本
//        ret = octm1_readblock(OCT_BLK_PURSE_OTHER_ORG_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_purse_other_org.buf, tmpbuf, 16);
//
//        //专用钱包副本
//        ret = octm1_readblock(OCT_BLK_PURSE_OTHER_BAK_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_purse_other_bak.buf, tmpbuf, 16);
//
//        //公共信息区正本
//        ret = octm1_readblock(OCT_BLK_PUBLIC_ORG_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_public_org.buf, tmpbuf, 16);
//
//        //公共信息区副本
//        ret = octm1_readblock(OCT_BLK_PUBLIC_BAK_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_public_bak.buf, tmpbuf, 16);
//
//        //消费过程历史
//        ret = octm1_readblock(OCT_BLK_HISTORY_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_history.buf, tmpbuf, 16);
//
//        //地铁信息区正本
//        ret = octm1_readblock(OCT_BLK_METRO_ORG_IDX, tmpbuf);
//        if (ret) break;
//        memcpy(octinf.blk_metro_org.buf, tmpbuf, 16);
//
//        //地铁信息区副本
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
//        //进行恢复流程操作，以保证钱包区、公共信息区的数据是正确的
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
//        //输出赋值
//        //钱包值
//        lpdata->szbalance[0] = octinf.blk_purse_common_org.buf[3];
//        lpdata->szbalance[1] = octinf.blk_purse_common_org.buf[2];
//        lpdata->szbalance[2] = octinf.blk_purse_common_org.buf[1];
//        lpdata->szbalance[3] = octinf.blk_purse_common_org.buf[0];
////=============================================================================
//        //发行信息区数据项
//        lpdata->type_major = octinf.blk_issue.detail.type_major;//卡片主类型 50消费用卡
//        lpdata->type_minjor = octinf.blk_issue.detail.type_minjor;//应用子类型
//        lpdata->type_minjor = octinf.blk_issue.detail.type_minjor;//应用子类型
//        memcpy(lpdata->appserial, octinf.blk_issue.detail.appserial, 4); //卡应用编号
//        memcpy(lpdata->sale_dt, octinf.blk_issue.detail.sale_dt, 5);//售卡时间 yymmddhhnn
//        memcpy(lpdata->sale_addr, octinf.blk_issue.detail.sale_addr, 2);//售卡地点编号 bcd
//        lpdata->deposit = octinf.blk_issue.detail.deposit;//押金,元
//        lpdata->appflag = octinf.blk_issue.detail.appflag;//应用标识
//
//		memcpy(g_octcurrinf.logiccardnumber, lpdata->appserial, 4);
//
//
////==========================================================================================
//        //应用索引区数据项
//        memcpy(lpdata->verify_dt, octinf.blk_addtional.detail.verify_dt, 3); //审核情况 yymmdd
//        lpdata->pursemode = octinf.blk_addtional.detail.pursemode; //卡内储值情况
//        memcpy(lpdata->city, octinf.blk_addtional.detail.city, 2);  //城市代码
//        memcpy(lpdata->industry, octinf.blk_addtional.detail.industry,2); //行业代码
////=================================================
//        //公共信息区数据项
//        memcpy(lpdata->common_count, octinf.blk_public_org.detail.common_count, 2); //公共钱包交易次数,高位在前
//        memcpy(lpdata->other_count, octinf.blk_public_org.detail.other_count, 2);//[2];  //专用钱包交易次数,高位在前
//        memcpy(lpdata->dt_trdate, octinf.blk_public_org.detail.dt_trdate, 4); //[4]; //交易日期yyyy-mm-dd
//        lpdata->lock_flag = octinf.blk_public_org.detail.lock_flag; //04锁卡，其它正常
////=================================================
//        //公共信息历史块数据项
//        lpdata->tr_type = octinf.blk_history.detail.tr_type;//交易类型
//        memcpy(lpdata->servicer_code, octinf.blk_history.detail.servicer_code, 2);//服务商代码
//        memcpy(lpdata->terminal_no, octinf.blk_history.detail.terminal_no, 4);//[4];   //POS编号 4BYTE;
//        memcpy(lpdata->dt_trdate2, octinf.blk_history.detail.dt_trdate, 3);//[3]; //交易日期yy-mm-dd
//        memcpy(lpdata->banlance, octinf.blk_history.detail.banlance, 3);  //交易前余额
//        memcpy(lpdata->tr_amount, octinf.blk_history.detail.tr_amount, 2);    //交易金额
////==================================================
//        //地铁信息区数据项
//        memcpy(lpdata->metro_inf, octinf.blk_metro_org.buf, 14);
//        return(0);
//    }
//    while (0);
//
//    return(ret);
//
//}

//==========================================================================================
//函 数 名: octm1_getdata2
//功    能: 获取m1卡关键数据
//入口参数:
//          idx_sam:当前应用SAM卡对应的卡槽序列号(从0开始)
//     lpcardnumber:寻卡过程中获取的4字节的物理卡号
//出口参数:
//          lprev:取得的数据内容,内容如下
//					//==================================================
//					    unsigned char szbalance[4];   // 公共钱包
//					    unsigned char szbalance2[4]; // 专用钱包
//					//==================================================
//					    unsigned char type_major;//卡片主类型 50消费用卡
//					    unsigned char type_minjor;//应用子类型
//					                              // 01：普通钱包卡
//					                              // A3：成人卡
//					                              // A4：学生卡
//					                              // A5：老年人票卡
//					                              // A6：残疾人票卡
//					                              // A7：公务员票卡
//					                              // A8：家属学生卡
//					    unsigned char appserial[4]; //应用卡编号 bcd
//					    unsigned char sale_dt[5];//发售时间 bcd, yymmddhhnn
//					    unsigned char sale_addr[2]; //发售地点编号 bcd,
//					    unsigned char deposit;//押金,元
//					    unsigned char appflag;//应用标识,AA：未启用；BB：启用；=02走CPU卡流程
//					//===================
//					    unsigned char verify_dt[3]; //审核情况 yymmdd
//					    unsigned char pursemode; //卡内储值情况
//					                             //0x00：卡内专用钱包、公共钱包均未启用；
//					                             //0x01：专用钱包已启用；
//					                             //0x02：公共钱包已启用；
//					                             //0x03：专用钱包、公共钱包均已启用
//					    unsigned char city[2];  //城市代码
//					    unsigned char industry[2]; //行业代码
//					//=================================================
//					    unsigned char common_count[2]; //公共钱包交易次数,高位在前
//					    unsigned char other_count[2];  //专用钱包交易次数,高位在前
//					    unsigned char dt_trdate[4]; //交易日期yyyy-mm-dd
//					    unsigned char lock_flag; //04锁卡，其它正常
//					//=================================================
//					    unsigned char tr_type;//交易类型
//					    unsigned char servicer_code[2];//服务商代码
//					    unsigned char terminal_no[4];   //POS编号 4BYTE;
//					    unsigned char dt_trdate2[3]; //过程历史交易日期yy-mm-dd
//					    unsigned char banlance[3];  //交易前余额
//					    unsigned char tr_amount[2];    //交易金额
//					//==================================================
//						unsigned char metro_inf[14]; //不需要CRC16;


//返 回 值:
//          0=成功
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
        //导出密钥
        //密钥在初始化SAM卡时已导出
//        ret = octsam_exportkey(OCTM1SAM_SOCKET);
//        if (ret) break;
		if (octsam_chk(idx_sam))
		{
			ret = 0xE5;
			break;
		}

        //发行数据块
        ret = octm1_readblock(OCT_BLK_ISSUESUE_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_issue.buf, tmpbuf, 16);


        //应用索引数据块
        ret = octm1_readblock(OCT_BLK_ADDTIONALISSUE_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_addtional.buf, tmpbuf, 16);

        //公共钱包正本
        ret = octm1_readblock(OCT_BLK_PURSE_ORG_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_purse_common_org.buf, tmpbuf, 16);


        //公共钱包副本
        ret = octm1_readblock(OCT_BLK_PURSE_BAK_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_purse_common_bak.buf, tmpbuf, 16);

        //专用钱包正本
        ret = octm1_readblock(OCT_BLK_PURSE_OTHER_ORG_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_purse_other_org.buf, tmpbuf, 16);

        //专用钱包副本
        ret = octm1_readblock(OCT_BLK_PURSE_OTHER_BAK_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_purse_other_bak.buf, tmpbuf, 16);

        //公共信息区正本
        ret = octm1_readblock(OCT_BLK_PUBLIC_ORG_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_public_org.buf, tmpbuf, 16);

        //公共信息区副本
        ret = octm1_readblock(OCT_BLK_PUBLIC_BAK_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_public_bak.buf, tmpbuf, 16);

        //消费过程历史
        ret = octm1_readblock(OCT_BLK_HISTORY_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_history.buf, tmpbuf, 16);

        //地铁信息区正本
        ret = octm1_readblock(OCT_BLK_METRO_ORG_IDX, tmpbuf);
        if (ret) break;
        memcpy(octinf.blk_metro_org.buf, tmpbuf, 16);

        //地铁信息区副本
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

        //进行恢复流程操作，以保证钱包区、公共信息区的数据是正确的
        if (octinf.blk_issue.detail.type_minjor==0xA4 || octinf.blk_issue.detail.type_minjor==0xB4 || octinf.blk_issue.detail.type_minjor==0xA7)
        {
			//学生卡以专用钱包恢复为主，公用钱包恢复不需判断执行状态
			ret = octm1_other_purse_recover(&octinf, 1);
			if (ret) break;
			octm1_common_purse_recover(&octinf, 0);
        }
        else
        {
			//非学生卡以公用钱包恢复为主，专用钱包恢复不需判断执行状态
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


        //输出赋值
        //钱包值
        lpdata->szbalance[0] = octinf.blk_purse_common_org.buf[3];
        lpdata->szbalance[1] = octinf.blk_purse_common_org.buf[2];
        lpdata->szbalance[2] = octinf.blk_purse_common_org.buf[1];
        lpdata->szbalance[3] = octinf.blk_purse_common_org.buf[0];

		lpdata->szbalance2[0] = octinf.blk_purse_other_org.buf[3];
		lpdata->szbalance2[1] = octinf.blk_purse_other_org.buf[2];
		lpdata->szbalance2[2] = octinf.blk_purse_other_org.buf[1];
		lpdata->szbalance2[3] = octinf.blk_purse_other_org.buf[0];

//=============================================================================
        //发行信息区数据项
        lpdata->type_major = octinf.blk_issue.detail.type_major;//卡片主类型 50消费用卡
        lpdata->type_minjor = octinf.blk_issue.detail.type_minjor;//应用子类型
        lpdata->type_minjor = octinf.blk_issue.detail.type_minjor;//应用子类型
        memcpy(lpdata->appserial, octinf.blk_issue.detail.appserial, 4); //卡应用编号
        memcpy(lpdata->sale_dt, octinf.blk_issue.detail.sale_dt, 5);//售卡时间 yymmddhhnn
        memcpy(lpdata->sale_addr, octinf.blk_issue.detail.sale_addr, 2);//售卡地点编号 bcd
        lpdata->deposit = octinf.blk_issue.detail.deposit;//押金,元
        lpdata->appflag = octinf.blk_issue.detail.appflag;//应用标识

		memcpy(g_octcurrinf.logiccardnumber, lpdata->appserial, 4);


//==========================================================================================
        //应用索引区数据项
        memcpy(lpdata->verify_dt, octinf.blk_addtional.detail.verify_dt, 3); //审核情况 yymmdd
        lpdata->pursemode = octinf.blk_addtional.detail.pursemode; //卡内储值情况
        memcpy(lpdata->city, octinf.blk_addtional.detail.city, 2);  //城市代码
        memcpy(lpdata->industry, octinf.blk_addtional.detail.industry,2); //行业代码
//=================================================
        //公共信息区数据项
        memcpy(lpdata->common_count, octinf.blk_public_org.detail.common_count, 2); //公共钱包交易次数,高位在前
        memcpy(lpdata->other_count, octinf.blk_public_org.detail.other_count, 2);//[2];  //专用钱包交易次数,高位在前
        memcpy(lpdata->dt_trdate, octinf.blk_public_org.detail.dt_trdate, 4); //[4]; //交易日期yyyy-mm-dd
        lpdata->lock_flag = octinf.blk_public_org.detail.lock_flag; //04锁卡，其它正常
//=================================================
        //公共信息历史块数据项
        lpdata->tr_type = octinf.blk_history.detail.tr_type;//交易类型
        memcpy(lpdata->servicer_code, octinf.blk_history.detail.servicer_code, 2);//服务商代码
        memcpy(lpdata->terminal_no, octinf.blk_history.detail.terminal_no, 4);//[4];   //POS编号 4BYTE;
        memcpy(lpdata->dt_trdate2, octinf.blk_history.detail.dt_trdate, 3);//[3]; //交易日期yy-mm-dd
        memcpy(lpdata->banlance, octinf.blk_history.detail.banlance, 3);  //交易前余额
        memcpy(lpdata->tr_amount, octinf.blk_history.detail.tr_amount, 2);    //交易金额
//==================================================
        //地铁信息区数据项
        memcpy(lpdata->metro_inf, octinf.blk_metro_org.buf, 14);
        return(0);
    }
    while (0);

    return(ret);

}


//==========================================================================================
//函 数 名: octm1_writedata
//功    能: 对M1卡进行写操作
//入口参数:
//            idx_sam:当前应用SAM卡对应的卡槽序列号(从0开始)
//          nopertion:操作方式, 0=黑名单解锁,1=黑名单锁卡(不写地铁信息区也不扣费，只写黑名单标记), 2=只更新地铁信息区，=3只扣费不操作地铁信息区, =4扣费的同时写地铁信息区,
//          tr_dttime:消费时间，只有在要扣值时才使用, bcd码, yyyymmddhhnnss
//             lvalue:要消费的值
//          metro_inf:地铁信息块 16byte
//
//出口参数:
//          无
//返 回 值:
//          0=成功
//       0xEE=写卡失败，需要时行交易确认
//       其它=写卡失败，无需处理
//==========================================================================================
//unsigned char octm1_writedata(unsigned char idx_sam, unsigned char nopertion, unsigned char *tr_dttime, unsigned long lvalue, unsigned char *metro_inf)
//{
//
//    const unsigned char tr_type=0x03;//固定为0x03表示公共钱包消费
//    const unsigned char service_code[2]={0x00,0x00};//公交未定义，先强制0x00
//    const unsigned char pos_code[4]={0x00,0x00};//公交未给出定义，先强制0x00
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
//            //黑名单锁卡(不写地铁信息区也不扣费，只写黑名单标记)
//            if (nopertion==1)
//            {
//                lppublic->lock_flag = 0x04;
//            }
//            else
//            {
//                lppublic->lock_flag = 0x00;
//            }
//            crc_cu(g_public_inf, 16);//计算CRC8
//            ret = octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX, g_public_inf);
//            if (ret)
//            {
//                ret = 0xEE;
//            };
//
//            //正本写成功后，副本即使操作失败也当做成功
//            octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX, g_public_inf);
//        }
//        else if (nopertion==2)
//        {
//            //========================================================
//            //只更新地铁信息区
//            ret = octm1_writeblock(OCT_BLK_METRO_ORG_IDX, metro_inf);
//            if (ret)
//            {
//                ret = 0xEE;
//            };
//
//            //正本写成功后，副本即使操作失败也当做成功
//            octm1_writeblock(OCT_BLK_METRO_BAK_IDX, metro_inf);
//
//        }
//        else if ((nopertion==3) ||(nopertion==4))
//        {
//            //============================================================
//            //3=只扣费不操作地铁信息区
//            //4=//扣费的同时写地铁信息区
//            if (lvalue > g_nbalance)
//            {
//                return 0x25;
//            }
//
//
//            //写交易过程历史
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
//            //写公共信息区, 进程标识=0x11,公共计数+1, 交易类型=0x03
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
//                //需写入地铁信息区
//                ret = octm1_writeblock(OCT_BLK_METRO_ORG_IDX, metro_inf);
//                if (ret)
//                {
//                    ret = 0xEE;
//                    break;
//                }
//
//            }
//
//            //钱包正本扣款
//            //0xC0=消费，
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
//            //钱包正本扣款成功后的操作失败都当做交易成功
//            //写公共信息区, 进程标识=0x13
//            lppublic->proc_flag = 0x13;
//            crc_cu(g_public_inf, 16);
//            ret = octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX, g_public_inf);
//            if (ret)
//            {
//                ret = 0;
//                break;
//            }
//
//            //钱包正本到副本
//            ret = octm1_copy_purse(OCT_BLK_PURSE_BAK_IDX, OCT_BLK_PURSE_ORG_IDX);
//            if (ret)
//            {
//                ret = 0x0;
//                break;
//            }
//
//
//            //写公共信息区, 进程标识=0x13
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
//            //操作方式参数错误，不可识别
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
//函 数 名: octm1_writedata2
//功    能: 对M1卡进行写操作
//入口参数:
//            idx_sam:当前应用SAM卡对应的卡槽序列号(从0开始)
//          nopertion:操作方式, 0=黑名单解锁,1=黑名单锁卡(不写地铁信息区也不扣费，只写黑名单标记), 2=只更新地铁信息区，=3只扣费不操作地铁信息区, =4扣费的同时写地铁信息区,
//          tr_dttime:消费时间，只有在要扣值时才使用, bcd码, yyyymmddhhnnss
//       npursetype:要处理的钱包类别, 0=公用钱包, 1或非0值=专用钱包
//             lvalue:要消费的值
//          metro_inf:地铁信息块 16byte
//
//出口参数:
//          无
//返 回 值:
//          0=成功
//       0xEE=写卡失败，需要时行交易确认
//       其它=写卡失败，无需处理
//==========================================================================================
unsigned char octm1_writedata2(unsigned char idx_sam, unsigned char nopertion, unsigned char *tr_dttime, unsigned char npursetype, unsigned long lvalue, unsigned char *metro_inf)
{

    unsigned char tr_type=0x03;//固定为0x03表示公共钱包消费
    const unsigned char service_code[2]={0x00,0x00};//公交未定义，先强制0x00
    const unsigned char pos_code[4]={0x00,0x00};//公交未给出定义，先强制0x00

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
            //黑名单锁卡(不写地铁信息区也不扣费，只写黑名单标记)
            if (nopertion==1)
            {
                lppublic->lock_flag = 0x04;
            }
            else
            {
                lppublic->lock_flag = 0x00;
            }
            crc_cu(g_public_inf, 16);//计算CRC8
            ret = octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX, g_public_inf);
            if (ret)
            {
                ret = 0xEE;
            };

            //正本写成功后，副本即使操作失败也当做成功
            octm1_writeblock(OCT_BLK_PUBLIC_BAK_IDX, g_public_inf);
        }
        else if (nopertion==2)
        {
            //========================================================
            //只更新地铁信息区
            ret = octm1_writeblock(OCT_BLK_METRO_ORG_IDX, metro_inf);
            if (ret)
            {
                ret = 0xEE;
            };

            //正本写成功后，副本即使操作失败也当做成功
            octm1_writeblock(OCT_BLK_METRO_BAK_IDX, metro_inf);
        }
        else if ((nopertion==3) ||(nopertion==4))
        {
            //============================================================
            //3=只扣费不操作地铁信息区
            //4=//扣费的同时写地铁信息区
			if (npursetype == 0)
			{
				//公用钱包余额判断
	            if (lvalue > g_nbalance)
	            {
	                return 0x25;
	            }
				tr_type=0x03;
            }
            else
            {
				//专用钱包余额判断
	            if (lvalue > g_nbalance2)
	            {
	                return 0x25;
	            }
	            tr_type=0x04;
            }


            //写交易过程历史
            lphistory->tr_type = tr_type;
            memcpy(lphistory->servicer_code, service_code, 2);
            memcpy(lphistory->terminal_no, pos_code, 4);
            memcpy(lphistory->dt_trdate, tr_dttime+1, 3);//yymmdd
			if (npursetype == 0)
			{
				//公用钱包操作
	            lphistory->banlance[0] = ((g_nbalance & 0x00FF0000) >> 16);
	            lphistory->banlance[1] = ((g_nbalance & 0x0000FF00) >> 8);
	            lphistory->banlance[2] = (g_nbalance & 0x000000FF);
	            lphistory->tr_amount[0] = ((lvalue & 0x0000FF00) >> 8);
	            lphistory->tr_amount[1] = (lvalue & 0x000000FF);
			}
			else
			{
				//专用钱包操作
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


            //写公共信息区, 进程标识=0x11,公共计数+1, 交易类型=0x03
            octm1_inc_public_tr_count(0, lppublic);
			if (npursetype == 0)
			{
				//公用钱包操作
            	lppublic->proc_flag = 0x11;
			}
			else
			{
				//专用钱包操作
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
                //需写入地铁信息区
                ret = octm1_writeblock(OCT_BLK_METRO_ORG_IDX, metro_inf);
                if (ret)
                {
                    ret = 0xEE;
                    break;
                }

            }

            //钱包正本扣款
            //0xC0=消费，
			if (npursetype == 0)
			{
				//公用钱包操作
	            ret = octm1_write_purse(OCT_BLK_PURSE_ORG_IDX, 0xC0,lvalue);
			}
			else
			{
				//专用钱包操作
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

            //钱包正本扣款成功后的操作失败都当做交易成功
            //写公共信息区, 进程标识=0x13
			if (npursetype == 0)
			{
				//公用钱包操作
            	lppublic->proc_flag = 0x13;
			}
			else
			{
				//专用钱包操作
				lppublic->proc_flag = 0x14;
			}
            crc_cu(g_public_inf, 16);
            ret = octm1_writeblock(OCT_BLK_PUBLIC_ORG_IDX, g_public_inf);
            if (ret)
            {
                ret = 0;
                break;
            }

            //钱包正本到副本
			if (npursetype == 0)
			{
				//公用钱包操作
            	ret = octm1_copy_purse(OCT_BLK_PURSE_BAK_IDX, OCT_BLK_PURSE_ORG_IDX);
			}
			else
			{
				//专用钱包操作
            	ret = octm1_copy_purse(OCT_BLK_PURSE_OTHER_BAK_IDX, OCT_BLK_PURSE_OTHER_ORG_IDX);
			}

            if (ret)
            {
                ret = 0x0;
                break;
            }


            //写公共信息区, 进程标识=0x13
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
            //操作方式参数错误，不可识别
            ret = 0xE1;
        }
    }
    while (0);

    return(ret);
}



//==========================================================================================
//函 数 名: octm1_gettac
//功    能: 生成TAC码
//入口参数:
//          idx_sam:当前应用SAM卡对应的卡槽序列号(从0开始)
//      lpappnumber:当前卡片的应用卡编号, 在octm1_getdata函数中已获取
//           lpdata:要参与TAC计算的数据内容 16byte
//
//出口参数:
//             lptac:要写入的地铁信息块 4byte
//返 回 值:
//          0=成功
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
//函 数 名: octm1_judge
//功    能: 长沙公交卡判断,当m1卡的选卡select返回值sak为CPU卡时，要对此卡判断是否长沙的CPU仿M1卡,
//          如果能不读取M1数据块则认为是纯CPU卡，函数返回，并在函数调用后继续判断是什么行业的CPU卡;
//          如果能读取M1数据块则为公交cpu仿m1,再对m1卡的发行区启用标志进行判断,=0x01/0xBB，则以公交M1卡的
//          流程进行处理,如果=2则直接走公交CPU卡流程，如为其它标志则函数返回，并在函数调用后继续判断是什么行业的CPU卡;
//入口参数:
//      lpcardnumber:票卡的物理卡号, 在searchcard函数中已获取
//
//出口参数:
//             无
//返 回 值:
//          0=无卡
//		    2=后续按公交m1流程处理
//          4=后续按公交cpu卡流程处理
//          3=后续需要继续判断是哪个行业的CPU卡
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

		//不是公交cpu仿M1卡 , 函数后继续判断CPU类型
		ret=ISO14443A_Request( 0x52, tmpbuf );
		if (ret)
		{
			ret=ISO14443A_Request( 0x52, tmpbuf );
		}
		if (ret)
		{
			//寻不到卡则返回无卡
			return 0x00;
		}
		ret=ISO14443A_Select(lpcardnumber, tmpbuf);
		if (ret)
		{
			//找不到刚才判断的卡则判断为无卡;
			return 0x00;
		}

		//其它CPU卡，函数调用后需要继续判断是哪个行业的CPU卡
		return 0x03;
	}

	//如果能对cpu卡进行读取m1数据块，说明是公交仿cpu卡仿m1

	//if ((lpissue->appflag == 0x01)||(lpissue->appflag == 0xBB))
	//{
	//	//公交M1卡 , 后续走公交m1消费流程
	//	return 0x02;
	//}
	//else if (lpissue->appflag  == 0x02)
	//{
	//	//公交cpu卡 , 后续走公交CPU消费流程
	//	return 0x04;

	//}
	//else if (lpissue->appflag == 0xaa)
	//{
	//	return 98;	// 车票未启用
	//}
	//else
	//{

 //       //其它CPU卡，函数调用后需要继续判断是哪个行业的CPU卡
	//	return 0x03;
	//}

	if (lpissue->appflag  == 0x02)
	{
		//公交cpu卡 , 后续走公交CPU消费流程
		return 0x04;
	}
	else
	{
		return 0x02;	// 不转入CPU流程的全部当作M1,进入M1流程再判断启用标识
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

			//不是公交cpu仿M1卡 , 函数后继续判断CPU类型
			if (ISO14443A_Request( 0x52, tmpbuf ) != 0)
			{
				//寻不到卡则返回无卡
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
				//找不到刚才判断的卡则判断为无卡;
				dbg_formatvar("octm1_readblock:ISO14443A_Select");
				ret = ERR_CARD_NONE;
				break;
			}

			//其它CPU卡，函数调用后需要继续判断是哪个行业的CPU卡
			type = 0x03;
			break;
		}

		//如果能对cpu卡进行读取m1数据块，说明是公交仿cpu卡仿m1

		if (lpissue->appflag  == 0x02)
		{
			//公交cpu卡 , 后续走公交CPU消费流程
			type = 0x04;
		}
		else
		{
			type = 0x02;	// 不转入CPU流程的全部当作M1,进入M1流程再判断启用标识
		}

	} while (0);

	return ret;
}
