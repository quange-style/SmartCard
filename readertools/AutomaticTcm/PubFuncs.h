//********************************************************************
// �ļ�����		: PubFuncs.h
// ����ʱ��		: 2011/06/02
// �޸�ʱ��		:
// �޸���		:
// �ļ�����		: ģ��ͷ�ļ�
// ����			: ��ƽ̨���컯�ӿڵ����·�װ������������ģ��������ͬ��
//				  ��ʽ�������ƹ��ܵĽӿڣ���Ҫ��Qnxϵͳ�Ĳ���API
//********************************************************************

#pragma once

// ascll�ַ�ת��ΪHEX�ֽڣ���'1'->0x01, 'a'->0x0a, 'A'->0x0a
BYTE Char2Hex(char ch);

// ��2�ֽ�charת��Ϊһ��BYTE��HEX
BYTE TwoChar2Hex(char * lpChar);

// �ַ���ת��ΪHEX���У���"123456789A"->"\x12\x34\x56\x78\x9A"
void String2HexSeq(char * lpString, size_t lenString, LPBYTE lpHex, size_t lenHex);

// ���ֽ�Hexת1�ֽ�ascll
char Hex2Char(BYTE bHex);

// 1�ֽ�Hexת2�ֽ�ascll
void Hex2TwoChar(BYTE bHex, char * pChar);

// HEX��ת��Ϊ�ַ���
void HexSeq2String(LPBYTE pHex, size_t lenHex, char * pString, size_t strMax);

// ���ֽ���ֵ��BCD����ֵת��
BYTE Hex_Ex_Bcd(BYTE bHex, bool btoBcd);

// �ж�һ��BYTEֵ�Ƿ�BCD��ʽ����hex��ʽ���Ƿ���a��b��c��d��e��f
bool ValidBcd(BYTE bValue);

// ���ַ���������Ҫ����Э���н����ֽ���ǰ���ֽ��ں�����
int ReverseOrder(char * pBuffer, int nLen, int nSizeUnit = 2);

// �滻�ַ����е�ָ���ַ��������ָ������
void TruncatChar(char cSrc, char cTarg, char * pSrc, char * pTarg, size_t len);

// ����Crc
WORD Crc16(LPBYTE lpData, DWORD nLen);

// ��ʼ��CRC32��
void InitCrcTable();

// ����CRC32
DWORD Crc32(BYTE * pSrcData, DWORD nLenData);

// ����CRC32�ķ�����Ҫ����У���ļ����߶��ļ��߼���
DWORD Crc32Reverse(DWORD dwInitCrc, BYTE * pSrcData, DWORD nLenData);

// У��CRC32
bool CheckCrc32(BYTE * pSrcData, DWORD nLenData, BYTE * pCrcTarg);

/////////////////////////////////////////////////////////////////////////////////////////////////
// 3DES�㷨��غ���
// 64bit���ݵ�64byte����
void _64_Bit2Byte(LPBYTE pBits64, LPBYTE pByte64);

// ���ݷ�ɢ�仯��������ԭʼ����
void _Disperse(LPBYTE pBuffer, const BYTE pRule[], int nLen);

// ���ݷ�ɢ�仯������ԭʼ����
void _Disperse(LPBYTE pTarg, LPBYTE pSrc, const BYTE pRule[], int nLen);

// 3DES
void _TripleDes(LPBYTE pDataSrc, LPBYTE pInitKey, bool flag);

// �Գ�����8���������ݽ��ж��3DES
void _TripleDesEx(LPBYTE pData, int nLenData, LPBYTE pKey, LPBYTE pFactor, LPBYTE pMac);
/////////////////////////////////////////////////////////////////////////////////////////////////

// д��־, bLogLevel:������־�ļ������������ڳ����弶��(����ʹ�������ļ�)��д��־������д
// ��־����,��ֵԽС����Խ��,д��־�μ�GSFile��ԱWriteLog
enum _LogLevel{
	Log_Special = 0,			// ��Щ���������Ҫ��¼����ʱ����Ԥ����ļ����Ƕ��٣���д��־
	Log_Err_Reader = 1,			// ͨѶ����
	Log_Err_Ticket = 2,			// Ʊ������
	Log_Err_Busness = 3,		// ҵ�񼶱�
	Log_Err_Interface = 4,		// API�ӿڼ���
};
void WriteLog(_LogLevel Level, char * fmt, ...);

/////////////////////////////////////////////////////////////////////////////////////////////////
// ʱ�亯��
// �Ƿ�����
bool LeapYear(int nYear);

// �Ƿ�����,pYearΪBCD��ʽ
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
// BCDʱ���Ƿ�Ϸ�
bool TimeValid(LPBYTE pBcdTime, BYTE bTimeFormat);

// ϵͳ��ʽ���ں�BCD���ڻ�ת����У�����ںϷ��ԣ�bForward=true\false��ʾ����\����ת��
void Date_Bcd_Ex_Sys(LPBYTE pBcd, SYSTEMTIME * pSys, bool bForward);

// ϵͳ��ʽʱ���BCDʱ�以ת����У��ʱ��Ϸ��ԣ�bForward=true\false��ʾ����\����ת��
void Time_Bcd_Ex_Sys(LPBYTE pBcd, SYSTEMTIME * pSys, bool bForward);

// BCD������ÿ���µ��������
BYTE MonthMaxDay(LPBYTE lpBcdMonth);
BYTE MonthMaxDay(int nYear, int nMonth);

// ��ȡ��ǰʱ���BCD��ʽ
void GetLocalTimeBcd(LPBYTE pBcdTime);

// BCD��TM��ת
void TM_BCD(LPBYTE lpTm, int lenTm, LPBYTE lpBCD, int lenBCD, bool bForword=true);

// BCDʱ��Ӽ�
void TimeBcdCac(LPBYTE lpBcdTime, BYTE bFormat, short nDays = 0, short nHours = 0, short nMin = 0, short nSec = 0);

// �Ƚ�BCD��Ч��
int CmpExpire(LPBYTE lpBcdNow, LPBYTE lpBcdExpire, BYTE bFormat, short nDays = 0, short nHours = 0, short nMin = 0, short nSec = 0);

// �ж��Ƿ�ʱ
bool TimeOut(LPBYTE pTimeNow, LPBYTE pTimeSrc, int nTimeInMin);

// ��ȡĳ��ʱ������ڵ���Ӫ��
void TimeForWorkDay(BYTE * pTimePoint, BYTE * pSWorkDay = NULL, BYTE * pEWorkDay = NULL);

// ʱ��pTimeSrc����ʱ��pTimeTarg����Ӫ�յ�λ�ã�С��0:��Ӫ��ǰ��0:��Ӫ���У�����0:��Ӫ�պ�
int TimeWorkDayCmp(BYTE * pTimeSrc, BYTE * pTimeTarg);
/////////////////////////////////////////////////////////////////////////////////////////////////
// ����ģ��

// �ۼӺ�
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

// ����
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

// �ַ���ת��ΪHex��ֵ����"789A"->"\x789A"��T1���Ϊ����������������ͳ���
template <typename T1>
void String2Hex(char * pString, int nlenString, T1& Hex)
{
	Hex = 0;
	for (int i=0;i<nlenString;i++)
	{
		Hex = (Hex << 4) + Char2Hex(pString[i]);
	}
};

// bytes����ת��Ϊһ����ֵ����ֵ�ĳ���Ϊ���������õ���󳤶ȣ�bOrder=true\false��ʾת������ֽ���ǰ\�ں�
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

// ����ֵת��ΪBCD���У���101->"\x01\x01"
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

// ����ֵת��ΪBCD���У���101->"\x0101"
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

// ��BCDת��Ϊ��ֵ����"\x0101"->101
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

// ����ֵת��ΪBCD���У���"\x01\x01"->101
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

// ȡһ����������λ�������Ҷ���
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

// ȡһ������ֵ��������֧�ֵ�����ͣ�һ��BYTE
template<typename T>
static BYTE ByteGet(T Value, int nByteNo)
{
	return (BYTE)((Value >> (nByteNo * 8)) & 0xFF);
}