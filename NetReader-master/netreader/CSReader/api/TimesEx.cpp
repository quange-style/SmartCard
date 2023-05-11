#include "TimesEx.h"
#include "Publics.h"


TimesEx::TimesEx(void)
{
}

bool TimesEx::year_is_leapyear(uint16_t year)
{
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
        return true;

    return false;
}

bool TimesEx::year_is_leapyear(uint8_t * p_bcd_year)
{
    uint16_t val_year = 0;
    val_year = Publics::bcds_to_val<uint16_t>(p_bcd_year, 2);

    return year_is_leapyear(val_year);
}

// BCD时间是否合法
bool TimesEx::bcd_time_valid(uint8_t * p_bcd_time, uint8_t time_format)
{
    if((time_format & T_YYYY) && memcmp(p_bcd_time, "\x00\x00", 2) == 0)
        return false;
    else if((time_format & T_MM) && (p_bcd_time[2] == 0 || p_bcd_time[2] > 0x12 || !Publics::byte_bcd_valid(p_bcd_time[2])))
        return false;
    else if((time_format & T_DD) && (p_bcd_time[3] == 0 || p_bcd_time[3] > month_max_day(p_bcd_time) || !Publics::byte_bcd_valid(p_bcd_time[3])))
        return false;
    else if((time_format & T_HH) && (p_bcd_time[4] >= 0x24 || !Publics::byte_bcd_valid(p_bcd_time[4])))
        return false;
    else if((time_format & T_MNMN) && (p_bcd_time[5] >= 0x60 || !Publics::byte_bcd_valid(p_bcd_time[5])))
        return false;
    else if((time_format & T_HH) && (p_bcd_time[6] >= 0x60 || !Publics::byte_bcd_valid(p_bcd_time[6])))
        return false;

    return true;
}

// BCD年月中每个月的最大天数
uint8_t TimesEx::month_max_day(uint8_t * p_bcd_yyyymm)
{
    int year = Publics::bcds_to_val<uint16_t>(p_bcd_yyyymm, 2);
    int month = Publics::bcd_to_val(p_bcd_yyyymm[2]);

    uint8_t	day = month_max_day(year, month);

    return Publics::val_to_bcd(day);
}

uint8_t TimesEx::month_max_day(uint16_t year, uint8_t month)
{
    static uint8_t days1[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static uint8_t days2[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (year_is_leapyear(year))
        return days2[month - 1];
    else
        return days1[month - 1];
}

// BCD与TM互转
void TimesEx::tm2_bcd4_exchange(uint8_t * p_tm2, uint8_t * p_bcd4, bool exchage_direction)
{
    uint16_t sub_month_2000_1;

	if (exchage_direction)
	{
		memset(p_bcd4, 0, 4);
		sub_month_2000_1 = (uint16_t)((p_tm2[0] << 2) + (p_tm2[1] >> 6));

		if (sub_month_2000_1 > 0)
		{
			Publics::val_to_bcds((uint16_t)(sub_month_2000_1 / 12 + 2000), p_bcd4, 2);
			p_bcd4[2] = Publics::val_to_bcd((uint8_t)(sub_month_2000_1 % 12 + 1));
			p_bcd4[3] = Publics::val_to_bcd((uint8_t)((p_tm2[1] >> 1) & 0x1F));
		}
	}
	else
	{
        uint16_t year;
        uint8_t month, day;

        memset(p_tm2, 0, 2);

        year = Publics::bcds_to_val<uint16_t>(p_bcd4, 2);
        month = Publics::bcd_to_val(p_bcd4[2]);
        day = Publics::bcd_to_val(p_bcd4[3]);

        sub_month_2000_1 = (year - 2000) * 12 + month - 1;
        p_tm2[0] = (uint8_t)(sub_month_2000_1 >> 2);
        p_tm2[1] = (uint8_t)(((sub_month_2000_1 & 0x0003) << 6) + (day << 1));

    }
}

void TimesEx::tm4_bcd7_exchange(uint8_t * p_tm4, uint8_t * p_bcd7, bool exchage_direction)
{
    uint16_t sub_month_2000_1;

    if (exchage_direction)
    {
        memset(p_bcd7, 0, 7);

        sub_month_2000_1 = (uint16_t)((p_tm4[0] << 2) + (p_tm4[1] >> 6));
		if (sub_month_2000_1 > 0)
		{
			Publics::val_to_bcds((uint16_t)(sub_month_2000_1 / 12 + 2000), p_bcd7, 2);
			p_bcd7[2] = Publics::val_to_bcd((sub_month_2000_1 % 12) + 1);
			p_bcd7[3] = Publics::val_to_bcd((p_tm4[1] >> 1) & 0x1F);
			p_bcd7[4] = Publics::val_to_bcd(((p_tm4[1] & 0x01) << 4) + (p_tm4[2] >> 4));
			p_bcd7[5] = Publics::val_to_bcd(((p_tm4[2] & 0x0F) << 2) + (p_tm4[3] >> 6));
			p_bcd7[6] = Publics::val_to_bcd(p_tm4[3] & 0x3F);
		}
	}
    else
    {
        uint16_t year;
        uint8_t month, day, hour, minute, second;

        memset(p_tm4, 0, 4);

        year = Publics::bcds_to_val<uint16_t>(p_bcd7, 2);
        month = Publics::bcd_to_val(p_bcd7[2]);
        day = Publics::bcd_to_val(p_bcd7[3]);
        hour = Publics::bcd_to_val(p_bcd7[4]);
        minute = Publics::bcd_to_val(p_bcd7[5]);
        second = Publics::bcd_to_val(p_bcd7[6]);

        sub_month_2000_1 = (year - 2000) * 12 + month - 1;
        p_tm4[0] = (uint8_t)(sub_month_2000_1 >> 2);
        p_tm4[1] = (uint8_t)(((sub_month_2000_1 & 0x0003) << 6) + (day << 1) + (hour >> 4));
        p_tm4[2] = (uint8_t)(((hour << 4) & 0xF0) + ((minute >> 2) & 0x0F));
        p_tm4[3] = (uint8_t)(((minute & 0x03) << 6) + second);
    }
}

// BCD时间加减
void TimesEx::bcd_time_calculate(uint8_t * p_bcd_time, uint8_t time_format, int days/* = 0*/, int hours/* = 0*/, int mins/* = 0*/, int secs/* = 0*/)
{
    int newYear, newMonth, newDay, newHour, newMin, newSec;
    int nMonthDay;

    newSec = newMin = newHour = newDay = newMonth = newYear = 0;

    if (time_format & T_SS)
    {
        newSec = Publics::bcd_to_val(p_bcd_time[6]) + secs;
    }
    if (time_format & T_MNMN)
    {
        newMin = Publics::bcd_to_val(p_bcd_time[5]) + mins + newSec / 60;
        newSec %= 60;
        if (newSec < 0)
        {
            newMin --;
            newSec += 60;
        }
    }
    if (time_format & T_HH)
    {
        newHour = Publics::bcd_to_val(p_bcd_time[4]) + hours + newMin / 60;
        newMin %= 60;
        if (newMin < 0)
        {
            newHour --;
            newMin += 60;
        }
    }

    if (time_format & T_DD)
    {
        newDay = Publics::bcd_to_val(p_bcd_time[3]) + days + newHour / 24;
        newHour %= 24;
        if (newHour < 0)
        {
            newDay --;
            newHour += 24;
        }
    }

    newMonth = Publics::bcd_to_val(p_bcd_time[2]);
    newYear = Publics::bcds_to_val<int>(p_bcd_time, 2);

    if (newDay > 0)
    {
        while (newDay > (nMonthDay = month_max_day(newYear, newMonth)))
        {
            newMonth ++;
            newDay -= nMonthDay;
            if (newMonth > 12)
            {
                newYear += newMonth / 12;
                newMonth %= 12;
            }
        }
    }
    else
    {
        while (newDay <= 0)
        {
            newMonth --;
            if (newMonth == 0)
            {
                newMonth = 12;
                newYear --;
            }
            nMonthDay = month_max_day(newYear, newMonth);
            newDay += nMonthDay;
        }
    }

    Publics::val_to_bcds(newYear, p_bcd_time, 2);
    p_bcd_time[2] = Publics::val_to_bcd((uint8_t)(newMonth));

    if (time_format & T_DD)
        p_bcd_time[3] = Publics::val_to_bcd((uint8_t)(newDay));
    if (time_format & T_HH)
        p_bcd_time[4] = Publics::val_to_bcd((uint8_t)(newHour));
    if (time_format & T_MNMN)
        p_bcd_time[5] = Publics::val_to_bcd((uint8_t)(newMin));
    if (time_format & T_SS)
        p_bcd_time[6] = Publics::val_to_bcd((uint8_t)(newSec));
}

// 比较BCD有效期
int TimesEx::CmpExpire(uint8_t * lpBcdNow, uint8_t * lpBcdExpire, uint8_t bFormat, short nDays, short nHours, short nMin, short nSec)
{
    int i;
    uint8_t bNewExpire[7] = {0};

    for (i=0; i<7; i++)
    {
        if (bFormat & (0x01 << i))
            bNewExpire[i] = lpBcdExpire[i];
        else
            break;
    }

    if ((bFormat & T_HH) == 0 && nHours > 0)
    {
        nDays += 1;
    }

    bcd_time_calculate(bNewExpire, T_TIME, nDays, nHours, nMin, nSec);

    return memcmp(lpBcdNow, bNewExpire, 7);
}

// 判断是否超时
bool TimesEx::timeout(uint8_t * p_bcd_time_target, uint8_t * p_bcd_time_src, int sub_in_minute)
{
    uint8_t time_src[7] = {0};

    memcpy(time_src, p_bcd_time_src, 7);
    bcd_time_calculate(time_src, T_TIME, 0, 0, sub_in_minute);

    if (memcmp(p_bcd_time_target, time_src, 7) > 0)
        return true;

    return false;
}

// 获取某个时间点所在的运营日
void TimesEx::TimeForWorkDay(uint8_t * pTimePoint, uint8_t * pSWorkDay/* = NULL*/, uint8_t * pEWorkDay/* = NULL*/)
{
    //BYTE bSWorkDay[7] = {0};
    //BYTE bEWorkDay[7] = {0};

    //memcpy(bSWorkDay, pTimePoint, 4);

    //memcpy(bEWorkDay, pTimePoint, 4);
    //TimeBcdCac(bEWorkDay, T_DATE, 1);
    //memcpy(bEWorkDay + 4, EXPIRE_DELAY_POINT, 3);

    //// 如果某个时间是0点到5点间，应同时属于两个运营日，否则只属于一个
    //if (memcmp(pTimePoint + 4, EXPIRE_DELAY_POINT, 3) <= 0)
    //{
    //	TimeBcdCac(bSWorkDay, T_DATE, -1);
    //}

    //if (pSWorkDay != NULL)
    //	memcpy(pSWorkDay, bSWorkDay, 7);
    //if (pEWorkDay != NULL)
    //	memcpy(pEWorkDay, bEWorkDay, 7);
}

// 时间pTimeSrc对于时间pTimeTarg的运营日的位置，小于0:运营日前，0:运营日中，大于0:运营日后
int TimesEx::TimeWorkDayCmp(uint8_t * pTimeSrc, uint8_t * pTimeTarg)
{
    int nRet			= 0;
    //uint8_t bSWorkDay[7]	= {0};
    //uint8_t bEWorkDay[7]	= {0};

    //TimeForWorkDay(pTimeTarg, bSWorkDay, bEWorkDay);
    //if (memcmp(pTimeSrc, bSWorkDay, 7) <= 0)
    //    nRet = -1;
    //else if (memcmp(pTimeSrc, bEWorkDay, 7) > 0)
    //    nRet = 1;

	return memcmp(pTimeSrc, pTimeTarg, 4);

    return nRet;
}

// 将BCD时间转换为tm时间
void TimesEx::time_bcd_to_tm(uint8_t * p_time_bcd, uint8_t bFormat, PTMSTRUCT p_tm)
{
	memset(p_tm, 0, sizeof(TMSTRUCT));

	if ((bFormat & T_YYYY) == T_YYYY)
		p_tm->tm_year = Publics::bcds_to_val<int>(p_time_bcd, 2);

	if ((bFormat & T_MM) == T_MM)
		p_tm->tm_mon = (p_time_bcd[2] / 0x10 * 0x0a) + (p_time_bcd[2] % 0x10);

	if ((bFormat & T_DD) == T_DD)
		p_tm->tm_mday = (p_time_bcd[3] / 0x10 * 0x0a) + (p_time_bcd[3] % 0x10);

	if ((bFormat & T_HH) == T_HH)
		p_tm->tm_hour = (p_time_bcd[4] / 0x10 * 0x0a) + (p_time_bcd[4] % 0x10);

	if ((bFormat & T_MNMN) == T_MNMN)
		p_tm->tm_min = (p_time_bcd[5] / 0x10 * 0x0a) + (p_time_bcd[5] % 0x10);

	if ((bFormat & T_SS) == T_SS)
		p_tm->tm_sec = (p_time_bcd[6] / 0x10 * 0x0a) + (p_time_bcd[6] % 0x10);
}

// 判断某个日期是星期几
uint8_t TimesEx::weekday_theday(uint8_t * p_date_bcd)
{
	TMSTRUCT tm;
	int sum_arr[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
	time_bcd_to_tm(p_date_bcd, T_DATE, &tm);
	
	int sum = sum_arr[tm.tm_mon - 1] + tm.tm_mday;
	if(tm.tm_year % 400==0 || (tm.tm_year % 4 == 0 && tm.tm_year % 100 != 0)) /*判断是不是闰年*/
	{
		if(tm.tm_mon > 2) /*如果是闰年且月份大于2,总天数应该加一天*/
			sum++;
	}

	int a=(tm.tm_year - 1) / 4 - (tm.tm_year - 1) / 100 + (tm.tm_year - 1) / 400;
	sum=(a * 366) + ((tm.tm_year - a - 1) * 365) + sum;
	
	return (uint8_t)(sum % 7);
}

// 补足年份中的高两位
void TimesEx::century_fill(uint8_t * p_time)
{
	if (bcd_time_valid(p_time, T_MM | T_DD))
	{
		if (p_time[1] < 0x80)
			p_time[0] = 0x20;
		else
			p_time[0] = 0x19;
	}
}
