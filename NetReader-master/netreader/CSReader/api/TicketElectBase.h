// ����Ʊ������
// Created by flpkp on 2020/3/17.
//
#pragma once

#include "TicketBase.h"
#include "Structs.h"
#include "EnumTypes.h"

class TicketElectBase : public TicketBase{
public:
    RETINFO bom_elect_update(PETICKET_ABNORML_HANDLE p_update, uint8_t* dataBack, uint16_t& len_data);

    RETINFO EPayData(PETICKETPAYDEALINFO p_purse, uint8_t* p_time, char* paytype);

protected:
    RETINFO write_card(ETYTKOPER operType, uint8_t * e_ticketInfo, uint16_t length, uint8_t tac_type, long trade_amount, long& sam_seq, char * p_trade_tac);
    uint16_t write_card(uint8_t tac_type, uint8_t * p_app_ctrl, int record_id, uint8_t lock_flag, uint8_t * p_file_data, long& sam_seq);
    virtual void get_elect_update_info(ETYTKOPER operType, uint8_t * p_eTicket_info,uint16_t len_eTicket_info);
    //��ȡ����Ʊ����
    virtual ElectTicketType getCurrentTicketType()=0;
    //������ά��ͷ����Ϣ �������ɳ�й����ͨ�������ۼ�Ʊƽ̨SLE���APPҵ�����淶��
    void getQRHeadInfo(uint8_t *p_grInfo);
    //������ά�������й�����Ϣ �������ɳ�й����ͨ�������ۼ�Ʊƽ̨SLE��˿�APPҵ�����淶��
    void getQRDataComInfo(uint8_t *p_grInfo);

    virtual RETINFO analyse_free_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PBOMETICANALYZE p_eticanalyse);

    //todo: �ŵ���������Ϊprotected
    //����Ʊ�쳣����  ���ȥ��������д
    virtual void getDealData(ETYTKOPER operType,uint16_t& len_data,uint8_t * dataBack,long amount);
    //����Ʊբ��˫������֤���Ż���������
    RETINFO check_online(ETYTKOPER operType,PETICKETAUTHINFO p_eticketauthinfo);

private:
    ETYTKOPER convertType(int trad_type);

};
