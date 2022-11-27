#pragma once

#include <math.h>
#include <stdint.h>
#include <string>
#include "../json/json.h"
#include "../link/myprintf.h"
#include "../api/TicketElecT.h"

using namespace std;



class bleBaseProtocol
{
public:
	bleBaseProtocol(void);

	// 组织返回的Json数据
	static int baseProtocol(uint8_t * passcode, uint8_t *  p_eTicket_info, int len_eTicket, uint8_t * revData);

//protected:

};