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

    // ���������Ƿ��д������
    bool prm_for_reader(uint16_t prm_type);

	// �����Ƿ��������ļ���
	bool name_in_config(char * p_file_name);

    // ����������õ��ļ�
    bool save_prm_config(uint16_t prm_type, const string& strPath);

    // ��ѯ�����汾
    bool query_prm_version(uint16_t prm_type, char * p_prm_name);

    // �������в���
    int load_all_prms();

    // ���ص�������
    uint16_t load_prm(const uint16_t prm_type, const char * p_prm_path, bool cmd_reset = false, bool only_check = false);

    // ��ѯ�豸���Ʋ���
    void query_device_ctrl(DEVICE_CTRL& ctrl);

    // ��ѯվ��
    uint16_t valid_station(uint16_t station_id);

    // ��ѯ��Ӧ���豸�Ƿ�ע��
    int device_registed(uint16_t station_id, uint8_t device_type, uint16_t device_id);

    // ��ѯSAM�����ձ�
    bool device_from_sam(const char * p_sam_id, const uint8_t sam_type, uint8_t * p_bcd_station, uint8_t * p_device_type, uint8_t * p_bcd_device_id);

    // ��ѯSAM�����ձ�
	bool sam_from_device(const uint8_t * p_bcd_station, const ETPDVC device_type, const uint8_t * p_bcd_device_id, const uint8_t sam_type, char * p_sam_id);
	bool sam_from_device(const uint16_t station_id, const ETPDVC device_type, const uint16_t device_id, const uint8_t sam_type, char * p_sam_id);
	// ��ѯSAM�����Ƿ��Ӧ��ǰ�豸
	bool sam_counterpart(uint8_t sam_sock, char * p_sam_id, uint8_t * p_station, uint8_t dev_type, uint8_t * p_dev_id, bool check_sam_match);

    // ��ѯƱ������
    int query_ticket_prm(uint8_t * p_ticket_type, TICKET_PARAM& prm);

    // ��ѯ�˴�Ʊ����
    int query_tct_prm(uint8_t * p_ticket_type, TCT_PARAM& prm);

    // �豸�Ƿ�֧�ֵ�ǰƱ��
    static uint16_t device_support_ticket(ETPDVC target_device_type, char * p_ticket_usable_device);

	// �豸�Ƿ�֧�ֵ�ǰƱ����ֵ
	static uint16_t device_support_charge(ETPDVC target_device_type, char * p_ticket_charge_device);

	// �豸�Ƿ�֧�ֵ�ǰƱ������
	static uint16_t device_support_sale(ETPDVC target_device_type, char * p_ticket_sell_device);

	// �Ƿ�ɷ��ۣ������豸ȫ��Ϊ0ʱ��������
	static bool permit_sale(char * p_ticket_sell_device);

    // ���ҵ���������
    bool query_black_mtr(char * p_mtr_card_logical_id);

    // ����һ��ͨ������
    bool query_black_oct(char * p_oct_card_logical_id);

	// ���ҽ�ͨ��������
    bool query_black_jtb(char * p_jtb_card_logical_id);

	// ���ҽ�ͨ��������
    bool query_white_jtb(char * p_jtb_card_issue_code);

	// �ж��Ƿ��ǻ�����ͨƱ��
	bool is_interconnect(uint8_t * interCardFlag, uint8_t * citycode);

    // �Ƿ��վ��ʱ
    bool exit_timeout(uint16_t entry_station, uint8_t *  p_time_entry, uint8_t *  p_time_exit, long * p_penalty = NULL);

	// ��ȡ��Ч��վʱ��
    bool get_validexittime(uint16_t entry_station, uint8_t *  p_time_entry, uint8_t *  p_time_exit, long * p_penalty = NULL);
	

    // ��ѯƱ��,lTradeΪ���׵�ʵ�ʽ�lFareΪԭƱ�ۣ�����ƱƱ�ۣ�
    uint16_t query_fare(uint8_t * p_time_now, uint8_t * p_ticket_type,
                   uint16_t entry_station, uint8_t * p_time_entry, long * p_fare_actual, long * p_fare_init = NULL/**/);

    // �����Ʊ��
    int query_lowest_fare(uint8_t * p_time_now, uint8_t * p_ticket_type, long& fare_lowest_actual, long * p_fare_lowest_init = NULL);

	// ��Ÿ����Ʊ��
    int query_cf_lowest_fare(uint8_t * p_time_now, uint8_t * p_ticket_type, uint16_t entry_station,long& fare_lowest_actual, long * p_fare_lowest_init = NULL);

	//��ѯ��·���Ʊ��
	int query_line_lowest_fare(uint8_t * p_time_now, uint16_t entry_station,uint8_t * p_ticket_type, long& fare_lowest_actual, long * p_fare_lowest_ini);

	//��ѯ����վ���Ʊ��
	int query_highest_fare(uint8_t * p_time_now, /*uint8_t * p_ticket_type, long& fare_lowest_actual, */long * p_fare_lowest_init = NULL);

	// ����Ʊ�۲����շ�����
	uint16_t fare_zone_by_sjt_fare(uint8_t * p_ticket_type, long fare, uint8_t& fare_zone);

    // �Ƿ���ͬ��վ
    bool same_station(uint16_t station_id0, uint16_t station_id1);

    // �������Ϸ���
    uint16_t valid_prm(const char * p_prm_pathname);

    // ���Ľ���ģʽ�����ڽ��ս�����Ϣ
	void modify_degrade(uint8_t type, uint8_t * p_time, uint8_t * p_station, uint8_t flag);

    // ��ѯ��ǰ����ģʽ
    void query_degrade_mode(MODE_EFFECT_FLAGS& mdEffect);

	// ����ģʽ��ʷ��¼�Ƿ�Ե�ǰƱ����Ӱ��
	// prepared_period���Ƿ���Ԥ��Ч�ڣ� p_time_now������ʱ�䣬p_time_last���ϴζ�Ӧ�Ĳ���ʱ�䣬station_last���ϴζ�Ӧ�Ĳ���վ��
	bool tk_in_mode(DGTYPE mode, bool prepared_period, uint8_t * p_time_now, uint8_t * p_time_last, uint16_t station_last);

    // ES�ĵ���ƱƱ���Ƿ���ָ��ģʽ�����ڵ�Ӱ��(ES����Ч����ģʽ��ʼ��ʼ֮��)
    bool EsEffectByAppointedMode(DGTYPE dgAppointed, uint8_t * pCurTime, uint8_t * pLExpireE, uint16_t UseStation = 0);

    // ��ѯբ��ͨ������
    uint16_t passageway_allow_pass(uint8_t * p_ticket_type, uint8_t cur_door_type);

    // ���������������
    int query_policy_penalty(uint8_t * p_ticket_type, int policy_code, short& penalty);

    uint16_t config_param(char * p_name_prm);

    uint16_t get_prm_info(uint16_t prm_type, char * p_prm_name);

	// ɾ�������ļ�֮��Ĳ���
	void delete_prm_outof_config();

	// �豸�Ƿ���ɳ�ʼ���������ز�������д���쳣�������豸�����޷�֪��������û�е����豸��ʼ����
	void device_initialize();
	uint16_t device_initialized();

	// �����뱾վ����ķ��������������ĳ�վ
	uint16_t nearest_uncheckorder_station();


	uint16_t config_param_itp(char * p_name_prm);

	// �������Ϸ���
	uint16_t valid_prm_itp(const char * p_prm_pathname);

	// ���ص�������
	uint16_t load_prm_itp(const uint16_t prm_type, const char * p_prm_path, bool cmd_reset = false, bool only_check = false);

	uint16_t get_prm_info_itp(uint16_t iprm_type, char * p_iprm_name);

	// ���Ĳ�������
	bool save_prm_config_itp(uint16_t prm_type, const string& strPath);

	bool load_itp_dat_config();

	// ITP���������Ƿ��д������
    bool iprm_for_reader(uint16_t prm_type);

private:
    static PRM_FUNC_LIST m_prm_for_reader[];
	static IPRM_FUNC_LIST m_iprm_for_reader[];

    // �������ͺ��ļ�·����Ϣ
    map<uint16_t, string>	m_prm_config;

	// �������ͺ��ļ�·����Ϣ
	map<uint16_t, string>	m_iprm_config;

    // ��վ����
    vector<STATION_CONFIG> m_prm_station_config;

    // �豸���Ʋ���
    DEVICE_CTRL m_prm_device_ctrl;

    // sam�����ձ�
    SAM_COMPARE_TABLE m_prm_sam_compare;

    // ���˳�վ��
    vector< std::pair<uint16_t, uint16_t> > m_prm_changing_station;

    // Ʊ������
    vector<TICKET_PARAM> m_prm_ticket;

    // Ʊ�۱�
    FARE_PARAM m_prm_fare;

    // ����ģʽ
    vector<DEGRADE_MODE> m_prm_degrade;

    // �˴�Ʊר�ò���
    vector<TCT_PARAM> m_prm_tct;

    // ר��ͨ������
    map<uint16_t, bool> m_prm_passageway;

    // һ��ͨ������
    BLACK_PARAM m_prm_oct_black;

    // ����������
    BLACK_PARAM m_prm_mtr_black;

    // ��ͨ��������
    BLACK_PARAM m_prm_jtb_black;

	// ��ͨ��������
    WHITE_PARAM m_prm_jtb_white;

    // �����������
    vector<POLICY_PENALTY> m_prm_policy_penaty;

	bool device_init_flag;

	// ��ͨ��������
    //WHITE_PARAM m_prm_jtb_white;

	// ITPƱ�۱�
    ITP_FARE_PARAM m_iprm_fare;

protected:

    // �����������еĲ����Ƿ��㹻
    bool prm_config_unison();

    // ���������Ƿ�ƥ��
    bool prm_section_match(char * p_prm_section_header, uint16_t section_type);

    // �豸���Ʋ���
    int load_device_control(const char * p_prm_path, bool only_check = false);
    void unload_device_control();

    // ��վ���ñ�
    int load_station_config(const char * p_prm_path, bool only_check = false);
    void unload_station_config();

    // SAM�����ձ�
    int load_sam_compare(const char * p_prm_path, bool only_check = false);
    void unload_sam_compare();

    // ���˳�վ��
    int load_change_station(const char * p_prm_path, bool only_check = false);
    void unload_change_station();

    // Ʊ������
    int load_ticket_prm(const char * p_prm_path, bool only_check = false);
    void unload_ticket_prm();

    // Ʊ�۲���
    int load_fare_prm(const char * p_prm_path, bool only_check = false);
    void unload_fare_prm();

    // ����ģʽ����
    int load_degrade_mode(const char * p_prm_path, bool only_check = false);
    void unload_degrade_mode();

    // �˴�Ʊר�ò���
    int load_tct_config(const char * p_prm_path, bool only_check = false);
    void unload_tct_config();

    // ����ר��ͨ������
    int load_special_passageway(const char * p_prm_path, bool only_check = false);
    void unload_special_passageway();

    // ��������������
    int load_mtr_single_black(const char * p_prm_path, bool only_check = false);
    void unload_mtr_single_black();
    int load_mtr_section_black(const char * p_prm_path, bool only_check = false);
    void unload_mtr_section_black();

	//��ͨ��������������������
	int load_jtb_single_black(const char * p_prm_path, bool only_check = false);
    void unload_jtb_single_black();
    int load_jtb_white(const char * p_prm_path, bool only_check = false);
    void unload_jtb_white();

    // ����OCT���������Ų���
    int load_oct_single_black(const char * p_prm_path, bool only_check = false);
    void unload_oct_single_black();
    int load_oct_section_black(const char * p_prm_path, bool only_check = false);
    void unload_oct_section_black();


    // ���������������
    int load_policy_penalty(const char * p_prm_path, bool only_check = false);
    void unload_policy_penalty();

	// �����д��Ӧ�ò���
	int load_reader_app(const char * p_prm_path, bool only_check = false);

    // ���ַ�����һά�ַ�����
    char * method_of_bisection(char * p_target_str, int len_to_cmp, char * p_array_1d, int foruint16_t_step, int record_count);

	// ���ַ����Ҷ�ά�ַ�����
	bool method_of_bisection(char * p_target_str, int len_to_cmp, char ** p_array_2d, int size_1d);

	// ���ַ����Ҷ�ά�ַ�����
	bool method_of_bisection(char * p_target_str, int len_to_cmp, int len_record, char ** p_array_2d, int record_count, char ** pp_find);

    // Ʊ�۲������
    uint16_t query_fare_zone(uint16_t entry_station_id, uint8_t& fare_zone);
	uint16_t query_min_fare_zone(uint16_t entry_station_id, uint8_t& fare_zone);
	uint16_t query_max_fare_zone(uint16_t entry_station_id,uint8_t& fare_zone);
	uint8_t query_holiday_type(uint8_t * p_time_now, uint16_t ticket_type, uint8_t * p_time_entry);
	bool student_ticket(uint16_t ticket_type);
	bool time_in_idle(uint8_t weekday_today, uint8_t * p_time_src);
    uint8_t query_time_code(uint8_t * p_time_now, uint16_t ticket_type, uint8_t * p_time_entry);
    uint16_t query_fare_config(uint8_t * p_time_now, uint16_t ticket_type, uint8_t time_code, uint16_t& fare_table_id);
    uint16_t query_fare(uint8_t * p_time_now, uint8_t fare_zone, uint16_t fare_table_id, long * p_fare);
	// ���ҽ�վվ�㵽��վ�ĳ�վ��ʱʱ��
	uint16_t query_overtime_min(uint16_t entry_station, uint16_t& timeout_in_minutes);


	// �������˲���
	int load_networktop(const char * p_prm_path, bool only_check = false);
	void unload_networktop();

	// ��������
	int load_calendars(const char * p_prm_path, bool only_check = false);
	void unload_calendars();

	// ��Ʊ����
	int load_eticket(const char * p_prm_path, bool only_check = false);
	void unload_eticket();

	// ���ʲ���
	int load_rate(const char * p_prm_path, bool only_check = false);
	void unload_rate();

	// ���뻥������д��Ӧ�ò���
	int load_itp_reader_app(const char * p_prm_path, bool only_check = false);

	// ��������������
	int load_itp_mtr_single_black(const char * p_prm_path, bool only_check = false);
	void unload_itp_mtr_single_black();

	// �ⲿƱ��ӳ�����
	int load_ext_ticket(const char * p_prm_path, bool only_check = false);
	void unload_ext_ticket();

	// ITP�����ļ�ͷ�Ƿ���ȷ
    bool iprm_head_match(char * p_prm_section_header, uint16_t section_type);

};

extern ParamMgr g_Parameter;
