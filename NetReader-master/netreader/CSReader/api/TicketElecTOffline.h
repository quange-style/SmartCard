#pragma once
#include "../link/linker.h"
#include "TicketElectBase.h"
#include "EnumTypes.h"
#include "TicketsStructs.h"

class TicketElecTOffline : public TicketElectBase
{
public:
	TicketElecTOffline(void);
	TicketElecTOffline(char * p_current_sam, char * p_sam_posid, uint8_t * physic_info);
	~TicketElecTOffline(void);

	//
	// ��������Ʊ�����У�����ʵ���ϲ�ͬ�Ľӿ�ȫ������Ϊ���麯��
	//
	RETINFO read_card(ETYTKOPER operType, TICKET_PARAM& ticket_prm, uint8_t * p_read_init = NULL);

	RETINFO analyse_common(uint8_t wk_area, MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PTICKETLOCK p_lock, uint16_t& lenlock, PBOMANALYZE p_analyse, char * p_mode);
	RETINFO analyse_free_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PBOMETICANALYZE p_eticanalyse);
	RETINFO analyse_work_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PPURSETRADE p_purse, PBOMETICANALYZE p_eticanalyse);

	RETINFO entry_gate(PENTRYGATE p_entry, PETICKETDEALINFO p_eticketdealinfo,char cOnlineRes);
	RETINFO exit_gate(PPURSETRADE p_exit, PETICKETDEALINFO p_eticketdealinfo, MODE_EFFECT_FLAGS * p_degrade_falgs,char cOnlineRes);

	//RETINFO getETicket(PETICKETDEALINFO p_eticketdealinfo);
	RETINFO getETicket(PGETETICKETRESPINFO p_geteticketrespinfo);
	RETINFO bom_update(PTICKETUPDATE p_update, uint8_t * p_entry_station);
	//�����ѻ�Ʊ���⣬�����������������д��
	void transfer_edata_for_out(ETYTKOPER operType, void * pTradeStruct);

protected:

	void get_qrInfo(uint8_t * p_grInfo);

    //���õ�ǰ����Ʊ����
    ElectTicketType getCurrentTicketType();
    //����Ʊ�쳣���ף�������д
    void getDealData(ETYTKOPER operType,uint16_t& len_data,uint8_t * dataBack,long amount);

};
