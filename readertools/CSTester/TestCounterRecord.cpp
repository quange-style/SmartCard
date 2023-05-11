#include "stdafx.h"
#include "TestCounterRecord.h"
int TestCounterRecord::saleTotleCounts = 0;
int TestCounterRecord::saleSuccessCount = 0;	//发售成功次数
int TestCounterRecord::saleFailureCount = 0;	// 发售失败数量

void TestCounterRecord::setSuccessCount() {
	saleTotleCounts += 1;
	saleSuccessCount += 1;
}
void TestCounterRecord::setFailureCount() {
	saleTotleCounts += 1;
	saleFailureCount += 1;
}

int TestCounterRecord::getCurrentTotleCounts() {
	return saleTotleCounts;
}
int TestCounterRecord::getCurrentSuccessCount() {
	return saleSuccessCount;
}
int TestCounterRecord::getCurrentFailureCount() {
	return saleFailureCount;
}

int TestCounterRecord::getFailureRate() {
	int rate = saleSuccessCount / saleTotleCounts;
	return rate;
}
