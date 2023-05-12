#pragma once
#include "TicketBase.h"
#include "../link/linker.h"

// 地铁CPU卡相关结构
typedef struct
{
	uint8_t * p_issue_base;			// 发行基本应用
	uint8_t * p_public_base;		// 公共应用基本数据文件
	uint8_t * p_owner_base;			// 持卡人基本数据文件
	uint8_t * p_wallet;				// 钱包专用
	uint8_t * p_his_all;			// 所有历史记录
	uint8_t * p_trade_assist;		// 交易辅助（符合记录文件）	
	uint8_t * p_metro;				// 轨道交通（符合记录文件）	
	uint8_t * p_ctrl_record;		// 应用辅助，即应用控制记录（符合记录文件）	
	uint8_t * p_app_ctrl;			// 应用控制文件
	uint8_t * p_charge_count;		// 联机交易计数
	uint8_t * p_consume_count;		// 脱机交易计数

}METRO_CPU_INF, * P_METRO_CPU_INF;

class TicketSvt : public TicketBase
{
#define EXTERN_STATUS_INIT		0		// 初始化
#define EXTERN_STATUS_SALE		1		// ES或发售
#define EXTERN_STATUS_RFND		2		// 退款
#define EXTERN_STATUS_DSTY		3		// 注销

public:
	TicketSvt(void);
	TicketSvt(char * p_current_sam, char * p_sam_posid, uint8_t * physic_info);
	~TicketSvt(void);

	// 统一票卡状态，分析和交易中的状态都用此状态
	uint8_t unified_status();

	//
	// 对于所有票卡都有，但是实现上不同的接口全部定义为纯虚函数
	//
	RETINFO read_card(ETYTKOPER operType, TICKET_PARAM& ticket_prm, uint8_t * p_read_init = NULL);

	RETINFO analyse_common(uint8_t wk_area, MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PTICKETLOCK p_lock, uint16_t& lenlock, PBOMANALYZE p_analyse, char * p_mode);

	RETINFO analyse_work_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PPURSETRADE p_purse, PBOMETICANALYZE p_eticanalyse);

	RETINFO analyse_free_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PBOMETICANALYZE p_eticanalyse);

	uint32_t add_oper_status(uint8_t wk_area, RETINFO ret, TICKET_PARAM * p_ticket_prm);

	RETINFO entry_gate(PENTRYGATE p_entry, PETICKETDEALINFO p_eticketdealinfo,char cOnlineRes);

	RETINFO exit_gate(PPURSETRADE p_exit, PETICKETDEALINFO p_eticketdealinfo, MODE_EFFECT_FLAGS * p_degrade_falgs,char cOnlineRes);

	RETINFO check_online(ETYTKOPER operType,PETICKETAUTHINFO p_eticketauthinfo);

	RETINFO sale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, POTHERSALE p_svt_sale, uint16_t& len_svt, TICKET_PARAM& ticket_prm);

	RETINFO esale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, PETICKETDEALINFO p_eticketdealinfo, uint16_t& len_eticket_sale, TICKET_PARAM& ticket_prm);

	RETINFO thirdesale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, PETICKETDEALINFO p_eticketdealinfo, uint16_t& len_eticket_sale, TICKET_PARAM& ticket_prm , char * cNetTranNumber);
	
	RETINFO bom_update(PTICKETUPDATE p_update, uint8_t * p_entry_station);

	RETINFO bom_refund(PDIRECTREFUND p_refund);

	//RETINFO svt_increase(PPURSETRADE p_purse, uint8_t * p_time, uint8_t * p_mac2);

	//RETINFO svt_decrease(PPURSETRADE p_purse, uint8_t u_pay_flag);

	RETINFO format_history(uint8_t&	his_count, PHSSVT p_his_array, int his_max);

	RETINFO lock(PTICKETLOCK p_lock);

	RETINFO deffer(PTICKETDEFER p_deffer, TICKET_PARAM& ticket_prm);

	bool last_trade_need_continue(uint8_t status_targ, uint16_t err_targ, uint16_t err_src);

	RETINFO continue_last_trade(void * p_trade);

	RETINFO data_version_upgrade();
	
	RETINFO svt_increase(PPURSETRADE p_purse, uint8_t* p_time, uint8_t* p_mac2, uint8_t paytype);
		
	RETINFO svt_decrease(PPURSETRADE p_purse, uint8_t u_pay_flag, uint8_t paytype);
protected:

	uint8_t m_u_buffer[2048];
	METRO_CPU_INF m_ReadInf;

	uint16_t metro_svt_read(P_METRO_CPU_INF p_mtr_inf);

	METRO_CPU_INF file_need_read(ETYTKOPER type, uint8_t * p_buffer);
	METRO_CPU_INF file_need_write(ETYTKOPER type, P_METRO_CPU_INF p_read_inf);

	void get_issue_base(uint8_t * p_issue_base);
	void get_public_base(uint8_t * p_public_base);
	void get_owner_base(uint8_t * p_owner_base);
	void get_wallet(uint8_t * p_wallet);
	void get_his_all(uint8_t * p_his_all);
	void get_trade_assist(uint8_t * p_trade_assist);
	void get_metro(uint8_t * p_metro);
	void get_ctrl_record(uint8_t * p_ctrl_record);
	void get_app_ctrl(uint8_t * p_app_ctrl);
	void get_change_count(uint8_t * p_charge_count);
	void get_consume_count(uint8_t * p_consume_count);

	RETINFO write_card(ETYTKOPER operType, METRO_CPU_INF write_inf, uint8_t tac_type, long trade_amount, long& sam_seq, char * p_trade_tac);
	uint16_t write_card(uint8_t tac_type, uint8_t * p_app_ctrl, int record_id, uint8_t lock_flag, uint8_t * p_file_data, long& sam_seq);

	uint16_t set_trade_assist(uint8_t * p_trade_assist);
	uint16_t set_metro(uint8_t tac_type, uint8_t * p_metro, uint32_t lValue, uint8_t * p_respond);
	uint16_t set_ctrl_record(uint8_t * p_ctrl_record);
	uint16_t set_app_ctrl(uint8_t * p_app_ctrl);

	RETINFO set_complex_file(uint8_t tac_type, long lAmount, uint8_t * p_trade_assist, uint8_t * p_metro, uint8_t * p_ctrl_record, uint8_t * p_recv);

	bool format_history(uint8_t * p_his_buffer, HSSVT& his);

	void save_last_stack(ETYTKOPER operType, void * p_trade, size_t size_trade, void * p_written_inf, size_t size_written, bool save_to_file);
	
	// 赋值给上次交易未赋值的交易因子
	//void set_confirm_factor(uint8_t status, long sam_seq, char * p_tac);

private:
	SimpleStatus get_simple_status(uint8_t init_status);

	uint16_t read_wallet(long * p_wallet);
};
