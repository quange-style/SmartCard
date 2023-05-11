// Declares.h

#pragma once
#include <stdio.h>


// ���ú�
#define SIZE_THREAD_STACK		4096
#define UTC_TIME_START			"\x19\x70\x01\x01\x00\x00\x00"

#define NAME_PRM_CONFIG			(char *)"PrmConfig.dat"
#define NAME_TRADE_FOLDER		(char *)"/data/Trade/"
#define NAME_LOG_FOLDER			(char *)"/data/Log/"
#define NAME_PRM_FOLDER			(char *)"ParamFolder"
#define NAME_CONFIRM_FILE		(char *)"Confirm.his"
#define NAME_TEMP_FOLDER		(char *)"Tmp"
#define NAME_TEMP_APP			(char *)"CSReader"
#define NAME_SCRIPT				(char *)"api_script.sh"

// SAM������
#define SAM_TYPE_PSAM				(uint8_t)0x00
#define SAM_TYPE_ISAM				(uint8_t)0x01
#define SAM_TYPE_CSAM				(uint8_t)0x02
#define SAM_TYPE_MSAM				(uint8_t)0x03
#define SAM_TYPE_HSAM				(uint8_t)0x04

// ������
#define BAUD_ID_9600				(uint8_t)0x00
#define BAUD_ID_19200				(uint8_t)0x01
#define BAUD_ID_38400				(uint8_t)0x02
#define BAUD_ID_57600				(uint8_t)0x03
#define BAUD_ID_115200				(uint8_t)0x04

// BOM��TVM����Ʊ��ʱ�Ŀɲ���״̬
#define ALLOW_CHARGE				0x01				// �Ƿ�ɳ�(ֵ/�˴�)
#define ALLOW_UPDATE				0x01<<1				// �Ƿ�ɸ���
#define ALLOW_SALE					0x01<<2				// �Ƿ�ɷ���
#define ALLOW_ACTIVE				0x01<<3				// �Ƿ�ɼ���
#define ALLOW_DIFFER				0x01<<4				// �Ƿ������
#define ALLOW_REFUND				0x01<<5				// �Ƿ���˿�
#define ALLOW_UNLOCK				0x01<<6				// �Ƿ�ɽ���
#define ALLOW_REFUND_DE				0x01<<7				// �Ƿ����Ѻ��

// Ʊ����׼״̬
#define USTATUS_INIT				(uint8_t)200			 // ��ʼ��
#define USTATUS_ES					(uint8_t)201			 // Init II (Pre�Cvalue loaded @E/S)(ESԤ��ֵ)
#define USTATUS_SALE				(uint8_t)202			 // BOM/TVM����
#define USTATUS_EXIT				(uint8_t)203			 // ��վ(exit)
#define USTATUS_EXIT_T				(uint8_t)204			 // �г�����ģʽ��վ(exit during Train�Cdisruption)
#define USTATUS_UPD_OUT				(uint8_t)205			 // ��վBOM����(upgrade at BOM for Entry)
#define USTATUS_UPD_OUT_FREE		(uint8_t)206			 // �Ǹ�������Ѹ��£�BOM/pca �Ǹ�������
#define USTATUS_UPD_OUT_FARE		(uint8_t)207			 // �Ǹ��������Ѹ��£�BOM/pca �Ǹ�������
#define USTATUS_ENTRY				(uint8_t)208			 // ��վ(entry)
#define USTATUS_UPD_IN				(uint8_t)209			 // ��վBOM����(upgrade at BOM for Exit)
#define USTATUS_UPD_WO_STA			(uint8_t)210			 // �޽�վ����£�BOM/pca ��������
#define USTATUS_UPD_TM_OUT			(uint8_t)211			 // ��ʱ���£�BOM/pca ��������
#define USTATUS_UPD_TP_OUT			(uint8_t)212			 // ���˸��£�BOM/pca ��������
#define USTATUS_EXIT_ONLY			(uint8_t)213			 // ET for Exit(��վƱ)
#define USTATUS_REFUND				(uint8_t)214			 // �˿�
#define USTATUS_DETROY				(uint8_t)215			 // ��Ʊע��
#define USTATUS_UNDEFINED			(uint8_t)255			 // δ֪״̬

// Ʊ����������״̬
#define LIFE_INIT					(uint8_t)100			 // ��ʼ��
#define LIFE_ES						(uint8_t)101			 // Init II (Pre�Cvalue loaded @E/S)(ESԤ��ֵ)
#define LIFE_SALE					(uint8_t)102			 // BOM/TVM����
#define LIFE_EXIT					(uint8_t)103			 // ��վ(exit)
#define LIFE_EXIT_T					(uint8_t)104			 // �г�����ģʽ��վ(exit during Train�Cdisruption)
#define LIFE_UPD_OUT				(uint8_t)105			 // ��վBOM����(upgrade at BOM for Entry)
#define LIFE_UPD_OUT_FREE			(uint8_t)106			 // �Ǹ�������Ѹ��£�BOM/pca �Ǹ�������
#define LIFE_UPD_OUT_FARE			(uint8_t)107			 // �Ǹ��������Ѹ��£�BOM/pca �Ǹ�������
#define LIFE_ENTRY					(uint8_t)108			 // ��վ(entry)
#define LIFE_UPD_IN					(uint8_t)109			 // ��վBOM����(upgrade at BOM for Exit)
#define LIFE_UPD_WO_STA				(uint8_t)110			 // �޽�վ����£�BOM/pca ��������
#define LIFE_UPD_TM_OUT				(uint8_t)111			 // ��ʱ���£�BOM/pca ��������
#define LIFE_UPD_TP_OUT				(uint8_t)112			 // ���˸��£�BOM/pca ��������
#define LIFE_EXIT_ONLY				(uint8_t)113			 // ET for Exit(��վƱ)
#define LIFE_REFUND					(uint8_t)114			 // �˿�
#define LIFE_DETROY					(uint8_t)115			 // ��Ʊע��
#define LIFE_DEFFER					(uint8_t)116			 // ����
#define LIFE_CHARGE					(uint8_t)117			 // ��ֵ
#define LIFE_ACTIVE					(uint8_t)118			 // ����
#define LIFE_DECREASE				(uint8_t)119			 // ��ֵ
#define LIFE_LOCKED					(uint8_t)120			 // ����
#define LIFE_UNLOCKED				(uint8_t)121			 // ����
#define LIFE_EXIT_TK_EXIT			(uint8_t)122			 // ��վƱ��վ
#define LIFE_CONSUME_COMMON			(uint8_t)180			 // ��ͨ����
#define LIFE_CONSUME_COMPOUND		(uint8_t)181			 // ��������
#define LIFE_CONFIRM				(uint8_t)198			 // ����ȷ�ϳɹ�
#define LIFE_CONFIRM_RM				(uint8_t)199			 // �������ȷ��
#define LIFE_UNDEFINED				(uint8_t)255			 // δ�������

// ����CPU���ļ�����
#define LENM_ISSUE_BASE				0x28	
#define LENM_PUBLIC_BASE			0x1E	
#define LENM_OWNER_BASE				0xA4
#define LENM_WALLET					0x04			
#define LENM_HIS_ALL				0x02B2					// �������ѣ�������ѣ���ֵ0x17 * 0x0A * 3			
#define LENM_TRADE_ASSIST			0x30			
#define LENM_METRO					0x30			
#define LENM_CTRL_RECORD			0x18
#define LENM_APP_CTRL				0x20
#define LENM_CHANGE_CNT				0x02
#define LENM_CONSUME_CNT			0x02

// ����CPU���ļ�����
#define LENB_ISSUE_BASE				0x30	
#define LENB_APP_INDEX				0x10
#define LENB_PUBLIC_BASE			0x1E	
#define LENB_OWNER_BASE				0x37
#define LENB_WALLET					0x04			
#define LENB_HIS_ALL				0xE6			
#define LENB_METRO					0x20			
#define LENB_CHANGE_CNT				0x02
#define LENB_CONSUME_CNT			0x02

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����BAE_������Ͷ���
// �����豸����
typedef enum
{
    dvcLCC			= 0,
    dvcSC			= 1,
    dvcTVM			= 2,
    dvcBOM			= 3,
    dvcEntryAGM		= 4,
    dvcExitAGM		= 5,
    dvcBidirectAGM	= 6,
    dvcTCM			= 7,
    dvcPCA			= 8,
    dvcES			= 9,
    dvcICCS			= 99
} ETPDVC;

// Ʊ����������
typedef enum
{
	operNone,
    operAnalyse,
    operUpdate,
	operSale,		// ����operSvtSale��operSjtSale
    operSvtSale,
    operSjtSale,
    operSjtClear,
    operDecr,
    operIncr,
    operDeffer,
    operRefund,
    operLock,
    operEntry,
    operExit,
    operTkInf
} ETYTKOPER;

typedef enum 
{
	mediNone		= 0, 
	mediUltraLight	= 0x01, 
	mediBusMifare	= 0x02,
	mediMetroCpu	= 0x03, 
	mediBusCpu		= 0x04,
	mediBusMfEX		= 0x05
} ETPMDM;

// ����Ʊ��״̬�����е�״̬���ܶ�Ӧ�ɾ���״̬����������ͳһ
typedef enum
{
	SStatus_Exit = 1,
	SStatus_Exit_Tt = 2,
	SStatus_Exit_Only = 3,
	SStatus_Upd_FZone = 4,
	SStatus_Entry = 5,
	SStatus_Upd_WZone = 6,
	SStatus_Init = 7,
	SStatus_Sale = 8,
	SStatus_Es = 9,
	SStatus_Refund = 10,
	SStatus_Invalid = 11
} SimpleStatus;
