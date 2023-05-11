// ParamStructs.h
#pragma once

#include <map>
#include <vector>
#include <stdint.h>
#include <string>
using namespace std;

// 设备控制参数
typedef struct
{
    long		sc_auto_destroy;				// SC操作员无操作自动注销时间 [单位: 秒]
    long		sc_reconnect_lcc;				// SC重新连接LCC的周期 [单位: 秒]
    long		bom_auto_destroy;				// BOM运营人员自动注销时间 [单位: 秒]
    long		entry_exit_timeout;				// 进/出闸TIMEOUT [单位: 秒]
    long		degrade_effect_time;			// 降级模式的影响有效期 [单位：天]
    long		svt_charge_max;					// 储值卡充值钱包充值上限。Unit : 分
    long		loyate_charge_max;				// 积分钱包充值上限。Unit : 次
    long		sjt_sale_max;					// 单程票发售钱包充值上限。Unit : 张
    long		svt_charge_min;					// 储值卡充值钱包充值下限。Unit : 分
    long		loyate_charge_min;				// 积分钱包充值下限。Unit : 次
    long		sjt_sale_min;					// 单程票发售钱包充值下限。Unit : 张

} DEVICE_CTRL, * P_DEVICE_CTRL;

// 车站配置表
typedef struct
{
    uint16_t	station_id;						// 车站表
    uint16_t	device_id;
    uint8_t		device_type;

} STATION_CONFIG, * P_STATION_CONFIG;

// SAM卡对照表
typedef struct
{
    int			record_count;					// 记录条数
    char **		sam_device_info;				// SAM卡号设备信息

} SAM_COMPARE_TABLE, * P_SAM_COMPARE_TABLE;

// 票卡参数结构
typedef struct
{
    uint16_t	ticket_type;					// 票卡类型
    uint8_t		wallet_type;					// 钱包类型
    long		balance_max;					// 车票余额/次上限
    bool		permit_over_draft;				// 是否允许透支
    long		value_over_draft;				// 透支额度
    bool		permit_charge;					// 是否允许充值
    long		charge_max;						// 每次充值上限
    uint8_t		update_fare_mode;				// 更新时收费方式
    bool		check_update_station;			// 是否检查付费区非本站更新
    bool		check_update_day;				// 是否检查付费区非本日更新
    bool		pemite_refund;					// 是否允许退款
    long		refund_min_count;				// 退款时使用次数限制
    long		daliy_trade_max;				// 日乘坐次数上限
    long		month_trade_max;				// 月乘坐次数上线
    long		effect_time_from_sale;			// 有效时间（从发售开始的时间，单位：分钟）
    bool		permit_deffer;					// 是否允许延期
    long		deffer_days;					// 可延长天数
    long		deposite;						// 押金
    long		prime_cost;						// 售价（卡成本）
    long		sale_brokerage;					// 发售手续费
    bool		need_active_for_use;			// 使用前是否需激活
    bool		check_blacklist;				// 是否检查黑名单
    bool		check_balance;					// 是否检查余额/余次

	bool		check_phy_expire;				// 是否检查物理有效期
	bool		check_logic_expire;				// 是否检查逻辑有效期
    //uint8_t		check_order;				// 进出次序检查
    // (0：进出站均不检查,1：进出站均检查,2：进站不检出站检查,3：进站检查出站不检查)
    bool		check_entry_order;
    bool		check_exit_order;
    bool		check_exit_timeout;				// 是否检查超时
    bool		check_over_travel;				// 是否检查超乘
    bool		limit_station;					// 是否限制进出站点
    bool		limit_the_Station;				// 是否只允许本站进出
    char		chargable_device[16];			// 充值设备
    char		usable_device[16];				// 哪种设备可使用
    bool		entry_sell_station;				// 本站进出控制标记
    long		refund_max_value;				// 即时退款余额上限，超过此上限需申请
    long		refund_brokerage;				// 退款手续费
	char		sell_device[16];				// 发售设备
	bool		permit_refund_deposite;			// 是否允许退成本押金

	bool		has_query;						// 表示当前票卡对应的票卡参数是否已经被查询，提高效率

} TICKET_PARAM, * P_TICKET_PARAM;

// 乘次票专用参数
typedef struct
{
    uint16_t ticket_type;						// 票卡类型
    long	once_charge_value;					// 每次充值金额
    long	once_charge_count;					// 每次充值乘次
    uint8_t	charge_start_next_month;			// 下月充值起始日
    uint8_t	charge_end_next_month;				// 下月充值截至日
    long	exit_timeout_penalty_count;			// 出站超时乘次罚金
    long	exit_timeout_penalty_value;			// 出站超时普通罚金
    long	entry_timeout_penalty_count;		// 进站超时乘次罚金
    long	entry_timeout_penalty_value;		// 进站超时普通罚金

} TCT_PARAM, * P_TCT_PARAM;

////////////////////////////////////////////////////////////////
// 收费区段结构，以进站站点作为键值
typedef struct
{
    uint16_t	station_id;
	uint16_t	exit_station_id;
    uint8_t		fare_zone;						// 收费区段
    uint16_t	ride_time_limit;				// 乘车时间限制
    uint16_t	penalty_for_timeout;			// 超时罚金

} FARE_ZONE, * P_FARE_ZONE;


////////////////////////////////////////////////////////////////
// 每个站点需要保存磁浮的站点区段
//typedef struct
//{
//    uint16_t	cf_station_id;
//	uint16_t	cf_exit_station_id;
//    uint8_t		fare_zone;						// 收费区段
//    uint16_t	ride_time_limit;				// 乘车时间限制
//    uint16_t	penalty_for_timeout;			// 超时罚金
//
//} CF_FARE_ZONE, * P_CF_FARE_ZONE;

// 收费配置结构，以本结构作为键值
typedef struct
{
    uint16_t	ticket_type;					// 票价表ID
    uint8_t		ride_time_code;					// 乘车时间代码
    uint16_t	fare_table_id;					// 票价表ID
} FARE_CONFIG_TABLE, * P_FARE_CONFIG_TABLE;

// 票价结构
typedef struct
{
    uint16_t	fare_table_id;					// 票价表ID
    uint8_t		fare_zone;						// 收费区段
    long		fare;

} FARE_TABLE, * P_FARE_TABLE;

// 节假日结构
typedef struct
{
    uint8_t		holiday_code;					// 节假日代码
    uint8_t		start_date[4];					// 开始时间
    uint8_t		end_date[4];					// 结束时间

} HOLIDAY_TABLE, * P_HOLIDAY_TABLE;

// 非繁忙时间结构
typedef struct
{
    uint8_t		weekday;						// 星期
    uint8_t		start_time[2];					// 开始时间
    uint8_t		end_time[2];					// 结束时间

} IDLE_TIME_TABLE, * P_IDLE_TIME_TABLE;


///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// ITP参数   收费区段结构，以进站站点作为键值
typedef struct
{
    uint16_t	station_id;
	uint16_t	exit_station_id;
    uint8_t		fare_zone;						// 收费区段
    uint16_t	ride_time_limit;				// 乘车时间限制
    uint16_t	penalty_for_timeout;			// 超时罚金

} ITP_FARE_ZONE, * P_ITP_FARE_ZONE;

// 收费配置结构，以本结构作为键值
typedef struct
{
    uint16_t	ticket_type;					// 票价表ID
    uint8_t		ride_time_code;					// 乘车时间代码
    uint16_t	fare_table_id;					// 票价表ID
} ITP_FARE_CONFIG_TABLE, * P_ITP_FARE_CONFIG_TABLE;

// 票价结构
typedef struct
{
    uint16_t	fare_table_id;					// 票价表ID
    uint8_t		fare_zone;						// 收费区段
    long		fare;

} ITP_FARE_TABLE, * P_ITP_FARE_TABLE;

// 节假日结构
typedef struct
{
    uint8_t		holiday_code;					// 节假日代码
    uint8_t		start_date[4];					// 开始时间
    uint8_t		end_date[4];					// 结束时间

} ITP_HOLIDAY_TABLE, * P_ITP_HOLIDAY_TABLE;

// 非繁忙时间结构
typedef struct
{
    uint8_t		weekday;						// 星期
    uint8_t		start_time[2];					// 开始时间
    uint8_t		end_time[2];					// 结束时间

} ITP_IDLE_TIME_TABLE, * P_ITP_IDLE_TIME_TABLE;

///////////////////////////////////////////////////////////////
// 黑名单参数
// 黑名单单号
typedef struct
{
    int		record_count;						// 记录条数
    char ** logical_id;							// 逻辑卡号

} SINGLE_BLACK, * P_SINGLE_BLACK;

typedef struct
{
    int		record_count;						// 记录条数
    char ** issue_code;							// 发卡机构代码
	char ** issue_ident_no;					    // 发卡机构识别码IIN

} SINGLE_WHITE, * P_SINGLE_WHITE;

// 行政罚金参数，以此结构为键值
typedef struct
{
    uint8_t		policy_code;					// 行政代码
    uint16_t	ticket_type;					// 票卡类型
	uint16_t	line_station;					// 车站代码
    uint16_t	penalty;						// 罚金
} POLICY_PENALTY, * P_POLICY_PENALTY;

// 降级模式 《ITP与SLE系统脱、联机报文接口规范》
typedef enum DEGRADE_TYPE
{
    degrade_null				= 0,
    degrade_Trouble				= 1, //列车故障模式
    degrade_Uncheck_Order		= 2, //进站免检模式
    degrade_Uncheck_DateTime	= 3, //日期免检模式
    degrade_Uncheck_Fare		= 5, //车费免检模式
    degrade_Green_Light			= 6  //紧急放行模式

} DGTYPE;

typedef struct
{
    uint16_t	degrade_station;				// 发生降级的站点
    DGTYPE		degrade_type;					// 降级模式的类型
    uint8_t		degrade_start_time[7];			// 降级模式的开始时间
    uint8_t		degrade_end_time[7];			// 降级模式结束时间
    uint8_t		degrade_susceptive[7];			// 降级模式影响期结束时间，在使用时计算

} DEGRADE_MODE, * P_DEGRADE_MODE;

// 当前票卡所受的降级模式影响
typedef struct
{
    bool		emergency;						// 紧急模式
    bool		train_trouble;					// 列车故障
    bool		uncheck_order;					// 进出次序免检
    bool		uncheck_datetime;				// 有效期时间免检
    bool		uncheck_fare;					// 车费免检

    bool		entry_by_emergency;				// 已进站的车票是否受紧急模式敏感期影响
    bool		es_sjt_by_emergency;			// ES的单程票是否受紧急模式敏感期影响
    bool		sell_sjt_by_emergency;			// 发售的单程票否受紧急模式敏感期影响
    bool		exit_train_trouble_effect;		// 列车故障模式出站的单程票是否受列车故障的敏感期影响
    bool		es_sjt_by_train_trouble;		// ES的单程票是否受列车故障模式敏感期影响
    bool		sell_sjt_by_train_trouble;		// 发售的单程票是否受列车故障模式敏感期影响
	bool		update_free_by_emergency;		// 非付费区更新的单程票否受紧急模式敏感期影响
	bool		update_work_by_emergency;		// 付费区更新车票否受紧急模式敏感期影响
} MODE_EFFECT_FLAGS, * P_MODE_EFFECT_FLAGS;


typedef struct
{
	vector<FARE_ZONE>			vec_zone;
	vector<FARE_CONFIG_TABLE>	vec_fare_config;
	vector<FARE_TABLE>			vec_fare_table;
	vector<HOLIDAY_TABLE>		vec_holiday_table;
	vector<IDLE_TIME_TABLE>		vec_idle_time;
	vector<FARE_ZONE>			vec_cf_zone;

} FARE_PARAM, * P_FARE_PARAM;


typedef struct
{
	vector<ITP_FARE_ZONE>			vec_itp_zone;
	vector<ITP_FARE_CONFIG_TABLE>	vec_itp_fare_config;
	vector<ITP_FARE_TABLE>			vec_itp_fare_table;
	vector<ITP_HOLIDAY_TABLE>		vec_itp_holiday_table;
	vector<ITP_IDLE_TIME_TABLE>		vec_itp_idle_time;
	vector<ITP_FARE_ZONE>			vec_itp_cf_zone;

} ITP_FARE_PARAM, * P_ITP_FARE_PARAM;

typedef struct
{
	SINGLE_BLACK					blacks_single;					// 黑名单单号
	vector< std::pair<string, string> >	blacks_section;				// 黑名单段号

} BLACK_PARAM, * P_BLACK_PARAM;

typedef struct
{
	SINGLE_WHITE					whites_single;					// 白名单单号
	vector< std::pair<string, string> >	whites_section;				// 白名单段号
} WHITE_PARAM, * P_WHITE_PARAM;
