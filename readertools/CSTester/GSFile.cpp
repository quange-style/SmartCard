// GSFile.cpp

#include "stdafx.h"
#include "GSFile.h"

GSFile::GSFile(void)
{
	memset(m_flBuffer, 0, SIZE_FILE_BUFFER);
	memset(m_flTemp, 0, SIZE_FILE_BUFFER);
	m_posBuf = m_nOnceRead = 0;
	m_pFile = NULL;
}

GSFile::~GSFile(void)
{
	if (m_pFile != NULL)
		Close();
}

GSFile::GSFile(const char * lpFilePath, const char * openFlag)
{
	memset(m_flBuffer, 0, SIZE_FILE_BUFFER);
	memset(m_flTemp, 0, SIZE_FILE_BUFFER);
	m_posBuf = m_nOnceRead = 0;
	m_pFile = fopen(lpFilePath, openFlag);;
}

bool GSFile::Open(const char * lpFilePath, const char * openFlag)
{
	if ((m_pFile = fopen(lpFilePath, openFlag)) != 0)
		return true;

	return false;
}

void GSFile::Close()
{
	fclose(m_pFile);
	m_pFile = NULL;
}

size_t GSFile::Read(void * _rBuffer, size_t _sizeBuf, size_t _DesToRead)
{
	return fread_s(_rBuffer, _sizeBuf, 1, _DesToRead, m_pFile);
}

size_t GSFile::Write(void * _wBuffer, size_t _DesToWrite)
{
	return fwrite(_wBuffer, 1, _DesToWrite, m_pFile);
}

bool GSFile::ReadLine(void * _rBuffer, size_t size)
{
	bool bResult	= false;
	int nTotal		= 0;		// �����ݵ�ʵ�ʳ��ȣ���Ϊһ�β�һ����ȡ��������һ������
	int nOnce		= 0;		// ����ȡ�õ������ݵĳ��ȣ�������λ�����û��ȡ��'\n'��β�����ݾͼ���
	LPBYTE prBuf	= (LPBYTE)_rBuffer;
	
	memset(_rBuffer, 0, size);

	// �ݲ����������ݳ�Խ_rBuffer��С������
	while(!bResult)
	{
		bResult = GetLine(m_flBuffer + m_posBuf, m_nOnceRead - m_posBuf, prBuf + nTotal, nOnce);
		m_posBuf += nOnce;
		if (!bResult)
		{
			nTotal += nOnce;
			if (feof(m_pFile))
			{
				bResult = (nTotal == 0 ? false : true);
				break;
			}

			memset(m_flBuffer, 0, SIZE_FILE_BUFFER);
			m_nOnceRead = (long)Read(m_flBuffer, SIZE_FILE_BUFFER, SIZE_FILE_BUFFER);
			m_posBuf = 0;
		}
	}

	return bResult;
}

int GSFile::Seek(long _Offset, long _srcPos)
{
	return fseek(m_pFile, _Offset, _srcPos);
}

DWORD GSFile::FileSize(char * lpFilePath)
{
	int handleFile;
	DWORD dwSize = 0;
	if (_sopen_s(&handleFile, lpFilePath, _O_RDONLY, _SH_DENYNO, 0) == 0)	
	{
		dwSize = _filelength(handleFile);
		close(handleFile);
	}

	return dwSize;
}

DWORD GSFile::FileSize()
{
	DWORD dwSize;
	long lPos = ftell(m_pFile);
	Seek(0, SEEK_END);
	dwSize = (DWORD)ftell(m_pFile);
	Seek(0, lPos);
	return dwSize;
}

bool GSFile::GetLine(LPBYTE lpSrc, int sizeSrc, LPBYTE lpTarg, int& sizeTarg)
{
	bool bRet = false;
	int i;
	if (sizeSrc <= 0)
	{
		return bRet;
	}
	for (i=0;i<sizeSrc;i++)
	{
		if (lpSrc[i] == '\n')
		{
			bRet = true;
			break;
		}
	}
	if (bRet)
		sizeTarg = i + 1;
	else
		sizeTarg = i;
	memcpy(lpTarg, lpSrc, i);
	return bRet;
}

// ��һ���ļ�ȫ·���л�ȡһ���ļ���
char * GSFile::FileNameFromPathName(char * lpFullPath)
{
	int nLen = (int)strlen(lpFullPath);
	while(nLen != 0)
	{
		if (*(lpFullPath + nLen) == '\\')
			break;
		nLen --;
	}

	return (lpFullPath + nLen + 1);
}
