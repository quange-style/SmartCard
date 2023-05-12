#pragma once
#include "TicketBase.h"
#include "../link/linker.h"

// 公交CPU卡相关结构
typedef struct
{
	uint8_t * p_issue_base;			// 发行基本应用
	uint8_t * p_app_index;			// 卡应用索引文件
	uint8_t * p_public_base;		// 公共基本应用
	uint8_t * p_owner;				// 持卡人基本数据文件
	uint8_t * p_wallet;				// 钱包专用
	uint8_t * p_metro;				// 轨道交通（符合记录文件）
	uint8_t * p_his_all;			// 所有历史记录
	uint8_t * p_charge_count;		// 联机交易计数
	uint8_t * p_consume_count;		// 脱机交易计数

}BUS_CPU_INF, * P_BUS_CPU_INF;

class TicketBus : public TicketBase
{
public:
	TicketBus(void);
	TicketBus(char * p_current_sam, char * p_sam_posid, uint8_t * physic_info);
	~TicketBus(void);

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

	uint8_t m_u_buffer[1024];
	BUS_CPU_INF m_ReadInf;

	uint16_t bus_svt_read(P_BUS_CPU_INF p_bus_inf);
	BUS_CPU_INF file_need_read(ETYTKOPER type, uint8_t * p_buffer);
	BUS_CPU_INF file_need_write(ETYTKOPER type, P_BUS_CPU_INF p_read_inf);

	void get_issue_base(uint8_t * p_issue_base);
	void get_app_index(uint8_t * p_issue_base, TICKET_PARAM& ticket_prm);
	void get_public_base(uint8_t * p_public_base);
	void get_owner_base(uint8_t * p_owner_base);
	void get_wallet(uint8_t * p_wallet);
	void get_his_all(uint8_t * p_his_all);
	void get_metro(uint8_t * p_metro);
	void get_change_count(uint8_t * p_charge_count);
	void get_consume_count(uint8_t * p_consume_count);

	RETINFO write_card(ETYTKOPER operType, BUS_CPU_INF write_inf, uint8_t tac_type, long trade_amount, long& sam_seq, char * p_trade_tac);
	RETINFO write_card(uint8_t tac_type, uint8_t lock_flag, long& sam_seq);

	void set_metro(uint8_t * p_metro);

	void save_last_stack(ETYTKOPER operType, void * p_trade, size_t size_trade, void * p_written_inf, size_t size_written, bool save_to_file);

	// 赋值给上次交易未赋值的交易因子
	//void set_confirm_factor(uint8_t status, long sam_seq, char * p_tac);

private:
	uint8_t	m_start_use_flag;

	SimpleStatus get_simple_status(uint8_t init_status);

	void get_certificate_name(uint8_t cert_type, char * p_name);

	void contrast_ticket_type(uint8_t * p_ticket_type);

	uint16_t oct_get_lock_flag();

	bool format_history(uint8_t * p_his_buffer, HSSVT& his);

	bool use_wallet_special(uint8_t * p_logic_type);

	// 是否检查启用标识
	bool need_check_use_flag(uint8_t * p_logic_type);

};
