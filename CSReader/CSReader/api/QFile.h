#pragma once

#include <stdint.h>
#include <stdio.h>

#ifndef _POSIX_PATH_MAX
#define _POSIX_PATH_MAX		255
#endif

class QFile
{

public:
    QFile(void);
    ~QFile(void);

#define mode_read				"r"			// 只读
#define mode_read_text			"rt"		// 文本只读
#define mode_read_binary		"rb"		// 二进制只读
#define mode_write				"w"			// 只写，如果文件存在将被覆盖
#define mode_read_and_write		"r+"		// 读写
#define mode_add_by_create		"a+"		// 追加

    QFile(const char * p_file_path, const char * p_open_mode);

    bool open(const char * p_file_path, const char * p_open_mode);

    void close();

    bool opened();

    size_t read(void * p_read_buffer, size_t size_read_buffer, size_t len_to_read);

    size_t write(void * p_write_buffer, size_t len_to_write);

    size_t write_on_position(void * p_write_buffer, size_t len_to_write, long posistion_to_write);

	bool read_line(void * p_read_buffer, size_t size_read_buffer, size_t& read_cnt);

	bool read_line_itp(void * p_read_buffer, size_t size_read_buffer, size_t& read_cnt);

    int seek(long offset, long src_pos);

    long length();

    inline void flush();

    static long length(const char * p_file_path);

    static char * name_from_path(char * p_path);

    static void init_running_directory();

    static char * running_directory();

    static void running_directory(char * p_directory);

private:

    long m_ptr_of_buffer;

    long m_len_once_read;

    FILE * m_p_stream;

    static char m_current_directory[_POSIX_PATH_MAX];

#define SIZE_FILE_BUFFER 1024
    uint8_t m_buffer[SIZE_FILE_BUFFER];

#define SIZE_FILE_ITP_BUFFER 50000
	uint8_t m_itp_buffer[SIZE_FILE_ITP_BUFFER];

    // 获取缓冲中的一行数据，以换行符为准
	bool get_line_from_buffer(uint8_t * p_buffer, size_t size_buffer, size_t line_odd, uint8_t * p_line, size_t& len_line);
};
