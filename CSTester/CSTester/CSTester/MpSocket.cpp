#include "StdAfx.h"
#include "MpSocket.h"
#include "PubFuncs.h"

MpSocket::MpSocket(void)
{
	m_Socket = 0;
}

MpSocket::~MpSocket(void)
{
	WSACleanup();
}

int MpSocket::MpConnect(const char * lpServerIP, const u_short nPortServer)
{
	int nReturn			= -1;
	WSADATA wsd;
	SOCKADDR_IN addr;

	int TimeOut			= 5000;			//设置发送超时5秒
	u_long ulMode		= 1;			// 设置异步模式
	struct timeval timeout ;
	fd_set fdset;

	do 
	{
		if (WSAStartup(MAKEWORD(2, 2), &wsd) != NO_ERROR)
			break;

		m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_Socket == INVALID_SOCKET)
			break;

		if(setsockopt(m_Socket, SOL_SOCKET, SO_SNDTIMEO,(char *)&TimeOut, sizeof(TimeOut)) == SOCKET_ERROR)
			break;

		////设置非阻塞方式连接
		if(ioctlsocket(m_Socket, FIONBIO, (unsigned long*)&ulMode) == SOCKET_ERROR)
			break;

		addr.sin_family			= AF_INET;
		addr.sin_port			= htons(nPortServer);
		addr.sin_addr.s_addr	= inet_addr(lpServerIP);

		// 采用异步方式连接，避免死锁
		connect(m_Socket, (struct sockaddr *)&addr, sizeof(addr));
		//if (connect(m_Socket, (SOCKADDR *)&addr, sizeof(addr) == SOCKET_ERROR))
		//{
		//	closesocket(m_Socket);
		//	m_Socket = 0;
		//	break;;
		//}

		FD_ZERO(&fdset);
		FD_SET(m_Socket, &fdset);
		timeout.tv_sec = 5;			//连接超时5秒
		timeout.tv_usec = 0;
		if (select(0, NULL, &fdset, NULL, &timeout) <= 0 )
		{
			closesocket(m_Socket);
			break;;
		}

		m_bEndThread = false;
		m_hEventRecv = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_hEventSend = CreateEvent(NULL, FALSE, FALSE, NULL);

		CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)RecvProc, this, 0, NULL);

		SetEvent(m_hEventRecv);

		nReturn = 0;

	} while (0);

	return nReturn;
}

// 断开连接
int MpSocket::MpDisconnect()
{
	int nReturn = 0;

	m_bEndThread = true;
	SetEvent(m_hEventRecv);
	SetEvent(m_hEventSend);

	if (m_Socket != 0)
	{
		closesocket(m_Socket);
		m_Socket = 0;
	}

	return nReturn;
}

int MpSocket::MpSend(LPBYTE lpSend, int nToSend)
{
	int nReturn		= 0;
	int nSendTotal	= 0;

	while(nSendTotal != nToSend)
	{
		nReturn = send(m_Socket, (char *)lpSend, nToSend, 0);
		if (nReturn == SOCKET_ERROR)
		{
			nReturn = -1;
			closesocket(m_Socket);
			break;
		}
		else
		{
			nSendTotal += nReturn;
		}
		nReturn = 0;
	}
	return nReturn;
}

int MpSocket::MpRecv(LPBYTE lpRecv, int * pRecved/*=NULL*/)
{
	int nReturn		= 0;
	int ntotalRead	= 0;
	WORD wLenData	= 0;

	fd_set fdRead;
	TIMEVAL	aTime;

	aTime.tv_sec = 5;	// 定义读事件超时时间5秒
	aTime.tv_usec = 0;


	// 循环接收数据
	while (TRUE)
	{
		// 置空fdRead事件为空
		FD_ZERO(&fdRead);

		// 给客户端socket设置读事件
		FD_SET(m_Socket, &fdRead);	

		// 调用select函数，判断是否有读事件发生
		nReturn = select(0, &fdRead, NULL, NULL, &aTime);
		if (nReturn <= 0)		//读错误或读超时
		{
			closesocket(m_Socket);
			nReturn = -1;
			break;
		}
		else if (FD_ISSET(m_Socket, &fdRead))				//发生读事件
		{
			nReturn = recv(m_Socket, (char *)(lpRecv + ntotalRead), 256, 0);
			if (nReturn == SOCKET_ERROR || nReturn == 0)
			{
				closesocket(m_Socket);
				nReturn = -1;
				break;
			}

			ntotalRead += nReturn;

			if (ntotalRead >= 5)
				memcpy(&wLenData, lpRecv + 3, 2);

			if (wLenData + 6 == ntotalRead && lpRecv[ntotalRead - 1] == 0x03)		
			{
				if (pRecved != NULL)
				{
					*pRecved = ntotalRead;
				}
				nReturn = 0;
				break;
			}
		}
		else
		{
			break;
		}
		nReturn = 0;
	}

	return nReturn;
}

// 获取服务器IP信息
void MpSocket::CurrentIP(char * lpszIP, int nLen, LPBYTE lpIPBuf)
{
	char	szName[255]	= {0};
	int		nIndex		= 0;
	int		nCount		= 0;
	struct  sockaddr_in  ad	= {0};

	nCount			= sizeof(struct sockaddr_in);
	getsockname(m_Socket, (struct sockaddr*)&ad, &nCount);
	sprintf_s(lpszIP, nLen, "%d.%d.%d.%d", ad.sin_addr.S_un.S_un_b.s_b1,ad.sin_addr.S_un.S_un_b.s_b2,
		ad.sin_addr.S_un.S_un_b.s_b3,ad.sin_addr.S_un.S_un_b.s_b4);


	nCount = 0;
	for(int n = 0; n < (int)strlen(lpszIP); n++)
	{
		if(lpszIP[n] != '.')
		{
			szName[nIndex++]	= lpszIP[n];
		}
		else
		{
			lpIPBuf[nCount++] = atoi(szName);
			ZeroMemory(szName, sizeof(szName));
			nIndex			= 0;
		}
	}

	lpIPBuf[3] = atoi(szName);
}

// 获取服务器IP信息，只支持IPV4
void MpSocket::CurrentIP(LPBYTE lpIPBuf)
{
	struct  sockaddr_in  ad	= {0};

	int nCount	= (int)sizeof(struct sockaddr_in);
	getsockname(m_Socket, (struct sockaddr*)&ad, &nCount);
	lpIPBuf[0] = ad.sin_addr.S_un.S_un_b.s_b1;
	lpIPBuf[1] = ad.sin_addr.S_un.S_un_b.s_b2;
	lpIPBuf[2] = ad.sin_addr.S_un.S_un_b.s_b3;
	lpIPBuf[3] = ad.sin_addr.S_un.S_un_b.s_b4;
}

// 格式化充值通讯返回的错误
int MpSocket::FormatCommError(LPBYTE lpRecv, int nPos, int nLen)
{
	int nReturn = 0;
	char szTemp[16] = {0};
	memcpy(szTemp, lpRecv + nPos, nLen);
	nReturn = atoi(szTemp);

	if (nReturn)	nReturn = -1;

	return nReturn;
}

void MpSocket::RecvProc(void * lparam)
{
	MpSocket * pClass	= (MpSocket *)lparam;
	int nRecved			= 0;

	while(!pClass->m_bEndThread)
	{
		if (WaitForSingleObject(pClass->m_hEventRecv, INFINITE) == WAIT_OBJECT_0 && !pClass->m_bEndThread)
		{
			memset(pClass->m_bRecvBuf, 0, sizeof(pClass->m_bRecvBuf));
			if (pClass->MpRecv(pClass->m_bRecvBuf, &nRecved) == 0)
			{
				pClass->m_bRecvID = pClass->m_bRecvBuf[2];
				pClass->m_bTargetMsgType = pClass->m_bRecvBuf[1];

				pClass->m_nLenRecv = nRecved;

				SetEvent(pClass->m_hEventSend);					
			}
		}
	}
}

int MpSocket::SendSrcWaitTarget(BYTE bDataType, LPBYTE lpData, WORD lenData, int nTimes, BYTE bPackageType, LPBYTE lpRecv, int * pRecved/* = NULL*/)
{
	int nRet			= -1;
	BYTE bSend[1024]	= {0};
	int nToSend			= 0;
	int i;

	do 
	{
		// 查询包接收完毕才开始发送对应请求
		if (WaitForSingleObject(m_hEventSend, 3000) != WAIT_OBJECT_0)
			break;

		// 组织正常的包数据
		nToSend = FormPackage(bSend, bDataType, lpData, lenData);
		nRet = MpSend(bSend, nToSend);
		if (nRet != 0)
			break;

		SetEvent(m_hEventRecv);

		// 发送请求报文后发送查询包，直到服务器处理完毕
		for (i=0;i<nTimes;i++)
		{
			if (WaitForSingleObject(m_hEventSend, 3000) != WAIT_OBJECT_0)
			{
				nRet = -1;
				break;
			}			

			if (m_bTargetMsgType == bPackageType)
			{
				nRet = 0;
				memcpy(lpRecv, m_bRecvBuf, m_nLenRecv);
				if (pRecved != NULL) *pRecved = m_nLenRecv;
				break;
			}

			nToSend = FormPackage(bSend, 0x01, NULL, 0);
			SetEvent(m_hEventRecv);
			nRet = MpSend(bSend, nToSend);
			if (nRet != 0)
				break;

			Sleep(50);
		}

	} while (0);

	return nRet;
}
																											 
int MpSocket::FormPackage(LPBYTE lpPackage, BYTE bDataType, LPBYTE lpData, WORD lenData)
{
	int nRet = 5;

	lpPackage[0] = 0xEB;
	lpPackage[1] = bDataType;
	lpPackage[2] = m_bRecvID;
	memcpy(lpPackage + 3, &lenData, 2);

	memcpy(lpPackage + nRet, lpData, lenData);
	nRet += lenData;

	lpPackage[nRet] = 0x03;
	
	return (nRet + 1);
}