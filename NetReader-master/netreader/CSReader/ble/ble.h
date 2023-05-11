#pragma once

#include <math.h>
#include <stdint.h>
#include <string>
#include "../json/json.h"
#include "../link/myprintf.h"
#include "../api/TicketElecT.h"
#include "../api/CmdSort.h"
#include "../api/CmdSort.h"

using namespace Json;
using namespace std;

extern CmdSort cmd_ble;


// ble������ַ����
typedef struct
{
	uint8_t head[12];				// ����ͷ
	uint8_t mac_address[6];			// MAC��ַ
	uint8_t unknowInfo[6];			// δ֪��Ϣ1
	uint8_t ble_name[16];			// ��������
	uint8_t endInfo[11];			// ������Ϣ

}SCAN_SINGLE_BLE_INF, * P_SCAN_SINGLE_BLE_INF;

// bleȫ����ַ����
typedef struct
{
	SCAN_SINGLE_BLE_INF ssbi[100];

}SCAN_ALL_BLE_INF, * P_SCAN_ALL_BLE_INF;

// �������ݽ������ݸ�ʽ��
typedef struct
{
	uint8_t head[12];				// ����ͷ
	uint8_t data_len[2];			// �����򳤶�
	uint8_t flag;					// �����
	uint8_t data[19];				// ������
	uint8_t endInfo[5];				// ������Ϣ

}BLE_PACKAGE_SINGLE, * P_BLE_PACKAGE_SINGLE;
// �������ݽ������ݸ�ʽ��
typedef struct
{
	BLE_PACKAGE_SINGLE bps[50];

}BLE_PACKAGE_ALL, * P_BLE_PACKAGE_ALL;



class ble
{
public:
    ble(void);
	static SCAN_ALL_BLE_INF scanAllBleInf;


    // ��֯���ص�Json����
    static string ble_getDealJson(ETYTKOPER operType,uint16_t& len_data);

	static int ble_writeBack(ETYTKOPER operType,uint8_t *  p_eTicket_entry_info,uint8_t *  p_eTicket_exit_info,uint8_t *  p_eTicket_get_info, uint16_t length);

	static string ble_checkTicket(ETYTKOPER operType);

	static void ble_setbaudrate();
	static void ble_setSubscriber();

	static int ble_connect(uint8_t * address);

	static void  ble_DataExchange(uint8_t * data,int dataLength,uint16_t& len_data,uint8_t * revData);

	static void ble_scan(/*uint16_t& len_data*/);

	static int ble_scanStop();

	static int format_bleInfo(uint8_t * p_cmd_recved/*, P_SCAN_SINGLE_BLE_INF p_ssbi*/, int sabi_max);
	static int format_bleInfo37(uint8_t * p_cmd_recved/*, P_SCAN_SINGLE_BLE_INF p_ssbi*/, int sabi_max);

	static bool getMacAddress(SCAN_ALL_BLE_INF sabi,uint8_t * revData,uint8_t * qr_passcode,int count);

	static void ble_close();

	static void format_ble_DataExchange(uint8_t * p_cmd_recved,int dataLength,uint16_t& len_data,uint8_t * revData);

	static bool is_LastPackege(uint8_t * p_buffer,int sumlen);

	static int ble_getWriteBackData(ETYTKOPER operType,uint8_t *  p_base_add_info, uint16_t length,uint8_t * revData);

	static int ble_getServiceHandle(/*uint8_t * serviceNo, uint8_t * handleNo*/);

	static int ble_all_oper(ETYTKOPER operType,uint8_t * passcode, uint8_t * p_eTicket_info, int lengthData);

	static void ble_handshake();

	static void ble_reset(int flag);//0�������и�λ��1:���ظ�λ
	static bool ble_get_version(uint8_t * ble_version);

	static void ble_set_baud(int speed);

	static void ble_send_test(uint8_t * sendData, int flag, int length);

	static uint16_t ble_err_exchange(int reslut);

	static void setBauTest();

protected:

	SCAN_SINGLE_BLE_INF m_SingleBleInf;
	SCAN_ALL_BLE_INF m_AllBleInf;

    static void format_bleInfo(uint8_t * p_ssbi_buffer, SCAN_SINGLE_BLE_INF& ssbi);

	static void format_bleInfo37(uint8_t * p_ssbi_buffer, SCAN_SINGLE_BLE_INF& ssbi);

	static void format_ble_DataExchange(uint8_t * p_eps_buffer, BLE_PACKAGE_SINGLE& eps);

	//static void ble_splitPackage(uint8_t * p_cmd_recved,int dataLength, uint8_t * revData,uint16_t& len_data);

};
