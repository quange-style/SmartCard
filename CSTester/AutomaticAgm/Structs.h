// Structs.h

#pragma once

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

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

// 2.1	读写器初始化信息
typedef struct {
	uint8_t		bReaderReadyStatus;				// 读写器就绪状态
	SAMSTATUS  SamStatus[4];					// 各SAM卡状态信息，暂定4卡槽，参见// 2.3
}READERSTATUS, * PREADERSTATUS;

// 2.4	入闸交易
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
	long		lTradeCount;					// 消费计数
	char		cTkAppMode;						//卡应用模式 ， '1'：CPU， '2'：M1/UL, '3'：仿M1
}ENTRYGATE, * PENTRYGATE;

// 2.5	钱包交易
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
	long		lChargeCount;					// 充值计数
	long		lTradeCount;					// 消费计数
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
	char		cSamPosId[12];					// POS终端编号
	char		cTkAppMode;						//卡应用模式 ， '1'：CPU， '2'：M1/UL, '3'：仿M1
}PURSETRADE, * PPURSETRADE;

// 2.6	非单程票发售记录
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

// 2.7	单程票发售记录
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
	long		lTradeCount;					// 消费计数
	char		cLogicalID[20];					// TOKEN 逻辑ID
	char		cPhysicalID[20];				// TOKEN 物理ID
	uint8_t		bStatus;						// TOKEN状态
	short		nChargeValue;					// 充值金额
	uint8_t		bTicketType[2];					// 单程票卡类型
	uint8_t		bZoneID;						// 区段代码
	char		cMACorTAC[10];					// SAM生成的交易检查代码,	默认值：
	uint8_t		bDepositorCost;					// 成本押金(BCD)
	short		nAmountCost;					// 成本押金金额
	char		cOperatorID[6];					// 操作员代码(Login)
	uint8_t		bBOMShiftID;					// BOM班次序号, 每个BOM每天重置
	long		lBrokerage;						// 手续费
	char		cTestFlag;						// 卡应用标识
	char		cClassicType[2];				// 交易分类
	char		cSamPosId[12];					// POS终端编号
}SJTSALE, * PSJTSALE;

// 2.8	交易确认信息结构
typedef struct {
	uint8_t	bTradeType;							// 交易类型
	uint8_t	bData[512];							// 上次的交易记录结构的拷贝
}TRANSACTCONFIRM, * PTRANSACTCONFIRM;

// 2.9	更新交易
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
	long		lTradeCount;					// 消费计数
	uint8_t		bStatus;						// 交易状态代码
	char		cUpdateZone;					// 更新区域
	uint8_t		bUpdateReasonCode;				// 更新原因
												// 更新原因代码表
												// 原因代码	描述（Description）
												// 付费区：01－出站超时，02-超乘，03-无进站码
												// 非付费区：10－有进站码，12－进站超时
	uint8_t		dtUpdateDate[7];				// 更新日期时间YYYYMMDDHHMMSS
	uint8_t		bPaymentMode;					// 罚金支付方式
												// 	1-现金，2-储值卡， 3-一卡通
												//	4-Credit， 5- civil charge ‘默认值’’
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

// 2.10	票卡延期交易
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

// 2.11	票卡加解锁交易
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

// 2.12	退款交易
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
	long		lTradeCount;					// 消费计数
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
	char		cSamPosId[12];					// POS终端编号
}DIRECTREFUND, * PDIRECTREFUND;

// 2.13	票卡分析返回结构
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
	uint8_t		bRfu[32];

}BOMANALYZE, * PBOMANALYZE;

// 2.14	储值票单条历史结构
typedef struct {
	uint8_t		dtDate[7];						// 交易时间（BCD码）
	uint8_t		bStationID[2];					// 交易车站ID号（BCD码）
	uint8_t		bStatus;						// 票卡生命周期索引,具体定义见附录五
	long		lTradeAmount;					// 交易金额,单位分
	uint8_t		bDeviceID[2];					// 设备编码（BCD）
	uint8_t		bDeviceType;					// 设备类型
	char		cSAMID[16];						// sam id
}HSSVT, * PHSSVT;

// 2.15	读储值票历史信息结构
typedef struct {
	char 		cPhysicalID[20]; 					// 物理卡号,
	char 		cLogicalID[20]; 					// 逻辑卡号
	uint8_t 		bTicketType[2];						// 车票类型,车票主类型+车票子类型
	uint8_t		bUsefulCount; 					// 票卡内有效的历史数据条数
	HSSVT		pHistory[17];						// 旧地铁历史记录，参见// 2.16
}SVTHISTORY, * PSVTHISTORY;

// 2.16	TCM读票卡信息返回信息
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
	HSSVT	bhs[17];							// 历史记录，参见// 2.16
}TICKETINFO, * PTICKETINFO;

// 2.17	ES订单输入信息结构
typedef struct {
	char		cOrderNo[14];  					// 订单号
	char		cApplicationNO[10];				// 记名卡申请码
	uint16_t	wTicketType;					// 车票类型
	uint32_t	dwSysFlow;   					// 系统流水（只对单程票有效，储值票无效）
	long		lDeposite;						// 押金
	long		lValue;           				// 车票钱包预赋金额
	uint8_t		dtStartDate[7];					// 逻辑有效期开始时间(BCD)
	uint8_t		dtEndDate[7];					// 逻辑有效期结束时间(BCD)
	uint8_t		dtExpire[7];					// 车票物理有效期截止时间(BCD)
}ORDERSIN, * PORDERSIN;

// 2.18	ES订单输出信息结构
typedef struct {
	char		cTradeType[2];					// 交易类型
	char		cOrderNo[14];				 	// 订单编号
	uint8_t		bTicketType[2];					// 票卡类型
	char		cApplicationNO[10];				//记名卡申请码
	char		cLogicalID[20];					// 票卡逻辑卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	uint8_t		dtDate[14];						// 制票日期时间
	long		lBalance;						// 票卡余额, 单位为分(默认为)
	uint8_t		bStartDate[7];					// 逻辑有效期开始时间(BCD)
	uint8_t		bEndDate[7];					// 逻辑有效期结束时间(BCD)
	char		cSAMID[16];						// E/S SAM逻辑卡号, 默认值:’’
}ORDERSOUT, * PORDERSOUT;

// 2.19	ES票卡分析返回结构
typedef struct {
	uint8_t		bStatus;						// 交易状态－参见附录四
	uint8_t		bTicketType[2];					// 票卡类型
	char		cLogicalID[20];					// 票卡逻辑卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	uint8_t		bCharacter;						// 票卡物理类型OCT,UL,FM,F:其他
	uint8_t		bIssueStatus;					// 发行状态0 –未发行;1 –已发行;2 –注销
	char		dtIssueDate[7];					// 制票日期时间日期时间
	char		dtExpire[7];					// 物理有效期, YYYYMMDDHHMMSS，
	long		lBalance;						// 余额, 单位为分/次(默认为)
	long 		lDeposite;						// 押金
}ESANALYZE, * PESANALYZE;

// 2.20	降级模式消息
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
typedef struct READER_VERSION{
	uint8_t		verApi[2];						// API识别版本
	uint8_t		verApiFile[5];					// API文件版本，日期＋序号
	uint8_t		verRfDev;						// Rf驱动识别版本
	uint8_t		verRfFile[5];					// Rf文件版本，日期＋序号
	uint8_t		verSamDev;						// Sam驱动识别版本
	uint8_t		verSamFile[5];					// Sam驱动文件版本，日期＋序号
}READERVERSION, * PREADERVERSION;

#pragma pack(pop)
