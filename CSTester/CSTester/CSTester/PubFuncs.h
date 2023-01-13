//********************************************************************
// 文件名称		: PubFuncs.h
// 创建时间		: 2011/06/02
// 修改时间		:
// 修改人		:
// 文件类型		: 模块头文件
// 描述			: 对平台差异化接口的重新封装，让其在其他模块能以相同的
//				  形式调用类似功能的接口，主要是Qnx系统的部分API
//********************************************************************

#pragma once

// ascll字符转换为HEX字节，如'1'->0x01, 'a'->0x0a, 'A'->0x0a
BYTE Char2Hex(char ch);

// 将2字节char转换为一个BYTE的HEX
BYTE TwoChar2Hex(char * lpChar);

// 字符串转换为HEX序列，如"123456789A"->"\x12\x34\x56\x78\x9A"
void String2HexSeq(char * lpString, size_t lenString, LPBYTE lpHex, size_t lenHex);

// 半字节Hex转1字节ascll
char Hex2Char(BYTE bHex);

// 1字节Hex转2字节ascll
void Hex2TwoChar(BYTE bHex, char * pChar);

// HEX串转换为字符串
void HexSeq2String(LPBYTE pHex, size_t lenHex, char * pString, size_t strMax);

// 单字节数值与BCD码数值转换
BYTE Hex_Ex_Bcd(BYTE bHex, bool btoBcd);

// 判断一个BYTE值是否BCD形式，即hex形式中是否有a，b，c，d，e，f
bool ValidBcd(BYTE bValue);

// 把字符串倒序，主要用于协议中金额低字节在前高字节在后的情况
int ReverseOrder(char * pBuffer, int nLen, int nSizeUnit = 2);

// 替换字符串中的指定字符，输出到指定缓冲
void TruncatChar(char cSrc, char cTarg, char * pSrc, char * pTarg, size_t len);

// 计算Crc
WORD Crc16(LPBYTE lpData, DWORD nLen);

// 初始化CRC32表
void InitCrcTable();

// 计算CRC32
DWORD Crc32(BYTE * pSrcData, DWORD nLenData);

// 计算CRC32的反，主要用于校验文件，边读文件边计算
DWORD Crc32Reverse(DWORD dwInitCrc, BYTE * pSrcData, DWORD nLenData);

// 校验CRC32
bool CheckCrc32(BYTE * pSrcData, DWORD nLenData, BYTE * pCrcTarg);

/////////////////////////////////////////////////////////////////////////////////////////////////
// 3DES算法相关函数
// 64bit数据到64byte数据
void _64_Bit2Byte(LPBYTE pBits64, LPBYTE pByte64);

// 数据分散变化，不保留原始数据
void _Disperse(LPBYTE pBuffer, const BYTE pRule[], int nLen);

// 数据分散变化，保留原始数据
void _Disperse(LPBYTE pTarg, LPBYTE pSrc, const BYTE pRule[], int nLen);

// 3DES
void _TripleDes(LPBYTE pDataSrc, LPBYTE pInitKey, bool flag);

// 对长度是8倍数的数据进行多次3DES
void _TripleDesEx(LPBYTE pData, int nLenData, LPBYTE pKey, LPBYTE pFactor, LPBYTE pMac);
/////////////////////////////////////////////////////////////////////////////////////////////////

// 写日志, bLogLevel:本次日志的级别，如果级别高于程序定义级别(可能使用配置文件)就写日志，否则不写
// 日志级别,数值越小级别越高,写日志参见GSFile成员WriteLog
enum _LogLevel{
	Log_Special = 0,			// 有些特殊情况需要纪录，此时不论预定义的级别是多少，都写日志
	Log_Err_Reader = 1,			// 通讯错误
	Log_Err_Ticket = 2,			// 票卡级别
	Log_Err_Busness = 3,		// 业务级别
	Log_Err_Interface = 4,		// API接口级别
};
void WriteLog(_LogLevel Level, char * fmt, ...);

/////////////////////////////////////////////////////////////////////////////////////////////////
// 时间函数
// 是否闰年
bool LeapYear(int nYear);

// 是否闰年,pYear为BCD形式
bool LeapYear(LPBYTE pYear);

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
bool TimeValid(LPBYTE pBcdTime, BYTE bTimeFormat);

// 系统格式日期和BCD日期互转，不校验日期合法性，bForward=true\false表示正向\逆向转换
void Date_Bcd_Ex_Sys(LPBYTE pBcd, SYSTEMTIME * pSys, bool bForward);

// 系统格式时间和BCD时间互转，不校验时间合法性，bForward=true\false表示正向\逆向转换
void Time_Bcd_Ex_Sys(LPBYTE pBcd, SYSTEMTIME * pSys, bool bForward);

// BCD年月中每个月的最大天数
BYTE MonthMaxDay(LPBYTE lpBcdMonth);
BYTE MonthMaxDay(int nYear, int nMonth);

// 获取当前时间的BCD格式
void GetLocalTimeBcd(LPBYTE pBcdTime);

// BCD与TM互转
void TM_BCD(LPBYTE lpTm, int lenTm, LPBYTE lpBCD, int lenBCD, bool bForword=true);

// BCD时间加减
void TimeBcdCac(LPBYTE lpBcdTime, BYTE bFormat, short nDays = 0, short nHours = 0, short nMin = 0, short nSec = 0);

// 比较BCD有效期
int CmpExpire(LPBYTE lpBcdNow, LPBYTE lpBcdExpire, BYTE bFormat, short nDays = 0, short nHours = 0, short nMin = 0, short nSec = 0);

// 判断是否超时
bool TimeOut(LPBYTE pTimeNow, LPBYTE pTimeSrc, int nTimeInMin);

// 获取某个时间点所在的运营日
void TimeForWorkDay(BYTE * pTimePoint, BYTE * pSWorkDay = NULL, BYTE * pEWorkDay = NULL);

// 时间pTimeSrc对于时间pTimeTarg的运营日的位置，小于0:运营日前，0:运营日中，大于0:运营日后
int TimeWorkDayCmp(BYTE * pTimeSrc, BYTE * pTimeTarg);
/////////////////////////////////////////////////////////////////////////////////////////////////
// 函数模板

// 累加和
template <typename T1>
T1 AddSum(T1 * pData)
{
	T1 retCheck	= 0;

	while (pData != NULL && *pData != 0)
	{
		retCheck += *pData;
		pData++;
	}

	return retCheck;
};

// 异或和
template <typename T1>
T1 CheckSum(T1 * pData, size_t nLen)
{
	T1 retCheck	= 0;

	for (size_t i=0;i<nLen;i++)
	{
		retCheck ^= *(pData + i);
	}

	return retCheck;
};

// 字符串转换为Hex数值，如"789A"->"\x789A"，T1最大为编译器最长的内置类型长度
template <typename T1>
void String2Hex(char * pString, int nlenString, T1& Hex)
{
	Hex = 0;
	for (int i=0;i<nlenString;i++)
	{
		Hex = (Hex << 4) + Char2Hex(pString[i]);
	}
};

// bytes序列转换为一个整值，整值的长度为编译器内置的最大长度，bOrder=true\false表示转换后高字节在前\在后
template <typename T1>
void HexSeq2Value(LPBYTE pHexSeq, int lenSeq, T1& tValue, bool bOrder)
{
	int size = (int)sizeof(T1);
	if (lenSeq < size) size = lenSeq;

	tValue = 0;
	if (bOrder)
	{
		for (int i=0;i<size;i++)
		{
			tValue = (tValue << 8) + pHexSeq[i];
		}
	}
	else
	{
		for (int i=size;i>0;i--)
		{
			tValue = (tValue << 8) + pHexSeq[i-1];
		}
	}
}

// 将数值转换为BCD序列，如101->"\x01\x01"
template <typename T1>
void Hex2BcdSeq(T1 tValue, LPBYTE pBcd, int lenBcd)
{
	BYTE bHex;
	int size = sizeof(T1);
	if (size > lenBcd)	size = lenBcd;

	for (int i=size;i>0;i--)
	{
		bHex = (BYTE)(tValue % 100);
		pBcd[i-1] = Hex_Ex_Bcd(bHex, true);
		tValue = tValue / 100;
	}
}

// 将数值转换为BCD序列，如101->"\x0101"
template <typename T1>
T1 Hex2Bcd(T1 tValue, int lenInDeci)
{
	T1 tRet		= 0;
	T1 tMulti	= 1;

	for (int i=0;i<lenInDeci;i++)
	{
		tRet += (tValue % 0x0a) * tMulti;
		tValue /= 0x0a;
		tMulti *= 0x10;
	}

	return tRet;
}

// 将BCD转换为数值，如"\x0101"->101
template <typename T1>
T1 Bcd2Hex(T1 tBcd, int lenInDeci)
{
	T1 tRet		= 0;
	T1 tMulti	= 1;

	for (int i=0;i<lenInDeci;i++)
	{
		tRet += (tBcd % 0x10) * tMulti;
		tBcd >>= 4;
		tMulti *= 0x0A;
	}

	return tRet;
}

// 将数值转换为BCD序列，如"\x01\x01"->101
template <typename T1>
void BcdSeq2Value(LPBYTE pBcd, int lenBcd, T1& tValue)
{
	int size = sizeof(T1);
	tValue = 0;
	if (lenBcd > size)	lenBcd = size;

	for (int i=0;i<size;i++)
	{
		tValue = (tValue * 100) + Hex_Ex_Bcd(pBcd[i], false);
	}
}

// 取一个数的连续位，并向右对齐
template<typename T>
static T BitGet(T Value, int bStart, int bEnd)
{
	int typesize = sizeof(T) * 8 - 1;

	if (bStart >= bEnd)
	{
		Value <<= (typesize - bStart);
		Value >>= typesize - bStart + bEnd;
	}
	else if (bEnd > bStart)
	{
		Value <<= (typesize - bEnd);
		Value >>= typesize - bEnd + bStart;
	}
	return Value;
}

// 取一个整型值（编译器支持的最长类型）一个BYTE
template<typename T>
static BYTE ByteGet(T Value, int nByteNo)
{
	return (BYTE)((Value >> (nByteNo * 8)) & 0xFF);
}