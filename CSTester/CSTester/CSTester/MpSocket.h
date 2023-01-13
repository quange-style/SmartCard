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

	// 向服务器发送数据
	int MpSend(LPBYTE lpSend, int nToSend);

	// 接收数据
	int MpRecv(LPBYTE lpRecv, int * pRecved = NULL);

	static void RecvProc(void * lparam);

	int FormPackage(LPBYTE lpPackage, BYTE bDataType, LPBYTE lpData, WORD lenData);

public:
	// 连接服务器
	//int MpConnect(const char * lpServerIP = "192.168.1.200", const u_short nPortServer = 8090);
	int MpConnect(const char * lpServerIP = "10.200.48.140", const u_short nPortServer = 8090);

	// 断开连接
	int MpDisconnect();

	// 获取IP信息
	void CurrentIP(char * lpszIP, int nLen, LPBYTE lpIPBuf);

	// 获取服务器IP信息，只支持IPV4
	void CurrentIP(LPBYTE lpIPBuf);

	// 格式化充值通讯返回的错误
	int FormatCommError(LPBYTE lpRecv, int nPos, int nLen);

	// 向服务器发送申请，并等待相应的回复
	int SendSrcWaitTarget(BYTE bDataType, LPBYTE lpData, WORD lenData, int nTimes, BYTE bPackageType, LPBYTE lpRecv, int * pRecved = NULL);
};
