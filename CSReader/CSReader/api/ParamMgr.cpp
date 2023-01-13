#include "ParamMgr.h"
#include "Publics.h"
#include "Api.h"
#include "TimesEx.h"
#include "DataSecurity.h"
#include "CmdSort.h"
#include "Errors.h"
#include "QFile.h"
#include "PackageMgr.h"
#include <stdlib.h>
#include "../link/linker.h"
#include "Records.h"
#include <dirent.h>
#include <sys/types.h>
#include "../link/myprintf.h"
#include <unistd.h>

ParamMgr g_Parameter;

// 参数类型
#define PRM_0101_DEVICE_CTRL		(uint16_t)0x0101				// 设备控制参数
#define PRM_0202_STATION_CFG		(uint16_t)0x0202				// 车站表
#define PRM_0203_SAM_CMP			(uint16_t)0x0203				// SAM卡对照表
#define PRM_0204_STATION_EX			(uint16_t)0x0204				// 换乘车站代码表
#define PRM_0301_TICKET				(uint16_t)0x0301				// 票卡参数
#define PRM_0302_ALLEY_WAY			(uint16_t)0x0302				// 闸机专用通道参数
#define PRM_0303_TCT				(uint16_t)0x0303				// 乘次票专用参数
#define PRM_0400_FARE				(uint16_t)0x0400				// 票价文件
#define PRM_0501_DEGRADE			(uint16_t)0x0501				// 降级模式使用记录
#define PRM_0601_SINGLE_BLACK_M		(uint16_t)0x0601				// 地铁单个黑名单
#define PRM_0602_SECTION_BLACK_M	(uint16_t)0x0602				// 地铁黑名单段
#define PRM_0603_SINGLE_BLACK_O		(uint16_t)0x0603				// 一卡通单个黑名单
#define PRM_0604_SECTION_BLACK_O	(uint16_t)0x0604				// 一卡通黑名单段
#define PRM_0605_JTB_SINGLE_BLACK_O	(uint16_t)0x0605				// 交通部一卡通单个黑名单
#define PRM_0606_JTB_WHITE_O	    (uint16_t)0x0606				// 交通部一卡通白名单
#define PRM_0801_POLICE_FINE		(uint16_t)0x0801				// 行政罚金参数
#define PRM_9120_READER				(uint16_t)0x9120				// 读写器应用参数
#define PRM_9220_READER				(uint16_t)0x9220				// 读写器应用参数
#define PRM_9320_READER				(uint16_t)0x9320				// 读写器应用参数
#define PRM_9420_READER				(uint16_t)0x9420				// 读写器应用参数
#define PRM_9620_READER				(uint16_t)0x9620				// 读写器应用参数



// 互联读写器相关参数
#define IPRM_0001_NETWORKTOP_CFG	(uint16_t)0x0001				// 网络拓扑参数
#define IPRM_0002_CALENDARS_CFG		(uint16_t)0x0002				// 日历参数
#define IPRM_0003_ETICKET_CFG		(uint16_t)0x0003				// 车票参数
#define IPRM_0004_RATE_CFG			(uint16_t)0x0004				// 费率参数
#define IPRM_0005_SYSTEM_CFG		(uint16_t)0x0005				// 系统参数
#define IPRM_0006_AGMNETWORK_CFG	(uint16_t)0x0006				// 路网AGM参数
#define IPRM_0007_TVMNETWORK_CFG	(uint16_t)0x0007				// 路网TVM参数
#define IPRM_0010_METROBLCAK_CFG	(uint16_t)0x0010				// 地铁票全量黑名单
#define IPRM_0012_EXTTICKET_CFG		(uint16_t)0x0012				// 外部票种映射参数
#define IPRM_0080_COMMUNICATION_CFG	(uint16_t)0x0080				// 通讯参数
#define IPRM_1002_READER            (uint16_t)0x1002				// 互联网读写器应用参数


ParamMgr::PRM_FUNC_LIST ParamMgr::m_prm_for_reader[] =
{
    {PRM_0101_DEVICE_CTRL,		&ParamMgr::load_device_control},
    {PRM_0202_STATION_CFG,		&ParamMgr::load_station_config},
    {PRM_0203_SAM_CMP,			&ParamMgr::load_sam_compare},
    {PRM_0204_STATION_EX,		&ParamMgr::load_change_station},
    {PRM_0301_TICKET,			&ParamMgr::load_ticket_prm},
    {PRM_0302_ALLEY_WAY,		&ParamMgr::load_special_passageway},
    {PRM_0303_TCT,				&ParamMgr::load_tct_config},
    {PRM_0400_FARE,				&ParamMgr::load_fare_prm},
    {PRM_0501_DEGRADE,			&ParamMgr::load_degrade_mode},
    {PRM_0601_SINGLE_BLACK_M,	&ParamMgr::load_mtr_single_black},
    {PRM_0602_SECTION_BLACK_M,	&ParamMgr::load_mtr_section_black},
    {PRM_0603_SINGLE_BLACK_O,	&ParamMgr::load_oct_single_black},
    {PRM_0604_SECTION_BLACK_O,	&ParamMgr::load_oct_section_black},
	{PRM_0605_JTB_SINGLE_BLACK_O,&ParamMgr::load_jtb_single_black},
	{PRM_0606_JTB_WHITE_O,	    &ParamMgr::load_jtb_white},
    {PRM_0801_POLICE_FINE,		&ParamMgr::load_policy_penalty},
	{PRM_9120_READER,			&ParamMgr::load_reader_app},
	{PRM_9220_READER,			&ParamMgr::load_reader_app},
	{PRM_9320_READER,			&ParamMgr::load_reader_app},
	{PRM_9420_READER,			&ParamMgr::load_reader_app},
	{PRM_9620_READER,			&ParamMgr::load_reader_app}/*,

	{IPRM_0001_NETWORKTOP_CFG,	&ParamMgr::load_networktop},
	{IPRM_0002_CALENDARS_CFG,	&ParamMgr::load_calendars},
	{IPRM_0003_ETICKET_CFG,		&ParamMgr::load_eticket},
	{IPRM_0004_RATE_CFG,		&ParamMgr::load_rate},
	{IPRM_1002_READER,			&ParamMgr::load_reader_app}*/
};

ParamMgr::IPRM_FUNC_LIST ParamMgr::m_iprm_for_reader[] =
{
	{IPRM_0001_NETWORKTOP_CFG,	&ParamMgr::load_networktop},
	{IPRM_0002_CALENDARS_CFG,	&ParamMgr::load_calendars},
	{IPRM_0003_ETICKET_CFG,		&ParamMgr::load_eticket},
	{IPRM_0004_RATE_CFG,		&ParamMgr::load_rate},
	//{IPRM_0005_SYSTEM_CFG,		&ParamMgr::load_ticket_prm},
	//{IPRM_0006_AGMNETWORK_CFG,	&ParamMgr::load_special_passageway},
	//{IPRM_0007_TVMNETWORK_CFG,	&ParamMgr::load_tct_config},
	{IPRM_0010_METROBLCAK_CFG,	&ParamMgr::load_itp_mtr_single_black},
	{IPRM_0012_EXTTICKET_CFG,	&ParamMgr::load_ext_ticket},
	//{IPRM_0080_COMMUNICATION_CFG,&ParamMgr::load_mtr_single_black},
	{IPRM_1002_READER,			&ParamMgr::load_reader_app}
};


//
// class definition
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ParamMgr::ParamMgr(void)
{
	device_init_flag = false;
}

ParamMgr::~ParamMgr(void)
{
    unload_device_control();
    unload_station_config();
    unload_sam_compare();
    unload_change_station();
    unload_ticket_prm();
    unload_special_passageway();
    unload_tct_config();
    unload_fare_prm();
    unload_degrade_mode();
    unload_mtr_single_black();
    unload_mtr_section_black();
    unload_oct_single_black();
    unload_oct_section_black();
	unload_jtb_white();
	unload_jtb_single_black();
    unload_policy_penalty();
}


bool ParamMgr::load_itp_dat_config()
{
	char sz_temp[_POSIX_PATH_MAX]	= {0};
    char sz_name_prm[32]			= {0};
	size_t read_cnt					= 0;
	uint16_t prm_type;
    size_t len_prm_name;
	QFile file;

    m_iprm_config.clear();

    sprintf(sz_temp, "%s/%s", QFile::running_directory(), NAME_IPRM_CONFIG);

    if (access(sz_temp, 0) == 0)
    {
        if (file.open(sz_temp, mode_read_text))
        {
            while (file.read_line(sz_temp, sizeof(sz_temp), read_cnt))
            {
                prm_type = Publics::string_to_bcd<uint16_t>(sz_temp, 4);
                if (iprm_for_reader(prm_type))
                {
                    memset(sz_name_prm, 0, sizeof(sz_name_prm));
                    len_prm_name = strlen(sz_temp);
                    if (sz_temp[len_prm_name - 1] == '\n')
                        len_prm_name -= 6;
                    else
                        len_prm_name -= 5;

                    memcpy(sz_name_prm, sz_temp + 5, len_prm_name);
                    m_iprm_config.insert(make_pair(prm_type, sz_name_prm));
                }
            }
            file.close();

            return true;
        }
    }

    return false;

}

bool ParamMgr::load_dat_config()
{
	char sz_temp[_POSIX_PATH_MAX]	= {0};
    char sz_name_prm[32]			= {0};
	size_t read_cnt					= 0;
	uint16_t prm_type;
    size_t len_prm_name;
	QFile file;

    m_prm_config.clear();

    sprintf(sz_temp, "%s/%s", QFile::running_directory(), NAME_PRM_CONFIG);
	HS_LOG("load m_prm_config %s \n",sz_temp);

    if (access(sz_temp, 0) == 0)
    {
    
		HS_LOG("file exist \n");
        if (file.open(sz_temp, mode_read_text))
        {
        
			HS_LOG("file open succes  \n");
            while (file.read_line(sz_temp, sizeof(sz_temp), read_cnt))
            {
            
                prm_type = Publics::string_to_bcd<uint16_t>(sz_temp, 4);
				
				HS_LOG("read_line read_cnt=%d  prm_type=0x%x\n",read_cnt,prm_type);
                if (prm_for_reader(prm_type))
                {
                    memset(sz_name_prm, 0, sizeof(sz_name_prm));
                    len_prm_name = strlen(sz_temp);
                    if (sz_temp[len_prm_name - 1] == '\n')
                        len_prm_name -= 6;
                    else
                        len_prm_name -= 5;

                    memcpy(sz_name_prm, sz_temp + 5, len_prm_name);
					
					HS_LOG("prm_type=%d sz_name_prm %s \n",prm_type,sz_name_prm);
                    m_prm_config.insert(make_pair(prm_type, sz_name_prm));
                }
            }
            file.close();
			HS_LOG("file close succes  \n");

            return true;
        }
    }


    return false;

}

// ITP参数类型是否读写器必须
bool ParamMgr::iprm_for_reader(uint16_t prm_type)
{
    for (uint8_t i=0; i<sizeof(m_iprm_for_reader)/sizeof(m_iprm_for_reader[0]); i++)
    {
        if (prm_type == m_iprm_for_reader[i].iprm_type)
            return true;
    }
    return false;
}

// 参数类型是否读写器必须
bool ParamMgr::prm_for_reader(uint16_t prm_type)
{
    for (uint8_t i=0; i<sizeof(m_prm_for_reader)/sizeof(m_prm_for_reader[0]); i++)
    {
        if (prm_type == m_prm_for_reader[i].prm_type)
            return true;
    }
    return false;
}

// 参数是否则配置文件中
bool ParamMgr::name_in_config(char * p_file_name)
{
	bool ret = false;

	map<uint16_t, string>::iterator itor;
	for (itor=m_prm_config.begin(); itor!=m_prm_config.end(); ++itor)
	{
		if (strcmp(p_file_name, (itor->second).c_str()) == 0)
		{
			ret = true;
			break;
		}
	}

	return ret;
}

// 更改参数配置
bool ParamMgr::save_prm_config(uint16_t prm_type, const string& strPath)
{
    char sz_to_write[_POSIX_PATH_MAX + 8]	= {0};
    char sz_config_path[_POSIX_PATH_MAX]	= {0};
    size_t len_to_write						= 0;
    bool add_new_to_config					= true;
	bool ret								= true;
	QFile file;


	do
	{
		sprintf(sz_config_path, "%s/%s", QFile::running_directory(), NAME_PRM_CONFIG);
		HS_LOG("save m_prm_config %s \n",sz_config_path);

		if (!file.open(sz_config_path, mode_write))
			break;

		map<uint16_t, string>::iterator itor;
		for (itor=m_prm_config.begin(); itor!=m_prm_config.end(); ++itor)
		{
			if (prm_type == itor->first)
			{
				add_new_to_config = false;
				itor->second = strPath;
			}
			sprintf(sz_to_write, "%04x:%s\n", itor->first, (itor->second).c_str());
			
			HS_LOG("sz_to_write %s \n",sz_to_write);
			len_to_write = strlen(sz_to_write);
			if (file.write(sz_to_write, len_to_write) < len_to_write)
			{
				ret = false;
				break;
			}
		}

		
		HS_LOG("ret %d add_new_to_config=%d  \n",ret,add_new_to_config);

		if (ret && add_new_to_config)
		{
			m_prm_config.insert(make_pair(prm_type, strPath));

			sprintf(sz_to_write, "%04x:%s\n", prm_type, strPath.c_str());
			
			HS_LOG("sz_to_write %s \n",sz_to_write);
			len_to_write = strlen(sz_to_write);
			if (file.write(sz_to_write, len_to_write) < len_to_write)
			{
				ret = false;
			}
		}

		file.close();

	} while (0);

	if (!ret)
		g_Record.log_out(0, level_error, "save_prm_config(%04x,%s)=false", prm_type, strPath.c_str());

	HS_LOG("ret %d save_prm_config(%04x,%s) \n",ret, prm_type, strPath.c_str());

    return ret;
}

// 查询参数版本
bool ParamMgr::query_prm_version(uint16_t prm_type, char * p_prm_name)
{
    map<uint16_t, string>::iterator itor = m_prm_config.find(prm_type);
    if (itor != m_prm_config.end())
    {
        strcpy(p_prm_name, itor->second.c_str());
        return true;
    }

    return false;
}

// 加载所有参数
int ParamMgr::load_all_prms()
{
    int ret							= 0;
	char prm_path[_POSIX_PATH_MAX]	= {0};


    if (prm_config_unison())
    {
        map<uint16_t, string>::iterator itor;
        for (itor=m_prm_config.begin(); itor!= m_prm_config.end(); ++itor)
        {
			sprintf(prm_path, "%s/%s/%s", QFile::running_directory(), NAME_PRM_FOLDER, itor->second.c_str());
			HS_LOG("m_prm_config prm_path=%s \n", prm_path);

            ret = load_prm(itor->first, prm_path);
            if (ret)	break;
        }
    }
    else
    {
        ret = ERR_PARAM_NOT_EXIST;
    }

    return ret;
}

// 载入读写器应用参数
int ParamMgr::load_reader_app(const char * p_prm_path, bool only_check/* = false*/)
{
	return 0;
}

// 二分法查找一维字符数组
char * ParamMgr::method_of_bisection(char * p_target_str, int len_to_cmp, char * p_array_1d, int foruint16_t_step, int record_count)
{
    int start_ptr, mid_ptr, end_ptr;
    int result_cmp;
    char * p_src_str;

    if (record_count > 0)
    {
        start_ptr = 0;
        end_ptr = record_count - 1;

        do
        {
            mid_ptr = (start_ptr + end_ptr) / 2;

            p_src_str = p_array_1d + mid_ptr * foruint16_t_step;

            result_cmp = memcmp(p_target_str, p_src_str, len_to_cmp);

            if (result_cmp > 0)				// 进入下半区
                start_ptr = mid_ptr + 1;
            else if (result_cmp < 0)		// 进入上半区
                end_ptr = mid_ptr - 1;
            else
                return p_src_str;

        }
        while (start_ptr < end_ptr);
    }

    return NULL;
}

// 二分法查找二维字符数组
bool ParamMgr::method_of_bisection(char * p_target_str, int len_to_cmp, char ** p_array_2d, int size_1d)
{
    int start_ptr, mid_ptr, end_ptr;
    int result_cmp;

    if (size_1d > 0)
    {
        start_ptr = 0;
        end_ptr = size_1d - 1;

        do
        {
            mid_ptr = (start_ptr + end_ptr) / 2;

            result_cmp = memcmp(p_target_str, p_array_2d[mid_ptr], len_to_cmp);

            if (result_cmp > 0)				// 进入下半区
                start_ptr = mid_ptr + 1;
            else if (result_cmp < 0)		// 进入上半区
                end_ptr = mid_ptr - 1;
            else
				return true;

        } while (end_ptr >= start_ptr);
    }

    return false;
}
// 二分法查找二维字符数组
bool ParamMgr::method_of_bisection(char * p_target_str, int len_to_cmp, int len_record, char ** p_array_2d, int record_count, char ** pp_find)
{
	int start_ptr, mid_ptr, end_ptr;
	int result_cmp;

	if (record_count > 0)
	{
		start_ptr = 0;
		end_ptr = record_count - 1;

		do
		{
			mid_ptr = (start_ptr + end_ptr) / 2;

			result_cmp = memcmp(p_target_str, p_array_2d[mid_ptr], len_to_cmp);

			if (result_cmp > 0)				// 进入下半区
				start_ptr = mid_ptr + 1;
			else if (result_cmp < 0)		// 进入上半区
				end_ptr = mid_ptr - 1;
			else
			{
				if (pp_find != NULL)
					*pp_find = p_array_2d[mid_ptr];
				return true;
			}

		} while (end_ptr >= start_ptr);
	}

	return false;
}


// 检查参数配置中的参数是否足够
bool ParamMgr::prm_config_unison()
{
    // 一次检查完所有的配置

    bool ret = true;
    for (size_t i=0; i<sizeof(m_prm_for_reader)/sizeof(m_prm_for_reader[0]); i++)
    {
    
		HS_LOG("prm_config_unison lack of %04x i=%d\n", m_prm_for_reader[i].prm_type,i);
        if (m_prm_config.find(m_prm_for_reader[i].prm_type) == m_prm_config.end() && (!(((m_prm_for_reader[i].prm_type & 0xF0F0) == 0x9020) || (m_prm_for_reader[i].prm_type == 0x1002))))
        {
        
            ret = false;
			HS_LOG("m_prm_config size=%d \n", m_prm_config.size());
			HS_LOG("prm_config_unison lack of %04x\n", m_prm_for_reader[i].prm_type);
			g_Record.log_out(0, level_error, "prm_config_unison lack of %04x", m_prm_for_reader[i].prm_type);
			break;
        }
    }
    return ret;
}

// 参数段首是否匹配
bool ParamMgr::prm_section_match(char * p_prm_section_header, uint16_t section_type)
{
    char sz_section_type[5] = {0};
    sprintf(sz_section_type, "%04X", section_type);

	if (p_prm_section_header[4] == ':')
		return (memcmp(p_prm_section_header, sz_section_type, 4) == 0);
	return false;
}


// ITP参数文件头是否正确
bool ParamMgr::iprm_head_match(char * p_iprm_section_header, uint16_t section_type)
{
	char sz_section_type[5] = {0};
    sprintf(sz_section_type, "%04X", section_type);

	char sz_iprm_section[5] = {0};
	Publics::bcds_to_string((uint8_t *)(p_iprm_section_header + 9), 2, sz_iprm_section, 4);

	return (memcmp(sz_iprm_section, sz_section_type, 4) == 0);
}

// 设备是否完成初始化，即加载参数（读写器异常重启后，设备程序无法知晓，导致没有调用设备初始化）
void ParamMgr::device_initialize()
{
	device_init_flag = true;
}

uint16_t ParamMgr::device_initialized()
{
	HS_LOG("device_init_flag %d\n", device_init_flag);

	if (device_init_flag)
		return 0;

	return ERR_DEVICE_UNINIT;
}

// 加载单个参数
uint16_t ParamMgr::load_prm(const uint16_t prm_type, const char * p_prm_path, bool cmd_reset/* = false*/, bool only_check/* = false*/)
{
	uint16_t ret = 0;

	do
	{

	
		HS_LOG("prm_type=%x \n", prm_type);
		// 直接移动到下载文件启用消息
		if ((uint16_t)(prm_type & 0xF0F0) == 0x9020)
		{
			if (cmd_reset)
			{
				CmdSort::m_flag_app_run = false;
				g_Record.log_out(ret, level_error, "reader reset......");
			}
			break;
		}

		/*
		if (!DataSecurity::crc32_in_file_end_valid(p_prm_path))
		{
			ret = ERR_PARAM_INVALID;
			g_Record.log_out(ret, level_error, "the crc of file %s is wrong", p_prm_path);
			break;
		}*/

		P_FUNC_PRM pfun;
		for (size_t i=0; i<sizeof(m_prm_for_reader)/sizeof(m_prm_for_reader[0]); i++)
		{
			if (m_prm_for_reader[i].prm_type == prm_type)
			{
				pfun = m_prm_for_reader[i].fun;
				ret = (uint16_t)(this->*pfun)(p_prm_path, only_check);
				break;
			}
		}

	} while (0);

	HS_LOG("load_prm(%04x,%s, bool,bool)=%04x", prm_type, p_prm_path, ret);

	g_Record.log_out(ret, level_invalid, "load_prm(%04x,%s, bool,bool)=%04x", prm_type, p_prm_path, ret);

	return ret;
}

// 载入设备控制参数
int ParamMgr::load_device_control(const char * p_prm_path, bool only_check/* = false*/)
{
    char sz_read[128]	= {0};
	int ret				= 0;
	size_t read_cnt		= 0;
    QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		if (!only_check)
		{
			unload_device_control();
		}

		while(fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			if (prm_section_match(sz_read, PRM_0101_DEVICE_CTRL))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
					m_prm_device_ctrl.sc_auto_destroy = Publics::string_to_val<long>(sz_read + 5, 4);
					m_prm_device_ctrl.sc_reconnect_lcc = Publics::string_to_val<long>(sz_read + 9, 4);
					m_prm_device_ctrl.bom_auto_destroy = Publics::string_to_val<long>(sz_read + 13, 4);
					m_prm_device_ctrl.entry_exit_timeout = Publics::string_to_val<long>(sz_read + 17, 4);
					m_prm_device_ctrl.degrade_effect_time = Publics::string_to_val<long>(sz_read + 21, 2);
					m_prm_device_ctrl.svt_charge_max = Publics::string_to_val<long>(sz_read + 23, 8);
					m_prm_device_ctrl.loyate_charge_max = Publics::string_to_val<long>(sz_read + 31, 8);
					m_prm_device_ctrl.sjt_sale_max = Publics::string_to_val<long>(sz_read + 39, 8);
					m_prm_device_ctrl.svt_charge_min = Publics::string_to_val<long>(sz_read + 47, 8);
					m_prm_device_ctrl.loyate_charge_min = Publics::string_to_val<long>(sz_read + 55, 8);
					m_prm_device_ctrl.sjt_sale_min = Publics::string_to_val<long>(sz_read + 63, 8);
				}
			}
		}

		fp.close();

	} while (0);

    return ret;
}
void ParamMgr::unload_device_control()
{
    memset(&m_prm_device_ctrl, 0, sizeof(DEVICE_CTRL));
}
void ParamMgr::query_device_ctrl(DEVICE_CTRL& ctrl)
{
    memcpy(&ctrl, &m_prm_device_ctrl, sizeof(DEVICE_CTRL));
}

// 载入车站表
int ParamMgr::load_station_config(const char * p_prm_path, bool only_check/* = false*/)
{
#define LEN_STATION_LINE	(5 + 4 + 3 + 2 + 1 + 3 + 3 + 15 + 30 + 2)

    STATION_CONFIG cfg	= {0};
    char sz_read[128]	= {0};
	int ret				= 0;
	size_t read_cnt		= 0;
    QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		if (!only_check)
		{
			unload_station_config();
		}

		while(fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			if (prm_section_match(sz_read, PRM_0202_STATION_CFG))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::FREE_SEQ))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
					memset(&cfg, 0, sizeof(cfg));

					cfg.station_id = Publics::string_to_bcd<uint16_t>(sz_read + 5, 4);
					cfg.device_id = Publics::string_to_bcd<uint16_t>(sz_read + 9, 3);
					cfg.device_type = Publics::string_to_bcd<uint8_t>(sz_read + 12, 2);

					m_prm_station_config.push_back(cfg);
				}
			}
		}

		fp.close();

	} while (0);

    return ret;
}
void ParamMgr::unload_station_config()
{
    m_prm_station_config.clear();
}
uint16_t ParamMgr::valid_station(uint16_t station_id)
{
    for (uint32_t i=0; i<m_prm_station_config.size(); i++)
    {
        if (station_id == m_prm_station_config[i].station_id)
            return 0;
    }

    return ERR_STATION_INVALID;
}
int ParamMgr::device_registed(uint16_t station_id, uint8_t device_type, uint16_t device_id)
{
    for (uint32_t i=0; i<m_prm_station_config.size(); i++)
    {
        if (station_id == m_prm_station_config[i].station_id &&
                device_type == m_prm_station_config[i].device_type &&
                device_id == m_prm_station_config[i].device_id)
            return 0;
    }

    return ERR_DEVICE_INVALID;
}

// 载入SAM卡对照表
int ParamMgr::load_sam_compare(const char * p_prm_path, bool only_check/* = false*/)
{
#define LEN_SAMCMP_LINE		(5 + 16 + 1 + 4 + 3 + 2 + 2)
#define LEN_SAMCMP_DATA		(16 + 1 + 4 + 3 + 2)

    char sz_read[128]	= {0};
    uint32_t file_size	= 0;
	int ret				= 0;
	size_t read_cnt		= 0;
    QFile fp;


	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		file_size = (uint32_t)(fp.length() - 12);
		if (file_size % LEN_SAMCMP_LINE != 0)
		{
			ret = ERR_PARAM_INVALID;
			break;
		}

		if (!only_check)
		{
			unload_sam_compare();

			m_prm_sam_compare.sam_device_info = new(std::nothrow) char*[file_size / LEN_SAMCMP_LINE];
			if (m_prm_sam_compare.sam_device_info == NULL)
			{
				ret = ERR_UNDEFINED;
				break;
			}
		}

		while(fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			if (prm_section_match(sz_read, PRM_0203_SAM_CMP))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::HEX_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
					m_prm_sam_compare.sam_device_info[m_prm_sam_compare.record_count] = new(std::nothrow) char[LEN_SAMCMP_DATA];
					if (m_prm_sam_compare.sam_device_info[m_prm_sam_compare.record_count] == NULL)
					{
						ret = ERR_UNDEFINED;
						break;
					}
					memcpy(m_prm_sam_compare.sam_device_info[m_prm_sam_compare.record_count], sz_read + 5, LEN_SAMCMP_DATA);
					m_prm_sam_compare.record_count ++;
				}
			}
		}

		fp.close();

	} while (0);

    return ret;
}
void ParamMgr::unload_sam_compare()
{
    if (m_prm_sam_compare.sam_device_info != NULL)
    {
        for (int i=0; i<m_prm_sam_compare.record_count; i++)
        {
            if (m_prm_sam_compare.sam_device_info[i] != NULL)
            {
                delete []m_prm_sam_compare.sam_device_info[i];
                m_prm_sam_compare.sam_device_info[i] = NULL;
            }
        }

        delete []m_prm_sam_compare.sam_device_info;
        m_prm_sam_compare.sam_device_info = NULL;
    }

    m_prm_sam_compare.record_count = 0;
}
bool ParamMgr::device_from_sam(const char * p_sam_id, const uint8_t sam_type, uint8_t * p_bcd_station, uint8_t * p_device_type, uint8_t * p_bcd_device_id)
{
    bool find_result		= false;
    uint8_t bStationID[2]	= {0};
    uint8_t bDevType		= 0;
    uint8_t bDevID[2]		= {0};
	char sz_temp[32]		= {0};

	sprintf(sz_temp, "%s%d", p_sam_id, sam_type + 1);
	char * p_record = NULL;
	if (method_of_bisection(sz_temp, 17, LEN_SAMCMP_DATA, m_prm_sam_compare.sam_device_info, m_prm_sam_compare.record_count, &p_record))
	{
		Publics::string_to_hexs(p_record + 17, 4, bStationID, sizeof(bStationID));
		Publics::string_to_hexs(p_record + 21, 3, bDevID, sizeof(bDevID));
		Publics::string_to_hexs(p_record + 24, 2, &bDevType, sizeof(bDevType));
	}

    if (p_bcd_station != NULL)
    	memcpy(p_bcd_station, bStationID, sizeof(bStationID));
    if (p_bcd_device_id != NULL)
    	memcpy(p_bcd_device_id, bDevID, sizeof(bDevID));
    if (p_device_type != NULL)
    	*p_device_type = bDevType;

    return find_result;
}
bool ParamMgr::sam_from_device(const uint8_t * p_bcd_station, const ETPDVC device_type, const uint8_t * p_bcd_device_id, const uint8_t sam_type, char * p_sam_id)
{
    bool find_result	= false;

	memset(p_sam_id, '0', 16);
    //char devInfo[32]	= {0};
    //char * p			= NULL;

    //memset(pSamID, '0', 16);
    //p = m_PrmSamCmp.SamDevID;
    //sprintf(devInfo, "%02X%02X%02X%02X%02X", pbcdStationID[0], pbcdStationID[1], dvcType, pbcdDevID[0], pbcdDevID[1]);
    //for (int i=0;i<m_PrmSamCmp.nRecord;i++)
    //{
    //	if (memcmp(p + 17, devInfo, 10) == 0)
    //	{
    //		bFind = TRUE;
    //		break;
    //	}
    //	p += LEN_SAMCMP_DATA;
    //}
    //if (bFind)
    //{
    //	bFind = FALSE;

    //	switch (bSamType)
    //	{
    //	case SOCK_SAM_OLD:
    //		// LSAM1, PSAM2, PISAM7
    //		if (p[16] == '1' || p[16] == '2' || p[16] == '7')
    //			bFind = TRUE;
    //		break;
    //	case SOCK_SAM_OCT:
    //		// PSAM0-CPU, ISAM3, PSAM4-MF
    //		if (p[16] == '0' || p[16] == '3' || p[16] == '4')
    //			bFind = TRUE;
    //		break;
    //	case SOCK_SAM_MOBILE:
    //		// PSAM9
    //		if (p[16] == '9')
    //			bFind = TRUE;
    //		break;
    //	case SOCK_SAM_NEW:
    //		// ISAM5, PSAM6, PISAM7
    //		if (p[16] == '5' || p[16] == '6' || p[16] == '8')
    //			bFind = TRUE;
    //		break;
    //	}

    //	if (bFind)
    //	{
    //		memcmp(pSamID, p, 16);
    //	}
    //}

    return find_result;
}
bool ParamMgr::sam_from_device(const uint16_t station_id, const ETPDVC device_type, const uint16_t device_id, const uint8_t sam_type, char * p_sam_id)
{
	uint8_t bcd_station[2];
	uint8_t bcd_device[2];

	bcd_station[0] = (uint8_t)((station_id >> 8) & 0xFF);
	bcd_station[0] = (uint8_t)(station_id & 0xFF);
	bcd_device[0] = (uint8_t)((device_id >> 8) & 0xFF);
	bcd_device[0] = (uint8_t)(device_id & 0xFF);

	return sam_from_device(bcd_station, device_type, bcd_device, sam_type, p_sam_id);
}
bool ParamMgr::sam_counterpart(uint8_t sam_sock, char * p_sam_id, uint8_t * p_station, uint8_t dev_type, uint8_t * p_dev_id, bool check_sam_match)
{
	bool ret				= false;
	char sz_src_device[32]	= {0};
	char sz_sam_inf[32]		= {0};
	char * p_find			= NULL;

	if (check_sam_match)
	{
		sprintf(sz_src_device, "%02X%02X%X%02X%02X", p_station[0], p_station[1], p_dev_id[0], p_dev_id[1], dev_type);
		sprintf(sz_sam_inf, "%s%X", p_sam_id, sam_sock + 1);
		if (method_of_bisection(sz_sam_inf, 17, LEN_SAMCMP_DATA, m_prm_sam_compare.sam_device_info, m_prm_sam_compare.record_count, &p_find))
		{
			if (memcmp(sz_src_device, p_find + 17, 9) == 0)
				ret = true;
		}
	}

	return ret;
}

// 载入换乘车站表
int ParamMgr::load_change_station(const char * p_prm_path, bool only_check/* = false*/)
{
    char sz_read[128]	= {0};
	int	ret				= 0;
	size_t read_cnt		= 0;
    uint16_t station1, station2;
    QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		if (!only_check)
		{
			unload_change_station();
		}

		while(fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			if (prm_section_match(sz_read, PRM_0204_STATION_EX))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
					station1 = Publics::string_to_bcd<uint16_t>(sz_read + 5, 4);
					station2 = Publics::string_to_bcd<uint16_t>(sz_read + 9, 4);

					m_prm_changing_station.push_back(make_pair(station1, station2));
				}
			}
		}
		fp.close();

	} while (0);

	return ret;
}
void ParamMgr::unload_change_station()
{
    m_prm_changing_station.clear();
}

// 载入票卡参数
int ParamMgr::load_ticket_prm(const char * p_prm_path, bool only_check/* = false*/)
{
    char sz_read[256]	= {0};
	int mov				= 0;
	int ret				= 0;
	size_t read_cnt		= 0;

    TICKET_PARAM node_ticket;
    QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		if (!only_check)
		{
			unload_ticket_prm();
		}

		while (fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			if (prm_section_match(sz_read, PRM_0301_TICKET))
			{
				memset(&node_ticket, 0, sizeof(TICKET_PARAM));

				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
					mov = 5;
					node_ticket.ticket_type			= Publics::string_to_bcd<uint16_t>(sz_read + mov, 4);
					mov += 4;
					node_ticket.wallet_type			= sz_read[mov] - '0';
					mov += 1;
					node_ticket.balance_max			= Publics::string_to_val<long>(sz_read + mov, 6);
					mov += 6;
					node_ticket.permit_over_draft	= Publics::char_to_bool(sz_read[mov]);
					mov += 1;
					node_ticket.value_over_draft	= Publics::string_to_val<long>(sz_read + mov, 6);
					mov += 6;
					node_ticket.permit_charge		= Publics::char_to_bool(sz_read[mov]);
					mov += 1;
					node_ticket.charge_max			= Publics::string_to_val<long>(sz_read + mov, 6);
					mov += 6;
					node_ticket.update_fare_mode	= sz_read[mov] - '0';
					mov += 1;
					node_ticket.check_update_station = Publics::char_to_bool(sz_read[mov]);
					mov += 1;
					node_ticket.check_update_day	= Publics::char_to_bool(sz_read[mov]);
					mov += 1;
					node_ticket.pemite_refund		= Publics::char_to_bool(sz_read[mov]);
					mov += 1;
					node_ticket.refund_min_count	= Publics::string_to_val<long>(sz_read + mov, 3);
					mov += 3;
					node_ticket.daliy_trade_max		= Publics::string_to_val<long>(sz_read + mov, 2);
					mov += 2;
					node_ticket.month_trade_max		= Publics::string_to_val<long>(sz_read + mov, 4);
					mov += 4;
					node_ticket.effect_time_from_sale = Publics::string_to_val<long>(sz_read + mov, 16);
					mov += 16;
					node_ticket.permit_deffer		= Publics::char_to_bool(sz_read[mov]);
					mov += 1;
					node_ticket.deffer_days			= Publics::string_to_val<long>(sz_read + mov, 4);
					mov += 4;
					node_ticket.deposite			= Publics::string_to_val<long>(sz_read + mov, 6);
					mov += 6;
					node_ticket.prime_cost			= Publics::string_to_val<long>(sz_read + mov, 6);
					mov += 6;
					node_ticket.sale_brokerage		= Publics::string_to_val<long>(sz_read + mov, 6);
					mov += 6;
					node_ticket.need_active_for_use = Publics::char_to_bool(sz_read[mov]);
					mov += 1;
					node_ticket.check_blacklist		= Publics::char_to_bool(sz_read[mov]);
					mov += 1;
					node_ticket.check_balance		= Publics::char_to_bool(sz_read[mov]);
					mov += 1;

					if (sz_read[mov] == '1' || sz_read[mov] == '3')
						node_ticket.check_phy_expire = true;
					if (sz_read[mov] == '1' || sz_read[mov] == '2')
						node_ticket.check_logic_expire = true;
					mov += 1;

					if (sz_read[mov] == '1' || sz_read[mov] == '3')
						node_ticket.check_entry_order = true;
					if (sz_read[mov] == '1' || sz_read[mov] == '2')
						node_ticket.check_exit_order = true;
					mov += 1;

					node_ticket.check_exit_timeout	= Publics::char_to_bool(sz_read[mov]);
					mov += 1;
					node_ticket.check_over_travel	= Publics::char_to_bool(sz_read[mov]);
					mov += 1;
					node_ticket.limit_station		= Publics::char_to_bool(sz_read[mov]);
					mov += 1;
					node_ticket.limit_the_Station	= Publics::char_to_bool(sz_read[mov]);
					mov += 1;
					memcpy(node_ticket.chargable_device, sz_read + mov, 16);
					mov += 16;
					memcpy(node_ticket.usable_device, sz_read + mov, 16);
					mov += 16;
					node_ticket.entry_sell_station	= !Publics::char_to_bool(sz_read[mov]);
					mov += 1;
					node_ticket.refund_max_value	= Publics::string_to_val<long>(sz_read + mov, 6);
					mov += 6;
					node_ticket.refund_brokerage	= Publics::string_to_val<long>(sz_read + mov, 6);
					mov += 6;
					memcpy(node_ticket.sell_device, sz_read + mov, 16);
					mov += 16;

					node_ticket.permit_refund_deposite = Publics::char_to_bool(sz_read[mov]);

					m_prm_ticket.push_back(node_ticket);
				}
			}
		}
		fp.close();

	} while (0);

    return ret;
}
void ParamMgr::unload_ticket_prm()
{
    m_prm_ticket.clear();
}
int ParamMgr::query_ticket_prm(uint8_t * p_ticket_type, TICKET_PARAM& prm)
{
	int ret = ERR_CARD_TYPE;
	if (!prm.has_query)
	{
		memset(&prm, 0, sizeof(prm));
		uint16_t ticket_type = (p_ticket_type[0] << 8) + p_ticket_type[1];

		for (uint32_t i=0; i<m_prm_ticket.size(); i++)
		{
			if (m_prm_ticket[i].ticket_type == ticket_type)
			{
				prm = m_prm_ticket[i];
				prm.has_query = true;
				return 0;
			}
		}
	}
	else
	{
		ret = 0;
	}

    return ret;
}
uint16_t ParamMgr::device_support_ticket(ETPDVC target_device_type, char * p_ticket_usable_device)
{
    if (p_ticket_usable_device[target_device_type - 2] == '1')
        return 0;

    return ERR_DEVICE_SUPPROT;
}
uint16_t ParamMgr::device_support_charge(ETPDVC target_device_type, char * p_ticket_charge_device)
{
	if (p_ticket_charge_device[target_device_type - 2] == '1')
		return 0;

	return ERR_DEVICE_SUPPROT;
}

uint16_t ParamMgr::device_support_sale(ETPDVC target_device_type, char * p_ticket_sell_device)
{
	if (p_ticket_sell_device[target_device_type - 2] == '1')
		return 0;

	return ERR_DEVICE_SUPPROT;
}
// 是否可发售，发售设备全部为0时不允许发售
bool ParamMgr::permit_sale(char * p_ticket_sell_device)
{
	char sz_temp[16];

	memset(sz_temp, '0', sizeof(sz_temp));

	return (memcmp(sz_temp, p_ticket_sell_device, 16) != 0);
}

// 载入票价参数
int ParamMgr::load_fare_prm(const char * p_prm_path, bool only_check/* = false*/)
{
    char sz_read[128]	= {0};
	int ret				= 0;
	size_t read_cnt		= 0;
    uint16_t exit_station;	// 为BCD形式
    FARE_ZONE frZone;
	FARE_ZONE cffrZone;
    FARE_CONFIG_TABLE frConfig;
    FARE_TABLE frTable;
    HOLIDAY_TABLE Holiday;
    IDLE_TIME_TABLE iTime;
    QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		if (!only_check)
		{
			unload_fare_prm();
		}

		while (fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			if (prm_section_match(sz_read, 0x0401))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
					exit_station = Publics::string_to_bcd<uint16_t>(sz_read + 9, 4);
					if (exit_station == Api::current_station_id)
					{
						frZone.station_id = Publics::string_to_bcd<uint16_t>(sz_read + 5, 4);
						frZone.exit_station_id = Publics::string_to_bcd<uint16_t>(sz_read + 9, 4);
						frZone.fare_zone = Publics::string_to_val<uint8_t>(sz_read + 13, 2);
						frZone.ride_time_limit = Publics::string_to_val<uint16_t>(sz_read + 15, 5);
						frZone.penalty_for_timeout = Publics::string_to_val<uint16_t>(sz_read + 20, 4);

						m_prm_fare.vec_zone.push_back(frZone);
					}

					////////////////////////
					//加载磁浮票价区段//////
					////////////////////////
					if((sz_read[5] == '6') &&(sz_read[9] == '6') )
					{
						cffrZone.station_id = Publics::string_to_bcd<uint16_t>(sz_read + 5, 4);
						cffrZone.exit_station_id = Publics::string_to_bcd<uint16_t>(sz_read + 9, 4);
						cffrZone.fare_zone = Publics::string_to_val<uint8_t>(sz_read + 13, 2);
						cffrZone.ride_time_limit = Publics::string_to_val<uint16_t>(sz_read + 15, 5);
						cffrZone.penalty_for_timeout = Publics::string_to_val<uint16_t>(sz_read + 20, 4);

						m_prm_fare.vec_cf_zone.push_back(cffrZone);
					}

				}
			}
			else if (prm_section_match(sz_read, 0x0402))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
					frConfig.ticket_type = Publics::string_to_bcd<uint16_t>(sz_read + 5, 4);
					frConfig.ride_time_code = sz_read[9] - '0';
					frConfig.fare_table_id = Publics::string_to_val<uint16_t>(sz_read + 10, 3);

					m_prm_fare.vec_fare_config.push_back(frConfig);
				}
			}
			else if (prm_section_match(sz_read, 0x0403))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
					frTable.fare_zone = Publics::string_to_val<uint8_t>(sz_read + 8, 2);
					frTable.fare_table_id = Publics::string_to_val<uint16_t>(sz_read + 5, 3);
					frTable.fare = Publics::string_to_val<long>(sz_read + 10, 10);

					m_prm_fare.vec_fare_table.push_back(frTable);
				}
			}
			else if (prm_section_match(sz_read, 0x0404))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
					Holiday.holiday_code = sz_read[5] - '0';
					Publics::string_to_bcds(sz_read + 6, 8, Holiday.start_date, sizeof(Holiday.start_date));
					Publics::string_to_bcds(sz_read + 14, 8, Holiday.end_date, sizeof(Holiday.end_date));

					m_prm_fare.vec_holiday_table.push_back(Holiday);
				}
			}
			else if (prm_section_match(sz_read, 0x0405))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
					iTime.weekday = sz_read[5] - '0';

					Publics::string_to_bcds(sz_read + 6, 4, iTime.start_time, sizeof(iTime.start_time));
					Publics::string_to_bcds(sz_read + 10, 4, iTime.end_time, sizeof(iTime.end_time));

					m_prm_fare.vec_idle_time.push_back(iTime);
				}
			}
		}

		fp.close();

	} while (0);

    return ret;
}
void ParamMgr::unload_fare_prm()
{
    m_prm_fare.vec_zone.clear();
    m_prm_fare.vec_fare_config.clear();
    m_prm_fare.vec_fare_table.clear();
    m_prm_fare.vec_holiday_table.clear();
    m_prm_fare.vec_idle_time.clear();
}

// 载入降级模式参数
int ParamMgr::load_degrade_mode(const char * p_prm_path, bool only_check/* = false*/)
{
#define DEGRADE_END_DEFAULT		"\x20\x99\x12\x31\x23\x59\x59"

    char sz_read[128]		= {0};
	int ret					= 0;
	size_t read_cnt			= 0;
    DEGRADE_MODE mode;
    QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		if (!only_check)
		{
			unload_degrade_mode();
		}

		while (fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			if (prm_section_match(sz_read, PRM_0501_DEGRADE))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
					mode.degrade_type = (DGTYPE)Publics::string_to_val<uint16_t>(sz_read + 9, 3);
					mode.degrade_station = Publics::string_to_bcd<uint16_t>(sz_read + 5, 4);
					Publics::string_to_bcds(sz_read + 12, 14, mode.degrade_start_time, sizeof(mode.degrade_start_time));

					if (Publics::every_equal<char>(0x30, sz_read + 26, 14) || Publics::every_equal<char>(0x20, sz_read + 26, 14))
						memcpy(mode.degrade_end_time, DEGRADE_END_DEFAULT, 7);
					else
						Publics::string_to_bcds(sz_read + 26, 14, mode.degrade_end_time, sizeof(mode.degrade_end_time));


					memcpy(mode.degrade_susceptive, mode.degrade_end_time, sizeof(mode.degrade_susceptive));
					if (mode.degrade_type == degrade_Trouble || mode.degrade_type == degrade_Green_Light)
					{
						if (memcmp(mode.degrade_susceptive, DEGRADE_END_DEFAULT, 7) != 0)
						{
							TimesEx::bcd_time_calculate(mode.degrade_susceptive, T_DATE, m_prm_device_ctrl.degrade_effect_time);
						}
					}

					m_prm_degrade.push_back(mode);
				}
			}
		}
		fp.close();

	} while (0);

    return ret;
}
void ParamMgr::unload_degrade_mode()
{
    m_prm_degrade.clear();
}

// 载入乘此票专用参数
int ParamMgr::load_tct_config(const char * p_prm_path, bool only_check/* = false*/)
{
    char sz_read[128]	= {0};
	int ret				= 0;
	size_t read_cnt		= 0;
    TCT_PARAM tct_prm;
    QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		if (!only_check)
		{
			unload_tct_config();
		}

		while (fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			if (prm_section_match(sz_read, PRM_0303_TCT))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
					tct_prm.ticket_type = Publics::string_to_bcd<uint16_t>(sz_read + 5, 4);
					tct_prm.once_charge_value = Publics::string_to_val<long>(sz_read + 9, 6);
					tct_prm.once_charge_count = Publics::string_to_val<long>(sz_read + 15, 4);
					tct_prm.charge_start_next_month = Publics::string_to_val<uint8_t>(sz_read + 19, 2);
					tct_prm.charge_end_next_month = Publics::string_to_val<uint8_t>(sz_read + 21, 2);
					tct_prm.exit_timeout_penalty_count = Publics::string_to_val<long>(sz_read + 23, 2);
					tct_prm.exit_timeout_penalty_value = Publics::string_to_val<long>(sz_read + 25, 4);
					tct_prm.entry_timeout_penalty_count = Publics::string_to_val<long>(sz_read + 29, 2);
					tct_prm.entry_timeout_penalty_value = Publics::string_to_val<long>(sz_read + 31, 4);

					m_prm_tct.push_back(tct_prm);
				}
			}
		}
		fp.close();

	} while (0);

	return ret;
}
void ParamMgr::unload_tct_config()
{
    m_prm_tct.clear();
}
int ParamMgr::query_tct_prm(uint8_t * p_ticket_type, TCT_PARAM& prm)
{
    uint16_t ticket_type = (p_ticket_type[0] << 8) + p_ticket_type[1];

    for (uint32_t i=0; i<m_prm_tct.size(); i++)
    {
        if (ticket_type == m_prm_tct[i].ticket_type)
        {
            prm = m_prm_tct[i];
            return 0;
        }
    }

    return ERR_CARD_TYPE;
}

// 载入专用通道参数
int ParamMgr::load_special_passageway(const char * p_prm_path, bool only_check/* = false*/)
{
    char sz_read[128]	= {0};
	int ret				= 0;
	size_t read_cnt		= 0;
    uint16_t ticket_type;
    bool discount_flag;
    QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		if (!only_check)
		{
			unload_special_passageway();
		}

		while (fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			if (prm_section_match(sz_read, PRM_0302_ALLEY_WAY))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
					ticket_type = Publics::string_to_bcd<uint16_t>(sz_read + 5, 4);
					discount_flag = Publics::char_to_bool(sz_read[9]);

					m_prm_passageway.insert(make_pair(ticket_type, discount_flag));
				}
			}
		}
		fp.close();

	} while (0);

    return ret;
}
void ParamMgr::unload_special_passageway()
{
    m_prm_passageway.clear();
}
// 查询闸机通道参数
uint16_t ParamMgr::passageway_allow_pass(uint8_t * p_ticket_type, uint8_t cur_door_type)
{
	uint16_t ticket_type = (p_ticket_type[0] << 8) | p_ticket_type[1];
	bool discount_type = false;

	map<uint16_t, bool>::iterator itor = m_prm_passageway.find(ticket_type);
	if (itor != m_prm_passageway.end())
		discount_type = itor->second;

	if (discount_type && cur_door_type == 1)
		return ERR_PASSAGEWAY_1;
	else if (!discount_type && cur_door_type == 2)
		return ERR_PASSAGEWAY_2;

	return 0;
}

// 地铁黑名单参数
int ParamMgr::load_mtr_single_black(const char * p_prm_path, bool only_check/* = false*/)
{
#define LEN_OBLACK_LINE	(5 + 20 + 3 + 2)

    char sz_read[128]	= {0};
	int ret				= 0;
	size_t read_cnt		= 0;
	int file_size		= 0;
    QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		file_size = (int)(fp.length() - 12);
		if (file_size % LEN_OBLACK_LINE != 0)
		{
			ret = ERR_PARAM_INVALID;
			break;
		}

		if (!only_check)
		{
			unload_mtr_single_black();

			m_prm_mtr_black.blacks_single.logical_id = new(std::nothrow) char*[file_size / LEN_OBLACK_LINE];
			if (m_prm_mtr_black.blacks_single.logical_id == NULL)
			{
				ret = ERR_UNDEFINED;
				break;
			}
		}

		while(fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			if (prm_section_match(sz_read, PRM_0601_SINGLE_BLACK_M))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
					m_prm_mtr_black.blacks_single.logical_id[m_prm_mtr_black.blacks_single.record_count] = new(std::nothrow) char[20];
					if (m_prm_mtr_black.blacks_single.logical_id[m_prm_mtr_black.blacks_single.record_count] == NULL)
					{
						ret = ERR_UNDEFINED;
						break;
					}
					memcpy(m_prm_mtr_black.blacks_single.logical_id[m_prm_mtr_black.blacks_single.record_count], sz_read + 5, 20);
					m_prm_mtr_black.blacks_single.record_count ++;
				}
			}
		}

		fp.close();

	} while (0);

	return ret;
}
void ParamMgr::unload_mtr_single_black()
{
    if (m_prm_mtr_black.blacks_single.logical_id != NULL)
    {
        for (int i=0; i<m_prm_mtr_black.blacks_single.record_count; i++)
        {
            if (m_prm_mtr_black.blacks_single.logical_id[i] != NULL)
            {
                delete []m_prm_mtr_black.blacks_single.logical_id[i];
                m_prm_mtr_black.blacks_single.logical_id[i] = NULL;
            }
        }

        delete []m_prm_mtr_black.blacks_single.logical_id;
        m_prm_mtr_black.blacks_single.logical_id = NULL;
    }

    m_prm_mtr_black.blacks_single.record_count = 0;
}
int ParamMgr::load_mtr_section_black(const char * p_prm_path, bool only_check/* = false*/)
{
    char sz_read[128]		= {0};
    char sz_start_id[21]	= {0};
    char sz_end_id[21]		= {0};
	int ret					= 0;
	size_t read_cnt			= 0;
    QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		if (!only_check)
		{
			unload_mtr_section_black();
		}

		while(fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			if (prm_section_match(sz_read, PRM_0602_SECTION_BLACK_M))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_FILE_ACCESS;
					break;
				}

				if (!only_check)
				{
					memcpy(sz_start_id, sz_read + 5, 20);
					memcpy(sz_end_id, sz_read + 25, 20);
					m_prm_mtr_black.blacks_section.push_back(make_pair(string(sz_start_id), string(sz_end_id)));
				}
			}
		}
		fp.close();

	} while (0);

    return ret;
}
void ParamMgr::unload_mtr_section_black()
{
    m_prm_mtr_black.blacks_section.clear();
}


bool ParamMgr::query_black_mtr(char * p_mtr_card_logical_id)
{
    if (method_of_bisection(p_mtr_card_logical_id, 20, m_prm_mtr_black.blacks_single.logical_id, m_prm_mtr_black.blacks_single.record_count))
        return true;

    for (uint32_t i=0; i<m_prm_mtr_black.blacks_section.size(); i++)
    {
        if (memcmp(m_prm_mtr_black.blacks_section[i].first.c_str(), p_mtr_card_logical_id, 20) <= 0 &&
                memcmp(m_prm_mtr_black.blacks_section[i].second.c_str(), p_mtr_card_logical_id, 20) >= 0)
            return true;
    }

    return false;
}



// 交通部黑名单参数
int ParamMgr::load_jtb_single_black(const char * p_prm_path, bool only_check/* = false*/)
{
#define LEN_OBLACK_LINE	(5 + 20 + 3 + 2)

    char sz_read[128]	= {0};
	int ret				= 0;
	size_t read_cnt		= 0;
	int file_size		= 0;
    QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		file_size = (int)(fp.length() - 12);
		if (file_size % LEN_OBLACK_LINE != 0)
		{
			ret = ERR_PARAM_INVALID;
			break;
		}

		if (!only_check)
		{
			unload_jtb_single_black();

			m_prm_jtb_black.blacks_single.logical_id = new(std::nothrow) char*[file_size / LEN_OBLACK_LINE];
			if (m_prm_jtb_black.blacks_single.logical_id == NULL)
			{
				ret = ERR_UNDEFINED;
				break;
			}
		}

		while(fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			if (prm_section_match(sz_read, PRM_0605_JTB_SINGLE_BLACK_O))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
					m_prm_jtb_black.blacks_single.logical_id[m_prm_jtb_black.blacks_single.record_count] = new(std::nothrow) char[20];
					if (m_prm_jtb_black.blacks_single.logical_id[m_prm_jtb_black.blacks_single.record_count] == NULL)
					{
						ret = ERR_UNDEFINED;
						break;
					}
					memcpy(m_prm_jtb_black.blacks_single.logical_id[m_prm_jtb_black.blacks_single.record_count], sz_read + 5, 20);
					m_prm_jtb_black.blacks_single.record_count ++;
				}
			}
		}

		fp.close();

	} while (0);

	return ret;
}
void ParamMgr::unload_jtb_single_black()
{
    if (m_prm_jtb_black.blacks_single.logical_id != NULL)
    {
        for (int i=0; i<m_prm_jtb_black.blacks_single.record_count; i++)
        {
            if (m_prm_jtb_black.blacks_single.logical_id[i] != NULL)
            {
                delete []m_prm_jtb_black.blacks_single.logical_id[i];
                m_prm_jtb_black.blacks_single.logical_id[i] = NULL;
            }
        }

        delete []m_prm_jtb_black.blacks_single.logical_id;
        m_prm_jtb_black.blacks_single.logical_id = NULL;
    }

    m_prm_jtb_black.blacks_single.record_count = 0;
}





bool ParamMgr::query_black_jtb(char * p_jtb_card_logical_id)
{
    if (method_of_bisection(p_jtb_card_logical_id, 19, m_prm_jtb_black.blacks_single.logical_id, m_prm_jtb_black.blacks_single.record_count))
        return true;

    for (uint32_t i=0; i<m_prm_jtb_black.blacks_section.size(); i++)
    {
        if (memcmp(m_prm_jtb_black.blacks_section[i].first.c_str(), p_jtb_card_logical_id, 19) <= 0 &&
                memcmp(m_prm_jtb_black.blacks_section[i].second.c_str(), p_jtb_card_logical_id, 19) >= 0)
            return true;
    }

    return false;
}



// 交通部白名单参数
int ParamMgr::load_jtb_white(const char * p_prm_path, bool only_check/* = false*/)
{
#define LEN_OWHITE_LINE	(5 + 11 + 10 + 2)

    char sz_read[128]	= {0};
	int ret				= 0;
	size_t read_cnt		= 0;
	int file_size		= 0;
    QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		file_size = (int)(fp.length() - 12);
		if (file_size % LEN_OWHITE_LINE != 0)
		{
			ret = ERR_PARAM_INVALID;
			break;
		}

		if (!only_check)
		{
			unload_jtb_white();

			m_prm_jtb_white.whites_single.issue_code = new(std::nothrow) char*[file_size / LEN_OWHITE_LINE];
			if (m_prm_jtb_white.whites_single.issue_code == NULL)
			{
				ret = ERR_UNDEFINED;
				break;
			}
		}

		while(fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			if (prm_section_match(sz_read, PRM_0606_JTB_WHITE_O))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
					m_prm_jtb_white.whites_single.issue_code[m_prm_jtb_white.whites_single.record_count] = new(std::nothrow) char[11];
					if (m_prm_jtb_white.whites_single.issue_code[m_prm_jtb_white.whites_single.record_count] == NULL)
					{
						ret = ERR_UNDEFINED;
						break;
					}
					memcpy(m_prm_jtb_white.whites_single.issue_code[m_prm_jtb_white.whites_single.record_count], sz_read + 5, 11);
					m_prm_jtb_white.whites_single.record_count ++;
				}
			}
		}

		fp.close();

	} while (0);

	return ret;
}
void ParamMgr::unload_jtb_white()
{
    if (m_prm_jtb_white.whites_single.issue_code != NULL)
    {
        for (int i=0; i<m_prm_jtb_white.whites_single.record_count; i++)
        {
            if (m_prm_jtb_white.whites_single.issue_code[i] != NULL)
            {
                delete []m_prm_jtb_white.whites_single.issue_code[i];
                m_prm_jtb_white.whites_single.issue_code[i] = NULL;
            }
        }

        delete []m_prm_jtb_white.whites_single.issue_code;
        m_prm_jtb_white.whites_single.issue_code = NULL;
    }

    m_prm_jtb_white.whites_single.record_count = 0;
}





bool ParamMgr::query_white_jtb(char * p_jtb_card_issue_code)
{
    if (method_of_bisection(p_jtb_card_issue_code, 8, m_prm_jtb_white.whites_single.issue_code, m_prm_jtb_white.whites_single.record_count))
        return true;

    for (uint32_t i=0; i<m_prm_jtb_white.whites_section.size(); i++)
    {
        if (memcmp(m_prm_jtb_white.whites_section[i].first.c_str(), p_jtb_card_issue_code, 8) <= 0 &&
                memcmp(m_prm_jtb_white.whites_section[i].second.c_str(), p_jtb_card_issue_code, 8) >= 0)
            return true;
    }

    return false;
}

bool ParamMgr::is_interconnect(uint8_t * interCardFlag, uint8_t * citycode)
{
	uint16_t	city_id;
	city_id = (uint16_t)((citycode[0] << 8) + citycode[1]);

	//dbg_formatvar("city_id=%04X",city_id);

	//dbg_formatvar("Api::current_city_id=%04X",Api::current_city_id);

	if(city_id != Api::current_city_id)//异地票卡需要判断是否互联互通
	{
		if(memcmp(interCardFlag,"\x00\x01",2) == 0)
			return true;
	}
	else
		return true;

    return false;
}

// OCT黑名单参数
int ParamMgr::load_oct_single_black(const char * p_prm_path, bool only_check/* = false*/)
{
#define LEN_OBLACK_LINE	(5 + 20 + 3 + 2)

    char sz_read[128]	= {0};
	int ret				= 0;
	size_t read_cnt		= 0;
	int file_size		= 0;
    QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		file_size = (int)(fp.length() - 12);
		if (file_size % LEN_OBLACK_LINE != 0)
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		if (!only_check)
		{
			unload_oct_single_black();

			m_prm_oct_black.blacks_single.logical_id = new(std::nothrow) char*[file_size / LEN_OBLACK_LINE];
			if (m_prm_oct_black.blacks_single.logical_id == NULL)
			{
				ret = ERR_UNDEFINED;
				break;
			}
		}

		while(fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			if (prm_section_match(sz_read, PRM_0603_SINGLE_BLACK_O))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_FILE_ACCESS;
					break;
				}

				if (!only_check)
				{
					m_prm_oct_black.blacks_single.logical_id[m_prm_oct_black.blacks_single.record_count] = new(std::nothrow) char[20];
					if (m_prm_oct_black.blacks_single.logical_id[m_prm_oct_black.blacks_single.record_count] == NULL)
					{
						ret = ERR_UNDEFINED;
						break;
					}
					memcpy(m_prm_oct_black.blacks_single.logical_id[m_prm_oct_black.blacks_single.record_count], sz_read + 5, 20);
					m_prm_oct_black.blacks_single.record_count ++;
				}
			}
		}

		fp.close();

	} while (0);

    return ret;
}
void ParamMgr::unload_oct_single_black()
{
    if (m_prm_oct_black.blacks_single.logical_id != NULL)
    {
        for (int i=0; i<m_prm_oct_black.blacks_single.record_count; i++)
        {
            if (m_prm_oct_black.blacks_single.logical_id[i] != NULL)
            {
                delete []m_prm_oct_black.blacks_single.logical_id[i];
                m_prm_oct_black.blacks_single.logical_id[i] = NULL;
            }
        }

        delete []m_prm_oct_black.blacks_single.logical_id;
        m_prm_oct_black.blacks_single.logical_id = NULL;
    }

    m_prm_oct_black.blacks_single.record_count = 0;
}
int ParamMgr::load_oct_section_black(const char * p_prm_path, bool only_check/* = false*/)
{
    char sz_read[128]		= {0};
    char sz_start_id[21]	= {0};
    char sz_end_id[21]		= {0};
	int ret					= 0;
	size_t read_cnt			= 0;
    QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		if (!only_check)
		{
			unload_oct_section_black();
		}

		while(fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			if (prm_section_match(sz_read, PRM_0604_SECTION_BLACK_O))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_FILE_ACCESS;
					break;
				}

				if (!only_check)
				{
					memcpy(sz_start_id, sz_read + 5, 20);
					memcpy(sz_end_id, sz_read + 25, 20);
					m_prm_oct_black.blacks_section.push_back(make_pair(string(sz_start_id), string(sz_end_id)));
				}
			}
		}
		fp.close();

	} while (0);

	return ret;
}
void ParamMgr::unload_oct_section_black()
{
    m_prm_oct_black.blacks_section.clear();
}

bool ParamMgr::query_black_oct(char * p_oct_card_logical_id)
{
    if (method_of_bisection(p_oct_card_logical_id, 20, m_prm_oct_black.blacks_single.logical_id, m_prm_oct_black.blacks_single.record_count))
        return true;

    for (uint32_t i=0; i<m_prm_oct_black.blacks_section.size(); i++)
    {
        if (memcmp(m_prm_oct_black.blacks_section[i].first.c_str(), p_oct_card_logical_id, 20) <= 0 &&
                memcmp(m_prm_oct_black.blacks_section[i].second.c_str(), p_oct_card_logical_id, 20) >= 0)
            return true;
    }

    return false;
}

// 行政罚金参数
int ParamMgr::load_policy_penalty(const char * p_prm_path, bool only_check/* = false*/)
{
    char sz_read[128]			= {0};
    //char sz_current_station[5]	= {0};
	int ret						= 0;
	size_t read_cnt				= 0;
    POLICY_PENALTY policy_penalty;
    QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
		}

		if (!only_check)
		{
			unload_policy_penalty();
		}

		while (fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			if (prm_section_match(sz_read, PRM_0801_POLICE_FINE))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
					// 只加载本站,本线路和全线网的
					policy_penalty.line_station = Publics::string_to_bcd<uint16_t>(sz_read + 11, 4);

					if (policy_penalty.line_station == Api::current_station_id || policy_penalty.line_station == 0x0000 ||
						((policy_penalty.line_station & 0xFF00) == (Api::current_station_id & 0xFF00) && (policy_penalty.line_station & 0xFF) == 0x00))
					{
						policy_penalty.policy_code	= Publics::string_to_val<uint8_t>(sz_read + 5, 2);
						policy_penalty.penalty = Publics::string_to_val<uint16_t>(sz_read + 7, 4);
						policy_penalty.ticket_type = Publics::string_to_bcd<uint16_t>(sz_read + 15, 4);

						m_prm_policy_penaty.push_back(policy_penalty);
					}
				}
			}
		}
		fp.close();

	} while (0);

	return ret;
}
void ParamMgr::unload_policy_penalty()
{
    m_prm_policy_penaty.clear();
}

// 是否出站超时
bool ParamMgr::exit_timeout(uint16_t entry_station, uint8_t *  p_time_entry, uint8_t *  p_time_exit, long * p_penalty/* = NULL*/)
{
    uint16_t timeout_in_minutes;
    for (uint16_t i=0; i<m_prm_fare.vec_zone.size(); i++)
    {
        if (m_prm_fare.vec_zone[i].station_id == entry_station)
        {
            timeout_in_minutes = m_prm_fare.vec_zone[i].ride_time_limit;

            if (TimesEx::timeout(p_time_exit, p_time_entry, timeout_in_minutes))
            {
                if (p_penalty != NULL)
                    * p_penalty = m_prm_fare.vec_zone[i].penalty_for_timeout;
                return true;
            }
        }
    }
    return false;
}


// 获取有效出站时间
bool ParamMgr::get_validexittime(uint16_t entry_station, uint8_t *  p_time_entry, uint8_t *  p_time_exit, long * p_penalty/* = NULL*/)
{
    uint16_t timeout_in_minutes;
    for (uint16_t i=0; i<m_prm_fare.vec_zone.size(); i++)
    {
        if (m_prm_fare.vec_zone[i].station_id == entry_station)
        {
            timeout_in_minutes = m_prm_fare.vec_zone[i].ride_time_limit;

			memcpy(p_time_exit, p_time_entry, 7);
			TimesEx::bcd_time_calculate(p_time_exit, T_TIME, 0, 0, timeout_in_minutes);

            return true;
        }
    }
    return false;
}

// 查询票价,lTrade为交易的实际金额，lFare为原票价（单程票票价）
uint16_t ParamMgr::query_fare(uint8_t * p_time_now, uint8_t * p_ticket_type,
			   uint16_t entry_station, uint8_t * p_time_entry, long * p_fare_actual, long * p_fare_init/* = NULL*/)
{
    uint16_t ret			= 0;
    uint8_t fare_zone		= 0;		// 区段代码
    uint8_t time_code		= 0;		// 时间代码
    uint16_t fare_table_id	= 0;		// 票价表ID
    uint16_t ticket_type	= 0;

	do
    {

		//dbg_formatvar("p_ticket_type:%02x%02x",p_ticket_type[0],p_ticket_type[1]);

        ticket_type = (p_ticket_type[0] << 8) + p_ticket_type[1];

        // 查找收费区段
        ret = query_fare_zone(entry_station, fare_zone);
        if (ret != 0)	break;

		//dbg_formatvar("fare_zone:%d",fare_zone);

        // 获取时间代码
        time_code = query_time_code(p_time_now, ticket_type, p_time_entry);

		//dbg_formatvar("time_code:%d",time_code);

        // 查找收费配置表
        ret = query_fare_config(p_time_now, ticket_type, time_code, fare_table_id);
        if (ret != 0)	break;

		//dbg_formatvar("fare_table_id:%d",fare_table_id);

        // 查找票价表
        ret = query_fare(p_time_now, fare_zone, fare_table_id, p_fare_actual);
        if (ret != 0)	break;

		//dbg_formatvar("p_fare_actual:%d",&p_fare_actual);
		//dbg_formatvar("p_fare_actual:%d",*p_fare_actual);

        if (p_fare_init == NULL)	break;

        // 查找单程票收费配置表
        ret = query_fare_config(p_time_now, 0x0100, time_code, fare_table_id);
        if (ret != 0)	break;

        // 查找单程票票价表
        ret = query_fare(p_time_now, fare_zone, fare_table_id, p_fare_init);

    }
    while (0);

    return ret;
}


// 查磁浮最低票价
int ParamMgr::query_cf_lowest_fare(uint8_t * p_time_now, uint8_t * p_ticket_type,uint16_t entry_station,long& fare_lowest_actual, long * p_fare_lowest_ini)
{
    int ret					= 0;
    uint8_t fare_zone		= 0;		// 区段代码
    uint8_t time_code		= 0;		// 时间代码
    uint16_t fare_table_id	= 0;		// 票价表ID
    uint16_t ticket_type	= 0;

    do
    {
        fare_lowest_actual = 200;

        ticket_type = (p_ticket_type[0] << 8) + p_ticket_type[1];

        // 直接赋值最小区段
        //fare_zone = 1;
		// 查找收费区段
        ret = query_min_fare_zone(entry_station, fare_zone);
        //if (ret != 0)	break;

        // 获取时间代码
        time_code = 1;

        // 查找收费配置表
        ret = query_fare_config(p_time_now, ticket_type, time_code, fare_table_id);
        if (ret != 0)	break;

        // 查找票价表
        ret = query_fare(p_time_now, fare_zone, fare_table_id, &fare_lowest_actual);
        if (ret != 0)	break;

        if (p_fare_lowest_ini == NULL)	break;

        // 查找单程票收费配置表
        ret = query_fare_config(p_time_now, 0x0100, time_code, fare_table_id);
        if (ret != 0)	break;

        // 查找单程票票价表
        ret = query_fare(p_time_now, fare_zone, fare_table_id, p_fare_lowest_ini);

    } while (0);

    return ret;
}

// 查最低票价
int ParamMgr::query_lowest_fare(uint8_t * p_time_now, uint8_t * p_ticket_type, long& fare_lowest_actual, long * p_fare_lowest_ini)
{
    int ret					= 0;
    uint8_t fare_zone		= 0;		// 区段代码
    uint8_t time_code		= 0;		// 时间代码
    uint16_t fare_table_id	= 0;		// 票价表ID
    uint16_t ticket_type	= 0;

    do
    {
        fare_lowest_actual = 200;

        ticket_type = (p_ticket_type[0] << 8) + p_ticket_type[1];

        // 直接赋值最小区段
        fare_zone = 1;
		// 查找收费区段
        //ret = query_min_fare_zone(tag_station, fare_zone);
        //f (ret != 0)	break;

        // 获取时间代码
        time_code = 1;

        // 查找收费配置表
        ret = query_fare_config(p_time_now, ticket_type, time_code, fare_table_id);
        if (ret != 0)	break;

        // 查找票价表
        ret = query_fare(p_time_now, fare_zone, fare_table_id, &fare_lowest_actual);
        if (ret != 0)	break;

        if (p_fare_lowest_ini == NULL)	break;

        // 查找单程票收费配置表
        ret = query_fare_config(p_time_now, 0x0100, time_code, fare_table_id);
        if (ret != 0)	break;

        // 查找单程票票价表
        ret = query_fare(p_time_now, fare_zone, fare_table_id, p_fare_lowest_ini);

    } while (0);

    return ret;
}

// 查询线路最低票价
int ParamMgr::query_line_lowest_fare(uint8_t * p_time_now, uint16_t entry_station,uint8_t * p_ticket_type, long& fare_lowest_actual, long * p_fare_lowest_ini)
{
    int ret					= 0;
    uint8_t fare_zone		= 0;		// 区段代码
    uint8_t time_code		= 0;		// 时间代码
    uint16_t fare_table_id	= 0;		// 票价表ID
    uint16_t ticket_type	= 0;

    do
    {
        fare_lowest_actual = 200;

        ticket_type = (p_ticket_type[0] << 8) + p_ticket_type[1];

        // 直接赋值最小区段
        fare_zone = 1;

        // 获取时间代码
        time_code = 1;

        // 查找收费配置表
        ret = query_fare_config(p_time_now, ticket_type, time_code, fare_table_id);
        if (ret != 0)	break;

        // 查找票价表
        ret = query_fare(p_time_now, fare_zone, fare_table_id, &fare_lowest_actual);
        if (ret != 0)	break;

        if (p_fare_lowest_ini == NULL)	break;

        // 查找单程票收费配置表
        ret = query_fare_config(p_time_now, 0x0100, time_code, fare_table_id);
        if (ret != 0)	break;

        // 查找单程票票价表
        ret = query_fare(p_time_now, fare_zone, fare_table_id, p_fare_lowest_ini);

    } while (0);

    return ret;
}


//查询到本站最高票价
int ParamMgr::query_highest_fare(uint8_t * p_time_now, long * p_fare_highest_ini)
{
	for (uint16_t i=0; i<m_prm_fare.vec_zone.size(); i++)
	{
		//dbg_formatvar("i=%d",i);
		//dbg_formatvar("Api::current_station_id=%d",Api::current_station_id);
		//dbg_formatvar("m_prm_fare.vec_zone[i].station_id=%d",m_prm_fare.vec_zone[i].station_id);
		if (m_prm_fare.vec_zone[i].station_id == Api::current_station_id)
		{
			//dbg_formatvar("ii=%d",i);
			//dbg_formatvar("m_prm_fare.vec_zone[i].ride_time_limit=%d",m_prm_fare.vec_zone[i].ride_time_limit);
			* p_fare_highest_ini = m_prm_fare.vec_zone[i].penalty_for_timeout;
			//dbg_formatvar("p_fare_highest_ini=%ld",*p_fare_highest_ini);
			break;
		}

	}

	return 0;
}

// 根据票价查找收费区段
uint16_t ParamMgr::fare_zone_by_sjt_fare(uint8_t * p_ticket_type, long fare, uint8_t& fare_zone)
{
	uint16_t ret			= ERR_INPUT_PARAM;
	uint16_t fare_table_id	= 0;		// 票价表ID
	uint16_t ticket_type	= 0;
	FARE_TABLE fare_table;

	do
	{
		ticket_type = (p_ticket_type[0] << 8) + p_ticket_type[1];

		// 通过票卡类型和乘车时间代码查找票价表ID，时间代码默认为工作日
		ret = query_fare_config(NULL, ticket_type, 0, fare_table_id);
		if (ret != 0)	break;

		// 根据收费和票价表ID反查区段
		for (uint32_t i=0; i<m_prm_fare.vec_fare_table.size(); i++)
		{
			fare_table = m_prm_fare.vec_fare_table.at(i);
			if (fare_table.fare == fare && fare_table.fare_table_id == fare_table_id)
			{
				fare_zone = fare_table.fare_zone;
				ret = 0;
				break;
			}
		}

	} while (0);

	return ret;
}

// 是否相同车站
bool ParamMgr::same_station(uint16_t station_id0, uint16_t station_id1)
{
    for (uint32_t i=0; i<m_prm_changing_station.size(); i++)
    {
        if ((m_prm_changing_station[i].first == station_id0 && m_prm_changing_station[i].second == station_id1) ||
                (m_prm_changing_station[i].first == station_id1 && m_prm_changing_station[i].second == station_id0))
            return true;
    }

    return (station_id0 == station_id1);
}

// 查找收费区段
uint16_t ParamMgr::query_fare_zone(uint16_t entry_station_id, uint8_t& fare_zone)
{
    for (uint32_t i=0; i<m_prm_fare.vec_zone.size(); i++)
    {
        if ((m_prm_fare.vec_zone[i].station_id == entry_station_id))
        {
            fare_zone = m_prm_fare.vec_zone[i].fare_zone;
            return 0;
        }
    }

    return ERR_STATION_INVALID;
}


// 查找收费区段 仅通过进站站点查询最低的收费区段
uint16_t ParamMgr::query_min_fare_zone(uint16_t entry_station_id, uint8_t& fare_zone)
{
	uint16_t ret = ERR_STATION_INVALID;

	uint32_t i;
	FARE_ZONE tmpZone;

	if ((entry_station_id & 0xFF00) == 0x6000)
	{
		for (i=0; i<m_prm_fare.vec_cf_zone.size(); i++)
		{
			tmpZone = m_prm_fare.vec_cf_zone[i];
			if ((entry_station_id & 0xFF00) == (tmpZone.station_id& 0XFF00) && tmpZone.station_id == tmpZone.exit_station_id)
			{
				fare_zone = m_prm_fare.vec_cf_zone[i].fare_zone;
				ret = 0;
				break;
			}
		}
	}
	else
	{
		for (i=0; i<m_prm_fare.vec_zone.size(); i++)
		{
			tmpZone = m_prm_fare.vec_zone[i];

			// 至少应与目标站点线路相同，否则导致在地铁进站的车票磁浮非付费区查最低票价不正确
			/*
			if ((entry_station_id & 0xFF00) == (tmpZone.station_id& 0XFF00) && tmpZone.station_id == tmpZone.exit_station_id)
			{
				fare_zone = m_prm_fare.vec_zone[i].fare_zone;
				ret = 0;
				break;
			}*/
			if (m_prm_fare.vec_zone[i].station_id == entry_station_id)
			{
				fare_zone = m_prm_fare.vec_zone[i].fare_zone;
				return 0;
			}
		}
		if (ret == ERR_STATION_INVALID)
		{
			if ((Api::current_station_id & 0xFF00) == 0x6000)
			{
				fare_zone = 1;
				ret = 0;
			}
		}
	}

	return ret;
}


// 查找最大收费区段
uint16_t ParamMgr::query_max_fare_zone(uint16_t entry_station_id,uint8_t& fare_zone)
{
	fare_zone = m_prm_fare.vec_zone[0].fare_zone;
	//dbg_formatvar("size=%d",m_prm_fare.vec_zone.size());
	//dbg_formatvar("fare_zone=%d",fare_zone);
	//dbg_formatvar("station_id=%s",m_prm_fare.vec_zone[0].station_id);
	for (uint32_t i=0; i<m_prm_fare.vec_zone.size()-1; i++)
	{
		if (m_prm_fare.vec_zone[i].station_id == entry_station_id)
		{
			if (m_prm_fare.vec_zone[i+1].fare_zone > m_prm_fare.vec_zone[i].fare_zone)
			{
				fare_zone = m_prm_fare.vec_zone[i+1].fare_zone;
			}
		}

	}

	return 0;
}


uint8_t ParamMgr::query_holiday_type(uint8_t * p_time_now, uint16_t ticket_type, uint8_t * p_time_entry)
{
	uint8_t holiday_type = 0;
	HOLIDAY_TABLE hly_tab;

	for (uint16_t i=0;i<m_prm_fare.vec_holiday_table.size();i++)
	{
		hly_tab = m_prm_fare.vec_holiday_table[i];

		if ((memcmp(p_time_entry, hly_tab.start_date, 4) >= 0 && memcmp(p_time_entry, hly_tab.end_date, 4) <= 0) ||
			(memcmp(p_time_now, hly_tab.start_date, 4) >= 0 && memcmp(p_time_now, hly_tab.end_date, 4) <= 0))
		{
			holiday_type = hly_tab.holiday_code;
			if (holiday_type == 1)				// 由于公共假期优先级高，所以搜到公共假期就直接退出，如果是学校假期还要看是否公共假期
				break;
		}
	}

	return holiday_type;
}
bool ParamMgr::student_ticket(uint16_t ticket_type)
{
	uint16_t student_ticket_array[] = {0x0201};
	for (uint8_t i=0;i<sizeof(student_ticket_array)/sizeof(student_ticket_array[0]);i++)
	{
		if (ticket_type == student_ticket_array[i])
			return true;
	}

	return false;
}
bool ParamMgr::time_in_idle(uint8_t weekday_today, uint8_t * p_time_src)
{
	IDLE_TIME_TABLE it_tab;

	for (uint8_t i=0;i<m_prm_fare.vec_idle_time.size();i++)
	{
		it_tab = m_prm_fare.vec_idle_time[i];
		if (weekday_today == it_tab.weekday && memcmp(p_time_src + 4, it_tab.start_time, 2) >= 0 && memcmp(p_time_src + 4, it_tab.end_time, 2) <= 0)
		{
			return true;
		}
	}

	return false;
}
uint8_t ParamMgr::query_time_code(uint8_t * p_time_now, uint16_t ticket_type, uint8_t * p_time_entry)
{
    uint8_t time_code		= 0;
	uint8_t holiday_type	= 0;
	uint8_t weekday_s		= 0;
	uint8_t weekday_e		= 0;

	do
	{
		// 在何种假期
		holiday_type = query_holiday_type(p_time_now, ticket_type, p_time_entry);

		if (holiday_type == 1)	// 公共假期
		{
			time_code = 4;
			break;
		}
		if (holiday_type == 2 && student_ticket(ticket_type))	// 学校假期
		{
			time_code = 5;
			break;
		}

		weekday_s = TimesEx::weekday_theday(p_time_entry);
		weekday_e = TimesEx::weekday_theday(p_time_now);

		// 在非繁忙时间
		if (time_in_idle(weekday_e, p_time_now) || time_in_idle(weekday_s, p_time_entry))
		{
			time_code = 1;
			break;
		}

		// 是否在周末
		if (weekday_e == 6)
		{
			time_code = 2;
			break;
		}

		if (weekday_e == 0)
		{
			time_code = 3;
			break;
		}

		if (weekday_s == 6)
		{
			time_code = 2;
			break;
		}

		if (weekday_s == 0)
		{
			time_code = 3;
			break;
		}

	} while (0);

    return time_code;
}
uint16_t ParamMgr::query_fare_config(uint8_t * p_time_now, uint16_t ticket_type, uint8_t time_code, uint16_t& fare_table_id)
{
    // 查找收费配置表
    for (uint32_t i=0; i<m_prm_fare.vec_fare_config.size(); i++)
    {
        if (m_prm_fare.vec_fare_config[i].ride_time_code == time_code && m_prm_fare.vec_fare_config[i].ticket_type == ticket_type)
        {
            fare_table_id = m_prm_fare.vec_fare_config[i].fare_table_id;
            return 0;
        }
    }

    return ERR_CARD_TYPE;
}

// 根据区段和票价表ID得到票价
uint16_t ParamMgr::query_fare(uint8_t * p_time_now, uint8_t fare_zone, uint16_t fare_table_id, long * p_fare)
{
    FARE_TABLE fare_table;
    for (uint32_t i=0; i<m_prm_fare.vec_fare_table.size(); i++)
    {
        fare_table = m_prm_fare.vec_fare_table.at(i);
        if (fare_table.fare_zone == fare_zone && fare_table.fare_table_id == fare_table_id)
        {
            *p_fare = fare_table.fare;
            return 0;
        }
    }

    return ERR_CARD_TYPE;
}

// 检查参数合法性
uint16_t ParamMgr::valid_prm(const char * p_prm_pathname)
{
    uint16_t ret = 0;

    do
    {
		if (access(p_prm_pathname, 0) != 0)
		{
			ret = ERR_PARAM_NOT_EXIST;
			g_Record.log_out(ret, level_error, "file %s can not access", p_prm_pathname);
			break;
		}

		if (!DataSecurity::crc32_in_file_end_valid(p_prm_pathname))
		{
			ret = ERR_PARAM_INVALID;
			g_Record.log_out(ret, level_error, "the crc of file %s is wrong", p_prm_pathname);
			break;
		}
    }
    while (0);

    return ret;
}

// 更改降级模式，用于接收降级消息
void ParamMgr::modify_degrade(uint8_t type, uint8_t * p_time, uint8_t * p_station, uint8_t flag)
{
	P_DEGRADE_MODE pTmpMode	= NULL;
	bool bAdNew = false;
	uint16_t mode_station	= (uint16_t)((p_station[0] << 8) + p_station[1]);

	if (flag == 0x01)		// 设置降级
	{
		for (size_t i=0;i<m_prm_degrade.size();i++)
		{
			if (type == m_prm_degrade[i].degrade_type && same_station(m_prm_degrade[i].degrade_station, mode_station) &&
				memcmp(m_prm_degrade[i].degrade_end_time, DEGRADE_END_DEFAULT, 7) == 0)
			{
				memcpy(m_prm_degrade[i].degrade_start_time, p_time, 7);
				pTmpMode = &m_prm_degrade[i];
				bAdNew = true;
				break;
			}
		}

		if (bAdNew == false)
		{
			DEGRADE_MODE newDgMode	= {0};

			memcpy(newDgMode.degrade_start_time, p_time, 7);
			newDgMode.degrade_station = mode_station;
			newDgMode.degrade_type = (DGTYPE)type;
			memcpy(newDgMode.degrade_end_time, DEGRADE_END_DEFAULT, 7);

			m_prm_degrade.push_back(newDgMode);
			pTmpMode = &m_prm_degrade[m_prm_degrade.size() - 1];
		}
	}
	else if (flag == 0x02)	// 取消降级
	{
		for (size_t i=0;i<m_prm_degrade.size();i++)
		{
			if (type == m_prm_degrade[i].degrade_type && same_station(m_prm_degrade[i].degrade_station, mode_station) &&
				memcmp(m_prm_degrade[i].degrade_end_time, DEGRADE_END_DEFAULT, 7) == 0)
			{
				memcpy(m_prm_degrade[i].degrade_end_time, p_time, 7);
				pTmpMode = &m_prm_degrade[i];
				break;
			}
		}
	}

	if (pTmpMode != NULL)
	{
		memcpy(pTmpMode->degrade_susceptive, pTmpMode->degrade_end_time, 7);
		if (type == degrade_Trouble || type == degrade_Green_Light)
		{
			if (memcmp(pTmpMode->degrade_susceptive, DEGRADE_END_DEFAULT, 7) != 0)
			{
				TimesEx::bcd_time_calculate(pTmpMode->degrade_susceptive, T_DATE, m_prm_device_ctrl.degrade_effect_time);
			}
		}
	}
}


// 查询当前降级模式
void ParamMgr::query_degrade_mode(MODE_EFFECT_FLAGS& mdEffect)
{
	//dbg_formatvar("m_prm_degrade.size:[%d]",m_prm_degrade.size());
	for (size_t i=0;i<m_prm_degrade.size();i++)
	{
		dbg_formatvar("i:[%d]",i);
		dbg_dumpmemory("m_time_now:",CmdSort::m_time_now,7);
		dbg_dumpmemory("degrade_start_time:",m_prm_degrade[i].degrade_start_time,7);
		dbg_dumpmemory("degrade_end_time:",m_prm_degrade[i].degrade_end_time,7);
		if (memcmp(CmdSort::m_time_now, m_prm_degrade[i].degrade_start_time, 7) >= 0 &&
			memcmp(CmdSort::m_time_now, m_prm_degrade[i].degrade_end_time, 7) <= 0)
		{
			if (m_prm_degrade[i].degrade_type == degrade_Uncheck_Order)
			{
				mdEffect.uncheck_order = true;
			}
			else if (same_station(Api::current_station_id, m_prm_degrade[i].degrade_station))
			{
				if (m_prm_degrade[i].degrade_type == degrade_Trouble)
					mdEffect.train_trouble = true;
				else if (m_prm_degrade[i].degrade_type == degrade_Uncheck_DateTime)
					mdEffect.uncheck_datetime = true;
				else if (m_prm_degrade[i].degrade_type == degrade_Uncheck_Fare)
					mdEffect.uncheck_fare = true;
				else if (m_prm_degrade[i].degrade_type == degrade_Green_Light)
					mdEffect.emergency = true;
			}
		}
	}
}

// 降级模式历史记录是否对当前票卡有影响
// prepared_period：是否有预生效期， p_time_now：现在时间，p_time_last：上次对应的操作时间，station_last：上次对应的操作站点
bool ParamMgr::tk_in_mode(DGTYPE mode, bool prepared_period, uint8_t * p_time_now, uint8_t * p_time_last, uint16_t station_last)
{
	for (size_t i=0;i<m_prm_degrade.size();i++)
	{
		// 指定的模式
		if (mode != degrade_null && m_prm_degrade[i].degrade_type != mode)
			continue;

		// 站点相同
		if (station_last != 0 && !same_station(station_last, m_prm_degrade[i].degrade_station))
			continue;

		// 上次时间在模式结束之前
		if (memcmp(p_time_last, m_prm_degrade[i].degrade_end_time, 7) >= 0)
			continue;

		// 上次时间在模式开始的当日
		if (prepared_period && memcmp(p_time_last, m_prm_degrade[i].degrade_start_time , 4) != 0)
			continue;

		// 当前时间在敏感期内
		if (memcmp(p_time_now, m_prm_degrade[i].degrade_susceptive, 7) <= 0)
			return true;
	}

	return false;
}

// ES的单程票票卡是否受指定模式敏感期的影响(ES的有效期在模式开始或开始之后)
bool ParamMgr::EsEffectByAppointedMode(DGTYPE dgAppointed, uint8_t * pCurTime, uint8_t * pLExpireE, uint16_t UseStation/* = 0*/)
{
	for (size_t i=0;i<m_prm_degrade.size();i++)
	{
		// 指定的模式
		if (m_prm_degrade[i].degrade_type != dgAppointed)
			continue;

		// 发生模式的站点与预置票使用站点相同
		if (UseStation != 0 && !same_station(UseStation, m_prm_degrade[i].degrade_station))
			continue;

		// 预置票有效期在模式发生之后
		if (memcmp(pLExpireE, m_prm_degrade[i].degrade_start_time, 7) <= 0)
			continue;

		// 当前时间在敏感期内
		if (memcmp(pCurTime, m_prm_degrade[i].degrade_susceptive, 7) <= 0)
			return true;
	}

	return false;
}

// 查找行政罚金参数
int ParamMgr::query_policy_penalty(uint8_t * p_ticket_type, int policy_code, short& penalty)
{
    //POLICY_PENALTY policy_penalty;
	short val_line_net = -1;
	short val_line_cur = -1;
	short val_station = -1;

    uint16_t ticket_type = (p_ticket_type[0] << 8) + p_ticket_type[1];
    //policy_penalty.policy_code = policy_code;

    for (uint32_t i=0; i<m_prm_policy_penaty.size(); i++)
    {
        if (m_prm_policy_penaty[i].policy_code == policy_code && m_prm_policy_penaty[i].ticket_type == ticket_type)
        {
			if ((m_prm_policy_penaty[i].line_station & 0xFF00) == (Api::current_station_id & 0xFF00))		// 线路为当前线路
			{
				if ((m_prm_policy_penaty[i].line_station & 0xFF) == (Api::current_station_id & 0xFF))		// 线路站点为当前线路站点
				{
					val_station = m_prm_policy_penaty[i].penalty;
					break;
				}
				else if ((m_prm_policy_penaty[i].line_station & 0xFF) == 0x00)	// 只有线路为当前线路
				{
					val_line_cur = m_prm_policy_penaty[i].penalty;
				}
			}
			else if(m_prm_policy_penaty[i].line_station == 0x0000)		// 全线网
			{
				val_line_net = m_prm_policy_penaty[i].penalty;
			}
        }
    }

	if (val_station != -1)
	{
		penalty = val_station;
	}
	else if (val_line_cur != -1)
	{
		penalty = val_line_cur;
	}
	else if (val_line_net != -1)
	{
		penalty = val_line_net;
	}
	else
	{
		return ERR_INPUT_INVALID;
	}

	return 0;
}

uint16_t ParamMgr::config_param(char * p_name_prm)
{
    char source_path[_POSIX_PATH_MAX]	= {0};
    char target_path[_POSIX_PATH_MAX]	= {0};
    char name_old[_POSIX_PATH_MAX]		= {0};
    char old_path[_POSIX_PATH_MAX]		= {0};
    uint16_t prm_type					= 0;
	uint16_t ret						= 0;
	bool reset							= false;

	do
	{
	
		HS_LOG("config_param %s \n",p_name_prm);
		if (memcmp(p_name_prm, "PRM.", 4) != 0)
		{
			ret = ERR_PARAM_INVALID;
			g_Record.log_out(ret, level_error, "name of parameter %s invalid", p_name_prm);
			break;
		}

		prm_type = Publics::string_to_bcd<uint16_t>(p_name_prm + 4, 4);

		sprintf(source_path, "%s/%s/%s", QFile::running_directory(), NAME_TEMP_FOLDER, p_name_prm);

		HS_LOG("source_path %s prm_type=0x%x\n",source_path,prm_type);

		if ((prm_type & 0xF0F0) == 0x9020)
		{
			// 存在需要更新驱动的情况，先卸载驱动
			samdev_destory();
			rfdev_destory();

			sprintf(target_path, "%s/%s", QFile::running_directory(), NAME_TEMP_FOLDER);
			ret = PackageMgr::UnpackageFiles(source_path, target_path);
			if (ret != 0)				break;

			sprintf(source_path, "%s/%s/%s", QFile::running_directory(), NAME_TEMP_FOLDER, NAME_SCRIPT);
			if (PackageMgr::ExcuteShell(source_path) != 0)
			{
				ret = ERR_FILE_ACCESS;
				
				HS_LOG("ERR_FILE_ACCESS \n");
				break;
			}

			reset = true;
		}
		else
		{
			// 其他类型参数
			ret = valid_prm(source_path);
			if (ret != 0){
				HS_LOG("valid_prm \n");
				break;
			}

			ret = load_prm(prm_type, source_path, false, true);
			if (ret != 0){				
				HS_LOG("load_prm \n");
				break;
			}	

			sprintf(target_path, "%s/%s/%s", QFile::running_directory(), NAME_PRM_FOLDER, p_name_prm);
			if (rename(source_path, target_path) != 0)
			{
				ret = ERR_FILE_ACCESS;
				g_Record.log_out(ret, level_error, "rename(%s,%s)=-1", source_path, target_path);
				HS_LOG("rename(%s,%s)=-1\n", source_path, target_path);
				break;
			}

			ret = get_prm_info(prm_type, name_old);
			if (ret != 0 && ret != ERR_PARAM_NOT_EXIST)	break;

			if (strcmp(name_old, p_name_prm) != 0)
			{
				sprintf(old_path, "%s/%s/%s", QFile::running_directory(), NAME_PRM_FOLDER, name_old);
				remove(old_path);
			}
		}

		if (!save_prm_config(prm_type, p_name_prm))
		{
			ret = ERR_FILE_ACCESS;
			HS_LOG("save_prm_config \n");
			break;
		}

		ret = load_prm(prm_type, target_path, reset);

	} while (0);

	g_Record.log_out(ret, level_disaster, "config_param(%s)=%04x", p_name_prm, ret);
	HS_LOG("config_param(%s)=%04x \n", p_name_prm, ret);
	return ret;
}

uint16_t ParamMgr::get_prm_info(uint16_t prm_type, char * p_prm_name)
{
    map<uint16_t, string>::iterator itor = m_prm_config.find(prm_type);
    if (itor == m_prm_config.end())
        return ERR_PARAM_NOT_EXIST;

    strcpy(p_prm_name, itor->second.c_str());
    return ERR_SUCCEED;
}

void ParamMgr::delete_prm_outof_config()
{
	struct dirent *	ent					= NULL;
	char p_path_dir[_POSIX_PATH_MAX]	= {0};
	char pathname[_POSIX_PATH_MAX]		= {0};

	sprintf(p_path_dir, "%s/%s", QFile::running_directory(), NAME_PRM_FOLDER);

	if (access(p_path_dir, 0) == 0)
	{
		DIR * pDir = opendir(p_path_dir);

		while((ent = readdir(pDir)) != NULL)
		{
			if(ent->d_type & DT_REG)
			{
				if (memcmp(ent->d_name, NAME_TEMP_APP, strlen(NAME_TEMP_APP)) != 0)
				{
					if (memcmp(ent->d_name, "PRM.", 4) != 0 || !name_in_config(ent->d_name))
					{
						sprintf(pathname, "%s/%s", p_path_dir, ent->d_name);
						remove(pathname);
					}
				}
			}
		}

		closedir(pDir);
	}
}

// 查找离本站最近的发生进出次序免检的车站,
uint16_t ParamMgr::nearest_uncheckorder_station()
{
	uint8_t u_sensitive_time[7]	= {0};
	uint8_t fare_zone			= 0;
	uint8_t fare_zone_min		= 0xFF;
	uint16_t u_mins				= 0;
	uint16_t u_station			= 0;

	// 寻找区在敏感期内区段最小的车站
	for (uint32_t i=0;i<m_prm_degrade.size();i++)
	{
		if (m_prm_degrade[i].degrade_type == degrade_Uncheck_Order)
		{
			if (query_overtime_min(m_prm_degrade[i].degrade_station, u_mins) == 0)
			{
				memcpy(u_sensitive_time, m_prm_degrade[i].degrade_end_time, 7);
				TimesEx::bcd_time_calculate(u_sensitive_time, T_TIME, 0, 0, u_mins);

				// 时间首先应该在发生站点的敏感期内
				if (memcmp(CmdSort::m_time_now, m_prm_degrade[i].degrade_start_time, 7) >= 0 && memcmp(CmdSort::m_time_now, u_sensitive_time, 7) <= 0)
				{
					if (query_fare_zone(m_prm_degrade[i].degrade_station, fare_zone) == 0)
					{
						if (fare_zone < fare_zone_min)
						{
							fare_zone_min = fare_zone;
							u_station = m_prm_degrade[i].degrade_station;
						}
					}
				}
			}
		}
	}

	return u_station;
}

// 是否出站超时
uint16_t ParamMgr::query_overtime_min(uint16_t entry_station, uint16_t& timeout_in_minutes)
{
	//uint16_t ret = ERR_STATION_INVALID;
	//for (uint16_t i=0; i<m_prm_fare.vec_zone.size(); i++)
	//{
	//	if (m_prm_fare.vec_zone[i].station_id == entry_station)
	//	{
	//		timeout_in_minutes = m_prm_fare.vec_zone[i].ride_time_limit;
	//		ret = 0;
	//		break;
	//	}
	//}

	//g_Record.log_out(ret, level_error, "query_overtime_min(%04x, timeout_in_minutes)=%04x", entry_station, ret);

	//return ret;
	timeout_in_minutes = 180;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////互联网设备参数处理//////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 载入网络拓扑参数 0001
int ParamMgr::load_networktop(const char * p_prm_path, bool only_check/* = false*/)
{
	char sz_read[128]	= {0};
	int ret				= 0;
	size_t read_cnt		= 0;
	QFile fp;

	do
	{
		//g_Record.log_out_debug(0, level_error, "p_prm_path of itp parameter %s", p_prm_path);
		if (!fp.open(p_prm_path, mode_read_text))
		{
			//g_Record.log_out_debug(0, level_error, "fp open 0001 error");
			ret = ERR_FILE_ACCESS;
			break;
		}

		if (!only_check)
		{
			unload_networktop();
		}

		/*
		while(fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			if (prm_section_match(sz_read, IPRM_0001_NETWORKTOP_CFG))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
				}
			}
		}*/

		fp.close();

	} while (0);

	return ret;
}
void ParamMgr::unload_networktop()
{
	//memset(&m_prm_device_ctrl, 0, sizeof(DEVICE_CTRL));
}
/*
void ParamMgr::query_networktop()
{
	//memcpy(&ctrl, &m_prm_device_ctrl, sizeof(DEVICE_CTRL));
}*/


// 载入日历参数 0002
int ParamMgr::load_calendars(const char * p_prm_path, bool only_check/* = false*/)
{
	char sz_read[128]	= {0};
	int ret				= 0;
	size_t read_cnt		= 0;
	QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		if (!only_check)
		{
			unload_calendars();
		}

		//while(fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			//if (prm_section_match(sz_read, IPRM_0002_CALENDARS_CFG))
			{
				/*
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}*/

				if (!only_check)
				{

				}
			}
		}

		fp.close();

	} while (0);

	return ret;
}
void ParamMgr::unload_calendars()
{
	//memset(&m_prm_device_ctrl, 0, sizeof(DEVICE_CTRL));
}


// 载入车票参数 0003
int ParamMgr::load_eticket(const char * p_prm_path, bool only_check/* = false*/)
{
	char sz_read[128]	= {0};
	int ret				= 0;
	size_t read_cnt		= 0;
	QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		if (!only_check)
		{
			unload_eticket();
		}

		/*
		while(fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			if (prm_section_match(sz_read, IPRM_0003_ETICKET_CFG))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
				}
			}
		}*/

		fp.close();

	} while (0);

	return ret;
}
void ParamMgr::unload_eticket()
{
	//memset(&m_prm_device_ctrl, 0, sizeof(DEVICE_CTRL));
}


// 载入费率参数  0004
int ParamMgr::load_rate(const char * p_prm_path, bool only_check/* = false*/)
{
	char sz_read[50000]	= {0};
	int ret				= 0;
	size_t read_cnt		= 0;
	QFile fp;
	uint16_t exit_station;	// 为BCD形式
    ITP_FARE_ZONE itpFrZone;
	//ITP_FARE_ZONE itpCffrZone;
    ITP_FARE_CONFIG_TABLE itpFrConfig;
    ITP_FARE_TABLE itpFrTable;
    ITP_HOLIDAY_TABLE itpHoliday;
    ITP_IDLE_TIME_TABLE itpiTime;

	uint8_t file_type;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		if (!only_check)
		{
			unload_rate();
		}

		//g_Record.log_out_debug(0, level_error,"load_rate");

		/*
		while(fp.read_line_itp(sz_read, sizeof(sz_read), read_cnt))
		{
			if (iprm_head_match(sz_read, IPRM_0004_RATE_CFG))
			{
				if (!only_check)
				{
					file_type = (uint8_t)sz_read[0];
					g_Record.log_out_debug(0, level_error,"file type : [%02X]",file_type);

					g_Record.log_buffer_debug("file generate date : ", (uint8_t *)(sz_read + 1), 7);


					exit_station = Publics::string_to_bcd<uint16_t>(sz_read + 9, 4);
					if (exit_station == Api::current_station_id)
					{
						itpFrZone.station_id = Publics::string_to_bcd<uint16_t>(sz_read + 5, 4);
						itpFrZone.exit_station_id = Publics::string_to_bcd<uint16_t>(sz_read + 9, 4);
						itpFrZone.fare_zone = Publics::string_to_val<uint8_t>(sz_read + 13, 2);
						itpFrZone.ride_time_limit = Publics::string_to_val<uint16_t>(sz_read + 15, 5);
						itpFrZone.penalty_for_timeout = Publics::string_to_val<uint16_t>(sz_read + 20, 4);

						m_iprm_fare.vec_itp_zone.push_back(itpFrZone);
				}
			}
		}*/

		fp.close();

	} while (0);

	return ret;
}
void ParamMgr::unload_rate()
{
	//memset(&m_prm_device_ctrl, 0, sizeof(DEVICE_CTRL));
}


uint16_t ParamMgr::config_param_itp(char * p_name_prm)
{
	char source_path[_POSIX_PATH_MAX]	= {0};
	char target_path[_POSIX_PATH_MAX]	= {0};
	char name_old[_POSIX_PATH_MAX]		= {0};
	char old_path[_POSIX_PATH_MAX]		= {0};
	uint16_t prm_type					= 0;
	uint16_t ret						= 0;
	bool reset							= false;

	do
	{
		if (memcmp(p_name_prm, "IPRM.", 5) != 0)
		{
			ret = ERR_PARAM_INVALID;
			g_Record.log_out(ret, level_error, "name of itp parameter %s invalid", p_name_prm);
			break;
		}

		prm_type = Publics::string_to_bcd<uint16_t>(p_name_prm + 5, 4);

		sprintf(source_path, "%s/%s/%s", QFile::running_directory(), NAME_TEMP_FOLDER, p_name_prm);
		//g_Record.log_out(0, level_error, "source_path of itp parameter %s", source_path);
		g_Record.log_out_debug(0, level_error, "source_path of itp parameter %s", source_path);

		if (prm_type == 0x1002)
		{
			// 存在需要更新驱动的情况，先卸载驱动

			samdev_destory();
			rfdev_destory();

			sprintf(target_path, "%s/%s", QFile::running_directory(), NAME_TEMP_FOLDER);
			g_Record.log_out_debug(0, level_error, "target_path of itp parameter %s", target_path);
			ret = PackageMgr::UnpackageFiles(source_path, target_path);
			if (ret != 0)				break;

			sprintf(source_path, "%s/%s/%s", QFile::running_directory(), NAME_TEMP_FOLDER, NAME_SCRIPT);
			if (PackageMgr::ExcuteShell(source_path) != 0)
			{
				ret = ERR_FILE_ACCESS;
				break;
			}

			reset = true;
		}
		else
		{
			// 其他类型参数
			ret = valid_prm_itp(source_path);
			if (ret != 0)	break;

			ret = load_prm_itp(prm_type, source_path, false, true);
			if (ret != 0)	break;

			sprintf(target_path, "%s/%s/%s", QFile::running_directory(), NAME_IPRM_FOLDER, p_name_prm);
			g_Record.log_out_debug(0, level_error, "target_path of itp parameter %s", target_path);
			if (rename(source_path, target_path) != 0)
			{
				ret = ERR_FILE_ACCESS;
				g_Record.log_out(ret, level_error, "rename(%s,%s)=-1", source_path, target_path);
				break;
			}

			ret = get_prm_info_itp(prm_type, name_old);
			if (ret != 0 && ret != ERR_PARAM_NOT_EXIST)	break;

			if (strcmp(name_old, p_name_prm) != 0)
			{
				sprintf(old_path, "%s/%s/%s", QFile::running_directory(), NAME_IPRM_FOLDER, name_old);
				remove(old_path);
			}
		}

		if (!save_prm_config_itp(prm_type, p_name_prm))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		ret = load_prm_itp(prm_type, target_path, reset);

	} while (0);

	g_Record.log_out(ret, level_disaster, "config_param_itp(%s)=%04x", p_name_prm, ret);

	return ret;
}


// 检查参数合法性
uint16_t ParamMgr::valid_prm_itp(const char * p_prm_pathname)
{
	uint16_t ret = 0;

	do
	{
		if (access(p_prm_pathname, 0) != 0)
		{
			ret = ERR_PARAM_NOT_EXIST;
			g_Record.log_out(ret, level_error, "file %s can not access", p_prm_pathname);
			break;
		}

		/*
		if (!DataSecurity::crc32_in_file_end_valid(p_prm_pathname))
		{
			ret = ERR_PARAM_INVALID;
			g_Record.log_out(ret, level_error, "the crc of file %s is wrong", p_prm_pathname);
			break;
		}*/
	}
	while (0);

	return ret;
}


// 加载单个参数
uint16_t ParamMgr::load_prm_itp(const uint16_t prm_type, const char * p_prm_path, bool cmd_reset/* = false*/, bool only_check/* = false*/)
{
	uint16_t ret = 0;

	do
	{
		// 直接移动到下载文件启用消息
		if ((uint16_t)(prm_type) == 0x1002)
		{
			if (cmd_reset)
			{
				CmdSort::m_flag_app_run = false;
				g_Record.log_out(ret, level_error, "reader reset......");
			}
			break;
		}

		/*
		if (!DataSecurity::crc32_in_file_end_valid(p_prm_path))
		{
			ret = ERR_PARAM_INVALID;
			g_Record.log_out(ret, level_error, "the crc of file %s is wrong", p_prm_path);
			break;
		}*/

		P_FUNC_IPRM pfun;
		for (size_t i=0; i<sizeof(m_iprm_for_reader)/sizeof(m_iprm_for_reader[0]); i++)
		{
			if (m_iprm_for_reader[i].iprm_type == prm_type)
			{
				pfun = m_iprm_for_reader[i].ifun;
				ret = (uint16_t)(this->*pfun)(p_prm_path, only_check);
				break;
			}
		}

	} while (0);

	g_Record.log_out(ret, level_invalid, "load_prm_itp(%04x,%s, bool,bool)=%04x", prm_type, p_prm_path, ret);

	return ret;
}

uint16_t ParamMgr::get_prm_info_itp(uint16_t iprm_type, char * p_iprm_name)
{
	map<uint16_t, string>::iterator itor = m_iprm_config.find(iprm_type);
	if (itor == m_iprm_config.end())
		return ERR_PARAM_NOT_EXIST;

	strcpy(p_iprm_name, itor->second.c_str());
	return ERR_SUCCEED;
}


// 更改参数配置
bool ParamMgr::save_prm_config_itp(uint16_t iprm_type, const string& strPath)
{
	char sz_to_write[_POSIX_PATH_MAX + 8]	= {0};
	char sz_config_path[_POSIX_PATH_MAX]	= {0};
	size_t len_to_write						= 0;
	bool add_new_to_config					= true;
	bool ret								= true;
	QFile file;

	do
	{
		sprintf(sz_config_path, "%s/%s", QFile::running_directory(), NAME_IPRM_CONFIG);

		if (!file.open(sz_config_path, mode_write))
			break;

		map<uint16_t, string>::iterator itor;
		for (itor=m_iprm_config.begin(); itor!=m_iprm_config.end(); ++itor)
		{
			if (iprm_type == itor->first)
			{
				add_new_to_config = false;
				itor->second = strPath;
			}
			sprintf(sz_to_write, "%04x:%s\n", itor->first, (itor->second).c_str());
			len_to_write = strlen(sz_to_write);
			if (file.write(sz_to_write, len_to_write) < len_to_write)
			{
				ret = false;
				break;
			}
		}

		if (ret && add_new_to_config)
		{
			m_iprm_config.insert(make_pair(iprm_type, strPath));

			sprintf(sz_to_write, "%04x:%s\n", iprm_type, strPath.c_str());
			len_to_write = strlen(sz_to_write);
			if (file.write(sz_to_write, len_to_write) < len_to_write)
			{
				ret = false;
			}
		}

		file.close();

	} while (0);

	if (!ret)
		g_Record.log_out(0, level_error, "save_prm_config_itp(%04x,%s)=false", iprm_type, strPath.c_str());

	return ret;
}

// 载入读写器应用参数
int ParamMgr::load_itp_reader_app(const char * p_prm_path, bool only_check/* = false*/)
{
	return 0;
}



// ITP地铁黑名单参数0010
int ParamMgr::load_itp_mtr_single_black(const char * p_prm_path, bool only_check/* = false*/)
{
#define LEN_OBLACK_LINE	(5 + 20 + 3 + 2)

	char sz_read[128]	= {0};
	int ret				= 0;
	size_t read_cnt		= 0;
	int file_size		= 0;
	QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		/*
		file_size = (int)(fp.length() - 12);
		if (file_size % LEN_OBLACK_LINE != 0)
		{
			ret = ERR_PARAM_INVALID;
			break;
		}*/

		if (!only_check)
		{
			unload_mtr_single_black();

			m_prm_mtr_black.blacks_single.logical_id = new(std::nothrow) char*[file_size / LEN_OBLACK_LINE];
			if (m_prm_mtr_black.blacks_single.logical_id == NULL)
			{
				ret = ERR_UNDEFINED;
				break;
			}
		}

		while(fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			/*
			if (prm_section_match(sz_read, IPRM_0010_METROBLCAK_CFG))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
					m_prm_mtr_black.blacks_single.logical_id[m_prm_mtr_black.blacks_single.record_count] = new(std::nothrow) char[20];
					if (m_prm_mtr_black.blacks_single.logical_id[m_prm_mtr_black.blacks_single.record_count] == NULL)
					{
						ret = ERR_UNDEFINED;
						break;
					}
					memcpy(m_prm_mtr_black.blacks_single.logical_id[m_prm_mtr_black.blacks_single.record_count], sz_read + 5, 20);
					m_prm_mtr_black.blacks_single.record_count ++;
				}
			}*/
		}

		fp.close();

	} while (0);

	return ret;
}
void ParamMgr::unload_itp_mtr_single_black()
{
	if (m_prm_mtr_black.blacks_single.logical_id != NULL)
	{
		for (int i=0; i<m_prm_mtr_black.blacks_single.record_count; i++)
		{
			if (m_prm_mtr_black.blacks_single.logical_id[i] != NULL)
			{
				delete []m_prm_mtr_black.blacks_single.logical_id[i];
				m_prm_mtr_black.blacks_single.logical_id[i] = NULL;
			}
		}

		delete []m_prm_mtr_black.blacks_single.logical_id;
		m_prm_mtr_black.blacks_single.logical_id = NULL;
	}

	m_prm_mtr_black.blacks_single.record_count = 0;
}


// 载入外部票种映射参数  0012
int ParamMgr::load_ext_ticket(const char * p_prm_path, bool only_check/* = false*/)
{
	char sz_read[128]	= {0};
	int ret				= 0;
	size_t read_cnt		= 0;
	QFile fp;

	do
	{
		if (!fp.open(p_prm_path, mode_read_text))
		{
			ret = ERR_FILE_ACCESS;
			break;
		}

		if (!only_check)
		{
			unload_rate();
		}

		while(fp.read_line(sz_read, sizeof(sz_read), read_cnt))
		{
			/*
			if (prm_section_match(sz_read, IPRM_0004_RATE_CFG))
			{
				if (!Publics::string_is_expected(sz_read + 5, read_cnt - 7, Publics::NUMERIC_SEQ, 0x20))
				{
					ret = ERR_PARAM_INVALID;
					break;
				}

				if (!only_check)
				{
				}
			}*/
		}

		fp.close();

	} while (0);

	return ret;
}
void ParamMgr::unload_ext_ticket()
{
	//memset(&m_prm_device_ctrl, 0, sizeof(DEVICE_CTRL));
}
