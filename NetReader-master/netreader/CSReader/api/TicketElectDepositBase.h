//���Ӵ�ֵƱ������
// Created by flpkp on 2020/3/22.
//

#pragma once

#include "TicketElectBase.h"

class TicketElectDepositBase : public TicketElectBase{
protected:
    //����Ʊ���ף�������д
    void getDealData(ETYTKOPER operType,uint16_t& len_data,uint8_t * dataBack,long amount);
};


