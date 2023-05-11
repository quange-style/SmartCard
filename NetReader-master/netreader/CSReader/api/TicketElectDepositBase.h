//电子储值票基础类
// Created by flpkp on 2020/3/22.
//

#pragma once

#include "TicketElectBase.h"

class TicketElectDepositBase : public TicketElectBase{
protected:
    //电子票交易，蓝牙回写
    void getDealData(ETYTKOPER operType,uint16_t& len_data,uint8_t * dataBack,long amount);
};


