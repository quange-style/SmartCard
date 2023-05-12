#pragma once

#include <time.h>
#include <stdint.h>

typedef struct tm TMSTRUCT, * PTMSTRUCT;

class TimesEx
{
public:
    TimesEx(void);

    // 是否闰年
    static bool year_is_leapyear(uint16_t year);

    // 是否闰年,pYear为BCD形式
    static bool year_is_leapyear(uint8_t * p_bcd_year);

#define T_YY		0x01
#define T_YYYY		0x03
#define T_MM		(0x01 << 2)
#define T_DD		(0x01 << 3)
#define T_DATE	(T_YYYY | T_MM | T_DD)
#define T_HH		(0x01 << 4)
#define T_MNMN		(0x01 << 5)
#define T_SS		(0x01 << 6)
#define T_TIME		(T_DATE | T_HH | T_MNMN | T_SS)
    // BCD时间是否合法
    static bool bcd_time_valid(uint8_t * p_bcd_time, uint8_t time_format);

    // BCD年月中每个月的最大天数
    static uint8_t month_max_day(uint8_t * p_bcd_yyyymm);
    static uint8_t month_max_day(uint16_t year, uint8_t month);

    // BCD与TM互转
    static void tm2_bcd4_exchange(uint8_t * p_tm2, uint8_t * p_bcd4, bool exchage_direction);
    static void tm4_bcd7_exchange(uint8_t * p_tm4, uint8_t * p_bcd7, bool exchage_direction);
    static void TM_BCD(uint8_t * lpTm, int lenTm, uint8_t * lpBCD, int lenBCD, bool bForuint16_t);

    // BCD时间加减
    static void bcd_time_calculate(uint8_t * p_bcd_time, uint8_t time_format, int days = 0, int hours = 0, int mins = 0, int secs = 0);

    // 比较BCD有效期
    static int CmpExpire(uint8_t * lpBcdNow, uint8_t * lpBcdExpire, uint8_t bFormat, short nDays = 0, short nHours = 0, short nMin = 0, short nSec = 0);

    // 判断是否超时
    static bool timeout(uint8_t * p_bcd_time_target, uint8_t * p_bcd_time_src, int sub_in_minute);

    // 获取某个时间点所在的运营日
    static void TimeForWorkDay(uint8_t * pTimePoint, uint8_t * pSWorkDay = NULL, uint8_t * pEWorkDay = NULL);

    // 时间pTimeSrc对于时间pTimeTarg的运营日的位置，小于0:运营日前，0:运营日中，大于0:运营日后
    static int TimeWorkDayCmp(uint8_t * pTimeSrc, uint8_t * pTimeTarg);

	// 将BCD时间转换为tm时间
	static void time_bcd_to_tm(uint8_t * p_time_bcd, uint8_t bFormat, PTMSTRUCT p_tm);

	// 判断某个日期是星期几
	static uint8_t weekday_theday(uint8_t * p_date_bcd);

	// 补足年份中的高两位
	static void century_fill(uint8_t * p_time);
};

