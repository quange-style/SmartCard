// Structs.h

#pragma once

#include "Declares.h"
#pragma pack(push, 1)

// 2.21	错误提示信息结构
typedef struct {
	uint16_t	wErrCode;						// 错误码
	uint8_t		bNoticeCode;					// 关联提示码
	uint8_t		bRfu[2];						// 扩充保留字段
}RETINFO, * PRETINFO;

// 2.3	SAM卡信息查询
typedef struct {
	uint8_t		bSAMStatus;						// SAM卡状态–(0x00表示正常，其他-后续字段无效)
	char		cSAMID[16];						// SAM卡ID号
}SAMSTATUS, * PSAMSTATUS;

// 4.1	读写器初始化信息
typedef struct {
	uint8_t		bReaderReadyStatus;				// 读写器就绪状态
	SAMSTATUS  SamStatus[4];					// 各SAM卡状态信息，暂定4卡槽，参见// 2.3
}READERSTATUS, * PREADERSTATUS;

// 4.4	入闸交易
typedef struct {
	char		cTradeType[2];					// 交易类型
	char		cStationID[4];					// 线路站点代码
	char		cDevType[2];					// 设备类型
	char		cDeviceID[3];					// 设备代码(BCD)，第0字节仅低4bit有效
	char		cSAMID[16];						// SAM卡逻辑卡号
	long		lSAMTrSeqNo;					// SAM卡脱机交易流水号
	uint8_t		bTicketType[2];					// 票卡类型
	char		cLogicalID[20];					// 票卡逻辑卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	uint8_t		dtDate[7];						// 进站日期时间如;20060211160903
	uint8_t		bStatus;						// 交易状态代码
	long		lBalance;						// 余额
	char		cTestFlag;						// 卡应用标识
	char		cLimitMode[3];					// 乘次票限制模式
	uint8_t		bLimitEntryID[2];				// 限制进站站点
	uint8_t		bLimitExitID[2];				// 限制出站站点
	char		cEntryMode;						// 进闸工作模式
	long		lTradeCount;					// 票卡扣款交易计数
	char		cTkAppMode;						//卡应用模式 ， '1'：CPU， '2'：M1/UL, '3'：仿M1
}ENTRYGATE, * PENTRYGATE;

// 4.5	钱包交易
typedef struct {
	char		cTradeType[2];					// 交易类型
	char		cStationID[4];					// 线路站点代码
	char		cDevType[2];					// 设备类型
	char		cDeviceID[3];					// 设备代码(BCD)，第0字节仅低4bit有效
	char		cSAMID[16];						// 本次交易SAM逻辑卡号
	long		lSAMTrSeqNo;					// 本次交易SAM卡脱机交易流水号
	uint8_t		dtDate[7];						// 日期时间(BCD)
	uint8_t		bTicketType[2];					// 票卡类型(BCD)
	char		cLogicalID[20];					// 票卡逻辑卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	uint8_t		bStatus;						// 交易状态代码	(BCD)
	long		lTradeAmount;					// 交易金额, 单位为分
	long		lBalance;						// 余额, 单位为分
	long		lChargeCount;					// 票卡充值计数
	long		lTradeCount;					// 票卡扣款交易计数
	char		cPaymentType[2];				// 支付类型
	char		cReceiptID[4];					// 支付凭证码
	char		cMACorTAC[10];					// 交易认证码
	uint8_t		bEntryStationID[2];				// 入口站点代码(BCD)
	char		cEntrySAMID[16];				// 入口SAM逻辑卡号
	uint8_t		dtEntryDate[7];					// 进站日期时间(BCD)
	char		cOperatorID[6];					// 操作员代码(Login)
	uint8_t		bBOMShiftID;					// BOM班次序号
	char		cSamLast[16];					// 上次交易的SAM卡号
	uint8_t		dtLast[7];						// 上次交易日期时间
	long		lTradeWallet;					// 钱包交易额，卡片实际需要变动的钱包值
	char		cTestFlag;						// 卡应用标识
	char		cLimitMode[3];					// 乘次票限制模式
	uint8_t		bLimitEntryID[2];				// 限制进站站点
	uint8_t		bLimitExitID[2];				// 限制出站站点
	char		cExitMode;						// 出闸工作模式
	char		cCityCode[4];					// 城市代码
	char		cIndustryCode[4];				// 行业代码
	char		cClassicType[2];				// 交易分类
	char		cSamPosId[12];					// SAM卡终端编码
	char		cTkAppMode;						// 卡应用模式 ， '1'：CPU， '2'：M1/UL, '3'：仿M1
	char        cRecordVer[2];                  // 记录版本，暂时默认为11
	char		cIssueCardCode[16];             // 发卡机构代码
	char		cKeyVer[2];                     // 密钥版本号
	char		cKeyIndex[2];                   // 密钥索引
	char		cRandom[8];                     // 伪随机数
	char		cAlgFlag[2];                    // 算法标识
	char		cCardHolderName[40];			// 持卡人姓名
	char		cCertificateType[2];			// 证件类型
	char		cCertificateCode[30];			// 证件号码
	char		cOriginalCardType[8];			// 原来票卡类型
	char		cCardLocation[4];               // 卡属地
	uint8_t     cPayType;				        // 支付方式，如现金、微信支付宝等

}PURSETRADE, * PPURSETRADE;

//4.6	非单程票发售记录
typedef struct {
	char		cTradeType[2];					// 交易类型
	char		cStationID[4];					// 线路站点代码
	char		cDevType[2];					// 设备类型
	char		cDeviceID[3];					// 设备代码(BCD)，第0字节仅低4bit有效
	uint8_t		bTicketType[2];					// 票卡类型
	char		cLogicalID[20];					// 票卡逻辑卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	uint8_t		bStatus;						// 交易状态代码
	char		cBusinessseqno[12];				// 脱机业务流水号
	char		cSAMID[16];						// SAM卡逻辑卡号（单程票）
	long		lSAMTrSeqNo;					// SAM卡脱机交易流水号（单程票）
	uint8_t		bAmountType;					// 金额类型
	short		nAmount;						// 押金
	uint8_t		dtDate[7];						// 销售时间BCD
	char		cReceiptID[4];					// 支付凭证码
	char		cOperatorID[6];					// 操作员代码(Login)
	uint8_t		bBOMShfitID;					// BOM班次序号每个BOM每天重置	(BCD)
	long		lBrokerage;						// 手续费
	char		cTestFlag;						// 卡应用标识
}OTHERSALE, * POTHERSALE;

// 4.7	单程票发售记录
typedef struct {
	char		cTradeType[2];					// 交易类型
	char		cStationID[4];					// 线路站点代码
	char		cDevType[2];					// 设备类型
	char		cDeviceID[3];					// 设备代码(BCD)，第0字节仅低4bit有效
	char		cSAMID[16];						// SAM卡逻辑卡号（单程票）
	long		lSAMTrSeqNo;					// SAM卡脱机交易流水号（单程票）
	uint8_t		dtDate[7];						// 时间
	uint8_t		bPaymentMeans;					// 支付方式
	char		cPaymentTKLogicalID[20];		// 支付票卡逻辑卡号
	long		lTradeCount;					// 单程票扣款计数
	char		cLogicalID[20];					// TOKEN 逻辑ID
	char		cPhysicalID[20];				// TOKEN 物理ID
	uint8_t		bStatus;						// TOKEN状态
	short		nChargeValue;					// 充值金额
	uint8_t		bTicketType[2];					// 单程票卡类型
	uint8_t		bZoneID;						// 区段代码
	char		cMACorTAC[10];					// 交易认证码
	uint8_t		bDepositorCost;					// 成本押金(BCD)
	short		nAmountCost;					// 成本押金金额
	char		cOperatorID[6];					// 操作员代码(Login)
	uint8_t		bBOMShiftID;					// BOM班次序号, 每个BOM每天重置
	long		lBrokerage;						// 手续费
	char		cTestFlag;						// 卡应用标识
	char		cClassicType[2];				// 交易分类
	char		cSamPosId[12];					// SAM卡终端编码
}SJTSALE, * PSJTSALE;

// 4.8	交易确认信息结构
typedef struct {
	uint8_t	bTradeType;							// 交易类型
	uint8_t	bData[512];							// 上次的交易记录结构的拷贝
}TRANSACTCONFIRM, * PTRANSACTCONFIRM;

// 4.9	更新交易
typedef struct {
	char		cTradeType[2];					// 交易类型
	char		cStationID[4];					// 线路站点代码
	char		cDevType[2];					// 设备类型
	char		cDeviceID[3];					// 设备代码(BCD)，第0字节仅低4bit有效
	char		cSAMID[16];						// SAM卡逻辑卡号
	long		lSAMTrSeqNo;					// 更新设备SAM卡脱机交易流水号
	uint8_t		bTicketType[2];					// 票卡类型
	char		cLogicalID[20];					// 票卡逻辑卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	long		lTradeCount;					// 票卡扣款交易计数
	uint8_t		bStatus;						// 交易状态代码
	char		cUpdateZone;					// 更新区域
	uint8_t		bUpdateReasonCode;				// 更新原因
												// 更新原因代码表
												// 原因代码	描述（Description）
												// 付费区：01－出站超时，02-超乘，03-无进站码
												// 非付费区：10－有进站码，12－进站超时
	uint8_t		dtUpdateDate[7];				// 更新日期时间YYYYMMDDHHMMSS
	uint8_t		bPaymentMode;					// 罚金支付方式
												// 	1-现金，2-储值卡， 默认值-0
	short		nForfeiture;					// 罚金, 需缴纳的罚款金额（现金支付），单位分
	char		cReceiptID[4];					// 支付凭证码
	char		cOperatorID[6];					// 操作员代码(Login)
	uint8_t		bEntryStationID[2];				// 进站线路站点代码
	uint8_t		bBOMShfitID;					// BOM班次序号每个BOM每天重置	（BCD）
	char		cTestFlag;						// 卡应用标识
	char		cLimitMode[3];					// 乘次票限制模式
	uint8_t		bLimitEntryID[2];				// 限制进站站点
	uint8_t		bLimitExitID[2];				// 限制出站站点
	char		cTkAppMode;						//卡应用模式 ， '1'：CPU， '2'：M1/UL, '3'：仿M1
}TICKETUPDATE, * PTICKETUPDATE;

// 4.10	票卡延期交易
typedef struct {
	char		cTradeType[2];					// 交易类型
	char		cStationID[4];					// 线路站点代码
	char		cDevType[2];					// 设备类型
	char		cDeviceID[3];					// 设备代码(BCD)，第0字节仅低4bit有效
	char		cSAMID[16];						// SAM卡逻辑卡号
	long		lSAMTrSeqNo;					// SAM卡脱机交易流水号
	uint8_t		bTicketType[2];					// 票卡类型
	char		cLogicalID[20];					// 票卡逻辑卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	uint8_t		bStatus;						// 交易状态代码
	uint8_t		dtOldExpiryDate[7];				// 原有效时间
	uint8_t		dtNewExpiryDate[7];				// 现有效时间
	uint8_t		dtOperateDate[7];				// 操作时间
	char		cOperatorID[6];					// 操作员代码(Login)
	uint8_t		bBOMShfitID;					// BOM班次序号每个BOM每天重置	（BCD）
	char		cTestFlag;						// 卡应用标识
}TICKETDEFER, * PTICKETDEFER;

// 4.11	票卡加解锁交易
typedef struct {
	char		cTradeType[2];					// 交易类型
	char		cStationID[4];					// 线路站点代码
	char		cDevType[2];					// 设备类型
	char		cDeviceID[3];					// 设备代码(BCD)，第0字节仅低4bit有效
	char		cSAMID[16];						// SAM卡逻辑卡号
	long		lSAMTrSeqNo;					// SAM卡脱机交易流水号
	uint8_t		bTicketType[2];					// 票卡类型
	char		cLogicalID[20];					// 票卡逻辑卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	uint8_t		bStatus;						// 交易状态代码
	char		cLockFlag;						// 加解锁标志
	uint8_t		dtDate[7];						// 时间YYYYMMDDHHMMSS
	char		cOperatorID[6];					// 操作员代码(Login)
	uint8_t		bBOMShfitID;					// BOM班次序号每个BOM每天重置	(BCD)
	char		cTestFlag;						// 卡应用标识
	char		cTkAppMode;						//卡应用模式 ， '1'：CPU， '2'：M1/UL, '3'：仿M1
}TICKETLOCK, * PTICKETLOCK;

// 4.12	退款交易
typedef struct {
	char		cTradeType[2];					// 交易类型
	char		cStationID[4];					// 线路站点代码
	char		cDevType[2];					// 设备类型
	char		cDeviceID[3];					// 设备代码(BCD)，第0字节仅低4bit有效
	char		cSAMID[16];						// SAM卡逻辑卡号
	long		lSAMTrSeqNo;					// SAM卡脱机交易流水号
	uint8_t		bTicketType[2];					// 票卡类型
	char		cLogicalID[20];					// 票卡逻辑卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	uint8_t		bStatus;						// 交易状态代码
	long		lBalanceReturned;				// 退还钱包金额, 单位为分
	short		nDepositReturned;				// 退还押金, 单位为分
	short		nForfeiture;					// 罚款, 单位为分
	uint8_t		bForfeitReason;					// 罚款原因
	long		lTradeCount;					// 票卡扣款交易计数
	uint8_t		bReturnTypeCode;				// 退款类型, 0－即时退款；－非即时退款
	uint8_t		dtDate[7];						// 时间YYYYMMDDHHMMSS
	char		cReceiptID[4];					// 凭证ID
	uint8_t		dtApplyDate[7];					// 申请日期时间, YYYYMMDDHHMMSS
	char		cMACOrTAC[10];					// 交易认证码
	char		cOperatorID[6];					// 操作员代码(Login)
	uint8_t		bBOMShfitID;					// BOM班次序号每个BOM每天重置	（BCD）
	long		lBrokerage;						// 手续费
	char		cTestFlag;						// 卡应用标识
	char		cClassicType[2];				// 交易分类
	char		cSamPosId[12];					// SAM卡终端编码
}DIRECTREFUND, * PDIRECTREFUND;

// 4.13	票卡分析返回结构
typedef struct {
	char		cPhysicalID[20];				// 物理卡号,
	char		cLogicalID[20];					// 逻辑卡号
	uint8_t		bTicketType[2];					// 车票类型(主类型+子类型)
	long		lBalance;						// 余值
	long 		lDepositorCost;					// 押金
	long		lLimitedBalance;				// 车票最高上限值

	long		lPenalty;						// 更新罚金

	uint8_t		bIssueData[4];					// 发行时间
	uint8_t		bExpiry[7];						// 物理有效期截止时间(BCD)
	uint8_t		bStartDate[7];					// 逻辑有效期开始时间(BCD)
	uint8_t		bEndDate[7];					// 逻辑有效期结束时间(BCD)
	uint8_t		bStatus;						// 交易状态
	uint8_t		bLastStationID[2];				// 上次交易线路站点(BCD)
	uint8_t		bLastDeviceID[2];				// 上次交易设备编号(BCD),第0字节仅低4bit有效
	uint8_t 	bLastDeviceType; 				// 上次交易设备类型
	uint8_t		dtLastDate[7];					// 上次交易时间
	uint8_t		bEntrySationID[2];				// 上次进站站点
	uint8_t		dtEntryDate[7];					// 上次进站时间
	uint32_t	dwOperationStauts;				// 可操作状态与ErrInfo的错误对应
												// 按位对应以下几种，1表示可操作
												// 	bit0:是否可充(值/乘次)
												// 	bit1:是否可更新
												// 	bit2:是否可发售
												// 	bit3:是否可激活
												// 	bit4:是否可延期
												// 	bit5:是否可退款
												// 	bit6:是否可解锁
												//  bit7:是否可退押金
	uint8_t		bLimitMode;						// 限制模式
	uint8_t		bLimitEntryID[2];				// 进站站点限制（BCD码）
	uint8_t		bLimitExitID[2];				// 出站站点限制(BCD码)
	uint8_t		bActiveStatus;					// 激活状态或激活标记
	char		cCityCode[4];					// 城市代码
	char		cSellerCode[4];					// 发行商代码
	long		lChargeCount;					// 充值计数
	long		lTradeCount;					// 消费计数
	uint8_t		bCertificateType;				// 证件类型
	char		cCertificateCode[32];			// 证件代码
	char		cCertificateName[20];			// 证件持有人姓名
	char		cTestFlag;						// 卡应用标识
	long		lChargeUpper;					// 充值上限，可充值标志位非0时有效
	long 		lDepositeSale;					// 发售押金，可发售标志位非0是有效
	long		lBrokerage;						// 手续费，后续操作需要收费时有效，如非单程票发售、退款

	char		cTkAppMode;						// 卡应用模式 ， '1'：CPU， '2'：M1/UL, '3'：仿M1
	uint8_t		bCertExpire[4];					// 证件有效期，目前主要用于公交学生票，当该有效期过期时，学生票按普通票扣费
	uint16_t	wError;							// 错误码，对应更新罚金lPenalty
	long		lPenalty1;						// 更新罚金lPenalty1
	uint16_t	wError1;						// 错误码1，对应更新罚金1

	uint8_t		bRfu[19];

}BOMANALYZE, * PBOMANALYZE;

// 4.21	电子车票分析返回结构
typedef struct {
	char		cIssueCode[4];					// 发码方标识
	char		cVersion[2];					// 版本号，此版本号为结构版本号，也为发行版本号
	char		cCardStatus;					// 电子票卡状态
	char		cTicketLogicalNo[20];			// 逻辑卡号
	char		cTicketSubType[2];				// 车票子类型
	long		lBalance;						// 余值
	long		lTicketCount;					// 计数器
	char		cCenterCode[32];				// 中心票号

	char 		cSaleDate[14];					// 售票时间
	char		cValidDate[8];					// 有效期

	char		cStartStationId[4];				// 起始站编码
	char		cEndStationId[4];				// 终点站编码
	char		cTicketNum[2];					// 张数
	char		cEntryDate[14];					// 进站时间
	char        cEntryStationId[4];				// 进站站点
	char		cExitDate[14];					// 出站时间
	char        cExitStationId[4];				// 出站站点

	char        cValidExitDate[14];				// 有效出站时间
	char		cLastDealType[2];				// 末次交易类型
	char		cLastDealDate[14];				// 末次交易时间

	char		cPenaltyType[2];				// 罚款类型
	long		lPenalty1;						// 罚款金额  超时罚金
	long		lPenalty2;						// 更新罚金  超乘更新补票


}BOMETICANALYZE, * PBOMETICANALYZE;

// 4.14	储值票单条历史结构
typedef struct {
	uint8_t		dtDate[7];						// 交易时间（BCD码）
	uint8_t		bStationID[2];					// 交易车站ID号（BCD码）
	uint8_t		bStatus;						// 票卡生命周期索引,具体定义见附录五
	long		lTradeAmount;					// 交易金额,单位分
	uint8_t		bDeviceID[2];					// 设备编码（BCD）
	uint8_t		bDeviceType;					// 设备类型
	char		cSAMID[16];						// sam id
}HSSVT, * PHSSVT;

// 4.16	TCM读票卡信息返回信息
typedef struct {
	char		cPhysicalID[20];				// 物理卡号,
	char		cLogicalID[20];					// 逻辑卡号
	uint8_t		bTicketType[2];					// 车票类型(主类型+子类型)
	long		lBalance;						// 余值
	long 		lDepositorCost;					// 押金
	long		lLimitedBalance;				// 车票最高上限值
	uint8_t		bIssueData[4];					// 发行时间
	uint8_t		bExpiry[7];						// 物理有效期截止时间(BCD)
	uint8_t		bStartDate[7];					// 逻辑有效期开始时间(BCD)
	uint8_t		bEndDate[7];					// 逻辑有效期结束时间(BCD)
	uint8_t		bStatus;						// 交易状态
	uint8_t		bLastStationID[2];				// 上次交易线路站点(BCD)
	uint8_t		bLastDeviceID[2];				// 上次交易设备编号(BCD),第0字节仅低4bit有效
	uint8_t 	bLastDeviceType; 				// 上次交易设备类型
	uint8_t		dtLastDate[7];					// 上次交易时间
	uint8_t		bEntrySationID[2];				// 上次进站站点
	uint8_t		dtEntryDate[7];					// 上次进站时间
	uint8_t		bLimitMode;						// 限制模式
	uint8_t		bLimitEntryID[2];				// 进站站点限制（BCD码）
	uint8_t		bLimitExitID[2];				// 出站站点限制(BCD码)
	uint8_t		bActiveStatus;					// 激活状态或激活标记
	char		cCityCode[4];					// 城市代码
	char		cSellerCode[4];					// 发行商代码
	long		lChargeCount;					// 充值计数
	long		lTradeCount;					// 消费计数
	uint8_t		bCertificateType;				// 证件类型
	char		cCertificateCode[32];			// 证件代码
	char		cCertificateName[20];			// 证件持有人姓名
	char		cTestFlag;						// 卡应用标识
	uint8_t		bUsefulCount;					// 有用的数据条数
	char		cTkAppMode;						// 卡应用模式 ， '1'：CPU， '2'：M1/UL, '3'：仿M1
	uint8_t		bCertExpire[4];					// 证件有效期，目前主要用于公交学生票，当该有效期过期时，学生票按普通票扣费
	HSSVT		bhs[17];						// 历史记录，参见// 2.16
}TICKETINFO, * PTICKETINFO;


// 4.18	互联网取票返回信息机构
typedef struct {
	char		cAmount[2];						// 单程票张数
	char		cSum[8];						// 总金额
	char		cCenterCode[32];				// 票号
	char		cTicketLogicalNo[20];			// 票卡逻辑号
	char        cStartStation[4];				// 起始站点编码
	char        cTerminalStation[4];			// 终点站编码

}GETETICKETRESPINFO, * PGETETICKETRESPINFO;


// 4.19	电子车票交易信息结构
typedef struct {
	char		cTxnDate[8];					// 运营日期
	char		cTransCode[2];					// 交易状态
	char		cTicketMainType[2];				// 车票主类型
	char		cTicketSubType[2];				// 车票子类型
	char		cChipType[2];					// 票卡芯片类型
	char		cTicketLogicalNo[20];			// 逻辑卡号
	char		cCenterCode[32];				// 中心票号
	char		cCenterMac[8];					// 中心校验码
	char		cRealTicketNo[16];				// 实体卡号
	char		cMark[2];						// 状态码
	char		cTicketCSN[16];					// 票卡CSN
	char		cTicketCount[10];				// 票计数器
	char		cPsamNo[16];					// PSAM卡号
	char		cPsamSeq[10];					// PSAM卡流水号
	char		cTxnTms[14];					// 交易日期时间
	char		cValidOutTms[14];				// 有效出站时间
	char		cPayType[2];					// 支付方式
	char		cBeforeTxnBalance[10];			// 交易前票值
	char		cTxnAmountNo[6];				// 交易次数
	char		cTxnAmount[6];					// 交易金额
	char		cDiscountAmt[6];				// 优惠金额
	char		cTxnStation[4];					// 当前车站
	char		cLastStation[4];				// 上次使用车站
	char		cLastTxnTms[14];				// 上次使用时间
	char		cDevNodeId[9];					// 终端设备标识
	char		cDevSeqNo[10];					// 终端设备流水号
	char		cTestMark[2];					// 测试交易标志
	char		cQRSerialNo[8];					// 二维码车票OD序列号
	char		cOverAmount[6];					// 闸机超时扣款金额
	char		cTac[8];					    // TAC
	char		cNetTranNumber[64];				// 互联网订单号
	char		cReserve[32];					// 保留域

}ETICKETDEALINFO, * PETICKETDEALINFO;

// 4.20	电子车票联机验证信息结构
typedef struct {
    char		cTransCode[2];					// 交易状态
    char		cTicketMainType[2];				// 车票主类型
    char		cTicketSubType[2];				// 车票子类型
    char		cTicketLogicalNo[20];			// 逻辑卡号
    char		cCenterCode[32];				// 中心票号
    char		cCenterMac[8];					// 中心校验码
    char		cQrNetStatus;				    // 电子票当前网络状态标识  1.网络正常 2.网络中断
}ETICKETAUTHINFO, * PETICKETAUTHINFO;

// 4.20	降级模式消息
typedef struct {
	uint8_t		bDegradeType;					// 降级模式的类型
	uint8_t		bTime[7];						// 降级模式开始/结束时间
	uint16_t	wStationID;						// 降级启用/结束的站点
	uint8_t		bFlag;							// 0x01-启用/0x02-结束降级的标志
}DEGRADECMD, * PDEGRADECMD;

// 寄存器数据结构定义
typedef struct {
	uint16_t	wIndex;
	uint32_t	dwRegister;
}REGISTER, * PREGISTER;

typedef struct{
	char 		cMsgType[2];					// 消息类型“55”
	uint8_t		dtTime[7];						// 时间
	char		cStation[4];					// 线路站点
	char		cDevType[2];					// 设备类型
	char		cDevId[3];						// 设备编码
	char		cSamId[16];						// SAM卡号
	char		cFreedom[16];					// 随机数
}SAM_ACTIVE, * P_SAM_ACTIVE;

// 充值初始化
typedef struct{
	char 		cMsgType[2];					// 消息类型“51”
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
	char		cMac[8];						// 充值认证码
	char		cFreedom[8];					// 随机数
	char		cLastPosID[16];					// 上次交易终端编码（站点 ＋ 设备类型 ＋ 设备编码）
	uint8_t		dtTimeLast[7];					// 上次时间
	char		cOperatorId[6];					// 操作员编码
}CHARGE_INIT, * P_CHARGE_INIT;

// 撤销初始化
typedef struct{
	char 		cMsgType[2];					// 消息类型“53”
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
	char		cBusinessType[2];				// 业务类型“18”
	char		cWalletType;					// 值类型
	long		lAmount;						// 撤销金额
	long		lBalcance;						// 余额
	char		cLastPosID[16];					// 上次交易终端编码（站点 ＋ 设备类型 ＋ 设备编码）
	uint8_t		dtTimeLast[7];					// 上次时间
	char		cOperatorId[6];					// 操作员编码
}DESCIND_INIT, * P_DESCIND_INIT;

// 读写器版本信息
typedef struct {
	uint8_t		verApi[2];						// API识别版本
	uint8_t		verApiFile[5];					// API文件版本，日期＋序号
	uint8_t		verRfDev;						// Rf驱动识别版本
	uint8_t		verRfFile[5];					// Rf文件版本，日期＋序号
	uint8_t		verSamDev;						// Sam驱动识别版本
	uint8_t		verSamFile[5];					// Sam驱动文件版本，日期＋序号
}READERVERSION, * PREADERVERSION;


// 本地文件保存设置
typedef struct
{
	uint16_t	trade_save_days;
	uint16_t	log_save_days;
	int			log_level;
}LOCCONFIG, * PLOCCONFIG;


// 交易确认需要保存的信息结构
typedef struct
{
	int					operType;				// 交易类型
	long				balance;				// 交易后的余额
	long				charge_counter;			// CPU卡使用
	long				consume_counter;		// CPU卡使用
	union										// 交易
	{
		SJTSALE			sjtSale;				// 单程票发售
		OTHERSALE		svtSale;				// 储值票发售
		ENTRYGATE		entry;					// 进闸
		PURSETRADE		purse;					// 钱包交易
		TICKETDEFER		deffer;					// 延期
		TICKETUPDATE	update;					// 更新
		DIRECTREFUND	refund;					// 退款
	}trade;
	uint32_t			size_trade;				// 交易大小
	uint8_t				dataWritten[512];		// 上次写卡数据
	uint32_t			sizeWritten;			// 写数据的大小

}CONFIRM_POINT;

// 交易确认因子，从保存的上次交易结构中获取
typedef struct
{
	char		logic_id[21];
	uint8_t		trade_time[7];
	long 		trade_amount;

	char		device_inf[9];
	uint8_t		tac_type;
}CONFIRM_FACTOR;

// 地铁CPU卡相关结构，交易确认使用
typedef struct
{
	uint8_t flag_trade_assist;
	uint8_t w_trade_assist[LENM_TRADE_ASSIST];			// 交易辅助（符合记录文件）
	uint8_t flag_metro;
	uint8_t w_metro[LENM_METRO];						// 轨道交通（符合记录文件）
	uint8_t flag_ctrl_record;
	uint8_t w_ctrl_record[LENM_CTRL_RECORD];			// 应用辅助，即应用控制记录（符合记录文件）
	uint8_t flag_app_ctrl;
	uint8_t w_app_ctrl[LENM_APP_CTRL];					// 应用控制文件

}METRO_CPU_INF_W, * P_METRO_CPU_INF_W;

// 公交CPU卡相关结构，交易确认使用
typedef struct
{
	uint8_t flag_metro;
	uint8_t w_metro[LENB_METRO];						// 轨道交通（符合记录文件）

}BUS_CPU_INF_W, * P_BUS_CPU_INF_W;


// 电子票交易确认需要保存的信息结构
typedef struct
{
	int			operType;						// 交易类型
	char		cTxnTms[14+1];					// 交易日期时间
	char		cTxnAmount[6+1];				// 交易金额
	char		cCenterCode[32+1];				// 中心票号
	char		cVervifyCode[8];				// 中心校检码
	ETICKETDEALINFO eticketdealinfo;

}CONFIRM_POINT_E;

// 交易信息结构，可以是进站，出站、更新等
typedef struct
{
    uint16_t		station_id;				// 站点
    ETPDVC			dvc_type;				// 设备类型
    uint16_t		dvc_id;					// 设备编码
    uint8_t			time[7];				// 交易时间
    char			sam[17];				// SAM卡号
} TradeInfo, * PTradeInfo;


typedef struct
{
    long	balance;			//余额
    char	cardNo[20+1];		//逻辑卡号
    char    centerCode[32+1];   //票号
    long    counter;
    //char	mac1[8]
    //char	mac2[]
    int		mainType;
    char	payAccount[20+1];
    long	payAmount;
    char	payMethod[1+1];
    //paySequence
    //payTime
    long	price;
    //saleTime
    char	startStation[4+1];
    int		subType;
    char	terminalStation[4+1];
    char	ticketType;
    //validDate
    //version
}BleTicketData,* PBleTicketData;


typedef struct
{
    char	ticketVersion[2+1];			//版本号
    char	subType[2+1];				//子类型
    char    cardNo[20+1];				//卡号
    long    balance;					//余额
    long    counter;					//计数器
    char	centerCode[32+1];			//票号
    char	mac[8+1];                   //中心校验码
    char	saleTime[14+1];				//售票时间
    char	validDate[8+1];				//有效期
    char	startStation[4+1];			//起始站编码,或进站站点
    char    entryTime[14+1];            //进站时间
    char	terminalStation[4+1];		//终点站编码
    long    price;						//单价
    char    amount[2+1];				//张数
    long	sum;						//总价
    char	vervifyCode[8+1];			//验证码
    char    applyTime[14+1];            //当前二维码生成时间
    char	entryDeviceType[2+1];       //实际进站设备类型
    char	entryDeviceCode[2+1];       //实际进站设备序号
    char    validOutTime[14+1];         //有效出站时间
    char    NetTranNumber[64+1];        //互联网订单号
    long	TxnAmount;                  //交易金额
    char    exitTime[14+1];				//出站时间
    char    exitStation[4+1];			//出站编码
    char	exitDeviceType[2+1];        //实际出站设备类型
    char	exitDeviceCode[2+1];        //实际出站设备序号
    char	cPenaltyType[2];			//罚款类型
    long	lPenalty1;					//罚款金额超时
    long	lPenalty2;					//更新罚金超程
	char    lastTradeTime[14];			//上次交易时间
	char	lasttredeType[2];			//上次交易类型
	char 	centerCodeProductTime[14+1];//当前票号生成时间
	char 	retainField[10+1];			//降级二维码保留域
}QRTicketData,* PQRTicketData;

// 交通部交易信息结构，可以是进站，出站、更新等
typedef struct
{
    char            city_code[4];           // 城市代码
    uint8_t         institutionCode[8];		// 机构标识
    uint16_t		station_id;				// 站点
    uint8_t			time[7];				// 交易时间
    char			sam[17];				// SAM卡号-终端编码
    uint8_t         line_id;                // 线路号
    long            deal_fee;               // 交易金额
    long            balance;                // 进站钱包余额
} JTBTradeInfo, * PJTBTradeInfo;

// 卡数据结构
typedef struct
{
    ETPMDM			physical_type;			// 物理类型
    char			issue_code[4];			// 发行方主代码
    char			city_code[4];			// 城市代码
    char			industry_code[4];		// 行业代码
    char			physical_id[21];		// 物理卡号
    char			logical_id[21];			// 逻辑卡号
    uint8_t			key_flag;				// 应用标识（正式票，测试票）

    uint8_t			date_issue[4];			// 发行时间
    uint8_t			phy_peroidE[4];			// 物理有效期
    uint8_t			logical_peroidS[7];		// 逻辑有效期
    uint8_t			logical_peroidE[7];
    int				effect_mins;			// 有效分钟数（发售时写入发售后的有效期时间，延期时加上可延期时间）

    char			certificate_name[20];	// 证件持有人姓名
    char			certificate_code[32];	// 证件代码
    uint8_t			certificate_type;		// 证件类型

    uint8_t			cert_peroidE[4];		// 证件有效期
    uint8_t			charge_peroidS[4];		// 充值有效期
    uint8_t			charge_peroidE[4];
    uint8_t			active_peroidS[7];		// 激活有效期
    uint8_t			active_peroidE[7];

    uint8_t			logical_type[2];		// 逻辑票卡类型
    uint8_t			fare_type[2];			// 票价类型
    long			wallet_value;			// 钱包余值
    long			deposit;				// 押金
    long			wallet_value_max;		// 钱包上限
    uint8_t			list_falg;				// 名单标识
    uint16_t		charge_counter;			// 充值累计
    long			daliy_trade_counter;	// 日交易次数
    long			total_trade_counter;	// 总交易次数
    long			daliy_update_counter;	// 日更新次数
    long			total_update_counter;	// 总更新次数
    uint8_t			app_lock_flag;			// 应用锁定标记（发售或多日票激活时使用）
    uint8_t			globle_status;			// 全局状态(初始化，可使用，退款，)
    uint8_t			init_status;			// 原始状态
    SimpleStatus	simple_status;			// 转换为标准的票卡状态

    uint8_t			limit_mode;				// 限制进出模式
    uint8_t			limit_entry_station[2];	// 限制进站站点
    uint8_t			limit_exit_station[2];	// 限制出站站点

    PTradeInfo		p_entry_info;			// 进站信息
    PTradeInfo		p_update_info;			// 更新信息
    PTradeInfo		p_exit_info;			// 出站信息
    TradeInfo		read_last_info;			// 上次交易信息，赋值用read_last_info，写卡用write_last_info
    TradeInfo		write_last_info;

    PJTBTradeInfo		p_jtb_entry_info;	// 交通部票卡进站信息
    PJTBTradeInfo		p_jtb_update_info;	// 交通部票卡更新信息
    PJTBTradeInfo		p_jtb_exit_info;	// 交通部票卡出站信息
    JTBTradeInfo		jtb_read_last_info;	// 上次交易信息，赋值用read_last_info，写卡用write_last_info
    JTBTradeInfo		jtb_write_last_info;

    char			ticket_app_mode;		// 卡应用模式

    //17管理信息文件
    char			international_code[8+1];	// 国际代码
    char			province_code[4+1];		    // 省级代码
    uint8_t			city_code_17[2];		    // 城市代码
    uint8_t			interCardFlag[2];		    // 互通卡种
    uint8_t			jtbCardType;		        // 卡种类型

    // 多日票相关信息
    //short			limit_days;				// 乘次票使用天数

    uint8_t qr_flag;

    //二维码结构读取
    char			qr_issueFlag[32+1];		    //发码方标识
    char			qr_version[2+1];			//版本号  此版本号为结构版本号，也为发行版本号
    char			qr_encMode[2+1];			//加密方式
    uint8_t			qr_passcode[32+1];		    //PASSCODE
    char			qr_timestamp[14+1];		    //时间戳
    char			qr_bleMac[12+1];			//手机蓝牙识别码
    char			qr_rfu[20+1];			    //预留
    char            qr_ticket_status;           //票卡状态
    char			qr_ticker_isTest;		    //1生产 2测试
	char			qr_Intnet_ststus;			//网络状态标识1.网络正常 2.网络中断
    //蓝牙数据结构读取
    BleTicketData	ble_ticketdata;

    QRTicketData	qr_ticketdata;

    uint8_t			ble_macAddress[6];		//蓝牙地址

    uint8_t      	qr_info[256];

    char      		third_qr_info[64];    //第三方支付二维码

    uint8_t      	timeAndcenterCode[46+1];    //时间戳和中心票号 用来方式重复进闸

} TICKET_DATA, * P_TICKET_DATA;
//电子票异常处理交易
typedef struct{
    char    trad_type[2];       //交易类型
    char    center_code[32];    //中心票号
    long    amount_timeout;     //超时罚金
    long    amount_over_take;   //超乘补票金额
    char    station_code[4];    //待补车站代码
	char	net_result[1];		//BOM联机更新结果，0为更新成功，可先组交易上去，非0为等待蓝牙结果更新
} ETICKET_ABNORML_HANDLE, * PETICKET_ABNORML_HANDLE;


// 4.19	电子车票支付交易信息结构
typedef struct {
	char		cTxnDate[8];					// 运营日期
	char		cTransCode[2];					// 交易类型
	char		cTicketMainType[2];				// 车票主类型
	char		cTicketSubType[2];				// 车票子类型
	char		cChipType[2];					// 票卡芯片类型
	char		cTicketLogicalNo[20];			// 逻辑卡号
	char		cCenterCode[32];				// 中心票号
	char		cCenterMac[8];					// 中心校验码
	char		cRealTicketNo[16];				// 实体卡号
	char		cMark[2];						// 取票状态码
	char		cTicketCSN[16];					// 票卡CSN
	char		cTicketCount[10];				// 票计数器
	char		cPsamNo[16];					// PSAM卡号
	char		cPsamSeq[10];					// PSAM卡流水号
	char		cTxnTms[14];					// 交易日期时间
	char		cValidOutTms[14];				// 有效出站时间
	char		cPayType[2];					// 支付方式
	char		cBeforeTxnBalance[10];			// 交易前票值
	char		cTxnAmountNo[6];				// 交易次数
	char		cTxnAmount[6];					// 交易金额
	char		cDiscountAmt[6];				// 优惠金额
	char		cTxnStation[4];					// 当前车站
	char		cLastStation[4];				// 上次使用车站
	char		cLastTxnTms[14];				// 上次使用时间
	char		cDevNodeId[9];					// 终端设备标识
	char		cDevSeqNo[10];					// 终端设备流水号
	char		cTestMark[2];					// 测试交易标志
	char		cQRSerialNo[8];					// 二维码车票OD序列号
	char		cOverAmount[6];					// 闸机超时扣款金额
	char		cTac[8];					    // TAC
	char		cNetTranNumber[64];				// 互联网订单号
	char		cReserve[32];					// 保留域32

}ETICKETPAYDEALINFO, * PETICKETPAYDEALINFO;

// 4.22	电子单程票卡返回结构
typedef struct {
	char		cIssueCode[4];					// 发码方标识
	char		cVersion[2];					// 版本号
	char		qr_Intnet_ststus;				// 网络状态标识1.网络正常 2.网络中断
	char		cCardStatus;					// 电子票卡状态
	char		cTicketLogicalNo[20];			// 逻辑卡号
	char		cTicketSubType[2];				// 车票子类型
	long		lBalance;						// 余值
	long		lTicketCount;					// 计数器
	char		cCenterCode[32];				// 中心票号

	char 		cSaleDate[14];					// 售票时间
	char		cValidDate[8];					// 有效期

	char		cStartStationId[4];				// 起始站编码
	char		cEndStationId[4];				// 终点站编码
	long		cPrice;							// 单价
	char		cTicketNum[2];					// 张数
	char		cEntryDate[14];					// 进站时间
	char        cEntryStationId[4];				// 进站站点
	char		cExitDate[14];					// 出站时间
	char        cExitStationId[4];				// 出站站点

	char        cValidExitDate[14];				// 有效出站时间
	char		cMacCode[8];				    // 中心验证码
    char		cPsamNo[16];					// PSAM卡号
    char		cPsamSeq[10];					// PSAM卡流水号
	char        cVervifyCode[8];                //验证码
}BOMESJTRETURN, * PBOMESJTRETURN;

// 4.23	电子储值票卡返回结构
typedef struct {
	char		cIssueCode[4];					// 发码方标识
	char		cVersion[2];					// 版本号
	char		qr_Intnet_ststus;				// 网络状态标识1.网络正常 2.网络中断
	char		cCardStatus;					// 电子票卡状态
	char		cTicketLogicalNo[20];			// 逻辑卡号
	char		cTicketSubType[2];				// 车票子类型
	long		lBalance;						// 余值
	char		cCenterCode[32];				// 中心票号
	char		cEntryDate[14];					// 进站时间
	char        cEntryStationId[4];				// 进站站点
	char		cExitDate[14];					// 出站时间
	char        cExitStationId[4];				// 出站站点

	char        cValidExitDate[14];				// 有效出站时间
	char		cLastDealType[2];				// 末次交易类型
	char		cLastDealDate[14];				// 末次交易时间
	char		cMacCode[8];				    // 中心验证码
	char		cBornTime[14];					// 生成时间 后付费储值票生效
	char        cVervifyCode[8];                 //验证码
    char		cPsamNo[16];					// PSAM卡号
    char		cPsamSeq[10];					// PSAM卡流水号
    char        cEntryDevice[5];                // 进站设备编码  设备类型(2)+设备编号(3)
    char        cExitDevice[5];                 // 出站设备编码  设备类型(2)+设备编号(3)

}BOMESVTRETURN, * PBOMESVTRETURN;

// 4.24	降级电子票卡返回结构
typedef struct {
	char		cIssueCode[4];					// 发码方标识
	char		cVersion[2];					// 版本号
	char		qr_Intnet_ststus;				// 网络状态标识1.网络正常 2.网络中断
	char		cCardStatus;					// 电子票卡状态
	char		cTicketLogicalNo[20];			// 逻辑卡号
	char		cTicketSubType[2];				// 车票子类型
	long		lBalance;						// 余额
	char		cCenterCode[32];				// 中心票号

	char		cGenerateDate[14];				// 降级二维码生成时间
	char        cCenterCodeGenDate[14];	        // 中心票号生成时间
	char        cRetain[10];				    // 保留域
	char		cMacCode[8];				    // 中心验证码

}BOMEOFFLINERETURN, * PBOMEOFFLINERETURN;


#pragma pack(pop)

