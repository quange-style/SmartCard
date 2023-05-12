#include "Download.h"
#include "QFile.h"
#include "Structs.h"
#include "CmdSort.h"
#include "ParamMgr.h"
#include "DataSecurity.h"
#include "Errors.h"
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

//Download::Download(Serial * p_ser)
//{
//	m_p_objSer = p_ser;
//}

Download::~Download()
{

}

void Download::file_mgr(uint8_t * p_msg_file, uint8_t * p_data_send, uint16_t& len_send)
{
    uint8_t flag_start_end			= 0; //参数下载启用标识
    uint32_t file_size				= 0;	// size in cmd
    uint32_t file_crc				= 0;	// real size
    uint8_t file_name_len			= 0;
    char file_name[256]				= {0};


    flag_start_end = p_msg_file[16];
    memcpy(&file_size, p_msg_file + 17, 4);
    memcpy(&file_crc, p_msg_file + 21, 4);
    file_name_len = p_msg_file[25];
    memcpy(file_name, p_msg_file + 26, file_name_len);

    if (flag_start_end == 0)
    {
        recv_new_file(p_msg_file, file_name, file_size, file_crc, p_data_send, len_send);
    }
    else if (flag_start_end == 1)
    {
        make_file_effect(p_msg_file, file_name, file_size, file_crc, p_data_send, len_send);
    }

}

void Download::make_file_effect(uint8_t * p_msg_file, char * p_file_name, uint32_t size_file, uint32_t crc_file, uint8_t * p_data_send, uint16_t& len_send)
{
    RETINFO ret							= {0};

    ret.wErrCode = g_Parameter.config_param(p_file_name);

    len_send = sizeof(ret);
    memcpy(p_data_send, &ret, len_send);
}

void Download::recv_new_file(uint8_t * p_msg_file, char * p_file_name, uint32_t size_file, uint32_t crc_file, uint8_t * p_data_send, uint16_t& len_send)
{
    FILE * fp							= NULL;
    RETINFO ret							= {0};
    int received						= 0;
    const uint16_t size_file_buf		= 50000;
    uint8_t file_buffer[size_file_buf]	= {0};
    uint16_t len_cur_recv				= 0;
    uint32_t size_last					= 0;
    uint32_t total_recv					= 0;
    char file_path[_POSIX_PATH_MAX]		= {0};
    uint32_t crc_calc					= 0xFFFFFFFF;



    sprintf(file_path, "%s/%s", QFile::running_directory(), NAME_TEMP_FOLDER);
    if (access(file_path, 0) != 0 && mkdir(file_path, 0755) != 0)
    {
        ret.wErrCode = ERR_FILE_ACCESS;
        len_send = sizeof(ret);
        memcpy(p_data_send, &ret, len_send);
        return;
    }

    strcat(file_path, "/");
    strcat(file_path, p_file_name);
    if (access(file_path, 0) == 0)
        remove(file_path);

    fp = fopen(file_path, "w+b");
    if (fp == NULL)
    {
        ret.wErrCode = ERR_FILE_ACCESS;
        len_send = sizeof(ret);
        memcpy(p_data_send, &ret, len_send);
        return;
    }

    // 答复准备接收
    CmdSort::cmd_send(p_msg_file, (uint8_t *)(&ret), sizeof(ret));

    while(true)
    {

        received = libserial_recv_package(size_file_buf, 100, file_buffer);
        if (received <= 0)
        {
            //ret.wErrCode = ERR_RECIVE_DATA;
            //break;
            len_send = 0;
            return;		// 超时不响应
        }

        memcpy(&len_cur_recv, file_buffer + 16, 2);
        memcpy(&size_last, file_buffer + 18, 4);

        fwrite(file_buffer + 22, 1, len_cur_recv, fp);

        total_recv += len_cur_recv;

        crc_calc = DataSecurity::Crc32Reverse(crc_calc, file_buffer + 22, len_cur_recv);
        if (size_last == 0)
        {
            crc_calc = ~crc_calc;
            break;
        }
        CmdSort::cmd_send(file_buffer, (uint8_t *)(&ret), sizeof(ret));
    }

    fclose(fp);

    // 检查文件是否正确
    if (size_last == 0)
    {
        if (total_recv != size_file || crc_file != crc_calc)
        {
            ret.wErrCode = ERR_PARAM_INVALID;
        }
    }

    CmdSort::cmd_send(file_buffer, (uint8_t *)(&ret), sizeof(ret));

    len_send = 0;
}
