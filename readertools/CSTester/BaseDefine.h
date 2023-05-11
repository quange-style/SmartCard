//********************************************************************
// 文件名称	: BaseDefine.h
// 创建时间	: 2011/06/02
// 修改时间	:
// 修改人	:
// 文件类型	: 模块头文件
// 描述		: 声明模块对外接口结构
//********************************************************************

#pragma once
#pragma pack( push, before_include6,1 )

typedef struct _ReadStatus{
	BYTE		bReaderReadyStatus;				// 读写器就绪状态
	BYTE		bMetroSAMStatus;				// METRO_SAM卡状态(0x00-正常, 0xFF-故障)
	char		cMetroSAMID[16];				// METRO_SAM卡ID
	long		lMetroSAMSVTBalance;			// METRO_SAM卡SVTBalance
	long		lMetroSAMTripCountBalance;		// METRO_SAM卡TripCountBalance
	long		lMetroSAMSJTBalance;			// METRO_SAM卡SJTBalance
	BYTE		bOCTSAMStatus;					// OCT_SAM卡状态 – (0x00-正常, 0xFF-故障)
	char		cOCTSAMID[16];					// OCT_SAM卡ID
	long		lOCTSAMSVTBalance;				// OCT_SAM卡SVTBalance
	long		lOCTSAMTripCountBalance;		// OCT_SAM卡TripCountBalance
	long		lOCTSAMSJTBalance;				// OCT_SAM卡SJTBalance
	BYTE		bNewSAMStatus;					// NEW_SAM卡状态 – (0x00-正常, 0xFF-故障)
	char		cNewSAMID[16];					// NEW_SAM卡ID
	long		lNewSAMSVTBalance;				// NEW_SAM卡SVTBalance
	long		lNewSAMTripCountBalance;		// NEW_SAM卡TripCountBalance
	long		lNewSAMSJTBalance;				// NEW_SAM卡SJTBalance
	BYTE		bMobileStatus;					// Mobile卡状态 – (0x00-正常, 0xFF-故障)
	char		cMobileSAMID[16];				// Mobile卡ID
	long		lMobileSAMSVTBalance;			// Mobile卡SVTBalance
	long		lMobileSAMTripCountBalance;		// Mobile卡TripCountBalance
	long		lMobileSAMSJTBalance;			// Mobile卡SJTBalance
}READSTATUS, *PREADSTATUS;

typedef struct _ReaderVersion{
	char		cAPIVersion[11];				// API版本信息
	char		cFirmwareVersion[11];			// Firmware版本信息
}READERVERSION, *PREADERVERSION;


typedef struct _SAMCardStatus{
	BYTE		bSAMStatus;						// SAM卡状态 –(0x00表示正常，0xFF-表示故障后续参数无用)
	char		cSAMID[16];						// SAM卡ID号
	long		lSAMSVTBalance ;				// SAM卡SVTBalance (非地铁卡此值为0)
	long		lSAMTripCountBalance;			// SAM卡TripCountBalance (非地铁卡此值为0)
	long		lSAMSJTBalance;					// SAM卡SJTBalance (非地铁卡此值为0)
}SAMCARDSTATUS, *PSAMCARDSTATUS;
	
typedef struct _MpSamIncrease
{
	char 		cSAMID[16];						// SAM卡逻辑卡号
	long		lBalance; 						// SAM钱包余值
}MPSAMINCREASE, *PMPSAMINCREASE;


typedef struct _EntryGate{
	//-------------------------------------------------------------------------------
	// 对乘客的进站信息进行记录
	//-------------------------------------------------------------------------------
	char		cTradeType[2];					// 交易类型
	BYTE		bStationID[2];					// 站点代码
	BYTE		bDevGroupType;					// 设备类型
	BYTE		bDeviceID[2];					// 设备代码
	char		cSAMID[16];						// SAM卡逻辑卡号
	long		lSAMTrSeqNo;					// SAM卡脱机交易流水号
	BYTE		bTicketType[2];					// 票卡类型
	char		cLogicalID[16];					// 票卡逻辑卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	BYTE		dtDate[7];						// 进站日期时间 如;20060211160903
	BYTE		bStatus;						// 卡状态代码
	long		lBalance;						// 余额

	BYTE		bCountTimes[2];					// 羊城通优惠门槛月份
	short 		nBusTimes;						// 羊城通公交门槛计数
	short  		nMtrTimes;						// 羊城通地铁门槛计数
	short   	nUnionTimes;					// 羊城通公交地铁联乘计数

	long		lChargeCount;					// 票卡充值交易计数
	long		lTradeCount;					// 票卡消费交易计数
	char		cTac[10];						// 交易认证码
	char		cPayType[2];					// 支付类型
	char		cLastSam[16];					// 上次交易的SAM卡号
	BYTE		dtLast[7];						// 上次交易时间
	char		cTRSort[2];						// 交易分类
	char		cAreaCode[4];					// 区域代码
	char		cAreaTKType[4];					// 区域卡类型
	long		lOverdraft;						// 透支额度
	char		cCpuTK;							// 票卡分类
	char		cExpiredTK;						// 过期票卡
	short		nEffectDays;					// 乘次票使用天数
	BYTE		dtActive[7];					// 乘次票激活时间
	char		cLimitMode[3];					// 乘次票限制模式
	BYTE		bLimitEntryID[2];				// 限制进站站点
	BYTE		bLimitExitID[2];				// 限制出站站点

	long   		lRFU1;							// 预留字段1
	long 		lRFU2;							// 预留字段2
}ENTRYGATE, *PENTRYGATE;

typedef struct _PurseTrade{
	//-------------------------------------------------------------------------------
	// 票卡钱包金额发生变化（扣值、充值）时而产生的记录。根据交易类型可分为：
	// 免费消费、乘车消费、钱包提现、钱包充值、专用钱包圈存到行业钱包、
	// 一般消费、购买单程票、支付罚金。
	//-------------------------------------------------------------------------------

	char		cTradeType[2];					// 交易类型
	BYTE		bStationID[2];					// 站点代码	(BCD)
	BYTE		bDevGroupType;					// 设备类型	(BCD) 
	BYTE		bDeviceID[2];					// 设备代码	(BCD)
	char		cSAMID[16];						// 本次交易SAM逻辑卡号
	long		lSAMTrSeqNo;					// 本次交易SAM卡脱机交易流水号
	BYTE		dtDate[7];						// 日期时间	(BCD)
	BYTE		bTicketType[2];					// 票卡类型	(BCD)
	char		cLogicalID[16];					// 票卡逻辑卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	BYTE		bStatus;						// 卡状态代码	(BCD)
	long		lTradeAmount;					// 交易金额, 单位为分
	long		lBalance;						// 余额, 单位为分
	long		lTicketchargeCount;				// 票卡充值交易计数, 每次充值交易累加1
	long		lTicketMarkdownCount;			// 票卡扣款交易计数, 每次扣款交易减1
	char		cPaymentType[2];				// 支付类型
	char		cReceiptID[4];					// 凭证ID, 0001—9999	默认值:0000
	char		cMACorTAC[10];					// 交易认证码
	BYTE		bEntryStationID[2];				// 入口站点代码	(BCD)
	char		cEntrySAMID[16];				// 入口SAM逻辑卡号, 默认值:’0000000000’，非乘车消费时为上次设备的SAM卡号。
	BYTE		dtEntryDate[7];					// 进站日期时间, YYYYMMDDHHMMSS	默认值:00000000000000		非乘车消费时为上次设备操作时间。
	char		cOperatorID[6];					// 操作员代码(Login)
	BYTE		bBOMShiftID;					// BOM班次序号
	char		cLastSAMID[16];					// 上次交易SAM逻辑卡号, 终端设备的SAM卡逻辑卡号，默认值为本次交易设备编号。
	BYTE		dtLastDate[7];					// 上次交易日期时间, YYYYMMDDHHMMSS，默认值为本次交易日期时间

	long	 	lFare;							// 单位为分的长整型，读写器查询0100票种的票价表得到	
	BYTE		bCountTimes[2];					// 羊城通优惠门槛月份
	short 		nBusTimes;						// 羊城通公交门槛计数
	short  		nMtrTimes;						// 羊城通地铁门槛计数
	short   	nUnionTimes;					// 羊城通公交地铁联乘计数

	char		cTRSort[2];						// 交易分类
	char		cAreaCode[4];					// 区域代码
	char		cAreaTKType[4];					// 区域卡类型
	long		lOverdraft;						// 透支额度
	char		cCpuTK;							// 票卡分类
	char		cExpiredTK;						// 过期票卡
	short		nEffectDays;					// 乘次票使用天数
	BYTE		dtActive[7];					// 乘次票激活时间
	char		cLimitMode[3];					// 乘次票限制模式
	BYTE		bLimitEntryID[2];				// 限制进站站点
	BYTE		bLimitExitID[2];				// 限制出站站点

	long   		lRFU1;							// 预留字段1
	long 		lRFU2;							// 预留字段2

}PURSETRADE, *PPURSETRADE;

typedef struct _TvmSamIncreat{
	char		cSAMPacketID[8];				// SAM主钱包ID
	long		lBalance;						// SAM钱包余值
}TVMSAMINCREAT, *PTVMSAMINCREAT;

//非单程票发售记录
typedef struct _OtherSaleRecord {
	char		cTradeType[2];					// 交易类型
	BYTE		bStationID[2];					// 站点代码
	BYTE		bDevGroupType;					// 设备类型
	BYTE		bDeviceID[2];					// 设备代码
	BYTE		bTicketType[2];					// 票卡类型
	char		cLogicalID[16];					// 票卡逻辑卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	BYTE		bStatus;						// 卡状态代码	
	char		cBusinessseqno[12];				// 脱机业务流水号
	char		cSAMID[16];						// SAM卡逻辑卡号（单程票）
	long		lSAMTrSeqNo;					// SAM卡脱机交易流水号（单程票）
	BYTE		bAmountType;					// 金额类型BCD
	short		nAmount;						// 金额
	BYTE		dtDate[7];						// 销售时间BCD
	char		cReceiptID[4];					// 凭证ID
	char		cOperatorID[6];					// 操作员代码(Login)
	BYTE		bBOMShfitID;					// BOM班次序号 每个BOM每天重置，第1个操作员登录为1，第2个操作员登录为2…	默认值：00	(BCD)
}OTHERSALERECORD, *POTHERSALERECORD;

//单程票发售记录
typedef struct _SJTSaleRecord {
	char		cTradeType[2];					// 交易类型
	BYTE		bStationID[2];					// 站点代码
	BYTE		bDevGroupType;					// 设备类型
	BYTE		bDeviceID[2];					// 设备代码
	char		cSAMID[16];						// SAM卡逻辑卡号（单程票）
	long		lSAMTrSeqNo;					// SAM卡脱机交易流水号（单程票）
	BYTE		dtDate[7];						// 时间
	BYTE		bPaymentMeans;					// 支付方式
	char		cPaymentTKLogicalID[16];		// 支付票卡逻辑卡号
	long		lTicketMarkdownCount;			// 票卡扣款交易计数, 每次扣款交易累加1
	char		cLogicalID[16];					// TOKEN 逻辑ID
	char		cPhysicalID[20];				// TOKEN 物理ID
	BYTE		bStatus;						// TOKEN状态
	short		nchargeValue;					// 充值金额
	BYTE		bTicketType[2];					// 单程票卡类型
	BYTE		bZoneID;						// 区段代码
	char		cMACorTAC[10];					// SAM生成的交易检查代码,	默认值：0000000000
	BYTE		bDepositorCost;					// 成本/押金, 00－押金，01－成本，02－不使用（押金可退，成本不可退）
	short		nAmountofDepositorCost;			// 成本/押金 金额, 单位为分的整数，默认0
	char		cOperatorID[6];					// 操作员代码(Login)
	BYTE		bBOMShiftID;					// BOM班次序号, 每个BOM每天重置，第1个操作员登录为1，第2个操作员登录为2…	默认值：00
}SJTSALERECORD, *PSJTSALERECORD;

typedef struct _TransactConfirm
{
	BYTE	bTradeType;						// 交易类型
	BYTE	bData[255];						// 上次的交易记录内容
}TRANSACTCONFIRM, *PTRANSACTCONFIRM;

typedef struct _TvmSamIncrease{
	char		cSAMMainPacketID[8];
	long		lSAMPacketBalance;

}TVMSAMINCREASE, *PTVMSAMINCREASE;

typedef struct _TicketUpdate {
	char		cTradeType[2];					// 交易类型
	BYTE		bStationID[2];					// 站点代码
	BYTE		bDevGroupType;					// 设备类型
	BYTE		bDeviceID[2];					// 设备代码
	char		cSAMID[16];						// SAM卡逻辑卡号
	long		lSAMTrSeqNo;					// 更新设备SAM卡脱机交易流水号
	BYTE		bTicketType[2];					// 票卡类型
	char		cLogicalID[16];					// 票卡逻辑卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	long		lTicketMarkdownCount;			// 票卡扣款交易计数
	BYTE		bStatus;						// 卡状态代码
	BYTE		bUpdateArea;					// 更新区域‘1’－付费区，‘2’－非付费区
	BYTE		bUpdateReasonCode;				// 更新原因
												//	更新原因代码表
												//		原因代码	描述（Description）
												//		付费区
												//		01 	出站超时
												//		02	超乘
												//		03	无进站码
												//
												//		非付费区
												//		10	有进站码
												//		12	进站超时
	BYTE		dtUpdateDate[7];				// 更新日期时间 YYYYMMDDHHMMSS
	BYTE		bPaymentMode;					// 支付方式 1-现金//2-储值卡//3-羊城通//4-Credit//5- civil charge ‘默认值’00’
	short		nForfeiture;					// 罚款金额, 需缴纳的罚款金额（用现金支付），单位为分的整数, 默认：0。
	char		cReceiptID[4];					// 凭证ID
	char		cOperatorID[6];					// 操作员代码(Login)
	BYTE		bEntryStationID[2];				// 入口站点代码
	BYTE		bBOMShfitID;					// BOM班次序号 每个BOM每天重置，第1个操作员登录为1，第2个操作员登录为2…	默认值：00

	BYTE		bCountTimes[2];					// 羊城通优惠门槛月份
	short 		nBusTimes;						// 羊城通公交门槛计数
	short  		nMtrTimes;						// 羊城通地铁门槛计数
	short   	nUnionTimes;					// 羊城通公交地铁联乘计数

	long		lBalance;						// 余额
	long		lTokenFare;						// 原票价
	long		lChargeCount;					// 充值交易计数
	char		cTac[10];						// 交易认证码
	char		cPayType[2];					// 支付类型
	char		cEntrySam[16];					// 进站SAM卡号
	BYTE		dtEntry[7];						// 进站日期时间
	char		cLastSam[16];					// 上次交易的SAM卡号
	BYTE		dtLast[7];						// 上次交易时间
	char		cTRSort[2];						// 交易分类
	char		cAreaCode[4];					// 区域代码
	char		cAreaTKType[4];					// 区域卡类型
	long		lOverdraft;						// 透支额度
	char		cCpuTK;							// 票卡分类
	char		cExpiredTK;						// 过期票卡
	short		nEffectDays;					// 乘次票使用天数
	BYTE		dtActive[7];					// 乘次票激活时间
	char		cLimitMode[3];					// 乘次票限制模式
	BYTE		bLimitEntryID[2];				// 限制进站站点
	BYTE		bLimitExitID[2];				// 限制出站站点

	long   		lRFU1;							// 预留字段1
	long 		lRFU2;							// 预留字段2
}TICKETUPDATE, *PTICKETUPDATE;

typedef struct _TicketDefer {
	char		cTradeType[2];					// 交易类型
	BYTE		bStationID[2];					// 站点代码
	BYTE		bDevGroupType;					// 设备类型
	BYTE		bDeviceID[2];					// 设备代码
	char		cSAMID[16];						// SAM卡逻辑卡号
	long		lSAMTrSeqNo;					// SAM卡脱机交易流水号
	BYTE		bTicketType[2];					// 票卡类型
	char		cLogicalID[16];					// 票卡逻辑卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	BYTE		bStatus;						// 卡状态代码
	BYTE		dtOldExpiryDate[7];				// 原有效时间
	BYTE		dtNewExpiryDate[7];				// 现有效时间
	BYTE		dtOperateDate[7];				// 操作时间
	char		cOperatorID[6];					// 操作员代码(Login)
	BYTE		bBOMShfitID;					// BOM班次序号 每个BOM每天重置，第1个操作员登录为1，第2个操作员登录为2…	默认值：00

}TICKETDEFER, *PTICKETDEFER;

typedef struct _TicketLock {	 	
	char		cTradeType[2];					// 交易类型
	BYTE		bStationID[2];					// 站点代码
	BYTE		bDevGroupType;					// 设备类型
	BYTE		bDeviceID[2];					// 设备代码
	char		cSAMID[16];						// SAM卡逻辑卡号
	long		lSAMTrSeqNo;					// SAM卡脱机交易流水号
	BYTE		bTicketType[2];					// 票卡类型
	char		cLogicalID[16];					// 票卡逻辑卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	BYTE		bStatus;						// 卡状态代码
	char		cLockFlag;						// 加解锁标志
	BYTE		dtDate[7];						// 时间 YYYYMMDDHHMMSS
	char		cOperatorID[6];					// 操作员代码(Login)
	BYTE		bBOMShfitID;					// BOM班次序号 每个BOM每天重置，第1个操作员登录为1，第2个操作员登录为2…	默认值：00
}TICKETLOCK, *PTICKETLOCK;

typedef struct	_DirectRefund {
	char		cTradeType[2];					// 交易类型
	BYTE		bStationID[2];					// 站点代码
	BYTE		bDevGroupType;					// 设备类型
	BYTE		bDeviceID[2];					// 设备代码
	char		cSAMID[16];						// SAM卡逻辑卡号
	long		lSAMTrSeqNo;					// SAM卡脱机交易流水号
	BYTE		bTicketType[2];					// 票卡类型
	char		cLogicalID[16];					// 票卡逻辑卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	BYTE		bStatus;						// 卡状态代码
	long		lBalanceReturned;				// 退卡内金额, 单位为分
	short		nDepositReturned;				// 退押金, 单位为分
	short		nForfeiture;					// 罚款, 单位为分
	BYTE		bForfeitReason;					// 罚款原因
	long		lTicketConsumeCount;			// 票卡扣款交易计数
	BYTE		bReturnTypeCode;				// 退款类型, 0－即时退款；1－非即时退款
	BYTE		dtDate[7];						// 时间 YYYYMMDDHHMMSS
	char		cReceiptID[4];					// 凭证ID
	BYTE		dtApplyDate[7];					// 申请日期时间, YYYYMMDDHHMMSS
	char		cMACOrTAC[10];					// 交易认证码, 羊城通：TAC(DTKi,())	地铁票卡：（待定）
	char		cOperatorID[6];					// 操作员代码(Login)
	BYTE		bBOMShfitID;					// BOM班次序号 每个BOM每天重置，第1个操作员登录为1，第2个操作员登录为2…	默认值：00
}DIRECTREFUND, *PDIRECTREFUND;

typedef struct _DelayRefundInput {
	int			nPort;							// 设备通信端口编号
	char		cOperatorID[6];					// 操作员代码(Login)
	char		cPassengerName[8];				// 乘客姓名
	char		cpassengerTel[12];				// 乘客电话
	BYTE		bCertificateType;				// 证件类型
	char		cCertificateNo[18];				// 证件号码
	BYTE		bBadFlag;						// 票卡是否折损
	BYTE		bBOMShfitID;					// BOM班次序号 每个BOM每天重置，第1个操作员登录为1，第2个操作员登录为2…	默认值：00
}DELAYREFUNDINPUT, *PDELAYREFUNDINPUT;

typedef struct _DelayRefundApply {
	char		cTradeType[2];					// 交易类型
	BYTE		bStationID[2];					// 站点代码
	BYTE		bDevGroupType;					// 设备类型
	BYTE		bDeviceID[2];					// 设备代码
	BYTE		bTicketType[2];					// 票卡类型
	char		cLogicalID[16];					// 票卡逻辑卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	char		cTicketPrintID[16];				// 票卡印刻号
	BYTE		dtDate[7];						// 时间 YYYYMMDDHHMMSS
	char		cReceiptID[4];					// 凭证ID
	char		cOperatorID[6];					// 操作员代码(Login)
	char		cPassengerName[8];				// 乘客姓名
	char		cpassengerTel[12];				// 乘客电话
	BYTE		bCertificateType;				// 证件类型
	char		cCertificateNo[18];				// 证件号码
	BYTE		bBadFlag;						// 票卡是否折损
	BYTE		bBOMShfitID;					// BOM班次序号 每个BOM每天重置，第1个操作员登录为1，第2个操作员登录为2…	默认值：00
}DELAYREFUNDAPPLY, *PDELAYREFUNDAPPLY;

typedef struct HistoryTranscat
{
	int 	nRecordLen ; 		//记录长度
	char	szData[256]; 		//数据内容，实际有效长度为RecordLen; 
}HISTORYTRANSCAT, *PHISTORYTRANSCAT;


typedef struct _BomTicketAnalyze 
{
	char		cPhysicalID[20];				// 物理卡号,
	char		cLogicalID[16];					// 逻辑卡号
	BYTE		bTicketType[2];					// 车票类型(主类型+子类型)
	long		lLoyalty;						// 卡积分
	long		lBalance;						// 余值
	long		lTripBlance;					// 乘次余额
	long		lCurrentCount;					// 当月剩余次数	
	long		lProximoCount;					// 下月可用次数
	long 		lDepositorCost;					// 押金
	long		lLimitedBalance;				// 车票最高上限值
	long		lForfeiture;					// 更新罚金，无进站码不使用(根据票价表查询)；
	BYTE		bIssueData[4];					// 发行时间
	BYTE		bExpiry[4];						// 物理有效期(BCD)
	BYTE		bStartDate[4];					// 起用开始时间
	BYTE		bEndDate[4];					// 起用结束时间
	WORD		wLineLimit;						// 线路限制

	BYTE		bStatus;						// 票卡状态

	BYTE 		bLastStatus; 					// 上次交易状态
	BYTE		bLastStationID[2];				// 上次进/出/发售地点
	BYTE		bLastDeviceID[2];				// 上次进/出/发售设备编号
	BYTE 		bLastDeviceType; 				// 上次操作设备类型
	BYTE		dtLastDate[7];					// 上次进/出/发售地点时间

	BYTE		bUpdateSationID[2];				// 上次更新站点
	BYTE		dtUpdateDate[7];				// 上次更新时间

	long		lUpdatePerDay;					// 车票日更新次数
	long		lUpdateTotal;					// 车票总更新次数

	long		lTripPerDay;					// 车票日乘次数
	BYTE		bActived;						// 老免激活（0-未激活，1-已激活）
	BYTE		bTCT;							// 是否使用乘次钱包(只对乘次票有效 0-未使用，1-已使用)

	BYTE		bCountTimes[2];					// 羊城通优惠门槛月份
	short 		nBusTimes;						// 羊城通公交门槛计数
	short  		nMtrTimes;						// 羊城通地铁门槛计数
	short   	nUnionTimes;					// 羊城通公交地铁联乘计数

	BYTE		dtDaliyActive[7];				// 多日票激活时间
	BYTE		bDaliyEffectDays;				// 多日票有效期天数
	BYTE		bLimitEntryID[2];				// 进站站点限制
	BYTE		bLimitExitID[2];				// 出站站点限制
	WORD		wLimitMode;						// 进出站限制模式
	long		lOverdraft;						// 透支额度
	char		cAreaCode[4];					// 区域代码

	DWORD		dwTickErrStauts;				// 票卡状态,票卡状态大致有以下几种，按位对应以下几种状态(从最低位向高位)1表示:
													// bit0:付费区出站超时
													// bit1:付费区超乘
													// bit2:付费区无进站码
													// bit3:非付费区有进站码
													// bit4:单程票回收
													// bit5:非付费区进站超时
													// bit6:正副公共区BCC错
													// bit7:黑名单卡(已被加锁)
													// bit8:票卡逻辑有效期过期
													// bit9:票卡物理过期
													// bit10:非付费区车票钱包低于最低票价
													// bit11:非付费区非本站进站码
													// bit12:已退款
													// bit13:激活过期
													// bit15:非本站更新或发售
													// bit16:非本日更新车票
													// bit17:羊城通证件有效期过期（票卡正常）
													// bit18:付费区无进站码，10分钟内本站出站成功
	DWORD		dwOperationStauts;				// 可操作状态, 可操作状态按位对应以下几种状态(从最低位向高位)1表示可操作
													// bit0:是否可充(值/乘次)
													// bit1:是否可更新
													// bit2:是否可发售
													// bit3:是否可激活
													// bit4:是否可延期
													// bit5:是否可退款
													// bit6:是否可解锁
													// bit7:允许乘次票余额转结
													// bit8:允许羊城通门槛值清零（对TVM无效）
}BOMTICKETANALYZE, *PBOMTICKETANALYZE;

typedef struct _hsSvt {
	BYTE	dtDate[7];			// 交易时间（BCD码）
	BYTE	bStationID[2];		// 交易车站ID号（BCD码）
	BYTE	bStatus;			// 票卡生命周期索引,具体定义见附录6
	long	lTradeAmount;		// 交易金额,单位分
	BYTE	bDeviceID[2];		// 设备编码（BCD）
	BYTE	bDeviceType;		// 设备类型
	char	cSAMID[16];			// sam id
}HSSVT, *PHSSVT;

typedef struct _SvtHistory
{
	char 	cPhysicalID[20]; 					// 物理卡号,
	char 	cLogicalID[16]; 					// 逻辑卡号
	BYTE 	bTicketType[2];						// 车票类型,车票主类型+车票子类型
	BYTE	bUsefulCount; 						// 票卡内有效的历史数据条数
	HSSVT	pHistory[17];						// 旧地铁历史记录
}SVTHISTORY, *PSVTHISTORY;

typedef struct _TicketInfo // modified by santa 2006-11-08 11:47
{
	char		cPhysicalID[20];				// 物理卡号
	char		cLogicalID[16];					// 逻辑卡号
	BYTE		bTicketType[2];					// 票卡类型
	long		lBalance;						// 余值
	long		lMCurMonTrip;					// 地铁本月剩余乘次
	long 		lMFutureTrip;					// 地铁下月剩余乘次
	long		lBCurMonTrip;					// 公交本月剩余乘次
	long 		lBFutureTrip;					// 公交下月剩余乘次
	BYTE		bExpiry[4];						// 有效期 (BCD)
	BYTE		bStatus;						// 状态 参见附录5
	BYTE		bLastStationID[2];				// 只限于在地铁使用时的车站ID（对于第一次使用为全＂FF＂）
	BYTE		dtLastDate[7];					// 最后一次操作时间(只针对地铁系统内的车票，从未在地铁使用的羊城通进入时填全FF)

	BYTE		bCountTimes[2];					// 羊城通优惠门槛月份
	short 		nBusTimes;						// 羊城通公交门槛计数
	short  		nMtrTimes;						// 羊城通地铁门槛计数
	short   	nUnionTimes;					// 羊城通公交地铁联乘计数

	BYTE		bUsefulCount;					// 有用的数据条数
	HSSVT		bhs[17];						// 历史记录,见各种票卡记录的格式

}TICKETINFO, *PTICKETINFO;

// 寄存器数据结构定义
typedef struct _Register{
	WORD	wIndex;
	DWORD	dwRegister;
}REGISTER, *PREGISTER;


//-------------------------------------------------------------------------
// ES 部分数据结构的定义
typedef struct _ESInit{
	char		cOperationType[2];				// 操作类型
	char		cOrderNo[14];					// 订单编号
	char		cTicketType[4];					// 票卡类型
	char		cApplicationNO[10];				// 申请编号,针对记名卡，默认为“0000000000
	char		cLogicalID[16];					// 票卡逻辑卡号
	char		cFaceID[16];					// 印刻卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	char		dtDate[14];						// 制票日期时间日期时间
	long		lBalance;						// 余额, 单位为分(默认为0)
	char		dtExpire[8];					// 有效期, YYYYMMDDHHMMSS， 
	char		cSAMID[16];						// E/S SAM逻辑卡号, 默认值:’0000000000’
	char		cLine[2];						// 用以限制预制票进闸时适用的线路范围。此字段仅对预制票有效，非预制票本字段默认为：00。
	char		cStationNo[2];					// 用以限制预制票进闸时适用的站点范围。此字段仅对预制票有效，非预制票本字段默认为：00。
	char		cTctDateStart[8];				// YYYYMMDD(指在此日期后开始有效)。	默认值：00000000，表示从制票开始有效
}ESINIT, *PESINIT;

typedef struct _ESAnalyze{
	BYTE		bStatus;						// 票卡状态
	char		cTicketType[4];					// 票卡类型
	char		cLogicalID[16];					// 票卡逻辑卡号
	char		cPhysicalID[20];				// 票卡物理卡号
	BYTE		bcharacter;						// 票卡物理类型 1：OCT；2：020；3：080；4：UL；5：FM；F:其他
	BYTE		bIssueStatus;					// 发行状态 0 – 已发行;1 – 未发行;2 – 发行失败	
	char		dtDate[14];						// 制票日期时间日期时间
	char		dtExpire[8];					// 有效期, YYYYMMDDHHMMSS， 
	char		cSAMID[16];						// E/S SAM逻辑卡号, 默认值:’0000000000’
	long		lBalance;						// 余额, 单位为分(默认为0)
	long 	    lDeposite;					    // 押金
	char		cLine[2];						// 此字段仅对预制票有效，用以限制预制票进闸时适用的线路范围。非预制票本字段默认为：00。
	char		cStationNo[2];			        // 用以限制预制票进闸时适用的站点范围。此字段仅对预制票有效，非预制票本字段默认为：00。
	char		cDateStart[8];			        // YYYYMMDD(指在此日期后开始有效)。	默认值：00000000，表示从制票开始有效
	char		cDateEnd[8];				    // YYYYMMDD(指在此日期后开始失效)。
}ESANALYZE, *PESANALYZE;


// 降级模式消息结构
typedef struct _DegradeCmd{
	BYTE		bDegradeType;					// 降级模式的类型
	BYTE		bTime[7];						// 降级模式开始/结束时间
	WORD		wStationID;						// 降级启用/结束的站点
	BYTE		bFlag;							// 启用/结束降级的标志 0x01 - ON   //	0x02 - OFF
}DEGRADECMD, *PDEGRADECMD;


#pragma pack( pop, before_include6)
