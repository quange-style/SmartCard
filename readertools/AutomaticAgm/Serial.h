#pragma once

#include "Structs.h"

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

	inline void SetTimeOut(DWORD dwTimeOut)
	{
		m_dwTimeOut = dwTimeOut;
	}

protected:
	// ͨѶ�����Ӧ�Ķ˿ں�
	int m_nPort;

	// ���˵���
	HANDLE m_hReadPoint;

	// д�˵���
	HANDLE m_hWritePoint;

	// ������¼�
	HANDLE m_hEventRead;

	// �˿ڴ򿪱�ʶ
	bool m_bOpen;

	// ����̱߳�ʶ
	bool m_bWatch;

	// �Ƿ�ѭ���������ݱ�ʶ
	bool m_bRead;

	// ͨѶ��ʱʱ�䣬Ĭ��3��
	DWORD m_dwTimeOut;

	// �������
	HANDLE m_hMutex;

protected:

	HANDLE m_hEventWrite;
	
	COMSTAT m_comStat;

	size_t FormatData(BYTE Anti, BYTE beep, BYTE cmdClassic, BYTE cmd, LPBYTE pDataDomain, WORD lenDomain, LPBYTE pDataToSend);

	int ReceiveComplete(LPBYTE pRecv, size_t nRecv);

	// ���ڼ���߳�
	static void WatchProc(LPVOID lparam);

	BOOL Send(LPVOID lpBuffer, size_t nBytesToWrite);

	BOOL Receive(LPVOID lpBuffer, size_t nSizeBuffer, size_t& nBytesRead);

	void GetLocalTimeBcd(LPBYTE pTimeBcd);

	WORD Crc16(LPBYTE ptr, size_t len);

	// ����ϴε�����
	void ClearLastData();
};

extern Serial g_Serial;