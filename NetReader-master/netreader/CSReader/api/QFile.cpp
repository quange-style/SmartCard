#include "QFile.h"
#include "Declares.h"
#include "CmdSort.h"
#include <stdarg.h>
#include <unistd.h>
#include "Records.h"

char QFile::m_current_directory[_POSIX_PATH_MAX] = {0};

QFile::QFile(void)
{
    m_p_stream = NULL;
    memset(m_buffer, 0, SIZE_FILE_BUFFER);
    m_ptr_of_buffer = 0;
    m_len_once_read = 0;

	///////////////////////////////

	memset(m_itp_buffer, 0, SIZE_FILE_ITP_BUFFER);
}

QFile::~QFile(void)
{
    if (m_p_stream != NULL)
        close();
}

QFile::QFile(const char * p_file_path, const char * p_open_mode)
{
    memset(m_buffer, 0, SIZE_FILE_BUFFER);
    m_ptr_of_buffer = 0;
    m_len_once_read = 0;

    m_p_stream = fopen(p_file_path, p_open_mode);

	///////////////////////////////////////////
	memset(m_itp_buffer, 0, SIZE_FILE_ITP_BUFFER);
}

bool QFile::open(const char * p_file_path, const char * p_open_mode)
{
    m_p_stream = fopen(p_file_path, p_open_mode);

    return opened();
}

void QFile::close()
{
    if (m_p_stream != NULL)
    {
        fclose(m_p_stream);
        m_p_stream = NULL;
    }
}

inline bool QFile::opened()
{
    return (m_p_stream != NULL);
}

size_t QFile::read(void * p_read_buffer, size_t size_read_buffer, size_t len_to_read)
{
    if (len_to_read > size_read_buffer)
        len_to_read = size_read_buffer;

    return fread(p_read_buffer, 1, len_to_read, m_p_stream);
}

size_t QFile::write(void * p_write_buffer, size_t len_to_write)
{
    return fwrite(p_write_buffer, 1, len_to_write, m_p_stream);
}

size_t QFile::write_on_position(void * p_write_buffer, size_t len_to_write, long posistion_to_write)
{
    long pos_init = ftell(m_p_stream);

    seek(posistion_to_write, SEEK_SET);
    size_t len_written = write(p_write_buffer, len_to_write);
    seek(pos_init, SEEK_SET);

    return len_written;
}

bool QFile::read_line(void * p_read_buffer, size_t size_read_buffer, size_t& read_cnt)
{
	bool result		= false;

	// 本次取得的行数据的长度，如果本次缓冲中没有取得'\n'结尾的数据就继续
	size_t once		= 0;

	uint8_t* p_buf	= (uint8_t*)p_read_buffer;
	read_cnt		= 0;

	memset(p_buf, 0, size_read_buffer);

	// 暂不考虑行数据超越_rBuffer大小的问题
	while(!result)
	{
		result = get_line_from_buffer(m_buffer + m_ptr_of_buffer, m_len_once_read - m_ptr_of_buffer, size_read_buffer - read_cnt, p_buf + read_cnt, once);
		m_ptr_of_buffer += once;
		read_cnt += once;

		if (!result)
		{
			if (feof(m_p_stream) > 0)
			{
				result = (read_cnt == 0 ? false : true);
				break;
			}

			memset(m_buffer, 0, SIZE_FILE_BUFFER);
			m_len_once_read = (long)read(m_buffer, SIZE_FILE_BUFFER, SIZE_FILE_BUFFER);
			m_ptr_of_buffer = 0;
		}
	}

	return result;
}

bool QFile::read_line_itp(void * p_read_buffer, size_t size_read_buffer, size_t& read_cnt)
{
	bool result		= false;

	// 本次取得的行数据的长度，如果本次缓冲中没有取得'\n'结尾的数据就继续
	size_t once		= 0;

	uint8_t* p_buf	= (uint8_t*)p_read_buffer;
	read_cnt		= 0;

	memset(p_buf, 0, size_read_buffer);

	// 暂不考虑行数据超越_rBuffer大小的问题
	while(!result)
	{
		result = get_line_from_buffer(m_itp_buffer + m_ptr_of_buffer, m_len_once_read - m_ptr_of_buffer, size_read_buffer - read_cnt, p_buf + read_cnt, once);
		m_ptr_of_buffer += once;
		read_cnt += once;

		//g_Record.log_out_debug(0, level_error,"result : %d", result);
		//g_Record.log_out_debug(0, level_error,"read_cnt : %d", read_cnt);

		if (!result)
		{
			if (feof(m_p_stream) > 0)
			{
				result = (read_cnt == 0 ? false : true);
				break;
			}

			memset(m_itp_buffer, 0, SIZE_FILE_ITP_BUFFER);
			m_len_once_read = (long)read(m_buffer, SIZE_FILE_ITP_BUFFER, SIZE_FILE_ITP_BUFFER);
			m_ptr_of_buffer = 0;
		}
	}

	return result;
}

bool QFile::get_line_from_buffer(uint8_t * p_buffer, size_t size_buffer, size_t line_odd, uint8_t * p_line, size_t& len_line)
{
	for (len_line=0; len_line<size_buffer; len_line++)
	{
		if (len_line == line_odd)
			return true;

		if (p_buffer[len_line] == '\r')
		{
			len_line += 1;
			if (len_line >= size_buffer)
				break;
		}
		if (p_buffer[len_line] == '\n')
		{
			len_line += 1;
			return true;
		}

		p_line[len_line] = p_buffer[len_line];
	}

	return false;
}

int QFile::seek(long offset, long src_pos)
{
    return fseek(m_p_stream, offset, src_pos);
}

long QFile::length()
{
    long files_size = 0;

    long init_pos = ftell(m_p_stream);

    seek(0, SEEK_END);
    files_size = ftell(m_p_stream);
    seek(0, init_pos);

    return files_size;
}

void QFile::flush()
{
    fflush(m_p_stream);
}

long QFile::length(const char * p_file_path)
{
    long file_size = 0;

    FILE * fp = fopen(p_file_path, mode_read);
    if (fp != 0)
    {
        fseek(fp, 0, SEEK_END);
        file_size = ftell(fp);

        fclose(fp);
    }

    return file_size;
}

char * QFile::name_from_path(char * p_path)
{
    size_t len_path = strlen(p_path);

    while(len_path != 0)
    {
        if (*(p_path + len_path) == '\\' || *(p_path + len_path) == '/')
            break;
        len_path --;
    }

    return (p_path + len_path + 1);
}

void QFile::init_running_directory()
{
    getcwd(m_current_directory, _POSIX_PATH_MAX);
}

char * QFile::running_directory()
{
    return (char *)m_current_directory;
}

void QFile::running_directory(char * p_directory)
{
    strcpy(p_directory, m_current_directory);
}
