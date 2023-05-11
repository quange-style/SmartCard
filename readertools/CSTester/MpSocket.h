#pragma once

#include "SockStruct.h"
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")


class MpSocket
{
public:
	MpSocket(void);
	~MpSocket(void);

protected:
	SOCKET m_Socket;

	bool m_bEndThread;
	HANDLE m_hEventRecv;
	HANDLE m_hEventSend;
	BYTE m_bRecvID;
	BYTE m_bRecvBuf[1024];
	WORD m_nLenRecv;
	BYTE m_bTargetMsgType;

	// ���������������
	int MpSend(LPBYTE lpSend, int nToSend);

	// ��������
	int MpRecv(LPBYTE lpRecv, int * pRecved = NULL);

	static void RecvProc(void * lparam);

	int FormPackage(LPBYTE lpPackage, BYTE bDataType, LPBYTE lpData, WORD lenData);

public:
	// ���ӷ�����
	//int MpConnect(const char * lpServerIP = "192.168.1.200", const u_short nPortServer = 8090);
	int MpConnect(const char * lpServerIP = "10.200.48.140", const u_short nPortServer = 8090);

	// �Ͽ�����
	int MpDisconnect();

	// ��ȡIP��Ϣ
	void CurrentIP(char * lpszIP, int nLen, LPBYTE lpIPBuf);

	// ��ȡ������IP��Ϣ��ֻ֧��IPV4
	void CurrentIP(LPBYTE lpIPBuf);

	// ��ʽ����ֵͨѶ���صĴ���
	int FormatCommError(LPBYTE lpRecv, int nPos, int nLen);

	// ��������������룬���ȴ���Ӧ�Ļظ�
	int SendSrcWaitTarget(BYTE bDataType, LPBYTE lpData, WORD lenData, int nTimes, BYTE bPackageType, LPBYTE lpRecv, int * pRecved = NULL);
};
