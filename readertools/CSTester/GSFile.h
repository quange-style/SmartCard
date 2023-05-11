//********************************************************************
// �ļ�����		: GSFile.h
// ����ʱ��		: 2011/06/02
// �޸�ʱ��		:
// �޸���		:
// �ļ�����		: ��ͷ�ļ�
// ����			: ʵ�ֹ��ܶ������ļ���д�࣬ʹ�ļ���д����ȫ������
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

	// �вι��캯��������������Open
	GSFile(const char * lpFilePath, const char * openFlag);
	~GSFile(void);

// �򿪷�ʽ��
#define ModeRead		"r"			// ֻ��
#define ModeTRead		"rt"		// �ı�ֻ��
#define ModeBRead		"rb"		// ������ֻ��
#define ModeWrite		"w"			// ֻд������ļ����ڽ�������
#define ModeR_W			"r+"		// ��д
#define ModeAppend		"a+"		// ׷��


	// ���ļ���lpFilePath:�ļ�·����nOpenFlag:��ģʽ
	bool Open(const char * lpFilePath, const char * openFlag);

	// �ر��ļ�
	void Close();

	// �ļ��Ƿ���жϣ�����вι��캯��ʹ��
	bool IsOpen(){ return m_pFile != NULL;}

	// ���ļ�,_rBuffer:�����壬_DesToRead����Ҫ��ȡ���ֽ���������ʵ�ʶ�ȡ���ֽ���
	size_t Read(void * _rBuffer, size_t _sizeBuf, size_t _DesToRead);

	// д�ļ�,_wBuffer:д���壬_DesToWrite����Ҫд����ֽ���������ʵ��д����ֽ���
	size_t Write(void * _wBuffer, size_t _DesToWrite);

	// ��һ������,_rBuffer:������,����ܶ���һ��'\n',����true,���򷵻�false
	bool ReadLine(void * _rBuffer, size_t size);

	// �ƶ��ļ�ָ��, _Offset:�����_DesPos��ƫ�ƣ�_DesPos��Ŀ��λ��
	int Seek(long _Offset, long _srcPos);

	// ��ȡ�ļ���С
	static DWORD FileSize(char * lpFilePath);
	DWORD FileSize();

	// ��һ���ļ�ȫ·���л�ȡһ���ļ���
	static char * FileNameFromPathName(char * lpFullPath);

protected:
	// �ļ�����
	BYTE m_flBuffer[SIZE_FILE_BUFFER];
	BYTE m_flTemp[SIZE_FILE_BUFFER];

	// ����λ�ñ�ʶ
	long m_posBuf;

	// �ļ�ָ��
	FILE * m_pFile;

	// һ��ʵ�ʻ���ĳ���
	long m_nOnceRead;

	// ��ȡ�����е�һ�����ݣ��Ի��з�Ϊ׼
	bool GetLine(LPBYTE lpSrc, int sizeSrc, LPBYTE lpTarg, int& sizeTarg);
};
