//Ʊ���������
// Created by deken on 2020/3/16.
//
#pragma once
#pragma pack(push, 1)

#include <stdint.h>

//��վ��Ϣ
typedef struct
{
    char		cTxnCode[2];					// ��������

    char		cInTime[14];					// ��վ����ʱ����;20060211160903
    char		cLineCode[2];					// ��·���
    char		cStationCode[2];				// վ����
    char		cDevType[2];					// �豸����
    char		cDeviceID[3];					// �豸����(BCD)����0�ֽڽ���4bit��Ч
    char		cSAMID[16];						// SAM���߼�����
    long		lSAMTrSeqNo;					// SAM���ѻ�������ˮ��
    char		cStatus[2];						// ״̬��
    char		cVerifyCode[8];					// ��֤��

}ETICKET_ENTRY_INF, * P_ETICKET_ENTRY_INF;

//��վ��Ϣ
typedef struct
{
    char		cTxnCode[2];					// ��������

    char		cInTime[14];					// ��վ����ʱ����;20060211160903
    char		cLineCode[2];					// ��·���
    char		cStationCode[2];				// վ����
    char		cDevType[2];					// �豸����
    char		cDeviceID[3];					// �豸����(BCD)����0�ֽڽ���4bit��Ч
    char		cSAMID[16];						// SAM���߼�����
    long		lSAMTrSeqNo;					// SAM���ѻ�������ˮ��
    long		lDebitAmount;					// ʵ�۽��
    char		cStatus[2];						// ״̬��
    char		cVerifyCode[8];					// ��֤��

}ETICKET_EXIT_INF, * P_ETICKET_EXIT_INF;

//ȡƱ��Ϣ
typedef struct
{
    char		cTxnCode[2];					// ��������

    char		cInTime[14];					// ��Ʊ����ʱ����;20060211160903
    char		cLineCode[2];					// ��·���
    char		cStationCode[2];				// վ����
    char		cDevType[2];					// �豸����
    char		cDeviceID[3];					// �豸����(BCD)����0�ֽڽ���4bit��Ч
    char		cSAMID[16];						// SAM���߼�����
    long		lSAMTrSeqNo;					// SAM���ѻ�������ˮ��
    char		cCardNo[16];					// �߼�����
    char		cStatus[2];						// ״̬��
    char		cVerifyCode[8];					// ��֤��

}ETICKET_GET_INF, * P_ETICKET_GET_INF;

// ����CPU����ؽṹ
typedef struct
{
    uint8_t * p_issue_base;			// ���л���Ӧ��
    uint8_t * p_public_base;		// ����Ӧ�û��������ļ�
    uint8_t * p_owner_base;			// �ֿ��˻��������ļ�
    uint8_t * p_manage_info;		// ������Ϣ�ļ�-�ж��Ƿ�����ͨƱ��
    uint8_t * p_wallet;				// Ǯ��ר��
    uint8_t * p_his_all;			// ������ʷ��¼
    uint8_t * p_trade_assist;		// ���׸��������ϼ�¼�ļ���
    uint8_t * p_metro;				// �����ͨ�����ϼ�¼�ļ���
    uint8_t * p_ctrl_record;		// Ӧ�ø�������Ӧ�ÿ��Ƽ�¼�����ϼ�¼�ļ���
    uint8_t * p_app_ctrl;			// Ӧ�ÿ����ļ�
    uint8_t * p_charge_count;		// �������׼���
    uint8_t * p_consume_count;		// �ѻ����׼���

    uint8_t *  p_eTicket_base_info;			    // Ʊ����Ϣ
    uint8_t *  p_eTicket_entry_info;			// ��վ��Ϣ
    uint8_t *  p_eTicket_exit_info;				// ��վ��Ϣ
    uint8_t *  p_eTicket_get_info;			    // ȡƱ��Ϣ

    /*
    P_ETICKET_ENTRY_INF p_eTicket_entry_info;			// ��վ��Ϣ
    P_ETICKET_EXIT_INF  p_eTicket_exit_info;			// ��վ��Ϣ
    P_ETICKET_GET_INF  p_eTicket_get_info;			    // ȡƱ��Ϣ
    */

}METRO_ELECT_INF, * P_METRO_ELECT_INF;

#pragma pack(pop)


