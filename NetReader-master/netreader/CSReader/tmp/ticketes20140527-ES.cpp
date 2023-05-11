#define _TICKETES_


#include <signal.h>
#include <stdlib.h>
#include <stdarg.h>
#include "../api/api.h"
#include "../api/structs.h"
#include "../api/TimesEx.h"
#include "../api/CmdSort.h"
#include "../api/publics.h"
#include "../api/errors.h"
#include "../api/DataSecurity.h"
#include "../api/TicketBase.h"

#include "myprintf.h"


#include "ticketes.h"
#include "algorithm.h"
#include "iso14443.h"
#include "linker.h"

#define LEN_ISSUE_BASE			0x28
#define LEN_PUBLIC_BASE			0x1E
#define LEN_OWNER_BASE			0x38
#define LEN_WALLET				0x04
#define LEN_HIS_LAST			0x17
#define LEN_HIS_ALL				0xE6
#define LEN_TRADE_ASSIST		0x30
#define LEN_METRO				0x30
#define LEN_CTRL_RECORD			0x18
#define LEN_APP_CTRL			0x20
#define LEN_CHANGE_CNT			0x02
#define LEN_CONSUME_CNT			0x02

#define _NEWTICKET_   //操作的是否是新票卡，两种储值卡创建方式不一样,最终采用新票卡格式
//#define _ENABLE_FIXKEY_     //是否采用固定密钥，采用固定密钥时逻辑卡号和密钥强制使用程序内部的，其它值取外部 传入
#define _ENABLE_CLEAR_      //是否允许清除票卡接口



typedef unsigned char bcd;
#pragma pack(push, 1)

//初始化输入接口
typedef struct
{
    char szorderno[14]; // 订单号 14 0 char
    char szapplyno[10]; // 申请编号 10 14 char
    uint8_t bcardtype[2];  // 车票类型 2 24 Byte[2]
    uint8_t logicalid[8];  //应用序列号 8 26 Byte[2]
    uint8_t bdeposite;  //押金 1 34 Byte
    long lbalance;   //钱包初始值 4 35 Dword
    uint16_t caps;   //可充值上限 2 39 Word
    uint8_t bactive;  //发售激活标志 1 41 Byte
    uint8_t issuecode[2]; //发卡方代码 2 42 Bcd[2]
    uint8_t citycode[2]; //城市代码 2 44 Bcd[2]
    uint8_t industrycode[2];//行业代码 2 46 Bcd[2]
    uint8_t testflag;  //测试标记 1 48 Byte
    uint8_t issuedate[4]; //发行日期 4 49 Bcd[4]
    uint8_t cardversion[2]; //卡版本号 2 53 Byte[2]
    uint8_t effectivefrom[4];//卡启动日期 4 55 Bcd[4]
    uint8_t effectiveto[4]; //卡截至日期 4 59 Bcd[4]
//=================================================
    uint8_t logiceffectivefrom[7]; //逻辑起始时间
    uint16_t logicectivetime; //逻辑有效天数
//=================================================

    uint8_t appversion;  //应用版本号 1 63 Byte
    uint8_t passmode;  //出入模式 1 64 Byte
    uint8_t entryline[2]; //可入线路站点 2 65 Bcd[2]
    uint8_t exitline[2]; //可出线路站点 2 67 Bcd[2]

    uint8_t mf_tkey[16]; //MF传输密钥 16 69 Byte[16]
    uint8_t mf_mkey[16]; //MF主控密钥 16 85 Byte[16]
    uint8_t mf_mamk[16]; //MF维护密钥 16 101 Byte[16]
    uint8_t mf_eak[16];     //MF外部认证密钥

    uint8_t adf1_tkey[16]; //ADF1传输密钥 16 117 Byte[16]
    uint8_t adf1_mkey[16]; //ADF1主控密钥 16 133 Byte[16]
    uint8_t adf1_mamk_00[16];//ADF1应用维护密钥00 16 229 Byte[16]
    uint8_t adf1_mpk_0101[16]; //ADF1消费密钥01 16 165 Byte[16]
    uint8_t adf1_mlk_0201[16]; //ADF1圈存密钥 16 149 Byte[16]
//   uint8_t adf1_mulk_0201[16]; //ADF1圈提密钥 16 149 Byte[16]
//   uint8_t adf1_muk_0201[16]; //ADF1 修改透支限额密钥16 149 Byte[16]
    uint8_t adf1_mtk[16]; //ADF1的TAC密钥 16 181 Byte[16]
    uint8_t adf1_mamk_01[16];//ADF1应用维护密钥01 16 245 Byte[16]
    uint8_t adf1_mamk_02[16];//ADF1应用维护密钥02 16 261 Byte[16]
    uint8_t adf1_mabk[16]; //ADF1应用锁定密钥 16 197 Byte[16]
    uint8_t adf1_mauk[16]; //ADF1应用解锁密钥 16 197 Byte[16]
    uint8_t adf1_mpuk[16]; //ADF1解锁PIN密钥 16 197 Byte[16]
    uint8_t adf1_mprk[16]; //ADF1重装PIN密钥 16 213 Byte[16]
    uint8_t adf1_muk[16];//ADF1修改透支额密钥03 16 277 Byte[16]
    uint8_t adf1_mulk[16];//ADF1圈提密钥 16 293 Byte[16]
    uint8_t adf1_eak[16];//ADF1外部认证密钥 16 309 Byte[16]
    uint8_t adf1_pin[16];//ADF116 325 Byte[16]
} TISSUE_INPUT, *LPTISSUE_INPUT;

//预赋值输入接口
typedef struct
{
    char szorderno[14]; // 订单号 14 0 char
    char szapplyno[10]; // 申请编号 10 14 char
    uint8_t bcardtype[2];  // 车票类型 2 24 Byte[2]
    uint8_t logicalid[8];  //应用序列号 8 26 Byte[2]
    uint8_t bdeposite;  //押金 1 34 Byte
    long lbalance;   //钱包初始值 4 35 Dword
    uint16_t caps;   //可充值上限 2 39 Word
//=================================================
    uint8_t bactive;  //发售激活标志 1 41 Byte
    uint8_t logiceffectivefrom[7]; //逻辑起始时间
    uint16_t logicectivetime; //逻辑有效天数
//=================================================

    uint8_t passmode;  //出入模式 1 64 Byte
    uint8_t entryline[2]; //可入线路站点 2 65 Bcd[2]
    uint8_t exitline[2]; //可出线路站点 2 67 Bcd[2]

    uint8_t adf1_mlk_0201[16]; //ADF1圈存密钥 16 149 Byte[16]
    uint8_t adf1_mpk_0101[16]; //ADF1消费密钥01 16 165 Byte[16]
    uint8_t adf1_mtk[16]; //ADF1的TAC密钥 16 181 Byte[16]
//    uint8_t adf1_mpuk[16]; //ADF1解锁PIN密钥 16 197 Byte[16]
//    uint8_t adf1_mprk[16]; //ADF1重装PIN密钥 16 213 Byte[16]
    uint8_t adf1_mamk_00[16];//ADF1应用维护密钥00 16 229 Byte[16]
    uint8_t adf1_mamk_01[16];//ADF1应用维护密钥01 16 245 Byte[16]
    uint8_t adf1_mamk_02[16];//ADF1应用维护密钥02 16 261 Byte[16]
//    uint8_t adf1_mamk_03[16];//ADF1应用维护密钥03 16 277 Byte[16]
    uint8_t adf1_pin[16];//ADF1的PIN码 16 293 Byte[16]

}TEVALUATE_INPUT, *LPTEVALUATE_INPUT;

// //预付值输入接口
// typedef struct
// {
//     char szorderno[14]; // 订单号 14 0 char
//     char szapplyno[14]; // 申请编号 10 14 char
//     uint8_t bcardtype[2];  // 车票类型 2 24 Byte[2]
//     uint8_t logicalid[8];  //应用序列号 8 26 Byte[2]
//     uint8_t bdeposite;  //押金 1 34 Byte
//     long lbalance;   //钱包初始值 4 35 Dword
//     uint16_t caps;   //可充值上限 2 39 Word
//
//     uint8_t passmode;  //出入模式 1 64 Byte
//     uint8_t entryline[2]; //可入线路站点 2 65 Bcd[2]
//     uint8_t exitline[2]; //可出线路站点 2 67 Bcd[2]
//
//     uint8_t adf1_mlk_0201[16]; //ADF1圈存密钥 16 149 Byte[16]
//     uint8_t adf1_mpk_0101[16]; //ADF1消费密钥01 16 165 Byte[16]
//     uint8_t adf1_mtk[16]; //ADF1的TAC密钥 16 181 Byte[16]
// //    uint8_t adf1_mpuk[16]; //ADF1解锁PIN密钥 16 197 Byte[16]
// //    uint8_t adf1_mprk[16]; //ADF1重装PIN密钥 16 213 Byte[16]
//     uint8_t adf1_mamk_00[16];//ADF1应用维护密钥00 16 229 Byte[16]
//     uint8_t adf1_mamk_01[16];//ADF1应用维护密钥01 16 245 Byte[16]
//     uint8_t adf1_mamk_02[16];//ADF1应用维护密钥02 16 261 Byte[16]
// //    uint8_t adf1_mamk_03[16];//ADF1应用维护密钥03 16 277 Byte[16]
//     uint8_t adf1_pin[16];//ADF1的PIN码 16 293 Byte[16]
//
// }TEVALUATE_INPUT, *LPTEVALUATE_INPUT;

//注销输入接口
typedef struct
{
    char szorderno[14]; // 订单号 14 0 char
    char szapplyno[10]; // 申请编号 10 14 char
    uint8_t bcardtype[2];  // 车票类型 2 24 Byte[2]
    uint8_t logicalid[8];  //应用序列号 8 26 Byte[2]
    uint8_t bdeposite;  //押金 1 34 Byte
    long lbalance;   //钱包初始值 4 35 Dword
    uint8_t adf1_mpk_0101[16]; //ADF1消费密钥01 16 165 Byte[16]
    uint8_t adf1_mtk[16]; //ADF1的TAC密钥 16 181 Byte[16]
//    uint8_t adf1_mpuk[16]; //ADF1解锁PIN密钥 16 197 Byte[16]
//    uint8_t adf1_mprk[16]; //ADF1重装PIN密钥 16 213 Byte[16]
    uint8_t adf1_mamk_00[16];//ADF1应用维护密钥00 16 229 Byte[16]
    uint8_t adf1_mamk_01[16];//ADF1应用维护密钥01 16 245 Byte[16]
    uint8_t adf1_mamk_02[16];//ADF1应用维护密钥02 16 261 Byte[16]
//    uint8_t adf1_mamk_03[16];//ADF1应用维护密钥03 16 277 Byte[16]

}TDESTROY_INPUT, *LPTDESTROY_INPUT;



//========================================
//充值初始化结返回结构
//========================================
typedef struct InitforLoadStr
{
    unsigned char        Balance[4];                 // 余额
    unsigned char        OnlineDealSerial[2]; /** 联机交易序号 **/
    unsigned char        EncryVer;                   // 密钥版本号
    unsigned char        CalcMarker;                 // 算法标识
    unsigned char        Random[4];                  // 伪随机数
    unsigned char        MAC1[4];                      // mac1
} InitforLoadStr_t;


//=====================================
//文件信息
//发行基本信息-- 主文件下
typedef struct
{
    bcd issuecode[2];               //城市代码
    bcd citycode[2];                //城市代码
    bcd industrycode[2];            //行业代码
    unsigned char testflag;         //测试标记
    unsigned char rfu;              //保留数据
    unsigned char logicnumber[8];   //应用序列号
    unsigned char cardtype[2];      //卡类型
    bcd issuedate[4];//yyyymmdd         //发行日期
    unsigned char issuedevice[6];   //发行设备信息
    unsigned char  cardversion[2];  //卡版本号
    unsigned char effectivefrom[4]; //卡启用日期yyyymmdd
    unsigned char effectiveto[4];   //卡截至日期yyyymmdd
    unsigned char rfu2[2];          //保留
}TMF_ISSUE,*LPTMF_ISSUE;//40Byte

//公共应用基本数据文件-- ADF1目录下
typedef struct
{
    bcd issuecode[2];               //发卡方代码
    bcd citycode[2];                //城市代码
    bcd industrycode[2];            //行业代码
    unsigned char rfu[2];           //保留数据
    bcd appstatus;                  //启用标识
    bcd appversion;                 //应用版本
    unsigned char testflag;         //测试标记
    unsigned char rfu2;             //
    unsigned char logicnumber[8];   //应用序列号
    unsigned char effectivefrom[4]; //应用启用日期yyyymmdd
    unsigned char effectiveto[4];   //应用截至日期yyyymmdd
    unsigned char rfu3[2];          //保留
}TPUBLICINF,*LPTPUBLICINF; //总长30 BYTE


//持卡人基本数据文件
typedef struct
{

}TPERSONALIZED,*LPPERSONALIZED;

//应用控制文件
typedef struct
{
    unsigned char saleflag;     //发售激活标记
    unsigned char deposit;      //发售押金
    unsigned char saledevice[6];   //发售设备信息
    unsigned char passmode;     //出入模式判断
    unsigned char entryline;    //可入线路
    unsigned char entrystation; //可入站点
    unsigned char exitline;     //可出线路
    unsigned char exitstation;  //可出站点
    unsigned char pursecaps[2];    //钱包上限
    unsigned char rfu[17];      //预留
}TAPPCTRL,*LPTAPPCTRL;
//文件信息结构
//==========================================
// typedef struct
// {
//  // 4BYTE
//  uint32_t tm4:32; // 最后一次处理时间
//  // 2BYTE
//  uint16_t line:6; // 最后一次处理站点
//  uint16_t station:6; // 最后一次处理站点
//  uint16_t devicetype:4; // 设备类型
//  // 4BYTE
//  uint32_t deviceid:10; // 设备号
//  uint32_t balance:14; // 剩余金额(分)
//  uint32_t status:5; // 卡状态
//  uint32_t mark:3; // 特殊标记
//  // 2BYTE
//  uint16_t entry_line:6;  //入口线路
//  uint16_t entry_station:6; // 入口站点
//  uint16_t rfu1:4; // 预留
//  // 1BYTE
//  uint8_t rfu2:8; // 预留
//  // 2BYTE
//  uint16_t tradecount:16; //交易累计
//  // 1BYTE
//  uint8_t check:8; // 校验
// }TSJTDATAREA, *LPTSJTDATAREA;

// typedef struct
// {
//     // 4BYTE
// uint32_t tm4:32; // 最后一次处理时间
//     // 2BYTE
// uint16_t line:6; // 最后一次处理站点
// uint16_t station:6; // 最后一次处理站点
// uint16_t devicetype:4; // 设备类型
//     // 4BYTE
// uint32_t deviceid:10; // 设备号
// uint32_t balance:14; // 剩余金额(分)
// uint32_t status:5; // 卡状态
// uint32_t mark:3; // 特殊标记
//     // 2BYTE
// uint16_t entry_line:6;  //入口线路
// uint16_t entry_station:6; // 入口站点
// uint16_t rfu1:4; // 预留
//     // 1BYTE
// uint8_t rfu2:8; // 预留
//     // 2BYTE
// uint16_t tradecount:16; //交易累计
//     // 1BYTE
// uint8_t check:8; // 校验
// }TSJTDATAREA, *LPTSJTDATAREA;



//=====================================================
//CPU卡密钥
// typedef struct{
//  //MF密钥
//  unsigned char TK_MF[17];  //传输密钥
//  unsigned char mf_mkey[17];//主控密钥
//  unsigned char mf_mamk[17];//维护密钥
//  //ADF1密钥
//  unsigned char adf1_tkey[17];      //传输密钥
//  unsigned char adf1_mkey[17];      //主控密钥
//  unsigned char MLK_ADF1_0201[17];  //圈存交易密钥
//  unsigned char MPK_ADF1_0101[17];  //消费交易密钥01
//  unsigned char MPK_ADF1_0102[17];  //消费交易密钥02
//  unsigned char MPK_ADF1_0103[17];  //消费交易密钥03
//  unsigned char MPK_ADF1_0104[17];  //消费交易密钥04
//  unsigned char MPK_ADF1_0105[17];  //消费交易密钥05
//  unsigned char MTK_ADF1[17];       //TAC 密钥
//  unsigned char MPUK_ADF1[17];      //解锁PIN 密钥
//  unsigned char MRPK_ADF1[17];      //重装PIN 密钥
//  unsigned char MAMK_ADF1_00[17];   //应用维护密钥01
//  unsigned char MAMK_ADF1_01[17];   //应用维护密钥02
//  unsigned char MAMK_ADF1_02[17];   //应用维护密钥03
//  unsigned char MAMK_ADF1_03[17];   //应用维护密钥04
//  unsigned char PIN_ADF1[4];       //密码
//  //ADF2密钥
//  unsigned char ADF2_TKEY[17];      //传输密钥
//  unsigned char ADF2_MKEY[17];      //主控密钥
//  unsigned char MLK_ADF2_0201[17];  //圈存交易密钥
//  unsigned char MPK_ADF2_0101[17];  //消费交易密钥01
//  unsigned char MPK_ADF2_0102[17];  //消费交易密钥02
//  unsigned char MPK_ADF2_0103[17];  //消费交易密钥03
//  unsigned char MPK_ADF2_0104[17];  //消费交易密钥04
//  unsigned char MPK_ADF2_0105[17];  //消费交易密钥05
//  unsigned char MTK_ADF2[17];       //TAC 密钥
//  unsigned char MPUK_ADF2[17];      //解锁PIN 密钥
//  unsigned char MRPK_ADF2[17];      //重装PIN 密钥
//  unsigned char MAMK_ADF2_00[17];   //应用维护密钥01
//  unsigned char MAMK_ADF2_01[17];   //应用维护密钥02
//  unsigned char MAMK_ADF2_02[17];   //应用维护密钥03
//  unsigned char MAMK_ADF2_03[17];   //应用维护密钥04
//  unsigned char PIN_ADF2[4];       //密码
// }TTICK_KEY,*LPTTICKK_KEY;

// typedef struct{
//  //MF密钥
//  unsigned char TK_MF[17];  //传输密钥
//  unsigned char mf_mkey[17];//主控密钥
//  unsigned char mf_mamk[17];//维护密钥
//  //ADF1密钥
//  unsigned char adf1_tkey[17];      //传输密钥
//  unsigned char adf1_mkey[17];      //主控密钥
//  unsigned char MLK_ADF1_0201[17];  //圈存交易密钥
//  unsigned char MPK_ADF1_0101[17];  //消费交易密钥01
//  unsigned char MTK_ADF1[17];       //TAC 密钥
//  unsigned char MPUK_ADF1[17];      //解锁PIN 密钥
//  unsigned char MRPK_ADF1[17];      //重装PIN 密钥
//  unsigned char MAMK_ADF1_00[17];   //应用维护密钥01
//  unsigned char MAMK_ADF1_01[17];   //应用维护密钥02
//  unsigned char MAMK_ADF1_02[17];   //应用维护密钥03
//  unsigned char MAMK_ADF1_03[17];   //应用维护密钥04
//  unsigned char PIN_ADF1[4];       //密码
// }TTICK_KEY,*LPTTICKK_KEY;

// typedef struct
// {
//     //MF密钥
//     unsigned char mf_tkey[17];  //传输密钥1
//     unsigned char mf_mkey[17];//主控密钥2
//     unsigned char mf_mamk[17];//维护密钥3
// //    unsigned char MF_PIN[17];//
//     //ADF1密钥
//     unsigned char adf1_tkey[17];      //传输密钥4
//     unsigned char adf1_mkey[17];      //主控密钥5
//     unsigned char adf1_mlk_0201[17];  //圈存交易密钥6
//     unsigned char adf1_mpk_0101[17];  //消费交易密钥01 7
//     unsigned char adf1_mtk[17];       //TAC 密钥8
//     unsigned char adf1_mpuk[17];      //解锁PIN 密钥9
//     unsigned char adf1_mprk[17];      //重装PIN 密钥10
//     unsigned char adf1_mamk_00[17];   //应用维护密钥01 11
//     unsigned char adf1_mamk_01[17];   //应用维护密钥02 12
//     unsigned char adf1_mamk_02[17];   //应用维护密钥03 13
//     unsigned char ADF1_MAMK_03[17];   //应用维护密钥04 14
//     unsigned char adf1_pin[17];       //密码15
// }TTICK_KEY,*LPTTICKK_KEY;

typedef struct
{
    //MF密钥
    unsigned char mf_tkey[17];  //传输密钥1
    unsigned char mf_mkey[17];//主控密钥2
    unsigned char mf_mamk[17];//维护密钥3
    unsigned char mf_eak[17];//外部认证密钥
    //ADF1密钥
    unsigned char adf1_tkey[17];      //传输密钥4
    unsigned char adf1_mkey[17];      //主控密钥5
    unsigned char adf1_mamk_00[17];   //应用维护密钥01 11
    unsigned char adf1_mpk_0101[17];  //消费交易密钥01 7
    unsigned char adf1_mlk_0201[17];  //圈存交易密钥6
    unsigned char adf1_mtk[17];       //TAC 密钥8
    unsigned char adf1_mamk_01[17];   //应用维护密钥02 12
    unsigned char adf1_mamk_02[17];   //应用维护密钥03 13
    unsigned char adf1_mabk[17];   //应用锁定密钥
    unsigned char adf1_mauk[17];   //应用解锁密钥
    unsigned char adf1_mpuk[17];      //解锁PIN 密钥9
    unsigned char adf1_mprk[17];      //重装PIN 密钥10
    unsigned char adf1_muk[17];   //修改透支限额密钥
    unsigned char adf1_mulk[17];   //圈提密钥
    unsigned char adf1_eak[17];   //外部认证密钥
    unsigned char adf1_pin[17];       //密码15
}TTICK_KEY,*LPTTICKK_KEY;


typedef struct
{
    bcd issuecode[2];               //发卡方代码
    bcd citycode[2];                //城市代码
    bcd industrycode[2];            //行业代码
    bcd testflag;                   //主应用和ADF1 应用共用
    unsigned char logicnumber[8];   //应用序列号主应用和ADF1 应用共用
    unsigned char cardtype[2];      //卡类型
    //主应用区
    bcd issuedate[4];//yyyymmdd         //发行日期
    unsigned char issuedevice[6];   //发行设备信息
    unsigned char  cardversion[2];  //卡版本号
    unsigned char effectivefrom[4]; //卡启用日期yyyymmdd
    unsigned char effectiveto[4];   //卡截至日期yyyymmdd
    //ADF1
    unsigned char szcaps[2];   //可充值上限 2 39 Word
    unsigned char appstatus;      //启用标识
    bcd appversion;  //卡版本号
    unsigned char appeffectivefrom[4]; //应用启用日期yyyymmdd
    unsigned char appeffectiveto[4];   //应用截至日期yyyymmdd

//============================================
    unsigned char logiceffectivefrom[7]; //逻辑起始时间
    unsigned char szlogicectivetime[4]; //逻辑有效天数

    //应用控制文件
    unsigned char saleflag;     //发售激活标记
    unsigned char deposit;      //发售押金
    unsigned char saledevice[6];   //发售设备信息
    unsigned char passmode;     //出入模式判断
    unsigned char entryline;    //可入线路
    unsigned char entrystation; //可入站点
    unsigned char exitline;     //可出线路
    unsigned char exitstation;  //可出站点
} TTICKETINF, *LPTTICKETINF;

typedef struct
{
    char szorderno[14]; // 订单号 14 0 char
    char szapplyno[14]; // 申请编号 10 14 char
    uint8_t bcardtype[2];  // 车票类型 2 24 Byte[2]

    unsigned char logicnumber[16];
    unsigned char physical_id[8];
    unsigned char tm[7];
    unsigned char deviceno[6];
    unsigned int balance;//
    TTICKETINF inf;
    TTICK_KEY key;
} TINPUT, *LPTINPUT;

typedef uint16_t (* LPFUNCTION_ES_READ)(uint8_t *lpoutput);
typedef uint16_t (* LPFUNCTION_ES_OPT)(uint8_t *lpinput, uint8_t *lpoutput);

typedef struct
{
    LPFUNCTION_ES_READ esapi_analyse;
    LPFUNCTION_ES_OPT esapi_init;
    LPFUNCTION_ES_OPT esapi_evaluate;
    LPFUNCTION_ES_OPT esapi_destroy;
    LPFUNCTION_ES_OPT esapi_recode;
    LPFUNCTION_ES_OPT esapi_clear;

} TESFUNC, *LPTESFUNC;

#if 1
uint16_t es_ul_read(uint8_t *lpoutput);

uint16_t es_ul_init(uint8_t *lpinput, uint8_t *lpoutput);
uint16_t es_ul_evaluate(uint8_t *lpinput, uint8_t *lpoutput);
uint16_t es_ul_destroy(uint8_t *lpinput, uint8_t *lpoutput);
uint16_t es_ul_recode(uint8_t *lpinput, uint8_t *lpoutput);

uint16_t es_ul_clear(uint8_t *lpinput, uint8_t *lpoutput);



uint16_t es_svt_read(uint8_t *lpoutput);
uint16_t es_svt_init(uint8_t *lpinput, uint8_t *lpoutput);
uint16_t es_svt_evaluate(uint8_t *lpinput, uint8_t *lpoutput);
uint16_t es_svt_destroy(uint8_t *lpinput, uint8_t *lpoutput);
uint16_t es_svt_recode(uint8_t *lpinput, uint8_t *lpoutput);

uint16_t es_svt_clear(uint8_t *lpinput, uint8_t *lpoutput);

#endif


TESFUNC esfun_call[2]=
{
    {es_ul_read, es_ul_init, es_ul_evaluate, es_ul_destroy, es_ul_recode,es_ul_clear},
    {es_svt_read, es_svt_init, es_svt_evaluate, es_svt_destroy, es_svt_recode,es_svt_clear}
};


#pragma pack(pop)


//static TTICK_KEY l_key=
//{
//    //MF
//    {"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"},//传输密钥
//    {"\x88\x88\x88\x88\x88\x88\x88\x99\x99\x99\x99\x99\x99\x99\x99\x99"},//主控密钥
//    {"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"},//维护密钥
//    {"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"},//外部认证密钥
//
//    //ADF1
//    {"\xD5\x3A\x54\x37\x86\x3E\x02\x42\x84\xAA\xF4\xC6\x61\x50\xBD\xBB"}, //传输密钥
//    {"\xE9\x07\x36\x3D\xC8\xD2\xC4\x7C\x91\x2A\xE2\x88\x15\x73\x33\xA6"},//主控密钥
//    {"\x74\x6F\xD0\x3B\x2D\xAA\x35\x60\x57\xF9\x75\x1E\x98\x36\x08\x7B"},//应用维护密钥00
//    {"\x7D\xF9\x84\x02\xF4\x7B\xFF\xEB\xC8\xDC\x0E\x33\xA8\x06\x57\x16"},//消费交易的密钥01
//    {"\x4C\x78\x31\x1A\xFC\x8F\x36\xB4\xAC\xAD\xBC\xB2\x61\xD9\xF5\x9A"},//圈存交易的密钥
//    {"\xBB\x47\x8B\xA9\xB0\xD2\x0D\x5F\x99\x48\x8F\x84\xBB\x6D\x62\xB0"},//TAC的密钥
//    {"\x01\x4B\x6B\xC2\xDC\x80\xE8\x25\x60\x89\x78\xB3\xA3\xB3\x24\x50"},//应用维护密钥01
//    {"\x9F\xE2\x7F\xC9\x2B\x0E\xA7\x60\x94\xAC\xA3\xF2\xB5\x6E\x61\x6A"},//应用维护密钥02
//    {"\x69\xA7\x4A\xA1\xE8\xDA\x04\x1D\x85\x5A\x8A\x14\x75\xAB\x22\xBE"},//应用锁定密钥
//    {"\x22\xB0\xAC\xAD\xB9\x9E\x6C\xF3\x29\x79\xDE\xE7\xE4\xCA\xDA\xA7"},//应用解锁密钥
//    {"\x82\xCB\xE1\x95\xC1\x21\xCA\xA3\x10\x09\x04\x50\xEC\x9A\x76\xCF"},//解锁PIN密钥
//    {"\x29\xA4\x02\x00\x13\xE4\x27\xD4\xD0\x27\x5B\xEA\x03\x8B\xBD\x3A"},//重装PIN密钥
//    {"\x42\x2C\xF9\x81\x18\xF7\xF4\x04\xAA\x47\x11\x8C\xF7\x51\xF9\xE7"},//圈提密钥
//    {"\x42\x2C\xF9\x81\x18\xF7\xF4\x04\xAA\x47\x11\x8C\xF7\x51\xF9\xE7"},//修改透支限额密钥
//    {"\x42\x2C\xF9\x81\x18\xF7\xF4\x04\xAA\x47\x11\x8C\xF7\x51\xF9\xE7"},//外部认证密钥01
//    {"\x12\x34\x56\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"}  // pin码
//
//};

// TTICK_KEY l_key=
// {
//  //MF
//  {"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"},
//  {"\x88\x88\x88\x88\x88\x88\x88\x99\x99\x99\x99\x99\x99\x99\x99\x99"},
//  {"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"},
//  //ADF1
//  {"\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"},
//  {"\x01\x88\x88\x88\x88\x88\x88\x99\x99\x99\x99\x99\x12\x34\x56\x78"},
//  {"\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x23\x45\x67\x81"},
//  {"\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x34\x56\x78\x12"},
//  {"\x1B\x2B\x1B\x2B\x1B\x2B\x1B\x2B\x1B\x2B\x1B\x2B\x45\x67\x81\x23"},
//  {"\x1B\x3B\x1B\x3B\x1B\x3B\x1B\x3B\x1B\x3B\x1B\x3B\x56\x78\x12\x34"},
//  {"\x1B\x4B\x1B\x4B\x1B\x4B\x1B\x4B\x1B\x4B\x1B\x4B\x67\x81\x23\x45"},
//  {"\x1B\x5B\x1B\x5B\x1B\x5B\x1B\x5B\x1B\x5B\x1B\x5B\x78\x12\x34\x56"},
//  {"\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x81\x23\x45\x67"},
//  {"\x1C\x1D\x1C\x1D\x1C\x1D\x1C\x1D\x1C\x1D\x1C\x1D\x12\x34\x56\x78"},
//  {"\x1C\x2D\x1C\x2D\x1C\x2D\x1C\x2D\x1C\x2D\x1C\x2D\x23\x45\x67\x81"},
//  {"\x1C\x1E\x1C\x1E\x1C\x1E\x1C\x1E\x1C\x1E\x1C\x1E\x34\x56\x78\x12"},
//  {"\x1C\x2E\x1C\x2E\x1C\x2E\x1C\x2E\x1C\x2E\x1C\x2E\x45\x67\x81\x23"},
//  {"\x1C\x3E\x1C\x3E\x1C\x3E\x1C\x3E\x1C\x3E\x1C\x3E\x56\x78\x12\x34"},
//  {"\x1C\x4E\x1C\x4E\x1C\x4E\x1C\x4E\x1C\x4E\x1C\x4E\x67\x81\x23\x45"},
//  {"\x12\x34\x56"},
//  //ADF2
//  {"\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"},
//  {"\x01\x88\x88\x88\x88\x88\x88\x99\x99\x99\x99\x99\x12\x34\x56\x78"},
//  {"\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x23\x45\x67\x81"},
//  {"\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x34\x56\x78\x12"},
//  {"\x1B\x2B\x1B\x2B\x1B\x2B\x1B\x2B\x1B\x2B\x1B\x2B\x45\x67\x81\x23"},
//  {"\x1B\x3B\x1B\x3B\x1B\x3B\x1B\x3B\x1B\x3B\x1B\x3B\x56\x78\x12\x34"},
//  {"\x1B\x4B\x1B\x4B\x1B\x4B\x1B\x4B\x1B\x4B\x1B\x4B\x67\x81\x23\x45"},
//  {"\x1B\x5B\x1B\x5B\x1B\x5B\x1B\x5B\x1B\x5B\x1B\x5B\x78\x12\x34\x56"},
//  {"\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x81\x23\x45\x67"},
//  {"\x1C\x1D\x1C\x1D\x1C\x1D\x1C\x1D\x1C\x1D\x1C\x1D\x12\x34\x56\x78"},
//  {"\x1C\x2D\x1C\x2D\x1C\x2D\x1C\x2D\x1C\x2D\x1C\x2D\x23\x45\x67\x81"},
//  {"\x1C\x1E\x1C\x1E\x1C\x1E\x1C\x1E\x1C\x1E\x1C\x1E\x34\x56\x78\x12"},
//  {"\x1C\x2E\x1C\x2E\x1C\x2E\x1C\x2E\x1C\x2E\x1C\x2E\x45\x67\x81\x23"},
//  {"\x1C\x3E\x1C\x3E\x1C\x3E\x1C\x3E\x1C\x3E\x1C\x3E\x56\x78\x12\x34"},
//  {"\x1C\x4E\x1C\x4E\x1C\x4E\x1C\x4E\x1C\x4E\x1C\x4E\x67\x81\x23\x45"},
//  {"\x12\x34\x56"}
// };

//static unsigned char l_logicnumber[16]={0x90,0x00,0x00,0x00,0x10,0x00,0x00,0x10};


static TINPUT g_input;

static int function_idx=0;// 函数数组索引, 1=储值卡, 0=章程票

// static unsigned short CRC16_TAB[] = {
//  0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
//  0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
//  0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
//  0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
//  0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
//  0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
//  0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
//  0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
//  0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
//  0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
//  0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
//  0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
//  0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
//  0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
//  0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
//  0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
//  0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
//  0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
//  0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
//  0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
//  0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
//  0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
//  0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
//  0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
//  0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
//  0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
//  0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
//  0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
//  0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
//  0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
//  0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
//  0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
// };
#define CRC_AFFIX  "CS-METRO-2013@GDMH"
//
// unsigned short metro_crc16(uint8_t * lpData, unsigned short nLen)
// {
//  unsigned short wCrc = 0xFFFF;
//  unsigned short n;
//  for (n=0;n<nLen;n++)
//  {
//   wCrc = (wCrc >> 8) ^ CRC16_TAB[(wCrc ^ lpData[n]) & 0xFF];
//  }
//
//  return (unsigned short)(wCrc^0xFFFF);
// }

void add_metro_area_crc(unsigned char * p_metro, unsigned short len_area)
{
    unsigned char data_src[256] = {0};
    unsigned short mtr_crc;
    size_t pos = strlen(CRC_AFFIX);

    memcpy(data_src, CRC_AFFIX, pos);
    memcpy(data_src + pos, p_metro, len_area);

    //mtr_crc = metro_crc16(data_src, pos + len_area - 2);
    mtr_crc=DataSecurity::Crc16(data_src, pos + len_area - 2);
    memcpy(p_metro + len_area - 2, &mtr_crc, 2);
}


//============================================================
//函数名: set_function_idx
//功能     :  配置调用函数类
//入口参数:
//                      idx: 0=调用单程票类函数; 1=调用储值票类
//出口参数: 无
//返回值:      无
//============================================================
void set_function_idx(int idx )
{
    switch (idx)
    {
        case 0:
            function_idx = 0;
            break;
        case 1:
            function_idx = 1;
            break;
        default:
            break;
    }

}



//===============================================================
//函数: data_encrypt
//功能: 对数据项进行加密
//入口参数:
//                      nlen=要进行加密的数据长度
//                   lpdata=要进行加密的数据内容
//                    lpkey=密钥
//出口参数:
//                  lpdata=加密后的数据内容
//返  回   值:
//                  加密后的数据长度
//===============================================================
int data_encrypt(unsigned char nlen, unsigned char *lpdata, unsigned char *lpkey)
{
    const unsigned char filldata[8]={0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char szdata[256];
    int data_len=0;
    unsigned char fill_len=0;
    unsigned char block_len=0;
    unsigned char i,offset;

    memset(szdata, 0x00, sizeof(szdata));
    szdata[0] = nlen;
    data_len = 1;
    memcpy(szdata+data_len, lpdata, nlen);
    data_len += nlen;

    fill_len = data_len % 8;
    block_len = data_len / 8;
    if (fill_len)
    {
        fill_len = 8 - fill_len;
        //不为8  的整倍
        memcpy(szdata+data_len, filldata, fill_len);
        block_len++;

    }
    else
    {
        fill_len = 8;
        memcpy(szdata+data_len, filldata, 8);

    }
    data_len += fill_len;

    //////dbg_dumpmemory("szdata=",szdata, data_len);


    for (i=0, offset=0; i<block_len; i++, offset+=8)
    {
        TripleDes(szdata+offset, szdata+offset, lpkey, ENCRYPT);

    }

    //////dbg_dumpmemory("szdata result =",szdata, data_len);

    //data_len = block_len<<3;
    memcpy(lpdata, szdata, data_len);

    return (data_len);

}

//=============================================
// 函数名:  apdu_getrandom
// 功能:  获取随机数
//入口参数:
//                      lpdata: 接收数据的首址
//                      ngetlen: 取随机数长度
//出口参数"
//                      lpdata: 随机数
//返回值
//              0:成功
//==============================================
int apdu_getrandom(unsigned char *lpdata, int ngetlen)
{

    unsigned char ulen;
    unsigned char respone_len;
    unsigned char sztmp[256];
    unsigned short sam_sw;
    int nresult = -2;


    memcpy(sztmp, "\x00\x84\x00\x00\x08", 5);
    sztmp[4]=ngetlen;
    ulen = 5;
    respone_len = MifareProCom(ulen, sztmp, &sam_sw);
    if ((respone_len==0) || (0x9000!=sam_sw))
    {
        nresult = -2;
    }
    memcpy(lpdata,"\x00\x00\x00\x00\x00\x00\x00\x00",8);

    memcpy(lpdata, sztmp, ngetlen);
    nresult = 0;
    return 0;

}

//int apdu_createfile_key(

//====================================================================
// 函数名:  writeul_direct
// 功能:  将数据直接写入指定页不做任何处理
//入口参数:
//                      nstart_page: 接收数据的首址
//                                 nlen: 取随机数长度
//        lpbuffer: 数据指针
//出口参数"
//                      无
//返回值
//              0:成功
//====================================================================
int writeul_direct(int nstart_page, unsigned char nlen, unsigned char *lpbuffer)
{
    int nresult=-1;
    int i,j;
    unsigned char status = 0;
    int offset_pager=nstart_page;
    int pagecnt;

    pagecnt = nlen / 4;
    if (nlen%4)
    {
        pagecnt++;
    }

    do
    {
        for (i=0,j=0;i<pagecnt;i++,j+=4)
        {
            //////dbg_formatvar("page=%d",offset_pager+i);
            status = ISO14443A_Write_UL(offset_pager+i,lpbuffer+j);
            if (status)
            {
                break;
            }
        }

        if (status == 0) nresult = 0;

    }
    while (0);

    return nresult;
}

//====================================================================
// 函数名:  ul_4page_encyrpt
// 功能:  对单程票数据进行加密及填充CRC码
//入口参数:
//        lpbuffer: 16BYTE 数据指针
//                              initkey: 1BYTE 计算CRC8 的初始值
//                            en_data: 4BYTE 加密值
//出口参数"
//                      无
//返回值
//              0:成功
//====================================================================
void ul_4page_encyrpt(unsigned char *lpbuffer, unsigned char initkey, unsigned char *en_data)
{
    //int nresult=-1;
    int i;//,j;
    //unsigned char status;
    unsigned char crc8;
    //int pagecnt;
    unsigned char en_tmp[4], en_next[4];

    memcpy(en_tmp, en_data, 4);
    ////dbg_dumpmemory("w_key=",en_tmp,4);

    crc8 = generate_crc8(lpbuffer, 15, initkey);
    lpbuffer[15]=crc8;


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

}


//=============================================
// 函数名:  apdu_exteralauth
// 功能:  外部认证
//入口参数:
//                      lprandom: 随机数
//                      lpkey: 16字节的密钥
//出口参数无
//
//返回值
//              0:成功
//==============================================
int apdu_exteralauth(unsigned char *lprandom, unsigned char *lpkey)
{

    unsigned char ulen;
    unsigned char respone_len;
    unsigned char sztmp[256];
    unsigned char szdata[8];
    unsigned short sam_sw;
    int nresult = -2;


    memcpy(szdata, lprandom, 8);


    TripleDes(szdata, szdata, lpkey, ENCRYPT);

    memcpy(sztmp, "\x00\x82\x00\x00\x08",5);
    ulen = 5;
    memcpy(sztmp+5, szdata, 8);
    ulen += 8;
// ////dbg_dumpmemory("externalauth=",sztmp,ulen);
    respone_len = MifareProCom(ulen, sztmp, &sam_sw);
// ////dbg_dumpmemory("externalauth=",sztmp,respone_len);
    if ((respone_len==0) || (0x9000!=sam_sw))
    {
        nresult = -2;
    }

    nresult = 0;
    return 0;


}


//============================================================
//函数名: issue_setvalue
//功能     :  票卡初始化数据的设置(  重编码共用此接口 )
//入口参数:
//                      lpdata: 按照TINPUT 结构排序的数据
//出口参数: 无
//返回值:      无
//============================================================
void setvalue_issue(void *lpdata)
{


    LPTISSUE_INPUT lpinput = (LPTISSUE_INPUT)(lpdata);
    unsigned long ntmp;
    //全局输入参数结构包含了三种业务输入接口的内容
    //赋值时只需赋到相应的变量，在业务调用时直接使用对应数据即可
    //

    memcpy(g_input.logicnumber, lpinput->logicalid, 8);
	//Records::log_buffer("逻辑卡号", lpinput->logicalid, 8);

    g_input.balance = lpinput->lbalance;

    //////dbg_formatvar("balance=%d",g_input.purse);


    ////dbg_dumpmemory("lpinput=",(unsigned char *)lpdata, 325+16);
    ////dbg_dumpmemory("lpinput->szorderno=",(unsigned char *)lpinput->szorderno, 14);
    ////dbg_dumpmemory("lpinput->szapplyno=",(unsigned char *)lpinput->szapplyno, 10);
    ////dbg_dumpmemory("lpinput->bcardtype=",lpinput->bcardtype, 2);
    ////dbg_dumpmemory("g_input.logicnumber=",g_input.logicnumber, 8);
    ////dbg_formatvar("lpinput->bdeposite=%02X",lpinput->bdeposite);
    ////dbg_dumpmemory("lpinput->lbalance=",(unsigned char *)&lpinput->lbalance, 4);
    ////dbg_dumpmemory("lpinput->caps=",(unsigned char *)&lpinput->caps, 2);
    ////dbg_formatvar("lpinput->bactive=%02X",lpinput->bactive);
    ////dbg_dumpmemory("lpinput->issuecode=",lpinput->issuecode, 2);
    ////dbg_dumpmemory("lpinput->citycode=",lpinput->citycode, 2);
    ////dbg_dumpmemory("lpinput->industrycode=",lpinput->industrycode, 2);
    ////dbg_formatvar("lpinput->testflag=%02X",lpinput->testflag);
    ////dbg_dumpmemory("lpinput->issuedate=",lpinput->issuedate, 4);
    ////dbg_dumpmemory("lpinput->cardversion=",lpinput->cardversion, 2);
    ////dbg_dumpmemory("lpinput->effectivefrom=",lpinput->effectivefrom, 4);
    ////dbg_dumpmemory("lpinput->effectiveto=",lpinput->effectiveto, 4);
    ////dbg_formatvar("lpinput->appversion=%02X",lpinput->appversion);
    ////dbg_formatvar("lpinput->passmode=%02X",lpinput->passmode);
    ////dbg_formatvar("lpinput->entryline=%02X%02X",lpinput->entryline[0],lpinput->entryline[1]);
    ////dbg_formatvar("lpinput->exitline=%02X%02X",lpinput->exitline[0],lpinput->exitline[1]);
    ////dbg_dumpmemory("lpinput->mf_tkey=",lpinput->mf_tkey, 16);
//    dbg_dumpmemory("lpinput->mf_mkey=",lpinput->mf_mkey, 16);
    ////dbg_dumpmemory("lpinput->mf_mamk=",lpinput->mf_mamk, 16);
    ////dbg_dumpmemory("lpinput->adf1_tkey=",lpinput->adf1_tkey, 16);
    ////dbg_dumpmemory("lpinput->adf1_mkey=",lpinput->adf1_mkey, 16);
    ////dbg_dumpmemory("lpinput->adf1_mlk_0201=",lpinput->adf1_mlk_0201, 16);
    ////dbg_dumpmemory("lpinput->adf1_mpk_0101=",lpinput->adf1_mpk_0101, 16);
    ////dbg_dumpmemory("lpinput->adf1_mtk=",lpinput->adf1_mtk, 16);
    ////dbg_dumpmemory("lpinput->adf1_mpuk=",lpinput->adf1_mpuk, 16);
    ////dbg_dumpmemory("lpinput->adf1_mrpk=",lpinput->adf1_mprk, 16);
    ////dbg_dumpmemory("lpinput->adf1_mamk_00=",lpinput->adf1_mamk_00, 16);
    ////dbg_dumpmemory("lpinput->adf1_mamk_01=",lpinput->adf1_mamk_01, 16);
    ////dbg_dumpmemory("lpinput->adf1_mamk_02=",lpinput->adf1_mamk_02, 16);
    ////dbg_dumpmemory("lpinput->adf1_pin=",lpinput->adf1_pin, 16);

//    //dbg_dumpmemory("lpinput=",(unsigned char *)lpdata, 325+16);
//     //dbg_dumpmemory("lpinput->szorderno=",(unsigned char *)lpinput->szorderno, 14);
//     //dbg_dumpmemory("lpinput->szapplyno=",(unsigned char *)lpinput->szapplyno, 10);
//     //dbg_dumpmemory("lpinput->bcardtype=",lpinput->bcardtype, 2);
//     //dbg_dumpmemory("g_input.logicnumber=",g_input.logicnumber, 8);
//     //dbg_formatvar("lpinput->bdeposite=%02X",lpinput->bdeposite);
//     //dbg_dumpmemory("lpinput->lbalance=",(unsigned char *)&lpinput->lbalance, 4);
//     //dbg_dumpmemory("lpinput->caps=",(unsigned char *)&lpinput->caps, 2);
//     //dbg_formatvar("lpinput->bactive=%02X",lpinput->bactive);
//     //dbg_dumpmemory("lpinput->issuecode=",lpinput->issuecode, 2);
//     //dbg_dumpmemory("lpinput->citycode=",lpinput->citycode, 2);
//     //dbg_dumpmemory("lpinput->industrycode=",lpinput->industrycode, 2);
//     //dbg_formatvar("lpinput->testflag=%02X",lpinput->testflag);
//     //dbg_dumpmemory("lpinput->issuedate=",lpinput->issuedate, 4);
//     //dbg_dumpmemory("lpinput->cardversion=",lpinput->cardversion, 2);
//     //dbg_dumpmemory("lpinput->effectivefrom=",lpinput->effectivefrom, 4);
//     //dbg_dumpmemory("lpinput->effectiveto=",lpinput->effectiveto, 4);
//     //dbg_formatvar("lpinput->appversion=%02X",lpinput->appversion);
//     //dbg_formatvar("lpinput->passmode=%02X",lpinput->passmode);
//     //dbg_formatvar("lpinput->entryline=%02X%02X",lpinput->entryline[0],lpinput->entryline[1]);
//     //dbg_formatvar("lpinput->exitline=%02X%02X",lpinput->exitline[0],lpinput->exitline[1]);
//     //dbg_dumpmemory("lpinput->mf_tkey=",lpinput->mf_tkey, 16);
//     //dbg_dumpmemory("lpinput->mf_mkey=",lpinput->mf_mkey, 16);
//     //dbg_dumpmemory("lpinput->mf_mamk=",lpinput->mf_mamk, 16);
//     //dbg_dumpmemory("lpinput->adf1_tkey=",lpinput->adf1_tkey, 16);
//     //dbg_dumpmemory("lpinput->adf1_mkey=",lpinput->adf1_mkey, 16);
//     //dbg_dumpmemory("lpinput->adf1_mlk_0201=",lpinput->adf1_mlk_0201, 16);
//     //dbg_dumpmemory("lpinput->adf1_mpk_0101=",lpinput->adf1_mpk_0101, 16);
//     //dbg_dumpmemory("lpinput->adf1_mtk=",lpinput->adf1_mtk, 16);
//     //dbg_dumpmemory("lpinput->adf1_mpuk=",lpinput->adf1_mpuk, 16);
//     //dbg_dumpmemory("lpinput->adf1_mrpk=",lpinput->adf1_mprk, 16);
//     //dbg_dumpmemory("lpinput->adf1_mamk_00=",lpinput->adf1_mamk_00, 16);
//     //dbg_dumpmemory("lpinput->adf1_mamk_01=",lpinput->adf1_mamk_01, 16);
//     //dbg_dumpmemory("lpinput->adf1_mamk_02=",lpinput->adf1_mamk_02, 16);
//     //dbg_dumpmemory("lpinput->adf1_pin=",lpinput->adf1_pin, 16);

//



    memcpy(g_input.key.mf_tkey, lpinput->mf_tkey, 16);
    memcpy(g_input.key.mf_mkey, lpinput->mf_mkey, 16);
    memcpy(g_input.key.mf_mamk, lpinput->mf_mamk, 16);
    memcpy(g_input.key.mf_eak, lpinput->mf_eak, 16);

    memcpy(g_input.key.adf1_tkey, lpinput->adf1_tkey, 16);
    memcpy(g_input.key.adf1_mkey, lpinput->adf1_mkey, 16);
//#ifndef _NEWTICKET_
    memcpy(g_input.key.adf1_mamk_00, lpinput->adf1_mamk_00, 16);
//	#else
//		memcpy(g_input.key.adf1_mamk_01, lpinput->adf1_mamk_00, 16);
//	#endif

    memcpy(g_input.key.adf1_mpk_0101, lpinput->adf1_mpk_0101, 16);
    memcpy(g_input.key.adf1_mlk_0201, lpinput->adf1_mlk_0201, 16);
    memcpy(g_input.key.adf1_mtk, lpinput->adf1_mtk, 16);

//#ifndef _NEWTICKET_
    memcpy(g_input.key.adf1_mamk_01, lpinput->adf1_mamk_01, 16);
//	#else
//		memcpy(g_input.key.adf1_mamk_00, lpinput->adf1_mamk_01, 16);
//	#endif

    memcpy(g_input.key.adf1_mamk_02, lpinput->adf1_mamk_02, 16);

    memcpy(g_input.key.adf1_mabk, lpinput->adf1_mabk, 16);
    memcpy(g_input.key.adf1_mauk, lpinput->adf1_mauk, 16);
    memcpy(g_input.key.adf1_mpuk, lpinput->adf1_mpuk, 16);
    memcpy(g_input.key.adf1_mprk, lpinput->adf1_mprk, 16);
    memcpy(g_input.key.adf1_muk, lpinput->adf1_muk, 16);
    memcpy(g_input.key.adf1_mulk, lpinput->adf1_mulk, 16);
    memcpy(g_input.key.adf1_eak, lpinput->adf1_eak, 16);
    memcpy(g_input.key.adf1_pin, "\x12\x34\x56", 3);


    //===调试时强制赋
//    memcpy(g_input.logicnumber, l_logicnumber, 8);
#ifdef _ENABLE_FIXKEY_
    memcpy((unsigned char *)&g_input.key, (unsigned char *)&l_key, sizeof(l_key));
#endif

    //发行基本信息文件
    memcpy(g_input.inf.issuedate, lpinput->issuedate, 4); //发行日期
    memcpy(g_input.inf.issuecode, lpinput->issuecode, 2);//发卡方代码
    memcpy(g_input.inf.citycode, lpinput->citycode, 2);//城市代码
    memcpy(g_input.inf.industrycode, lpinput->industrycode, 2);//行业代码
	g_input.inf.testflag = lpinput->testflag;					// 测试标记



    memcpy(g_input.inf.cardtype,lpinput->bcardtype, 2);
    memcpy(g_input.inf.issuedate, lpinput->issuedate, 4);
    memcpy(g_input.inf.issuedevice, "\x11\x22\x33\x44\x55\x66", 6);
    memcpy(g_input.inf.cardversion, lpinput->cardversion, 2);




    memcpy(g_input.inf.effectivefrom, lpinput->effectivefrom, 4); //卡启用日期
    memcpy(g_input.inf.effectiveto, lpinput->effectiveto, 4);   //卡有效日期

//add by shiyulong in 2013-12-06
    g_input.inf.deposit = lpinput->bdeposite;
    g_input.inf.saleflag = lpinput->bactive;

    //dbg_dumpmemory("logiceffectivefrom=",lpinput->logiceffectivefrom,7);
    memcpy(g_input.inf.logiceffectivefrom, lpinput->logiceffectivefrom, 7); //逻辑起始时间
    //dbg_formatvar("logicectivetime=%d",lpinput->logicectivetime);
    ntmp = lpinput->logicectivetime * 24 * 60;
    //dbg_formatvar("logicectivetime2=%04X",ntmp);

    g_input.inf.szlogicectivetime[0] = ((ntmp & 0xFF000000) >> 24);
    g_input.inf.szlogicectivetime[1] = ((ntmp & 0x00FF0000) >> 16);
    g_input.inf.szlogicectivetime[2] = ((ntmp & 0x0000FF00) >> 8);
    g_input.inf.szlogicectivetime[3] = (ntmp & 0xFF);

    //dbg_dumpmemory("logiceffectivefrom=",g_input.inf.szlogicectivetime,7);


    g_input.inf.szcaps[0] = ((lpinput->caps & 0xFF00) >> 8); //充值上限
    g_input.inf.szcaps[1] = (lpinput->caps & 0xFF); //充值上限




    //发行基本信息文件
//     memcpy(g_input.inf.issuecode, "\x53\x20", 2);//发卡方代码
//     memcpy(g_input.inf.citycode, "\x41\x00", 2);//城市代码
//     memcpy(g_input.inf.industrycode, "\x00\x00", 2);//城市代码
//     g_input.inf.testflag = 0x01;    // 测试标记
//
//     memcpy(g_input.inf.cardtype,"\x02\x00", 2);
//     memcpy(g_input.inf.issuedate, "\x20\x13\x04\x16", 4);
//     memcpy(g_input.inf.issuedevice, "\x11\x22\x33\x44\x55\x66", 6);
//     memcpy(g_input.inf.cardversion, "\x00\x01", 2);
//
//
//     memcpy(g_input.inf.effectivefrom, "\x20\x13\x06\x01", 4); //卡启用日期
//     memcpy(g_input.inf.effectiveto, "\x20\x19\x12\x31", 4);   //卡有效日期

    //公共应用基本数据

    g_input.inf.appstatus=0x01;
    g_input.inf.appversion=lpinput->appversion;



    memcpy(g_input.inf.appeffectivefrom, lpinput->effectivefrom, 4); //卡启用日期
    memcpy(g_input.inf.appeffectiveto, lpinput->effectiveto, 4);   //卡有效日期


    //公共应用基本数据

//     g_input.inf.appstatus=0x00;
//     g_input.inf.appversion=0x01;
//
//     memcpy(g_input.inf.appeffectivefrom, "\x20\x13\x06\x01", 4); //卡启用日期
//     memcpy(g_input.inf.appeffectiveto, "\x20\x19\x12\x31", 4);   //卡有效日期

    //应用控制
    g_input.inf.saleflag = lpinput->bactive;   //发售激活标记
    g_input.inf.deposit = lpinput->bdeposite;   //发售押金
    memcpy(g_input.inf.saledevice, "\x00\x00\x00\x00\x00\x00", 6);  //发售设备信息
    g_input.inf.passmode = lpinput->passmode;   //出入模式判断
    g_input.inf.entryline=lpinput->entryline[0];   //可入线路
    g_input.inf.entrystation=lpinput->entryline[1];   //可入站点
    g_input.inf.exitline = lpinput->exitline[0];   //可出线路
    g_input.inf.exitstation = lpinput->exitline[1];  //可出站点




    //应用控制
//     g_input.inf.saleflag = 0;   //发售激活标记
//     g_input.inf.deposit = 0;   //发售押金
//     memcpy(g_input.inf.saledevice, "\x00\x00\x00\x00\x00\x00", 6);  //发售设备信息
//     g_input.inf.passmode = 0;   //出入模式判断
//     g_input.inf.entryline = 0;   //可入线路
//     g_input.inf.entrystation = 0; //可入站点
//     g_input.inf.exitline = 0;   //可出线路
//     g_input.inf.exitstation = 0;  //可出站点

    //====================================
    //memcpy(&g_input, lpdata, sizeof(g_input));//正式使用时赋值

}

//============================================================
//函数名: setvalue_evaluate
//功能     :  票卡预赋值数据的设置
//入口参数:
//                      lpdata: 按照TINPUT 结构排序的数据
//出口参数: 无
//返回值:      无
//============================================================
void setvalue_evaluate(void *lpdata)
{

    unsigned long ntmp;
    LPTEVALUATE_INPUT lpinput = (LPTEVALUATE_INPUT)(lpdata);

    //全局输入参数结构包含了三种业务输入接口的内容
    //赋值时只需赋到相应的变量，在业务调用时直接使用对应数据即可
		g_input.inf.deposit = lpinput->bdeposite;
		g_input.inf.saleflag = lpinput->bactive;

    memcpy(g_input.logicnumber, lpinput->logicalid, 8);
    g_input.balance = lpinput->lbalance;

//add by shiyulong in 2013-12-06
    //dbg_dumpmemory("logictime=",lpinput->logiceffectivefrom,7);
    memcpy(g_input.inf.logiceffectivefrom, lpinput->logiceffectivefrom, 7); //逻辑起始时间
    ntmp = lpinput->logicectivetime * 24 * 60;
    g_input.inf.szlogicectivetime[0] = ((ntmp & 0xFF000000) >> 24);
    g_input.inf.szlogicectivetime[1] = ((ntmp & 0x00FF0000) >> 16);
    g_input.inf.szlogicectivetime[2] = ((ntmp & 0x0000FF00) >> 8);
    g_input.inf.szlogicectivetime[3] = (ntmp & 0xFF);

    g_input.inf.szcaps[0] = ((lpinput->caps & 0xFF00) >> 8); //充值上限
    g_input.inf.szcaps[1] = (lpinput->caps & 0xFF); //充值上限



    ////dbg_formatvar("balance=%d",g_input.balance);

    ////dbg_dumpmemory("1=",(unsigned char *)lpdata, 14);
    ////dbg_dumpmemory("2=",(unsigned char *)lpdata+14, 10);
    ////dbg_dumpmemory("3=",(unsigned char *)lpdata+24, 2);
    ////dbg_dumpmemory("4=",(unsigned char *)lpdata+26, 8);
    ////dbg_dumpmemory("5=",(unsigned char *)lpdata+34, 1);
    ////dbg_dumpmemory("6=",(unsigned char *)lpdata+35, 4);



//     memcpy(g_input.key.mf_tkey, lpinput->mf_tkey, 16);
//     memcpy(g_input.key.mf_mkey, lpinput->mf_mkey, 16);
//     memcpy(g_input.key.mf_mamk, lpinput->mf_mamk, 16);
//     memcpy(g_input.key.mf_eak, lpinput->mf_eak, 16);

//     memcpy(g_input.key.adf1_tkey, lpinput->adf1_tkey, 16);
//     memcpy(g_input.key.adf1_mkey, lpinput->adf1_mkey, 16);
//#ifndef _NEWTICKET_

	//dbg_dumpmemory("lpdata=", lpdata,sizeof(TEVALUATE_INPUT));

	//dbg_dumpmemory("adf1_mamk_00=", lpinput->adf1_mamk_00,16);
	//dbg_dumpmemory("adf1_mamk_01=", lpinput->adf1_mamk_01,16);
	//dbg_dumpmemory("adf1_mamk_02=", lpinput->adf1_mamk_02,16);

    memcpy(g_input.key.adf1_mamk_00, lpinput->adf1_mamk_00, 16);
//	#else
//		memcpy(g_input.key.adf1_mamk_00, lpinput->adf1_mamk_01, 16);
//	#endif
    memcpy(g_input.key.adf1_mpk_0101, lpinput->adf1_mpk_0101, 16);
    memcpy(g_input.key.adf1_mlk_0201, lpinput->adf1_mlk_0201, 16);
    memcpy(g_input.key.adf1_mtk, lpinput->adf1_mtk, 16);

//#ifndef _NEWTICKET_
    memcpy(g_input.key.adf1_mamk_01, lpinput->adf1_mamk_01, 16);
//	#else
//		memcpy(g_input.key.adf1_mamk_01, lpinput->adf1_mamk_00, 16);
//	#endif

    memcpy(g_input.key.adf1_mamk_02, lpinput->adf1_mamk_02, 16);


//     memcpy(g_input.key.adf1_mabk, lpinput->adf1_mabk, 16);
//     memcpy(g_input.key.adf1_mauk, lpinput->adf1_mauk, 16);
//     memcpy(g_input.key.adf1_mpuk, lpinput->adf1_mpuk, 16);
//     memcpy(g_input.key.adf1_mprk, lpinput->adf1_mprk, 16);
//     memcpy(g_input.key.adf1_muk, lpinput->adf1_muk, 16);
//     memcpy(g_input.key.adf1_mulk, lpinput->adf1_mulk, 16);
//     memcpy(g_input.key.adf1_eak, lpinput->adf1_eak, 16);
//     memcpy(g_input.key.adf1_pin, lpinput->adf1_pin, 16);


    //===调试时强制赋
//    memcpy(g_input.logicnumber, l_logicnumber, 8);
#ifdef _ENABLE_FIXKEY_
    memcpy((unsigned char *)&g_input.key, (unsigned char *)&l_key, sizeof(l_key));
#endif

    //发行基本信息文件
//    memcpy(g_input.inf.issuecode, lpinput->issuecode, 2);//发卡方代码
//    memcpy(g_input.inf.citycode, lpinput->citycode, 2);//城市代码
//    memcpy(g_input.inf.industrycode, lpinput->industrycode, 2);//行业代码




//     memcpy(g_input.inf.cardtype,lpinput->bcardtype, 2);
//     memcpy(g_input.inf.issuedate, lpinput->issuedate, 4);
//     memcpy(g_input.inf.issuedevice, "\x11\x22\x33\x44\x55\x66", 6);
//     memcpy(g_input.inf.cardversion, lpinput->cardversion, 2);




//     memcpy(g_input.inf.effectivefrom, lpinput->effectivefrom, 4); //卡启用日期
//     memcpy(g_input.inf.effectiveto, lpinput->effectiveto, 4);   //卡有效日期




    //发行基本信息文件
//     memcpy(g_input.inf.issuecode, "\x53\x20", 2);//发卡方代码
//     memcpy(g_input.inf.citycode, "\x41\x00", 2);//城市代码
//     memcpy(g_input.inf.industrycode, "\x00\x00", 2);//城市代码
//     g_input.inf.testflag = 0x01;    // 测试标记
//
//     memcpy(g_input.inf.cardtype,"\x02\x00", 2);
//     memcpy(g_input.inf.issuedate, "\x20\x13\x04\x16", 4);
//     memcpy(g_input.inf.issuedevice, "\x11\x22\x33\x44\x55\x66", 6);
//     memcpy(g_input.inf.cardversion, "\x00\x01", 2);
//
//
//     memcpy(g_input.inf.effectivefrom, "\x20\x13\x06\x01", 4); //卡启用日期
//     memcpy(g_input.inf.effectiveto, "\x20\x19\x12\x31", 4);   //卡有效日期

    //公共应用基本数据

//     g_input.inf.appstatus=0x01;
//     g_input.inf.appversion=lpinput->appversion;
//
//
//
//     memcpy(g_input.inf.appeffectivefrom, lpinput->effectivefrom, 4); //卡启用日期
//     memcpy(g_input.inf.appeffectiveto, lpinput->effectiveto, 4);   //卡有效日期


    //公共应用基本数据

//     g_input.inf.appstatus=0x00;
//     g_input.inf.appversion=0x01;
//
//     memcpy(g_input.inf.appeffectivefrom, "\x20\x13\x06\x01", 4); //卡启用日期
//     memcpy(g_input.inf.appeffectiveto, "\x20\x19\x12\x31", 4);   //卡有效日期

    //应用控制
//     g_input.inf.saleflag = lpinput->bactive;   //发售激活标记
//     g_input.inf.deposit = lpinput->bdeposite;   //发售押金
    memcpy(g_input.inf.saledevice, "\x00\x00\x00\x00\x00\x00", 6);  //发售设备信息
    g_input.inf.passmode = lpinput->passmode;   //出入模式判断
    g_input.inf.entryline=lpinput->entryline[0];   //可入线路
    g_input.inf.entrystation=lpinput->entryline[1];   //可入站点
    g_input.inf.exitline = lpinput->exitline[0];   //可出线路
    g_input.inf.exitstation = lpinput->exitline[1];  //可出站点




    //应用控制
//     g_input.inf.saleflag = 0;   //发售激活标记
//     g_input.inf.deposit = 0;   //发售押金
//     memcpy(g_input.inf.saledevice, "\x00\x00\x00\x00\x00\x00", 6);  //发售设备信息
//     g_input.inf.passmode = 0;   //出入模式判断
//     g_input.inf.entryline = 0;   //可入线路
//     g_input.inf.entrystation = 0; //可入站点
//     g_input.inf.exitline = 0;   //可出线路
//     g_input.inf.exitstation = 0;  //可出站点

    //====================================
    //memcpy(&g_input, lpdata, sizeof(g_input));//正式使用时赋值

}

//============================================================
//函数名: setvalue_evaluate
//功能     :  票卡预赋值数据的设置
//入口参数:
//                      lpdata: 按照TINPUT 结构排序的数据
//出口参数: 无
//返回值:      无
//============================================================
void setvalue_destroy(void *lpdata)
{

//  订单号 14
//  申请编号 10
//  车票类型 2
//  应用序列号 8
//  押金 1
//  钱包值 4
//  ADF1消费密钥01 16
//  ADF1的TAC密钥 16
//  ADF1应用维护密钥 16
//  ADF1应用维护密钥01 16
//  ADF1应用维护密钥02 16


    LPTDESTROY_INPUT lpinput = (LPTDESTROY_INPUT)(lpdata);

    //全局输入参数结构包含了三种业务输入接口的内容
    //赋值时只需赋到相应的变量，在业务调用时直接使用对应数据即可

    memcpy(g_input.logicnumber, lpinput->logicalid, 8);
    g_input.balance = lpinput->lbalance;

    memcpy(g_input.key.adf1_mpk_0101, lpinput->adf1_mpk_0101, 16);
    memcpy(g_input.key.adf1_mtk, lpinput->adf1_mtk, 16);
    memcpy(g_input.key.adf1_mamk_00, lpinput->adf1_mamk_00, 16);
    memcpy(g_input.key.adf1_mamk_01, lpinput->adf1_mamk_01, 16);
    memcpy(g_input.key.adf1_mamk_02, lpinput->adf1_mamk_02, 16);



    //===调试时强制赋
//    memcpy(g_input.logicnumber, l_logicnumber, 8);
#ifdef _ENABLE_FIXKEY_
    memcpy((unsigned char *)&g_input.key, (unsigned char *)&l_key, sizeof(l_key));
#endif

    //发行基本信息文件
//     memcpy(g_input.inf.issuecode, lpinput->issuecode, 2);//发卡方代码
//     memcpy(g_input.inf.citycode, lpinput->citycode, 2);//城市代码
//     memcpy(g_input.inf.industrycode, lpinput->industrycode, 2);//行业代码




//     memcpy(g_input.inf.cardtype,lpinput->bcardtype, 2);
//     memcpy(g_input.inf.issuedate, lpinput->issuedate, 4);
//     memcpy(g_input.inf.issuedevice, "\x11\x22\x33\x44\x55\x66", 6);
//     memcpy(g_input.inf.cardversion, lpinput->cardversion, 2);




//     memcpy(g_input.inf.effectivefrom, lpinput->effectivefrom, 4); //卡启用日期
//     memcpy(g_input.inf.effectiveto, lpinput->effectiveto, 4);   //卡有效日期




    //发行基本信息文件
//     memcpy(g_input.inf.issuecode, "\x53\x20", 2);//发卡方代码
//     memcpy(g_input.inf.citycode, "\x41\x00", 2);//城市代码
//     memcpy(g_input.inf.industrycode, "\x00\x00", 2);//城市代码
//     g_input.inf.testflag = 0x01;    // 测试标记
//
//     memcpy(g_input.inf.cardtype,"\x02\x00", 2);
//     memcpy(g_input.inf.issuedate, "\x20\x13\x04\x16", 4);
//     memcpy(g_input.inf.issuedevice, "\x11\x22\x33\x44\x55\x66", 6);
//     memcpy(g_input.inf.cardversion, "\x00\x01", 2);
//
//
//     memcpy(g_input.inf.effectivefrom, "\x20\x13\x06\x01", 4); //卡启用日期
//     memcpy(g_input.inf.effectiveto, "\x20\x19\x12\x31", 4);   //卡有效日期

    //公共应用基本数据

//     g_input.inf.appstatus=0x01;
//     g_input.inf.appversion=lpinput->appversion;



//     memcpy(g_input.inf.appeffectivefrom, lpinput->effectivefrom, 4); //卡启用日期
//     memcpy(g_input.inf.appeffectiveto, lpinput->effectiveto, 4);   //卡有效日期


    //公共应用基本数据

//     g_input.inf.appstatus=0x00;
//     g_input.inf.appversion=0x01;
//
//     memcpy(g_input.inf.appeffectivefrom, "\x20\x13\x06\x01", 4); //卡启用日期
//     memcpy(g_input.inf.appeffectiveto, "\x20\x19\x12\x31", 4);   //卡有效日期

    //应用控制
//     g_input.inf.saleflag = lpinput->bactive;   //发售激活标记
//     g_input.inf.deposit = lpinput->bdeposite;   //发售押金
    memcpy(g_input.inf.saledevice, "\x00\x00\x00\x00\x00\x00", 6);  //发售设备信息
//     g_input.inf.passmode = lpinput->passmode;   //出入模式判断
//     g_input.inf.entryline=lpinput->entryline[0];   //可入线路
//     g_input.inf.entrystation=lpinput->entryline[1];   //可入站点
//     g_input.inf.exitline = lpinput->exitline[0];   //可出线路
//     g_input.inf.exitstation = lpinput->exitline[1];  //可出站点




    //应用控制
//     g_input.inf.saleflag = 0;   //发售激活标记
//     g_input.inf.deposit = 0;   //发售押金
//     memcpy(g_input.inf.saledevice, "\x00\x00\x00\x00\x00\x00", 6);  //发售设备信息
//     g_input.inf.passmode = 0;   //出入模式判断
//     g_input.inf.entryline = 0;   //可入线路
//     g_input.inf.entrystation = 0; //可入站点
//     g_input.inf.exitline = 0;   //可出线路
//     g_input.inf.exitstation = 0;  //可出站点

    //====================================
    //memcpy(&g_input, lpdata, sizeof(g_input));//正式使用时赋值

}

#ifndef _NEWTICKET_

//储值票删除票卡原有结构
int svt_reset(void)
{
    unsigned char szdata[8];
    unsigned char szcmd[256];
    unsigned char respone_len;
    unsigned short sam_sw;
    unsigned char ulen;
    int nresult = -2;

    do
    {

        nresult=svt_selectfile(0x3F00);
// ////dbg_formatvar((char *)"svt_selectfile=%0d", nresult);
        if (nresult<0)
        {
            nresult = 4;//ERR_CARD_READ;
        };

        if (nresult)
        {
            break;
        }


        if (apdu_getrandom(szdata,8)==0)
        {
            if (apdu_exteralauth(szdata,g_input.key.mf_mkey)==0)
            {
                nresult = 0;
            }
            else
                nresult = 1;

//  ////dbg_formatvar((char *)"exteralauth=%0d", nresult);
        }

        if (nresult)
        {
            break;
        }


        if (apdu_getrandom(szdata,4)==0)
        {

            memset(szcmd, 0x00, sizeof(szcmd));
            memcpy(szcmd, "\x84\x0E\x00\x00\x18",5);
            ulen = 5;
            memcpy(szcmd+ulen, "\xF0\x02\x91\xFF",4);
            ulen += 4;
            memcpy(szcmd+ulen, g_input.key.mf_tkey, 16);
            ulen += 16;
            MAC_3(g_input.key.mf_mkey, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //////dbg_dumpmemory("delete mf =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //////dbg_dumpmemory("delete mf |=",szcmd,respone_len);

            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 2;
            }

            if (0x9000==sam_sw)
            {
                nresult = 0;
            }
            //////dbg_formatvar((char *)"delete=%0d", nresult);
        }

    }
    while (0);

    return nresult;

}


//储值票创建主目录
int svt_create_mf(void)
{
    unsigned char ulen;
    unsigned char respone_len;
    unsigned char sztmp[256];
    unsigned char szcmd[256];
    unsigned char szdata[8];
    unsigned char tmp_len;
    unsigned short sam_sw;
    LPTMF_ISSUE lpmfissueinf;
    int nresult = 0;

    do
    {

        //认证传输密钥
        if (apdu_getrandom(szdata,8)==0)
        {
            if (apdu_exteralauth(szdata,g_input.key.mf_tkey))
            {
                nresult = 1;
                break;
            }
        }

        if (nresult) break;

        //创建MF(PBOC MF:1PAY.SYS.DDF01)(MF的文件名长度为0E，不能修改)
        //AID=3F00
        //MF空间=1A00
        //文件属性=5E:
        //DF有效性=01（有效）
        //钱包初始化使能位=0（不初始化）
        //删除DF使能=1（DF可删除）
        //DF下是否具有FCI文件=1（具有FCI文件）
        //创建DF下文件是否需要加MAC=1（需要）；
        //是否PBOC应用=1（强制为1）
        //钱包消费是否追加交易记录=0(不追加)；
        //创建权限=91(创建MF下的EF前需要进行外部认证)
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd, "\x04\xE0\x00\x38\x1E\x3F\x00\x38\x1A\x00\x5E\x00\x00\x00\x00\x91\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", 31);
            ulen = 31;
            MAC_3(g_input.key.mf_tkey, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            ////dbg_dumpmemory("create mf =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            ////dbg_dumpmemory("create mf |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 2;
                break;
            }

        }

        memcpy(szcmd, "\x00\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", 19);
        ulen = 19;
        //////dbg_dumpmemory("select mf =",szcmd,ulen);
        respone_len = MifareProCom(ulen, szcmd, &sam_sw);
        //////dbg_dumpmemory("select mf |=",szcmd,respone_len);
        if ((respone_len==0) || (0x9000!=sam_sw))
        {
            nresult = 3;
            break;
        }

        //认证传输密钥
        if (apdu_getrandom(szdata,8)==0)
        {
            if (apdu_exteralauth(szdata,g_input.key.mf_tkey))
            {
                nresult = 4;
                break;
            }
        }

        //FID=0000 创建MF下的KEY文件
        //可创建18h条密钥，每条密钥记录长度14h
        //文件属性=C0（写至少需要MAC，写至少需要加密）
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd, "\x04\xE0\x00\x08\x0F\x00\x00\x08\x18\x14\xC0\x00\x00\x00\x01\x01", 16);
            ulen = 16;
            MAC_3(g_input.key.mf_tkey, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //////dbg_dumpmemory("create mfkey0000=",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //////dbg_dumpmemory("create mfkey0000 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 5;
                break;
            }

        }

        //DCCK  卡片主控密钥
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x01\x1C", 5);
            ulen = 5;
            memcpy(sztmp,"\x10\x01\x01\x33",4);
            memcpy(sztmp+4, g_input.key.mf_mkey, 16);
            tmp_len = data_encrypt(20, sztmp, g_input.key.mf_tkey);


            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.mf_tkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //////dbg_dumpmemory("create  mf dcck =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //////dbg_dumpmemory("create  mf dcck |=",szcmd,respone_len);

            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 6;
                break;
            }
        }

        //DCMK  卡片应用维护密钥
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x02\x1C", 5);
            ulen = 5;
            memcpy(sztmp,"\x30\x01\x00\x33",4);
            memcpy(sztmp+4, g_input.key.mf_mamk, 16);
            tmp_len = data_encrypt(20, sztmp, g_input.key.mf_mkey);


            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.mf_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //////dbg_dumpmemory("create mf dcmk =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //////dbg_dumpmemory("create mf dcmk |=",szcmd,respone_len);

            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 7;
                break;
            }
        }


        //FID=0001 创建目录信息文件 DCMK
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd, "\x04\xE0\x00\x05\x0F\x00\x01\x05\x01\x00\x08\x00\x00\x00\x00\x00", 16);
            ulen = 16;
            MAC_3(g_input.key.mf_mkey, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //////dbg_dumpmemory("create FID0001=",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //////dbg_dumpmemory("create FID0001 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 8;
                break;
            }

        }

        //更新目录信息0001
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd, "\x04\xDC\x01\x0C\x1C", 5);
            ulen = 5;
            memcpy(szcmd+ulen, "\x61\x11\x4F\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01\x50\x09\x43\x53\x5F\x4D\x54\x52\x5F\x30\x31", 24);
            ulen += 24;
            MAC_3(g_input.key.mf_mamk, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //////dbg_dumpmemory("update FID0000=",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //////dbg_dumpmemory("update FID0000 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 9;
                break;
            }

        }


        //;FID=0005 创建发卡方基本信息文件  DCMK控制
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd, "\x04\xE0\x00\x01\x0F\x00\x05\x01\x00\x28\x08\x00\x00\x00\x00\x00", 16);
            ulen = 16;
            MAC_3(g_input.key.mf_mkey, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //////dbg_dumpmemory("create FID0005=",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //////dbg_dumpmemory("create FID0005 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 10;
                break;
            }

        }

        //更新发卡方信息文件0005
        if (apdu_getrandom(szdata,4)==0)
        {

            memset(szcmd, 0x00, sizeof(szcmd));

            memcpy(szcmd, "\x04\xD6\x85\x00\x2C", 5);
            ulen = 5;
            lpmfissueinf = (LPTMF_ISSUE)(szcmd+ulen);

            memcpy(lpmfissueinf->issuecode, g_input.inf.issuecode, 2);//发卡方代码
            memcpy(lpmfissueinf->citycode, g_input.inf.citycode, 2);//城市代码
            memcpy(lpmfissueinf->industrycode, g_input.inf.industrycode, 2);//城市代码
            lpmfissueinf->testflag = g_input.inf.testflag;      // 测试标记
            memcpy(lpmfissueinf->logicnumber, g_input.logicnumber, 8); //逻辑卡号
            memcpy(lpmfissueinf->cardtype,g_input.inf.cardtype, 2);
            memcpy(lpmfissueinf->issuedate, g_input.inf.issuedate, 4);
            memcpy(lpmfissueinf->issuedevice, g_input.inf.issuedevice, 6);
            memcpy(lpmfissueinf->cardversion, g_input.inf.cardversion, 2);


            memcpy(lpmfissueinf->effectivefrom, g_input.inf.effectivefrom, 4);
            memcpy(lpmfissueinf->effectiveto, g_input.inf.effectiveto, 4);


            //    memcpy(lpmfissueinf->issuecode, "\x53\x20", 2);//发卡方代码
            //    memcpy(lpmfissueinf->citycode, "\x41\x00", 2);//城市代码
            //    memcpy(lpmfissueinf->industrycode, "\x00\x00", 2);//城市代码
            //    lpmfissueinf->testflag = 0x01;                // 测试标记
            //    memcpy(lpmfissueinf->logicnumber, "\x90\x00\x00\x00\x10\x00\x00\x10", 8); //逻辑卡号
            //    memcpy(lpmfissueinf->cardtype,"\x02\x00", 2);
            //    memcpy(lpmfissueinf->issuedate, "\x20\x13\x04\x16", 4);
            //    memcpy(lpmfissueinf->issuedevice, "\x11\x22\x33\x44\x55\x66", 6);
            //    memcpy(lpmfissueinf->cardversion, "\x00\x01", 2);
            //
            //
            //    memcpy(lpmfissueinf->effectivefrom, "\x20\x13\x06\x01", 4);
            //    memcpy(lpmfissueinf->effectiveto, "\x20\x19\x12\x31", 4);
            //
            //    memcpy(lpmfissueinf->effectivefrom, "\x20\x13\x06\x01", 4);//应用启用日期
            //    memcpy(lpmfissueinf->effectiveto, "\x20\x19\x12\x31", 4);//应用结束日期

            ulen += sizeof(TMF_ISSUE);
            MAC_3(g_input.key.mf_mamk, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //////dbg_dumpmemory("update FID0005=",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //////dbg_dumpmemory("update FID0005 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 11;
                break;
            }

        }


        //FID=EF0D，创建FCI文件
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd, "\x04\xE0\x00\x01\x0F\xEF\x0D\x01\x00\x03\x08\x00\x00\x00\x00\x00", 16);
            ulen = 16;
            MAC_3(g_input.key.mf_mkey, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //////dbg_dumpmemory("create MF FCI  =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //////dbg_dumpmemory("create MF FCI |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 12;
                break;
            }

        }

        //更新FCI文件EF0D
        //88(目录基本文件的SFI标签) 01(长度) 01(目录基本文件的SFI)

        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd, "\x04\xD6\x8D\x00\x07\x88\x01\x01", 8);
            ulen = 8;
            MAC_3(g_input.key.mf_mamk, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //////dbg_dumpmemory("update MF FCI  =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //////dbg_dumpmemory("update MF FCI |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 13;
                break;
            }

        }




    }
    while (0);

    return nresult;

}


//储值票创建应用目录
int svt_create_adf1(void)
{

    unsigned char ulen;
    unsigned char respone_len;
    unsigned char sztmp[256];
    unsigned char szcmd[256];
    unsigned char szdata[9];
    unsigned char tmp_len;
    unsigned short respone_sw;
    unsigned char keyidx=1;
    LPTPUBLICINF lppublic;

    unsigned long value;
    InitforLoadStr_t InitforLoad;


    unsigned char moneybuf[4];
    unsigned char calMacKey[8];
    unsigned char m_datetime[8];
    unsigned char inf[100];


    int i;

    //对应7 个复合记录, 数据分别为标识和长度
    unsigned char complex_record[7][2]=
    {
        {0x01,0x30},{0x02,0x30},{0x03,0x30},{0x04,0x30},{0x05,0x30},{0x09,0x30},{0x11,0x18}
    };

    int nresult = 0;


    do
    {

        memcpy(m_datetime, CmdSort::m_time_now, 7);

        //注册ADF1
        memcpy(sztmp, "\x00\xEE\x01\x38\x20",5);
        ulen = 5;
        memcpy(sztmp+ulen, "\x10\x01\x38\x07\xC0\x32\x00\x00\x00\x00\x11\x09\xF0\x01\x01\x33", 16);
        ulen += 16;
        memcpy(sztmp+ulen, g_input.key.adf1_tkey, 16);
        ulen += 16;
        //////dbg_dumpmemory("register adf1 =",sztmp,ulen);
        respone_len = MifareProCom(ulen, sztmp, &respone_sw);
        //////dbg_dumpmemory("register adf1 |=",sztmp,respone_len);
        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 1;
            //break;
        }

        //认证ADF1的传输密钥
        if (apdu_getrandom(szdata,8)==0)
        {
            if (apdu_exteralauth(szdata,g_input.key.adf1_tkey))
            {
                nresult = 2;
                break;
            }
        }


        //创建ADF1
        if (apdu_getrandom(szdata,8)==0)
        {

            //////dbg_dumpmemory("random =",szdata,8);
            memcpy(sztmp, "\x04\xE0\x00\x38\x19\x10\x01\x38\x05\xC0\x7B\x00\x00\x00\x00\x11\x09\xF0\x00\x00\x41\x00\x53\x20\xAD\xF1", 26);
            memcpy(szcmd, "\x04\xE0\x00\x38\x19\x10\x01\x38\x05\xC0\x7B\x00\x00\x00\x00\x11\x09\xF0\x00\x00\x41\x00\x53\x20\xAD\xF1", 26);
            ulen = 26;

            MAC_3(g_input.key.adf1_tkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //////dbg_dumpmemory("create adf1 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //////dbg_dumpmemory("create adf1 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 3;
                break;
            }

        }

        //创建密钥文件0000
        memcpy(szcmd, "\x00\xE0\x00\x08\x0B\x00\x00\x08\x18\x14\xC0\x00\x00\x00\x01\x01", 16);
        ulen = 16;
        //////dbg_dumpmemory("create key0000 =",szcmd,ulen);
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //////dbg_dumpmemory("create key0000 |=",szcmd,respone_len);
        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 3;
            break;
        }

        //创建adf1 主控密钥10 (外部认证密钥)
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x01\x1C", 5);
            szcmd[3]=keyidx++;
            ulen = 5;
            memcpy(sztmp,"\x10\x01\x01\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mkey, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_tkey);

            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_tkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //////dbg_dumpmemory("create adf1 key_mkey =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //////dbg_dumpmemory("create adf1 key_mkey |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 4;
                break;
            }

        }

        // ADF1 创建消费密钥=====================
        //密钥类型: 40 (消费密钥)
        //版本: 01
        //序号: 01-05
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x02\x1C", 5);
            szcmd[3]=keyidx++;

            ulen = 5;
            memcpy(sztmp,"\x40\x01\x01\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mpk_0101, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);


            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //////dbg_dumpmemory("create adf1 MPK_0101 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //////dbg_dumpmemory("create adf1 MPK_0101 |=",szcmd,respone_len);

            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 5;
                break;
            }
        }


        // ADF1 创建圈存密钥=====================
        //密钥类型: E0 (圈存密钥)
        //版本: 02
        //序号: 01
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x07\x1C", 5);
            szcmd[3]=keyidx++;

            ulen = 5;
            memcpy(sztmp,"\xE0\x02\x01\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mlk_0201, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);


            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //////dbg_dumpmemory("create adf1 MLK_0101 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //////dbg_dumpmemory("create adf1 MLK_0101 |=",szcmd,respone_len);

            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 6;
                break;
            }
        }


        // ADF1 创建TAC 密钥=====================
        //密钥类型: 60 (圈存密钥)
        //版本: 01
        //序号: 01
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x08\x1C", 5);
            szcmd[3]=keyidx++;

            ulen = 5;
            memcpy(sztmp,"\x60\x01\x01\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mtk, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);

            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //////dbg_dumpmemory("create adf1 MTK_0101 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //////dbg_dumpmemory("create adf1 MTK_0101 |=",szcmd,respone_len);

            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 7;
                break;
            }
        }

        // ADF1 创建维护 密钥=====================
        //密钥类型: 30 (维护密钥)
        //版本: 01
        //序号: 00 - 03
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x09\x1C", 5);
            szcmd[3]=keyidx++;

            ulen = 5;
            memcpy(sztmp,"\x30\x01\x00\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mamk_00, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);


            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //////dbg_dumpmemory("create adf1 MAMK_0100 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //////dbg_dumpmemory("create adf1 MAMK_0100 |=",szcmd,respone_len);

            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 8;
                break;
            }
        }


        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x0A\x1C", 5);
            szcmd[3]=keyidx++;

            ulen = 5;
            memcpy(sztmp,"\x30\x01\x01\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mamk_01, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);


            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //////dbg_dumpmemory("create adf1 MAMK_0101 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //////dbg_dumpmemory("create adf1 MAMK_0101 |=",szcmd,respone_len);

            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 9;
                break;
            }
        }

        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x0B\x1C", 5);
            szcmd[3]=keyidx++;

            ulen = 5;
            memcpy(sztmp,"\x30\x01\x02\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mamk_02, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);


            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //////dbg_dumpmemory("create adf1 MAMK_0102 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //////dbg_dumpmemory("create adf1 MAMK_0102 |=",szcmd,respone_len);

            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 10;
                break;
            }
        }


        // ADF1 创建PIN =====================
        //密码固定: 08 00 01
        //解锁次数: 05
        //密码长度: 03
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x0D\x14", 5);
            szcmd[3]=keyidx++;

            ulen = 5;
            memcpy(sztmp,"\x08\x00\x00\x55\x03",5);
//            memcpy(sztmp+5, g_input.key.adf1_pin, 3);
            memcpy(sztmp+5, "\x12\x34\x56", 3);
            tmp_len = data_encrypt(8,sztmp,g_input.key.adf1_mkey);


            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //////dbg_dumpmemory("create adf1 PIN =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //////dbg_dumpmemory("create adf1 PIN |=",szcmd,respone_len);

            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 11;
                break;
            }
        }


        // ADF1 创建PIN  解锁密钥=====================
        //密钥固定: 20
        //版本: 01
        //序号: 02 （见1108的WRITE KEY/PIN指令说明）
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x0E\x1C", 5);
            szcmd[3]=keyidx;// 不加1

            ulen = 5;
            memcpy(sztmp,"\x20\x01\x02\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mpuk, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);


            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //////dbg_dumpmemory("create adf1 MPUK_0100 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //////dbg_dumpmemory("create adf1 MPUK_0100 |=",szcmd,respone_len);

            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 12;
                break;
            }
        }


        // ADF1 创建PIN 重装密钥=====================
        //密钥固定: 20
        //版本: 01
        //序号: 03 （见1108的WRITE KEY/PIN指令说明）
        if (apdu_getrandom(szdata,4)==0)
        {
            memcpy(szcmd,"\x84\xF0\x00\x0E\x1C", 5);
            szcmd[3]=keyidx;

            ulen = 5;
            memcpy(sztmp,"\x20\x01\x03\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mprk, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);


            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //////dbg_dumpmemory("create adf1 MRPK_0100 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //////dbg_dumpmemory("create adf1 MRPK_0100 |=",szcmd,respone_len);

            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 13;
                break;
            }
        }


        //0011  应用控制文件
        //二进制文件(01)，文件属性=0A
        //读自由，维护密钥00更新：文件属性=0A，读权限=00，写权限=00
        memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x11\x01\x00\x20\x0A\x00\x00\x00\x00\x00", 16);
        ulen = 16;
        //dbg_dumpmemory("create adf1 file_0011 =",szcmd,ulen);
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //////dbg_dumpmemory("create adf1 file_0011 |=",szcmd,respone_len);

        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 14;
            break;
        }


        //更新0011应用控制文件
        if (apdu_getrandom(szdata,4)==0)
        {
            memset(inf, 0x00, sizeof(inf));
            memset(szcmd, 0x00, sizeof(szcmd));
//    ulen = 0;
//             memcpy(szcmd, "\x04\xD6\x91\x00\x24",5);
//             ulen += 5;
//    ulen++;//激活标记
//    szcmd[ulen]=g_input.inf.deposit;
//    ulen++;
//    ulen += 6; //发售设备信息
//
//    szcmd[ulen]=g_input.inf.passmode;
//    ulen++;
//    szcmd[ulen]=g_input.inf.entryline;
//    ulen++;
//    szcmd[ulen]=g_input.inf.entrystation;
//    ulen++;
//    szcmd[ulen]=g_input.inf.exitline;
//    ulen++;
//    szcmd[ulen]=g_input.inf.exitstation;
//    ulen++;
//    ulen = 32+5;
//
//
//             MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);
//             ulen += 4;
//             //////dbg_dumpmemory("update adf1 file_0011 =",szcmd,ulen);
//             respone_len = MifareProCom(ulen, szcmd, &respone_sw);
//             //dbg_dumpmemory("update adf1 file_0011 |=",szcmd,respone_len);
//====================================================
//   inf[1]=g_input.inf.deposit;
//   inf[8]=g_input.inf.passmode;
//   inf[9]=g_input.inf.entryline;
//   inf[10]=g_input.inf.entrystation;
//   inf[11]=g_input.inf.exitline;
//   inf[12]=g_input.inf.exitstation;
            ulen = 0;
            inf[ulen++] = g_input.inf.saleflag;
            memcpy(inf+ulen,g_input.inf.szlogicectivetime, 4);
            ulen += 4;
            inf[ulen++]=g_input.inf.deposit;
            //====
            ulen += 6; //发售设备信息
            //====
            memcpy(inf+ulen, g_input.inf.szcaps, 2); //充值上限
            ulen += 2;

            inf[ulen++]=g_input.inf.passmode;     //通讯模式
            inf[ulen++]=g_input.inf.entryline;    //可入线路
            inf[ulen++]=g_input.inf.entrystation; //可入站点
            inf[ulen++]=g_input.inf.exitline;     //可出线路
            inf[ulen++]=g_input.inf.exitstation;  //可出站点


            memset(szcmd, 0x00, sizeof(szcmd));
            ulen = 0;
            memcpy(szcmd, "\x04\xD6\x91\x00\x24",5);
            ulen += 5;
            memcpy(szcmd+ulen, inf, LEN_APP_CTRL);
            ulen += LEN_APP_CTRL;

            MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //dbg_dumpmemory("update adf1 file_0011 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 15;
                break;
            }

        }

        //0012   保留文件
        //二进制文件(01)，文件属性=08
        //读自由，维护密钥00更新：文件属性=08，读权限=00，写权限=00
        memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x12\x01\x00\x20\x0A\x00\x00\x00\x00\x00", 16);
        //////dbg_dumpmemory("create adf1 file_0012 =",szcmd,ulen);
        ulen =16;
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //////dbg_dumpmemory("create adf1 file_0012 |=",szcmd,respone_len);
        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 16;
            break;
        }



        //0015   创建公共应用基本文件
        //二进制文件(01)，文件属性=09
        //读自由，维护密钥00更新：文件属性=09，读权限=00，写权限=00
        memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x15\x01\x00\x1E\x09\x00\x00\x00\x00\x00", 16);
        //////dbg_dumpmemory("create adf1 file_0015 =",szcmd,ulen);
        ulen =16;
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //////dbg_dumpmemory("create adf1 file_0015 |=",szcmd,respone_len);
        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 17;
            break;
        }


        //更新0015公共应用基本文件
        //注意更新逻辑卡号 sDData = 9000000010000010
        if (apdu_getrandom(szdata,4)==0)
        {
            memset(szcmd, 0x00, sizeof(szcmd));
            memcpy(szcmd, "\x04\xD6\x95\x00\x22",5);
            ulen = 5;

            lppublic = (LPTPUBLICINF)(szcmd+ulen);
            memcpy(lppublic->issuecode, g_input.inf.issuecode, 2);//发卡方代码
            memcpy(lppublic->citycode, g_input.inf.citycode, 2);//城市代码
            memcpy(lppublic->industrycode, g_input.inf.industrycode, 2);//城市代码
            lppublic->appstatus = g_input.inf.appstatus;               //启用标识
            lppublic->appversion = g_input.inf.appversion;              // 应用版本
            lppublic->testflag = g_input.inf.testflag;                // 测试标记
            memcpy(lppublic->logicnumber, g_input.logicnumber, 8); //逻辑卡号
            memcpy(lppublic->effectivefrom, g_input.inf.appeffectivefrom, 4);//应用启用日期
            memcpy(lppublic->effectiveto, g_input.inf.appeffectiveto, 4);//应用结束日期


            //    memcpy(lppublic->issuecode, "\x53\x20", 2);//发卡方代码
            //    memcpy(lppublic->citycode, "\x41\x00", 2);//城市代码
            //    lppublic->appstatus = 0x00;               //启用标识
            //    lppublic->appversion = 0x01;              // 应用版本
            //    lppublic->testflag = 0x01;                // 测试标记
            //    memcpy(lppublic->logicnumber, "\x90\x00\x00\x00\x10\x00\x00\x10", 8); //逻辑卡号
            //    memcpy(lppublic->effectivefrom, "\x20\x13\x06\x01", 4);//应用启用日期
            //    memcpy(lppublic->effectiveto, "\x20\x19\x12\x31", 4);//应用结束日期

            ulen +=sizeof(TPUBLICINF); //数据为全 00
            MAC_3(g_input.key.adf1_mamk_01, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //////dbg_dumpmemory("update adf1 file_0015 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //////dbg_dumpmemory("update adf1 file_0015 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 18;
                break;
            }

        }

        //0016   创建持卡人基本信息
        //二进制文件(01)，文件属性=0A
        //读自由，维护密钥02更新：文件属性=0A，读权限=00，写权限=00
        memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x16\x01\x00\x38\x0A\x00\x00\x00\x00\x00", 16);
        //////dbg_dumpmemory("create adf1 file_0016 =",szcmd,ulen);
        ulen =16;
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //////dbg_dumpmemory("create adf1 file_0016 |=",szcmd,respone_len);
        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 19;
            break;
        }


        //更新0016创建持卡人基本信息
        if (apdu_getrandom(szdata,4)==0)
        {

            memset(szcmd, 0x00, sizeof(szcmd));
            memcpy(szcmd, "\x04\xD6\x96\x00\x3C",5);
            ulen = 5;
            ulen += 56; //0x38  //数据为全 00
            MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //dbg_dumpmemory("update adf1 file_0016 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("update adf1 file_0016 |=",szcmd,respone_len);
            //////dbg_formatvar("update adf1 file_0016 |= %04X",respone_sw);

            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 20;
                break;
            }

        }

        //0002   创建钱包文件
        //钱包文件(02)
        memcpy(szcmd, "\x00\xE0\x00\x22\x0B\x00\x02\x22\x00\x43\x00\x60\x60\x60\x60\x60", 16);
        //////dbg_dumpmemory("create adf1 purse =",szcmd,ulen);
        ulen =16;
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //////dbg_dumpmemory("create adf1 purse |=",szcmd,respone_len);
        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 21;
            break;
        }


        //0018  创建交易明细文件
        //文件类型=交易明细文件(32)
        //应用类型=00（PBOC应用）
        memcpy(szcmd, "\x00\xE0\x00\x32\x0B\x00\x18\x32\x0A\x17\x00\x00\x00\x00\x00\x00", 16);
        //////dbg_dumpmemory("create adf1 consume record =",szcmd,ulen);
        ulen = 16;
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //////dbg_dumpmemory("create adf1 consume record |=",szcmd,respone_len);
        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 22;
            break;
        }

        //0017 全国复合消费文件
        //文件类型=TLV格式线性变长记录文件(05)，文件属性=0A
        //读自由，维护密钥02更新：文件属性=0A，读权限=00，写权限=00
        //文件长度=0200
//        memcpy(szcmd, "\x00\xE0\x00\x05\x0B\x00\x17\x05\x02\x00\x0A\x00\x00\x00\x00\x00", 16);
        memcpy(szcmd, "\x00\xE0\x00\x05\x0B\x00\x17\x05\x01\x50\x0A\x00\x00\x00\x00\x00", 16);
        //////dbg_dumpmemory("create adf1 complex file =",szcmd,ulen);
        ulen =16;
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //////dbg_dumpmemory("create adf1 complex file |=",szcmd,respone_len);
        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 23;
            break;
        }



        //增加记录01
        for (i=0;i<7;i++)
        {
            if (apdu_getrandom(szdata,4)==0)
            {
                memset(szcmd, 0x00, sizeof(szcmd));
                memcpy(szcmd, "\x04\xE2\x00\xB8\x34",5);
                szcmd[4] = complex_record[i][1]+4; //发送数据长度+  MAC 4 字节长度
                szcmd[5] = complex_record[i][0];   //记录标识
                szcmd[6] = complex_record[i][1]-2; //记录去掉2字节标头的长度
                if (complex_record[i][0]==0x02)
                {

                    //dbg_formatvar("current_station_id=%04X",Api::current_station_id);
                    //dbg_formatvar("current_device_id=%04X",Api::current_device_id);

                    szcmd[5+16] = ((Api::current_station_id >> 8) & 0x00FF);
                    szcmd[5+17] = (Api::current_station_id & 0xFF);
                    szcmd[5+18] = Api::current_device_type;
                    szcmd[5+18] <<= 4;
                    szcmd[5+18] &= 0xF0;
                    szcmd[5+18] |= ((Api::current_device_id >> 8) & 0x0F);
                    szcmd[5+19] |= (Api::current_device_id & 0xFF);


                    if (g_input.balance>0)
                    {
                        szcmd[9] =0x11; //轨道交通文件, 7bit-4bit:状态标志3bit-0bit:特殊标志 ( e/s初始化+ 正常)

                    }
                    else
                    {
                        szcmd[9] =0x09; //轨道交通文件, 7bit-4bit:状态标志3bit-0bit:特殊标志 ( e/s初始化+ 正常)
                    }
                    add_metro_area_crc(szcmd+5, LEN_METRO);

                }
                else if (complex_record[i][0]==0x11)
                {
                	if ((g_input.balance>0) &&(memcmp(g_input.inf.logiceffectivefrom,"\x00\x00\x00\x00\x00\x00\x00",7)!=0)&&(g_input.inf.saleflag==1))
                	{
						szcmd[7] = 1;
					}
                    memcpy(szcmd+9, g_input.inf.logiceffectivefrom, 7);
                    //dbg_dumpmemory("g_input.inf.logiceffectivefrom=",g_input.inf.logiceffectivefrom,7);
                }

                ulen = 5;
                ulen += complex_record[i][1]; //0x30  //数据为全 00
                MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);
                ulen += 4;
                //dbg_dumpmemory("update adf1 complex record 1 =",szcmd,ulen);
                respone_len = MifareProCom(ulen, szcmd, &respone_sw);
                //dbg_dumpmemory("update adf1 complex record 1 |=",szcmd,respone_len);
                if ((respone_len==0) || (0x9000!=respone_sw))
                {
                    nresult = 24;
                    break;
                }

            }
        }

        if (respone_sw!=0x9000)
        {
            nresult = 25;
            break;
        }

        //EF0D 创建FCI文件
        memcpy(szcmd, "\x00\xE0\x00\x01\x0B\xEF\x0D\x01\x00\x04\x00\x00\x00\x00\x00\x00", 16);
        //////dbg_dumpmemory("create adf1 FCI =",szcmd,ulen);
        ulen = 16;
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //////dbg_dumpmemory("create adf1 FCI |=",szcmd,respone_len);
        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 26;
            break;
        }


        //更新 FCI File
        //9F08(当前版本号标签) 01(长度) 02(当前版本号)
        memcpy(szcmd, "\x00\xD6\x8D\x00\x04\x9F\x08\x01\x02", 9);
        ulen = 9;
        //////dbg_dumpmemory("update adf1 FCI =",szcmd,ulen);
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //////dbg_dumpmemory("update adf1 FCI |=",szcmd,respone_len);
        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 27;
            break;
        }


        //==========================
        //如果是带值的初始化，则执行充值
        if (g_input.balance>0)
        {
            ulen = 8;
            memcpy(sztmp, "\x00\x20\x00\x00\x03\x12\x34\x56",8);
            ////dbg_dumpmemory("0020S=",sztmp,ulen);
            respone_len = MifareProCom(ulen,sztmp, &respone_sw);
            ////dbg_dumpmemory("0020R=",sztmp,respone_len);
            if ((respone_len==0 ) || (0x9000!=respone_sw))
            {
                nresult = 28;//ERR_VERIFYPIN;
                break;

            }

            value = g_input.balance;

            //获取充值信息
            ulen = 0;
            memcpy(sztmp, "\x80\x50\x00\x02\x0B",5);
            ulen += 5;
            sztmp[ulen] = 0x01;
            ulen += 1;
            sztmp[ulen] = ((value >> 24) & 0xFF);
            ulen += 1;
            sztmp[ulen] = ((value >> 16) & 0xFF);
            ulen += 1;
            sztmp[ulen] = ((value >> 8) & 0xFF);
            ulen += 1;
            sztmp[ulen] = (value & 0xFF);
            ulen += 1;
            memcpy(sztmp+ulen, "\x11\x22\x33\x44\x55\x66", 6);
            ulen += 6;


            ////dbg_dumpmemory("8050S=",sztmp,ulen);
            respone_len = MifareProCom(ulen,sztmp, &respone_sw);
            ////dbg_dumpmemory("8050R=",sztmp,respone_len);

            if ((respone_len==0 ) || (0x9000!=respone_sw))
            {
                nresult = 29;//ERR_RECHARGE_INIT;
                break;
            }
            memcpy((char *)&InitforLoad, sztmp, sizeof(InitforLoadStr_t));

            memset(moneybuf, 0, sizeof(moneybuf));
            memcpy(moneybuf, &InitforLoad.Random, 4);
            memcpy(moneybuf+4, &InitforLoad.OnlineDealSerial, 2);
            memcpy(moneybuf+6, "\x80\x00", 2);
            ////dbg_dumpmemory("adf1_mlk_0201=",g_input.key.adf1_mlk_0201, 16);
            TripleDes(calMacKey, moneybuf, g_input.key.adf1_mlk_0201, ENCRYPT);

            memset(sztmp, 0, sizeof(sztmp));
            sztmp[0] = (value & 0xFF000000) >> 24;
            sztmp[1] = (value & 0x00FF0000) >> 16;
            sztmp[2] = (value & 0x0000FF00) >> 8;
            sztmp[3] = (value & 0x000000FF);

            memcpy(sztmp+4, "\x02", 1);
            memcpy(sztmp+5, "\x11\x22\x33\x44\x55\x66", 6);

            memcpy(sztmp+11, m_datetime, 7); // bcd time


            MAC(calMacKey, 18, (const unsigned char *)("\x00\x00\x00\x00\x00\x00\x00\x00"), sztmp, sztmp);


            memcpy(calMacKey, sztmp, 4);

            memset(sztmp, 0, sizeof(sztmp));

            memcpy(sztmp, "\x80\x52\x00\x00\x0B", 5);
            //    memcpy(tmpbuf+5,"\x20\x11\x01\x20\x14\x20\x20", 7);
            memcpy(sztmp+5,  m_datetime, 7); // bcd time
            memcpy(sztmp+12,calMacKey, 4);
            ulen = 16;

            //dbg_dumpmemory("8052S=",sztmp,ulen);
            respone_len = MifareProCom(ulen,sztmp, &respone_sw);
            //dbg_dumpmemory("8052R=",sztmp,respone_len);
            if ((respone_len==0 ) || (0x9000!=respone_sw))
            {
                nresult = 30;//ERR_RECHARGE_WRITE;
                break;
            }

        }

        //=========================


        //////dbg_formatvar("init ticket");
        nresult = 0;
        Beep_Sleep(1);


    }
    while (0);


    return nresult;

}
#else

//储值票删除票卡原有结构
int svt_new_reset(void)
{
    unsigned char szdata[8];
    unsigned char szcmd[256];
    unsigned char respone_len;
    unsigned short sam_sw;
    unsigned char ulen;
    LPTMF_ISSUE lpmfissueinf;
    int nresult = 0;

    do
    {
//modify by shiyulong in 2014-05-26, 改变判断方式，改为读取发行文件判断

//	        nresult=svt_selectfile(0x3F00);
//	
//	        if (nresult<0)
//	        {
//	            nresult = 4;//ERR_CARD_READ;
//	        };

//	        if (nresult)
//	        {
//	            break;
//	        }

        // 读发行基本信息
        nresult = svt_readbinary(0x05, 0, 0x28, szcmd);
        if (nresult == -2)
        {
			//如果卡为白卡则认为洗卡成功
            nresult = 0;
            break;
        }
        else if ( nresult == -1)
        {
            nresult = 8;
            break;
        }
        
        lpmfissueinf = (LPTMF_ISSUE)szcmd;

		//比较逻辑卡号
        if (memcmp(lpmfissueinf->logicnumber,g_input.logicnumber,8)!=0)
        {
    		nresult = 7;
        	break;
        }
        
//modify by shiyulong in 2014-05-26, 完成


        if (apdu_getrandom(szdata,8)==0)
        {
            if (apdu_exteralauth(szdata,g_input.key.mf_mkey)==0)
            {
                nresult = 0;
                //dbg_formatvar("extern");
            }
            else
                nresult = 1;

        }

        if (nresult)
        {
            break;
        }


        if (apdu_getrandom(szdata,4)==0)
        {

            memset(szcmd, 0x00, sizeof(szcmd));
            memcpy(szcmd, "\x84\x0E\x00\x00\x04",5);
            ulen = 5;
//        memcpy(szcmd+ulen, "\xF0\x02\x91\xFF",4);
//        ulen += 4;
            memcpy(szcmd+ulen, g_input.key.mf_tkey, 16);
//        ulen += 16;
            MAC_3(g_input.key.mf_mkey, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //dbg_dumpmemory("delete mf =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //dbg_dumpmemory("delete mf |=",szcmd,respone_len);

            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 2;
            }

            if (0x9000==sam_sw)
            {
                nresult = 0;
            }
            //////dbg_formatvar((char *)"delete=%0d", nresult);


        }
    }
    while (0);


    //dbg_formatvar("clear=%d",nresult);
    return nresult;

}



//储值票创建主目录
int svt_new_create_mf(void)
{
    unsigned char ulen;
    unsigned char respone_len;
    unsigned char sztmp[256];
    unsigned char szcmd[256];
    unsigned char szdata[8];
    unsigned char tmp_len;
    unsigned short sam_sw;
    LPTMF_ISSUE lpmfissueinf;
    int nresult = 0;

    do
    {

        //认证传输密钥
        if (apdu_getrandom(szdata,8)==0)
        {
            if (apdu_exteralauth(szdata,g_input.key.mf_tkey))
            {
                nresult = 1;
                break;
            }
        }

        //创建MF(PBOC MF:1PAY.SYS.DDF01)(MF的文件名长度为0E，不能修改)
        //AID=3F00
        //MF空间=1A00
        //文件属性=5E:
        //DF有效性=01（有效）
        //钱包初始化使能位=0（不初始化）
        //删除DF使能=1（DF可删除）
        //DF下是否具有FCI文件=1（具有FCI文件）
        //创建DF下文件是否需要加MAC=1（需要）；
        //是否PBOC应用=1（强制为1）
        //钱包消费是否追加交易记录=0(不追加)；
        //创建权限=91(创建MF下的EF前需要进行外部认证)

//   ;【创建MF】
//   ;【AID = 3F00, MF空间 = 1CD0, 文件属性 = 5A( 0101 1010 )(见 COS手册22页说明)建立文件时不需要 MAC】
//   ;【封锁权限 = 00, 创建权限 = 91( 1001 0001 )（见 COS手册26页说明）】
//   ;【文件名长度 = 0E, 文件名 = 315041592E5359532E4444463031( MF:1PAY.SYS.DDF01 )】
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd, "\x04\xE0\x00\x38\x1E\x3F\x00\x38\x1C\xD0\x5A\x00\x00\x00\x00\x91\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", 31);
            ulen = 31;
            MAC_3(g_input.key.mf_tkey, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //dbg_dumpmemory("create mf =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //dbg_dumpmemory("create mf |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 2;
                break;
            }

        }


        memcpy(szcmd, "\x00\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", 19);
        ulen = 19;
        //dbg_dumpmemory("select mf =",szcmd,ulen);
        respone_len = MifareProCom(ulen, szcmd, &sam_sw);
        //dbg_dumpmemory("select mf |=",szcmd,respone_len);
        if ((respone_len==0) || (0x9000!=sam_sw))
        {
            nresult = 3;
            break;
        }


        //认证传输密钥
        if (apdu_getrandom(szdata,8)==0)
        {
            if (apdu_exteralauth(szdata,g_input.key.mf_tkey))
            {
                nresult = 4;
                break;
            }
        }

        //FID=0000 创建MF下的KEY文件
        //可创建18h条密钥，每条密钥记录长度14h
        //文件属性=C0（写至少需要MAC，写至少需要加密）

//   ;【创建Key文件】
//   ;【AID = FF02, 密钥数量 = 05, 密钥长度 = 14, 文件属性 = C0( 1100 0000 )(见 COS手册23页说明)密文 + MAC】
//   ;【当前密钥记录号 = 00, 配置字节 = 01(见 COS手册104页说明)】
//   ;【解锁PIN权限密钥权限 = 01, 建立/修改PIN权限 = 01（见 COS手册26页说明）】

        //if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd, "\x00\xE0\x00\x08\x0B\xFF\x02\x08\x05\x14\xC0\x00\x01\x00\x01\x01", 16);
            ulen = 16;
//            MAC_3(g_input.key.mf_tkey, ulen, szdata, szcmd, szcmd+ulen);
//            ulen += 4;
            //dbg_dumpmemory("create mfkey0000=",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //dbg_dumpmemory("create mfkey0000 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 5;
                break;
            }

        }

        //DCCK  卡片主控密钥
        //   ;【创建MF主控密钥】
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x01\x1C", 5);
            ulen = 5;
            memcpy(sztmp,"\x10\x01\x01\x33",4);
            memcpy(sztmp+4, g_input.key.mf_mkey, 16);
            tmp_len = data_encrypt(20, sztmp, g_input.key.mf_tkey);


            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.mf_tkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create  mf dcck =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //dbg_dumpmemory("create  mf dcck |=",szcmd,respone_len);

            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 6;
                break;
            }

        }



        //DCMK  卡片应用维护密钥
//   ;【创建MF应用维护密钥-类型30/文件更新密钥-类型70】
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x02\x1C", 5);
            ulen = 5;
            memcpy(sztmp,"\x70\x01\x00\x33",4);
            memcpy(sztmp+4, g_input.key.mf_mamk, 16);
            tmp_len = data_encrypt(20, sztmp, g_input.key.mf_mkey);


            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.mf_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create mf dcmk =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //dbg_dumpmemory("create mf dcmk |=",szcmd,respone_len);

            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 7;
                break;
            }
        }


        //    ;【创建MF外部认证密钥】
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x03\x1C", 5);
            ulen = 5;
            memcpy(sztmp,"\x10\x01\x02\x33",4);
            memcpy(sztmp+4, g_input.key.mf_mamk, 16);
            tmp_len = data_encrypt(20, sztmp, g_input.key.mf_mkey);


            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.mf_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create mf dcmk =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //dbg_dumpmemory("create mf dcmk |=",szcmd,respone_len);

            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 8;
                break;
            }
        }



        //FID=0001 创建目录信息文件 DCMK
//     ;【创建目录文件0001】
//   ;【文件标识 = 0001, 文件类型 = 05（线性变长记录文件(TLV)）, 文件长度 = 0100, 文件属性 = 08( 0000 1000 )(见 COS手册23页说明)更新要 MAC, 使用维护密钥】
//   ;【读权限 = 00, 写权限 = 00 (自由读写)】
//        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd, "\x00\xE0\x00\x05\x0B\x00\x01\x05\x01\x00\x08\x00\x00\x00\x00\x00", 16);
            ulen = 16;
//            MAC_3(g_input.key.mf_mkey, ulen, szdata, szcmd, szcmd+ulen);
//            ulen += 4;
            //dbg_dumpmemory("create FID0001=",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //dbg_dumpmemory("create FID0001 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 9;
                break;
            }

        }

        //更新目录信息0001
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd, "\x04\xDC\x01\x0C\x17", 5);
            ulen = 5;
            memcpy(szcmd+ulen, "\x70\x11\x61\x0F\x4F\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01\x50\x02\x3F\x01", 19);
            ulen += 19;
            MAC_3(g_input.key.mf_mamk, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //dbg_dumpmemory("update FID0000=",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //dbg_dumpmemory("update FID0000 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 10;
                break;
            }

        }





        //;FID=0005 创建发卡方基本信息文件  DCMK控制
//        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x05\x01\x00\x28\x08\x00\x00\x00\x00\x00", 16);
            ulen = 16;
//            MAC_3(g_input.key.mf_mkey, ulen, szdata, szcmd, szcmd+ulen);
//            ulen += 4;
            //dbg_dumpmemory("create FID0005=",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //dbg_dumpmemory("create FID0005 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 11;
                break;
            }

        }

        //更新发卡方信息文件0005
        if (apdu_getrandom(szdata,4)==0)
        {

            memset(szcmd, 0x00, sizeof(szcmd));

            memcpy(szcmd, "\x04\xD6\x85\x00\x2C", 5);
            ulen = 5;
            lpmfissueinf = (LPTMF_ISSUE)(szcmd+ulen);

            memcpy(lpmfissueinf->issuecode, g_input.inf.issuecode, 2);//发卡方代码
            memcpy(lpmfissueinf->citycode, g_input.inf.citycode, 2);//城市代码
            memcpy(lpmfissueinf->industrycode, g_input.inf.industrycode, 2);//城市代码
            lpmfissueinf->testflag = g_input.inf.testflag;      // 测试标记
            memcpy(lpmfissueinf->logicnumber, g_input.logicnumber, 8); //逻辑卡号
            memcpy(lpmfissueinf->cardtype,g_input.inf.cardtype, 2); //卡类型
            memcpy(lpmfissueinf->issuedate, g_input.inf.issuedate, 4);//发行日期
            memcpy(lpmfissueinf->issuedevice, g_input.inf.issuedevice, 6);//发行设备
            memcpy(lpmfissueinf->cardversion, g_input.inf.cardversion, 2);//卡版本


            memcpy(lpmfissueinf->effectivefrom, g_input.inf.effectivefrom, 4);//应用启用日期
            memcpy(lpmfissueinf->effectiveto, g_input.inf.effectiveto, 4);//应用结束日期


            ulen += sizeof(TMF_ISSUE);
            MAC_3(g_input.key.mf_mamk, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //dbg_dumpmemory("update FID0005=",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &sam_sw);
            //dbg_dumpmemory("update FID0005 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=sam_sw))
            {
                nresult = 12;
                break;
            }

        }


    }
    while (0);

    return nresult;

}




//储值票创建应用目录
int svt_new_create_adf1(void)
{

    unsigned char ulen;
    unsigned char respone_len;
    unsigned char sztmp[256];
    unsigned char szcmd[256];
    unsigned char szdata[9];
    unsigned char tmp_len;
    unsigned short respone_sw;
    //unsigned char keyidx=1;
    LPTPUBLICINF lppublic;

    unsigned long value;
    InitforLoadStr_t InitforLoad;

    unsigned char moneybuf[16];
    unsigned char calMacKey[8];
    unsigned char inf[100];
    unsigned char m_datetime[8];

    int i;

    //对应7 个复合记录, 数据分别为标识和长度
    unsigned char complex_record[7][2]=
    {
        {0x01,0x30},{0x02,0x30},{0x03,0x30},{0x04,0x30},{0x05,0x30},{0x09,0x30},{0x11,0x18}
    };

    int nresult = 0;


    do
    {

        memcpy(m_datetime, CmdSort::m_time_now, 7);
        //注册ADF1
//   ;【3F01】
//   ;【注册3F01】
//   ;【文件标识 = 3F01, 文件长度 = 0780, 文件属性 = 32( 0011 0010 )(见 COS手册22页说明)(在REGISTER DF中，文件属性字节除“DF有效性”控制位以外，其他位无实际意义) 】
//   ;【要注册的DF文件名长度 = 09 传输密钥 = F0 00 01 FF FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF(前面 4个字节是密钥类型F0, 密钥版本00, 密钥号01, 解锁次数FF)】

//00 EE 0138 20 3F01 38 0780 32 00000000 11 09 F0 00 01 FF FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        memcpy(sztmp, "\x00\xEE\x01\x38\x20",5);
        ulen = 5;
        memcpy(sztmp+ulen, "\x3F\x01\x38\x07\x80\x32\x00\x00\x00\x00\x11\x09\xF0\x00\x01\xFF", 16);
        ulen += 16;
//  memcpy(sztmp+ulen, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 16);
        memcpy(sztmp+ulen, g_input.key.adf1_tkey, 16);
        ulen += 16;
        //dbg_dumpmemory("register adf1 =",sztmp,ulen);
        respone_len = MifareProCom(ulen, sztmp, &respone_sw);
        //dbg_dumpmemory("register adf1 |=",sztmp,respone_len);
        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 1;
            //break;
        }

        //创建ADF1
//   ;【创建3F01】
//   ;【文件标识 = 3F01, 文件长度 = 0780, 文件属性 = 7B( 0111 1011 )(见 COS手册23页说明)】
//   ;【解锁PIN权限密钥权限 = 01, 建立/修改PIN权限 = 01（见 COS手册26页说明）】
//   ;【DF文件名长度 = 09 DF文件名 = A00000000386980701】
//  if (apdu_getrandom(szdata,8)==0)
        {
            //00 E0 00 38 15 3F01 38 0780 7B 00 00 00 00 91 09 A00000000386980701

            //////dbg_dumpmemory("random =",szdata,8);
            memcpy(szcmd, "\x00\xE0\x00\x38\x15\x3F\x01\x38\x07\x80\x7B\x00\x00\x00\x00\x91\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01", 26);
            //memcpy(szcmd, "\x04\xE0\x00\x38\x19\x10\x01\x38\x05\xC0\x7B\x00\x00\x00\x00\x11\x09\xF0\x00\x00\x41\x00\x53\x20\xAD\xF1", 26);
            ulen = 26;

            //MAC_3(g_input.key.adf1_tkey, ulen, szdata, szcmd, szcmd+ulen);

            //ulen += 4;
            //dbg_dumpmemory("create adf1 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("create adf1 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 2;
                break;
            }

        }


        //认证ADF1的传输密钥
        if (apdu_getrandom(szdata,8)==0)
        {
            if (apdu_exteralauth(szdata,g_input.key.adf1_tkey))
            {
                nresult = 3;
                break;
            }
        }



        //创建密钥文件0000
        memcpy(szcmd, "\x00\xE0\x00\x08\x0B\x00\x00\x08\x14\x14\xC0\x00\x01\x00\x01\x01", 16);
        ulen = 16;
        //dbg_dumpmemory("create key0000 =",szcmd,ulen);
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //dbg_dumpmemory("create key0000 |=",szcmd,respone_len);

        //创建adf1 主控密钥10 (外部认证密钥)
//   ;【注意主控密钥必须先用传输安装MF的主控，再用MF的主控安装DF的主控】
//   ;【创建DF01的主控密钥1】;Create Main Control Key ADF1
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x01\x1C", 5);
            //szcmd[3]=keyidx++;
            ulen = 5;
            memcpy(sztmp,"\x10\x01\x01\x33",4);
            memcpy(sztmp+4, g_input.key.mf_mkey, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_tkey);

            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_tkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create adf1 key_mkey =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("create adf1 key_mkey |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 4;
                break;
            }

        }

//   ;【创建DF01的主控密钥】;Create Main Control Key ADF1
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x01\x1C", 5);
            //szcmd[3]=keyidx++;
            ulen = 5;
            memcpy(sztmp,"\x10\x01\x01\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mkey, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.mf_mkey);

            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.mf_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create adf1 key_mkey2 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("create adf1 key_mkey2 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 5;
                break;
            }

        }


//  ;【创建DF01的应用维护密钥-类型30/文件更新密钥-类型70】
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x02\x1C", 5);
            //szcmd[3]=keyidx++;
            ulen = 5;
            memcpy(sztmp,"\x70\x01\x00\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mamk_00, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);

            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create adf1 mamk_00 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("create adf1 mamk_00 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 6;
                break;
            }

        }


//   ;【创建 DF01 消费密钥01】;80 F0 0003 14 400101 33 s_DPK01_DF01
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x03\x1C", 5);
            //szcmd[3]=keyidx++;
            ulen = 5;
            memcpy(sztmp,"\x40\x01\x01\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mpk_0101, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);

            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create adf1 mpk_0101 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("create adf1 mpk_0101 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 7;
                break;
            }
        }


        // ;【创建DF01的圈存密钥】;80 F0 0004 14 E00201 33 s_DLK01_DF01
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x04\x1C", 5);
            //szcmd[3]=keyidx++;
            ulen = 5;
            memcpy(sztmp,"\xE0\x02\x01\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mlk_0201, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);

            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create adf1 mlk_0201 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("create adf1 mlk_0201 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 8;
                break;
            }
        }

        // ;【创建DF01的TAC密钥】;80 F0 0005 14 600101 33 s_DTK01_DF01
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x05\x1C", 5);
            //szcmd[3]=keyidx++;
            ulen = 5;
            memcpy(sztmp,"\x60\x01\x01\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mtk, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);

            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create adf1 key_mtk =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("create adf1 key_mtk |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 9;
                break;
            }
        }

// ;【创建DF01 Pin】;80 F0 0006 0B 08 0000 33 06 s_PIN_DF01
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x06\x14", 5);
            //szcmd[3]=keyidx++;
            ulen = 5;
            memcpy(sztmp,"\x08\x00\x00\x88\x03",5);
            //memcpy(sztmp+5, g_input.key.adf1_pin, 3);
            memcpy(sztmp+5, "\x12\x34\x56", 3);
            tmp_len = data_encrypt(8,sztmp,g_input.key.adf1_mkey);

            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create adf1 key_pin =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("create adf1 key_pin |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 10;
                break;
            }
        }

//;【创建DF01的维护密钥01】;80 F0 0007 14 700101 33 s_DAMK01_DF01
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x07\x1C", 5);
            //szcmd[3]=keyidx++;
            ulen = 5;
            memcpy(sztmp,"\x70\x01\x01\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mamk_01, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);

            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create adf1 key_mamk_01 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("create adf1 key_mamk_01 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 11;
                break;
            }
        }

//;【创建DF01的维护密钥02】;80 F0 0007 14 700101 33 s_DAMK01_DF01
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x08\x1C", 5);
            //szcmd[3]=keyidx++;
            ulen = 5;
            memcpy(sztmp,"\x70\x01\x02\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mamk_02, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);

            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create adf1 key_mamk_02 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("create adf1 key_mamk_02 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 12;
                break;
            }
        }


//;【创建DF01的应用解锁密钥】;80 F0 0009 14 200102 33 s_APPUnBlock_DF01
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x09\x1C", 5);
            //szcmd[3]=keyidx++;
            ulen = 5;
            memcpy(sztmp,"\x50\x01\x01\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mauk, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);

            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create adf1 key_mauk =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("create adf1 key_mauk |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 13;
                break;
            }
        }

//;【创建DF01的应用解锁密钥】;80 F0 0009 14 200102 33 s_APPUnBlock_DF01
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x0A\x1C", 5);
            //szcmd[3]=keyidx++;
            ulen = 5;
            memcpy(sztmp,"\x90\x01\x01\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mabk, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);

            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create adf1 key_mabk =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("create adf1 key_mabk |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 14;
                break;
            }
        }

//;【创建DF01的Pin解锁密钥】;80 F0 000B 14 200102 33 s_DPUK_DF01
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x0B\x1C", 5);
            //szcmd[3]=keyidx++;
            ulen = 5;
            memcpy(sztmp,"\x20\x01\x02\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mpuk, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);

            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create adf1 key_puk =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("create adf1 key_puk |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 15;
                break;
            }
        }

//;【创建DF01的Reload Pin密钥】;80 F0 000C 14 200103 33 s_DRPK_DF01
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x0C\x1C", 5);
            //szcmd[3]=keyidx++;
            ulen = 5;
            memcpy(sztmp,"\x20\x01\x03\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mprk, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);

            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create adf1 key_mmprk =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("create adf1 key_mprk |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 16;
                break;
            }
        }


//;【创建DF01的修改透支限额密钥】;80 F0 000D 14 A00200 33 s_DUK_DF01
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x0D\x1C", 5);
            //szcmd[3]=keyidx++;
            ulen = 5;
            memcpy(sztmp,"\x20\x01\x03\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_muk, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);

            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create adf1 key_muk =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("create adf1 key_muk |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 17;
                break;
            }
        }

//;【创建DF01的圈提密钥】;80 F0 000E 14 C00200 33 s_DULK_DF01
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x0E\x1C", 5);
            //szcmd[3]=keyidx++;
            ulen = 5;
            memcpy(sztmp,"\xC0\x02\x00\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_mulk, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);

            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create adf1 key_mulk =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("create adf1 key_mulk |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 18;
                break;
            }
        }

//;【创建外部认证01】;80 F0 000F 14 100102 33 s_EAK_DF01
        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(szcmd,"\x84\xF0\x00\x0F\x1C", 5);
            //szcmd[3]=keyidx++;
            ulen = 5;
            memcpy(sztmp,"\x10\x01\x02\x33",4);
            memcpy(sztmp+4, g_input.key.adf1_eak, 16);
            tmp_len = data_encrypt(20,sztmp,g_input.key.adf1_mkey);

            memcpy(szcmd+ulen, sztmp, tmp_len);
            ulen += tmp_len;

            MAC_3(g_input.key.adf1_mkey, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("create adf1 key_eak =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("create adf1 key_eak |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 19;
                break;
            }
        }

//
//   ;【专用FCI文件】
//   ;【文件标识 = EF0D, 文件类型 = 01, 文件长度 = 0004, 文件属性 = 08( 0000 1000 )(见 COS手册23页说明)更新要 MAC, 使用维护密钥】
//   ;【读权限 = 00, 写权限 = 00 (自由读写)】
        memcpy(szcmd, "\x00\xE0\x00\x01\x0B\xEF\x0D\x01\x00\x04\x08\x00\x00\x00\x00\x00", 16);
        ulen = 16;
        //dbg_dumpmemory("create adf1 file_EF0D =",szcmd,ulen);
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //dbg_dumpmemory("create adf1 file_EF0D |=",szcmd,respone_len);

        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 20;
            break;
        }

        if (apdu_getrandom(szdata,4)==0)
        {
            memset(szcmd, 0x00, sizeof(szcmd));
            memcpy(szcmd, "\x04\xD6\x8D\x00\x08\x9F\x08\x01\x02",9);
            ulen = 5+4;
            MAC_3(g_input.key.adf1_mamk_00, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            ////dbg_dumpmemory("update adf1 file_EF0D =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            ////dbg_dumpmemory("update adf1 file_EF0D |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 21;
                break;
            }

        }

// ;【公共应用基本信息文件】
// ;【文件标识 = 0015, 文件类型 = 01, 文件长度 = 001E, 文件属性 = 09( 0000 1001 )(见 COS手册23页说明)更新要 MAC, 使用维护密钥】
// ;【读权限 = 00, 写权限 = 00 (自由读写)】

        memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x15\x01\x00\x1E\x09\x00\x00\x00\x00\x00", 16);

        ulen = 16;
        //dbg_dumpmemory("create adf1 file_0015 =",szcmd,ulen);
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //dbg_dumpmemory("create adf1 file_0015 |=",szcmd,respone_len);
        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 22;
            break;
        }

        //更新0015公共应用基本文件
        //注意更新逻辑卡号 sDData = 9000000010000010
        if (apdu_getrandom(szdata,4)==0)
        {
            memset(szcmd, 0x00, sizeof(szcmd));
            memcpy(szcmd, "\x04\xD6\x95\x00\x22",5);
            ulen = 5;

            lppublic = (LPTPUBLICINF)(szcmd+ulen);
            memcpy(lppublic->issuecode, g_input.inf.issuecode, 2);//发卡方代码
            memcpy(lppublic->citycode, g_input.inf.citycode, 2);//城市代码
            memcpy(lppublic->industrycode, g_input.inf.industrycode, 2);//城市代码
            lppublic->appstatus = g_input.inf.appstatus;      //启用标识
            lppublic->appversion = g_input.inf.appversion;    // 应用版本
            lppublic->testflag = g_input.inf.testflag;      // 测试标记
            memcpy(lppublic->logicnumber, g_input.logicnumber, 8); //逻辑卡号
            memcpy(lppublic->effectivefrom, g_input.inf.appeffectivefrom, 4);//应用启用日期
            memcpy(lppublic->effectiveto, g_input.inf.appeffectiveto, 4);//应用结束日期

            ulen +=sizeof(TPUBLICINF); //数据为全 00

            MAC_3(g_input.key.adf1_mamk_01, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("update adf1 file_0015 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("update adf1 file_0015 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 23;
                break;
            }

        }

// ;【持卡人个性信息文件】
// ;【文件标识 = 0016, 文件类型 = 01, 文件长度 = 005C, 文件属性 = 0A( 0000 1010 )(见 COS手册23页说明)更新要 MAC, 使用维护密钥02】
// ;【读权限 = 00, 写权限 = 00 (自由读写)】
//  00 E0 0001 0B 0016 01 005C 0A 000000 0000

        memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x16\x01\x00\x5C\x0A\x00\x00\x00\x00\x00", 16);
        ulen = 16;
        //dbg_dumpmemory("create adf1 file_0016 =",szcmd,ulen);
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //dbg_dumpmemory("create adf1 file_0016 |=",szcmd,respone_len);
        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 24;
            break;
        }

        //更新0016创建持卡人基本信息
        if (apdu_getrandom(szdata,4)==0)
        {

            memset(szcmd, 0x00, sizeof(szcmd));
            memcpy(szcmd, "\x04\xD6\x96\x00\x60",5);
            ulen = 5;
            ulen += 92; //0x38 //数据为全 00
            MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            ////dbg_dumpmemory("update adf1 file_0016 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            ////dbg_dumpmemory("update adf1 file_0016 |=",szcmd,respone_len);
            ////dbg_formatvar("update adf1 file_0016 |= %04X",respone_sw);

            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 25;
                break;
            }

        }


        //0002   创建钱包文件
        //钱包文件(02)
        //00 E0 0022 0B 0002 22 0043 00 6060606060
        memcpy(szcmd, "\x00\xE0\x00\x22\x0B\x00\x02\x22\x00\x43\x00\x60\x60\x60\x60\x60", 16);
        ulen = 16;
        //dbg_dumpmemory("create adf1 purse =",szcmd,ulen);
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //dbg_dumpmemory("create adf1 purse |=",szcmd,respone_len);
        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 26;
            break;
        }

//   ;【文件属性 = 01( 0000 0001 )为控制交易时不认证住建部的 mac，允许电子存折进行复合消费(见 COS手册105页说明)】
//   ;【文件类型 = 32, 文件长度 = 0A * 17】
//   ;【电子钱包本地交易明细记录文件】
// 00 E0 0032 0B 0018 32 0A17 01 000000 0000 ;judge:sw=9000
        memcpy(szcmd, "\x00\xE0\x00\x32\x0B\x00\x18\x32\x0A\x17\x01\x00\x00\x00\x00\x00", 16);
        ulen = 16;
        //dbg_dumpmemory("create adf1 purse detail =",szcmd,ulen);
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //dbg_dumpmemory("create adf1 detail |=",szcmd,respone_len);
        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 27;
            break;
        }

//   ;【电子钱包异地交易明细记录文件】
// 00 E0 0032 0B 0010 32 0A17 01 000000 0000 ;judge:sw=9000
        memcpy(szcmd, "\x00\xE0\x00\x32\x0B\x00\x10\x32\x0A\x17\x01\x00\x00\x00\x00\x00", 16);
        ulen = 16;
        //dbg_dumpmemory("create adf1 detail 2 =",szcmd,ulen);
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //dbg_dumpmemory("create adf1 detail 2 |=",szcmd,respone_len);
        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 28;
            break;
        }

//   ;【电子钱包充值交易明细记录文件】
// 00 E0 0032 0B 001A 32 0A17 01 000000 0000 ;judge:sw=9000
        memcpy(szcmd, "\x00\xE0\x00\x32\x0B\x00\x1A\x32\x0A\x17\x01\x00\x00\x00\x00\x00", 16);
        ulen = 16;
        //dbg_dumpmemory("create adf1 detail 3 =",szcmd,ulen);
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //dbg_dumpmemory("create adf1 detail 3 |=",szcmd,respone_len);
        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 29;
            break;
        }


//  ;【复合交易记录文件】
//  ;【文件标识 = 0017, 文件类型 = 05, 文件长度 = 0150, 文件属性 = 0A( 0000 1010 )(见 COS手册23页说明)更新要 MAC, 使用维护密钥02】
//;00 E0 0005 0B 0017 05 00A0 0A 000000 0000 ;judge:sw=9000
//00 E0 0005 0B 0017 05 0150 0A 000000 0000 ;judge:sw=9000
        memcpy(szcmd, "\x00\xE0\x00\x05\x0B\x00\x17\x05\x01\x50\x0A\x00\x00\x00\x00\x00", 16);
        ulen = 16;
        //dbg_dumpmemory("create adf1 complex file =",szcmd,ulen);
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //dbg_dumpmemory("create adf1 complex file |=",szcmd,respone_len);
        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 30;
            break;
        }

        //增加记录01
        for (i=0;i<7;i++)
        {
            if (apdu_getrandom(szdata,4)==0)
            {
                memset(szcmd, 0x00, sizeof(szcmd));
//                 memcpy(szcmd, "\x04\xE2\x00\xB8\x34",5);
//                 szcmd[4] = complex_record[i][1]+4; //发送数据长度+  MAC 4 字节长度
//                 szcmd[5] = complex_record[i][0];   //记录标识
//                 szcmd[6] = complex_record[i][1]-2; //记录去掉2字节标头的长度
                memcpy(szcmd, "\x04\xDC\x01\xBC\x34",5);
                szcmd[2] = i+1;
                szcmd[4] = complex_record[i][1]+4; //发送数据长度+ MAC 4 字节长度
                szcmd[5] = complex_record[i][0];   //记录标识
                szcmd[6] = complex_record[i][1]-2; //记录去掉2字节标头的长度

                if (complex_record[i][0]==0x02)
                {
                    szcmd[5+16] = ((Api::current_station_id >> 8) & 0x00FF);
                    szcmd[5+17] = (Api::current_station_id & 0xFF);
                    szcmd[5+18] = Api::current_device_type;
                    szcmd[5+18] <<= 4;
                    szcmd[5+18] &= 0xF0;
                    szcmd[5+18] |= ((Api::current_device_id >> 8) & 0x0F);
                    szcmd[5+19] |= (Api::current_device_id & 0xFF);


                    if (g_input.balance>0)
                    {
                        szcmd[9] =0x11; //轨道交通文件, 7bit-4bit:状态标志3bit-0bit:特殊标志 ( e/s初始化+ 正常)
                    }
                    else
                    {
                        szcmd[9] =0x09; //轨道交通文件, 7bit-4bit:状态标志3bit-0bit:特殊标志 ( e/s初始化+ 正常)
                    }
                    add_metro_area_crc(szcmd+5, LEN_METRO);

                }
                else if (complex_record[i][0]==0x11)
                {
                	if (/*(g_input.balance>0) &&*/(memcmp(g_input.inf.logiceffectivefrom,"\x00\x00\x00\x00\x00\x00\x00",7)!=0)&&(g_input.inf.saleflag==1))
                	{
						szcmd[7] = 1;
					}
                    memcpy(szcmd+9, g_input.inf.logiceffectivefrom, 7);
                }

                ulen = 5;
                ulen += complex_record[i][1]; //0x30  //数据为全 00
                MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);
                ulen += 4;
                ////dbg_dumpmemory("update adf1 complex record 1 =",szcmd,ulen);
                respone_len = MifareProCom(ulen, szcmd, &respone_sw);
                ////dbg_dumpmemory("update adf1 complex record 1 |=",szcmd,respone_len);
                if ((respone_len==0) || (0x9000!=respone_sw))
                {
                    nresult = 31;
                    break;
                }

//==================================================================
//                 memset(szcmd, 0x00, sizeof(szcmd));
//                 memcpy(szcmd, "\x04\xDC\x01\xBC\x34",5);
//                 szcmd[2] = i+1;
//                 szcmd[4] = complex_record[i][1]+4; //发送数据长度+ MAC 4 字节长度
//                 szcmd[5] = complex_record[i][0];   //记录标识
//                 szcmd[6] = complex_record[i][1]-2; //记录去掉2字节标头的长度
//
//                 szcmd[5+16] = ((Api::current_station_id >> 8) & 0x00FF);
//                 szcmd[5+17] = (Api::current_station_id & 0xFF);
//                 szcmd[5+18] = Api::current_device_type;
//                 szcmd[5+18] <<= 4;
//                 szcmd[5+18] &= 0xF0;
//                 szcmd[5+18] |= ((Api::current_device_id >> 12) & 0x0F);
//                 szcmd[5+19] |= (Api::current_device_id & 0xFF);
//
//                 if (complex_record[i][0]==0x02)
//                 {
//
//                     if (g_input.balance>0)
//                     {
//                         szcmd[9] =0x11; //轨道交通文件, 7bit-4bit:状态标志3bit-0bit:特殊标志 ( e/s初始化+ 正常)
//
//                     }
//                     else
//                     {
//                         szcmd[9] =0x09; //轨道交通文件, 7bit-4bit:状态标志3bit-0bit:特殊标志 ( e/s初始化+ 正常)
//                     }
//                 }
//
//                 ulen = 5;
//                 ulen += complex_record[i][1]; //0x30  //数据为全 00
//                 MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);
//                 ulen += 4;
//                 ////dbg_dumpmemory("update adf1 complex record 1 =",szcmd,ulen);
//                 respone_len = MifareProCom(ulen, szcmd, &respone_sw);
//                 ////dbg_dumpmemory("update adf1 complex record 1 |=",szcmd,respone_len);
//                 if ((respone_len==0) || (0x9000!=respone_sw))
//                 {
//                     nresult = -21;
//                     break;
//                 }

            }
        }


//   ;【保留文件11】
//   ;【文件标识 = 0011, 文件类型 = 01, 文件长度 = 0020, 文件属性 = 09( 0000 1001 )(见 COS手册23页说明)更新要 MAC, 使用维护密钥01】
//   ;【读权限 = 00, 写权限 = 00 (自由读写)】
//        memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x11\x01\x00\x20\x09\x00\x00\x00\x00\x00", 16);
        memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x11\x01\x00\x20\x0A\x00\x00\x00\x00\x00", 16);
        ulen = 16;
        //dbg_dumpmemory("create adf1 file_0011 =",szcmd,ulen);
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //dbg_dumpmemory("create adf1 file_0011 |=",szcmd,respone_len);

        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 32;
            break;
        }

        if (apdu_getrandom(szdata,4)==0)
        {

            memset(inf, 0x00, sizeof(inf));

            ulen = 0;
            inf[ulen++] = g_input.inf.saleflag;
            memcpy(inf+ulen,g_input.inf.szlogicectivetime, 4);
            ulen += 4;
            inf[ulen++]=g_input.inf.deposit;
            //====
            ulen += 6; //发售设备信息
            //====
            memcpy(inf+ulen, g_input.inf.szcaps, 2); //充值上限
            ulen += 2;

            inf[ulen++]=g_input.inf.passmode;     //通讯模式
            inf[ulen++]=g_input.inf.entryline;    //可入线路
            inf[ulen++]=g_input.inf.entrystation; //可入站点
            inf[ulen++]=g_input.inf.exitline;     //可出线路
            inf[ulen++]=g_input.inf.exitstation;  //可出站点


            memset(szcmd, 0x00, sizeof(szcmd));
            ulen = 0;
            memcpy(szcmd, "\x04\xD6\x91\x00\x24",5);
            ulen += 5;
            memcpy(szcmd+ulen, inf, LEN_APP_CTRL);
            ulen += LEN_APP_CTRL;

            MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //dbg_dumpmemory("update adf1 file_0011 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("update adf1 file_0011 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 33;
                break;
            }

//    ulen = 0;
//    inf[ulen++] = g_input.inf.saleflag;
//    memcpy(inf+ulen,g_input.inf.szlogicectivetime, 4);
//    ulen += 4;
//    inf[ulen++]=g_input.inf.deposit;
//    //====
//    ulen += 6; //发售设备信息
//    //====
//    memcpy(inf+ulen, g_input.inf.szcaps, 2); //充值上限
//    ulen += 2;
//
//    inf[ulen++]=g_input.inf.passmode;   //通讯模式
//    inf[ulen++]=g_input.inf.entryline;   //可入线路
//    inf[ulen++]=g_input.inf.entrystation; //可入站点
//    inf[ulen++]=g_input.inf.exitline;   //可出线路
//    inf[ulen++]=g_input.inf.exitstation;  //可出站点
//
//    memset(szcmd, 0x00, sizeof(szcmd));
//    ulen = 0;
//    memcpy(szcmd, "\x04\xD6\x91\x00\x24",5);
//    ulen += 5;
//    memcpy(szcmd+ulen, inf, LEN_APP_CTRL);
//    ulen += LEN_APP_CTRL;
//
//    MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);
//    ulen += 4;
//             ////dbg_dumpmemory("update adf1 file_0011 =",szcmd,ulen);
//             respone_len = MifareProCom(ulen, szcmd, &respone_sw);
//             ////dbg_dumpmemory("update adf1 file_0011 |=",szcmd,respone_len);

        }

// ;【保留文件12】
// ;【文件标识 = 0012, 文件类型 = 01, 文件长度 = 0020, 文件属性 = 0A( 0000 1010 )(见 COS手册23页说明)更新要 MAC, 使用维护密钥02】
// ;【读权限 = 00, 写权限 = 00 (自由读写)】

        memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x12\x01\x00\x20\x0A\x00\x00\x00\x00\x00", 16);
        ulen = 16;
        //dbg_dumpmemory("create adf1 file_0012 =",szcmd,ulen);
        respone_len = MifareProCom(ulen, szcmd, &respone_sw);
        //dbg_dumpmemory("create adf1 file_0012 |=",szcmd,respone_len);

        if ((respone_len==0) || (0x9000!=respone_sw))
        {
            nresult = 34;
            break;
        }

        if (apdu_getrandom(szdata,4)==0)
        {
            memset(szcmd, 0x00, sizeof(szcmd));
            memcpy(szcmd, "\x04\xD6\x92\x00\x24",5);
            ulen = 5;
            ulen +=32; //数据为全 00
            MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //dbg_dumpmemory("update adf1 file_0012 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("update adf1 file_0012 |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 35;
                break;
            }

        }


//====================
//==========================
//如果是带值的初始化，则执行充值
        if (g_input.balance>0)
        {
            ulen = 8;
            memcpy(sztmp, "\x00\x20\x00\x00\x03\x12\x34\x56",8);
            ////dbg_dumpmemory("0020S=",sztmp,ulen);
            respone_len = MifareProCom(ulen,sztmp, &respone_sw);
            ////dbg_dumpmemory("0020R=",sztmp,respone_len);
            if ((respone_len==0 ) || (0x9000!=respone_sw))
            {
                nresult = 36;//ERR_VERIFYPIN;
                break;

            }

            value = g_input.balance;

            //获取充值信息
            ulen = 0;
            memcpy(sztmp, "\x80\x50\x00\x02\x0B",5);
            ulen += 5;
            sztmp[ulen] = 0x01;
            ulen += 1;
            sztmp[ulen] = ((value >> 24) & 0xFF);
            ulen += 1;
            sztmp[ulen] = ((value >> 16) & 0xFF);
            ulen += 1;
            sztmp[ulen] = ((value >> 8) & 0xFF);
            ulen += 1;
            sztmp[ulen] = (value & 0xFF);
            ulen += 1;
            memcpy(sztmp+ulen, "\x11\x22\x33\x44\x55\x66", 6);
            ulen += 6;


            //dbg_dumpmemory("8050S=",sztmp,ulen);
            respone_len = MifareProCom(ulen,sztmp, &respone_sw);
            //dbg_dumpmemory("8050R=",sztmp,respone_len);

            if ((respone_len==0 ) || (0x9000!=respone_sw))
            {
                nresult = 37;//ERR_RECHARGE_INIT;
                break;
            }
            memcpy((char *)&InitforLoad, sztmp, sizeof(InitforLoadStr_t));

            memset(moneybuf, 0, sizeof(moneybuf));
            memcpy(moneybuf, &InitforLoad.Random, 4);
            memcpy(moneybuf+4, &InitforLoad.OnlineDealSerial, 2);
            memcpy(moneybuf+6, "\x80\x00", 2);
            ////dbg_dumpmemory("adf1_mlk_0201=",g_input.key.adf1_mlk_0201, 16);
            TripleDes(calMacKey, moneybuf, g_input.key.adf1_mlk_0201, ENCRYPT);

            memset(sztmp, 0, sizeof(sztmp));
            sztmp[0] = (value & 0xFF000000) >> 24;
            sztmp[1] = (value & 0x00FF0000) >> 16;
            sztmp[2] = (value & 0x0000FF00) >> 8;
            sztmp[3] = (value & 0x000000FF);

            memcpy(sztmp+4, "\x02", 1);
            memcpy(sztmp+5, "\x11\x22\x33\x44\x55\x66", 6);

            memcpy(sztmp+11, m_datetime, 7); // bcd time


            MAC(calMacKey, 18, (const unsigned char *)("\x00\x00\x00\x00\x00\x00\x00\x00"), sztmp, sztmp);


            memcpy(calMacKey, sztmp, 4);

            memset(sztmp, 0, sizeof(sztmp));

            memcpy(sztmp, "\x80\x52\x00\x00\x0B", 5);
            //    memcpy(tmpbuf+5,"\x20\x11\x01\x20\x14\x20\x20", 7);
            memcpy(sztmp+5,  m_datetime, 7); // bcd time
            memcpy(sztmp+12,calMacKey, 4);
            ulen = 16;

            //dbg_dumpmemory("8052S=",sztmp,ulen);
            respone_len = MifareProCom(ulen,sztmp, &respone_sw);
            //dbg_dumpmemory("8052R=",sztmp,respone_len);
            if ((respone_len==0 ) || (0x9000!=respone_sw))
            {
                nresult = 38;//ERR_RECHARGE_WRITE;
                break;
            }

        }

//=========================

//=========================

        //////dbg_formatvar("init ticket");
        nresult = 0;
        Beep_Sleep(1);


    }
    while (0);


    return nresult;

}






#endif


//票卡状态解析，将票卡内的状态对应为编一的外部状态。
uint8_t unified_status(uint8_t ticket_status)
{
#define USTATUS_INIT    (uint8_t)200    // 初始化
#define USTATUS_ES     (uint8_t)201    // Init II (Pre–value loaded @E/S)(ES预赋值)
#define USTATUS_SALE    (uint8_t)202    // BOM/TVM发售
#define USTATUS_EXIT    (uint8_t)203    // 出站(exit)
#define USTATUS_EXIT_T    (uint8_t)204    // 列车故障模式出站(exit during Train–disruption)
#define USTATUS_UPD_OUT    (uint8_t)205    // 进站BOM更新(upgrade at BOM for Entry)
#define USTATUS_UPD_OUT_FREE  (uint8_t)206    // 非付费区免费更新（BOM/pca 非付费区）
#define USTATUS_UPD_OUT_FARE  (uint8_t)207    // 非付费区付费更新（BOM/pca 非付费区）
#define USTATUS_ENTRY    (uint8_t)208    // 进站(entry)
#define USTATUS_UPD_IN    (uint8_t)209    // 出站BOM更新(upgrade at BOM for Exit)
#define USTATUS_UPD_WO_STA   (uint8_t)210    // 无进站码更新（BOM/pca 付费区）
#define USTATUS_UPD_TM_OUT   (uint8_t)211    // 超时更新（BOM/pca 付费区）
#define USTATUS_UPD_TP_OUT   (uint8_t)212    // 超乘更新（BOM/pca 付费区）
#define USTATUS_EXIT_ONLY   (uint8_t)213    // ET for Exit(出站票)
#define USTATUS_REFUND    (uint8_t)214    // 退卡
#define USTATUS_DETROY    (uint8_t)215    // 车票注销

    uint8_t ret = 0xFF;
    switch (ticket_status)
    {
        case 1://MS_Init:
            ret = USTATUS_INIT;
            break;
        case 2://MS_Es:
            ret = USTATUS_ES;
            break;
        case 3://MS_Sale:
            ret = USTATUS_SALE;
            break;
        case 4://MS_Entry:
            ret = USTATUS_ENTRY;
            break;
        case 5://MS_Exit:
            ret = USTATUS_EXIT;
            break;
        case 6://MS_Exit_Only:
            ret = USTATUS_EXIT_ONLY;
            break;
        case 7://MS_Exit_Tt:
            ret = USTATUS_EXIT_T;
            break;
        case 8://MS_Upd_FZone_Free:
            ret = USTATUS_UPD_OUT_FREE;
            break;
        case 9://MS_Upd_FZone_Fare:
            ret = USTATUS_UPD_OUT_FARE;
            break;
        case 10://MS_Upd_Wzone_Entry:
            ret = USTATUS_UPD_WO_STA;
            break;
        case 11://MS_Upd_Wzone_Exit:
            ret = USTATUS_UPD_IN;
            break;
        case 12://MS_Upd_Wzone_Time:
            ret = USTATUS_UPD_TM_OUT;
            break;
        case 13://MS_Upd_Wzone_Trip:
            ret = USTATUS_UPD_TP_OUT;
            break;
        case 14://MS_Upd_Wzone_Free:
            ret = USTATUS_UPD_IN;
            break;
        case 15://MS_Upd_Wzone_Free:
            ret = USTATUS_DETROY;
            break;
        default:
            ret = 0xFF;
    }
    return ret;
}

//====================================================
//函数:  setularea
//功能: 对单程票的数据处理信息区进行指定内容填充
//入口参数:
//                       valbuf: 处理数区首址
//                       val_idx: 处理区对应的变量序列编号
//                                    0=线路; 1=站点; 2=设备类型 ; 3=设备类型 ; 4=余额 ;
//                                    5=状态; 6=标记; 7=进站线路; 8=进站站点; 9=交易累计
//
//                        val: 要写入的值
//出口参数:  无
//====================================================
void setularea(uint8_t *valbuf,int val_idx, uint16_t val)
{
    uint8_t tmp1;
    uint16_t tmp2;
    switch (val_idx)
    {
        case 0:
            //线路
            //dbg_formatvar("val=%d",val);
            tmp1 = val;
            valbuf[0] &= 0x03;
            valbuf[0] |= (tmp1 << 2);
            break;
        case 1:
            //站点
            tmp1 = val;
            valbuf[0] &= 0xFC;
            valbuf[0] |= ((tmp1 & 0x3F) >> 4);
            valbuf[1] &= 0x0F;
            valbuf[1] |= ((tmp1 & 0x0F) << 4);
            break;
        case 2:
            //设备类型
            tmp1 = val;
            valbuf[1] &= 0xF0;
            valbuf[1] |= ((tmp1 & 0x0F) );
            break;
        case 3:
            //设备类型
            tmp2 = val;
            valbuf[2] &= 0x00;
            valbuf[3] &= 0x3F;
            valbuf[2] |= ((tmp2 & 0x03FF) >> 2);
            tmp1 = tmp2;
            valbuf[3] |= (tmp1 << 6);
            break;
        case 4:
            //余额
            tmp2 = val;
            valbuf[3] &= 0xC0;
            valbuf[4] &= 0x00;
            valbuf[3] |= ((tmp2 & 0x3FFF) >> 8);
            tmp1 = tmp2;
            valbuf[4] |= tmp1;
            break;
        case 5:
            //状态
            tmp1 = val;
            valbuf[5] &= 0x07;
            valbuf[5] |= ((tmp1 & 0x1F) <<3);
            break;
        case 6:
            // 标记
            tmp1 = val;
            valbuf[5] &= 0xF8;
            valbuf[5] |= (tmp1 & 0x07);
            break;
        case 7:
            //进站线路
            tmp1 = val;
            valbuf[6] &= 0x03;
            valbuf[6] |= (tmp1 << 2);
            break;
        case 8:
            //进站站点
            tmp1 = val;
            valbuf[6] &= 0xFC;
            valbuf[6] |= ((tmp1 & 0x3F) >> 4);
            valbuf[7] &= 0x0F;
            valbuf[7] |= ((tmp1 & 0x0F) << 4);
            break;
        case 9:
            //交易累计
            tmp2 = val;
            valbuf[9] = (tmp2 >> 8);
            valbuf[10] = tmp2 ;
            break;

    }

}





//单程票分析
uint16_t es_ul_read(uint8_t *lpoutput)
{
    uint8_t buf[500];
    int nresult=0;
    int counter_cmp;
    int m_valid_area_ptr = 0;
    int m_valid_area_startoffset;
    char sztmp[50];
    uint8_t utmp[50];
    uint16_t nret=0;
    PESANALYZE lpresult = (PESANALYZE)lpoutput;

    do
    {
        nresult = readul(SAM_SOCK_1, buf);

        switch (nresult)
        {
            case 0:
                //两个信息区都正确
                counter_cmp = memcmp(buf + 45, buf + 61, 2);
                if (counter_cmp >= 0 && memcmp(buf + 45, "\xff\xff", 2) != 0)
                    m_valid_area_ptr = 0;
                else
                    m_valid_area_ptr = 1;
                nret = 4;
                break;
            case 1:
                //信息区1正确
                m_valid_area_ptr = 1;
                nret = 4;
                break;
            case 2:
                //信息区0 正确
                m_valid_area_ptr = 0;
                nret = 4;
                break;
            case 3:
                //信息区都错误
                nret = 5;
                break;
            case -1:
                nret = 1;//无卡
                break;
            case -2:
                nret = 3;//读数据出错
                break;
            case -5:
                nret = 2;//sam卡操作失败
                break;
            default:
                //非系统卡
                nret = 2; //其它错误
                break;
        }

        if (nret < 3)
        {
            //未发行
            lpresult->bIssueStatus = 0; //未发行
            break;
        }

        if ( m_valid_area_ptr == 1)
        {
            m_valid_area_startoffset = 48; //有效处理信息区首字节地址
        }
        else
        {
            m_valid_area_startoffset = 32;//有效处理信息区首字节地址

        }

        //发行状态
        lpresult->bIssueStatus = 1; //已发行
        //状态
        utmp[0] = (((buf[m_valid_area_startoffset+9]) & 0xF8) >> 3);
        if (utmp[0]==15)
        {
            lpresult->bIssueStatus = 2;//已注销
        }

        lpresult->bStatus = unified_status(utmp[0]);


        //逻辑卡号
        sprintf(sztmp,"%012X%02X%02X%02X%02X", 0, buf[16], buf[17], buf[18], buf[19]);
        memcpy(lpresult->cLogicalID, sztmp, 20);
        //卡类型
        sprintf(sztmp,"%02X%02X",buf[26], buf[27]);
        memcpy(lpresult->cTicketType, sztmp, 4);
        //发行日期
        memset(utmp, 0x00, sizeof(utmp));
        TimesEx::tm2_bcd4_exchange(buf+24, utmp, 1);
        //dbg_dumpmemory("issue=",buf+24, 2);
        Publics::bcds_to_string(utmp, 7, lpresult->cIssueDate, 14);
        //dbg_dumpmemory("issuedate=",lpresult->cIssueDate, 14);
        //物理类型=1 单程票, =2 储值票
        lpresult->bCharacter = 1;
        //物理有效期
        //逻辑有效期开始时间
        //逻辑有效期结束时间
        TimesEx::tm4_bcd7_exchange(buf+m_valid_area_startoffset, utmp, true);
        Publics::bcds_to_string(utmp, 7, lpresult->cExpire, 8);
        Publics::bcds_to_string(utmp, 7, lpresult->cDateStart, 8);
        Publics::bcds_to_string(utmp, 7, lpresult->cDateEnd, 8);

        //SAM逻辑卡号


        //押金
        lpresult->lDeposite = 0; //单程票无押金
        //余额
        lpresult->lBalance = (buf[m_valid_area_startoffset+7]) & 0x3F;
        lpresult->lBalance <<= 8;
        lpresult->lBalance += buf[m_valid_area_startoffset+8];

        //线路
        utmp[0] = (buf[m_valid_area_startoffset+4] & 0xFC)>>2;
        sprintf(sztmp, "%02d", utmp[0]);
        memcpy(lpresult->cLine, sztmp, 2);

        //站点
        utmp[0] = (buf[m_valid_area_startoffset+4] & 0x03);
        utmp[0] <<= 4;
        utmp[0] += ((buf[m_valid_area_startoffset+5] & 0xF0)>>4);
        sprintf(sztmp, "%02d", utmp[0]);
        memcpy(lpresult->cStationNo, sztmp, 2);

        //add by shiyulong in 2013-10-14 , 增加交易累计返回

        lpresult->trade_count = buf[m_valid_area_startoffset+13];
        lpresult->trade_count <<= 8;
        lpresult->trade_count += buf[m_valid_area_startoffset+14];


        //以下参数单程票无需关心
        //多日票激活时间
        //多日票有效天数

        if (lpresult->bStatus==USTATUS_ES)
        {

            //限制进站线路
//         memcpy(lpresult->cLimitEntryLine, lpresult->cLine, 2);
//         memcpy(lpresult->cLimitEntryStation, lpresult->cStationNo, 2);

            //限制进站站点
        }

        //限制出站线路

        //限制出站站点

        //限制模式
        nret = 0;

    }
    while (0);

    //dbg_formatvar("=======================================");
    //dbg_dumpmemory("ret=",lpoutput,sizeof(ESANALYZE));
    //dbg_formatvar("~~");
    //dbg_formatvar("readul = %d",nret);
    //dbg_formatvar("issuestatus=%02X",lpresult->bIssueStatus);
    //dbg_formatvar("cTicketType=%02X",lpresult->bStatus);
    //dbg_formatvar("status=%02X",lpresult->cTicketType);
    //dbg_dumpmemory("logicalid=",lpresult->cLogicalID, 20);
    //dbg_dumpmemory("physicalid=",lpresult->cPhysicalID, 20);
    //dbg_formatvar("cphytype=%02X",lpresult->bCharacter, 1);
    //dbg_dumpmemory("cIssueDate=",lpresult->cIssueDate, 14);
    //dbg_dumpmemory("cExpire=",lpresult->cExpire, 8);
    //dbg_formatvar("Balance=%08X",lpresult->lBalance);
    //dbg_formatvar("lDeposite=%08X",lpresult->lDeposite);
    //dbg_dumpmemory("Line=",lpresult->cLine, 2);
    //dbg_dumpmemory("station=",lpresult->cStationNo, 2);
    //dbg_dumpmemory("cDateStart=",lpresult->cDateStart, 8);
    //dbg_dumpmemory("cDateEnd=",lpresult->cDateEnd, 8);
    //dbg_dumpmemory("dtDaliyActive=",lpresult->dtDaliyActive, 7);
    //dbg_dumpmemory("cLimitEntryLine=",lpresult->cLimitEntryLine, 2);
    //dbg_dumpmemory("cLimitEntryStation=",lpresult->cLimitEntryStation, 2);
    //dbg_dumpmemory("cLimitExitLine=",lpresult->cLimitExitLine, 2);
    //dbg_dumpmemory("cLimitExitStation=",lpresult->cLimitExitStation, 2);
    //dbg_dumpmemory("cLimitMode=",lpresult->cLimitMode, 3);
    //dbg_formatvar("trade_count=%d",lpresult->trade_count);

    return nret;

}

//储值票分析
uint16_t es_svt_read(uint8_t *lpoutput)
{

    uint8_t buf[500];
    //int nresult=0;
    uint16_t nret=0;
    //int counter_cmp;
    //int m_valid_area_ptr;
    //int m_valid_area_startoffset;
    char sztmp[50];
    uint8_t utmp[100];
    uint8_t tmp;
    unsigned long ntmp;
	unsigned char status_mtr	= 0;
	unsigned char status_globle	= 0;

    PESANALYZE lpresult = (PESANALYZE)lpoutput;


    do
    {
        // 复位
        if (svt_active(utmp) != 0)
        {
            nret = 1;
            //dbg_formatvar("active");
            break;
        }
        //dbg_formatvar("111111");

        lpresult->bIssueStatus = 0; //未发行

        // 选择主目录
        if (svt_selectfile(0x3f00) < 0)
        {

            nret = 2;
            break;

        }
        //dbg_formatvar("222222");

        //SAM逻辑卡号
        if (svt_selectfile(0x3F01) < 0)
        {
            if (svt_selectfile(0x1001) < 0)
            {
                nret = 4;

                break;
            }
        }

        // 选择主目录
        if (svt_selectfile(0x3f00) < 0)
        {

            nret = 2;
            break;

        }

        // 读发行基本信息
        if (svt_readbinary(0x05, 0, 0x28, buf) < 0)
        {
            nret = 5;
            break;
        }
        //dbg_formatvar("333333");

        //卡类型
        sprintf(sztmp,"%02X%02X",buf[16], buf[17]);
        memcpy(lpresult->cTicketType, sztmp, 4);


		//###########
		// wxf-20140401注释
		//###########
        ////发行状态
        //lpresult->bIssueStatus = 1; //已发行
        ////状态
        //utmp[0] = (((buf[9]) & 0xF8) >> 3);
        //if (utmp[0]==15)
        //{
        //    lpresult->bIssueStatus = 2;//已注销
        //}

        //lpresult->bStatus = unified_status(utmp[0]);


        //逻辑卡号
        sprintf(sztmp, "0000%02X%02X%02X%02X%02X%02X%02X%02X",
                buf[8], buf[9], buf[10], buf[11],
                buf[12], buf[13], buf[14], buf[15]);
        memcpy(lpresult->cLogicalID, sztmp, 20);
        //发行日期
        memset(utmp, 0x00, sizeof(utmp));
        memcpy(utmp, buf+18, 4);
        Publics::bcds_to_string(utmp, 7, lpresult->cIssueDate, 14);
        //物理类型=1 单程票, =2 储值票
        lpresult->bCharacter = 2;
        //物理有效期
        //逻辑有效期开始时间
        //逻辑有效期结束时间
        memcpy(utmp, buf+30, 4);
        Publics::bcds_to_string(utmp, 7, lpresult->cDateStart, 8);
        memcpy(utmp, buf+34, 4);
        Publics::bcds_to_string(utmp, 7, lpresult->cDateEnd, 8);
        //物理有效期修正
        Publics::bcds_to_string(utmp, 7, lpresult->cExpire, 8);

        //SAM逻辑卡号
        if (svt_selectfile(0x3F01) < 0)
        {
            if (svt_selectfile(0x1001) < 0)
            {
                nret = 4;

                break;
            }
        }

        //dbg_formatvar("444444");

        if (svt_getbalance(utmp)<0)
        {
            nret = 6;

            //dbg_formatvar("balance");
            break;
        }
        //dbg_formatvar("555555555555");

//        //dbg_dumpmemory("+balance=",utmp,4);

        //余额
        lpresult->lBalance = utmp[0];
        lpresult->lBalance <<= 8;
        lpresult->lBalance += utmp[1];
        lpresult->lBalance <<= 8;
        lpresult->lBalance += utmp[2];
        lpresult->lBalance <<= 8;
        lpresult->lBalance += utmp[3];

        // 轨道交通
        if (svt_readrecord(0x17, 2, LEN_METRO, buf) < 0)
        {
            nret = 7;

            break;
        }

        //dbg_formatvar("66666");

        //线路
//  //dbg_dumpmemory("buf=", buf,LEN_METRO);
//  //dbg_dumpmemory("station=======", buf+16,2);
        sprintf(sztmp, "%02X", buf[16]);
        memcpy(lpresult->cLine, sztmp, 2);

        //站点
        sprintf(sztmp, "%02X", buf[17]);
        memcpy(lpresult->cStationNo, sztmp, 2);

		//###########
		// wxf-20140401注释
		//###########
//        tmp = ((buf[4]>>3) & 0x1F);
////        //dbg_formatvar((char *)"status=%02X",tmp);
//
//        lpresult->bStatus = unified_status(tmp);
//
//        if ( lpresult->bStatus ==USTATUS_DETROY)
//        {
//            lpresult->bIssueStatus = 2;//已注销
//        }

		status_mtr = tmp = ((buf[4]>>3) & 0x1F);

        //以下参数单程票无需关心

        if (svt_readrecord(0x17, 0x11, LEN_CTRL_RECORD, buf) < 0)
        {
            nret = 8;

            break;
        }
        //dbg_formatvar("77777");

        //dbg_dumpmemory("0x11file=",buf, 0x18);
        //多日票激活时间
        memcpy(utmp, buf+4, 7);
        memcpy(lpresult->dtDaliyActive, buf+4, 7);



        // 读应用控制文件
        if (svt_readbinary(0x11, 0, LEN_APP_CTRL, buf) < 0)
        {
            nret = 9;
            break;

        }
        //dbg_dumpmemory("APP_CTRL=",buf, LEN_APP_CTRL);

		//###########
		// 状态信息
		//###########
		status_globle = buf[0];
		lpresult->bStatus = es_standard_status(status_globle, status_mtr);
		lpresult->bIssueStatus = es_get_issue_status(status_globle);

        //多日票有效天数

        ntmp = buf[1];
        ntmp <<= 8;
        ntmp += buf[2];
        ntmp <<= 8;
        ntmp += buf[3];
        ntmp <<= 8;
        ntmp += buf[4];

        lpresult->bEffectDay = ntmp/1440;

        //押金
        lpresult->lDeposite = buf[5]; //单程票无押金

        //限制模式

        //dbg_formatvar("8=%03d",buf[14]);
        sprintf(sztmp, "%03d", buf[14]);
        //dbg_formatvar("sztmp=%s",sztmp);
        memcpy(lpresult->cLimitMode, sztmp, 3);

        //限制进站线路
        sprintf(sztmp,"%02X",buf[15]);
        memcpy(lpresult->cLimitEntryLine, sztmp, 2);

        //限制进站站点
        sprintf(sztmp,"%02X",buf[16]);
        memcpy(lpresult->cLimitEntryStation, sztmp, 2);

        //限制出站线路
        sprintf(sztmp,"%02X",buf[17]);
        memcpy(lpresult->cLimitExitLine, sztmp, 2);

        //限制出站站点
        sprintf(sztmp,"%02X",buf[18]);
        memcpy(lpresult->cLimitExitStation, sztmp, 2);


        // 读持卡人信息

        if (svt_readbinary(0x16, 0, LEN_OWNER_BASE, buf) < 0)
        {
            nret = 10;
            break;
        }
        //////dbg_dumpmemory("private=",buf,56);
        lpresult->certificate_iscompany = buf[0];
        lpresult->certificate_ismetro = buf[1];
        memcpy(lpresult->certificate_name, buf+2,20);
        memcpy(lpresult->certificate_code, buf+22,32);
        lpresult->certificate_type = buf[54];
        lpresult->certificate_sex = buf[55];

        //add by shiyulong in 2013-10-14 , 增加交易累计返回
        lpresult->trade_count = 0;
        if (svt_getofflineserial(0,buf)>=0)
        {
            lpresult->trade_count = buf[0];
            lpresult->trade_count <<= 8;
            lpresult->trade_count += buf[1];
        }

        nret = 0;

    }
    while (0);

    //dbg_formatvar("=======================================");
    //dbg_formatvar("readsvt = %d",nret);
    //dbg_formatvar("issuestatus=%02X",lpresult->bIssueStatus);
    //dbg_formatvar("status=%02X",lpresult->bStatus);
    //dbg_formatvar("status=%02X",lpresult->cTicketType);
    //dbg_dumpmemory("logicalid=",lpresult->cLogicalID, 20);
    //dbg_dumpmemory("physicalid=",lpresult->cPhysicalID, 20);
    //dbg_formatvar("cphytype=%02X",lpresult->bCharacter, 1);
    //dbg_dumpmemory("cIssueDate=",lpresult->cIssueDate, 14);
    //dbg_dumpmemory("cExpire=",lpresult->cExpire, 8);
    //dbg_formatvar("Balance=%08X",lpresult->lBalance);
    //dbg_formatvar("lDeposite=%08X",lpresult->lDeposite);
    //dbg_dumpmemory("Line=",lpresult->cLine, 2);
    //dbg_dumpmemory("station=",lpresult->cStationNo, 2);
    //dbg_dumpmemory("cDateStart=",lpresult->cDateStart, 8);
    //dbg_dumpmemory("cDateEnd=",lpresult->cDateEnd, 8);
    //dbg_dumpmemory("dtDaliyActive=",lpresult->dtDaliyActive, 7);
    //dbg_formatvar("lpresult->bEffectDay=%d",lpresult->bEffectDay);
    //dbg_dumpmemory("cLimitEntryLine=",lpresult->cLimitEntryLine, 2);
    //dbg_dumpmemory("cLimitEntryStation=",lpresult->cLimitEntryStation, 2);
    //dbg_dumpmemory("cLimitExitLine=",lpresult->cLimitExitLine, 2);
    //dbg_dumpmemory("cLimitExitStation=",lpresult->cLimitExitStation, 2);
    //dbg_dumpmemory("cLimitMode=",lpresult->cLimitMode, 3);
    //dbg_formatvar("trade_count=%d",lpresult->trade_count);


    return nret;
}


//单程票初始化
uint16_t es_ul_init(uint8_t *lpinput, uint8_t *lpoutput)
{
    uint8_t ulpage[64];
    uint8_t sztmp[100];
    uint8_t sjtkey[7];
    //uint8_t crc8;
    uint16_t ntmp;
    uint16_t nret = 0;
    int nresult = -1;
//    LPTSJTDATAREA lparea=(LPTSJTDATAREA)&ulpage[32];

    do
    {
//  ////dbg_formatvar("sizeof=%d",sizeof(TSJTDATAREA));

        setvalue_issue(lpinput);
#ifdef _ENABLE_FIXKEY_
        memcpy(g_input.logicnumber, "\x00\x00\x00\x00\x30\x00\x10\x01", 8);
#endif
        memset(ulpage, 0, sizeof(ulpage));
        //发行区
        //memcpy(g_input.logicnumber, "\x30\x00\x10\x01", 4);
        memcpy(ulpage+16, g_input.logicnumber+4, 4);//系统流水号



// memcpy(sztmp, "\xEA\x32\x2C\x80", 4);//g_input.physical_id+3, 4);
        ////dbg_dumpmemory("physical=",g_input.physical_id,7);
        memcpy(sztmp, g_input.physical_id+3, 4);
        memcpy(sztmp+4, ulpage+16, 4);//g_input.logicnumber+4, 4);

#ifdef _ENABLE_FIXKEY_
        memcpy(g_input.key.mf_tkey, "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF", 16);
#endif

#ifdef _ENABLE_FIXKEY_
        TripleDes(sztmp, sztmp, g_input.key.mf_tkey, ENCRYPT);
        memcpy(ulpage+20, sztmp, 4);//认证信息
        //dbg_dumpmemory("sjtmak=",sztmp, 6);

#else
        memcpy(ulpage+20, g_input.key.mf_mkey, 4);//认证信息
#endif
        ////dbg_dumpmemory("issuezone=",ulpage+16, 8);

        memcpy(sztmp, g_input.physical_id+5, 2);


#ifdef _ENABLE_FIXKEY_
        memcpy(sztmp+2, "\x30\x00\x10\x01", 4);
        memcpy(sztmp+6, ulpage+20, 1);
        memcpy(sztmp+7, "\xA5", 1);

        memcpy(g_input.key.mf_mkey,  "\x12\x34\x56\x78\x9A\xBC\xDE\xF0\x12\x34\x56\x78\x9A\xBC\xDE\xF0", 16);
        TripleDes(sztmp, sztmp, g_input.key.mf_mkey, ENCRYPT);
        memcpy(sjtkey, sztmp, 6);
        //dbg_dumpmemory("sjtkey=",sjtkey, 6);
#else
        memcpy(sjtkey, g_input.key.mf_mkey+4, 6);
#endif
        ////dbg_dumpmemory("sjkey=",sztmp, 8);




// ////dbg_formatvar("MAC=%02X%02X%02X%02X", sztmp[0], sztmp[1], sztmp[2], sztmp[3]);
//
//  ////dbg_dumpmemory("MAC=", sztmp, 4);
//
//  nresult = 0;
//  break;

//  //dbg_dumpmemory("m_time_now=",CmdSort::m_time_now,7);

        //TimesEx::tm2_bcd4_exchange(ulpage+24,CmdSort::m_time_now, false);//发行日 期
        TimesEx::tm2_bcd4_exchange(ulpage+24,g_input.inf.issuedate, false);
        ulpage[25] &= 0xFE;
        ulpage[25] |= (g_input.inf.testflag & 0x01);//测试标记
        memcpy(ulpage+26, g_input.inf.cardtype, 2); //卡类型( 主类型+ 子类型 )
		//Records::log_buffer("04块:", ulpage + 16, 16);
        nresult = writeul_direct(4, 16, ulpage+16);
        if (nresult)
        {
            //dbg_formatvar("wrul2=%d",nresult);
            nret = 1;

            break;
        }

//  TimesEx::tm2_bcd4_exchange(ulpage+24, sztmp, 1);
//  //dbg_dumpmemory("m_time_now2=",sztmp,7);

//  Publics::bcds_to_string(utmp, 7, lpresult->cIssueDate, 14);


// ////dbg_dumpmemory("40_1=",ulpage+36,16);


        //dbg_formatvar("current_station_id=%04X",Api::current_station_id);
        setularea(ulpage+36, 0, Publics::bcd_to_val(((Api::current_station_id >> 8) & 0x00FF))); //线路
        setularea(ulpage+36, 1, Publics::bcd_to_val((Api::current_station_id & 0xFF))); //站点
        setularea(ulpage+36, 2, 9);//设备类型
        setularea(ulpage+36, 3, Publics::bcd_to_val(Api::current_device_id));

        //dbg_formatvar("balance=%d",g_input.balance);
        if (g_input.balance>0)
        {
            memset(sztmp, 0, sizeof(sztmp));
//            memcpy(sztmp, g_input.inf.effectiveto, 4);
            memcpy(sztmp, g_input.inf.logiceffectivefrom, 4);
            TimesEx::tm4_bcd7_exchange(ulpage+32, sztmp, false);// 处理日期

            ntmp = g_input.balance;
            setularea(ulpage+36, 4, ntmp);

            setularea(ulpage+36, 5, 2);//写入预赋值状态
            //dbg_formatvar("entryline=%02X",g_input.inf.entryline);
            setularea(ulpage+36, 0, Publics::bcd_to_val((g_input.inf.entryline& 0x00FF))); //线路
            setularea(ulpage+36, 1, Publics::bcd_to_val((g_input.inf.entrystation & 0xFF))); //站点

        }
        else
        {
            TimesEx::tm4_bcd7_exchange(ulpage+32,CmdSort::m_time_now, false);//处理日 期
            ////dbg_dumpmemory("ulpagetm=",ulpage+32,4);

            setularea(ulpage+36, 5, 1); //写入初始化状态

        }

        setularea(ulpage+36, 9, 1);

		//Records::log_buffer("12块:", ulpage + 48, 16);
		ul_4page_encyrpt(ulpage+48, sjtkey[1], sjtkey+2);
		nresult = writeul_direct(12, 16, ulpage+48);
        if (nresult)
        {
            //dbg_formatvar("wrul=%d",nresult);
            nret = 2;
            break;
        }


        //////dbg_dumpmemory("40_2=",ulpage+36,16);
        ////dbg_dumpmemory("wzone1=",ulpage+32,16);
		//Records::log_buffer("08块-1:", ulpage + 32, 16);
        ul_4page_encyrpt(ulpage+32, sjtkey[0], sjtkey+2);
		//Records::log_buffer("08块-2:", ulpage + 32, 16);
        //nresult=writeul(0, 0, ulpage+32);
        ////dbg_dumpmemory("wzone2=",ulpage+32,16);
        nresult = writeul_direct(8, 16, ulpage+32);

        if (nresult)
        {
            //dbg_formatvar("wrul=%d",nresult);
            nret = 3;
            break;
        }


        nret = 0;

        Beep_Sleep(1);

    }
    while (0);
    //dbg_formatvar("writeul=%d",nresult);

    return nret;
}

//==============================================
//单程票和储值票的业务实现
//==============================================


//储值票初始化
uint16_t es_svt_init(uint8_t *lpinput, uint8_t *lpoutput)
{

    //unsigned char sztmp[200];

    int nresult;
    uint16_t nret = 0;

    do
    {
//        searchcard(sztmp);


//        svt_active(sztmp);
//  ////dbg_dumpmemory((char *)"Init_input = ", param_stack, 199);
        setvalue_issue(lpinput);
#ifdef _ENABLE_FIXKEY_
        memcpy(g_input.logicnumber, l_logicnumber, 8);
#endif
//  return 5;

#ifndef _NEWTICKET_

        //删除结构，正式环境不用
//         if (svt_reset() != 0)
//         {
//             ret = 0xF001;//ERR_CLEAR_FILE;
//             //break;
//         }

        nresult=svt_create_mf();
        //dbg_formatvar("svt_create_mf=%d",nresult);
        if (nresult!= 0)
        {
            nret = nresult;

            break;
        }

        nresult = svt_create_adf1();
        if ( nresult!= 0)
        {
            nret = 100+nresult;
            break;
        }
#else
        //删除结构，正式环境不用
//         if (svt_new_reset() != 0)
//         {
//             ret = 0xF001;//ERR_CLEAR_FILE;
//             //break;
//         }

        nresult=svt_new_create_mf();
        //dbg_formatvar("svt_create_mf=%d",nresult);
        if (nresult!= 0)
        {
            nret = nresult;

            break;

        }

        nresult = svt_new_create_adf1();
        //dbg_formatvar("svt_create_adf1=%d",nresult);
        if ( nresult!= 0)
        {
            nret = 100+nresult;
            break;
        }


#endif
        nret = 0;

    }
    while (0);

    return nret;

}


//单程票预赋值接口
uint16_t es_ul_evaluate(uint8_t *lpinput, uint8_t *lpoutput)
{
    uint8_t buf[100];
    uint8_t wpagebuf[100];
    int nresult=0;
    int counter_cmp;
    int m_valid_area_ptr;
    int m_valid_area_startoffset;
    int m_next_area_startoffset;
    int m_next_startpage;

    unsigned short m_current_count;
    uint8_t sztmp[50];
    uint16_t ntmp;
//    uint8_t utmp[50];
    //PESANALYZE lpresult = (PESANALYZE)lpoutput;

    do
    {
        setvalue_evaluate(lpinput);

        nresult = readul(SAM_SOCK_1, buf);

        switch (nresult)
        {
            case 0:
                //两个信息区都正确
                counter_cmp = memcmp(buf + 45, buf + 61, 2);
                if (counter_cmp >= 0 && memcmp(buf + 45, "\xff\xff", 2) != 0)
                    m_valid_area_ptr = 0;
                else
                    m_valid_area_ptr = 1;

                nresult = 1;
                break;
            case 1:
                //信息区1正确
                m_valid_area_ptr = 1;
                nresult = 1;
                break;
            case 2:
                //信息区0 正确
                m_valid_area_ptr = 0;
                nresult = 1;
                break;
            case 3:
                //信息区都错误
                nresult = 0;
                break;
            default:
                //非系统卡
                nresult = -1;
                break;
        }

        if (nresult < 0)
        {
            nresult = 0x0A;
            break;
        }

        if (nresult>0)
        {
            if ( m_valid_area_ptr == 1)
            {
                m_valid_area_startoffset = 48; //当前有效处理信息区首字节地址
                m_next_area_startoffset = 32; //要写入的首址
                m_next_startpage = 8;

            }
            else
            {
                m_valid_area_startoffset = 32;//当前有效处理信息区首字节地址
                m_next_area_startoffset = 48; //要写入的首址
                m_next_startpage = 12;

            }

            m_current_count = buf[m_valid_area_startoffset+13];
            m_current_count <<= 8;
            m_current_count += buf[m_valid_area_startoffset+14];

            m_current_count++;


        }
        else
        {
            m_current_count = 0;
            m_valid_area_startoffset = 32;//有效处理信息区首字节地址
            m_next_area_startoffset = 32; //要写入的首址
            m_next_startpage = 8;

        }

        memset(wpagebuf, 0x00, sizeof(wpagebuf));

        setularea(wpagebuf+4, 0, Publics::bcd_to_val((g_input.inf.entryline& 0x00FF))); //线路
        setularea(wpagebuf+4, 1, Publics::bcd_to_val((g_input.inf.entrystation & 0xFF))); //站点

//        setularea(wpagebuf+4, 0, g_input.inf.entryline);//线路
//        setularea(wpagebuf+4, 1, g_input.inf.entrystation);//站点



        setularea(wpagebuf+4, 2, 9);//设备类型
        setularea(wpagebuf+4, 3, Publics::bcd_to_val(Api::current_device_id));
//        if (g_input.balance>0)
        {
            memset(sztmp, 0, sizeof(sztmp));
//            memcpy(sztmp, CmdSort::m_time_now, 4);
            memcpy(sztmp, g_input.inf.logiceffectivefrom, 4);
            TimesEx::tm4_bcd7_exchange(wpagebuf, sztmp, false);// 处理日期

            ntmp = g_input.balance;
            setularea(wpagebuf+4, 4, ntmp);

            setularea(wpagebuf+4, 5, 2);//写入预赋值状态

        }
//         else
//         {
//             TimesEx::tm4_bcd7_exchange(wpagebuf,CmdSort::m_time_now, false);//处理日 期
//
//             setularea(wpagebuf+4, 5, 1); //写入初始化状态
//
//             //lparea->status=1;
//         }
        setularea(wpagebuf+4, 9, m_current_count);

        nresult = writeul(8, 16, wpagebuf);

        if (nresult)
        {
            //////dbg_formatvar("wrul=%d",nresult);
            break;
        }

        nresult = 0;
        Beep_Sleep(1);


    }
    while (0);


    return nresult;

}

//储值票预赋值接口
uint16_t es_svt_evaluate(uint8_t *lpinput, uint8_t *lpoutput)
{
    unsigned char ulen;
    unsigned char respone_len;
    unsigned char sztmp[256];
    unsigned char szcmd[256];
    unsigned char szdata[8];
    //unsigned char tmp_len;
    unsigned char sam_len;
    unsigned short respone_sw;
    unsigned long value=1;
    InitforLoadStr_t InitforLoad;

    unsigned char moneybuf[16];
    unsigned char calMacKey[8];
    unsigned char m_datetime[8];
    unsigned char inf[100];

    //LPTPUBLICINF lppublic;
    //LPTMF_ISSUE lpmfissueinf;
//    PESANALYZE lpresult;
    int nresult=0;

    do
    {
        setvalue_evaluate(lpinput);

        memcpy(m_datetime, CmdSort::m_time_now, 7);

#ifdef _ENABLE_FIXKEY_
        memcpy(g_input.key.adf1_mamk_02, l_key.adf1_mamk_02, 16);
#endif

        if (svt_selectfile(0x1001) < 0)
        {
            memcpy(szdata, "\xA0\x00\x00\x00\x03\x86\x98\x07\x01", 9);
            if (svt_selecfileaid(9,szdata) < 0)
            {
                nresult = 4;//ERR_CARD_READ;
                break;
            }
        }


//         ulen = 8;
//         memcpy(sztmp, "\x00\x20\x00\x00\x03\x12\x34\x56",8);
//         ////dbg_dumpmemory("0020S=",sztmp,ulen);
//         respone_len = MifareProCom(ulen,sztmp, &respone_sw);
//         ////dbg_dumpmemory("0020R=",sztmp,respone_len);
//         if ((respone_len==0 ) || (0x9000!=respone_sw))
//         {
//             nresult = 0xF011;//ERR_VERIFYPIN;
//             break;
//
//         }
//
//   value = g_input.balance;
//
//         //获取充值信息
//         sam_len = 0;
//         memcpy(sztmp, "\x80\x50\x00\x02\x0B",5);
//         sam_len += 5;
//         sztmp[sam_len] = 0x01;
//         sam_len += 1;
//         sztmp[sam_len] = ((value >> 24) & 0xFF);
//         sam_len += 1;
//         sztmp[sam_len] = ((value >> 16) & 0xFF);
//         sam_len += 1;
//         sztmp[sam_len] = ((value >> 8) & 0xFF);
//         sam_len += 1;
//         sztmp[sam_len] = (value & 0xFF);
//         sam_len += 1;
//         memcpy(sztmp+sam_len, "\x11\x22\x33\x44\x55\x66", 6);
//         sam_len += 6;
//
//
//         ////dbg_dumpmemory("8050S=",sztmp,sam_len);
//         respone_len = MifareProCom(sam_len,sztmp, &respone_sw);
//         ////dbg_dumpmemory("8050R=",sztmp,respone_len);
//
//         if ((respone_len==0 ) || (0x9000!=respone_sw))
//         {
//             nresult = 0xF012;//ERR_RECHARGE_INIT;
//             break;
//         }
//         memcpy((char *)&InitforLoad, sztmp, sizeof(InitforLoadStr_t));
//
//         memset(moneybuf, 0, sizeof(moneybuf));
//         memcpy(moneybuf, &InitforLoad.Random, 4);
//         memcpy(moneybuf+4, &InitforLoad.OnlineDealSerial, 2);
//         memcpy(moneybuf+6, "\x80\x00", 2);
//         ////dbg_dumpmemory("adf1_mlk_0201=",g_input.key.adf1_mlk_0201, 16);
//         TripleDes(calMacKey, moneybuf, g_input.key.adf1_mlk_0201, ENCRYPT);
//
//         memset(sztmp, 0, sizeof(sztmp));
//         sztmp[0] = (value & 0xFF000000) >> 24;
//         sztmp[1] = (value & 0x00FF0000) >> 16;
//         sztmp[2] = (value & 0x0000FF00) >> 8;
//         sztmp[3] =  (value & 0x000000FF);
//
//         memcpy(sztmp+4, "\x02", 1);
//         memcpy(sztmp+5, "\x11\x22\x33\x44\x55\x66", 6);
//
//         memcpy(sztmp+11, m_datetime, 7); // bcd time
//
//
//         MAC(calMacKey, 18, (const unsigned char *)("\x00\x00\x00\x00\x00\x00\x00\x00"), sztmp, sztmp);
//
//
//         memcpy(calMacKey, sztmp, 4);
//
//         memset(sztmp, 0, sizeof(sztmp));
//
//         memcpy(sztmp, "\x80\x52\x00\x00\x0B", 5);
//         //    memcpy(tmpbuf+5,"\x20\x11\x01\x20\x14\x20\x20", 7);
//         memcpy(sztmp+5,  m_datetime, 7); // bcd time
//         memcpy(sztmp+12,calMacKey, 4);
//         sam_len = 16;
//
//         ////dbg_dumpmemory("8052S=",sztmp,sam_len);
//         respone_len = MifareProCom(sam_len,sztmp, &respone_sw);
//         ////dbg_dumpmemory("8052R=",sztmp,respone_len);
//         if ((respone_len==0 ) || (0x9000!=respone_sw))
//         {
//             nresult = 0xf013;//ERR_RECHARGE_WRITE;
//             break;
//         }
//
//         if (svt_readrecord(0x17,2,0,inf)<0)
//         {
//             nresult = 4;//ERR_CARD_READ;
//             break;
//         }
//
//         if (apdu_getrandom(szdata,4)==0)
//         {
//
//             inf[4] = 0x11;
//
//
//             //////dbg_dumpmemory("random =",szdata,8);
//             inf[4] = 0x11;
//             memcpy(szcmd, "\x04\xDC\x02\xB8\x34", 5);
//             ulen = 5;
//             memcpy(szcmd+5, inf, 48);
//             ulen +=48;
//             MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);
//             ulen += 4;
//             ////dbg_dumpmemory("update complex =",szcmd,ulen);
//             respone_len = MifareProCom(ulen, szcmd, &respone_sw);
//             ////dbg_dumpmemory("update complex |=",szcmd,respone_len);
//             if ((respone_len==0) || (0x9000!=respone_sw))
//             {
//                 //nresult = -2;
//                 break;
//             }
//             Beep_Sleep(1);
//
//         }

        memset(inf, 0x00, sizeof(inf));
        if (svt_readbinary(0x11, 0, LEN_APP_CTRL, inf) < 0)
        {
            nresult = 5;
            break;

        }


        //更新0011应用控制文件
        if (apdu_getrandom(szdata,4)==0)
        {

            ulen = 0;
            inf[ulen++] = g_input.inf.saleflag;
            memcpy(inf+ulen,g_input.inf.szlogicectivetime, 4);
            ulen += 4;
            inf[ulen++]=g_input.inf.deposit;
            //====
            ulen += 6; //发售设备信息
            //====
            memcpy(inf+ulen, g_input.inf.szcaps, 2); //充值上限
            ulen += 2;

            inf[ulen++]=g_input.inf.passmode;   //通讯模式
            inf[ulen++]=g_input.inf.entryline;   //可入线路
            inf[ulen++]=g_input.inf.entrystation; //可入站点
            inf[ulen++]=g_input.inf.exitline;   //可出线路
            inf[ulen++]=g_input.inf.exitstation;  //可出站点


            memset(szcmd, 0x00, sizeof(szcmd));
            ulen = 0;
            memcpy(szcmd, "\x04\xD6\x91\x00\x24",5);
            ulen += 5;
            memcpy(szcmd+ulen, inf, LEN_APP_CTRL);
            ulen += LEN_APP_CTRL;

			//dbg_dumpmemory("svt=adf1_mamk_02",g_input.key.adf1_mamk_02,16);
            MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("update adf1 file_0011 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("update adf1 file_0011 |=",szcmd,respone_len);
            if (0x9000!=respone_sw)
            {
                nresult = 6;//ERR_RECHARGE_INIT;
                break;
            }

//    inf[1]=g_input.inf.deposit;
//    inf[8]=g_input.inf.passmode;
//    inf[9]=g_input.inf.entryline;
//    inf[10]=g_input.inf.entrystation;
//    inf[11]=g_input.inf.exitline;
//    inf[12]=g_input.inf.exitstation;
//
//    memset(szcmd, 0x00, sizeof(szcmd));
//    ulen = 0;
//    memcpy(szcmd, "\x04\xD6\x91\x00\x24",5);
//    ulen += 5;
//    memcpy(szcmd+ulen, inf, LEN_APP_CTRL);
//    ulen += LEN_APP_CTRL;
//
//    MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);
//    ulen += 4;
//    //////dbg_dumpmemory("update adf1 file_0011 =",szcmd,ulen);
//    respone_len = MifareProCom(ulen, szcmd, &respone_sw);
//    //dbg_dumpmemory("update adf1 file_0011 |=",szcmd,respone_len);

        }


		if (g_input.balance>0)
		{
	        ulen = 8;
	        memcpy(sztmp, "\x00\x20\x00\x00\x03\x12\x34\x56",8);
	        ////dbg_dumpmemory("0020S=",sztmp,ulen);
	        respone_len = MifareProCom(ulen,sztmp, &respone_sw);
	        ////dbg_dumpmemory("0020R=",sztmp,respone_len);
	        if ((respone_len==0 ) || (0x9000!=respone_sw))
	        {
	            nresult = 7;//ERR_VERIFYPIN;
	            break;

	        }

	        value = g_input.balance;

	        //获取充值信息
	        sam_len = 0;
	        memcpy(sztmp, "\x80\x50\x00\x02\x0B",5);
	        sam_len += 5;
	        sztmp[sam_len] = 0x01;
	        sam_len += 1;
	        sztmp[sam_len] = ((value >> 24) & 0xFF);
	        sam_len += 1;
	        sztmp[sam_len] = ((value >> 16) & 0xFF);
	        sam_len += 1;
	        sztmp[sam_len] = ((value >> 8) & 0xFF);
	        sam_len += 1;
	        sztmp[sam_len] = (value & 0xFF);
	        sam_len += 1;
	        memcpy(sztmp+sam_len, "\x11\x22\x33\x44\x55\x66", 6);
	        sam_len += 6;


	        //dbg_dumpmemory("8050S=",sztmp,sam_len);
	        respone_len = MifareProCom(sam_len,sztmp, &respone_sw);
	        //dbg_dumpmemory("8050R=",sztmp,respone_len);

	        if ((respone_len==0 ) || (0x9000!=respone_sw))
	        {
	            nresult = 8;//ERR_RECHARGE_INIT;
	            break;
	        }
	        memcpy((char *)&InitforLoad, sztmp, sizeof(InitforLoadStr_t));

	        memset(moneybuf, 0, sizeof(moneybuf));
	        memcpy(moneybuf, &InitforLoad.Random, 4);
	        memcpy(moneybuf+4, &InitforLoad.OnlineDealSerial, 2);
	        memcpy(moneybuf+6, "\x80\x00", 2);
	        ////dbg_dumpmemory("adf1_mlk_0201=",g_input.key.adf1_mlk_0201, 16);
	        TripleDes(calMacKey, moneybuf, g_input.key.adf1_mlk_0201, ENCRYPT);

	        memset(sztmp, 0, sizeof(sztmp));
	        sztmp[0] = (value & 0xFF000000) >> 24;
	        sztmp[1] = (value & 0x00FF0000) >> 16;
	        sztmp[2] = (value & 0x0000FF00) >> 8;
	        sztmp[3] = (value & 0x000000FF);

	        memcpy(sztmp+4, "\x02", 1);
	        memcpy(sztmp+5, "\x11\x22\x33\x44\x55\x66", 6);

	        memcpy(sztmp+11, m_datetime, 7); // bcd time


	        MAC(calMacKey, 18, (const unsigned char *)("\x00\x00\x00\x00\x00\x00\x00\x00"), sztmp, sztmp);


	        memcpy(calMacKey, sztmp, 4);

	        memset(sztmp, 0, sizeof(sztmp));

	        memcpy(sztmp, "\x80\x52\x00\x00\x0B", 5);
	        //    memcpy(tmpbuf+5,"\x20\x11\x01\x20\x14\x20\x20", 7);
	        memcpy(sztmp+5,  m_datetime, 7); // bcd time
	        memcpy(sztmp+12,calMacKey, 4);
	        sam_len = 16;

	        //dbg_dumpmemory("8052S=",sztmp,sam_len);
	        respone_len = MifareProCom(sam_len,sztmp, &respone_sw);
	        //dbg_dumpmemory("8052R=",sztmp,respone_len);
	        if ((respone_len==0 ) || (0x9000!=respone_sw))
	        {
	            nresult = 9;//ERR_RECHARGE_WRITE;
	            break;
	        }
        }

        if (svt_readrecord(0x17,2,0,inf)<0)
        {
            nresult = 10;//ERR_CARD_READ;
            break;
        }

        if (apdu_getrandom(szdata,4)==0)
        {

            inf[4] = 0x11;


            //////dbg_dumpmemory("random =",szdata,8);
            inf[4] = 0x11;
            add_metro_area_crc(inf, LEN_METRO);
            memcpy(szcmd, "\x04\xDC\x02\xB8\x34", 5);
            ulen = 5;
            memcpy(szcmd+5, inf, 48);
            ulen +=48;
            MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //dbg_dumpmemory("update complex =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("update complex |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 11;
                break;
            }


        }

        if (svt_readrecord(0x17,0x11,0,inf)<0)
        {
            nresult = 12;//ERR_CARD_READ;
            break;
        }

        if (apdu_getrandom(szdata,4)==0)
        {

            memcpy(inf+4, g_input.inf.logiceffectivefrom, 7);
			if (/*(g_input.balance>0) &&*/(memcmp(g_input.inf.logiceffectivefrom,"\x00\x00\x00\x00\x00\x00\x00",7)!=0)&&(g_input.inf.saleflag==1))
			{
				inf[2] = 1;
			}

            memcpy(szcmd, "\x04\xDC\x11\xB8\x1C", 5);
            ulen = 5;
            memcpy(szcmd+5, inf, 24);
            ulen +=24;
            MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //dbg_dumpmemory("update complex =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("update complex |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 13;
                break;
            }
            Beep_Sleep(1);
            //dbg_formatvar("nresult = %d",nresult);

        }



    }
    while (0);

    return nresult;


}

//单程票注销接口
uint16_t es_ul_destroy(uint8_t *lpinput, uint8_t *lpoutput)
{
    uint8_t buf[100];
    uint8_t wpagebuf[100];
    int nresult=0;
    int counter_cmp;
    int m_valid_area_ptr;
    int m_valid_area_startoffset;
    int m_next_area_startoffset;
    int m_next_startpage;

    unsigned short m_current_count;
    //uint8_t sztmp[50];
    //uint16_t ntmp;
    //   uint8_t utmp[50];
    //PESANALYZE lpresult = (PESANALYZE)lpoutput;

    do
    {
        setvalue_evaluate(lpinput);

        nresult = readul(SAM_SOCK_1, buf);

        switch (nresult)
        {
            case 0:
                //两个信息区都正确
                counter_cmp = memcmp(buf + 45, buf + 61, 2);
                if (counter_cmp >= 0 && memcmp(buf + 45, "\xff\xff", 2) != 0)
                    m_valid_area_ptr = 0;
                else
                    m_valid_area_ptr = 1;

                nresult = 1;
                break;
            case 1:
                //信息区1正确
                m_valid_area_ptr = 1;
                nresult = 1;
                break;
            case 2:
                //信息区0 正确
                m_valid_area_ptr = 0;
                nresult = 1;
                break;
            case 3:
                //信息区都错误
                nresult = 0;
                break;
            default:
                //非系统卡
                nresult = -1;
                break;
        }

        if (nresult < 0)
        {
            nresult = 0x0A;
            break;
        }

        if (nresult>0)
        {
            if ( m_valid_area_ptr == 1)
            {
                m_valid_area_startoffset = 48; //当前有效处理信息区首字节地址
                m_next_area_startoffset = 32; //要写入的首址
                m_next_startpage = 8;

            }
            else
            {
                m_valid_area_startoffset = 32;//当前有效处理信息区首字节地址
                m_next_area_startoffset = 48; //要写入的首址
                m_next_startpage = 12;

            }

            m_current_count = buf[m_valid_area_startoffset+13];
            m_current_count <<= 8;
            m_current_count += buf[m_valid_area_startoffset+14];

            m_current_count++;


        }
        else
        {
            m_current_count = 0;
            m_valid_area_startoffset = 32;//有效处理信息区首字节地址
            m_next_area_startoffset = 32; //要写入的首址
            m_next_startpage = 8;

        }

        memset(wpagebuf, 0x00, sizeof(wpagebuf));

        setularea(wpagebuf+4, 0, g_input.inf.entryline);//设备类型
        setularea(wpagebuf+4, 1, g_input.inf.entryline);//设备类型
        setularea(wpagebuf+4, 2, 9);//设备类型
        setularea(wpagebuf+4, 3, Publics::bcd_to_val(Api::current_device_id));

        TimesEx::tm4_bcd7_exchange(wpagebuf+m_next_area_startoffset,CmdSort::m_time_now, false);//处理日 期

        setularea(wpagebuf+4, 5, 15); //写入注销

        setularea(wpagebuf+4, 9, m_current_count);

        nresult = writeul(8, 16, wpagebuf);

        if (nresult)
        {
            //////dbg_formatvar("wrul=%d",nresult);
            break;
        }

        nresult = 0;
        Beep_Sleep(1);


    }
    while (0);


    return nresult;

}


//储值票注销接口
uint16_t es_svt_destroy(uint8_t *lpinput, uint8_t *lpoutput)
{
    unsigned char ulen;
    unsigned char respone_len;
    //unsigned char sztmp[256];
    unsigned char szcmd[256];
    unsigned char szdata[8];
    //unsigned char tmp_len;
    //unsigned char sam_len;
    unsigned short respone_sw;
    unsigned char inf[100];
    uint16_t nresult = 0;
    do
    {
        ////dbg_formatvar("destory");
        setvalue_destroy(lpinput);
#ifdef _ENABLE_FIXKEY_
        memcpy(g_input.key.adf1_mamk_02, l_key.adf1_mamk_02, 16);
#endif

        if (svt_selectfile(0x1001) < 0)
        {
            memcpy(szdata,"\xA0\x00\x00\x00\x03\x86\x98\x07\x01", 9);
            if (svt_selecfileaid(9,szdata) < 0)
            {
                nresult = 4;//ERR_CARD_READ;
                break;
            }
        }


        ////dbg_formatvar("destory2");

        if (apdu_getrandom(szdata,4)==0)
        {

            memset(inf, 0x00, sizeof(inf));
            inf[0]=3;
            memset(szcmd, 0x00, sizeof(szcmd));
            ulen = 0;
            memcpy(szcmd, "\x04\xD6\x91\x00\x05",5);
            ulen += 5;
            memcpy(szcmd+ulen, inf, 1);
            ulen += 1;

			//dbg_dumpmemory("svt=adf1_mamk_02",g_input.key.adf1_mamk_02,16);
            MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);

            ulen += 4;
            //dbg_dumpmemory("update adf1 file_0011 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("update adf1 file_0011 |=",szcmd,respone_len);
            if (0x9000!=respone_sw)
            {
                nresult = 5;//ERR_RECHARGE_INIT;
                break;
            }


        }

        if (svt_readrecord(0x17,2,0,inf)<0)
        {
            nresult = 4;//ERR_CARD_READ;
            break;
        }
        ////dbg_dumpmemory("complex file=",inf ,48);

        if (apdu_getrandom(szdata,4)==0)
        {

            inf[4] = (inf[4] & 0x07) | 0x78;
            inf[16] = ((Api::current_station_id >> 8) & 0x00FF);
            inf[17] = (Api::current_station_id & 0xFF);
            inf[18] = Api::current_device_type;
            inf[18] <<= 4;
            inf[18] &= 0xF0;
            inf[18] |= ((Api::current_device_id >> 12) & 0x0F);
            inf[19] |= (Api::current_device_id & 0xFF);


            memcpy(szcmd, "\x04\xDC\x02\xBC\x34", 5);
            ulen = 5;
            memcpy(szcmd+5, inf, 48);
            ulen +=48;
            ////dbg_dumpmemory("random =",szdata,8);
            ////dbg_dumpmemory("g_input.key.adf1_mamk_02=",g_input.key.adf1_mamk_02, 16);
            MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            ////dbg_dumpmemory("update complex =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            ////dbg_dumpmemory("update complex |=",szcmd,respone_len);
            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 6;
                break;
            }
            nresult = 0;
            Beep_Sleep(1);

        }



    }
    while (0);

    return nresult;

}

//单程票重编码接口
uint16_t es_ul_recode(uint8_t *lpinput, uint8_t *lpoutput)
{
    uint8_t buf[100];
    uint8_t wpagebuf[100];
    int nresult=0;
    int counter_cmp;
    int m_valid_area_ptr;
    int m_valid_area_startoffset;
    int m_next_area_startoffset;
    int m_next_startpage;
    unsigned short m_current_count;
    //uint8_t sztmp[50];
    //uint16_t ntmp;
    //   uint8_t utmp[50];
    //PESANALYZE lpresult = (PESANALYZE)lpoutput;

    do
    {
        setvalue_issue(lpinput);

        nresult = readul(SAM_SOCK_1, buf);

        switch (nresult)
        {
            case 0:
                //两个信息区都正确
                counter_cmp = memcmp(buf + 45, buf + 61, 2);
                if (counter_cmp >= 0 && memcmp(buf + 45, "\xff\xff", 2) != 0)
                    m_valid_area_ptr = 0;
                else
                    m_valid_area_ptr = 1;

                nresult = 1;
                break;
            case 1:
                //信息区1正确
                m_valid_area_ptr = 1;
                nresult = 1;
                break;
            case 2:
                //信息区0 正确
                m_valid_area_ptr = 0;
                nresult = 1;
                break;
            case 3:
                //信息区都错误
                nresult = 0;
                break;
            default:
                //非系统卡
                nresult = -1;
                break;
        }

        if (nresult < 0)
        {
            nresult = 0x0A;
            break;
        }

        if (nresult>0)
        {
            if ( m_valid_area_ptr == 1)
            {
                m_valid_area_startoffset = 48; //当前有效处理信息区首字节地址
                m_next_area_startoffset = 32; //要写入的首址
                m_next_startpage = 8;
            }
            else
            {
                m_valid_area_startoffset = 32;//当前有效处理信息区首字节地址
                m_next_area_startoffset = 48; //要写入的首址
                m_next_startpage = 12;
            }

            m_current_count = buf[m_valid_area_startoffset+13];
            m_current_count <<= 8;
            m_current_count += buf[m_valid_area_startoffset+14];

            m_current_count++;


        }
        else
        {
            m_current_count = 0;
            m_valid_area_startoffset = 32;//有效处理信息区首字节地址
            m_next_area_startoffset = 32; //要写入的首址
            m_next_startpage = 8;

        }

        memset(wpagebuf, 0x00, sizeof(wpagebuf));

        setularea(wpagebuf+4, 0, g_input.inf.entryline);//设备类型
        setularea(wpagebuf+4, 1, g_input.inf.entryline);//设备类型
        setularea(wpagebuf+4, 2, 9);//设备类型
        setularea(wpagebuf+4, 3, Publics::bcd_to_val(Api::current_device_id));

        TimesEx::tm4_bcd7_exchange(wpagebuf+m_next_area_startoffset,CmdSort::m_time_now, false);//处理日 期

        setularea(wpagebuf+4, 5, 1); //写初始化状态

        setularea(wpagebuf+4, 9, m_current_count);

        nresult = writeul(m_next_startpage, 16, wpagebuf);

        if (nresult)
        {

            break;
        }

        nresult = 0;
        Beep_Sleep(1);


    }
    while (0);


    return nresult;

}

//储值票重编码接口
uint16_t es_svt_recode(uint8_t *lpinput, uint8_t *lpoutput)
{

    //unsigned char sztmp[200];

    int ret;

    do
    {
//        searchcard(sztmp);


//        svt_active(sztmp);
//  ////dbg_dumpmemory((char *)"Init_input = ", param_stack, 199);
        setvalue_issue(lpinput);
#ifdef _ENABLE_FIXKEY_
        memcpy(g_input.logicnumber, l_logicnumber, 8);
#endif


#ifndef _NEWTICKET_

        //删除结构，正式环境不用
        if (svt_reset() != 0)
        {
            ret = 0xF001;//ERR_CLEAR_FILE;
            break;
        }

        ret=svt_create_mf();
        if (ret!= 0)
        {
            ////dbg_formatvar((char *)"create_mf fail=%d",ret);
            ret = 0xF002;//ERR_CREATE_MF;
            break;
        }

        if (svt_create_adf1() != 0)
        {
            ret = 0xF003;//ERR_CREATE_ADF;
            //////dbg_formatvar((char *)"create_adf1 fail");
            break;
        }
#else
        //删除结构，正式环境不用
        if (svt_new_reset() != 0)
        {
            ret = 0xF001;//ERR_CLEAR_FILE;
            //break;
        }

        ret=svt_new_create_mf();
        if (ret!= 0)
        {
            ////dbg_formatvar((char *)"create_mf fail=%d",ret);
            ret = 0xF002;//ERR_CREATE_MF;
            break;
        }

        if (svt_new_create_adf1() != 0)
        {
            ret = 0xF003;//ERR_CREATE_ADF;
            //////dbg_formatvar((char *)"create_adf1 fail");
            break;
        }

#endif
        ret = 0;

    }
    while (0);

	return ret;
//     unsigned char ulen;
//     unsigned char respone_len;
//     unsigned char sztmp[256];
//     unsigned char szcmd[256];
//     unsigned char szdata[8];
//     unsigned char tmp_len;
//     unsigned char sam_len;
//     unsigned short respone_sw;
//     unsigned char inf[100];
//     uint16_t ret;
//  //dbg_formatvar("help");
//     do
//     {
//         memset((unsigned char *)&g_input.key, 0, sizeof(g_input.key));
//         setvalue_issue(lpinput);
//
// #ifdef _ENABLE_FIXKEY_
//         memcpy(g_input.key.adf1_mamk_02, l_key.adf1_mamk_02, 16);
// #endif
//
//         if (svt_selectfile(0x1001)<0)
//         {
//             ret = 4;//ERR_CARD_READ;
//             break;
//         };
//
//         if (svt_readrecord(0x17,2,0,inf)<0)
//         {
//             ret = 4;//ERR_CARD_READ;
//             break;
//         }
//         //////dbg_dumpmemory("complex file=",inf ,48);
//
//         if (apdu_getrandom(szdata,4)==0)
//         {
//
//             inf[4] = 0x09;
//
//
//             //////dbg_dumpmemory("random =",szdata,8);
//             memcpy(szcmd, "\x04\xDC\x02\xB8\x34", 5);
//             ulen = 5;
//             memcpy(szcmd+5, inf, 48);
//             ulen +=48;
//             MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);
//             ulen += 4;
//             //////dbg_dumpmemory("update complex =",szcmd,ulen);
//             respone_len = MifareProCom(ulen, szcmd, &respone_sw);
//             //////dbg_dumpmemory("update complex |=",szcmd,respone_len);
//             if ((respone_len==0) || (0x9000!=respone_sw))
//             {
//                 //nresult = -2;
//                 break;
//             }
//             Beep_Sleep(1);
//
//         }
//
//
//     }
//     while (0);

}

//单程票洗卡，只是内部调试用

uint16_t es_ul_clear(uint8_t *lpinput, uint8_t *lpoutput)
{
    uint8_t ulpage[64];
    //uint8_t sztmp[100];
    uint8_t npage;
    uint8_t i;
    //uint16_t ntmp;
    int nresult = -1;

    do
    {
		//setvalue_issue(lpinput);
        memset(ulpage, 0x00, sizeof(ulpage));
        for (i=0,npage=4;i<3;i++,npage+=4)
        {
            nresult = writeul_direct(npage, 16, ulpage+16);
            if (nresult)
            {
                break;
            }
        }
        Beep_Sleep(1);
    }
    while (0);


    return nresult;
}

//储值票洗卡，只是内部调试用

uint16_t es_svt_clear(uint8_t *lpinput, uint8_t *lpoutput)
{
    unsigned char utmp[200];

    int nresult;

    do
    {
// modify by shiyulong 2014-05-26, 洗卡前先进行寻卡，防止出错后要重新感应卡    
    	ISO14443A_Init(0);

        if (svt_active(utmp) != 0)
        {
            nresult = 9;
            break;
        }    
        nresult = 0;
// modify by shiyulong 2014-05-26, 完成    


        setvalue_issue(lpinput);

#ifdef _ENABLE_FIXKEY_
        memcpy(g_input.logicnumber, l_logicnumber, 8);
#endif

#ifndef _NEWTICKET_
        nresult = svt_reset();
        if ( nresult != 0)
        {
            Beep_Sleep(1);
        }

#else
        nresult = svt_new_reset();
        if ( nresult != 0)
        {
            Beep_Sleep(1);
        }
#endif
        Beep_Sleep(1);

    }
    while (0);
    //dbg_formatvar("clear22=%d",nresult);

    return nresult;

}


//==============================================
//单程票和储值票的业务实现结束
//==============================================




//==============================================
//单程票和储值票的业务外部调用接口
//==============================================
RETINFO esapi_analyse(uint8_t *lpoutput)
{
    int nresult=-1;
    RETINFO ret={0};
    uint8_t utmp[8];
    char physicalnumber[50]={0};
    PESANALYZE lpresult = (PESANALYZE)lpoutput;

    // 设置主天线为小天线工作模式
    change_antenna(0);
    ISO14443A_Init(1);

	nresult = searchcard_es(utmp);
// //dbg_formatvar("result=%d",nresult);

    switch (nresult)
    {
        case 0:
            nresult = ERR_CARD_NONE;
            ret.wErrCode = ERR_CARD_NONE;
            break;
        case 1:
            function_idx = 0;
            nresult = 0;
			sprintf(physicalnumber, "%02X%02X%02X%02X%02X%02X%02X%-6C",
				utmp[0], utmp[1], utmp[2], utmp[3], utmp[4], utmp[5], utmp[6], 0x20);
            break;
        case 3:
            function_idx = 1;
            nresult = 0;
			sprintf(physicalnumber, "%02X%02X%02X%02X%-12C",
				utmp[0], utmp[1], utmp[2], utmp[3], 0x20);
            break;
        case 99:
            nresult = ERR_CARD_MORE;
            ret.wErrCode = ERR_CARD_MORE;
            break;
        default:
            nresult = ERR_CARD_NONE;
            ret.wErrCode = ERR_CARD_NONE;
            break;
    }


    if (nresult==0)
    {
        memcpy(g_input.physical_id, utmp, 8);
        nresult = esfun_call[function_idx].esapi_analyse(lpoutput);

        //sprintf(physicalnumber, "%02X%02X%02X%02X%02X%02X%02X%-6C",
        //        utmp[0], utmp[1], utmp[2], utmp[3], utmp[4], utmp[5], utmp[6], 0x20);
        memcpy(lpresult->cPhysicalID, physicalnumber, 20);

        if (nresult)
        {
			if (nresult == 1)
			{
				//无卡
                ret.wErrCode = 0x02;

			}
			else if (nresult == 2)
			{
				//未发行
                ret.wErrCode = 0x00;
			}
			else
			{
	            if (function_idx)
	            {
	                ret.wErrCode = 0x0211;
	            }
	            else
	            {
	                ret.wErrCode = 0x0210;
	            }

	            ret.bNoticeCode = nresult;
			}
        }

    }

    return ret;

}


//车票初始化
RETINFO esapi_init(uint8_t *lpinput, uint8_t *lpoutput)
{
    int nresult;
    RETINFO ret={0};

    nresult = esfun_call[function_idx].esapi_init(lpinput,lpoutput);
    if (nresult)
    {
        if (function_idx)
        {
            ret.wErrCode = 0x0221;
        }
        else
        {
            ret.wErrCode = 0x0220;
        }
        ret.bNoticeCode = nresult;

    }
    return (ret);
}

//车票预付值
RETINFO esapi_evaluate(uint8_t *lpinput, uint8_t *lpoutput)
{
    int nresult;
    RETINFO ret={0};

    nresult = esfun_call[function_idx].esapi_evaluate(lpinput,lpoutput);
    if (nresult)
    {
        if (function_idx)
        {
            ret.wErrCode = 0x0231;
        }
        else
        {
            ret.wErrCode = 0x0230;
        }
        ret.bNoticeCode = nresult;

    }

    return (ret);
}

//车票注销
RETINFO esapi_destroy(uint8_t *lpinput, uint8_t *lpoutput)
{
    int nresult;
    RETINFO ret={0};

    nresult = esfun_call[function_idx].esapi_destroy(lpinput,lpoutput);
    if (nresult)
    {
        if (function_idx)
        {
            ret.wErrCode = 0x0241;
        }
        else
        {
            ret.wErrCode = 0x0240;
        }
        ret.bNoticeCode = nresult;

    }

    return (ret);
}

//车票重编码
RETINFO esapi_recode(uint8_t *lpinput, uint8_t *lpoutput)
{
    int nresult;
    RETINFO ret={0};

    nresult = esfun_call[function_idx].esapi_recode(lpinput,lpoutput);
    if (nresult)
    {
        if (function_idx)
        {
            ret.wErrCode = 0x0251;
        }
        else
        {
            ret.wErrCode = 0x0250;
        }
        ret.bNoticeCode = nresult;

    }

    return (ret);
}

//车票洗卡
RETINFO esapi_clear(uint8_t *lpinput, uint8_t *lpoutput)
{

#ifdef _ENABLE_CLEAR_
    int nresult;
    RETINFO ret={0};

    nresult = esfun_call[function_idx].esapi_clear(lpinput,lpoutput);
    //dbg_formatvar("nresult_esapi_clear=%d", nresult);
    if (nresult)
    {
        ret.wErrCode = 0x0271;
        ret.bNoticeCode = nresult;

    }

    return (ret);
#else
    ret.wErrCode = 5;
    return ret;
#endif

}


//车票个人化信息录入,  只能应用于储值票
RETINFO esapi_private(uint8_t *lpinput, uint8_t *lpoutput)
{
    unsigned char ulen;
    unsigned char respone_len;
    //unsigned char sztmp[256];
    unsigned char szcmd[256];
    unsigned char szdata[8];
    //unsigned char tmp_len;
    //unsigned char sam_len;
    unsigned short respone_sw;
    unsigned char inf[100];
    int nresult=-1;
    RETINFO ret={0};


    do
    {

        memset(inf, 0x00, sizeof(inf));
        memcpy(inf, lpinput+16, 56);

#ifdef _ENABLE_FIXKEY_
        memcpy(g_input.key.adf1_mamk_02, l_key.adf1_mamk_02, 16);
#else
        memcpy(g_input.key.adf1_mamk_02, lpinput, 16);

#endif

        if (svt_selectfile(0x1001) < 0)
        {
            memcpy(szdata, "\xA0\x00\x00\x00\x03\x86\x98\x07\x01", 9);
            if (svt_selecfileaid(9,szdata) < 0)
            {
                nresult = 1;//ERR_CARD_READ;
                break;
            }
        }

		//dbg_dumpmemory("key02=",g_input.key.adf1_mamk_02,16);


        //更新0016创建持卡人基本信息
        if (apdu_getrandom(szdata,4)==0)
        {

            memset(szcmd, 0x00, sizeof(szcmd));
            memcpy(szcmd, "\x04\xD6\x96\x00\x3C",5);
            ulen = 5;
            memcpy(szcmd+5, inf, 56);
            ulen += 56; //0x38  //数据为全 00
            MAC_3(g_input.key.adf1_mamk_02, ulen, szdata, szcmd, szcmd+ulen);
            ulen += 4;
            //dbg_dumpmemory("update adf1 file_0016 =",szcmd,ulen);
            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
            //dbg_dumpmemory("update adf1 file_0016 |=",szcmd,respone_len);
            //dbg_formatvar("update adf1 file_0016 |= %04X",respone_sw);

            if ((respone_len==0) || (0x9000!=respone_sw))
            {
                nresult = 0x02 ;
                break;
            }
            nresult = 0;
            Beep_Sleep(1);

        }

    }
    while (0);

    if (nresult)
    {
        ret.wErrCode = 0x0261;
        ret.bNoticeCode = nresult;
    }

    return ret;

}

unsigned char es_get_issue_status(unsigned char globle_status)
{
	unsigned char ret = 0;

	if (globle_status == 3)		// 注销
	{
		ret = 2;
	}
	else						// 其他状态
	{
		ret = 1;
	}

	return ret;
}

unsigned char es_standard_status(unsigned char globle_status, unsigned char metro_status)
{
	unsigned char ret = 0;

	if (globle_status == 3)		// 注销
	{
		ret = USTATUS_DETROY;
	}
	else if (globle_status == 2)
	{
		ret = USTATUS_REFUND;
	}
	else
	{
		ret = unified_status(metro_status);
	}

	return ret;

}
