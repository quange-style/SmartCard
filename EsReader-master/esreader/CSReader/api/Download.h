// DownLoad.h

#include "libserial.h"
#include <stdint.h>

#pragma once

class Download
{
public:
    //Download(Serial * p_ser);
    ~Download();

    void file_mgr(uint8_t * p_msg_file, uint8_t * p_data_send, uint16_t& len_send);

private:

    void make_file_effect(uint8_t * p_msg_file, char * p_file_name, uint32_t size_file, uint32_t crc_file, uint8_t * p_data_send, uint16_t& len_send);

    void recv_new_file(uint8_t * p_msg_file, char * p_file_name, uint32_t size_file, uint32_t crc_file, uint8_t * p_data_send, uint16_t& len_send);

    uint32_t m_file_size;
};
