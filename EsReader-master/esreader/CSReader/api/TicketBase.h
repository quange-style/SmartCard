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
#define _OUT_NULL(bits)			0, bits				// 此时用于占位符，第二个元素变成所占的位数
#define ENDLE	0, 0

// 交易信息结构，可以是进站，出站、更新等
typedef struct
{
    uint16_t		station_id;				// 站点
    ETPDVC			dvc_type;				// 设备类型
    uint16_t		dvc_id;					// 设备编码
    uint8_t			time[7];				// 交易时间
    char			sam[17];				// SAM卡号
} TradeInfo, * PTradeInfo;

// 卡数据结构
typedef struct
{
    ETPMDM			physical_type;			// 物理类型
	char			issue_code[4];			// 发行方主代码
	char			city_code[4];			// 城市代码
	char			industry_code[4];		// 行业代码
    char			physical_id[21];		// 物理卡号
    char			logical_id[21];			// 逻辑卡号
    uint8_t			key_flag;				// 应用标识（正式票，测试票）

    uint8_t			date_issue[4];			// 发行时间
    uint8_t			phy_peroidE[4];			// 物理有效期
    uint8_t			logical_peroidS[7];		// 逻辑有效期
    uint8_t			logical_peroidE[7];
	int				effect_mins;			// 有效分钟数（发售时写入发售后的有效期时间，延期时加上可延期时间）

	char			certificate_name[128];	// 证件持有人姓名
	char			certificate_code[32];	// 证件代码
	uint8_t			certificate_type;		// 证件类型

	uint8_t			cert_peroidE[4];		// 证件有效期
    uint8_t			charge_peroidS[4];		// 充值有效期
    uint8_t			charge_peroidE[4];
    uint8_t			active_peroidS[7];		// 激活有效期
    uint8_t			active_peroidE[7];

    uint8_t			logical_type[2];		// 逻辑票卡类型
	uint8_t			fare_type[2];			// 票价类型
    long			wallet_value;			// 钱包余值
    long			deposit;				// 押金
    long			wallet_value_max;		// 钱包上限
    uint8_t			list_falg;				// 名单标识
    uint16_t		charge_counter;			// 充值累计
    long			daliy_trade_counter;	// 日交易次数
    long			total_trade_counter;	// 总交易次数
    long			daliy_update_counter;	// 日更新次数
    long			total_update_counter;	// 总更新次数
	uint8_t			app_lock_flag;			// 应用锁定标记（发售或多日票激活时使用）
    uint8_t			globle_status;			// 全局状态(初始化，可使用，退款，)
    uint8_t			init_status;			// 原始状态
    SimpleStatus	simple_status;			// 转换为标准的票卡状态

	uint8_t			limit_mode;				// 限制进出模式
	uint8_t			limit_entry_station[2];	// 限制进站站点
	uint8_t			limit_exit_station[2];	// 限制出站站点

    PTradeInfo		p_entry_info;			// 进站信息
    PTradeInfo		p_update_info;			// 更新信息
    PTradeInfo		p_exit_info;			// 出站信息
    TradeInfo		read_last_info;			// 上次交易信息，赋值用read_last_info，写卡用write_last_info
    TradeInfo		write_last_info;

	char			ticket_app_mode;		// 卡应用模式

} TICKET_DATA, * P_TICKET_DATA;

extern uint8_t * PTR_TICKET;
extern size_t SIZE_TICKET;
extern int g_rfmain_status;
extern int g_rfsub_status;

class TicketBase
{
public:
	enum MStatus
	{
		MS_Init				= (uint8_t)1,	// 初始化
		MS_Es				= (uint8_t)2,	// 预赋值
		MS_Sale				= (uint8_t)3,	// BOM发售
		MS_Entry			= (uint8_t)4,	// 进闸
		MS_Exit				= (uint8_t)5,	// 出闸
		MS_Exit_Only		= (uint8_t)6,	// 出站票
		MS_Exit_Tt			= (uint8_t)7,	// 列车故障模式出站
		MS_Upd_FZone_Free	= (uint8_t)8,	// 非付费区免费更新
		MS_Upd_FZone_Fare	= (uint8_t)9,	// 非付费区付费更新
		MS_Upd_Wzone_Entry	= (uint8_t)10,	// 付费区入站码更新
		MS_Upd_Wzone_Exit	= (uint8_t)11,	// 付费区出站码更新
		MS_Upd_Wzone_Time	= (uint8_t)12,	// 付费区超时更新
		MS_Upd_Wzone_Trip	= (uint8_t)13,	// 付费区超程更新
		MS_Upd_Wzone_Free	= (uint8_t)14,	// 付费区免费更新
		MS_Refund			= (uint8_t)15,	// 退款
		MS_Increase			= (uint8_t)16,	// 充值(历史区专用)
		MS_Decrease			= (uint8_t)17,	// 减值(历史区专用)
		MS_Destroy			= (uint8_t)18,	// 注销
		MS_Lock				= (uint8_t)19,	// 加锁(历史区专用)
		MS_Unlock			= (uint8_t)20,	// 解锁(历史区专用)
		MS_Deffer			= (uint8_t)21	// 延期(历史区专用)
	};

public:
    TicketBase(void);
    TicketBase(char * p_current_sam, char * p_sam_posid, uint8_t * physic_info);

	static uint16_t search_card(TicketBase ** pp_ticket, uint8_t * p_forbid_ant = NULL, ETPMDM * p_forbid_type = NULL, uint8_t * p_current_id = NULL, ETYTKOPER oper_type = operNone);

    void transfer_data_for_out(ETYTKOPER operType, void * pTradeStruct);

    void invalid_time_change_current(uint8_t * p_time_invalid);

    void get_ticket_infos(TICKET_DATA& ticket_data);

	//
    // 对于所有票卡都有，但是实现上不同的接口全部定义为纯虚函数
    //
    virtual RETINFO read_card(ETYTKOPER operType, TICKET_PARAM& ticket_prm, uint8_t * p_read_init = NULL) = 0;

    // 统一票卡状态，分析和交易中的状态都用此状态
    virtual uint8_t unified_status();

    virtual RETINFO analyse_common(uint8_t wk_area, MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PTICKETLOCK p_lock, uint16_t& lenlock, PBOMANALYZE p_analyse, char * p_mode) = 0;

	virtual RETINFO analyse_work_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PPURSETRADE p_purse);

	virtual RETINFO analyse_free_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse);

	virtual uint32_t add_oper_status(uint8_t wk_area, RETINFO ret, TICKET_PARAM * p_ticket_prm);

    virtual RETINFO entry_gate(PENTRYGATE p_entry) = 0;

    virtual RETINFO exit_gate(PPURSETRADE p_exit, MODE_EFFECT_FLAGS * p_degrade_falgs) = 0;

    virtual RETINFO sale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, POTHERSALE p_svt_sale, uint16_t& len_svt, TICKET_PARAM& ticket_prm);

	virtual RETINFO exit_sjt_sale(PSJTSALE p_exit_sjt_sale);

	virtual RETINFO sjt_decrease();

    virtual RETINFO bom_update(PTICKETUPDATE p_update, uint8_t * p_entry_station) = 0;

    virtual RETINFO bom_refund(PDIRECTREFUND p_refund);

	virtual RETINFO svt_increase(PPURSETRADE p_purse, uint8_t * p_time, uint8_t * p_mac2);

#define PAY_UPD_PENALTY		(uint8_t)1
#define PAY_INC_DESIND		(uint8_t)2
#define PAY_BUY_TOKEN		(uint8_t)3
	virtual RETINFO svt_decrease(PPURSETRADE p_purse, uint8_t u_pay_flag);

	virtual RETINFO format_history(uint8_t&	his_count, PHSSVT p_his_array, int his_max);

	virtual RETINFO lock(PTICKETLOCK p_lock);

	virtual RETINFO deffer(PTICKETDEFER p_deffer, TICKET_PARAM& ticket_prm);

	virtual bool last_trade_need_continue(uint8_t status_targ, uint16_t err_targ, uint16_t err_src);

	virtual RETINFO continue_last_trade(void * p_trade);

	/*virtual */void unlock_reader(void * p_trade);

	virtual RETINFO data_version_upgrade();

	void clear_confirm(char * p_logic_id, bool check_logic_id, bool save_to_file);

	size_t size_of_confirm_trade();

	ETYTKOPER confirm_trade_type();

	// 读取交易确认历史信息
	static void load_confirm_from_file();

	// 检测是否存在需要交易确认的历史
	static bool check_confirm_his(int time_limit_min);

	bool confirm_current_card();

	// 获取当前发生的降级模式和敏感期影响信息
	void get_degrade_mode(MODE_EFFECT_FLAGS& degrade_falgs, uint8_t * p_time_now);

protected:

    TICKET_DATA m_ticket_data;

    const char * m_p_current_sam;
	const char * m_p_sam_posid;

	static CONFIRM_POINT cfm_point;

	//
    // 函数配合宏_pair使用，以免参数漏掉参数造成异常
	//
    //uint8_t cmd_sprintf(int nStart, uint8_t * p_dst_buf, ...);

	//
    // 函数配合宏_thr使用，以免参数漏掉参数造成异常
	//
    //void cmd_scanf(int nStart, uint8_t * p_src_buf, ...);

    bool station_is_current(uint16_t station_id);

    void current_trade_info(TradeInfo& src_info);

	bool metro_area_crc_valid(uint8_t * p_metro, uint16_t len_area);

	void add_metro_area_crc(uint8_t * p_metro, uint16_t len_area);

	// 保存写卡断点信息
	void save_last_stack(ETYTKOPER operType, void * p_trade, size_t size_trade, void * p_written_inf, size_t size_written, bool save_to_file);

	// 根据交易确认结构，获取交易确认因子
	static CONFIRM_FACTOR get_confirm_factor();

	// 赋值给上次交易未赋值的交易因子
	/*virtual*/ void set_confirm_factor(uint8_t status, long sam_seq, char * p_tac);

	// 检查是否复合交易确认的基本条件
	bool check_confirm(uint8_t status_targ, uint8_t status_src, uint16_t err_targ, uint16_t err_src);

	// 操作是否支持交易确认
	uint8_t support_confirm(ETYTKOPER operType);

	// 将交易确认结构保存到文件
	void save_confirm_to_file();

	bool need_check_period(uint8_t wk_area, SimpleStatus s_status);
};

