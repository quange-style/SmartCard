//ʱ����㹤��
// Created by flpkp on 2020/3/18.
//
#pragma once

#include <sys/time.h>


class TimeCalcUtils {
public:
    //���ݿ�ʼ������ʱ������ʱ�ĺ�����
   static long calcMsec(timeval &start,timeval  &end);
};


