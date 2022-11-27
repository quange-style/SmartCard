#pragma once

#include "../api/Declares.h"

class CLoopList
{
public:
	CLoopList(void);
	~CLoopList(void);

	// 查找符合条件的节点
	bool QueryList(char * passcord,ETYTKOPER operType);

	// 更新进闸队列
	void UpdateList(char * lpLogicalID,ETYTKOPER operType);

private:
#define TRADE_SPACE_COUNT		100
	char m_ArrQrCode[TRADE_SPACE_COUNT][256];
};
