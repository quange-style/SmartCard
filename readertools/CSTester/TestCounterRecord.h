#pragma once
#include "stdafx.h"
class TestCounterRecord
{

public:
//	void setTotleCounts();
	 void setSuccessCount();
	 void setFailureCount();

	 int getCurrentTotleCounts();
	 int getCurrentSuccessCount();
	 int getCurrentFailureCount();

	 int getFailureRate();

private:
	static int saleTotleCounts;	//发售次数
	static int saleSuccessCount;	//发售成功次数
	static int saleFailureCount;	// 发售失败数量
};

