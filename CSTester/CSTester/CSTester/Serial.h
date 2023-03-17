#pragma once


#define SIZE_RECV_BUFFER			1024 * 2

class Serial
{
public:
	Serial(void);
	~Serial(void);

	static BYTE m_SendID;

	int Open(const int nPort);

	int Close();

	RETINFO Communicate(BYTE Anti, BYTE beep, BYTE cmdClassic, BYTE cmd, LPBYTE pDataDomain, WORD lenDomain, BYTE * pRecv, size_t MaxRecv);
	
	inline int OwnerPort()
	{
		return m_nPort;
	}

	inline bool IsOpen()
	{
		return m_bOpen;
	}

	inline DWORD GetTimeOut()
	{
		return m_dwTimeOut;
	}

	inline void SetTimeOut(DWORD dwTimeOut)
	{
		m_dwTimeOut = dwTimeOut;
	}

protected:
	// 通讯对象对应的端口号
	int m_nPort;

	// 读端点句柄
	HANDLE m_hReadPoint;

	// 写端点句柄
	HANDLE m_hWritePoint;

	// 读完成事件
	HANDLE m_hEventRead;

	// 端口打开标识
	bool m_bOpen;

	// 监控线程标识
	bool m_bWatch;

	// 是否循环接受数据标识
	bool m_bRead;

	// 通讯超时时间，默认3秒
	DWORD m_dwTimeOut;

	// 互斥变量
	HANDLE m_hMutex;

protected:

	HANDLE m_hEventWrite;
	
	COMSTAT m_comStat;

	size_t FormatData(BYTE Anti, BYTE beep, BYTE cmdClassic, BYTE cmd, LPBYTE pDataDomain, WORD lenDomain, LPBYTE pDataToSend);

	int ReceiveComplete(LPBYTE pRecv, size_t nRecv);

	// 串口监控线程
	static void WatchProc(LPVOID lparam);

	BOOL Send(LPVOID lpBuffer, size_t nBytesToWrite);

	BOOL Receive(LPVOID lpBuffer, size_t nSizeBuffer, size_t& nBytesRead);

	void GetLocalTimeBcd(LPBYTE pTimeBcd);

	WORD Crc16(LPBYTE ptr, size_t len);

	// 清除上次的数据
	void ClearLastData();
};

extern Serial g_Serial;