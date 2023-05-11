// ParamStructs.h
#pragma once

#include <map>
#include <vector>
#include <stdint.h>
#include <string>
using namespace std;

// �豸���Ʋ���
typedef struct
{
    long		sc_auto_destroy;				// SC����Ա�޲����Զ�ע��ʱ�� [��λ: ��]
    long		sc_reconnect_lcc;				// SC��������LCC������ [��λ: ��]
    long		bom_auto_destroy;				// BOM��Ӫ��Ա�Զ�ע��ʱ�� [��λ: ��]
    long		entry_exit_timeout;				// ��/��բTIMEOUT [��λ: ��]
    long		degrade_effect_time;			// ����ģʽ��Ӱ����Ч�� [��λ����]
    long		svt_charge_max;					// ��ֵ����ֵǮ����ֵ���ޡ�Unit : ��
    long		loyate_charge_max;				// ����Ǯ����ֵ���ޡ�Unit : ��
    long		sjt_sale_max;					// ����Ʊ����Ǯ����ֵ���ޡ�Unit : ��
    long		svt_charge_min;					// ��ֵ����ֵǮ����ֵ���ޡ�Unit : ��
    long		loyate_charge_min;				// ����Ǯ����ֵ���ޡ�Unit : ��
    long		sjt_sale_min;					// ����Ʊ����Ǯ����ֵ���ޡ�Unit : ��

} DEVICE_CTRL, * P_DEVICE_CTRL;

// ��վ���ñ�
typedef struct
{
    uint16_t	station_id;						// ��վ��
    uint16_t	device_id;
    uint8_t		device_type;

} STATION_CONFIG, * P_STATION_CONFIG;

// SAM�����ձ�
typedef struct
{
    int			record_count;					// ��¼����
    char **		sam_device_info;				// SAM�����豸��Ϣ

} SAM_COMPARE_TABLE, * P_SAM_COMPARE_TABLE;

// Ʊ�������ṹ
typedef struct
{
    uint16_t	ticket_type;					// Ʊ������
    uint8_t		wallet_type;					// Ǯ������
    long		balance_max;					// ��Ʊ���/������
    bool		permit_over_draft;				// �Ƿ�����͸֧
    long		value_over_draft;				// ͸֧���
    bool		permit_charge;					// �Ƿ������ֵ
    long		charge_max;						// ÿ�γ�ֵ����
    uint8_t		update_fare_mode;				// ����ʱ�շѷ�ʽ
    bool		check_update_station;			// �Ƿ��鸶�����Ǳ�վ����
    bool		check_update_day;				// �Ƿ��鸶�����Ǳ��ո���
    bool		pemite_refund;					// �Ƿ������˿�
    long		refund_min_count;				// �˿�ʱʹ�ô�������
    long		daliy_trade_max;				// �ճ�����������
    long		month_trade_max;				// �³�����������
    long		effect_time_from_sale;			// ��Чʱ�䣨�ӷ��ۿ�ʼ��ʱ�䣬��λ�����ӣ�
    bool		permit_deffer;					// �Ƿ���������
    long		deffer_days;					// ���ӳ�����
    long		deposite;						// Ѻ��
    long		prime_cost;						// �ۼۣ����ɱ���
    long		sale_brokerage;					// ����������
    bool		need_active_for_use;			// ʹ��ǰ�Ƿ��輤��
    bool		check_blacklist;				// �Ƿ��������
    bool		check_balance;					// �Ƿ������/���

	bool		check_phy_expire;				// �Ƿ���������Ч��
	bool		check_logic_expire;				// �Ƿ����߼���Ч��
    //uint8_t		check_order;				// ����������
    // (0������վ�������,1������վ�����,2����վ�����վ���,3����վ����վ�����)
    bool		check_entry_order;
    bool		check_exit_order;
    bool		check_exit_timeout;				// �Ƿ��鳬ʱ
    bool		check_over_travel;				// �Ƿ��鳬��
    bool		limit_station;					// �Ƿ����ƽ���վ��
    bool		limit_the_Station;				// �Ƿ�ֻ����վ����
    char		chargable_device[16];			// ��ֵ�豸
    char		usable_device[16];				// �����豸��ʹ��
    bool		entry_sell_station;				// ��վ�������Ʊ��
    long		refund_max_value;				// ��ʱ�˿�������ޣ�����������������
    long		refund_brokerage;				// �˿�������
	char		sell_device[16];				// �����豸
	bool		permit_refund_deposite;			// �Ƿ������˳ɱ�Ѻ��

	bool		has_query;						// ��ʾ��ǰƱ����Ӧ��Ʊ�������Ƿ��Ѿ�����ѯ�����Ч��

} TICKET_PARAM, * P_TICKET_PARAM;

// �˴�Ʊר�ò���
typedef struct
{
    uint16_t ticket_type;						// Ʊ������
    long	once_charge_value;					// ÿ�γ�ֵ���
    long	once_charge_count;					// ÿ�γ�ֵ�˴�
    uint8_t	charge_start_next_month;			// ���³�ֵ��ʼ��
    uint8_t	charge_end_next_month;				// ���³�ֵ������
    long	exit_timeout_penalty_count;			// ��վ��ʱ�˴η���
    long	exit_timeout_penalty_value;			// ��վ��ʱ��ͨ����
    long	entry_timeout_penalty_count;		// ��վ��ʱ�˴η���
    long	entry_timeout_penalty_value;		// ��վ��ʱ��ͨ����

} TCT_PARAM, * P_TCT_PARAM;

////////////////////////////////////////////////////////////////
// �շ����νṹ���Խ�վվ����Ϊ��ֵ
typedef struct
{
    uint16_t	station_id;
	uint16_t	exit_station_id;
    uint8_t		fare_zone;						// �շ�����
    uint16_t	ride_time_limit;				// �˳�ʱ������
    uint16_t	penalty_for_timeout;			// ��ʱ����

} FARE_ZONE, * P_FARE_ZONE;


////////////////////////////////////////////////////////////////
// ÿ��վ����Ҫ����Ÿ���վ������
//typedef struct
//{
//    uint16_t	cf_station_id;
//	uint16_t	cf_exit_station_id;
//    uint8_t		fare_zone;						// �շ�����
//    uint16_t	ride_time_limit;				// �˳�ʱ������
//    uint16_t	penalty_for_timeout;			// ��ʱ����
//
//} CF_FARE_ZONE, * P_CF_FARE_ZONE;

// �շ����ýṹ���Ա��ṹ��Ϊ��ֵ
typedef struct
{
    uint16_t	ticket_type;					// Ʊ�۱�ID
    uint8_t		ride_time_code;					// �˳�ʱ�����
    uint16_t	fare_table_id;					// Ʊ�۱�ID
} FARE_CONFIG_TABLE, * P_FARE_CONFIG_TABLE;

// Ʊ�۽ṹ
typedef struct
{
    uint16_t	fare_table_id;					// Ʊ�۱�ID
    uint8_t		fare_zone;						// �շ�����
    long		fare;

} FARE_TABLE, * P_FARE_TABLE;

// �ڼ��սṹ
typedef struct
{
    uint8_t		holiday_code;					// �ڼ��մ���
    uint8_t		start_date[4];					// ��ʼʱ��
    uint8_t		end_date[4];					// ����ʱ��

} HOLIDAY_TABLE, * P_HOLIDAY_TABLE;

// �Ƿ�æʱ��ṹ
typedef struct
{
    uint8_t		weekday;						// ����
    uint8_t		start_time[2];					// ��ʼʱ��
    uint8_t		end_time[2];					// ����ʱ��

} IDLE_TIME_TABLE, * P_IDLE_TIME_TABLE;


///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// ITP����   �շ����νṹ���Խ�վվ����Ϊ��ֵ
typedef struct
{
    uint16_t	station_id;
	uint16_t	exit_station_id;
    uint8_t		fare_zone;						// �շ�����
    uint16_t	ride_time_limit;				// �˳�ʱ������
    uint16_t	penalty_for_timeout;			// ��ʱ����

} ITP_FARE_ZONE, * P_ITP_FARE_ZONE;

// �շ����ýṹ���Ա��ṹ��Ϊ��ֵ
typedef struct
{
    uint16_t	ticket_type;					// Ʊ�۱�ID
    uint8_t		ride_time_code;					// �˳�ʱ�����
    uint16_t	fare_table_id;					// Ʊ�۱�ID
} ITP_FARE_CONFIG_TABLE, * P_ITP_FARE_CONFIG_TABLE;

// Ʊ�۽ṹ
typedef struct
{
    uint16_t	fare_table_id;					// Ʊ�۱�ID
    uint8_t		fare_zone;						// �շ�����
    long		fare;

} ITP_FARE_TABLE, * P_ITP_FARE_TABLE;

// �ڼ��սṹ
typedef struct
{
    uint8_t		holiday_code;					// �ڼ��մ���
    uint8_t		start_date[4];					// ��ʼʱ��
    uint8_t		end_date[4];					// ����ʱ��

} ITP_HOLIDAY_TABLE, * P_ITP_HOLIDAY_TABLE;

// �Ƿ�æʱ��ṹ
typedef struct
{
    uint8_t		weekday;						// ����
    uint8_t		start_time[2];					// ��ʼʱ��
    uint8_t		end_time[2];					// ����ʱ��

} ITP_IDLE_TIME_TABLE, * P_ITP_IDLE_TIME_TABLE;

///////////////////////////////////////////////////////////////
// ����������
// ����������
typedef struct
{
    int		record_count;						// ��¼����
    char ** logical_id;							// �߼�����

} SINGLE_BLACK, * P_SINGLE_BLACK;

typedef struct
{
    int		record_count;						// ��¼����
    char ** issue_code;							// ������������
	char ** issue_ident_no;					    // ��������ʶ����IIN

} SINGLE_WHITE, * P_SINGLE_WHITE;

// ��������������Դ˽ṹΪ��ֵ
typedef struct
{
    uint8_t		policy_code;					// ��������
    uint16_t	ticket_type;					// Ʊ������
	uint16_t	line_station;					// ��վ����
    uint16_t	penalty;						// ����
} POLICY_PENALTY, * P_POLICY_PENALTY;

// ����ģʽ ��ITP��SLEϵͳ�ѡ��������Ľӿڹ淶��
typedef enum DEGRADE_TYPE
{
    degrade_null				= 0,
    degrade_Trouble				= 1, //�г�����ģʽ
    degrade_Uncheck_Order		= 2, //��վ���ģʽ
    degrade_Uncheck_DateTime	= 3, //�������ģʽ
    degrade_Uncheck_Fare		= 5, //�������ģʽ
    degrade_Green_Light			= 6  //��������ģʽ

} DGTYPE;

typedef struct
{
    uint16_t	degrade_station;				// ����������վ��
    DGTYPE		degrade_type;					// ����ģʽ������
    uint8_t		degrade_start_time[7];			// ����ģʽ�Ŀ�ʼʱ��
    uint8_t		degrade_end_time[7];			// ����ģʽ����ʱ��
    uint8_t		degrade_susceptive[7];			// ����ģʽӰ���ڽ���ʱ�䣬��ʹ��ʱ����

} DEGRADE_MODE, * P_DEGRADE_MODE;

// ��ǰƱ�����ܵĽ���ģʽӰ��
typedef struct
{
    bool		emergency;						// ����ģʽ
    bool		train_trouble;					// �г�����
    bool		uncheck_order;					// �����������
    bool		uncheck_datetime;				// ��Ч��ʱ�����
    bool		uncheck_fare;					// �������

    bool		entry_by_emergency;				// �ѽ�վ�ĳ�Ʊ�Ƿ��ܽ���ģʽ������Ӱ��
    bool		es_sjt_by_emergency;			// ES�ĵ���Ʊ�Ƿ��ܽ���ģʽ������Ӱ��
    bool		sell_sjt_by_emergency;			// ���۵ĵ���Ʊ���ܽ���ģʽ������Ӱ��
    bool		exit_train_trouble_effect;		// �г�����ģʽ��վ�ĵ���Ʊ�Ƿ����г����ϵ�������Ӱ��
    bool		es_sjt_by_train_trouble;		// ES�ĵ���Ʊ�Ƿ����г�����ģʽ������Ӱ��
    bool		sell_sjt_by_train_trouble;		// ���۵ĵ���Ʊ�Ƿ����г�����ģʽ������Ӱ��
	bool		update_free_by_emergency;		// �Ǹ��������µĵ���Ʊ���ܽ���ģʽ������Ӱ��
	bool		update_work_by_emergency;		// ���������³�Ʊ���ܽ���ģʽ������Ӱ��
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
	SINGLE_BLACK					blacks_single;					// ����������
	vector< std::pair<string, string> >	blacks_section;				// �������κ�

} BLACK_PARAM, * P_BLACK_PARAM;

typedef struct
{
	SINGLE_WHITE					whites_single;					// ����������
	vector< std::pair<string, string> >	whites_section;				// �������κ�
} WHITE_PARAM, * P_WHITE_PARAM;
