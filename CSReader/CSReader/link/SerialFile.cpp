#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SerialFile.h"

CLoopList::CLoopList(void)
{
	memset(&m_ArrQrCode, 0, sizeof(m_ArrQrCode));
}

CLoopList::~CLoopList(void)
{

}

// 查找符合条件的节点
bool CLoopList::QueryList(char * lpLogicalID,ETYTKOPER operType)
{
	bool bRet = false;
	int length = 0;

	if ((operType == operEntry))
	{
		length = 46;
	}else if(operType == operGetETicket){
		length = 32;
	}else if(operType == operThirdGetETicket)
	{
		length = 18;
	}else{
		length = 46;
	}

	for (int i=0;i<TRADE_SPACE_COUNT;i++)
	{
		if (memcmp(lpLogicalID, m_ArrQrCode[i], length) == 0)
		{
			bRet = true;
			break;
		}
	}

	return bRet;
}

// 更新进闸队列
void CLoopList::UpdateList(char * lpLogicalID,ETYTKOPER operType)
{
	char uTempID[256]	= {0};
	char uFrontID[256]	= {0};
	int length = 0;

	if ((operType == operEntry))
	{
		length = 46;
	}else if(operType == operGetETicket){
		length = 32;
	}else if(operType == operThirdGetETicket)
	{
		length = 18;
	}else{
		length = 46;
	}

	memcpy(uFrontID, lpLogicalID, length);
	for (int i=0;i<TRADE_SPACE_COUNT;i++)
	{
		memcpy(uTempID, m_ArrQrCode[i], length);
		memcpy(m_ArrQrCode[i], uFrontID, length);
		memcpy(uFrontID, uTempID, length);
	}
}
