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
	int nTotal		= 0;		// 行数据的实际长度，因为一次不一定能取得完整的一行数据
	int nOnce		= 0;		// 本次取得的行数据的长度，如果本次缓冲中没有取得'\n'结尾的数据就继续
	LPBYTE prBuf	= (LPBYTE)_rBuffer;
	
	memset(_rBuffer, 0, size);

	// 暂不考虑行数据超越_rBuffer大小的问题
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

// 从一个文件全路径中获取一个文件名
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
