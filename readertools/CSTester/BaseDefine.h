//********************************************************************
// �ļ�����	: BaseDefine.h
// ����ʱ��	: 2011/06/02
// �޸�ʱ��	:
// �޸���	:
// �ļ�����	: ģ��ͷ�ļ�
// ����		: ����ģ�����ӿڽṹ
//********************************************************************

#pragma once
#pragma pack( push, before_include6,1 )

typedef struct _ReadStatus{
	BYTE		bReaderReadyStatus;				// ��д������״̬
	BYTE		bMetroSAMStatus;				// METRO_SAM��״̬(0x00-����, 0xFF-����)
	char		cMetroSAMID[16];				// METRO_SAM��ID
	long		lMetroSAMSVTBalance;			// METRO_SAM��SVTBalance
	long		lMetroSAMTripCountBalance;		// METRO_SAM��TripCountBalance
	long		lMetroSAMSJTBalance;			// METRO_SAM��SJTBalance
	BYTE		bOCTSAMStatus;					// OCT_SAM��״̬ �C (0x00-����, 0xFF-����)
	char		cOCTSAMID[16];					// OCT_SAM��ID
	long		lOCTSAMSVTBalance;				// OCT_SAM��SVTBalance
	long		lOCTSAMTripCountBalance;		// OCT_SAM��TripCountBalance
	long		lOCTSAMSJTBalance;				// OCT_SAM��SJTBalance
	BYTE		bNewSAMStatus;					// NEW_SAM��״̬ �C (0x00-����, 0xFF-����)
	char		cNewSAMID[16];					// NEW_SAM��ID
	long		lNewSAMSVTBalance;				// NEW_SAM��SVTBalance
	long		lNewSAMTripCountBalance;		// NEW_SAM��TripCountBalance
	long		lNewSAMSJTBalance;				// NEW_SAM��SJTBalance
	BYTE		bMobileStatus;					// Mobile��״̬ �C (0x00-����, 0xFF-����)
	char		cMobileSAMID[16];				// Mobile��ID
	long		lMobileSAMSVTBalance;			// Mobile��SVTBalance
	long		lMobileSAMTripCountBalance;		// Mobile��TripCountBalance
	long		lMobileSAMSJTBalance;			// Mobile��SJTBalance
}READSTATUS, *PREADSTATUS;

typedef struct _ReaderVersion{
	char		cAPIVersion[11];				// API�汾��Ϣ
	char		cFirmwareVersion[11];			// Firmware�汾��Ϣ
}READERVERSION, *PREADERVERSION;


typedef struct _SAMCardStatus{
	BYTE		bSAMStatus;						// SAM��״̬ �C(0x00��ʾ������0xFF-��ʾ���Ϻ�����������)
	char		cSAMID[16];						// SAM��ID��
	long		lSAMSVTBalance ;				// SAM��SVTBalance (�ǵ�������ֵΪ0)
	long		lSAMTripCountBalance;			// SAM��TripCountBalance (�ǵ�������ֵΪ0)
	long		lSAMSJTBalance;					// SAM��SJTBalance (�ǵ�������ֵΪ0)
}SAMCARDSTATUS, *PSAMCARDSTATUS;
	
typedef struct _MpSamIncrease
{
	char 		cSAMID[16];						// SAM���߼�����
	long		lBalance; 						// SAMǮ����ֵ
}MPSAMINCREASE, *PMPSAMINCREASE;


typedef struct _EntryGate{
	//-------------------------------------------------------------------------------
	// �Գ˿͵Ľ�վ��Ϣ���м�¼
	//-------------------------------------------------------------------------------
	char		cTradeType[2];					// ��������
	BYTE		bStationID[2];					// վ�����
	BYTE		bDevGroupType;					// �豸����
	BYTE		bDeviceID[2];					// �豸����
	char		cSAMID[16];						// SAM���߼�����
	long		lSAMTrSeqNo;					// SAM���ѻ�������ˮ��
	BYTE		bTicketType[2];					// Ʊ������
	char		cLogicalID[16];					// Ʊ���߼�����
	char		cPhysicalID[20];				// Ʊ��������
	BYTE		dtDate[7];						// ��վ����ʱ�� ��;20060211160903
	BYTE		bStatus;						// ��״̬����
	long		lBalance;						// ���

	BYTE		bCountTimes[2];					// ���ͨ�Ż��ż��·�
	short 		nBusTimes;						// ���ͨ�����ż�����
	short  		nMtrTimes;						// ���ͨ�����ż�����
	short   	nUnionTimes;					// ���ͨ�����������˼���

	long		lChargeCount;					// Ʊ����ֵ���׼���
	long		lTradeCount;					// Ʊ�����ѽ��׼���
	char		cTac[10];						// ������֤��
	char		cPayType[2];					// ֧������
	char		cLastSam[16];					// �ϴν��׵�SAM����
	BYTE		dtLast[7];						// �ϴν���ʱ��
	char		cTRSort[2];						// ���׷���
	char		cAreaCode[4];					// �������
	char		cAreaTKType[4];					// ��������
	long		lOverdraft;						// ͸֧���
	char		cCpuTK;							// Ʊ������
	char		cExpiredTK;						// ����Ʊ��
	short		nEffectDays;					// �˴�Ʊʹ������
	BYTE		dtActive[7];					// �˴�Ʊ����ʱ��
	char		cLimitMode[3];					// �˴�Ʊ����ģʽ
	BYTE		bLimitEntryID[2];				// ���ƽ�վվ��
	BYTE		bLimitExitID[2];				// ���Ƴ�վվ��

	long   		lRFU1;							// Ԥ���ֶ�1
	long 		lRFU2;							// Ԥ���ֶ�2
}ENTRYGATE, *PENTRYGATE;

typedef struct _PurseTrade{
	//-------------------------------------------------------------------------------
	// Ʊ��Ǯ�������仯����ֵ����ֵ��ʱ�������ļ�¼�����ݽ������Ϳɷ�Ϊ��
	// ������ѡ��˳����ѡ�Ǯ�����֡�Ǯ����ֵ��ר��Ǯ��Ȧ�浽��ҵǮ����
	// һ�����ѡ����򵥳�Ʊ��֧������
	//-------------------------------------------------------------------------------

	char		cTradeType[2];					// ��������
	BYTE		bStationID[2];					// վ�����	(BCD)
	BYTE		bDevGroupType;					// �豸����	(BCD) 
	BYTE		bDeviceID[2];					// �豸����	(BCD)
	char		cSAMID[16];						// ���ν���SAM�߼�����
	long		lSAMTrSeqNo;					// ���ν���SAM���ѻ�������ˮ��
	BYTE		dtDate[7];						// ����ʱ��	(BCD)
	BYTE		bTicketType[2];					// Ʊ������	(BCD)
	char		cLogicalID[16];					// Ʊ���߼�����
	char		cPhysicalID[20];				// Ʊ��������
	BYTE		bStatus;						// ��״̬����	(BCD)
	long		lTradeAmount;					// ���׽��, ��λΪ��
	long		lBalance;						// ���, ��λΪ��
	long		lTicketchargeCount;				// Ʊ����ֵ���׼���, ÿ�γ�ֵ�����ۼ�1
	long		lTicketMarkdownCount;			// Ʊ���ۿ�׼���, ÿ�οۿ�׼�1
	char		cPaymentType[2];				// ֧������
	char		cReceiptID[4];					// ƾ֤ID, 0001��9999	Ĭ��ֵ:0000
	char		cMACorTAC[10];					// ������֤��
	BYTE		bEntryStationID[2];				// ���վ�����	(BCD)
	char		cEntrySAMID[16];				// ���SAM�߼�����, Ĭ��ֵ:��0000000000�����ǳ˳�����ʱΪ�ϴ��豸��SAM���š�
	BYTE		dtEntryDate[7];					// ��վ����ʱ��, YYYYMMDDHHMMSS	Ĭ��ֵ:00000000000000		�ǳ˳�����ʱΪ�ϴ��豸����ʱ�䡣
	char		cOperatorID[6];					// ����Ա����(Login)
	BYTE		bBOMShiftID;					// BOM������
	char		cLastSAMID[16];					// �ϴν���SAM�߼�����, �ն��豸��SAM���߼����ţ�Ĭ��ֵΪ���ν����豸��š�
	BYTE		dtLastDate[7];					// �ϴν�������ʱ��, YYYYMMDDHHMMSS��Ĭ��ֵΪ���ν�������ʱ��

	long	 	lFare;							// ��λΪ�ֵĳ����ͣ���д����ѯ0100Ʊ�ֵ�Ʊ�۱�õ�	
	BYTE		bCountTimes[2];					// ���ͨ�Ż��ż��·�
	short 		nBusTimes;						// ���ͨ�����ż�����
	short  		nMtrTimes;						// ���ͨ�����ż�����
	short   	nUnionTimes;					// ���ͨ�����������˼���

	char		cTRSort[2];						// ���׷���
	char		cAreaCode[4];					// �������
	char		cAreaTKType[4];					// ��������
	long		lOverdraft;						// ͸֧���
	char		cCpuTK;							// Ʊ������
	char		cExpiredTK;						// ����Ʊ��
	short		nEffectDays;					// �˴�Ʊʹ������
	BYTE		dtActive[7];					// �˴�Ʊ����ʱ��
	char		cLimitMode[3];					// �˴�Ʊ����ģʽ
	BYTE		bLimitEntryID[2];				// ���ƽ�վվ��
	BYTE		bLimitExitID[2];				// ���Ƴ�վվ��

	long   		lRFU1;							// Ԥ���ֶ�1
	long 		lRFU2;							// Ԥ���ֶ�2

}PURSETRADE, *PPURSETRADE;

typedef struct _TvmSamIncreat{
	char		cSAMPacketID[8];				// SAM��Ǯ��ID
	long		lBalance;						// SAMǮ����ֵ
}TVMSAMINCREAT, *PTVMSAMINCREAT;

//�ǵ���Ʊ���ۼ�¼
typedef struct _OtherSaleRecord {
	char		cTradeType[2];					// ��������
	BYTE		bStationID[2];					// վ�����
	BYTE		bDevGroupType;					// �豸����
	BYTE		bDeviceID[2];					// �豸����
	BYTE		bTicketType[2];					// Ʊ������
	char		cLogicalID[16];					// Ʊ���߼�����
	char		cPhysicalID[20];				// Ʊ��������
	BYTE		bStatus;						// ��״̬����	
	char		cBusinessseqno[12];				// �ѻ�ҵ����ˮ��
	char		cSAMID[16];						// SAM���߼����ţ�����Ʊ��
	long		lSAMTrSeqNo;					// SAM���ѻ�������ˮ�ţ�����Ʊ��
	BYTE		bAmountType;					// �������BCD
	short		nAmount;						// ���
	BYTE		dtDate[7];						// ����ʱ��BCD
	char		cReceiptID[4];					// ƾ֤ID
	char		cOperatorID[6];					// ����Ա����(Login)
	BYTE		bBOMShfitID;					// BOM������ ÿ��BOMÿ�����ã���1������Ա��¼Ϊ1����2������Ա��¼Ϊ2��	Ĭ��ֵ��00	(BCD)
}OTHERSALERECORD, *POTHERSALERECORD;

//����Ʊ���ۼ�¼
typedef struct _SJTSaleRecord {
	char		cTradeType[2];					// ��������
	BYTE		bStationID[2];					// վ�����
	BYTE		bDevGroupType;					// �豸����
	BYTE		bDeviceID[2];					// �豸����
	char		cSAMID[16];						// SAM���߼����ţ�����Ʊ��
	long		lSAMTrSeqNo;					// SAM���ѻ�������ˮ�ţ�����Ʊ��
	BYTE		dtDate[7];						// ʱ��
	BYTE		bPaymentMeans;					// ֧����ʽ
	char		cPaymentTKLogicalID[16];		// ֧��Ʊ���߼�����
	long		lTicketMarkdownCount;			// Ʊ���ۿ�׼���, ÿ�οۿ���ۼ�1
	char		cLogicalID[16];					// TOKEN �߼�ID
	char		cPhysicalID[20];				// TOKEN ����ID
	BYTE		bStatus;						// TOKEN״̬
	short		nchargeValue;					// ��ֵ���
	BYTE		bTicketType[2];					// ����Ʊ������
	BYTE		bZoneID;						// ���δ���
	char		cMACorTAC[10];					// SAM���ɵĽ��׼�����,	Ĭ��ֵ��0000000000
	BYTE		bDepositorCost;					// �ɱ�/Ѻ��, 00��Ѻ��01���ɱ���02����ʹ�ã�Ѻ����ˣ��ɱ������ˣ�
	short		nAmountofDepositorCost;			// �ɱ�/Ѻ�� ���, ��λΪ�ֵ�������Ĭ��0
	char		cOperatorID[6];					// ����Ա����(Login)
	BYTE		bBOMShiftID;					// BOM������, ÿ��BOMÿ�����ã���1������Ա��¼Ϊ1����2������Ա��¼Ϊ2��	Ĭ��ֵ��00
}SJTSALERECORD, *PSJTSALERECORD;

typedef struct _TransactConfirm
{
	BYTE	bTradeType;						// ��������
	BYTE	bData[255];						// �ϴεĽ��׼�¼����
}TRANSACTCONFIRM, *PTRANSACTCONFIRM;

typedef struct _TvmSamIncrease{
	char		cSAMMainPacketID[8];
	long		lSAMPacketBalance;

}TVMSAMINCREASE, *PTVMSAMINCREASE;

typedef struct _TicketUpdate {
	char		cTradeType[2];					// ��������
	BYTE		bStationID[2];					// վ�����
	BYTE		bDevGroupType;					// �豸����
	BYTE		bDeviceID[2];					// �豸����
	char		cSAMID[16];						// SAM���߼�����
	long		lSAMTrSeqNo;					// �����豸SAM���ѻ�������ˮ��
	BYTE		bTicketType[2];					// Ʊ������
	char		cLogicalID[16];					// Ʊ���߼�����
	char		cPhysicalID[20];				// Ʊ��������
	long		lTicketMarkdownCount;			// Ʊ���ۿ�׼���
	BYTE		bStatus;						// ��״̬����
	BYTE		bUpdateArea;					// ��������1��������������2�����Ǹ�����
	BYTE		bUpdateReasonCode;				// ����ԭ��
												//	����ԭ������
												//		ԭ�����	������Description��
												//		������
												//		01 	��վ��ʱ
												//		02	����
												//		03	�޽�վ��
												//
												//		�Ǹ�����
												//		10	�н�վ��
												//		12	��վ��ʱ
	BYTE		dtUpdateDate[7];				// ��������ʱ�� YYYYMMDDHHMMSS
	BYTE		bPaymentMode;					// ֧����ʽ 1-�ֽ�//2-��ֵ��//3-���ͨ//4-Credit//5- civil charge ��Ĭ��ֵ��00��
	short		nForfeiture;					// ������, ����ɵķ�������ֽ�֧��������λΪ�ֵ�����, Ĭ�ϣ�0��
	char		cReceiptID[4];					// ƾ֤ID
	char		cOperatorID[6];					// ����Ա����(Login)
	BYTE		bEntryStationID[2];				// ���վ�����
	BYTE		bBOMShfitID;					// BOM������ ÿ��BOMÿ�����ã���1������Ա��¼Ϊ1����2������Ա��¼Ϊ2��	Ĭ��ֵ��00

	BYTE		bCountTimes[2];					// ���ͨ�Ż��ż��·�
	short 		nBusTimes;						// ���ͨ�����ż�����
	short  		nMtrTimes;						// ���ͨ�����ż�����
	short   	nUnionTimes;					// ���ͨ�����������˼���

	long		lBalance;						// ���
	long		lTokenFare;						// ԭƱ��
	long		lChargeCount;					// ��ֵ���׼���
	char		cTac[10];						// ������֤��
	char		cPayType[2];					// ֧������
	char		cEntrySam[16];					// ��վSAM����
	BYTE		dtEntry[7];						// ��վ����ʱ��
	char		cLastSam[16];					// �ϴν��׵�SAM����
	BYTE		dtLast[7];						// �ϴν���ʱ��
	char		cTRSort[2];						// ���׷���
	char		cAreaCode[4];					// �������
	char		cAreaTKType[4];					// ��������
	long		lOverdraft;						// ͸֧���
	char		cCpuTK;							// Ʊ������
	char		cExpiredTK;						// ����Ʊ��
	short		nEffectDays;					// �˴�Ʊʹ������
	BYTE		dtActive[7];					// �˴�Ʊ����ʱ��
	char		cLimitMode[3];					// �˴�Ʊ����ģʽ
	BYTE		bLimitEntryID[2];				// ���ƽ�վվ��
	BYTE		bLimitExitID[2];				// ���Ƴ�վվ��

	long   		lRFU1;							// Ԥ���ֶ�1
	long 		lRFU2;							// Ԥ���ֶ�2
}TICKETUPDATE, *PTICKETUPDATE;

typedef struct _TicketDefer {
	char		cTradeType[2];					// ��������
	BYTE		bStationID[2];					// վ�����
	BYTE		bDevGroupType;					// �豸����
	BYTE		bDeviceID[2];					// �豸����
	char		cSAMID[16];						// SAM���߼�����
	long		lSAMTrSeqNo;					// SAM���ѻ�������ˮ��
	BYTE		bTicketType[2];					// Ʊ������
	char		cLogicalID[16];					// Ʊ���߼�����
	char		cPhysicalID[20];				// Ʊ��������
	BYTE		bStatus;						// ��״̬����
	BYTE		dtOldExpiryDate[7];				// ԭ��Чʱ��
	BYTE		dtNewExpiryDate[7];				// ����Чʱ��
	BYTE		dtOperateDate[7];				// ����ʱ��
	char		cOperatorID[6];					// ����Ա����(Login)
	BYTE		bBOMShfitID;					// BOM������ ÿ��BOMÿ�����ã���1������Ա��¼Ϊ1����2������Ա��¼Ϊ2��	Ĭ��ֵ��00

}TICKETDEFER, *PTICKETDEFER;

typedef struct _TicketLock {	 	
	char		cTradeType[2];					// ��������
	BYTE		bStationID[2];					// վ�����
	BYTE		bDevGroupType;					// �豸����
	BYTE		bDeviceID[2];					// �豸����
	char		cSAMID[16];						// SAM���߼�����
	long		lSAMTrSeqNo;					// SAM���ѻ�������ˮ��
	BYTE		bTicketType[2];					// Ʊ������
	char		cLogicalID[16];					// Ʊ���߼�����
	char		cPhysicalID[20];				// Ʊ��������
	BYTE		bStatus;						// ��״̬����
	char		cLockFlag;						// �ӽ�����־
	BYTE		dtDate[7];						// ʱ�� YYYYMMDDHHMMSS
	char		cOperatorID[6];					// ����Ա����(Login)
	BYTE		bBOMShfitID;					// BOM������ ÿ��BOMÿ�����ã���1������Ա��¼Ϊ1����2������Ա��¼Ϊ2��	Ĭ��ֵ��00
}TICKETLOCK, *PTICKETLOCK;

typedef struct	_DirectRefund {
	char		cTradeType[2];					// ��������
	BYTE		bStationID[2];					// վ�����
	BYTE		bDevGroupType;					// �豸����
	BYTE		bDeviceID[2];					// �豸����
	char		cSAMID[16];						// SAM���߼�����
	long		lSAMTrSeqNo;					// SAM���ѻ�������ˮ��
	BYTE		bTicketType[2];					// Ʊ������
	char		cLogicalID[16];					// Ʊ���߼�����
	char		cPhysicalID[20];				// Ʊ��������
	BYTE		bStatus;						// ��״̬����
	long		lBalanceReturned;				// �˿��ڽ��, ��λΪ��
	short		nDepositReturned;				// ��Ѻ��, ��λΪ��
	short		nForfeiture;					// ����, ��λΪ��
	BYTE		bForfeitReason;					// ����ԭ��
	long		lTicketConsumeCount;			// Ʊ���ۿ�׼���
	BYTE		bReturnTypeCode;				// �˿�����, 0����ʱ�˿1���Ǽ�ʱ�˿�
	BYTE		dtDate[7];						// ʱ�� YYYYMMDDHHMMSS
	char		cReceiptID[4];					// ƾ֤ID
	BYTE		dtApplyDate[7];					// ��������ʱ��, YYYYMMDDHHMMSS
	char		cMACOrTAC[10];					// ������֤��, ���ͨ��TAC(DTKi,())	����Ʊ������������
	char		cOperatorID[6];					// ����Ա����(Login)
	BYTE		bBOMShfitID;					// BOM������ ÿ��BOMÿ�����ã���1������Ա��¼Ϊ1����2������Ա��¼Ϊ2��	Ĭ��ֵ��00
}DIRECTREFUND, *PDIRECTREFUND;

typedef struct _DelayRefundInput {
	int			nPort;							// �豸ͨ�Ŷ˿ڱ��
	char		cOperatorID[6];					// ����Ա����(Login)
	char		cPassengerName[8];				// �˿�����
	char		cpassengerTel[12];				// �˿͵绰
	BYTE		bCertificateType;				// ֤������
	char		cCertificateNo[18];				// ֤������
	BYTE		bBadFlag;						// Ʊ���Ƿ�����
	BYTE		bBOMShfitID;					// BOM������ ÿ��BOMÿ�����ã���1������Ա��¼Ϊ1����2������Ա��¼Ϊ2��	Ĭ��ֵ��00
}DELAYREFUNDINPUT, *PDELAYREFUNDINPUT;

typedef struct _DelayRefundApply {
	char		cTradeType[2];					// ��������
	BYTE		bStationID[2];					// վ�����
	BYTE		bDevGroupType;					// �豸����
	BYTE		bDeviceID[2];					// �豸����
	BYTE		bTicketType[2];					// Ʊ������
	char		cLogicalID[16];					// Ʊ���߼�����
	char		cPhysicalID[20];				// Ʊ��������
	char		cTicketPrintID[16];				// Ʊ��ӡ�̺�
	BYTE		dtDate[7];						// ʱ�� YYYYMMDDHHMMSS
	char		cReceiptID[4];					// ƾ֤ID
	char		cOperatorID[6];					// ����Ա����(Login)
	char		cPassengerName[8];				// �˿�����
	char		cpassengerTel[12];				// �˿͵绰
	BYTE		bCertificateType;				// ֤������
	char		cCertificateNo[18];				// ֤������
	BYTE		bBadFlag;						// Ʊ���Ƿ�����
	BYTE		bBOMShfitID;					// BOM������ ÿ��BOMÿ�����ã���1������Ա��¼Ϊ1����2������Ա��¼Ϊ2��	Ĭ��ֵ��00
}DELAYREFUNDAPPLY, *PDELAYREFUNDAPPLY;

typedef struct HistoryTranscat
{
	int 	nRecordLen ; 		//��¼����
	char	szData[256]; 		//�������ݣ�ʵ����Ч����ΪRecordLen; 
}HISTORYTRANSCAT, *PHISTORYTRANSCAT;


typedef struct _BomTicketAnalyze 
{
	char		cPhysicalID[20];				// ������,
	char		cLogicalID[16];					// �߼�����
	BYTE		bTicketType[2];					// ��Ʊ����(������+������)
	long		lLoyalty;						// ������
	long		lBalance;						// ��ֵ
	long		lTripBlance;					// �˴����
	long		lCurrentCount;					// ����ʣ�����	
	long		lProximoCount;					// ���¿��ô���
	long 		lDepositorCost;					// Ѻ��
	long		lLimitedBalance;				// ��Ʊ�������ֵ
	long		lForfeiture;					// ���·����޽�վ�벻ʹ��(����Ʊ�۱��ѯ)��
	BYTE		bIssueData[4];					// ����ʱ��
	BYTE		bExpiry[4];						// ������Ч��(BCD)
	BYTE		bStartDate[4];					// ���ÿ�ʼʱ��
	BYTE		bEndDate[4];					// ���ý���ʱ��
	WORD		wLineLimit;						// ��·����

	BYTE		bStatus;						// Ʊ��״̬

	BYTE 		bLastStatus; 					// �ϴν���״̬
	BYTE		bLastStationID[2];				// �ϴν�/��/���۵ص�
	BYTE		bLastDeviceID[2];				// �ϴν�/��/�����豸���
	BYTE 		bLastDeviceType; 				// �ϴβ����豸����
	BYTE		dtLastDate[7];					// �ϴν�/��/���۵ص�ʱ��

	BYTE		bUpdateSationID[2];				// �ϴθ���վ��
	BYTE		dtUpdateDate[7];				// �ϴθ���ʱ��

	long		lUpdatePerDay;					// ��Ʊ�ո��´���
	long		lUpdateTotal;					// ��Ʊ�ܸ��´���

	long		lTripPerDay;					// ��Ʊ�ճ˴���
	BYTE		bActived;						// ���⼤�0-δ���1-�Ѽ��
	BYTE		bTCT;							// �Ƿ�ʹ�ó˴�Ǯ��(ֻ�Գ˴�Ʊ��Ч 0-δʹ�ã�1-��ʹ��)

	BYTE		bCountTimes[2];					// ���ͨ�Ż��ż��·�
	short 		nBusTimes;						// ���ͨ�����ż�����
	short  		nMtrTimes;						// ���ͨ�����ż�����
	short   	nUnionTimes;					// ���ͨ�����������˼���

	BYTE		dtDaliyActive[7];				// ����Ʊ����ʱ��
	BYTE		bDaliyEffectDays;				// ����Ʊ��Ч������
	BYTE		bLimitEntryID[2];				// ��վվ������
	BYTE		bLimitExitID[2];				// ��վվ������
	WORD		wLimitMode;						// ����վ����ģʽ
	long		lOverdraft;						// ͸֧���
	char		cAreaCode[4];					// �������

	DWORD		dwTickErrStauts;				// Ʊ��״̬,Ʊ��״̬���������¼��֣���λ��Ӧ���¼���״̬(�����λ���λ)1��ʾ:
													// bit0:��������վ��ʱ
													// bit1:����������
													// bit2:�������޽�վ��
													// bit3:�Ǹ������н�վ��
													// bit4:����Ʊ����
													// bit5:�Ǹ�������վ��ʱ
													// bit6:����������BCC��
													// bit7:��������(�ѱ�����)
													// bit8:Ʊ���߼���Ч�ڹ���
													// bit9:Ʊ���������
													// bit10:�Ǹ�������ƱǮ���������Ʊ��
													// bit11:�Ǹ������Ǳ�վ��վ��
													// bit12:���˿�
													// bit13:�������
													// bit15:�Ǳ�վ���»���
													// bit16:�Ǳ��ո��³�Ʊ
													// bit17:���֤ͨ����Ч�ڹ��ڣ�Ʊ��������
													// bit18:�������޽�վ�룬10�����ڱ�վ��վ�ɹ�
	DWORD		dwOperationStauts;				// �ɲ���״̬, �ɲ���״̬��λ��Ӧ���¼���״̬(�����λ���λ)1��ʾ�ɲ���
													// bit0:�Ƿ�ɳ�(ֵ/�˴�)
													// bit1:�Ƿ�ɸ���
													// bit2:�Ƿ�ɷ���
													// bit3:�Ƿ�ɼ���
													// bit4:�Ƿ������
													// bit5:�Ƿ���˿�
													// bit6:�Ƿ�ɽ���
													// bit7:����˴�Ʊ���ת��
													// bit8:�������ͨ�ż�ֵ���㣨��TVM��Ч��
}BOMTICKETANALYZE, *PBOMTICKETANALYZE;

typedef struct _hsSvt {
	BYTE	dtDate[7];			// ����ʱ�䣨BCD�룩
	BYTE	bStationID[2];		// ���׳�վID�ţ�BCD�룩
	BYTE	bStatus;			// Ʊ��������������,���嶨�����¼6
	long	lTradeAmount;		// ���׽��,��λ��
	BYTE	bDeviceID[2];		// �豸���루BCD��
	BYTE	bDeviceType;		// �豸����
	char	cSAMID[16];			// sam id
}HSSVT, *PHSSVT;

typedef struct _SvtHistory
{
	char 	cPhysicalID[20]; 					// ������,
	char 	cLogicalID[16]; 					// �߼�����
	BYTE 	bTicketType[2];						// ��Ʊ����,��Ʊ������+��Ʊ������
	BYTE	bUsefulCount; 						// Ʊ������Ч����ʷ��������
	HSSVT	pHistory[17];						// �ɵ�����ʷ��¼
}SVTHISTORY, *PSVTHISTORY;

typedef struct _TicketInfo // modified by santa 2006-11-08 11:47
{
	char		cPhysicalID[20];				// ������
	char		cLogicalID[16];					// �߼�����
	BYTE		bTicketType[2];					// Ʊ������
	long		lBalance;						// ��ֵ
	long		lMCurMonTrip;					// ��������ʣ��˴�
	long 		lMFutureTrip;					// ��������ʣ��˴�
	long		lBCurMonTrip;					// ��������ʣ��˴�
	long 		lBFutureTrip;					// ��������ʣ��˴�
	BYTE		bExpiry[4];						// ��Ч�� (BCD)
	BYTE		bStatus;						// ״̬ �μ���¼5
	BYTE		bLastStationID[2];				// ֻ�����ڵ���ʹ��ʱ�ĳ�վID�����ڵ�һ��ʹ��Ϊȫ��FF����
	BYTE		dtLastDate[7];					// ���һ�β���ʱ��(ֻ��Ե���ϵͳ�ڵĳ�Ʊ����δ�ڵ���ʹ�õ����ͨ����ʱ��ȫFF)

	BYTE		bCountTimes[2];					// ���ͨ�Ż��ż��·�
	short 		nBusTimes;						// ���ͨ�����ż�����
	short  		nMtrTimes;						// ���ͨ�����ż�����
	short   	nUnionTimes;					// ���ͨ�����������˼���

	BYTE		bUsefulCount;					// ���õ���������
	HSSVT		bhs[17];						// ��ʷ��¼,������Ʊ����¼�ĸ�ʽ

}TICKETINFO, *PTICKETINFO;

// �Ĵ������ݽṹ����
typedef struct _Register{
	WORD	wIndex;
	DWORD	dwRegister;
}REGISTER, *PREGISTER;


//-------------------------------------------------------------------------
// ES �������ݽṹ�Ķ���
typedef struct _ESInit{
	char		cOperationType[2];				// ��������
	char		cOrderNo[14];					// �������
	char		cTicketType[4];					// Ʊ������
	char		cApplicationNO[10];				// ������,��Լ�������Ĭ��Ϊ��0000000000
	char		cLogicalID[16];					// Ʊ���߼�����
	char		cFaceID[16];					// ӡ�̿���
	char		cPhysicalID[20];				// Ʊ��������
	char		dtDate[14];						// ��Ʊ����ʱ������ʱ��
	long		lBalance;						// ���, ��λΪ��(Ĭ��Ϊ0)
	char		dtExpire[8];					// ��Ч��, YYYYMMDDHHMMSS�� 
	char		cSAMID[16];						// E/S SAM�߼�����, Ĭ��ֵ:��0000000000��
	char		cLine[2];						// ��������Ԥ��Ʊ��բʱ���õ���·��Χ�����ֶν���Ԥ��Ʊ��Ч����Ԥ��Ʊ���ֶ�Ĭ��Ϊ��00��
	char		cStationNo[2];					// ��������Ԥ��Ʊ��բʱ���õ�վ�㷶Χ�����ֶν���Ԥ��Ʊ��Ч����Ԥ��Ʊ���ֶ�Ĭ��Ϊ��00��
	char		cTctDateStart[8];				// YYYYMMDD(ָ�ڴ����ں�ʼ��Ч)��	Ĭ��ֵ��00000000����ʾ����Ʊ��ʼ��Ч
}ESINIT, *PESINIT;

typedef struct _ESAnalyze{
	BYTE		bStatus;						// Ʊ��״̬
	char		cTicketType[4];					// Ʊ������
	char		cLogicalID[16];					// Ʊ���߼�����
	char		cPhysicalID[20];				// Ʊ��������
	BYTE		bcharacter;						// Ʊ���������� 1��OCT��2��020��3��080��4��UL��5��FM��F:����
	BYTE		bIssueStatus;					// ����״̬ 0 �C �ѷ���;1 �C δ����;2 �C ����ʧ��	
	char		dtDate[14];						// ��Ʊ����ʱ������ʱ��
	char		dtExpire[8];					// ��Ч��, YYYYMMDDHHMMSS�� 
	char		cSAMID[16];						// E/S SAM�߼�����, Ĭ��ֵ:��0000000000��
	long		lBalance;						// ���, ��λΪ��(Ĭ��Ϊ0)
	long 	    lDeposite;					    // Ѻ��
	char		cLine[2];						// ���ֶν���Ԥ��Ʊ��Ч����������Ԥ��Ʊ��բʱ���õ���·��Χ����Ԥ��Ʊ���ֶ�Ĭ��Ϊ��00��
	char		cStationNo[2];			        // ��������Ԥ��Ʊ��բʱ���õ�վ�㷶Χ�����ֶν���Ԥ��Ʊ��Ч����Ԥ��Ʊ���ֶ�Ĭ��Ϊ��00��
	char		cDateStart[8];			        // YYYYMMDD(ָ�ڴ����ں�ʼ��Ч)��	Ĭ��ֵ��00000000����ʾ����Ʊ��ʼ��Ч
	char		cDateEnd[8];				    // YYYYMMDD(ָ�ڴ����ں�ʼʧЧ)��
}ESANALYZE, *PESANALYZE;


// ����ģʽ��Ϣ�ṹ
typedef struct _DegradeCmd{
	BYTE		bDegradeType;					// ����ģʽ������
	BYTE		bTime[7];						// ����ģʽ��ʼ/����ʱ��
	WORD		wStationID;						// ��������/������վ��
	BYTE		bFlag;							// ����/���������ı�־ 0x01 - ON   //	0x02 - OFF
}DEGRADECMD, *PDEGRADECMD;


#pragma pack( pop, before_include6)
