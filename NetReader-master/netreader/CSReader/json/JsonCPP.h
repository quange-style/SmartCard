#pragma once

#include <math.h>
#include <stdint.h>
#include <string>
#include "../json/json.h"
#include "../link/myprintf.h"

using namespace Json;
using namespace std;

class JsonCPP
{
public:
    JsonCPP(void);

    // ��֯���ص�Json����
    static string GetRespJson(Value json_ticket,Value json_ticket_add);

};
