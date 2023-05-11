// Structs.h

#pragma once

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#pragma pack(push, 1)

// 2.21	������ʾ��Ϣ�ṹ
typedef struct {
	uint16_t	wErrCode;						// ������
	uint8_t		bNoticeCode;					// ������ʾ��
	uint8_t		bRfu[2];						// ���䱣���ֶ�
}RETINFO, * PRETINFO;

// 2.3	SAM����Ϣ��ѯ
typedef struct {
	uint8_t		bSAMStatus;						// SAM��״̬�C(0x00��ʾ����������-�����ֶ���Ч)
	char		cSAMID[16];						// SAM��ID��
}SAMSTATUS, * PSAMSTATUS;

// 2.1	��д����ʼ����Ϣ
typedef struct {
	uint8_t		bReaderReadyStatus;				// ��д������״̬
	SAMSTATUS  SamStatus[4];					// ��SAM��״̬��Ϣ���ݶ�4���ۣ��μ�// 2.3
}READERSTATUS, * PREADERSTATUS;

// 2.4	��բ����
typedef struct {
	char		cTradeType[2];					// ��������
	char		cStationID[4];					// ��·վ�����
	char		cDevType[2];					// �豸����
	char		cDeviceID[3];					// �豸����(BCD)����0�ֽڽ���4bit��Ч
	char		cSAMID[16];						// SAM���߼�����
	long		lSAMTrSeqNo;					// SAM���ѻ�������ˮ��
	uint8_t		bTicketType[2];					// Ʊ������
	char		cLogicalID[20];					// Ʊ���߼�����
	char		cPhysicalID[20];				// Ʊ��������
	uint8_t		dtDate[7];						// ��վ����ʱ����;20060211160903
	uint8_t		bStatus;						// ����״̬����
	long		lBalance;						// ���
	char		cTestFlag;						// ��Ӧ�ñ�ʶ
	char		cLimitMode[3];					// �˴�Ʊ����ģʽ
	uint8_t		bLimitEntryID[2];				// ���ƽ�վվ��
	uint8_t		bLimitExitID[2];				// ���Ƴ�վվ��
	char		cEntryMode;						// ��բ����ģʽ
	long		lTradeCount;					// ���Ѽ���
	char		cTkAppMode;						//��Ӧ��ģʽ �� '1'��CPU�� '2'��M1/UL, '3'����M1
}ENTRYGATE, * PENTRYGATE;

// 2.5	Ǯ������
typedef struct {
	char		cTradeType[2];					// ��������
	char		cStationID[4];					// ��·վ�����
	char		cDevType[2];					// �豸����
	char		cDeviceID[3];					// �豸����(BCD)����0�ֽڽ���4bit��Ч
	char		cSAMID[16];						// ���ν���SAM�߼�����
	long		lSAMTrSeqNo;					// ���ν���SAM���ѻ�������ˮ��
	uint8_t		dtDate[7];						// ����ʱ��(BCD)
	uint8_t		bTicketType[2];					// Ʊ������(BCD)
	char		cLogicalID[20];					// Ʊ���߼�����
	char		cPhysicalID[20];				// Ʊ��������
	uint8_t		bStatus;						// ����״̬����	(BCD)
	long		lTradeAmount;					// ���׽��, ��λΪ��
	long		lBalance;						// ���, ��λΪ��
	long		lChargeCount;					// ��ֵ����
	long		lTradeCount;					// ���Ѽ���
	char		cPaymentType[2];				// ֧������
	char		cReceiptID[4];					// ֧��ƾ֤��
	char		cMACorTAC[10];					// ������֤��
	uint8_t		bEntryStationID[2];				// ���վ�����(BCD)
	char		cEntrySAMID[16];				// ���SAM�߼�����
	uint8_t		dtEntryDate[7];					// ��վ����ʱ��(BCD)
	char		cOperatorID[6];					// ����Ա����(Login)
	uint8_t		bBOMShiftID;					// BOM������
	char		cSamLast[16];					// �ϴν��׵�SAM����
	uint8_t		dtLast[7];						// �ϴν�������ʱ��
	long		lTradeWallet;					// Ǯ�����׶��Ƭʵ����Ҫ�䶯��Ǯ��ֵ
	char		cTestFlag;						// ��Ӧ�ñ�ʶ
	char		cLimitMode[3];					// �˴�Ʊ����ģʽ
	uint8_t		bLimitEntryID[2];				// ���ƽ�վվ��
	uint8_t		bLimitExitID[2];				// ���Ƴ�վվ��
	char		cExitMode;						// ��բ����ģʽ
	char		cCityCode[4];					// ���д���
	char		cIndustryCode[4];				// ��ҵ����
	char		cClassicType[2];				// ���׷���
	char		cSamPosId[12];					// POS�ն˱��
	char		cTkAppMode;						//��Ӧ��ģʽ �� '1'��CPU�� '2'��M1/UL, '3'����M1
}PURSETRADE, * PPURSETRADE;

// 2.6	�ǵ���Ʊ���ۼ�¼
typedef struct {
	char		cTradeType[2];					// ��������
	char		cStationID[4];					// ��·վ�����
	char		cDevType[2];					// �豸����
	char		cDeviceID[3];					// �豸����(BCD)����0�ֽڽ���4bit��Ч
	uint8_t		bTicketType[2];					// Ʊ������
	char		cLogicalID[20];					// Ʊ���߼�����
	char		cPhysicalID[20];				// Ʊ��������
	uint8_t		bStatus;						// ����״̬����
	char		cBusinessseqno[12];				// �ѻ�ҵ����ˮ��
	char		cSAMID[16];						// SAM���߼����ţ�����Ʊ��
	long		lSAMTrSeqNo;					// SAM���ѻ�������ˮ�ţ�����Ʊ��
	uint8_t		bAmountType;					// �������
	short		nAmount;						// Ѻ��
	uint8_t		dtDate[7];						// ����ʱ��BCD
	char		cReceiptID[4];					// ֧��ƾ֤��
	char		cOperatorID[6];					// ����Ա����(Login)
	uint8_t		bBOMShfitID;					// BOM������ÿ��BOMÿ������	(BCD)
	long		lBrokerage;						// ������
	char		cTestFlag;						// ��Ӧ�ñ�ʶ
}OTHERSALE, * POTHERSALE;

// 2.7	����Ʊ���ۼ�¼
typedef struct {
	char		cTradeType[2];					// ��������
	char		cStationID[4];					// ��·վ�����
	char		cDevType[2];					// �豸����
	char		cDeviceID[3];					// �豸����(BCD)����0�ֽڽ���4bit��Ч
	char		cSAMID[16];						// SAM���߼����ţ�����Ʊ��
	long		lSAMTrSeqNo;					// SAM���ѻ�������ˮ�ţ�����Ʊ��
	uint8_t		dtDate[7];						// ʱ��
	uint8_t		bPaymentMeans;					// ֧����ʽ
	char		cPaymentTKLogicalID[20];		// ֧��Ʊ���߼�����
	long		lTradeCount;					// ���Ѽ���
	char		cLogicalID[20];					// TOKEN �߼�ID
	char		cPhysicalID[20];				// TOKEN ����ID
	uint8_t		bStatus;						// TOKEN״̬
	short		nChargeValue;					// ��ֵ���
	uint8_t		bTicketType[2];					// ����Ʊ������
	uint8_t		bZoneID;						// ���δ���
	char		cMACorTAC[10];					// SAM���ɵĽ��׼�����,	Ĭ��ֵ��
	uint8_t		bDepositorCost;					// �ɱ�Ѻ��(BCD)
	short		nAmountCost;					// �ɱ�Ѻ����
	char		cOperatorID[6];					// ����Ա����(Login)
	uint8_t		bBOMShiftID;					// BOM������, ÿ��BOMÿ������
	long		lBrokerage;						// ������
	char		cTestFlag;						// ��Ӧ�ñ�ʶ
	char		cClassicType[2];				// ���׷���
	char		cSamPosId[12];					// POS�ն˱��
}SJTSALE, * PSJTSALE;

// 2.8	����ȷ����Ϣ�ṹ
typedef struct {
	uint8_t	bTradeType;							// ��������
	uint8_t	bData[512];							// �ϴεĽ��׼�¼�ṹ�Ŀ���
}TRANSACTCONFIRM, * PTRANSACTCONFIRM;

// 2.9	���½���
typedef struct {
	char		cTradeType[2];					// ��������
	char		cStationID[4];					// ��·վ�����
	char		cDevType[2];					// �豸����
	char		cDeviceID[3];					// �豸����(BCD)����0�ֽڽ���4bit��Ч
	char		cSAMID[16];						// SAM���߼�����
	long		lSAMTrSeqNo;					// �����豸SAM���ѻ�������ˮ��
	uint8_t		bTicketType[2];					// Ʊ������
	char		cLogicalID[20];					// Ʊ���߼�����
	char		cPhysicalID[20];				// Ʊ��������
	long		lTradeCount;					// ���Ѽ���
	uint8_t		bStatus;						// ����״̬����
	char		cUpdateZone;					// ��������
	uint8_t		bUpdateReasonCode;				// ����ԭ��
												// ����ԭ������
												// ԭ�����	������Description��
												// ��������01����վ��ʱ��02-���ˣ�03-�޽�վ��
												// �Ǹ�������10���н�վ�룬12����վ��ʱ
	uint8_t		dtUpdateDate[7];				// ��������ʱ��YYYYMMDDHHMMSS
	uint8_t		bPaymentMode;					// ����֧����ʽ
												// 	1-�ֽ�2-��ֵ���� 3-һ��ͨ
												//	4-Credit�� 5- civil charge ��Ĭ��ֵ����
	short		nForfeiture;					// ����, ����ɵķ�����ֽ�֧��������λ��
	char		cReceiptID[4];					// ֧��ƾ֤��
	char		cOperatorID[6];					// ����Ա����(Login)
	uint8_t		bEntryStationID[2];				// ��վ��·վ�����
	uint8_t		bBOMShfitID;					// BOM������ÿ��BOMÿ������	��BCD��
	char		cTestFlag;						// ��Ӧ�ñ�ʶ
	char		cLimitMode[3];					// �˴�Ʊ����ģʽ
	uint8_t		bLimitEntryID[2];				// ���ƽ�վվ��
	uint8_t		bLimitExitID[2];				// ���Ƴ�վվ��
	char		cTkAppMode;						//��Ӧ��ģʽ �� '1'��CPU�� '2'��M1/UL, '3'����M1
}TICKETUPDATE, * PTICKETUPDATE;

// 2.10	Ʊ�����ڽ���
typedef struct {
	char		cTradeType[2];					// ��������
	char		cStationID[4];					// ��·վ�����
	char		cDevType[2];					// �豸����
	char		cDeviceID[3];					// �豸����(BCD)����0�ֽڽ���4bit��Ч
	char		cSAMID[16];						// SAM���߼�����
	long		lSAMTrSeqNo;					// SAM���ѻ�������ˮ��
	uint8_t		bTicketType[2];					// Ʊ������
	char		cLogicalID[20];					// Ʊ���߼�����
	char		cPhysicalID[20];				// Ʊ��������
	uint8_t		bStatus;						// ����״̬����
	uint8_t		dtOldExpiryDate[7];				// ԭ��Чʱ��
	uint8_t		dtNewExpiryDate[7];				// ����Чʱ��
	uint8_t		dtOperateDate[7];				// ����ʱ��
	char		cOperatorID[6];					// ����Ա����(Login)
	uint8_t		bBOMShfitID;					// BOM������ÿ��BOMÿ������	��BCD��
	char		cTestFlag;						// ��Ӧ�ñ�ʶ
}TICKETDEFER, * PTICKETDEFER;

// 2.11	Ʊ���ӽ�������
typedef struct {
	char		cTradeType[2];					// ��������
	char		cStationID[4];					// ��·վ�����
	char		cDevType[2];					// �豸����
	char		cDeviceID[3];					// �豸����(BCD)����0�ֽڽ���4bit��Ч
	char		cSAMID[16];						// SAM���߼�����
	long		lSAMTrSeqNo;					// SAM���ѻ�������ˮ��
	uint8_t		bTicketType[2];					// Ʊ������
	char		cLogicalID[20];					// Ʊ���߼�����
	char		cPhysicalID[20];				// Ʊ��������
	uint8_t		bStatus;						// ����״̬����
	char		cLockFlag;						// �ӽ�����־
	uint8_t		dtDate[7];						// ʱ��YYYYMMDDHHMMSS
	char		cOperatorID[6];					// ����Ա����(Login)
	uint8_t		bBOMShfitID;					// BOM������ÿ��BOMÿ������	(BCD)
	char		cTestFlag;						// ��Ӧ�ñ�ʶ
	char		cTkAppMode;						//��Ӧ��ģʽ �� '1'��CPU�� '2'��M1/UL, '3'����M1
}TICKETLOCK, * PTICKETLOCK;

// 2.12	�˿��
typedef struct {
	char		cTradeType[2];					// ��������
	char		cStationID[4];					// ��·վ�����
	char		cDevType[2];					// �豸����
	char		cDeviceID[3];					// �豸����(BCD)����0�ֽڽ���4bit��Ч
	char		cSAMID[16];						// SAM���߼�����
	long		lSAMTrSeqNo;					// SAM���ѻ�������ˮ��
	uint8_t		bTicketType[2];					// Ʊ������
	char		cLogicalID[20];					// Ʊ���߼�����
	char		cPhysicalID[20];				// Ʊ��������
	uint8_t		bStatus;						// ����״̬����
	long		lBalanceReturned;				// �˻�Ǯ�����, ��λΪ��
	short		nDepositReturned;				// �˻�Ѻ��, ��λΪ��
	short		nForfeiture;					// ����, ��λΪ��
	uint8_t		bForfeitReason;					// ����ԭ��
	long		lTradeCount;					// ���Ѽ���
	uint8_t		bReturnTypeCode;				// �˿�����, 0����ʱ�˿���Ǽ�ʱ�˿�
	uint8_t		dtDate[7];						// ʱ��YYYYMMDDHHMMSS
	char		cReceiptID[4];					// ƾ֤ID
	uint8_t		dtApplyDate[7];					// ��������ʱ��, YYYYMMDDHHMMSS
	char		cMACOrTAC[10];					// ������֤��
	char		cOperatorID[6];					// ����Ա����(Login)
	uint8_t		bBOMShfitID;					// BOM������ÿ��BOMÿ������	��BCD��
	long		lBrokerage;						// ������
	char		cTestFlag;						// ��Ӧ�ñ�ʶ
	char		cClassicType[2];				// ���׷���
	char		cSamPosId[12];					// POS�ն˱��
}DIRECTREFUND, * PDIRECTREFUND;

// 2.13	Ʊ���������ؽṹ
typedef struct {
	char		cPhysicalID[20];				// ������,
	char		cLogicalID[20];					// �߼�����
	uint8_t		bTicketType[2];					// ��Ʊ����(������+������)
	long		lBalance;						// ��ֵ
	long 		lDepositorCost;					// Ѻ��
	long		lLimitedBalance;				// ��Ʊ�������ֵ
	long		lPenalty;						// ���·���
	uint8_t		bIssueData[4];					// ����ʱ��
	uint8_t		bExpiry[7];						// ������Ч�ڽ�ֹʱ��(BCD)
	uint8_t		bStartDate[7];					// �߼���Ч�ڿ�ʼʱ��(BCD)
	uint8_t		bEndDate[7];					// �߼���Ч�ڽ���ʱ��(BCD)
	uint8_t		bStatus;						// ����״̬
	uint8_t		bLastStationID[2];				// �ϴν�����·վ��(BCD)
	uint8_t		bLastDeviceID[2];				// �ϴν����豸���(BCD),��0�ֽڽ���4bit��Ч
	uint8_t 	bLastDeviceType; 				// �ϴν����豸����
	uint8_t		dtLastDate[7];					// �ϴν���ʱ��
	uint8_t		bEntrySationID[2];				// �ϴν�վվ��
	uint8_t		dtEntryDate[7];					// �ϴν�վʱ��
	uint32_t	dwOperationStauts;				// �ɲ���״̬��ErrInfo�Ĵ����Ӧ
												// ��λ��Ӧ���¼��֣�1��ʾ�ɲ���
												// 	bit0:�Ƿ�ɳ�(ֵ/�˴�)
												// 	bit1:�Ƿ�ɸ���
												// 	bit2:�Ƿ�ɷ���
												// 	bit3:�Ƿ�ɼ���
												// 	bit4:�Ƿ������
												// 	bit5:�Ƿ���˿�
												// 	bit6:�Ƿ�ɽ���
	uint8_t		bLimitMode;						// ����ģʽ
	uint8_t		bLimitEntryID[2];				// ��վվ�����ƣ�BCD�룩
	uint8_t		bLimitExitID[2];				// ��վվ������(BCD��)
	uint8_t		bActiveStatus;					// ����״̬�򼤻���
	char		cCityCode[4];					// ���д���
	char		cSellerCode[4];					// �����̴���
	long		lChargeCount;					// ��ֵ����
	long		lTradeCount;					// ���Ѽ���
	uint8_t		bCertificateType;				// ֤������
	char		cCertificateCode[32];			// ֤������
	char		cCertificateName[20];			// ֤������������
	char		cTestFlag;						// ��Ӧ�ñ�ʶ
	long		lChargeUpper;					// ��ֵ���ޣ��ɳ�ֵ��־λ��0ʱ��Ч
	long 		lDepositeSale;					// ����Ѻ�𣬿ɷ��۱�־λ��0����Ч
	long		lBrokerage;						// �����ѣ�����������Ҫ�շ�ʱ��Ч����ǵ���Ʊ���ۡ��˿�
	uint8_t		bRfu[32];

}BOMANALYZE, * PBOMANALYZE;

// 2.14	��ֵƱ������ʷ�ṹ
typedef struct {
	uint8_t		dtDate[7];						// ����ʱ�䣨BCD�룩
	uint8_t		bStationID[2];					// ���׳�վID�ţ�BCD�룩
	uint8_t		bStatus;						// Ʊ��������������,���嶨�����¼��
	long		lTradeAmount;					// ���׽��,��λ��
	uint8_t		bDeviceID[2];					// �豸���루BCD��
	uint8_t		bDeviceType;					// �豸����
	char		cSAMID[16];						// sam id
}HSSVT, * PHSSVT;

// 2.15	����ֵƱ��ʷ��Ϣ�ṹ
typedef struct {
	char 		cPhysicalID[20]; 					// ������,
	char 		cLogicalID[20]; 					// �߼�����
	uint8_t 		bTicketType[2];						// ��Ʊ����,��Ʊ������+��Ʊ������
	uint8_t		bUsefulCount; 					// Ʊ������Ч����ʷ��������
	HSSVT		pHistory[17];						// �ɵ�����ʷ��¼���μ�// 2.16
}SVTHISTORY, * PSVTHISTORY;

// 2.16	TCM��Ʊ����Ϣ������Ϣ
typedef struct {
	char		cPhysicalID[20];				// ������,
	char		cLogicalID[20];					// �߼�����
	uint8_t		bTicketType[2];					// ��Ʊ����(������+������)
	long		lBalance;						// ��ֵ
	long 		lDepositorCost;					// Ѻ��
	long		lLimitedBalance;				// ��Ʊ�������ֵ
	uint8_t		bIssueData[4];					// ����ʱ��
	uint8_t		bExpiry[7];						// ������Ч�ڽ�ֹʱ��(BCD)
	uint8_t		bStartDate[7];					// �߼���Ч�ڿ�ʼʱ��(BCD)
	uint8_t		bEndDate[7];					// �߼���Ч�ڽ���ʱ��(BCD)
	uint8_t		bStatus;						// ����״̬
	uint8_t		bLastStationID[2];				// �ϴν�����·վ��(BCD)
	uint8_t		bLastDeviceID[2];				// �ϴν����豸���(BCD),��0�ֽڽ���4bit��Ч
	uint8_t 	bLastDeviceType; 				// �ϴν����豸����
	uint8_t		dtLastDate[7];					// �ϴν���ʱ��
	uint8_t		bEntrySationID[2];				// �ϴν�վվ��
	uint8_t		dtEntryDate[7];					// �ϴν�վʱ��
	uint8_t		bLimitMode;						// ����ģʽ
	uint8_t		bLimitEntryID[2];				// ��վվ�����ƣ�BCD�룩
	uint8_t		bLimitExitID[2];				// ��վվ������(BCD��)
	uint8_t		bActiveStatus;					// ����״̬�򼤻���
	char		cCityCode[4];					// ���д���
	char		cSellerCode[4];					// �����̴���
	long		lChargeCount;					// ��ֵ����
	long		lTradeCount;					// ���Ѽ���
	uint8_t		bCertificateType;				// ֤������
	char		cCertificateCode[32];			// ֤������
	char		cCertificateName[20];			// ֤������������
	char		cTestFlag;						// ��Ӧ�ñ�ʶ

	uint8_t		bUsefulCount;					// ���õ���������
	HSSVT	bhs[17];							// ��ʷ��¼���μ�// 2.16
}TICKETINFO, * PTICKETINFO;

// 2.17	ES����������Ϣ�ṹ
typedef struct {
	char		cOrderNo[14];  					// ������
	char		cApplicationNO[10];				// ������������
	uint16_t	wTicketType;					// ��Ʊ����
	uint32_t	dwSysFlow;   					// ϵͳ��ˮ��ֻ�Ե���Ʊ��Ч����ֵƱ��Ч��
	long		lDeposite;						// Ѻ��
	long		lValue;           				// ��ƱǮ��Ԥ�����
	uint8_t		dtStartDate[7];					// �߼���Ч�ڿ�ʼʱ��(BCD)
	uint8_t		dtEndDate[7];					// �߼���Ч�ڽ���ʱ��(BCD)
	uint8_t		dtExpire[7];					// ��Ʊ������Ч�ڽ�ֹʱ��(BCD)
}ORDERSIN, * PORDERSIN;

// 2.18	ES���������Ϣ�ṹ
typedef struct {
	char		cTradeType[2];					// ��������
	char		cOrderNo[14];				 	// �������
	uint8_t		bTicketType[2];					// Ʊ������
	char		cApplicationNO[10];				//������������
	char		cLogicalID[20];					// Ʊ���߼�����
	char		cPhysicalID[20];				// Ʊ��������
	uint8_t		dtDate[14];						// ��Ʊ����ʱ��
	long		lBalance;						// Ʊ�����, ��λΪ��(Ĭ��Ϊ)
	uint8_t		bStartDate[7];					// �߼���Ч�ڿ�ʼʱ��(BCD)
	uint8_t		bEndDate[7];					// �߼���Ч�ڽ���ʱ��(BCD)
	char		cSAMID[16];						// E/S SAM�߼�����, Ĭ��ֵ:����
}ORDERSOUT, * PORDERSOUT;

// 2.19	ESƱ���������ؽṹ
typedef struct {
	uint8_t		bStatus;						// ����״̬���μ���¼��
	uint8_t		bTicketType[2];					// Ʊ������
	char		cLogicalID[20];					// Ʊ���߼�����
	char		cPhysicalID[20];				// Ʊ��������
	uint8_t		bCharacter;						// Ʊ����������OCT,UL,FM,F:����
	uint8_t		bIssueStatus;					// ����״̬0 �Cδ����;1 �C�ѷ���;2 �Cע��
	char		dtIssueDate[7];					// ��Ʊ����ʱ������ʱ��
	char		dtExpire[7];					// ������Ч��, YYYYMMDDHHMMSS��
	long		lBalance;						// ���, ��λΪ��/��(Ĭ��Ϊ)
	long 		lDeposite;						// Ѻ��
}ESANALYZE, * PESANALYZE;

// 2.20	����ģʽ��Ϣ
typedef struct {
	uint8_t		bDegradeType;					// ����ģʽ������
	uint8_t		bTime[7];						// ����ģʽ��ʼ/����ʱ��
	uint16_t	wStationID;						// ��������/������վ��
	uint8_t		bFlag;							// 0x01-����/0x02-���������ı�־
}DEGRADECMD, * PDEGRADECMD;

// �Ĵ������ݽṹ����
typedef struct {
	uint16_t	wIndex;
	uint32_t	dwRegister;
}REGISTER, * PREGISTER;

typedef struct{
	char 		cMsgType[2];					// ��Ϣ���͡�55��
	uint8_t		dtTime[7];						// ʱ��
	char		cStation[4];					// ��·վ��
	char		cDevType[2];					// �豸����
	char		cDevId[3];						// �豸����
	char		cSamId[16];						// SAM����
	char		cFreedom[16];					// �����
}SAM_ACTIVE, * P_SAM_ACTIVE;

typedef struct{
	char 		cMsgType[2];					// ��Ϣ���͡�51��
	uint8_t		dtMsgTime[7];					// ��Ϣ����ʱ��
	char		cStation [4];					// վ��
	char		cDevType[2];					// �豸����
	char		cDevId[3];						// �豸����
	char		cSamId[16];						// SAM����
	long		lPosSeq;						// �ն˽������к�
	char		cNetPoint[16];					// �������
	char		cIssueCodeMain[4];				// ������������
	char 		cIssueCodeSub[4];				// �������ӱ���
	uint8_t		bTicketType[2];					// Ʊ������
	char		cLogicalId[20];					// Ʊ���߼�����
	char 		cPhysicalId[20];				// Ʊ��������
	char		cTestFlag;						// Ʊ�����ñ�ʶ
	long		lChargeCount;					// ����������ˮ
	long		lTradeCount;					// �ѻ�������ˮ
	char		cBusinessType[2];				// ҵ�����͡�14��
	char		cWalletType;					// ֵ����
	long		lAmount;						// ��ֵ���
	long		lBalcance;						// ���
	char		cMac[8];						// ��ֵ��֤��
	char		cFreedom[8];					// �����
	char		cLastPosID[16];					// �ϴν����ն˱��루վ�� �� �豸���� �� �豸���룩
	uint8_t		dtTimeLast[7];					// �ϴ�ʱ��
	char		cOperatorId[6];					// ����Ա����
}CHARGE_INIT, * P_CHARGE_INIT;

// ������ʼ��
typedef struct{
	char 		cMsgType[2];					// ��Ϣ���͡�53��
	uint8_t		dtMsgTime[7];					// ��Ϣ����ʱ��
	char		cStation [4];					// վ��
	char		cDevType[2];					// �豸����
	char		cDevId[3];						// �豸����
	char		cSamId[16];						// SAM����
	long		lPosSeq;						// �ն˽������к�
	char		cNetPoint[16];					// �������
	char		cIssueCodeMain[4];				// ������������
	char 		cIssueCodeSub[4];				// �������ӱ���
	uint8_t		bTicketType[2];					// Ʊ������
	char		cLogicalId[20];					// Ʊ���߼�����
	char 		cPhysicalId[20];				// Ʊ��������
	char		cTestFlag;						// Ʊ�����ñ�ʶ
	long		lChargeCount;					// ����������ˮ
	long		lTradeCount;					// �ѻ�������ˮ
	char		cBusinessType[2];				// ҵ�����͡�18��
	char		cWalletType;					// ֵ����
	long		lAmount;						// �������
	long		lBalcance;						// ���
	char		cLastPosID[16];					// �ϴν����ն˱��루վ�� �� �豸���� �� �豸���룩
	uint8_t		dtTimeLast[7];					// �ϴ�ʱ��
	char		cOperatorId[6];					// ����Ա����
}DESCIND_INIT, * P_DESCIND_INIT;

// ��д���汾��Ϣ
typedef struct READER_VERSION{
	uint8_t		verApi[2];						// APIʶ��汾
	uint8_t		verApiFile[5];					// API�ļ��汾�����ڣ����
	uint8_t		verRfDev;						// Rf����ʶ��汾
	uint8_t		verRfFile[5];					// Rf�ļ��汾�����ڣ����
	uint8_t		verSamDev;						// Sam����ʶ��汾
	uint8_t		verSamFile[5];					// Sam�����ļ��汾�����ڣ����
}READERVERSION, * PREADERVERSION;

#pragma pack(pop)
