//********************************************************************
// 文件名称		: GSFile.h
// 创建时间		: 2011/06/02
// 修改时间		:
// 修改人		:
// 文件类型		: 类头文件
// 描述			: 实现功能多样的文件读写类，使文件读写更安全，快速
//********************************************************************

#pragma once

#include <stdio.h>
#include <io.h>
#include <share.h>
#include <fcntl.h>

#define SIZE_FILE_BUFFER		1024 * 100

class GSFile
{
public:
	GSFile(void);

	// 有参构造函数，功能类似于Open
	GSFile(const char * lpFilePath, const char * openFlag);
	~GSFile(void);

// 打开方式宏
#define ModeRead		"r"			// 只读
#define ModeTRead		"rt"		// 文本只读
#define ModeBRead		"rb"		// 二进制只读
#define ModeWrite		"w"			// 只写，如果文件存在将被覆盖
#define ModeR_W			"r+"		// 读写
#define ModeAppend		"a+"		// 追加


	// 打开文件，lpFilePath:文件路径，nOpenFlag:打开模式
	bool Open(const char * lpFilePath, const char * openFlag);

	// 关闭文件
	void Close();

	// 文件是否打开判断，配合有参构造函数使用
	bool IsOpen(){ return m_pFile != NULL;}

	// 读文件,_rBuffer:读缓冲，_DesToRead：需要读取的字节数，返回实际读取的字节数
	size_t Read(void * _rBuffer, size_t _sizeBuf, size_t _DesToRead);

	// 写文件,_wBuffer:写缓冲，_DesToWrite：需要写入的字节数，返回实际写入的字节数
	size_t Write(void * _wBuffer, size_t _DesToWrite);

	// 读一行数据,_rBuffer:读缓冲,如果能读到一个'\n',返回true,否则返回false
	bool ReadLine(void * _rBuffer, size_t size);

	// 移动文件指针, _Offset:相对于_DesPos的偏移，_DesPos：目标位置
	int Seek(long _Offset, long _srcPos);

	// 获取文件大小
	static DWORD FileSize(char * lpFilePath);
	DWORD FileSize();

	// 从一个文件全路径中获取一个文件名
	static char * FileNameFromPathName(char * lpFullPath);

protected:
	// 文件缓冲
	BYTE m_flBuffer[SIZE_FILE_BUFFER];
	BYTE m_flTemp[SIZE_FILE_BUFFER];

	// 缓冲位置标识
	long m_posBuf;

	// 文件指针
	FILE * m_pFile;

	// 一次实际缓冲的长度
	long m_nOnceRead;

	// 获取缓冲中的一行数据，以换行符为准
	bool GetLine(LPBYTE lpSrc, int sizeSrc, LPBYTE lpTarg, int& sizeTarg);
};
