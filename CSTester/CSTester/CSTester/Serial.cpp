#include "StdAfx.h"
#include "Serial.h"
#include "PubFuncs.h"

BYTE Serial::m_SendID = 0;

Serial::Serial(void)
{
	m_bOpen = false;
	memset(&m_comStat, 0, sizeof(m_comStat));
	m_dwTimeOut = 5000;
}

Serial::~Serial(void)
{
	if (m_bOpen)
		Close();
}

int Serial::Open(const int nPort)
{
	int nRet				= -1;
	char szTemp[16]			= {0};
	DCB	dcb					= {0};	// 串口相关设置
	COMMTIMEOUTS TimeOuts	= {0};	// 串口超时结构

	do 
	{
		if (IsOpen())
			Close();

		sprintf(szTemp, "COM%d", nPort);
		m_hReadPoint = CreateFile(szTemp, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, NULL);
		if(m_hReadPoint == INVALID_HANDLE_VALUE)
			break;

		//- 设置通讯缓冲 -//
		if (!SetupComm(m_hReadPoint, 4096, 4096))
			break;

		//- 设置监控事件 -//
		if (!SetCommMask(m_hReadPoint, EV_RXCHAR))
			break;

		//- 把间隔超时设为最大,把总超时设为0将导致ReadFile立即返回并完成操作 -//
		//TimeOuts.ReadIntervalTimeout		= 1000;
		//TimeOuts.ReadTotalTimeoutConstant	= 1000;
		//TimeOuts.ReadTotalTimeoutMultiplier	= 1000;
		TimeOuts.ReadIntervalTimeout		= 300000;
		TimeOuts.ReadTotalTimeoutConstant	= 300000;
		TimeOuts.ReadTotalTimeoutMultiplier	= 300000;

		//- 设置写超时以指定WriteComm成员函数中的GetOverlappedResult函数的等待时间 -//
		TimeOuts.WriteTotalTimeoutConstant		= 1000;
		TimeOuts.WriteTotalTimeoutMultiplier	= 1000;
		if (!SetCommTimeouts(m_hReadPoint, &TimeOuts))
		{
			break;
		}

		if(!GetCommState(m_hReadPoint, &dcb))
		{
			break;
		}

		//- 设置串口参数 -//
		dcb.fBinary		= TRUE;
		dcb.BaudRate	= 115200;
		dcb.ByteSize	= 8; 
		dcb.fParity		= TRUE;
		dcb.Parity		= NOPARITY;
		dcb.StopBits	= ONESTOPBIT;
		if(!SetCommState(m_hReadPoint, &dcb))
		{
			break;
		}

		m_bWatch = true;
		m_hMutex = CreateEvent(NULL, FALSE, TRUE, NULL);
		m_hEventRead = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_hEventWrite = CreateEvent(NULL, FALSE, FALSE, NULL);
		CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)(WatchProc), this, 0, NULL);

		m_bOpen = true;
		m_nPort = nPort;
		nRet = 0;

	} while (0);

	if (!true && m_hReadPoint != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hReadPoint);
		m_hReadPoint = NULL;
	}

	return nRet;
}

int Serial::Close()
{
	if (m_bOpen)
	{
		m_bRead = false;
		m_bWatch = false;

		SetEvent(m_hEventWrite);

		CloseHandle(m_hEventRead);
		m_hEventRead = NULL;

		CloseHandle(m_hEventWrite);
		m_hEventWrite = NULL;

		CloseHandle(m_hReadPoint);
		m_hReadPoint = NULL;

		m_bOpen = false;
	}
	return 0;
}

size_t Serial::FormatData(BYTE Anti, BYTE beep, BYTE cmdClassic, BYTE cmd, LPBYTE pDataDomain, WORD lenDomain, LPBYTE pDataToSend)
{
	WORD wCrc16	= 0;

	pDataToSend[0] = 0xAA;
	pDataToSend[1] = 0x00;
	pDataToSend[2] = m_SendID;
	pDataToSend[3] = cmdClassic;
	pDataToSend[4] = cmd;
	pDataToSend[5] = 0x00;
	pDataToSend[6] = (Anti & 0x03) + ((beep & 0x03) << 2);
	GetLocalTimeBcd(pDataToSend + 7);
	memcpy(pDataToSend + 14, &lenDomain, 2);
	memcpy(pDataToSend + 16, pDataDomain, lenDomain);

	wCrc16 = Crc16(pDataToSend, lenDomain + 16);
	memcpy(pDataToSend + lenDomain + 16, &wCrc16, 2);

	return (lenDomain + 18);
}

int Serial::ReceiveComplete(LPBYTE pRecv, size_t nRecv)
{
	size_t lenData = 0;
	lenData = pRecv[5] + (pRecv[6] << 8);
	if (nRecv >= lenData + 9)
	{
		WORD crc = Crc16(pRecv, nRecv - 2);
		if (memcmp(&crc, pRecv + nRecv - 2, 2) == 0)
			return 0;
		return -1;
	}

	return 1;
}

RETINFO Serial::Communicate(BYTE Anti, BYTE beep, BYTE cmdClassic, BYTE cmd, LPBYTE pDataDomain, WORD lenDomain, BYTE * pRecv, size_t MaxRecv)
{
	RETINFO ret				= {0};
	int result				= 0;
	BYTE bSend[10300]		= {0};
	size_t nBytesToWrite	= 0;
	size_t nLenData			= 0;
	size_t nReadOnce		= 0;
	size_t nReadTotal		= 0;

	do 
	{
		ret.wErrCode = 0xFFFF;
		memset(pRecv, 0, MaxRecv);

		if (!m_bOpen)
			break;

		//PurgeComm(m_hReadPoint, PURGE_RXCLEAR | PURGE_RXABORT);
		ClearLastData();

		if (WaitForSingleObject(m_hMutex, 300000) != WAIT_OBJECT_0)
			break;

		nBytesToWrite = FormatData(Anti, beep, cmdClassic, cmd, pDataDomain, lenDomain, bSend);

		m_bRead = true;

		if (!Send(bSend, nBytesToWrite))
			break;

		do 
		{
			SetEvent(m_hEventWrite);

			m_SendID++;

			if (WaitForSingleObject(m_hEventRead, m_dwTimeOut) != WAIT_OBJECT_0)
			{
				m_SendID--;
				break;
			}

			if (!Receive(pRecv + nReadTotal, SIZE_RECV_BUFFER - nReadTotal, nReadOnce))
				break;

			nReadTotal += nReadOnce;

			result = ReceiveComplete(pRecv, nReadTotal);
			if (result < 0)	//接收出错
			{
				//PurgeComm(m_hReadPoint, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
				break;
			}
			else if (result == 0)
			{
				ret.wErrCode = 0;
				m_bRead = false;
			}

		} while (m_bRead);

		if (ret.wErrCode != 0)	break;

		memcpy(&ret, pRecv + 7, sizeof(ret));

		if (pRecv[2] != bSend[2])
			ret.wErrCode = 0xFF00;

	} while (0);

	PurgeComm(m_hReadPoint, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	SetEvent(m_hMutex);

	return ret;
}

// 串口监控线程
void Serial::WatchProc(LPVOID lparam)
{
	DWORD dwEvent;
	DWORD dwTrans;
	DWORD dwError;
	Serial * pCom	= (Serial *)lparam;
	OVERLAPPED os		= {0};


	os.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); 

	while(pCom->m_bWatch)
	{
		if (WaitForSingleObject(pCom->m_hEventWrite, INFINITE) == WAIT_OBJECT_0 && pCom->m_bWatch)
		{
			while(pCom->m_bRead)
			{
				ClearCommError(pCom->m_hReadPoint, &dwError, &pCom->m_comStat);
				if(pCom->m_comStat.cbInQue)													//有数据在缓冲
				{
					SetEvent(pCom->m_hEventRead);
					WaitForSingleObject(pCom->m_hEventWrite, INFINITE);
				}
				else
				{
					dwEvent = 0;
					if(!WaitCommEvent(pCom->m_hReadPoint, &dwEvent, &os))						//进行出错处理
					{
						if(GetLastError() == ERROR_IO_PENDING)
						{
							GetOverlappedResult(pCom->m_hReadPoint, &os, &dwTrans, TRUE);		//无限等待重复操作结果
						}
					}
				}
			}
		}
	}

	CloseHandle(os.hEvent);
}

BOOL Serial::Send(LPVOID lpBuffer, size_t nBytesToWrite)
{
	OVERLAPPED osWrite	= {0};
	DWORD nSend			= 0;
	BOOL bRet			= FALSE;

	osWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!WriteFile(m_hReadPoint, lpBuffer, (DWORD)nBytesToWrite, &nSend, &osWrite))
	{
		if (GetLastError() == ERROR_IO_PENDING && GetOverlappedResult(m_hReadPoint, &osWrite, &nSend, TRUE))
		{
			if (nSend == nBytesToWrite)
				bRet = TRUE;
		}
	}
	else if (nSend == nBytesToWrite)
	{
		bRet = TRUE;
	}
	
	CloseHandle(osWrite.hEvent);

	return bRet;
}

BOOL Serial::Receive(LPVOID lpBuffer, size_t nSizeBuffer, size_t& nBytesRead)
{
	BOOL bRet			= FALSE;
	OVERLAPPED osRead	= {0};
	DWORD dwError		= 0;

	nBytesRead = 0;
	osRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	ClearCommError(m_hReadPoint, &dwError, &m_comStat);

	if (m_comStat.cbInQue > 0)
	{
		if (m_comStat.cbInQue > nSizeBuffer)
		{
			nBytesRead = 0;
		}
		else if (!ReadFile(m_hReadPoint, lpBuffer, m_comStat.cbInQue, (LPDWORD)(&nBytesRead), &osRead))
		{
			if (GetLastError() == ERROR_IO_PENDING && GetOverlappedResult(m_hReadPoint, &osRead, (LPDWORD)(&nBytesRead), TRUE))
			{
				bRet = TRUE;
			}
		}
		else
		{
			bRet = TRUE;
		}
	}

	CloseHandle(osRead.hEvent);

	return bRet;

	//BOOL bRet			= FALSE;
	//OVERLAPPED osRead	= {0};
	//DWORD dwError		= 0;
	//DWORD dwComEvent	= EV_RXCHAR;

	//do 
	//{
	//	nBytesRead = 0;
	//	osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	//	if (!WaitCommEvent(m_hReadPoint, &dwComEvent, &osRead))
	//	{
	//		if (GetLastError() != ERROR_IO_PENDING)
	//			break;

	//		bRet = GetOverlappedResult(m_hReadPoint, &osRead, (LPDWORD)(&nBytesRead), TRUE);
	//		if (!bRet)	break;
	//	}

	//	ClearCommError(m_hReadPoint, &dwError, &m_comStat);

	//	if (m_comStat.cbInQue > 0)
	//	{
	//		if (m_comStat.cbInQue > nSizeBuffer)
	//		{
	//			nBytesRead = 0;
	//		}
	//		else if (!ReadFile(m_hReadPoint, lpBuffer, m_comStat.cbInQue, (LPDWORD)(&nBytesRead), &osRead))
	//		{
	//			if (GetLastError() == ERROR_IO_PENDING && GetOverlappedResult(m_hReadPoint, &osRead, (LPDWORD)(&nBytesRead), TRUE))
	//			{
	//				bRet = TRUE;
	//			}
	//		}
	//		else
	//		{
	//			bRet = TRUE;
	//		}
	//	}

	//} while (0);

	//CloseHandle(osRead.hEvent);

	//return bRet;
}

void Serial::GetLocalTimeBcd(LPBYTE pTimeBcd)
{
	SYSTEMTIME tm = {0};
	GetLocalTime(&tm);

	Time_Bcd_Ex_Sys(pTimeBcd, &tm, false);
}

WORD Serial::Crc16(LPBYTE ptr, size_t len)
{
	const static WORD crc_ta[16]=
	{
		0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
		0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef
	};

	WORD crc;
	BYTE da;


	crc=0;
	while (len-- != 0)
	{
		da = crc >> 12;
		crc <<= 4;
		crc ^= crc_ta[da ^ (*ptr >> 4)];

		da = crc >> 12;
		crc <<= 4;

		crc ^= crc_ta[da ^ (*ptr & 0x0f)];
		ptr++;
	}
	return (crc);
}


// 清除上次的数据
void Serial::ClearLastData()
{
	BYTE bRecv[1024] = {0};
	size_t size_red;
	if (Receive(bRecv, sizeof(bRecv), size_red))
	{
		PurgeComm(m_hReadPoint, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
		Sleep(50);
	}
}