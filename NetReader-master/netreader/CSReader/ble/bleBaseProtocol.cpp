#include "bleBaseProtocol.h"
#include "../json/json.h"
#include <string>
#include "../link/myprintf.h"
#include "ble.h"
#include "../api/Publics.h"
#include "../json/writer.h"
#include "../json/JsonCPP.h"

using namespace std;

bleBaseProtocol::bleBaseProtocol(void)
{
}

int bleBaseProtocol::baseProtocol(uint8_t * passcode, uint8_t *  p_eTicket_info, int len_eTicket, uint8_t * revData)
{
	uint8_t data_to_send[255] = {0};
	int len = 0;

	data_to_send[0] = 0xF7;
	len += 1;
	len += 1;
	if(Api::sysFlag == 0)
		data_to_send[2] = 0x14;//04苹果，14安卓
	else
		data_to_send[2] = 0x04;//04苹果，14安卓
	len += 1;

	Publics::string_to_bcds((char *)(passcode + 16), 16, data_to_send + len, 8);

	//memcpy(data_to_send + len, passcode + 16, 8);
	len += 8;

	memcpy(data_to_send + len, p_eTicket_info, len_eTicket);
	len += len_eTicket;

	data_to_send[1] = len - 2;
	dbg_formatvar("len = %d:",len);
	dbg_formatvar("data_to_send[1] = %d:",data_to_send[1]);

	memcpy(revData, data_to_send, len);

	return len;

}
