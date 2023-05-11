//时间计算工具
// Created by flpkp on 2020/3/18.
//
#pragma once

#include <sys/time.h>


class TimeCalcUtils {
public:
    //根据开始、结束时间计算耗时的毫秒数
   static long calcMsec(timeval &start,timeval  &end);
};


