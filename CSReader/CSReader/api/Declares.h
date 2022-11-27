// Declares.h

#pragma once
#include <stdio.h>


// 常用宏
#define SIZE_THREAD_STACK		4096
#define UTC_TIME_START			"\x19\x70\x01\x01\x00\x00\x00"

#define NAME_PRM_CONFIG			(char *)"PrmConfig.dat"
#define NAME_IPRM_CONFIG		(char *)"IPrmConfig.dat"
#define NAME_TRADE_FOLDER		(char *)"/data/Trade/"
#define NAME_LOG_FOLDER			(char *)"/data/Log/"
#define NAME_PRM_FOLDER			(char *)"ParamFolder"
#define NAME_IPRM_FOLDER		(char *)"IParamFolder"
#define NAME_CONFIRM_FILE		(char *)"Confirm.his"
#define NAME_TEMP_FOLDER		(char *)"Tmp"
#define NAME_TEMP_APP			(char *)"CSReader"
#define NAME_SCRIPT				(char *)"api_script.sh"
#define NAME_LOG                (char *)".log"

// SAM卡类型
#define SAM_TYPE_PSAM				(uint8_t)0x00
#define SAM_TYPE_ISAM				(uint8_t)0x01
#define SAM_TYPE_CSAM				(uint8_t)0x02
#define SAM_TYPE_MSAM				(uint8_t)0x03
#define SAM_TYPE_HSAM				(uint8_t)0x04

// 波特率
#define BAUD_ID_9600				(uint8_t)0x00
#define BAUD_ID_19200				(uint8_t)0x01
#define BAUD_ID_38400				(uint8_t)0x02
#define BAUD_ID_57600				(uint8_t)0x03
#define BAUD_ID_115200				(uint8_t)0x04

// BOM、TVM分析票卡时的可操作状态
#define ALLOW_CHARGE				0x01				// 是否可充(值/乘次)
#define ALLOW_UPDATE				0x01<<1				// 是否可更新
#define ALLOW_SALE					0x01<<2				// 是否可发售
#define ALLOW_ACTIVE				0x01<<3				// 是否可激活
#define ALLOW_DIFFER				0x01<<4				// 是否可延期
#define ALLOW_REFUND				0x01<<5				// 是否可退款
#define ALLOW_UNLOCK				0x01<<6				// 是否可解锁
#define ALLOW_REFUND_DE				0x01<<7				// 是否可退押金

// 票卡标准状态
#define USTATUS_INIT				(uint8_t)200			 // 初始化
#define USTATUS_ES					(uint8_t)201			 // Init II (Pre–value loaded @E/S)(ES预赋值)
#define USTATUS_SALE				(uint8_t)202			 // BOM/TVM发售
#define USTATUS_EXIT				(uint8_t)203			 // 出站(exit)
#define USTATUS_EXIT_T				(uint8_t)204			 // 列车故障模式出站(exit during Train–disruption)
#define USTATUS_UPD_OUT				(uint8_t)205			 // 进站BOM更新(upgrade at BOM for Entry)
#define USTATUS_UPD_OUT_FREE		(uint8_t)206			 // 非付费区免费更新（BOM/pca 非付费区）
#define USTATUS_UPD_OUT_FARE		(uint8_t)207			 // 非付费区付费更新（BOM/pca 非付费区）
#define USTATUS_ENTRY				(uint8_t)208			 // 进站(entry)
#define USTATUS_UPD_IN				(uint8_t)209			 // 出站BOM更新(upgrade at BOM for Exit)
#define USTATUS_UPD_WO_STA			(uint8_t)210			 // 无进站码更新（BOM/pca 付费区）
#define USTATUS_UPD_TM_OUT			(uint8_t)211			 // 超时更新（BOM/pca 付费区）
#define USTATUS_UPD_TP_OUT			(uint8_t)212			 // 超乘更新（BOM/pca 付费区）
#define USTATUS_EXIT_ONLY			(uint8_t)213			 // ET for Exit(出站票)
#define USTATUS_REFUND				(uint8_t)214			 // 退卡
#define USTATUS_DETROY				(uint8_t)215			 // 车票注销
#define USTATUS_UNDEFINED			(uint8_t)255			 // 未知状态

// 票卡生命周期状态
#define LIFE_INIT					(uint8_t)100			 // 初始化
#define LIFE_ES						(uint8_t)101			 // Init II (Pre–value loaded @E/S)(ES预赋值)
#define LIFE_SALE					(uint8_t)102			 // BOM/TVM发售
#define LIFE_EXIT					(uint8_t)103			 // 出站(exit)
#define LIFE_EXIT_T					(uint8_t)104			 // 列车故障模式出站(exit during Train–disruption)
#define LIFE_UPD_OUT				(uint8_t)105			 // 进站BOM更新(upgrade at BOM for Entry)
#define LIFE_UPD_OUT_FREE			(uint8_t)106			 // 非付费区免费更新（BOM/pca 非付费区）
#define LIFE_UPD_OUT_FARE			(uint8_t)107			 // 非付费区付费更新（BOM/pca 非付费区）
#define LIFE_ENTRY					(uint8_t)108			 // 进站(entry)
#define LIFE_UPD_IN					(uint8_t)109			 // 出站BOM更新(upgrade at BOM for Exit)
#define LIFE_UPD_WO_STA				(uint8_t)110			 // 无进站码更新（BOM/pca 付费区）
#define LIFE_UPD_TM_OUT				(uint8_t)111			 // 超时更新（BOM/pca 付费区）
#define LIFE_UPD_TP_OUT				(uint8_t)112			 // 超乘更新（BOM/pca 付费区）
#define LIFE_EXIT_ONLY				(uint8_t)113			 // ET for Exit(出站票)
#define LIFE_REFUND					(uint8_t)114			 // 退卡
#define LIFE_DETROY					(uint8_t)115			 // 车票注销
#define LIFE_DEFFER					(uint8_t)116			 // 延期
#define LIFE_CHARGE					(uint8_t)117			 // 充值
#define LIFE_ACTIVE					(uint8_t)118			 // 激活
#define LIFE_DECREASE				(uint8_t)119			 // 减值
#define LIFE_LOCKED					(uint8_t)120			 // 加锁
#define LIFE_UNLOCKED				(uint8_t)121			 // 解锁
#define LIFE_EXIT_TK_EXIT			(uint8_t)122			 // 出站票出站
#define LIFE_TCT_ENTRY_ACTIVE		(uint8_t)123			 // 次票进站激活
#define LIFE_CONSUME_COMMON			(uint8_t)180			 // 普通消费
#define LIFE_CONSUME_COMPOUND		(uint8_t)181			 // 复合消费
#define LIFE_CONFIRM				(uint8_t)198			 // 交易确认成功
#define LIFE_CONFIRM_RM				(uint8_t)199			 // 解除交易确认
#define LIFE_UNDEFINED				(uint8_t)255			 // 未定义操作

// 地铁CPU卡文件长度
#define LENM_ISSUE_BASE				0x28
#define LENM_PUBLIC_BASE			0x1E
#define LENM_OWNER_BASE				0x38
#define LENM_WALLET					0x04
#define LENM_HIS_ALL				0x02B2					// 本地消费，异地消费，充值0x17 * 0x0A * 3
#define LENM_TRADE_ASSIST			0x30
#define LENM_METRO					0x30
#define LENM_CTRL_RECORD			0x18
#define LENM_APP_CTRL				0x20
#define LENM_CHANGE_CNT				0x02
#define LENM_CONSUME_CNT			0x02
#define LENM_JTBOWNER_BASE          0x37
#define LENM_JTBMETRO               0x80
#define LENM_JTBMANAGE              0x3C

#define LENM_ETICKETBASE            0x244
#define LENM_ETICKETENTRY           0x244
#define LENM_ETICKETEXIT            0x244
#define LENM_ETICKETGET             0x244


// 公交CPU卡文件长度
#define LENB_ISSUE_BASE				0x30
#define LENB_APP_INDEX				0x10
#define LENB_PUBLIC_BASE			0x1E
#define LENB_OWNER_BASE				0x37
#define LENB_WALLET					0x04
#define LENB_HIS_ALL				0xE6
#define LENB_METRO					0x20
#define LENB_CHANGE_CNT				0x02
#define LENB_CONSUME_CNT			0x02

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 常用BAE_标记类型定义
// 地铁设备类型
typedef enum
{
    dvcLCC			= 0,
    dvcSC			= 1,
    dvcTVM			= 2,
    dvcBOM			= 3,
    dvcEntryAGM		= 4,
    dvcExitAGM		= 5,
    dvcBidirectAGM	= 6,
    dvcTCM			= 7,
    dvcPCA			= 8,
    dvcES			= 9,
    dvcICCS			= 99
} ETPDVC;

// 票卡操作类型
typedef enum
{
	operNone,
    operAnalyse,
    operUpdate,
	operSale,		// 包含operSvtSale和operSjtSale
    operSvtSale,
    operSjtSale,
    operSjtClear,
    operDecr,
    operIncr,
    operDeffer,
    operRefund,
    operLock,
    operEntry,
    operExit,
    operTkInf,
	operGetETicket,
	operThirdGetETicket,
	operGetETicketRecord,
	operUnloadTicketInfo
} ETYTKOPER;

// 蓝牙操作类型
typedef enum
{
	operScan,
	operConn,
	operDataEx,
	operWriteBack,
	operClose,
	operSetBaud,
	operGetHandle,
	operScanStop,
	operSetSub,
	operBleReset,
	operBleDownloadReset,
	operBleGetVersion,
	operBleTest,
	operBleTestOther
} ETBLEOPER;

typedef enum
{
	mediNone		= 0,
	mediUltraLight	= 0x01,
	mediBusMifare	= 0x02,
	mediMetroCpu	= 0x03,
	mediBusCpu		= 0x04,
	mediBusMfEX		= 0x05,
	mediMetroCpuTct = 0x06,
	mediJtbCpu      = 0x07,
	mediElecULTictet  = 0x08,
	mediElecSJTTictet  = 0x09,
	mediElecPostPayTictet  = 0x10,
	mediThirdPay  = 0x11
} ETPMDM;

// 精简票卡状态，所有的状态都能对应成精简状态，便于流程统一
typedef enum
{
	SStatus_Exit = 1,
	SStatus_Exit_Tt = 2,
	SStatus_Exit_Only = 3,
	SStatus_Upd_FZone = 4,//非付费区更新
	SStatus_Entry = 5,
	SStatus_Upd_WZone = 6,//付费区更新
	SStatus_Init = 7,
	SStatus_Sale = 8,
	SStatus_Es = 9,
	SStatus_Refund = 10,
	SStatus_Invalid = 11
} SimpleStatus;
