//
// Created by flpkp on 2020/3/18.
//

#include "TimeCalcUtils.h"

long TimeCalcUtils::calcMsec(timeval &start, timeval &end) {
    long time_use=0;
    long msec = 0;
    time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//Œ¢√Î
    msec = time_use / 1000;
    return msec;
}
