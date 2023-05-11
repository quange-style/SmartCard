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


// 4.18	������ȡƱ������Ϣ����
typedef struct {
	char		cAmount[2];						// ����Ʊ����
	char		cSum[8];						// �ܽ��
	char		cCenterCode[32];				// Ʊ��
	char		cTicketLogicalNo[20];			// Ʊ���߼���
	char        cStartStation[4];				// ��ʼվ�����
	char        cTerminalStation[4];			// �յ�վ����

}GETETICKETRESPINFO, * PGETETICKETRESPINFO;



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
	char        cRecordVer[2];                  // ��¼�汾����ʱĬ��Ϊ10
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

//4.20.���ӳ�Ʊ������֤��Ϣ�ṹ
typedef struct {
	char		cTradeType[2];					// ��������
	char		bTicketType[2];					// ��Ʊ������
	char		cTicketSubType[2];				// ��Ʊ������
	char		cTicketLogicalNo[20];			// �߼�����
	char		cCenterCode[32];				// ����Ʊ��
	char		cCenterMac[8];					//	MAC
	char		cInternetStatus;				//����״̬
}ETICKETONLINE, * PETICKETONLINE;
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
	char		cVersion[2];					// �汾��
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
	long		lPenalty1;						// ������
	long		lPenalty2;						// ���·���


}BOMETICANALYZE, * PBOMETICANALYZE;

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
	char		cTkAppMode;						// ��Ӧ��ģʽ �� '1'��CPU�� '2'��M1/UL, '3'����M1
	uint8_t		bCertExpire[4];					// ֤����Ч�ڣ�Ŀǰ��Ҫ���ڹ���ѧ��Ʊ��������Ч�ڹ���ʱ��ѧ��Ʊ����ͨƱ�۷�

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
}ESANALYZE1, * PESANALYZE1;

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


//ES����
typedef struct {
	uint8_t		bIssueStatus;				// ����״̬ 0 �Cδ����;1 �C�ѷ���;2 �C ע��
	uint8_t		bStatus;					// Ʊ��״̬ �� �Ը�¼5����Ʊ״̬��Ϊ׼
	char		cTicketType[4];				// Ʊ������
	char		cLogicalID[20];				// Ʊ���߼�����
	char		cPhysicalID[20];			// Ʊ��������
	uint8_t		bCharacter;					// Ʊ���������� 1��OCT��2��020��3��080��4��UL��5��FM��F:����
	char		cIssueDate[14];				// ��Ʊ����ʱ������ʱ��
	char		cExpire[8];					// ������Ч��, YYYYMMDDHHMMSS��
	char		RFU[16];					// E/S SAM�߼�����, Ĭ��ֵ:��0000000000��
	long		lBalance;					// ���, ��λΪ��/��(Ĭ��Ϊ0)
	long 		lDeposite;					// Ѻ��
	char		cLine[2];
	char		cStationNo[2];
	char		cDateStart[8];				// �߼���Ч�ڿ�ʼʱ��
	char		cDateEnd[8];				// �߼���Ч�ڽ���ʱ��
	uint8_t		dtDaliyActive[7];			// ����Ʊ����ʱ��
	uint16_t		bEffectDay;					// ����Ʊ��Ч����
	char		cLimitEntryLine[2];			// ���ƽ�վ��·
	char		cLimitEntryStation[2];		// ���ƽ�վվ��
	char		cLimitExitLine[2];			// ���Ƴ�վ��·
	char		cLimitExitStation[2];		// ���Ƴ�վվ��
	char		cLimitMode[3];			    // ����ģʽ

	uint8_t			certificate_iscompany;  //00-���˿�, 01-��λ��
	uint8_t			certificate_ismetro;  //������λ��
	char			certificate_name[128];	// ֤������������
	char			certificate_code[32];	// ֤������
	uint8_t			certificate_type;		// ֤������
	uint8_t			certificate_sex;		// ֤������

	uint16_t		trade_count;		// ���׼�����add by shiyulong in 2013-10-14

	// add by shiyulong on 2016-02-19

	unsigned char cardfactory[4];


}ESANALYZE, * PESANALYZE;


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
	uint8_t		cEPayType;						// ����֧����ʽ(΢�ţ�֧������)

}EPURSETRADE, * PEPURSETRADE;

// 4.21	���ӵ���Ʊֻ���ؽṹ
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
	char		cVervifyCode[8];				// ������֤��
}BOMESJTRETURN, * PBOMESJTRETURN;

// 4.21	���Ӵ�ֵƱֻ���ؽṹ
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
	char		cMacCode[8];					// ������֤��
	char		cBornTime[14];					// ����ʱ�� �󸶷Ѵ�ֵƱ��Ч

}BOMESVTRETURN, * PBOMESVTRETURN;

typedef struct
{
	uint8_t		DegradeFlag;					// ����ģʽ�ı�־
	uint8_t		WorkArea;						// ��������
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
	char		cEntryDate[14];					// ��վʱ��
	char        cEntryStationId[4];				// ��վվ��
	char		cExitDate[14];					// ��վʱ��
	char        cExitStationId[4];				// ��վվ��

	char        cValidExitDate[14];				// ��Ч��վʱ��
	char		cVervifyCode[8];				// ������֤��
}ITPTicketData, * PITPTicketData;

#pragma pack(pop)
