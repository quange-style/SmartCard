#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SerialFile.h"
#include "../api/TimesEx.h"

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
	uint8_t timeTemp[7]={0};
	uint8_t currentTime[7]={0};
	char cardNoTemp[20]={0};
	char currentCardNo[20]={0};

	if(operType == operGetETicket)
	{
		length = 32;
	}else if(operType == operThirdGetETicket)
	{
		length = 18;
	}else{
		length = 27;
	}

	if(length==27)//进出闸使用另外一种判重方式
	{
		for (int i=0;i<TRADE_SPACE_COUNT;i++)
		{
			memcpy(timeTemp,m_ArrQrCode[i],7);
			memcpy(cardNoTemp,m_ArrQrCode[i]+7,20);

			memcpy(currentTime,lpLogicalID,7);
			memcpy(currentCardNo,lpLogicalID+7,20);
			TimesEx::bcd_time_calculate(timeTemp, T_TIME, 0, 0, 0, 30);//当前时间添加30s
			if (memcmp(cardNoTemp, currentCardNo, 20) == 0&&memcmp(timeTemp, currentTime, 7) > 0)//卡号相等并且时间没有超过30s,判断为重复
			{
				bRet = true;
				break;
			}
		}
	}
	else//其他方式保留
	{
		for (int i=0;i<TRADE_SPACE_COUNT;i++)
		{
			if (memcmp(lpLogicalID, m_ArrQrCode[i], length) == 0)
			{
				bRet = true;
				break;
			}
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
		length = 27;
	}else if(operType == operGetETicket){
		length = 32;
	}else if(operType == operThirdGetETicket)
	{
		length = 18;
	}else{
		length = 27;
	}

	memcpy(uFrontID, lpLogicalID, length);
	for (int i=0;i<TRADE_SPACE_COUNT;i++)
	{
		memcpy(uTempID, m_ArrQrCode[i], length);
		memcpy(m_ArrQrCode[i], uFrontID, length);
		memcpy(uFrontID, uTempID, length);
	}
}
