#pragma once

#include "../api/Declares.h"

class CLoopList
{
public:
	CLoopList(void);
	~CLoopList(void);

	// ���ҷ��������Ľڵ�
	bool QueryList(char * passcord,ETYTKOPER operType);

	// ���½�բ����
	void UpdateList(char * lpLogicalID,ETYTKOPER operType);

private:
#define TRADE_SPACE_COUNT		30
	char m_ArrQrCode[TRADE_SPACE_COUNT][256];
};
