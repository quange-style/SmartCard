// Errors.h

#pragma once

//
// ������
//
#define ERR_SUCCEED					0x0000
#define ERR_UNDEFINED				0x0001		// δ�������
#define ERR_CARD_NONE				0x0002		// �޿�
#define ERR_CARD_MORE				0x0003		// �࿨
#define ERR_CARD_READ				0x0004		// ����ʧ��
#define ERR_CARD_WRITE				0x0005		// д��ʧ��
#define ERR_WITHOUT_SELL			0x0006		// δ���ۣ�ESԤ��ֵ���ĳ�Ʊ
#define ERR_CARD_STATUS				0x0007		// Ʊ��״̬����
#define ERR_CARD_REFUND				0x0008		// Ʊ�����˿ע����
#define ERR_CARD_LOCKED				0x0009		// ��������
#define ERR_CARD_INVALID			0x000A		// ��ЧƱ
#define ERR_OVER_PERIOD_P			0x000B		// Ʊ��������Ч�ڹ���
#define ERR_OVER_PERIOD_L			0x000C		// Ʊ���߼���Ч�ڹ���
#define ERR_OVER_PERIOD_A			0x000D		// Ʊ��������Ч�ڹ���
#define ERR_CARD_DISABLED			0x000E		// Ʊ��δ����
#define ERR_OVER_SYSTEM				0x000F		// �Ǳ�ϵͳ��

#define ERR_CARD_TYPE				0x0020		// Ʊ������δ����
#define ERR_STATION_INVALID			0x0021		// �Ƿ��ĳ�վ����
#define ERR_DEVICE_INVALID			ERR_STATION_INVALID
#define ERR_CARD_USE				0x0022		// �Ǳ�վʹ�õĳ�Ʊ
#define ERR_LACK_WALLET				0x0023		// Ʊ������
#define ERR_OVER_WALLET				0x0024		// Ʊ����������
#define ERR_ENTRY_STATION			0x0025		// �Ǹ������Ǳ�վ��վ
#define ERR_ENTRY_TIMEOUT			0x0026		// �Ǹ�������վ��ʱ
#define ERR_ENTRY_EVER				0x0027		// �Ǹ������н�վ��
#define ERR_WITHOUT_ENTRY			0x0028		// �������޽�վ��
#define ERR_OVER_MILEAGE			0x0029		// ����������
#define ERR_EXIT_TIMEOUT			0x002A		// ��������վ��ʱ
#define ERR_UPDATE_STATION			0x002B		// �Ǳ�վ���µĳ�Ʊ
#define ERR_UPDATE_DAY				0x002C		// �Ǳ��ո��µĳ�Ʊ
#define ERR_PASSAGEWAY_1			0x002D		// ��ͨƱͨ������֧���Ż�Ʊ
#define ERR_PASSAGEWAY_2			0x002E		// �Ż�Ʊͨ������֧����ͨƱ
#define ERR_LAST_EXIT_NEAR			0x002F		// ����X�������ڱ�վ��վ
#define ERR_OVER_TIME_MILEAGE		0x0030		// ��ʱ�ֳ���


#define ERR_CALL_INVALID			0x0040		// �Ƿ��Ľӿڵ���
#define ERR_DEVICE_SUPPROT			0x0041		// �豸��֧�ֵ�Ʊ������
#define ERR_DIFFRENT_CARD			0x0042		// ��ͬһ��Ʊ��
#define ERR_CALLING_INVALID			0x0043		// Ʊ����ǰ���÷Ƿ�
#define ERR_INPUT_PARAM				0x0044		// �Ƿ��Ĵ������
#define ERR_DEVICE_UNINIT			0x0045		// �豸δ��ʼ��
#define ERR_APP_NOT_EXSIT			0x0046		// �����ڵ�Ӧ�ýӿ�


#define ERR_PARAM_NOT_EXIST			0x0100		// �����ļ�������
#define ERR_PARAM_INVALID			0x0101		// �����ļ����Ϸ�
#define ERR_FILE_ACCESS				0x0102		// �ļ�����
#define ERR_INPUT_INVALID			0x0103		// ��Ч���������
#define ERR_HANDLER_OPEN			0x0104		// ͨѶ����򿪴���
#define ERR_SEND_DATA				0x0105		// ͨѶ��������ʧ��
#define ERR_RECIVE_DATA				0x0106		// ͨѶ��������ʧ��
#define ERR_DATA_INVALID			0x0107		// ͨѶ���ݷǷ�
#define ERR_SAM_RESPOND				0x0108		// SAM����Ӧ����
#define ERR_OVERLAPPED				0x0109		// �����ص�����λ�������ڶ�д��һ����������δִ����ϵ�����·����µ�ָ��
#define ERR_RF_NONE					0x010A		// Rfģ�����
//
// ES ר�д�����
//
#define ERR_CLEAR_FILE				0xF001		// ɾ�����ļ�����
#define ERR_CREATE_MF				0xF002		// ����MFʧ��
#define ERR_CREATE_ADF				0xF003		// ����ADFʧ��

//
// ������ʾ��
//
#define NTC_NONE					0x00		// �޹�����ʾ
#define NTC_TYPE_OVER_PERIOD		0x01		// Ʊ�ֹ��ڣ�������ͨƱ
#define NTC_TOKEN_RECLAIM			0x10		// ����Ʊ
#define NTC_MUST_CONFIRM			0x11		// ��Ҫ���н���ȷ��
#define NTC_EXIT_TRAIN_TROUBLE		0x12		// �г�����ģʽ��վ
