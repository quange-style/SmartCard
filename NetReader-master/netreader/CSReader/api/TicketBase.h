#pragma once

#include "ParamMgr.h"
#include <stdint.h>
#include "Structs.h"
#include "CmdSort.h"
#include "Declares.h"
#include "Records.h"

#define _IN_P(bits, ptr)		bits, &ptr, (int)sizeof(ptr)
#define _IN_NULL(bits)			bits, 0, 0
#define _OUT_B(bits, val)		bits, val
#define _OUT_NULL(bits)			0, bits				// ��ʱ����ռλ�����ڶ���Ԫ�ر����ռ��λ��
#define ENDLE	0, 0

#define SEARCH_CARD_ERR_TIME		100

extern uint8_t * PTR_TICKET;
extern size_t SIZE_TICKET;
extern int g_rfmain_status;
extern int g_rfsub_status;

class TicketBase
{
public:
	enum MStatus
	{
		MS_Init				= (uint8_t)1,	// ��ʼ��
		MS_Es				= (uint8_t)2,	// Ԥ��ֵ
		MS_Sale				= (uint8_t)3,	// BOM����
		MS_Entry			= (uint8_t)4,	// ��բ
		MS_Exit				= (uint8_t)5,	// ��բ
		MS_Exit_Only		= (uint8_t)6,	// ��վƱ
		MS_Exit_Tt			= (uint8_t)7,	// �г�����ģʽ��վ
		MS_Upd_FZone_Free	= (uint8_t)8,	// �Ǹ�������Ѹ���
		MS_Upd_FZone_Fare	= (uint8_t)9,	// �Ǹ��������Ѹ���
		MS_Upd_Wzone_Entry	= (uint8_t)10,	// ��������վ�����
		MS_Upd_Wzone_Exit	= (uint8_t)11,	// ��������վ�����
		MS_Upd_Wzone_Time	= (uint8_t)12,	// ��������ʱ����
		MS_Upd_Wzone_Trip	= (uint8_t)13,	// ���������̸���
		MS_Upd_Wzone_Free	= (uint8_t)14,	// ��������Ѹ���
		MS_Refund			= (uint8_t)15,	// �˿�
		MS_Increase			= (uint8_t)16,	// ��ֵ(��ʷ��ר��)
		MS_Decrease			= (uint8_t)17,	// ��ֵ(��ʷ��ר��)
		MS_Destroy			= (uint8_t)18,	// ע��
		MS_Lock				= (uint8_t)19,	// ����(��ʷ��ר��)
		MS_Unlock			= (uint8_t)20,	// ����(��ʷ��ר��)
		MS_Deffer			= (uint8_t)21	// ����(��ʷ��ר��)
	};

public:
    TicketBase(void);
    TicketBase(char * p_current_sam, char * p_sam_posid, uint8_t * physic_info);

	static uint16_t search_card(TicketBase ** pp_ticket, uint8_t * p_forbid_ant = NULL, ETPMDM * p_forbid_type = NULL, uint8_t * p_current_id = NULL, ETYTKOPER oper_type = operNone);

	static uint16_t search_card_div(char * cCenterCodeIn, char * type);
	//static uint16_t search_qr_card(TicketBase ** pp_ticket, uint8_t * p_forbid_ant = NULL, ETPMDM * p_forbid_type = NULL, uint8_t * p_current_id = NULL, ETYTKOPER oper_type = operNone);

    void transfer_data_for_out(ETYTKOPER operType, void * pTradeStruct);

	void transfer_jtb_data_for_out(ETYTKOPER operType, void * pTradeStruct);
	//�ظ�Ϊ�麯����Ϊ�ѻ�����Ʊ���ิд
	virtual void transfer_edata_for_out(ETYTKOPER operType, void * pTradeStruct/*, uint8_t bPaymentMeans*/);

	static void getDealDataUpload(ETYTKOPER operType,uint16_t& len_data,uint8_t * dataBack,char * succMount, char * failMount/*, char * date*/);

    void invalid_time_change_current(uint8_t * p_time_invalid);

    void get_ticket_infos(TICKET_DATA& ticket_data);

	void get_itpData(uint8_t* param_stack);
	//
    // ��������Ʊ�����У�����ʵ���ϲ�ͬ�Ľӿ�ȫ������Ϊ���麯��
    //
    virtual RETINFO read_card(ETYTKOPER operType, TICKET_PARAM& ticket_prm, uint8_t * p_read_init = NULL) = 0;

    // ͳһƱ��״̬�������ͽ����е�״̬���ô�״̬
    virtual uint8_t unified_status();

    virtual RETINFO analyse_common(uint8_t wk_area, MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PTICKETLOCK p_lock, uint16_t& lenlock, PBOMANALYZE p_analyse, char * p_mode) = 0;

	virtual RETINFO analyse_work_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PPURSETRADE p_purse, PBOMETICANALYZE p_eticanalyse);

	virtual RETINFO analyse_free_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PBOMETICANALYZE p_eticanalyse);

	virtual uint32_t add_oper_status(uint8_t wk_area, RETINFO ret, TICKET_PARAM * p_ticket_prm);

    virtual RETINFO entry_gate(PENTRYGATE p_entry,PETICKETDEALINFO p_eticketdealinfo,char cOnlineRes) = 0;

    virtual RETINFO exit_gate(PPURSETRADE p_exit,PETICKETDEALINFO p_eticketdealinfo, MODE_EFFECT_FLAGS * p_degrade_falgs,char cOnlineRes) = 0;

	virtual RETINFO check_online(ETYTKOPER operType,PETICKETAUTHINFO p_eticketauthinfo) = 0;

    virtual RETINFO sale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, POTHERSALE p_svt_sale, uint16_t& len_svt, TICKET_PARAM& ticket_prm);

	virtual RETINFO esale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, PETICKETDEALINFO p_eticketdealinfo, uint16_t& len_eticket_sale, TICKET_PARAM& ticket_prm);

	virtual RETINFO thirdesale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, PETICKETDEALINFO p_eticketdealinfo, uint16_t& len_eticket_sale, TICKET_PARAM& ticket_prm , char * cNetTranNumber);

	//virtual RETINFO getETicket(PETICKETDEALINFO p_eticketdealinfo);
	virtual RETINFO getETicket(PGETETICKETRESPINFO p_geteticketrespinfo);

	virtual RETINFO exit_sjt_sale(PSJTSALE p_exit_sjt_sale);

	virtual RETINFO sjt_decrease();

    virtual RETINFO bom_update(PTICKETUPDATE p_update, uint8_t * p_entry_station) = 0;
    virtual RETINFO bom_elect_update(PETICKET_ABNORML_HANDLE p_update, uint8_t* dataBack, uint16_t& len_data);


    virtual RETINFO bom_refund(PDIRECTREFUND p_refund);

	//virtual RETINFO svt_increase(PPURSETRADE p_purse, uint8_t * p_time, uint8_t * p_mac2);
	//֧����ʽĬ��Ϊ01 �ֽ�
	virtual RETINFO svt_increase(PPURSETRADE p_purse, uint8_t* p_time, uint8_t* p_mac2, uint8_t paytype=1);

	virtual RETINFO EPayData(ETICKETPAYDEALINFO p_purse, uint8_t* p_time, char* paytype);

#define PAY_UPD_PENALTY		(uint8_t)1
#define PAY_INC_DESIND		(uint8_t)2
#define PAY_BUY_TOKEN		(uint8_t)3
	//virtual RETINFO svt_decrease(PPURSETRADE p_purse, uint8_t u_pay_flag);
	//֧����ʽĬ��Ϊ01 �ֽ�
	virtual RETINFO svt_decrease(PPURSETRADE p_purse, uint8_t u_pay_flag, uint8_t paytype=1);

	virtual RETINFO format_history(uint8_t&	his_count, PHSSVT p_his_array, int his_max);

	virtual RETINFO lock(PTICKETLOCK p_lock);

	virtual RETINFO deffer(PTICKETDEFER p_deffer, TICKET_PARAM& ticket_prm);

	virtual bool last_trade_need_continue(uint8_t status_targ, uint16_t err_targ, uint16_t err_src);

	virtual RETINFO continue_last_trade(void * p_trade);

	/*virtual */void unlock_reader(void * p_trade);

	virtual RETINFO data_version_upgrade();

	void clear_confirm(char * p_logic_id, bool check_logic_id, bool save_to_file);

	void clear_confirm_e(char * cCenterCode, bool check_cCenterCode, bool save_to_file);

	size_t size_of_confirm_trade();

	ETYTKOPER confirm_trade_type();

	// ��ȡ����ȷ����ʷ��Ϣ
	static void load_confirm_from_file();

	// ����Ƿ������Ҫ����ȷ�ϵ���ʷ
	static bool check_confirm_his(int time_limit_min);

	bool confirm_current_card();

	// ��ȡ��ǰ�����Ľ���ģʽ��������Ӱ����Ϣ
	void get_degrade_mode(MODE_EFFECT_FLAGS& degrade_falgs, uint8_t * p_time_now);

protected:

	static TICKET_DATA m_ticket_data;

    const char * m_p_current_sam;
	const char * m_p_sam_posid;

	//static int search_card_err;

	static CONFIRM_POINT cfm_point;

	//
    // ������Ϻ�_pairʹ�ã��������©����������쳣
	//
    //uint8_t cmd_sprintf(int nStart, uint8_t * p_dst_buf, ...);

	//
    // ������Ϻ�_thrʹ�ã��������©����������쳣
	//
    //void cmd_scanf(int nStart, uint8_t * p_src_buf, ...);

    bool station_is_current(uint16_t station_id);

    void current_trade_info(TradeInfo& src_info);

	void current_jtbtrade_info(JTBTradeInfo& src_info);

	bool metro_area_crc_valid(uint8_t * p_metro, uint16_t len_area);

	void add_metro_area_crc(uint8_t * p_metro, uint16_t len_area);

	// ����д���ϵ���Ϣ
	void save_last_stack(ETYTKOPER operType, void * p_trade, size_t size_trade, void * p_written_inf, size_t size_written, bool save_to_file);

	// ���ݽ���ȷ�Ͻṹ����ȡ����ȷ������
	static CONFIRM_FACTOR get_confirm_factor();

	// ��ֵ���ϴν���δ��ֵ�Ľ�������
	/*virtual*/ void set_confirm_factor(uint8_t status, long sam_seq, char * p_tac);

	// ����Ƿ񸴺Ͻ���ȷ�ϵĻ�������
	bool check_confirm(uint8_t status_targ, uint8_t status_src, uint16_t err_targ, uint16_t err_src);

	// �����Ƿ�֧�ֽ���ȷ��
	uint8_t support_confirm(ETYTKOPER operType);

	// ������ȷ�Ͻṹ���浽�ļ�
	void save_confirm_to_file();

	bool need_check_period(uint8_t wk_area, SimpleStatus s_status);

	static CONFIRM_POINT_E cfm_point_e;
	//����Ʊ����ȷ������
	//����д���ϵ���Ϣ
	void save_last_stack_eticket(ETYTKOPER operType, void * p_trade, size_t size_trade, void * p_written_inf, size_t size_written, bool save_to_file);

	// ���ݽ���ȷ�Ͻṹ����ȡ����ȷ������
	static CONFIRM_POINT_E get_confirm_factor_e();

	//���뽻�׽ṹ�ڴ��Сָ��
	uint8_t * pMemoryPos;
	//��ͨ���� ���׽ṹ�ڴ��Сָ��
	uint8_t * pMemoryPos1;

};

