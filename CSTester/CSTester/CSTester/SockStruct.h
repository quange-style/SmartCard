//SockStruct.h

#pragma once

#pragma pack(push, 1)

typedef struct
{
	char	cMsgType[2];				// 消息类型	0	2	Char	取值’65’	中心-> 终端
	BYTE	dtTime[7];					// 消息生成时间	2	7	BCD	YYYYMMDD HH24MISS	
	long	lServerSeq;					// 系统参照号	9	4	LONG	后台处理流水号，作回执编号	
	char	cPosId[9];					// 终端编号	13	9	CHAR	线路代码(2)+车站代码(2)+设备类型(2)+设备编码(3)	
	char	cSamId[16];					// Sam卡逻辑卡号	22	16	CHAR	psam逻辑卡号	
	char	cMac[16];					// MAC	38	16	CHAR		
	char	cAck[2];					// 响应码	54	2	CHAR	00：正常；01:不符合业务逻辑或系统处理过程中出现异常，详细原因见错误描述	
	char	cDiscrib[2];				// 描述	56	2	CHAR		

}SAM_ACTIVE_ACK;

typedef struct
{
	char	cMsgType[2];				// 消息类型	0	2	Char	取值’61’	中心-> 终端
	BYTE	dtTime[7];					// 消息生成时间	2	7	BCD	YYYYMMDD HH24MISS	
	long	lServerSeq;					// 系统参照号	9	4	LONG	后台处理流水号	
	char	cPosId[9];					// 终端编号	13	9	CHAR	线路代码(2)+车站代码(2)+设备类型(2)+设备编码(3)	
	char	cSamId[16];					// Sam卡逻辑卡号	22	16	CHAR	psam逻辑卡号	
	long	lPosSeq;					// 易序列号	38	4	LONG	充值申请51传入	
	char	cLogicalID[20];				// 票卡逻辑卡号	42	20	CHAR		
	char	cMac[8];					// MAC2	62	8	CHAR	由中心生成的认证码，成功时有效 不成功时全0	
	BYTE	dtServer[7];				// 系统时间	70	7	BCD	由中心返回 充值设备依次作为票卡的充值时间，成功时有效 不成功时全0	
	char	cAck[2];					// 响应码	77	2	CHAR	00：正常；01:不符合业务逻辑或系统处理过程中出现异常，详细原因见错误描述	
	char	cDiscrib[2];				// 错误描述	79	2	CHAR		

}CHARGE_APP_ACK;

typedef struct
{
	char 		cMsgType[2];					// 消息类型“52”
	uint8_t		dtMsgTime[7];					// 消息生成时间
	char		cStation [4];					// 站点
	char		cDevType[2];					// 设备类型
	char		cDevId[3];						// 设备编码
	char		cSamId[16];						// SAM卡号
	long		lPosSeq;						// 终端交易序列号
	char		cNetPoint[16];					// 网点编码
	char		cIssueCodeMain[4];				// 发行商主编码
	char 		cIssueCodeSub[4];				// 发行商子编码
	uint8_t		bTicketType[2];					// 票卡类型
	char		cLogicalId[20];					// 票卡逻辑卡号
	char 		cPhysicalId[20];				// 票卡物理卡号
	char		cTestFlag;						// 票卡引用标识
	long		lChargeCount;					// 联机交易流水
	long		lTradeCount;					// 脱机交易流水
	char		cBusinessType[2];				// 业务类型“14”
	char		cWalletType;					// 值类型
	long		lAmount;						// 充值金额
	long		lBalcance;						// 余额
	char		cTac[8];						// 钱包交易TAC码
	char		cResult;						// 写卡结果
	char		cOperatorId[6];					// 操作员编码
	long		lServerSeq;						// 系统参照号，61返回
	uint8_t		dtTimeOper[7];					// 消息61返回的充值时间

}CHARGE_CONFIRM;


typedef struct
{
	char 		cMsgType[2];					// 消息类型“62”
	uint8_t		dtMsgTime[7];					// 消息生成时间
	long		lServerSeq;						// 系统参照号
	char		cPosId[9];						// 终端编号	13	9	CHAR	线路代码(2)+车站代码(2)+设备类型(2)+设备编码(3)	
	char		cSamId[16];						// SAM卡号
	long		lPosSeq;						// 终端交易序列号
	char		cLogicalId[20];					// 票卡逻辑卡号
	char		cAck[2];						// 响应码	77	2	CHAR	00：正常；01:不符合业务逻辑或系统处理过程中出现异常，详细原因见错误描述	
	char		cDiscrib[2];					// 错误描述	79	2	CHAR		

}CHARGE_CONFIRM_ACK;

typedef struct
{
	char	cMsgType[2];				// 消息类型	0	2	Char	取值’63’	中心-> 终端
	BYTE	dtTime[7];					// 消息生成时间	2	7	BCD	YYYYMMDD HH24MISS	
	long	lServerSeq;					// 系统参照号	9	4	LONG	后台处理流水号	
	char	cPosId[9];					// 终端编号	13	9	CHAR	线路代码(2)+车站代码(2)+设备类型(2)+设备编码(3)	
	char	cSamId[16];					// Sam卡逻辑卡号	22	16	CHAR	psam逻辑卡号	
	long	lPosSeq;					// 易序列号	38	4	LONG	充值申请51传入	
	char	cLogicalID[20];				// 票卡逻辑卡号	42	20	CHAR		
	BYTE	dtServer[7];				// 系统时间	70	7	BCD	由中心返回 充值设备依次作为票卡的充值时间，成功时有效 不成功时全0	
	char	cAck[2];					// 响应码	77	2	CHAR	00：正常；01:不符合业务逻辑或系统处理过程中出现异常，详细原因见错误描述	
	char	cDiscrib[2];				// 错误描述	79	2	CHAR		

}DESCIND_APP_ACK;


#pragma pack(pop)


typedef struct _CMD_INFO
{
	BYTE	cmdMain;
	BYTE	cmdSub;
	WORD	lenSend;
	BYTE	bSend[1024];
	WORD	lenRecv;
	BYTE	bRecv[1024];

}CMDINF, *PCMDINF;
