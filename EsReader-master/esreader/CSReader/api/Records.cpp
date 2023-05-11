// Records.cpp

#include "Records.h"
#include "Api.h"
#include "QFile.h"
#include "Publics.h"
#include "CmdSort.h"
#include "Errors.h"
#include "TimesEx.h"
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdarg.h>

Records g_Record;

uint16_t Records::err_last					= 0;
uint8_t Records::err_counter				= 0;
LOG_LEVEL Records::current_log_level		= level_warning;
uint16_t Records::current_trade_save_days	= 30;
uint16_t Records::current_log_save_days		= 30;

ERROR_LEVEL_TABLE Records::error_level_cmp[] =
{
	{ERR_SUCCEED,			level_normal},
	{ERR_UNDEFINED,			level_error},				// δ�������
	{ERR_CARD_NONE,			level_invalid},				// �޿�
	{ERR_CARD_MORE,			level_invalid},				// �࿨
	{ERR_CARD_READ,			level_error},				// ����ʧ��
	{ERR_CARD_WRITE,		level_error},				// д��ʧ��
	{ERR_WITHOUT_SELL,		level_warning},				// δ���ۣ�ESԤ��ֵ���ĳ�Ʊ
	{ERR_CARD_STATUS,		level_error},				// Ʊ��״̬����
	{ERR_CARD_REFUND,		level_warning},				// Ʊ�����˿ע����
	{ERR_CARD_LOCKED,		level_warning},				// ��������
	{ERR_CARD_INVALID,		level_error},				// ��ЧƱ
	{ERR_OVER_PERIOD_P,		level_warning},				// Ʊ��������Ч�ڹ���
	{ERR_OVER_PERIOD_L,		level_warning},				// Ʊ���߼���Ч�ڹ���
	{ERR_OVER_PERIOD_A,		level_warning},				// Ʊ��������Ч�ڹ���
	{ERR_CARD_TYPE,			level_error},				// Ʊ������δ����
	{ERR_STATION_INVALID,	level_warning},				// �Ƿ��ĳ�վ����
	//{ERR_DEVICE_INVALID,			ERR_STATION_INVALID
	{ERR_CARD_USE,			level_warning},				// �Ǳ�վʹ�õĳ�Ʊ
	{ERR_LACK_WALLET,		level_warning},				// Ʊ������
	{ERR_OVER_WALLET,		level_warning},				// Ʊ����������
	{ERR_ENTRY_STATION,		level_warning},				// �Ǹ������Ǳ�վ��վ
	{ERR_ENTRY_TIMEOUT,		level_warning},				// �Ǹ�������վ��ʱ
	{ERR_ENTRY_EVER,		level_warning},				// �Ǹ������н�վ��
	{ERR_WITHOUT_ENTRY,		level_warning},				// �������޽�վ��
	{ERR_OVER_MILEAGE,		level_warning},				// ����������
	{ERR_EXIT_TIMEOUT,		level_warning},				// ��������վ��ʱ
	{ERR_UPDATE_STATION,	level_warning},				// �Ǳ�վ���µĳ�Ʊ
	{ERR_UPDATE_DAY,		level_warning},				// �Ǳ��ո��µĳ�Ʊ
	{ERR_PASSAGEWAY_1,		level_warning},				// ��ͨƱͨ������֧���Ż�Ʊ
	{ERR_PASSAGEWAY_2,		level_warning},				// �Ż�Ʊͨ������֧����ͨƱ
	{ERR_LAST_EXIT_NEAR,	level_warning},				// ����X�������ڱ�վ��վ
	{ERR_CALL_INVALID,		level_warning},				// �Ƿ��Ľӿڵ���
	{ERR_DEVICE_SUPPROT,	level_warning},				// �豸��֧�ֵ�Ʊ������
	{ERR_DIFFRENT_CARD,		level_warning},				// ��ͬһ��Ʊ��
	{ERR_CALLING_INVALID,	level_warning},				// Ʊ����ǰ���÷Ƿ�
	{ERR_INPUT_PARAM,		level_warning},				// �Ƿ��Ĵ������
	{ERR_DEVICE_UNINIT,		level_error},				// �豸δ��ʼ��
	{ERR_PARAM_NOT_EXIST,	level_disaster},			// �����ļ�������
	{ERR_PARAM_INVALID,		level_disaster},			// �����ļ����Ϸ�
	{ERR_FILE_ACCESS,		level_disaster},			// �ļ�����
	{ERR_INPUT_INVALID,		level_warning},				// ��Ч���������
	{ERR_HANDLER_OPEN,		level_disaster},			// ͨѶ����򿪴���
	{ERR_SEND_DATA,			level_disaster},			// ͨѶ��������ʧ��
	{ERR_RECIVE_DATA,		level_disaster},			// ͨѶ��������ʧ��
	{ERR_DATA_INVALID,		level_disaster},			// ͨѶ���ݷǷ�
	{ERR_SAM_RESPOND,		level_disaster},			// SAM����Ӧ����
	{ERR_OVERLAPPED,		level_error},				// �����ص�����λ�������ڶ�д��һ����������δִ����ϵ�����·����µ�ָ��
	{ERR_RF_NONE,			level_disaster}				// Rfģ�����
};

Records::Records(void)
{

}

Records::~Records(void)
{

}

void Records::log_out(uint16_t err_code, LOG_LEVEL lvl_appoint, const char * fmt, ...)
{
	int cnt								= 0;
	FILE * fp							= NULL;
	char log_path_name[_POSIX_PATH_MAX]	= {0};

	if (reach_log_level(err_code, lvl_appoint))
	{
		// ȷ���ظ���־
		if (err_code == err_last && (err_code == ERR_DEVICE_UNINIT || err_code == ERR_PARAM_NOT_EXIST || 
			err_code == ERR_PARAM_INVALID || err_code == ERR_STATION_INVALID || err_code == ERR_CARD_NONE))
		{
			err_counter ++;
		}
		else
		{
			err_counter = 0;
		}

		err_last = err_code;

		if (err_counter < 2)
		{
			if(access(NAME_LOG_FOLDER, 0) == 0 || mkdir(NAME_LOG_FOLDER, 0755) == 0)
			{
				sprintf(log_path_name, "%s/%02x%02x%02x%02x.log", NAME_LOG_FOLDER,
					CmdSort::m_time_now[0], CmdSort::m_time_now[1], CmdSort::m_time_now[2], CmdSort::m_time_now[3]);

				fp = fopen(log_path_name, "a+");
				if (fp != NULL)
				{
					va_list argptr;

					cnt = fprintf(fp,"[%02x:%02x:%02x] ", CmdSort::m_time_now[4], CmdSort::m_time_now[5], CmdSort::m_time_now[6]);

					va_start(argptr, fmt);
					cnt = vfprintf(fp, fmt, argptr);
					va_end(argptr);

					if (err_counter == 1)
						fprintf(fp, ", The same code returned many times, omit...\r\n");
					else
						fprintf(fp, "\r\n");
					fclose(fp);
				}
			}
		}
	}
}

void Records::log_buffer(char * p_discription, uint8_t * p_buffer, int size_buffer)
{
	char szTemp[8] = {0};
	char szLog[2048] = {0};

	strcat(szLog, p_discription);
	for (int i=0;i<size_buffer;i++)
	{
		sprintf(szTemp, "%02X", p_buffer[i]);
		strcat(szLog, szTemp);
	}
	log_out(0, level_disaster, szLog);
}

void Records::error_level_sort()
{
	ERROR_LEVEL_TABLE temp_pair;

	size_t table_size = sizeof(error_level_cmp)/sizeof(error_level_cmp[0]);
	for (size_t i=0;i<table_size;i++)
	{
		for (size_t j=i+1;j<table_size;j++)
		{
			if (error_level_cmp[i].err_code > error_level_cmp[j].err_code)
			{
				temp_pair = error_level_cmp[i];
				error_level_cmp[i] = error_level_cmp[j];
				error_level_cmp[j] = temp_pair;
			}
		}
	}
}

uint16_t Records::set_record_param(uint16_t trade_save_days, uint16_t log_save_days, int lvl)
{
	uint16_t ret = ERR_INPUT_INVALID;

	do
	{
		if (trade_save_days > 50)
			break;
		current_trade_save_days = trade_save_days;

		if (log_save_days > 50)
			break;
		current_log_save_days = log_save_days;

		if (lvl < level_invalid || lvl > level_disaster)
			break;
		current_log_level = (LOG_LEVEL)lvl;

		g_Record.delete_file_overdue();

		ret = 0;

	} while (0);

	return ret;
}

void Records::get_record_param(uint16_t& trade_save_days, uint16_t& log_save_days, int& lvl)
{
	trade_save_days = current_trade_save_days;
	log_save_days = current_log_save_days;
	lvl = (int)current_log_level;
}

bool Records::reach_log_level(uint16_t err_code, LOG_LEVEL lvl_appoint)
{
	LOG_LEVEL lvl_find = level_invalid;
	size_t start_ptr = 0;
	size_t end_ptr = sizeof(error_level_cmp)/sizeof(error_level_cmp[0]) - 1;
	size_t mid_ptr;

	do
	{
		mid_ptr = (start_ptr + end_ptr) / 2;

		if (err_code > error_level_cmp[mid_ptr].err_code)				// �����°���
		{
			start_ptr = mid_ptr + 1;
		}
		else if (err_code < error_level_cmp[mid_ptr].err_code)		// �����ϰ���
		{
			end_ptr = mid_ptr - 1;
		}
		else
		{
			lvl_find = error_level_cmp[mid_ptr].err_level;
			break;
		}

	} while (start_ptr <= end_ptr);

	if (lvl_find >= current_log_level || lvl_appoint >= current_log_level)
		return true;

	return false;
}

bool Records::record_trade_start()
{
	if (access("/app/Trade", 0) == 0)
		system("rm -rf /app/Trade");

	if (access("/app/Log", 0) == 0)
		system("rm -rf /app/Log");

	if (access("/data", 0) != 0 && mkdir("/data", 0755) != 0)
		return false;

	if (access(NAME_LOG_FOLDER, 0) != 0 && mkdir(NAME_LOG_FOLDER, 0755) != 0)
		return false;

	if (access(NAME_TRADE_FOLDER, 0) != 0 && mkdir(NAME_TRADE_FOLDER, 0755) != 0)
		return false;

	if (sem_init(&semaphore_handle, 0, 0) != 0)
		return false;

	if (pthread_create(&thread_handle, NULL, record_proc, this) != 0)
		return false;

	return true;
}

void Records::record_trade_end()
{
	void * p_thread_result;
	sem_post(&semaphore_handle);
	pthread_join(thread_handle, &p_thread_result);
	sem_destroy(&semaphore_handle);
}

void Records::add_record(char * p_trade_type, void * p_trade_data, size_t size_trade_data)
{
	TDSTRUCT ts = {0};

	if (size_trade_data > 0)
	{
		Publics::string_to_bcds(p_trade_type, 2, &ts.trade_type, 1);
		memcpy(ts.trade_time, CmdSort::m_time_now, 7);
		memcpy(ts.trade_data, p_trade_data, size_trade_data);
		ts.size = size_trade_data;

		trade_array.push_back(ts);
		sem_post(&semaphore_handle);
	}
}

void Records::get_file_suffix(uint8_t trade_type, char * p_suffix)
{
	static TRADESUFFIX suffix_array[] =
	{
		{0x50, "sjt"},	// ����Ʊ����
		{0x51, "ots"},	// ��ֵƱ����
		{0x53, "eng"},	// ��բ
		{0x54, "pur"},	// Ǯ������
		{0x55, "dfr"},	// ����
		{0x56, "upd"},	// ����
		{0x57, "ref"},	// �˿�
		{0x59, "loc"}	// �ӽ���
	};

	for (size_t i=0;i<sizeof(suffix_array)/sizeof(suffix_array[0]);i++)
	{
		if (trade_type == suffix_array[i].trade_type)
			strcpy(p_suffix, suffix_array[i].file_suffix);
	}
}

// д����
uint16_t Records::record_trade(TDSTRUCT& tdStruct, char * pDir)
{
	char szPathName[_POSIX_PATH_MAX]	= {0};
	char szFileName[32]					= {0};
	uint16_t ret						= ERR_FILE_ACCESS;
	FILE * fp;

	if (access(pDir, 0) == 0)
	{
		get_file_name(tdStruct, szFileName);
		sprintf(szPathName, "%s%s", pDir, szFileName);

		fp = fopen(szPathName, "a+b");
		if (fp != NULL)
		{
			if (fwrite(tdStruct.trade_data, 1, tdStruct.size, fp) == tdStruct.size)
				ret = 0;

			fclose(fp);
		}
	}

	return ret;
}

void Records::get_file_name(TDSTRUCT& tdStruct, char * p_file_name)
{
	char sz_suffix[8]	= {0};
	char sz_date[16]	= {0};
	uint8_t api_ver[2]	= {0};

	get_file_suffix(tdStruct.trade_type, sz_suffix);
	Publics::bcds_to_string(tdStruct.trade_time, 4, sz_date, sizeof(sz_date));

	memcpy(api_ver, API_VERSION, 2);

	sprintf(p_file_name, "%s_%02X%02X.%s", sz_date, api_ver[0], api_ver[1], sz_suffix);
}

void * Records::record_proc(void * arg)
{
	Records * pRcd = (Records *)arg;
	vector<TDSTRUCT>::iterator itor;

	while (CmdSort::m_flag_app_run)
	{
		if (sem_wait(&pRcd->semaphore_handle) == 0)
		{
			for (size_t pos=0;pos<pRcd->trade_array.size();pos++)
			{
				usleep(1000);

				if (pRcd->record_trade(pRcd->trade_array[pos], (char *)NAME_TRADE_FOLDER) == 0)
				{
					pRcd->trade_array.erase(pRcd->trade_array.begin() + pos);
					pos --;
				}
			}

		}
	}

	return NULL;
}

void Records::ergodic_dir_delete_target(char * p_path_dir, uint8_t * p_today, int save_time, int date_offset_filename)
{
	struct dirent *	ent				= NULL;
	uint8_t temp_buf[8]				= {0};
	char date_start[9]				= {0};
	char date_end[9]				= {0};
	char pathname[_POSIX_PATH_MAX]	= {0};

	memcpy(temp_buf, p_today, 4);
	TimesEx::bcd_time_calculate(temp_buf, T_DATE, save_time);
	sprintf(date_start, "%02x%02x%02x%02x", temp_buf[0], temp_buf[1], temp_buf[2], temp_buf[3]);
	sprintf(date_end, "%02x%02x%02x%02x", p_today[0], p_today[1], p_today[2], p_today[3]);


	if (access(p_path_dir, 0) == 0)
	{
		DIR * pDir = opendir(p_path_dir);

		while((ent = readdir(pDir)) != NULL)
		{
			if(ent->d_type & DT_REG)
			{
				if (memcmp(ent->d_name + date_offset_filename, date_start, 8) < 0 ||
					memcmp(ent->d_name + date_offset_filename, date_end, 8) > 0)
				{
					sprintf(pathname, "%s/%s", p_path_dir, ent->d_name);
					remove(pathname);
				}
			}
		}

		closedir(pDir);
	}
}

void Records::delete_file_overdue()
{
	ergodic_dir_delete_target(NAME_TRADE_FOLDER, CmdSort::m_time_now, -28, 0);
	ergodic_dir_delete_target(NAME_LOG_FOLDER, CmdSort::m_time_now, -28, 0);
}


// �ж�һ���ļ����Ƿ�Ϊ��
bool Records::folder_null(char * p_folder)
{
	int num	= 0; 

	DIR * pDir = opendir(p_folder);

	while(readdir(pDir) != NULL)
	{
		num++;
	}

	closedir(pDir);

	return (bool)(num == 2);
}

void Records::run_time_count_start()
{
	gettimeofday(&m_tp_start, NULL);
}

void Records::run_time_count_end()
{
	gettimeofday(&m_tp_end, NULL);
}

int Records::run_time()
{
	return (int)(1000 * (m_tp_end.tv_sec - m_tp_start.tv_sec) + m_tp_end.tv_usec - m_tp_start.tv_usec);
}
