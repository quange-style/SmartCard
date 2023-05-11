// ParamMgr

#pragma once

#include "Declares.h"
#include "ParamStructs.h"
#include "QFile.h"
#include <stdint.h>

class ParamMgr
{
public:
    typedef int (ParamMgr::* P_FUNC_PRM)(const char *, bool);
    typedef struct
    {
        uint16_t	prm_type;
        P_FUNC_PRM		fun;
    } PRM_FUNC_LIST;

	typedef int (ParamMgr::* P_FUNC_IPRM)(const char *, bool);
	typedef struct
	{
		uint16_t	iprm_type;
		P_FUNC_IPRM		ifun;
	} IPRM_FUNC_LIST;
public:

    ParamMgr(void);
    ~ParamMgr(void);

    bool load_dat_config();

    // 参数类型是否读写器必须
    bool prm_for_reader(uint16_t prm_type);

	// 参数是否则配置文件中
	bool name_in_config(char * p_file_name);

    // 保存参数配置到文件
    bool save_prm_config(uint16_t prm_type, const string& strPath);

    // 查询参数版本
    bool query_prm_version(uint16_t prm_type, char * p_prm_name);

    // 加载所有参数
    int load_all_prms();

    // 加载单个参数
    uint16_t load_prm(const uint16_t prm_type, const char * p_prm_path, bool cmd_reset = false, bool only_check = false);

    // 查询设备控制参数
    void query_device_ctrl(DEVICE_CTRL& ctrl);

    // 查询站点
    uint16_t valid_station(uint16_t station_id);

    // 查询对应的设备是否注册
    int device_registed(uint16_t station_id, uint8_t device_type, uint16_t device_id);

    // 查询SAM卡对照表
    bool device_from_sam(const char * p_sam_id, const uint8_t sam_type, uint8_t * p_bcd_station, uint8_t * p_device_type, uint8_t * p_bcd_device_id);

    // 查询SAM卡对照表
	bool sam_from_device(const uint8_t * p_bcd_station, const ETPDVC device_type, const uint8_t * p_bcd_device_id, const uint8_t sam_type, char * p_sam_id);
	bool sam_from_device(const uint16_t station_id, const ETPDVC device_type, const uint16_t device_id, const uint8_t sam_type, char * p_sam_id);
	// 查询SAM卡号是否对应当前设备
	bool sam_counterpart(uint8_t sam_sock, char * p_sam_id, uint8_t * p_station, uint8_t dev_type, uint8_t * p_dev_id, bool check_sam_match);

    // 查询票卡参数
    int query_ticket_prm(uint8_t * p_ticket_type, TICKET_PARAM& prm);

    // 查询乘此票参数
    int query_tct_prm(uint8_t * p_ticket_type, TCT_PARAM& prm);

    // 设备是否支持当前票种
    static uint16_t device_support_ticket(ETPDVC target_device_type, char * p_ticket_usable_device);

	// 设备是否支持当前票卡充值
	static uint16_t device_support_charge(ETPDVC target_device_type, char * p_ticket_charge_device);

	// 设备是否支持当前票卡发售
	static uint16_t device_support_sale(ETPDVC target_device_type, char * p_ticket_sell_device);

	// 是否可发售，发售设备全部为0时不允许发售
	static bool permit_sale(char * p_ticket_sell_device);

    // 查找地铁黑名单
    bool query_black_mtr(char * p_mtr_card_logical_id);

    // 查找一卡通黑名单
    bool query_black_oct(char * p_oct_card_logical_id);

	// 查找交通部黑名单
    bool query_black_jtb(char * p_jtb_card_logical_id);

	// 查找交通部白名单
    bool query_white_jtb(char * p_jtb_card_issue_code);

	// 判断是否是互联互通票卡
	bool is_interconnect(uint8_t * interCardFlag, uint8_t * citycode);

    // 是否出站超时
    bool exit_timeout(uint16_t entry_station, uint8_t *  p_time_entry, uint8_t *  p_time_exit, long * p_penalty = NULL);

	// 获取有效出站时间
    bool get_validexittime(uint16_t entry_station, uint8_t *  p_time_entry, uint8_t *  p_time_exit, long * p_penalty = NULL);
	

    // 查询票价,lTrade为交易的实际金额，lFare为原票价（单程票票价）
    uint16_t query_fare(uint8_t * p_time_now, uint8_t * p_ticket_type,
                   uint16_t entry_station, uint8_t * p_time_entry, long * p_fare_actual, long * p_fare_init = NULL/**/);

    // 查最低票价
    int query_lowest_fare(uint8_t * p_time_now, uint8_t * p_ticket_type, long& fare_lowest_actual, long * p_fare_lowest_init = NULL);

	// 查磁浮最低票价
    int query_cf_lowest_fare(uint8_t * p_time_now, uint8_t * p_ticket_type, uint16_t entry_station,long& fare_lowest_actual, long * p_fare_lowest_init = NULL);

	//查询线路最低票价
	int query_line_lowest_fare(uint8_t * p_time_now, uint16_t entry_station,uint8_t * p_ticket_type, long& fare_lowest_actual, long * p_fare_lowest_ini);

	//查询到本站最高票价
	int query_highest_fare(uint8_t * p_time_now, /*uint8_t * p_ticket_type, long& fare_lowest_actual, */long * p_fare_lowest_init = NULL);

	// 根据票价查找收费区段
	uint16_t fare_zone_by_sjt_fare(uint8_t * p_ticket_type, long fare, uint8_t& fare_zone);

    // 是否相同车站
    bool same_station(uint16_t station_id0, uint16_t station_id1);

    // 检查参数合法性
    uint16_t valid_prm(const char * p_prm_pathname);

    // 更改降级模式，用于接收降级消息
	void modify_degrade(uint8_t type, uint8_t * p_time, uint8_t * p_station, uint8_t flag);

    // 查询当前降级模式
    void query_degrade_mode(MODE_EFFECT_FLAGS& mdEffect);

	// 降级模式历史记录是否对当前票卡有影响
	// prepared_period：是否有预生效期， p_time_now：现在时间，p_time_last：上次对应的操作时间，station_last：上次对应的操作站点
	bool tk_in_mode(DGTYPE mode, bool prepared_period, uint8_t * p_time_now, uint8_t * p_time_last, uint16_t station_last);

    // ES的单程票票卡是否受指定模式敏感期的影响(ES的有效期在模式开始或开始之后)
    bool EsEffectByAppointedMode(DGTYPE dgAppointed, uint8_t * pCurTime, uint8_t * pLExpireE, uint16_t UseStation = 0);

    // 查询闸机通道参数
    uint16_t passageway_allow_pass(uint8_t * p_ticket_type, uint8_t cur_door_type);

    // 查找行政罚金参数
    int query_policy_penalty(uint8_t * p_ticket_type, int policy_code, short& penalty);

    uint16_t config_param(char * p_name_prm);

    uint16_t get_prm_info(uint16_t prm_type, char * p_prm_name);

	// 删除配置文件之外的参数
	void delete_prm_outof_config();

	// 设备是否完成初始化，即加载参数（读写器异常重启后，设备程序无法知晓，导致没有调用设备初始化）
	void device_initialize();
	uint16_t device_initialized();

	// 查找离本站最近的发生进出次序免检的车站
	uint16_t nearest_uncheckorder_station();


	uint16_t config_param_itp(char * p_name_prm);

	// 检查参数合法性
	uint16_t valid_prm_itp(const char * p_prm_pathname);

	// 加载单个参数
	uint16_t load_prm_itp(const uint16_t prm_type, const char * p_prm_path, bool cmd_reset = false, bool only_check = false);

	uint16_t get_prm_info_itp(uint16_t iprm_type, char * p_iprm_name);

	// 更改参数配置
	bool save_prm_config_itp(uint16_t prm_type, const string& strPath);

	bool load_itp_dat_config();

	// ITP参数类型是否读写器必须
    bool iprm_for_reader(uint16_t prm_type);

private:
    static PRM_FUNC_LIST m_prm_for_reader[];
	static IPRM_FUNC_LIST m_iprm_for_reader[];

    // 参数类型和文件路径信息
    map<uint16_t, string>	m_prm_config;

	// 参数类型和文件路径信息
	map<uint16_t, string>	m_iprm_config;

    // 车站参数
    vector<STATION_CONFIG> m_prm_station_config;

    // 设备控制参数
    DEVICE_CTRL m_prm_device_ctrl;

    // sam卡对照表
    SAM_COMPARE_TABLE m_prm_sam_compare;

    // 换乘车站表
    vector< std::pair<uint16_t, uint16_t> > m_prm_changing_station;

    // 票卡参数
    vector<TICKET_PARAM> m_prm_ticket;

    // 票价表
    FARE_PARAM m_prm_fare;

    // 降级模式
    vector<DEGRADE_MODE> m_prm_degrade;

    // 乘次票专用参数
    vector<TCT_PARAM> m_prm_tct;

    // 专用通道参数
    map<uint16_t, bool> m_prm_passageway;

    // 一卡通黑名单
    BLACK_PARAM m_prm_oct_black;

    // 地铁黑名单
    BLACK_PARAM m_prm_mtr_black;

    // 交通部黑名单
    BLACK_PARAM m_prm_jtb_black;

	// 交通部白名单
    WHITE_PARAM m_prm_jtb_white;

    // 行政罚金参数
    vector<POLICY_PENALTY> m_prm_policy_penaty;

	bool device_init_flag;

	// 交通部白名单
    //WHITE_PARAM m_prm_jtb_white;

	// ITP票价表
    ITP_FARE_PARAM m_iprm_fare;

protected:

    // 检查参数配置中的参数是否足够
    bool prm_config_unison();

    // 参数段首是否匹配
    bool prm_section_match(char * p_prm_section_header, uint16_t section_type);

    // 设备控制参数
    int load_device_control(const char * p_prm_path, bool only_check = false);
    void unload_device_control();

    // 车站配置表
    int load_station_config(const char * p_prm_path, bool only_check = false);
    void unload_station_config();

    // SAM卡对照表
    int load_sam_compare(const char * p_prm_path, bool only_check = false);
    void unload_sam_compare();

    // 换乘车站表
    int load_change_station(const char * p_prm_path, bool only_check = false);
    void unload_change_station();

    // 票卡参数
    int load_ticket_prm(const char * p_prm_path, bool only_check = false);
    void unload_ticket_prm();

    // 票价参数
    int load_fare_prm(const char * p_prm_path, bool only_check = false);
    void unload_fare_prm();

    // 降级模式参数
    int load_degrade_mode(const char * p_prm_path, bool only_check = false);
    void unload_degrade_mode();

    // 乘此票专用参数
    int load_tct_config(const char * p_prm_path, bool only_check = false);
    void unload_tct_config();

    // 载入专用通道参数
    int load_special_passageway(const char * p_prm_path, bool only_check = false);
    void unload_special_passageway();

    // 地铁黑名单参数
    int load_mtr_single_black(const char * p_prm_path, bool only_check = false);
    void unload_mtr_single_black();
    int load_mtr_section_black(const char * p_prm_path, bool only_check = false);
    void unload_mtr_section_black();

	//交通部黑名单、白名单参数
	int load_jtb_single_black(const char * p_prm_path, bool only_check = false);
    void unload_jtb_single_black();
    int load_jtb_white(const char * p_prm_path, bool only_check = false);
    void unload_jtb_white();

    // 载入OCT黑名单单号参数
    int load_oct_single_black(const char * p_prm_path, bool only_check = false);
    void unload_oct_single_black();
    int load_oct_section_black(const char * p_prm_path, bool only_check = false);
    void unload_oct_section_black();


    // 载入行政罚金参数
    int load_policy_penalty(const char * p_prm_path, bool only_check = false);
    void unload_policy_penalty();

	// 载入读写器应用参数
	int load_reader_app(const char * p_prm_path, bool only_check = false);

    // 二分法查找一维字符数组
    char * method_of_bisection(char * p_target_str, int len_to_cmp, char * p_array_1d, int foruint16_t_step, int record_count);

	// 二分法查找二维字符数组
	bool method_of_bisection(char * p_target_str, int len_to_cmp, char ** p_array_2d, int size_1d);

	// 二分法查找二维字符数组
	bool method_of_bisection(char * p_target_str, int len_to_cmp, int len_record, char ** p_array_2d, int record_count, char ** pp_find);

    // 票价查找相关
    uint16_t query_fare_zone(uint16_t entry_station_id, uint8_t& fare_zone);
	uint16_t query_min_fare_zone(uint16_t entry_station_id, uint8_t& fare_zone);
	uint16_t query_max_fare_zone(uint16_t entry_station_id,uint8_t& fare_zone);
	uint8_t query_holiday_type(uint8_t * p_time_now, uint16_t ticket_type, uint8_t * p_time_entry);
	bool student_ticket(uint16_t ticket_type);
	bool time_in_idle(uint8_t weekday_today, uint8_t * p_time_src);
    uint8_t query_time_code(uint8_t * p_time_now, uint16_t ticket_type, uint8_t * p_time_entry);
    uint16_t query_fare_config(uint8_t * p_time_now, uint16_t ticket_type, uint8_t time_code, uint16_t& fare_table_id);
    uint16_t query_fare(uint8_t * p_time_now, uint8_t fare_zone, uint16_t fare_table_id, long * p_fare);
	// 查找进站站点到本站的出站超时时间
	uint16_t query_overtime_min(uint16_t entry_station, uint16_t& timeout_in_minutes);


	// 网络拓扑参数
	int load_networktop(const char * p_prm_path, bool only_check = false);
	void unload_networktop();

	// 日历参数
	int load_calendars(const char * p_prm_path, bool only_check = false);
	void unload_calendars();

	// 车票参数
	int load_eticket(const char * p_prm_path, bool only_check = false);
	void unload_eticket();

	// 费率参数
	int load_rate(const char * p_prm_path, bool only_check = false);
	void unload_rate();

	// 载入互联网读写器应用参数
	int load_itp_reader_app(const char * p_prm_path, bool only_check = false);

	// 地铁黑名单参数
	int load_itp_mtr_single_black(const char * p_prm_path, bool only_check = false);
	void unload_itp_mtr_single_black();

	// 外部票种映射参数
	int load_ext_ticket(const char * p_prm_path, bool only_check = false);
	void unload_ext_ticket();

	// ITP参数文件头是否正确
    bool iprm_head_match(char * p_prm_section_header, uint16_t section_type);

};

extern ParamMgr g_Parameter;
