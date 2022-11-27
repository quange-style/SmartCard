#pragma once
#include "TicketBase.h"

typedef struct
{
	uint8_t		last_time[7];
	uint8_t		last_line;
	uint8_t		last_station;
	uint8_t		last_dvc_type;
	uint16_t	last_dvc_id;
	uint8_t		last_status;
	uint8_t		last_status_special;
	uint16_t	last_wallet_val;
	uint8_t		entry_line;
	uint8_t		entry_station;
	uint16_t	trade_counter;
} UL_AREA, * P_UL_AREA;

typedef struct {
	uint8_t		area_ptr;
	uint8_t		area_buffer[2][16];
	uint8_t		area_write[16];
	UL_AREA		area_data[2];
}ULINF, * P_ULINF;

class TicketUl : public TicketBase
{
protected:

	static ULINF	m_ulInfo;

	uint16_t read_ul(uint8_t * p_ul_read_buf);

    void get_deal_area(P_UL_AREA p_deal_info, uint8_t * p_deal_area);

    void set_deal_area(P_UL_AREA p_deal_info, uint8_t * p_deal_area);

    SimpleStatus get_simple_status(uint8_t init_status);

	uint16_t read_equals_write(uint8_t * p_area_data);

    RETINFO write_card(ETYTKOPER operType, long trade_amount, long& sam_seq, char * p_trade_tac);
	
	uint16_t get_valid_ptr(ETYTKOPER operType, int read_result);
	// 赋值给上次交易未赋值的交易因子
	//void set_confirm_factor(uint8_t status, long sam_seq, char * p_tac);

public:
    TicketUl(void);
    TicketUl(char * p_current_sam, char * p_sam_posid, uint8_t * physic_info);

	// 统一票卡状态，分析和交易中的状态都用此状态
	uint8_t unified_status();

    RETINFO read_card(ETYTKOPER operType, TICKET_PARAM& ticket_prm, uint8_t * p_read_init = NULL);

    RETINFO analyse_common(uint8_t wk_area, MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PTICKETLOCK p_lock, uint16_t& lenlock, PBOMANALYZE p_analyse, char * p_mode);

    RETINFO analyse_work_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PPURSETRADE p_purse, PBOMETICANALYZE p_eticanalyse);

    RETINFO analyse_free_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PBOMETICANALYZE p_eticanalyse);

	uint32_t add_oper_status(uint8_t wk_area, RETINFO ret, TICKET_PARAM * p_ticket_prm);

    RETINFO entry_gate(PENTRYGATE p_entry,PETICKETDEALINFO p_eticketdealinfo);

    RETINFO exit_gate(PPURSETRADE p_exit,PETICKETDEALINFO p_eticketdealinfo, MODE_EFFECT_FLAGS * p_degrade_falgs);

    RETINFO sale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, POTHERSALE p_svt_sale, uint16_t& len_svt, TICKET_PARAM& ticket_prm);

	RETINFO esale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, PETICKETDEALINFO p_eticketdealinfo, uint16_t& len_eticket_sale, TICKET_PARAM& ticket_prm);

	RETINFO thirdesale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, PETICKETDEALINFO p_eticketdealinfo, uint16_t& len_eticket_sale, TICKET_PARAM& ticket_prm , char * cNetTranNumber);

    RETINFO exit_sjt_sale(PSJTSALE p_exit_sjt_sale);

	RETINFO sjt_decrease();

    RETINFO bom_update(PTICKETUPDATE p_update, uint8_t * p_entry_station);

    RETINFO bom_refund(PDIRECTREFUND p_refund);

	bool last_trade_need_continue(uint8_t status_targ, uint16_t err_targ, uint16_t err_src);

	RETINFO continue_last_trade(void * p_trade);

};

