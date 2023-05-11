#pragma once

#define _REENTRANT						// �߳̿ɳ���꣬����������ͷ�ļ�����ǰ

#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <vector>
using namespace std;

// ������Ϣ�ṹ
typedef struct _TRADE_STRUCT{
	uint8_t		trade_type;				// ��������
	uint8_t		trade_time[4];			// ��������
	size_t		size;					// ���׽ṹ�Ĵ�С
	uint8_t		trade_data[512];		// ��������
}TDSTRUCT, * PTDSTRUCT;

typedef struct _TRADE_SUFFIX{
	uint8_t		trade_type;				// ��������
	char		file_suffix[4];			// �����ļ���׺��
}TRADESUFFIX, * PTRADESUFFIX;

enum LOG_LEVEL
{
	level_invalid = 1,		// ��Ч
	level_normal,			// ��ͨ��һ��������ҵ��
	level_warning,			// ���棬ҵ���жϷ�����
	level_error,			// ���󣬵ײ���󣬰���Ʊ����д�������ȵ�
	level_disaster			// ���ѣ�Ӳ���޷������������ڴ����ʧ�ܵȻ���ɶ�д��Ӧ���޷���������
};

typedef struct {
	uint16_t	err_code;
	LOG_LEVEL	err_level;
}ERROR_LEVEL_TABLE;

class Records
{
public:
	Records(void);
	~Records(void);


	// д��־��err_code:��ǰ���ô����룬lvl:��ǰָ������ֻӰ�챾�Σ�ֻҪͨ��err_code���ҵ��ļ������lvl_appoint�������ϵͳ��־���𣬾�д��־
	void log_out(uint16_t err_code, LOG_LEVEL lvl_appoint, const char * fmt, ...);
	void log_buffer(const char * p_discription, uint8_t * p_buffer, int size_buffer, LOG_LEVEL lvl_appoint = level_disaster);

	void log_out_debug(uint16_t err_code, LOG_LEVEL lvl_appoint, const char * fmt, ...);
	void log_buffer_debug(const char * p_discription, uint8_t * p_buffer, int size_buffer);

	void error_level_sort();

	uint16_t set_record_param(uint16_t trade_save_days, uint16_t log_save_days, int lvl);

	void get_record_param(uint16_t& trade_save_days, uint16_t& log_save_days, int& lvl);

	bool record_trade_start();

	void record_trade_end();

	void add_record(char * p_trade_type, void * p_trade_data, size_t size_trade_data);

	void delete_file_overdue();

	// �ж�һ���ļ����Ƿ�Ϊ��
	static bool folder_null(char * p_folder);

	void run_time_count_start();

	void run_time_count_end();

	//��ȡ��־�ļ��д�С
	static unsigned long GetLogDirSize(char * path);

private:

	// �����ʱ�ṹ
	struct timeval m_tp_start;
	struct timeval m_tp_end;

	static uint16_t err_last;
	static uint8_t err_counter;

	static LOG_LEVEL current_log_level;

	static uint16_t current_trade_save_days;
	
	static uint16_t current_log_save_days;

	static ERROR_LEVEL_TABLE error_level_cmp[];

	vector<TDSTRUCT> trade_array;

	pthread_t thread_handle;

	sem_t semaphore_handle;

	void get_file_suffix(uint8_t trade_type, char * p_suffix);

	void get_file_name(TDSTRUCT& tdStruct, char * p_file_name);

	uint16_t record_trade(TDSTRUCT& tdStruct, char * pDir);

	static void * record_proc(void * arg);

	void ergodic_dir_delete_target(char * p_path_dir, uint8_t * p_today, int save_time, int date_offset_filename);

	bool reach_log_level(uint16_t err_code, LOG_LEVEL lvl_appoint);
};

extern Records g_Record;

#define COUTER_START	g_Record.run_time_count_start()
#define COUTER_ENDER	g_Record.run_time_count_end()
