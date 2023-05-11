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
	static int saleTotleCounts;	//���۴���
	static int saleSuccessCount;	//���۳ɹ�����
	static int saleFailureCount;	// ����ʧ������
};

