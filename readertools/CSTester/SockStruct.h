//SockStruct.h

#pragma once

#pragma pack(push, 1)

typedef struct
{
	char	cMsgType[2];				// ��Ϣ����	0	2	Char	ȡֵ��65��	����-> �ն�
	BYTE	dtTime[7];					// ��Ϣ����ʱ��	2	7	BCD	YYYYMMDD HH24MISS	
	long	lServerSeq;					// ϵͳ���պ�	9	4	LONG	��̨������ˮ�ţ�����ִ���	
	char	cPosId[9];					// �ն˱��	13	9	CHAR	��·����(2)+��վ����(2)+�豸����(2)+�豸����(3)	
	char	cSamId[16];					// Sam���߼�����	22	16	CHAR	psam�߼�����	
	char	cMac[16];					// MAC	38	16	CHAR		
	char	cAck[2];					// ��Ӧ��	54	2	CHAR	00��������01:������ҵ���߼���ϵͳ��������г����쳣����ϸԭ�����������	
	char	cDiscrib[2];				// ����	56	2	CHAR		

}SAM_ACTIVE_ACK;

typedef struct
{
	char	cMsgType[2];				// ��Ϣ����	0	2	Char	ȡֵ��61��	����-> �ն�
	BYTE	dtTime[7];					// ��Ϣ����ʱ��	2	7	BCD	YYYYMMDD HH24MISS	
	long	lServerSeq;					// ϵͳ���պ�	9	4	LONG	��̨������ˮ��	
	char	cPosId[9];					// �ն˱��	13	9	CHAR	��·����(2)+��վ����(2)+�豸����(2)+�豸����(3)	
	char	cSamId[16];					// Sam���߼�����	22	16	CHAR	psam�߼�����	
	long	lPosSeq;					// �����к�	38	4	LONG	��ֵ����51����	
	char	cLogicalID[20];				// Ʊ���߼�����	42	20	CHAR		
	char	cMac[8];					// MAC2	62	8	CHAR	���������ɵ���֤�룬�ɹ�ʱ��Ч ���ɹ�ʱȫ0	
	BYTE	dtServer[7];				// ϵͳʱ��	70	7	BCD	�����ķ��� ��ֵ�豸������ΪƱ���ĳ�ֵʱ�䣬�ɹ�ʱ��Ч ���ɹ�ʱȫ0	
	char	cAck[2];					// ��Ӧ��	77	2	CHAR	00��������01:������ҵ���߼���ϵͳ��������г����쳣����ϸԭ�����������	
	char	cDiscrib[2];				// ��������	79	2	CHAR		

}CHARGE_APP_ACK;

typedef struct
{
	char 		cMsgType[2];					// ��Ϣ���͡�52��
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
	char		cTac[8];						// Ǯ������TAC��
	char		cResult;						// д�����
	char		cOperatorId[6];					// ����Ա����
	long		lServerSeq;						// ϵͳ���պţ�61����
	uint8_t		dtTimeOper[7];					// ��Ϣ61���صĳ�ֵʱ��

}CHARGE_CONFIRM;


typedef struct
{
	char 		cMsgType[2];					// ��Ϣ���͡�62��
	uint8_t		dtMsgTime[7];					// ��Ϣ����ʱ��
	long		lServerSeq;						// ϵͳ���պ�
	char		cPosId[9];						// �ն˱��	13	9	CHAR	��·����(2)+��վ����(2)+�豸����(2)+�豸����(3)	
	char		cSamId[16];						// SAM����
	long		lPosSeq;						// �ն˽������к�
	char		cLogicalId[20];					// Ʊ���߼�����
	char		cAck[2];						// ��Ӧ��	77	2	CHAR	00��������01:������ҵ���߼���ϵͳ��������г����쳣����ϸԭ�����������	
	char		cDiscrib[2];					// ��������	79	2	CHAR		

}CHARGE_CONFIRM_ACK;

typedef struct
{
	char	cMsgType[2];				// ��Ϣ����	0	2	Char	ȡֵ��63��	����-> �ն�
	BYTE	dtTime[7];					// ��Ϣ����ʱ��	2	7	BCD	YYYYMMDD HH24MISS	
	long	lServerSeq;					// ϵͳ���պ�	9	4	LONG	��̨������ˮ��	
	char	cPosId[9];					// �ն˱��	13	9	CHAR	��·����(2)+��վ����(2)+�豸����(2)+�豸����(3)	
	char	cSamId[16];					// Sam���߼�����	22	16	CHAR	psam�߼�����	
	long	lPosSeq;					// �����к�	38	4	LONG	��ֵ����51����	
	char	cLogicalID[20];				// Ʊ���߼�����	42	20	CHAR		
	BYTE	dtServer[7];				// ϵͳʱ��	70	7	BCD	�����ķ��� ��ֵ�豸������ΪƱ���ĳ�ֵʱ�䣬�ɹ�ʱ��Ч ���ɹ�ʱȫ0	
	char	cAck[2];					// ��Ӧ��	77	2	CHAR	00��������01:������ҵ���߼���ϵͳ��������г����쳣����ϸԭ�����������	
	char	cDiscrib[2];				// ��������	79	2	CHAR		

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
