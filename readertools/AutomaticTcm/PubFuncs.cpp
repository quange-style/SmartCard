// PubFuncs.cpp

#include "stdafx.h"
#include "PubFuncs.h"

// ascll字符转换为HEX字节
BYTE Char2Hex(char ch)
{
#define SUB_CAPITAL			(BYTE)('A' - 0x0A)			// 大写字母字符与对应HEX的差值
#define SUB_SMALL			(BYTE)('a' - 0x0A)			// 小写字母字符与对应HEX的差值
#define SUB_NUMERIC			(BYTE)0x30					// 数字字符与对应的HEX值的差

	BYTE bHex = 0;
	if (ch >= '0' && ch <= '9')
		bHex = (BYTE)(ch - SUB_NUMERIC);
	else if (ch >= 'a' && ch <= 'z')
		bHex = (BYTE)(ch - SUB_SMALL);
	else if (ch >= 'A' && ch <= 'Z')
		bHex = (BYTE)(ch - SUB_CAPITAL);
	return bHex;

}

// 将2字节char转换为一个BYTE的HEX
BYTE TwoChar2Hex(char * lpChar)
{
	return ((Char2Hex(lpChar[0]) << 4) + Char2Hex(lpChar[1]));
}

// 字符串转换为HEX序列
void String2HexSeq(char * lpString, size_t lenString, LPBYTE lpHex, size_t lenHex)
{
	size_t nPosStr, nPosHex;

	nPosHex = nPosStr = 0;

	if (lenString > lenHex * 2)
		lenString = lenHex * 2;

	if (lenString % 2 != 0)
	{
		lpHex[nPosHex++] = Char2Hex(lpString[nPosStr++]);
	}

	for (;nPosStr<lenString;nPosStr+=2,nPosHex++)
	{
		lpHex[nPosHex] = (Char2Hex(lpString[nPosStr])<<4) + Char2Hex(lpString[nPosStr+1]);
	}

}

// 半字节Hex转1字节ascll
char Hex2Char(BYTE bHex)
{
	if (bHex >= 0 && bHex <= 9)
		return (char)(bHex + '0');
	else if (bHex >= 0x0A && bHex <= 0x0F)
		return (char)(bHex + 'A' - 0x0A);
	else
		return (char)(bHex);
}

// 1字节Hex转2字节ascll
void Hex2TwoChar(BYTE bHex, char * pChar)
{
	pChar[0] = Hex2Char((bHex >> 4) & 0x0F);
	pChar[1] = Hex2Char(bHex & 0x0F);
}

// HEX串转换为字符串
void HexSeq2String(LPBYTE pHex, size_t lenHex, char * pString, size_t strMax)
{
	size_t len;
	if (strMax < lenHex * 2)
		len = strMax / 2;
	else
		len = lenHex;

	for (size_t i=0;i<len;i++)
	{
		Hex2TwoChar(pHex[i], pString + 2 * i);
	}
}

// 单字节数值与BCD码数值转换
BYTE Hex_Ex_Bcd(BYTE bHex, bool btoBcd)
{
	if (btoBcd)
		return ((bHex % 0x0a) + bHex / 0x0a * 0x10);
	else
		return ((bHex % 0x10) + bHex / 0x10 * 0x0a);
}

// 判断一个BYTE值是否BCD形式，即hex形式中是否有a，b，c，d，e，f
bool ValidBcd(BYTE bValue)
{
	return ((bValue & 0xF0) < 0xA0 && (bValue & 0x0F) < 0x0A);
}

// 把字符串倒序，主要用于协议中金额低字节在前高字节在后的情况
int ReverseOrder(char * pBuffer, int nLen, int nSizeUnit/*=2*/)
{
	char szTemp[256]	= {0};

	if (nLen % nSizeUnit != 0 || nLen < nSizeUnit || nLen > sizeof(szTemp))
		return 1055;

	for (int i=0;i<nLen;i+=nSizeUnit)
	{
		memcpy(szTemp + i, pBuffer + nLen - i - nSizeUnit, nSizeUnit);
	}

	memcpy(pBuffer, szTemp, nLen);

	return 0;
}

// 替换字符串中的指定字符，输出到指定缓冲
void TruncatChar(char cSrc, char cTarg, char * pSrc, char * pTarg, size_t len)
{
	for (size_t i=0;i<len;i++)
	{
		if (pSrc[i] == cSrc)
			pTarg[i] = cTarg;
		else
			pTarg[i] = pSrc[i];
	}
}

// 计算Crc
const WORD CRC_TAB[] = {
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78 
};
WORD Crc16(LPBYTE lpData, DWORD nLen)
{
	WORD wCrc = 0xFFFF;
	for (DWORD n=0;n<nLen;n++)
	{
		wCrc = (wCrc >> 8) ^ CRC_TAB[(wCrc ^ lpData[n]) & 0xFF];
	}
	return (WORD)(wCrc^0xFFFF);
}

// 初始化CRC32表
// 初始化CRC32表
static DWORD g_CrcTable[256] =
{
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
};
//void InitCrcTable()
//{
//	DWORD c;  
//	DWORD i, j;  
//
//	for (i = 0; i < 256; i++) 
//	{  
//		c = i;  
//		for (j = 0; j < 8; j++) 
//		{  
//			if (c & 1)  
//				c = 0xEDB88320L ^ (c >> 1);  
//			else  
//				c = c >> 1;  
//		}  
//		g_CrcTable[i] = c;  
//	}  
//}

// 计算CRC32
DWORD Crc32(BYTE * pSrcData, DWORD nLenData)
{
	DWORD dwRet = 0xFFFFFFFF;

	for (DWORD i=0;i<nLenData;i++) 
	{
		dwRet = g_CrcTable[(dwRet ^ pSrcData[i]) & 0xFF] ^ (dwRet >> 8);
	}  
	return ~dwRet ;  
}

// 计算CRC32，主要用于校验文件
DWORD Crc32Reverse(DWORD dwInitCrc, BYTE * pSrcData, DWORD nLenData)
{
	for (DWORD i=0;i<nLenData;i++) 
	{  
		dwInitCrc = g_CrcTable[(dwInitCrc ^ pSrcData[i]) & 0xFF] ^ (dwInitCrc >> 8);  
	}  
	return dwInitCrc ;  
}

// 校验CRC32
bool CheckCrc32(BYTE * pSrcData, DWORD nLenData, BYTE * pCrcTarg)
{
	DWORD dwCrc = Crc32(pSrcData, nLenData);
	return (memcmp(&dwCrc, pCrcTarg, 4) == 0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// 3DES算法相关函数

// 64bit数据到64byte数据
void _64_Bit2Byte(LPBYTE pBits64, LPBYTE pByte64)
{
	int i, j, k;

	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			k = 8 - j - 1;
			pByte64[8 * i + j] = (pBits64[i] >> k) & 0x01;
		}
	}
}

// 数据分散变化，不保留原始数据
void _Disperse(LPBYTE pBuffer, const BYTE pRule[], int nLen)
{
	BYTE pTemp[64] = {0};

	memcpy(pTemp, pBuffer, nLen);
	
	for (int i=0;i<nLen;i++)
	{
		pBuffer[i] = pTemp[pRule[i]];
	}
}

// 数据分散变化，保留原始数据
void _Disperse(LPBYTE pTarg, LPBYTE pSrc, const BYTE pRule[], int nLen)
{
	for (int i=0;i<nLen;i++)
	{
		pTarg[i] = pSrc[pRule[i]];
	}
}

/* Table - s1～s8 */
const BYTE STable[8][64] = {
	14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7, 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
	4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13,

	15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10, 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
	0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15, 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9,

	10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8, 13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
	13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7, 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12,

	7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15, 13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
	10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4, 3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14,

	2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9, 14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
	4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14, 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3,

	12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11, 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
	9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6, 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13,

	4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1, 13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
	1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2, 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12,

	13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7, 1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
	7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8, 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11,
};
/* Table - Shift */
const BYTE shift_bits[16] = {
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 
};
/* Table - Binary */
const BYTE binary[64] = {
	0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1,	0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1,
	1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1,	1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1 
};
// 数据初始变换表
static const BYTE RuleD_Init[64] = 
{
	57, 49,41,33,25,17, 9, 1,59,51,43,35,27,19,11, 3,61,53,45,37,29,21,13, 5,63,55,47,39,31,23,15,7,
	56,48,40,32,24,16, 8, 0,58,50,42,34,26,18,10, 2,60,52,44,36,28,20,12, 4,62,54,46,38,30,22,14, 6
};
// 密钥初始变换表
static const BYTE RuleK_Init[56] = 
{
	56,48,40,32,24,16, 8, 0,57,49,41,33,25,17, 9, 1,58,50,42,34,26,18,10, 2,59,51,43,35,62,54,46,38,
	30,22,14, 6,61,53,45,37,29,21,13, 5,60,52,44,36,28,20,12, 4,27,19,11, 3
};
// E的n轮置换表
static const BYTE RuleE_n[48] =
{
	31, 0, 1, 2, 3, 4, 3, 4, 5, 6, 7, 8, 7, 8, 9,10,11,12,11,12,13,14,15,16,
	15,16,17,18,19,20,19,20,21,22,23,24,23,24,25,26,27,28,27,28,29,30,31, 0
};
// key的PC2置换表
static const BYTE RuleE_pc2[48] =
{
	13,16,10,23, 0, 4, 2,27,14, 5,20, 9,22,18,11, 3,25, 7,15, 6,26,19,12, 1,
	40,51,30,36,46,54,29,39,50,44,32,47,43,48,38,55,33,52,45,41,49,35,28,31
};
// key的P置换表
static const BYTE RuleE_p[32] =
{
	15, 6,19,20,28,11,27,16, 0,14,22,25, 4,17,30, 9, 1, 7,23,13,31,26, 2, 8,18,12,29, 5,21,10, 3,24
};
// 数据最后逆变换表
static const BYTE RuleD_Rv[64] = 
{
	39, 7,47,15,55,23,63,31,38, 6,46,14,54,22,62,30,37, 5,45,13,53,21,61,29,36, 4,44,12,52,20,60,28,
	35, 3,43,11,51,19,59,27,34, 2,42,10,50,18,58,26,33, 1,41, 9,49,17,57,25,32, 0,40, 8,48,16,56,24
};
// 3DES
void _TripleDes(LPBYTE pDataSrc, LPBYTE pInitKey, bool flag)
{
	BYTE Key_Ext[64]	= {0};
	BYTE Key_Work[56]	= {0};
	BYTE Key_n[48]		= {0};
	BYTE Data_Ext[64]	= {0};
	BYTE Data_Temp[64]	= {0};
	BYTE Data_Work[48]	= {0};
	BYTE bTemp[32]		= {0};
	BYTE temp1, temp2;
	int valindex;
	BYTE i, j, k, iter;

	// 数据和密钥都由64bit转换为64byte
	_64_Bit2Byte(pDataSrc, Data_Ext);
	_64_Bit2Byte(pInitKey, Key_Ext);

	// 数据和密钥进行初始置换
	_Disperse(Data_Temp, Data_Ext, RuleD_Init, sizeof(RuleD_Init));
	_Disperse(Key_Work, Key_Ext, RuleK_Init, sizeof(RuleK_Init));

	// 16轮置换
	for (iter=0; iter<16; iter++) 
	{
		memcpy(Data_Ext, Data_Temp + 32, 32);

		/* Calculation of F(R, K) */
		/* Permute - E */
		_Disperse(Data_Work, Data_Ext, RuleE_n, sizeof(RuleE_n));

		/* KS Function Begin */
		if (flag)
		{
			for (i=0;i<shift_bits[iter];i++) 
			{
				temp1 = Key_Work[0];
				temp2 = Key_Work[28];
				for (j=0; j<27; j++) 
				{
					Key_Work[j] = Key_Work[j+1];
					Key_Work[j+28] = Key_Work[j+29];
				}
				Key_Work[27] = temp1;
				Key_Work[55] = temp2;
			}
		} 
		else if (iter > 0)
		{
			for (i=0;i<shift_bits[16-iter];i++)
			{
				temp1 = Key_Work[27];
				temp2 = Key_Work[55];
				for (j=27; j>0; j--) 
				{
					Key_Work[j] = Key_Work[j-1];
					Key_Work[j+28] = Key_Work[j+27];
				}
				Key_Work[0] = temp1;
				Key_Work[28] = temp2;
			}
		}

		/* Permute Key_Work - PC2 */
		_Disperse(Key_n, Key_Work, RuleE_pc2, sizeof(RuleE_pc2));

		/* KS Function End */

		/* Data_Work XOR Key_n */
		for (i=0;i<48;i++)
			Data_Work[i] ^= Key_n[i];

		/* 8 s-functions */
		for (i=0;i<8;i++)
		{
			temp1 = 6 * i;
			temp2 = 6 * i + 1;
			valindex = (2 * Data_Work[temp1] + Data_Work[temp1 + 5]) * 16 + 2 * (2 * (2 * Data_Work[temp2] + Data_Work[temp2 + 1]) + Data_Work[temp2 + 2]) + Data_Work[temp2 + 3];
			valindex = STable[i][valindex] * 4;
			for(j=0;j<4;j++)
			{
				Key_n[4 * i + j] = binary[j + valindex];
			}
		}

		/* Permute - P */
		_Disperse(Data_Work, Key_n, RuleE_p, sizeof(RuleE_p));

		/* Data_Temp XOR Data_Work */
		for (i=0; i<32; i++) 
		{
			Data_Temp[i+32] = Data_Temp[i]^Data_Work[i];
			Data_Temp[i] = Data_Ext[i];
		}
	} /* End of Iter */

	/* Prepare Output */
	// 前32byte与后32字节交换
	memcpy(bTemp, Data_Temp, 32);
	memcpy(Data_Temp, Data_Temp + 32, 32);
	memcpy(Data_Temp + 32, bTemp, 32);

	/* Inverse Initial Permutation */
	_Disperse(Data_Ext, Data_Temp, RuleD_Rv, sizeof(RuleD_Rv));

	for (i=0,j=0;i<8;i++,j+=8)
	{
		pDataSrc[i] = 0x00;
		
		for(k=0;k<7;k++)
		{
			pDataSrc[i] = (pDataSrc[i] + Data_Ext[j+k]) * 2;
		}
		pDataSrc[i] = pDataSrc[i] + Data_Ext[j+7];
	}
}

// 对长度是8倍数的数据进行多次3DES
void _TripleDesEx(LPBYTE pData, int nLenData, LPBYTE pKey, LPBYTE pFactor, LPBYTE pMac)
{
	int i,j;									// 临时变量
	BYTE bAddData[8]	= {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	BYTE bAddLen		= (BYTE)(8 - nLenData % 8);

	//- 对数据补80 00 00 00...... -//
	memcpy(pData + nLenData, bAddData, bAddLen);
	nLenData += bAddLen;

	//- 8 字节密钥进行加密 -//
	memset(pMac, 0, 8);
	memcpy(pMac, pFactor, 8);			// 初始值付入
	for(i=0;i<nLenData/8;i++)
	{		
		for(j=0; j<8; j++)
		{
			//- 得到的结果和后续8 字节异或 -//
			pMac[j] ^= pData[i * 8 + j];
		}

		_TripleDes(pMac, pKey, true);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void WriteLog(_LogLevel Level, char * fmt, ...)
{
	//int cnt;
	//FILE *fp;
	//va_list argptr;
	//SYSTEMTIME tm;
	//char szPath[MAX_PATH]	= {0};
	//char szTemp[MAX_PATH]	= {0};

	//if (Level <= CurrentLevel)
	//{
	//	api_sprintf(szPath, "%s\\%s", CommMgr::GetRunningDir(), DIR_LOG_FILE);
	//	if(::_access(szPath, 0) == 0 || CreateDirectory(szPath, NULL)) 
	//	{
	//		GetLocalTime(&tm);
	//		api_sprintf(szTemp, "%s\\GS%04d%02d%02d.log", szPath, tm.wYear, tm.wMonth, tm.wDay);

	//		if (fopen_s(&fp, szTemp, "a+t") == 0)
	//		{
	//			memset(szTemp, 0, sizeof(szTemp));
	//			cnt = fprintf(fp,"[%02d:%02d:%02d:%03d] ", tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds);
	//			va_start(argptr,fmt);
	//			cnt = vfprintf(fp, fmt, argptr);
	//			va_end(argptr);
	//			fclose(fp);
	//		}
	//	}
	//}
}



/////////////////////////////////////////////////////////////////////////////////////////////////
// 时间函数

// 是否闰年
bool LeapYear(int nYear)
{
	if ((nYear % 4 == 0 && nYear % 100 != 0) || nYear % 400 == 0)
		return true;

	return false;
}

// 是否闰年,pYear为BCD形式
bool LeapYear(LPBYTE pYear)
{
	WORD wDecimalYear = (WORD)((Hex_Ex_Bcd(pYear[0], false) * 100) + Hex_Ex_Bcd(pYear[1], false));
	
	return LeapYear(wDecimalYear);
}

// BCD时间是否合法
bool TimeValid(LPBYTE pBcdTime, BYTE bTimeFormat)
{
	if((bTimeFormat & T_YYYY) && memcmp(pBcdTime, "\x00\x00", 2) == 0)
		return false;
	else if((bTimeFormat & T_MM) && (pBcdTime[2] == 0 || pBcdTime[2] > 0x12 || !ValidBcd(pBcdTime[2])))
		return false;
	else if((bTimeFormat & T_DD) && (pBcdTime[3] == 0 || pBcdTime[3] > MonthMaxDay(pBcdTime) || !ValidBcd(pBcdTime[3])))
		return false;
	else if((bTimeFormat & T_HH) && (pBcdTime[4] >= 0x24 || !ValidBcd(pBcdTime[4])))
		return false;
	else if((bTimeFormat & T_MNMN) && (pBcdTime[5] >= 0x60 || !ValidBcd(pBcdTime[5])))
		return false;
	else if((bTimeFormat & T_HH) && (pBcdTime[6] >= 0x60 || !ValidBcd(pBcdTime[6])))
		return false;
	
	return true;
}

// 系统格式日期和BCD日期互转，不校验日期合法性，bForward=true\false表示正向\逆向转换
void Date_Bcd_Ex_Sys(LPBYTE pBcd, SYSTEMTIME * pSys, bool bForward)
{
	if (bForward)
	{
		pSys->wYear		= (Hex_Ex_Bcd(pBcd[0], false) * 100) + Hex_Ex_Bcd(pBcd[1], false);
		pSys->wMonth	= Hex_Ex_Bcd(pBcd[2], false);
		pSys->wDay		= Hex_Ex_Bcd(pBcd[3], false);
	}
	else
	{
		pBcd[0] = Hex_Ex_Bcd((BYTE)(pSys->wYear / 100), true);
		pBcd[1] = Hex_Ex_Bcd((BYTE)(pSys->wYear % 100), true);
		pBcd[2] = Hex_Ex_Bcd((BYTE)pSys->wMonth, true);
		pBcd[3] = Hex_Ex_Bcd((BYTE)pSys->wDay, true);
	}
}

// 系统格式时间和BCD时间互转，不校验时间合法性，bForward=true\false表示正向\逆向转换
void Time_Bcd_Ex_Sys(LPBYTE pBcd, SYSTEMTIME * pSys, bool bForward)
{
	Date_Bcd_Ex_Sys(pBcd, pSys, bForward);

	if (bForward)
	{
		pSys->wHour		= Hex_Ex_Bcd(pBcd[4], false);
		pSys->wMinute	= Hex_Ex_Bcd(pBcd[5], false);
		pSys->wSecond	= Hex_Ex_Bcd(pBcd[6], false);
	}
	else
	{
		pBcd[4] = Hex_Ex_Bcd((BYTE)(pSys->wHour), true);
		pBcd[5] = Hex_Ex_Bcd((BYTE)(pSys->wMinute), true);
		pBcd[6] = Hex_Ex_Bcd((BYTE)(pSys->wSecond), true);
	}
}

// BCD年月中每个月的最大天数
BYTE MonthMaxDay(LPBYTE lpBcdMonth)
{
	static BYTE days1[] = {0x31, 0x28, 0x31, 0x30, 0x31, 0x30, 0x31, 0x31, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x30, 0x31};
	static BYTE days2[] = {0x31, 0x29, 0x31, 0x30, 0x31, 0x30, 0x31, 0x31, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x30, 0x31};

	if (LeapYear(lpBcdMonth))
		return days2[lpBcdMonth[2] - 1];
	else
		return days1[lpBcdMonth[2] - 1];
}

BYTE MonthMaxDay(int nYear, int nMonth)
{
	static BYTE days1[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	static BYTE days2[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	if (LeapYear(nYear))
		return days2[nMonth - 1];
	else
		return days1[nMonth - 1];
}

// 获取当前时间的BCD格式
void GetLocalTimeBcd(LPBYTE pBcdTime)
{
	SYSTEMTIME tm = {0};
	GetLocalTime(&tm);
	Time_Bcd_Ex_Sys(pBcdTime, &tm, false);
}

// BCD与TM互转
void TM_BCD(LPBYTE lpTm, int lenTm, LPBYTE lpBCD, int lenBCD, bool bForword/*=true*/)
{
	SYSTEMTIME tm = {0};
	if (bForword)
	{
		DWORD dwTemp;
		memset(lpBCD, 0, lenBCD);
		dwTemp = (lpTm[0] << 2) + ((lpTm[1] & 0xC0) >> 6);
		tm.wYear = (WORD)(dwTemp / 12 + 2000);
		if(dwTemp != 0)
			tm.wMonth = (BYTE)(dwTemp % 12) + 1;
		tm.wDay	= (lpTm[1] & 0x3E) >> 1;
		if (lenTm == 4 && lenBCD == 7)
		{
			tm.wHour = ((lpTm[1] & 0x01) << 4) + ((lpTm[2] & 0xF0) >> 4);
			tm.wMinute = ((lpTm[2] & 0x0F) << 2) + ((lpTm[3] & 0xC0) >> 6);
			tm.wSecond = lpTm[3] & 0x3F;
		}

		if (tm.wMonth > 0)
		{
			if (lenBCD <= 4)
				Date_Bcd_Ex_Sys(lpBCD, &tm, false);
			else
				Time_Bcd_Ex_Sys(lpBCD, &tm, false);
		}
	}
	else
	{
		memset(lpTm, 0, lenTm);
		if (lenTm <= 2)
			Date_Bcd_Ex_Sys(lpBCD, &tm, true);
		else
			Time_Bcd_Ex_Sys(lpBCD, &tm, true);

		tm.wYear %= 2000;

		if (tm.wMonth > 0)
		{
			lpTm[0]	= (tm.wYear * 12 + tm.wMonth -1) >> 2;
			lpTm[1]	= (((tm.wYear * 12 + tm.wMonth-1) & 0x0003) << 6) + (tm.wDay << 1) + (tm.wHour >> 4);
			if (lenTm == 4 && lenBCD == 7)
			{
				lpTm[2]	= ((tm.wHour & 0x0F) << 4) + ((tm.wMinute & 0x3C) >> 2);
				lpTm[3]	= ((tm.wMinute & 0x03) << 6) + tm.wSecond;
			}
		}
	}
}

// BCD时间加减
void TimeBcdCac(LPBYTE lpBcdTime, BYTE bFormat, short nDays, short nHours, short nMin, short nSec)
{
	int nNewYear, nNewMonth, nNewDay, nNewHour, nNewMinute, nNewSecond;
	int nMonthDay;

	nNewSecond = 0;
	nNewMinute = 0;
	nNewHour = 0;
	nNewDay = 0;
	nNewMonth = 0;
	nNewYear = 0;

	if (bFormat & T_SS)
	{
		nNewSecond = Hex_Ex_Bcd(lpBcdTime[6], false) + nSec;
	}
	if (bFormat & T_MNMN)
	{
		nNewMinute = Hex_Ex_Bcd(lpBcdTime[5], false) + nMin + nNewSecond / 60;
		nNewSecond %= 60;
		if (nNewSecond < 0)
		{
			nNewMinute --;
			nNewSecond += 60;
		}
	}
	if (bFormat & T_HH)
	{
		nNewHour = Hex_Ex_Bcd(lpBcdTime[4], false) + nHours + nNewMinute / 60;
		nNewMinute %= 60;
		if (nNewMinute < 0)
		{
			nNewHour --;
			nNewMinute += 60;
		}
	}

	if (bFormat & T_DD)
	{
		nNewDay = Hex_Ex_Bcd(lpBcdTime[3], false) + nDays + nNewHour / 24;
		nNewHour %= 24;
		if (nNewHour < 0)
		{
			nNewDay --;
			nNewHour += 24;
		}
	}

	nNewMonth = Hex_Ex_Bcd(lpBcdTime[2], false);
	nNewYear = Hex_Ex_Bcd(lpBcdTime[0], false) * 100 + Hex_Ex_Bcd(lpBcdTime[1], false);

	if (nNewDay > 0)
	{
		while (nNewDay > (nMonthDay = MonthMaxDay(nNewYear, nNewMonth)))
		{
			nNewMonth ++;
			nNewDay -= nMonthDay;
			if (nNewMonth > 12)	
			{
				nNewYear += nNewMonth / 12;
				nNewMonth %= 12;
			}
		}
	}
	else
	{
		while (nNewDay <= 0)
		{
			nNewMonth --;
			if (nNewMonth == 0)
			{
				nNewMonth = 12;
				nNewYear --;
			}
			nMonthDay = MonthMaxDay(nNewYear, nNewMonth);
			nNewDay += nMonthDay;
		}
	}

	lpBcdTime[0] = Hex_Ex_Bcd((BYTE)(nNewYear / 100), true);
	lpBcdTime[1] = Hex_Ex_Bcd((BYTE)(nNewYear % 100), true);
	lpBcdTime[2] = Hex_Ex_Bcd((BYTE)(nNewMonth), true);

	if (bFormat & T_DD)
		lpBcdTime[3] = Hex_Ex_Bcd((BYTE)(nNewDay), true);
	if (bFormat & T_HH)
		lpBcdTime[4] = Hex_Ex_Bcd((BYTE)(nNewHour), true);
	if (bFormat & T_MNMN)
		lpBcdTime[5] = Hex_Ex_Bcd((BYTE)(nNewMinute), true);
	if (bFormat & T_SS)
		lpBcdTime[6] = Hex_Ex_Bcd((BYTE)(nNewSecond), true);
}

// 比较BCD有效期
int CmpExpire(LPBYTE lpBcdNow, LPBYTE lpBcdExpire, BYTE bFormat, short nDays, short nHours, short nMin, short nSec)
{
	int i;
	BYTE bNewExpire[7] = {0};

	for (i=0;i<7;i++)
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

	TimeBcdCac(bNewExpire, T_TIME, nDays, nHours, nMin, nSec);

	return memcmp(lpBcdNow, bNewExpire, 7);
}

// 判断是否超时
bool TimeOut(LPBYTE pTimeNow, LPBYTE pTimeSrc, int nTimeInMin)
{
	BYTE bTimeSrc[7] = {0};

	memcpy(bTimeSrc, pTimeSrc, 7);
	TimeBcdCac(bTimeSrc, T_TIME, 0, 0, (short)nTimeInMin);

	if (memcmp(pTimeNow, bTimeSrc, 7) > 0)
		return true;

	return false;
}

// 获取某个时间点所在的运营日
void TimeForWorkDay(BYTE * pTimePoint, BYTE * pSWorkDay/* = NULL*/, BYTE * pEWorkDay/* = NULL*/)
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
int TimeWorkDayCmp(BYTE * pTimeSrc, BYTE * pTimeTarg)
{
	int nRet			= 0;
	BYTE bSWorkDay[7]	= {0};
	BYTE bEWorkDay[7]	= {0};

	TimeForWorkDay(pTimeTarg, bSWorkDay, bEWorkDay);
	if (memcmp(pTimeSrc, bSWorkDay, 7) <= 0)
		nRet = -1;
	else if (memcmp(pTimeSrc, bEWorkDay, 7) > 0)
		nRet = 1;

	return nRet;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
