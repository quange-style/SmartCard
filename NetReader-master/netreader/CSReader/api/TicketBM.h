#pragma once
#include "ticketbase.h"
#include "../link/linker.h"

// M1卡读卡结构
typedef struct {
	uint8_t		base_public[64];
	uint8_t		his_all[16];
	uint8_t		metro_inf[16];
}BUS_M1;

// 交易确认时使用
typedef struct {
	uint8_t		write_type;
	uint8_t		write_data[16];
	uint8_t		tac_data[16];
}WRITE_BUS_M1, * P_WRITE_BUS_M1;

class TicketBM : public TicketBase
{
public:
	TicketBM(void);
	TicketBM(char * p_current_sam, char * p_sam_posid, uint8_t * physic_info);
	~TicketBM(void);

	// 统一票卡状态，分析和交易中的状态都用此状态
	uint8_t unified_status();

	//
	// 对于所有票卡都有，但是实现上不同的接口全部定义为纯虚函数
	//
	RETINFO read_card(ETYTKOPER operType, TICKET_PARAM& ticket_prm, uint8_t * p_read_init = NULL);

	RETINFO analyse_common(uint8_t wk_area, MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PTICKETLOCK p_lock, uint16_t& lenlock, PBOMANALYZE p_analyse, char * p_mode);

	RETINFO analyse_work_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PPURSETRADE p_purse, PBOMETICANALYZE p_eticanalyse);

	RETINFO analyse_free_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PBOMETICANALYZE p_eticanalyse);

	RETINFO entry_gate(PENTRYGATE p_entry, PETICKETDEALINFO p_eticketdealinfo,char cOnlineRes);

	RETINFO exit_gate(PPURSETRADE p_exit, PETICKETDEALINFO p_eticketdealinfo, MODE_EFFECT_FLAGS * p_degrade_falgs,char cOnlineRes);

	RETINFO check_online(ETYTKOPER operType,PETICKETAUTHINFO p_eticketauthinfo);

	RETINFO bom_update(PTICKETUPDATE p_update, uint8_t * p_entry_station);

	//RETINFO svt_decrease(PPURSETRADE p_purse, uint8_t u_pay_flag);
	//支付方式默认为01 现金
	RETINFO svt_decrease(PPURSETRADE p_purse, uint8_t u_pay_flag, uint8_t paytype=1);
	RETINFO format_history(uint8_t&	his_count, PHSSVT p_his_array, int his_max);

	RETINFO lock(PTICKETLOCK p_lock);

	bool last_trade_need_continue(uint8_t status_targ, uint16_t err_targ, uint16_t err_src);

	RETINFO continue_last_trade(void * p_trade);

protected:

	RETINFO write_card(ETYTKOPER operType, long trade_amount, char * p_tac_type, char * p_trade_tac, P_WRITE_BUS_M1 p_write_inf);

	//void save_last_stack(ETYTKOPER operType, void * p_trade, size_t size_trade, void * p_written_inf, size_t size_written, bool save_to_file);

	// 赋值给上次交易未赋值的交易因子
	//void set_confirm_factor(uint8_t status, long sam_seq, char * p_tac);

private:
	uint8_t m_pid[4];

	uint8_t	m_start_use_flag;

	BUS_M1 m_ReadInf;

	SimpleStatus get_simple_status(uint8_t init_status);

	void contrast_ticket_type(uint8_t * p_ticket_type);

	bool format_history(uint8_t * p_his_buffer, HSSVT& his);

	// 获取写卡类型，并判断传入参数是否正确
	uint16_t get_write_type(ETYTKOPER operType, uint8_t& write_type, long trade_amount, uint8_t * p_time, uint8_t * p_metro);

	void set_metro_area();

	void set_tac_factor(uint8_t * p_tac_buf, uint8_t * p_time, long balance_after, uint16_t trade_amount, uint16_t trade_counter);

	// 票卡类型是否需要转到专用钱包
	bool use_wallet_special(uint8_t * p_logic_type);

	// 是否检查启用标识
	bool need_check_use_flag(uint8_t * p_logic_type);
};
