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

// ���ҷ��������Ľڵ�
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

	if(length==27)//����բʹ������һ�����ط�ʽ
	{
		for (int i=0;i<TRADE_SPACE_COUNT;i++)
		{
			memcpy(timeTemp,m_ArrQrCode[i],7);
			memcpy(cardNoTemp,m_ArrQrCode[i]+7,20);

			memcpy(currentTime,lpLogicalID,7);
			memcpy(currentCardNo,lpLogicalID+7,20);
			TimesEx::bcd_time_calculate(timeTemp, T_TIME, 0, 0, 0, 30);//��ǰʱ�����30s
			if (memcmp(cardNoTemp, currentCardNo, 20) == 0&&memcmp(timeTemp, currentTime, 7) > 0)//������Ȳ���ʱ��û�г���30s,�ж�Ϊ�ظ�
			{
				bRet = true;
				break;
			}
		}
	}
	else//������ʽ����
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

// ���½�բ����
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
