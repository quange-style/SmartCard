//票卡相关数据
// Created by deken on 2020/3/16.
//
#pragma once
#pragma pack(push, 1)

#include <stdint.h>

//进站信息
typedef struct
{
    char		cTxnCode[2];					// 交易类型

    char		cInTime[14];					// 进站日期时间如;20060211160903
    char		cLineCode[2];					// 线路编号
    char		cStationCode[2];				// 站点编号
    char		cDevType[2];					// 设备类型
    char		cDeviceID[3];					// 设备代码(BCD)，第0字节仅低4bit有效
    char		cSAMID[16];						// SAM卡逻辑卡号
    long		lSAMTrSeqNo;					// SAM卡脱机交易流水号
    char		cStatus[2];						// 状态码
    char		cVerifyCode[8];					// 验证码

}ETICKET_ENTRY_INF, * P_ETICKET_ENTRY_INF;

//出站信息
typedef struct
{
    char		cTxnCode[2];					// 交易类型

    char		cInTime[14];					// 出站日期时间如;20060211160903
    char		cLineCode[2];					// 线路编号
    char		cStationCode[2];				// 站点编号
    char		cDevType[2];					// 设备类型
    char		cDeviceID[3];					// 设备代码(BCD)，第0字节仅低4bit有效
    char		cSAMID[16];						// SAM卡逻辑卡号
    long		lSAMTrSeqNo;					// SAM卡脱机交易流水号
    long		lDebitAmount;					// 实扣金额
    char		cStatus[2];						// 状态码
    char		cVerifyCode[8];					// 验证码

}ETICKET_EXIT_INF, * P_ETICKET_EXIT_INF;

//取票信息
typedef struct
{
    char		cTxnCode[2];					// 交易类型

    char		cInTime[14];					// 出票日期时间如;20060211160903
    char		cLineCode[2];					// 线路编号
    char		cStationCode[2];				// 站点编号
    char		cDevType[2];					// 设备类型
    char		cDeviceID[3];					// 设备代码(BCD)，第0字节仅低4bit有效
    char		cSAMID[16];						// SAM卡逻辑卡号
    long		lSAMTrSeqNo;					// SAM卡脱机交易流水号
    char		cCardNo[16];					// 逻辑卡号
    char		cStatus[2];						// 状态码
    char		cVerifyCode[8];					// 验证码

}ETICKET_GET_INF, * P_ETICKET_GET_INF;

// 地铁CPU卡相关结构
typedef struct
{
    uint8_t * p_issue_base;			// 发行基本应用
    uint8_t * p_public_base;		// 公共应用基本数据文件
    uint8_t * p_owner_base;			// 持卡人基本数据文件
    uint8_t * p_manage_info;		// 管理信息文件-判断是否互联互通票卡
    uint8_t * p_wallet;				// 钱包专用
    uint8_t * p_his_all;			// 所有历史记录
    uint8_t * p_trade_assist;		// 交易辅助（符合记录文件）
    uint8_t * p_metro;				// 轨道交通（符合记录文件）
    uint8_t * p_ctrl_record;		// 应用辅助，即应用控制记录（符合记录文件）
    uint8_t * p_app_ctrl;			// 应用控制文件
    uint8_t * p_charge_count;		// 联机交易计数
    uint8_t * p_consume_count;		// 脱机交易计数

    uint8_t *  p_eTicket_base_info;			    // 票卡信息
    uint8_t *  p_eTicket_entry_info;			// 进站信息
    uint8_t *  p_eTicket_exit_info;				// 出站信息
    uint8_t *  p_eTicket_get_info;			    // 取票信息

    /*
    P_ETICKET_ENTRY_INF p_eTicket_entry_info;			// 进站信息
    P_ETICKET_EXIT_INF  p_eTicket_exit_info;			// 出站信息
    P_ETICKET_GET_INF  p_eTicket_get_info;			    // 取票信息
    */

}METRO_ELECT_INF, * P_METRO_ELECT_INF;

#pragma pack(pop)


