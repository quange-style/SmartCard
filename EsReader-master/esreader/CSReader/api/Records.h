#pragma once

#define _REENTRANT						// 线程可充入宏，必须在所有头文件引用前

#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <vector>
using namespace std;

// 交易信息结构
typedef struct _TRADE_STRUCT{
	uint8_t		trade_type;				// 交易类型
	uint8_t		trade_time[4];			// 交易日期
	size_t		size;					// 交易结构的大小
	uint8_t		trade_data[255];		// 交易数据
}TDSTRUCT, * PTDSTRUCT;

typedef struct _TRADE_SUFFIX{
	uint8_t		trade_type;				// 交易类型
	char		file_suffix[4];			// 交易文件后缀名
}TRADESUFFIX, * PTRADESUFFIX;

enum LOG_LEVEL
{
	level_invalid = 1,		// 无效
	level_normal,			// 普通，一般正常的业务
	level_warning,			// 警告，业务判断非正常
	level_error,			// 错误，底层错误，包括票卡读写，闪卡等等
	level_disaster			// 灾难，硬件无法正常工作，内存分配失败等会造成读写器应用无法正常工作
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


	// 写日志，err_code:当前调用错误码，lvl:当前指定级别，只影响本次，只要通过err_code查找到的级别或者lvl_appoint级别高于系统日志级别，就写日志
	void log_out(uint16_t err_code, LOG_LEVEL lvl_appoint, const char * fmt, ...);
	void log_buffer(char * p_discription, uint8_t * p_buffer, int size_buffer);

	void error_level_sort();

	uint16_t set_record_param(uint16_t trade_save_days, uint16_t log_save_days, int lvl);

	void get_record_param(uint16_t& trade_save_days, uint16_t& log_save_days, int& lvl);

	bool record_trade_start();

	void record_trade_end();

	void add_record(char * p_trade_type, void * p_trade_data, size_t size_trade_data);

	void delete_file_overdue();

	// 判断一个文件夹是否为空
	static bool folder_null(char * p_folder);

	void run_time_count_start();

	void run_time_count_end();

	int run_time();

private:

	// 程序计时结构
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
#define CAL_RUN_TIME	g_Record.run_time()
