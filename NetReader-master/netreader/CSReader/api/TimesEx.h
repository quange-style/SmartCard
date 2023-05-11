#pragma once

#include <time.h>
#include <stdint.h>

typedef struct tm TMSTRUCT, * PTMSTRUCT;

class TimesEx
{
public:
    TimesEx(void);

    // �Ƿ�����
    static bool year_is_leapyear(uint16_t year);

    // �Ƿ�����,pYearΪBCD��ʽ
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
    // BCDʱ���Ƿ�Ϸ�
    static bool bcd_time_valid(uint8_t * p_bcd_time, uint8_t time_format);

    // BCD������ÿ���µ��������
    static uint8_t month_max_day(uint8_t * p_bcd_yyyymm);
    static uint8_t month_max_day(uint16_t year, uint8_t month);

    // BCD��TM��ת
    static void tm2_bcd4_exchange(uint8_t * p_tm2, uint8_t * p_bcd4, bool exchage_direction);
    static void tm4_bcd7_exchange(uint8_t * p_tm4, uint8_t * p_bcd7, bool exchage_direction);
    static void TM_BCD(uint8_t * lpTm, int lenTm, uint8_t * lpBCD, int lenBCD, bool bForuint16_t);

    // BCDʱ��Ӽ�
    static void bcd_time_calculate(uint8_t * p_bcd_time, uint8_t time_format, int days = 0, int hours = 0, int mins = 0, int secs = 0);

    // �Ƚ�BCD��Ч��
    static int CmpExpire(uint8_t * lpBcdNow, uint8_t * lpBcdExpire, uint8_t bFormat, short nDays = 0, short nHours = 0, short nMin = 0, short nSec = 0);

    // �ж��Ƿ�ʱ
    static bool timeout(uint8_t * p_bcd_time_target, uint8_t * p_bcd_time_src, int sub_in_minute);

    // ��ȡĳ��ʱ������ڵ���Ӫ��
    static void TimeForWorkDay(uint8_t * pTimePoint, uint8_t * pSWorkDay = NULL, uint8_t * pEWorkDay = NULL);

    // ʱ��pTimeSrc����ʱ��pTimeTarg����Ӫ�յ�λ�ã�С��0:��Ӫ��ǰ��0:��Ӫ���У�����0:��Ӫ�պ�
    static int TimeWorkDayCmp(uint8_t * pTimeSrc, uint8_t * pTimeTarg);

	// ��BCDʱ��ת��Ϊtmʱ��
	static void time_bcd_to_tm(uint8_t * p_time_bcd, uint8_t bFormat, PTMSTRUCT p_tm);

	// �ж�ĳ�����������ڼ�
	static uint8_t weekday_theday(uint8_t * p_date_bcd);

	// ��������еĸ���λ
	static void century_fill(uint8_t * p_time);
};

