// Structs.h

#pragma once

#include "Declares.h"
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

// 4.1	��д����ʼ����Ϣ
typedef struct {
	uint8_t		bReaderReadyStatus;				// ��д������״̬
	SAMSTATUS  SamStatus[4];					// ��SAM��״̬��Ϣ���ݶ�4���ۣ��μ�// 2.3
}READERSTATUS, * PREADERSTATUS;

// 4.4	��բ����
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
	long		lTradeCount;					// Ʊ���ۿ�׼���
	char		cTkAppMode;						//��Ӧ��ģʽ �� '1'��CPU�� '2'��M1/UL, '3'����M1
}ENTRYGATE, * PENTRYGATE;

// 4.5	Ǯ������
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
	long		lChargeCount;					// Ʊ����ֵ����
	long		lTradeCount;					// Ʊ���ۿ�׼���
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
	char		cSamPosId[12];					// SAM���ն˱���
	char		cTkAppMode;						// ��Ӧ��ģʽ �� '1'��CPU�� '2'��M1/UL, '3'����M1
	char        cRecordVer[2];                  // ��¼�汾����ʱĬ��Ϊ11
	char		cIssueCardCode[16];             // ������������
	char		cKeyVer[2];                     // ��Կ�汾��
	char		cKeyIndex[2];                   // ��Կ����
	char		cRandom[8];                     // α�����
	char		cAlgFlag[2];                    // �㷨��ʶ
	char		cCardHolderName[40];			// �ֿ�������
	char		cCertificateType[2];			// ֤������
	char		cCertificateCode[30];			// ֤������
	char		cOriginalCardType[8];			// ԭ��Ʊ������
	char		cCardLocation[4];               // ������
	uint8_t     cPayType;				        // ֧����ʽ�����ֽ�΢��֧������

}PURSETRADE, * PPURSETRADE;

//4.6	�ǵ���Ʊ���ۼ�¼
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

// 4.7	����Ʊ���ۼ�¼
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
	long		lTradeCount;					// ����Ʊ�ۿ����
	char		cLogicalID[20];					// TOKEN �߼�ID
	char		cPhysicalID[20];				// TOKEN ����ID
	uint8_t		bStatus;						// TOKEN״̬
	short		nChargeValue;					// ��ֵ���
	uint8_t		bTicketType[2];					// ����Ʊ������
	uint8_t		bZoneID;						// ���δ���
	char		cMACorTAC[10];					// ������֤��
	uint8_t		bDepositorCost;					// �ɱ�Ѻ��(BCD)
	short		nAmountCost;					// �ɱ�Ѻ����
	char		cOperatorID[6];					// ����Ա����(Login)
	uint8_t		bBOMShiftID;					// BOM������, ÿ��BOMÿ������
	long		lBrokerage;						// ������
	char		cTestFlag;						// ��Ӧ�ñ�ʶ
	char		cClassicType[2];				// ���׷���
	char		cSamPosId[12];					// SAM���ն˱���
}SJTSALE, * PSJTSALE;

// 4.8	����ȷ����Ϣ�ṹ
typedef struct {
	uint8_t	bTradeType;							// ��������
	uint8_t	bData[512];							// �ϴεĽ��׼�¼�ṹ�Ŀ���
}TRANSACTCONFIRM, * PTRANSACTCONFIRM;

// 4.9	���½���
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
	long		lTradeCount;					// Ʊ���ۿ�׼���
	uint8_t		bStatus;						// ����״̬����
	char		cUpdateZone;					// ��������
	uint8_t		bUpdateReasonCode;				// ����ԭ��
												// ����ԭ������
												// ԭ�����	������Description��
												// ��������01����վ��ʱ��02-���ˣ�03-�޽�վ��
												// �Ǹ�������10���н�վ�룬12����վ��ʱ
	uint8_t		dtUpdateDate[7];				// ��������ʱ��YYYYMMDDHHMMSS
	uint8_t		bPaymentMode;					// ����֧����ʽ
												// 	1-�ֽ�2-��ֵ���� Ĭ��ֵ-0
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

// 4.10	Ʊ�����ڽ���
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

// 4.11	Ʊ���ӽ�������
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

// 4.12	�˿��
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
	long		lTradeCount;					// Ʊ���ۿ�׼���
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
	char		cSamPosId[12];					// SAM���ն˱���
}DIRECTREFUND, * PDIRECTREFUND;

// 4.13	Ʊ���������ؽṹ
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
												//  bit7:�Ƿ����Ѻ��
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

	char		cTkAppMode;						// ��Ӧ��ģʽ �� '1'��CPU�� '2'��M1/UL, '3'����M1
	uint8_t		bCertExpire[4];					// ֤����Ч�ڣ�Ŀǰ��Ҫ���ڹ���ѧ��Ʊ��������Ч�ڹ���ʱ��ѧ��Ʊ����ͨƱ�۷�
	uint16_t	wError;							// �����룬��Ӧ���·���lPenalty
	long		lPenalty1;						// ���·���lPenalty1
	uint16_t	wError1;						// ������1����Ӧ���·���1

	uint8_t		bRfu[19];

}BOMANALYZE, * PBOMANALYZE;

// 4.21	���ӳ�Ʊ�������ؽṹ
typedef struct {
	char		cIssueCode[4];					// ���뷽��ʶ
	char		cVersion[2];					// �汾�ţ��˰汾��Ϊ�ṹ�汾�ţ�ҲΪ���а汾��
	char		cCardStatus;					// ����Ʊ��״̬
	char		cTicketLogicalNo[20];			// �߼�����
	char		cTicketSubType[2];				// ��Ʊ������
	long		lBalance;						// ��ֵ
	long		lTicketCount;					// ������
	char		cCenterCode[32];				// ����Ʊ��

	char 		cSaleDate[14];					// ��Ʊʱ��
	char		cValidDate[8];					// ��Ч��

	char		cStartStationId[4];				// ��ʼվ����
	char		cEndStationId[4];				// �յ�վ����
	char		cTicketNum[2];					// ����
	char		cEntryDate[14];					// ��վʱ��
	char        cEntryStationId[4];				// ��վվ��
	char		cExitDate[14];					// ��վʱ��
	char        cExitStationId[4];				// ��վվ��

	char        cValidExitDate[14];				// ��Ч��վʱ��
	char		cLastDealType[2];				// ĩ�ν�������
	char		cLastDealDate[14];				// ĩ�ν���ʱ��

	char		cPenaltyType[2];				// ��������
	long		lPenalty1;						// ������  ��ʱ����
	long		lPenalty2;						// ���·���  ���˸��²�Ʊ


}BOMETICANALYZE, * PBOMETICANALYZE;

// 4.14	��ֵƱ������ʷ�ṹ
typedef struct {
	uint8_t		dtDate[7];						// ����ʱ�䣨BCD�룩
	uint8_t		bStationID[2];					// ���׳�վID�ţ�BCD�룩
	uint8_t		bStatus;						// Ʊ��������������,���嶨�����¼��
	long		lTradeAmount;					// ���׽��,��λ��
	uint8_t		bDeviceID[2];					// �豸���루BCD��
	uint8_t		bDeviceType;					// �豸����
	char		cSAMID[16];						// sam id
}HSSVT, * PHSSVT;

// 4.16	TCM��Ʊ����Ϣ������Ϣ
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
	char		cTkAppMode;						// ��Ӧ��ģʽ �� '1'��CPU�� '2'��M1/UL, '3'����M1
	uint8_t		bCertExpire[4];					// ֤����Ч�ڣ�Ŀǰ��Ҫ���ڹ���ѧ��Ʊ��������Ч�ڹ���ʱ��ѧ��Ʊ����ͨƱ�۷�
	HSSVT		bhs[17];						// ��ʷ��¼���μ�// 2.16
}TICKETINFO, * PTICKETINFO;


// 4.18	������ȡƱ������Ϣ����
typedef struct {
	char		cAmount[2];						// ����Ʊ����
	char		cSum[8];						// �ܽ��
	char		cCenterCode[32];				// Ʊ��
	char		cTicketLogicalNo[20];			// Ʊ���߼���
	char        cStartStation[4];				// ��ʼվ�����
	char        cTerminalStation[4];			// �յ�վ����

}GETETICKETRESPINFO, * PGETETICKETRESPINFO;


// 4.19	���ӳ�Ʊ������Ϣ�ṹ
typedef struct {
	char		cTxnDate[8];					// ��Ӫ����
	char		cTransCode[2];					// ����״̬
	char		cTicketMainType[2];				// ��Ʊ������
	char		cTicketSubType[2];				// ��Ʊ������
	char		cChipType[2];					// Ʊ��оƬ����
	char		cTicketLogicalNo[20];			// �߼�����
	char		cCenterCode[32];				// ����Ʊ��
	char		cCenterMac[8];					// ����У����
	char		cRealTicketNo[16];				// ʵ�忨��
	char		cMark[2];						// ״̬��
	char		cTicketCSN[16];					// Ʊ��CSN
	char		cTicketCount[10];				// Ʊ������
	char		cPsamNo[16];					// PSAM����
	char		cPsamSeq[10];					// PSAM����ˮ��
	char		cTxnTms[14];					// ��������ʱ��
	char		cValidOutTms[14];				// ��Ч��վʱ��
	char		cPayType[2];					// ֧����ʽ
	char		cBeforeTxnBalance[10];			// ����ǰƱֵ
	char		cTxnAmountNo[6];				// ���״���
	char		cTxnAmount[6];					// ���׽��
	char		cDiscountAmt[6];				// �Żݽ��
	char		cTxnStation[4];					// ��ǰ��վ
	char		cLastStation[4];				// �ϴ�ʹ�ó�վ
	char		cLastTxnTms[14];				// �ϴ�ʹ��ʱ��
	char		cDevNodeId[9];					// �ն��豸��ʶ
	char		cDevSeqNo[10];					// �ն��豸��ˮ��
	char		cTestMark[2];					// ���Խ��ױ�־
	char		cQRSerialNo[8];					// ��ά�복ƱOD���к�
	char		cOverAmount[6];					// բ����ʱ�ۿ���
	char		cTac[8];					    // TAC
	char		cNetTranNumber[64];				// ������������
	char		cReserve[32];					// ������

}ETICKETDEALINFO, * PETICKETDEALINFO;

// 4.20	���ӳ�Ʊ������֤��Ϣ�ṹ
typedef struct {
    char		cTransCode[2];					// ����״̬
    char		cTicketMainType[2];				// ��Ʊ������
    char		cTicketSubType[2];				// ��Ʊ������
    char		cTicketLogicalNo[20];			// �߼�����
    char		cCenterCode[32];				// ����Ʊ��
    char		cCenterMac[8];					// ����У����
    char		cQrNetStatus;				    // ����Ʊ��ǰ����״̬��ʶ  1.�������� 2.�����ж�
}ETICKETAUTHINFO, * PETICKETAUTHINFO;

// 4.20	����ģʽ��Ϣ
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

// ��ֵ��ʼ��
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
typedef struct {
	uint8_t		verApi[2];						// APIʶ��汾
	uint8_t		verApiFile[5];					// API�ļ��汾�����ڣ����
	uint8_t		verRfDev;						// Rf����ʶ��汾
	uint8_t		verRfFile[5];					// Rf�ļ��汾�����ڣ����
	uint8_t		verSamDev;						// Sam����ʶ��汾
	uint8_t		verSamFile[5];					// Sam�����ļ��汾�����ڣ����
}READERVERSION, * PREADERVERSION;


// �����ļ���������
typedef struct
{
	uint16_t	trade_save_days;
	uint16_t	log_save_days;
	int			log_level;
}LOCCONFIG, * PLOCCONFIG;


// ����ȷ����Ҫ�������Ϣ�ṹ
typedef struct
{
	int					operType;				// ��������
	long				balance;				// ���׺�����
	long				charge_counter;			// CPU��ʹ��
	long				consume_counter;		// CPU��ʹ��
	union										// ����
	{
		SJTSALE			sjtSale;				// ����Ʊ����
		OTHERSALE		svtSale;				// ��ֵƱ����
		ENTRYGATE		entry;					// ��բ
		PURSETRADE		purse;					// Ǯ������
		TICKETDEFER		deffer;					// ����
		TICKETUPDATE	update;					// ����
		DIRECTREFUND	refund;					// �˿�
	}trade;
	uint32_t			size_trade;				// ���״�С
	uint8_t				dataWritten[512];		// �ϴ�д������
	uint32_t			sizeWritten;			// д���ݵĴ�С

}CONFIRM_POINT;

// ����ȷ�����ӣ��ӱ�����ϴν��׽ṹ�л�ȡ
typedef struct
{
	char		logic_id[21];
	uint8_t		trade_time[7];
	long 		trade_amount;

	char		device_inf[9];
	uint8_t		tac_type;
}CONFIRM_FACTOR;

// ����CPU����ؽṹ������ȷ��ʹ��
typedef struct
{
	uint8_t flag_trade_assist;
	uint8_t w_trade_assist[LENM_TRADE_ASSIST];			// ���׸��������ϼ�¼�ļ���
	uint8_t flag_metro;
	uint8_t w_metro[LENM_METRO];						// �����ͨ�����ϼ�¼�ļ���
	uint8_t flag_ctrl_record;
	uint8_t w_ctrl_record[LENM_CTRL_RECORD];			// Ӧ�ø�������Ӧ�ÿ��Ƽ�¼�����ϼ�¼�ļ���
	uint8_t flag_app_ctrl;
	uint8_t w_app_ctrl[LENM_APP_CTRL];					// Ӧ�ÿ����ļ�

}METRO_CPU_INF_W, * P_METRO_CPU_INF_W;

// ����CPU����ؽṹ������ȷ��ʹ��
typedef struct
{
	uint8_t flag_metro;
	uint8_t w_metro[LENB_METRO];						// �����ͨ�����ϼ�¼�ļ���

}BUS_CPU_INF_W, * P_BUS_CPU_INF_W;


// ����Ʊ����ȷ����Ҫ�������Ϣ�ṹ
typedef struct
{
	int			operType;						// ��������
	char		cTxnTms[14+1];					// ��������ʱ��
	char		cTxnAmount[6+1];				// ���׽��
	char		cCenterCode[32+1];				// ����Ʊ��
	char		cVervifyCode[8];				// ����У����
	ETICKETDEALINFO eticketdealinfo;

}CONFIRM_POINT_E;

// ������Ϣ�ṹ�������ǽ�վ����վ�����µ�
typedef struct
{
    uint16_t		station_id;				// վ��
    ETPDVC			dvc_type;				// �豸����
    uint16_t		dvc_id;					// �豸����
    uint8_t			time[7];				// ����ʱ��
    char			sam[17];				// SAM����
} TradeInfo, * PTradeInfo;


typedef struct
{
    long	balance;			//���
    char	cardNo[20+1];		//�߼�����
    char    centerCode[32+1];   //Ʊ��
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
    char	ticketVersion[2+1];			//�汾��
    char	subType[2+1];				//������
    char    cardNo[20+1];				//����
    long    balance;					//���
    long    counter;					//������
    char	centerCode[32+1];			//Ʊ��
    char	mac[8+1];                   //����У����
    char	saleTime[14+1];				//��Ʊʱ��
    char	validDate[8+1];				//��Ч��
    char	startStation[4+1];			//��ʼվ����,���վվ��
    char    entryTime[14+1];            //��վʱ��
    char	terminalStation[4+1];		//�յ�վ����
    long    price;						//����
    char    amount[2+1];				//����
    long	sum;						//�ܼ�
    char	vervifyCode[8+1];			//��֤��
    char    applyTime[14+1];            //��ǰ��ά������ʱ��
    char	entryDeviceType[2+1];       //ʵ�ʽ�վ�豸����
    char	entryDeviceCode[2+1];       //ʵ�ʽ�վ�豸���
    char    validOutTime[14+1];         //��Ч��վʱ��
    char    NetTranNumber[64+1];        //������������
    long	TxnAmount;                  //���׽��
    char    exitTime[14+1];				//��վʱ��
    char    exitStation[4+1];			//��վ����
    char	exitDeviceType[2+1];        //ʵ�ʳ�վ�豸����
    char	exitDeviceCode[2+1];        //ʵ�ʳ�վ�豸���
    char	cPenaltyType[2];			//��������
    long	lPenalty1;					//�����ʱ
    long	lPenalty2;					//���·��𳬳�
	char    lastTradeTime[14];			//�ϴν���ʱ��
	char	lasttredeType[2];			//�ϴν�������
	char 	centerCodeProductTime[14+1];//��ǰƱ������ʱ��
	char 	retainField[10+1];			//������ά�뱣����
}QRTicketData,* PQRTicketData;

// ��ͨ��������Ϣ�ṹ�������ǽ�վ����վ�����µ�
typedef struct
{
    char            city_code[4];           // ���д���
    uint8_t         institutionCode[8];		// ������ʶ
    uint16_t		station_id;				// վ��
    uint8_t			time[7];				// ����ʱ��
    char			sam[17];				// SAM����-�ն˱���
    uint8_t         line_id;                // ��·��
    long            deal_fee;               // ���׽��
    long            balance;                // ��վǮ�����
} JTBTradeInfo, * PJTBTradeInfo;

// �����ݽṹ
typedef struct
{
    ETPMDM			physical_type;			// ��������
    char			issue_code[4];			// ���з�������
    char			city_code[4];			// ���д���
    char			industry_code[4];		// ��ҵ����
    char			physical_id[21];		// ������
    char			logical_id[21];			// �߼�����
    uint8_t			key_flag;				// Ӧ�ñ�ʶ����ʽƱ������Ʊ��

    uint8_t			date_issue[4];			// ����ʱ��
    uint8_t			phy_peroidE[4];			// ������Ч��
    uint8_t			logical_peroidS[7];		// �߼���Ч��
    uint8_t			logical_peroidE[7];
    int				effect_mins;			// ��Ч������������ʱд�뷢�ۺ����Ч��ʱ�䣬����ʱ���Ͽ�����ʱ�䣩

    char			certificate_name[20];	// ֤������������
    char			certificate_code[32];	// ֤������
    uint8_t			certificate_type;		// ֤������

    uint8_t			cert_peroidE[4];		// ֤����Ч��
    uint8_t			charge_peroidS[4];		// ��ֵ��Ч��
    uint8_t			charge_peroidE[4];
    uint8_t			active_peroidS[7];		// ������Ч��
    uint8_t			active_peroidE[7];

    uint8_t			logical_type[2];		// �߼�Ʊ������
    uint8_t			fare_type[2];			// Ʊ������
    long			wallet_value;			// Ǯ����ֵ
    long			deposit;				// Ѻ��
    long			wallet_value_max;		// Ǯ������
    uint8_t			list_falg;				// ������ʶ
    uint16_t		charge_counter;			// ��ֵ�ۼ�
    long			daliy_trade_counter;	// �ս��״���
    long			total_trade_counter;	// �ܽ��״���
    long			daliy_update_counter;	// �ո��´���
    long			total_update_counter;	// �ܸ��´���
    uint8_t			app_lock_flag;			// Ӧ��������ǣ����ۻ����Ʊ����ʱʹ�ã�
    uint8_t			globle_status;			// ȫ��״̬(��ʼ������ʹ�ã��˿)
    uint8_t			init_status;			// ԭʼ״̬
    SimpleStatus	simple_status;			// ת��Ϊ��׼��Ʊ��״̬

    uint8_t			limit_mode;				// ���ƽ���ģʽ
    uint8_t			limit_entry_station[2];	// ���ƽ�վվ��
    uint8_t			limit_exit_station[2];	// ���Ƴ�վվ��

    PTradeInfo		p_entry_info;			// ��վ��Ϣ
    PTradeInfo		p_update_info;			// ������Ϣ
    PTradeInfo		p_exit_info;			// ��վ��Ϣ
    TradeInfo		read_last_info;			// �ϴν�����Ϣ����ֵ��read_last_info��д����write_last_info
    TradeInfo		write_last_info;

    PJTBTradeInfo		p_jtb_entry_info;	// ��ͨ��Ʊ����վ��Ϣ
    PJTBTradeInfo		p_jtb_update_info;	// ��ͨ��Ʊ��������Ϣ
    PJTBTradeInfo		p_jtb_exit_info;	// ��ͨ��Ʊ����վ��Ϣ
    JTBTradeInfo		jtb_read_last_info;	// �ϴν�����Ϣ����ֵ��read_last_info��д����write_last_info
    JTBTradeInfo		jtb_write_last_info;

    char			ticket_app_mode;		// ��Ӧ��ģʽ

    //17������Ϣ�ļ�
    char			international_code[8+1];	// ���ʴ���
    char			province_code[4+1];		    // ʡ������
    uint8_t			city_code_17[2];		    // ���д���
    uint8_t			interCardFlag[2];		    // ��ͨ����
    uint8_t			jtbCardType;		        // ��������

    // ����Ʊ�����Ϣ
    //short			limit_days;				// �˴�Ʊʹ������

    uint8_t qr_flag;

    //��ά��ṹ��ȡ
    char			qr_issueFlag[32+1];		    //���뷽��ʶ
    char			qr_version[2+1];			//�汾��  �˰汾��Ϊ�ṹ�汾�ţ�ҲΪ���а汾��
    char			qr_encMode[2+1];			//���ܷ�ʽ
    uint8_t			qr_passcode[32+1];		    //PASSCODE
    char			qr_timestamp[14+1];		    //ʱ���
    char			qr_bleMac[12+1];			//�ֻ�����ʶ����
    char			qr_rfu[20+1];			    //Ԥ��
    char            qr_ticket_status;           //Ʊ��״̬
    char			qr_ticker_isTest;		    //1���� 2����
	char			qr_Intnet_ststus;			//����״̬��ʶ1.�������� 2.�����ж�
    //�������ݽṹ��ȡ
    BleTicketData	ble_ticketdata;

    QRTicketData	qr_ticketdata;

    uint8_t			ble_macAddress[6];		//������ַ

    uint8_t      	qr_info[256];

    char      		third_qr_info[64];    //������֧����ά��

    uint8_t      	timeAndcenterCode[46+1];    //ʱ���������Ʊ�� ������ʽ�ظ���բ

} TICKET_DATA, * P_TICKET_DATA;
//����Ʊ�쳣������
typedef struct{
    char    trad_type[2];       //��������
    char    center_code[32];    //����Ʊ��
    long    amount_timeout;     //��ʱ����
    long    amount_over_take;   //���˲�Ʊ���
    char    station_code[4];    //������վ����
	char	net_result[1];		//BOM�������½����0Ϊ���³ɹ��������齻����ȥ����0Ϊ�ȴ������������
} ETICKET_ABNORML_HANDLE, * PETICKET_ABNORML_HANDLE;


// 4.19	���ӳ�Ʊ֧��������Ϣ�ṹ
typedef struct {
	char		cTxnDate[8];					// ��Ӫ����
	char		cTransCode[2];					// ��������
	char		cTicketMainType[2];				// ��Ʊ������
	char		cTicketSubType[2];				// ��Ʊ������
	char		cChipType[2];					// Ʊ��оƬ����
	char		cTicketLogicalNo[20];			// �߼�����
	char		cCenterCode[32];				// ����Ʊ��
	char		cCenterMac[8];					// ����У����
	char		cRealTicketNo[16];				// ʵ�忨��
	char		cMark[2];						// ȡƱ״̬��
	char		cTicketCSN[16];					// Ʊ��CSN
	char		cTicketCount[10];				// Ʊ������
	char		cPsamNo[16];					// PSAM����
	char		cPsamSeq[10];					// PSAM����ˮ��
	char		cTxnTms[14];					// ��������ʱ��
	char		cValidOutTms[14];				// ��Ч��վʱ��
	char		cPayType[2];					// ֧����ʽ
	char		cBeforeTxnBalance[10];			// ����ǰƱֵ
	char		cTxnAmountNo[6];				// ���״���
	char		cTxnAmount[6];					// ���׽��
	char		cDiscountAmt[6];				// �Żݽ��
	char		cTxnStation[4];					// ��ǰ��վ
	char		cLastStation[4];				// �ϴ�ʹ�ó�վ
	char		cLastTxnTms[14];				// �ϴ�ʹ��ʱ��
	char		cDevNodeId[9];					// �ն��豸��ʶ
	char		cDevSeqNo[10];					// �ն��豸��ˮ��
	char		cTestMark[2];					// ���Խ��ױ�־
	char		cQRSerialNo[8];					// ��ά�복ƱOD���к�
	char		cOverAmount[6];					// բ����ʱ�ۿ���
	char		cTac[8];					    // TAC
	char		cNetTranNumber[64];				// ������������
	char		cReserve[32];					// ������32

}ETICKETPAYDEALINFO, * PETICKETPAYDEALINFO;

// 4.22	���ӵ���Ʊ�����ؽṹ
typedef struct {
	char		cIssueCode[4];					// ���뷽��ʶ
	char		cVersion[2];					// �汾��
	char		qr_Intnet_ststus;				// ����״̬��ʶ1.�������� 2.�����ж�
	char		cCardStatus;					// ����Ʊ��״̬
	char		cTicketLogicalNo[20];			// �߼�����
	char		cTicketSubType[2];				// ��Ʊ������
	long		lBalance;						// ��ֵ
	long		lTicketCount;					// ������
	char		cCenterCode[32];				// ����Ʊ��

	char 		cSaleDate[14];					// ��Ʊʱ��
	char		cValidDate[8];					// ��Ч��

	char		cStartStationId[4];				// ��ʼվ����
	char		cEndStationId[4];				// �յ�վ����
	long		cPrice;							// ����
	char		cTicketNum[2];					// ����
	char		cEntryDate[14];					// ��վʱ��
	char        cEntryStationId[4];				// ��վվ��
	char		cExitDate[14];					// ��վʱ��
	char        cExitStationId[4];				// ��վվ��

	char        cValidExitDate[14];				// ��Ч��վʱ��
	char		cMacCode[8];				    // ������֤��
    char		cPsamNo[16];					// PSAM����
    char		cPsamSeq[10];					// PSAM����ˮ��
	char        cVervifyCode[8];                //��֤��
}BOMESJTRETURN, * PBOMESJTRETURN;

// 4.23	���Ӵ�ֵƱ�����ؽṹ
typedef struct {
	char		cIssueCode[4];					// ���뷽��ʶ
	char		cVersion[2];					// �汾��
	char		qr_Intnet_ststus;				// ����״̬��ʶ1.�������� 2.�����ж�
	char		cCardStatus;					// ����Ʊ��״̬
	char		cTicketLogicalNo[20];			// �߼�����
	char		cTicketSubType[2];				// ��Ʊ������
	long		lBalance;						// ��ֵ
	char		cCenterCode[32];				// ����Ʊ��
	char		cEntryDate[14];					// ��վʱ��
	char        cEntryStationId[4];				// ��վվ��
	char		cExitDate[14];					// ��վʱ��
	char        cExitStationId[4];				// ��վվ��

	char        cValidExitDate[14];				// ��Ч��վʱ��
	char		cLastDealType[2];				// ĩ�ν�������
	char		cLastDealDate[14];				// ĩ�ν���ʱ��
	char		cMacCode[8];				    // ������֤��
	char		cBornTime[14];					// ����ʱ�� �󸶷Ѵ�ֵƱ��Ч
	char        cVervifyCode[8];                 //��֤��
    char		cPsamNo[16];					// PSAM����
    char		cPsamSeq[10];					// PSAM����ˮ��
    char        cEntryDevice[5];                // ��վ�豸����  �豸����(2)+�豸���(3)
    char        cExitDevice[5];                 // ��վ�豸����  �豸����(2)+�豸���(3)

}BOMESVTRETURN, * PBOMESVTRETURN;

// 4.24	��������Ʊ�����ؽṹ
typedef struct {
	char		cIssueCode[4];					// ���뷽��ʶ
	char		cVersion[2];					// �汾��
	char		qr_Intnet_ststus;				// ����״̬��ʶ1.�������� 2.�����ж�
	char		cCardStatus;					// ����Ʊ��״̬
	char		cTicketLogicalNo[20];			// �߼�����
	char		cTicketSubType[2];				// ��Ʊ������
	long		lBalance;						// ���
	char		cCenterCode[32];				// ����Ʊ��

	char		cGenerateDate[14];				// ������ά������ʱ��
	char        cCenterCodeGenDate[14];	        // ����Ʊ������ʱ��
	char        cRetain[10];				    // ������
	char		cMacCode[8];				    // ������֤��

}BOMEOFFLINERETURN, * PBOMEOFFLINERETURN;


#pragma pack(pop)

