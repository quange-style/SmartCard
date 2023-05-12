#pragma once

#include <stdint.h>

class DataSecurity
{
public:
    DataSecurity(void);


    // 计算Crc
    static uint16_t Crc16(uint8_t * lpData, uint16_t nLen);

    // 计算CRC32
    static uint32_t Crc32(uint8_t * pSrcData, uint32_t nLenData);

    // 计算CRC32的反，主要用于校验文件，边读文件边计算
    static uint32_t Crc32Reverse(uint32_t dwInitCrc, uint8_t * pSrcData, uint32_t nLenData);

    // 校验CRC32
    static bool CheckCrc32(uint8_t * pSrcData, uint32_t nLenData, uint8_t * pCrcTarg);

	// 校验CRC32
	static bool CheckCrc16(uint8_t * pSrcData, uint16_t nLenData, uint8_t * pCrcTarg);

    // 计算整个文件内容的CRC32校验
    static uint32_t crc32_of_file(char * p_path_file);

    // 文件末尾
    static bool crc32_in_file_end_valid(const char * p_path_file);
    /////////////////////////////////////////////////////////////////////////////////////////////////
    // 3DES算法相关函数
    // 64bit数据到64uint8_t数据
    static void _64_Bit2Bytes(uint8_t * pBits64, uint8_t * puint8_t64);

    // 数据分散变化，不保留原始数据
    static void _Disperse(uint8_t * pBuffer, const uint8_t pRule[], int nLen);

    // 数据分散变化，保留原始数据
    static void _Disperse(uint8_t * pTarg, uint8_t * pSrc, const uint8_t pRule[], int nLen);

    // 3DES
    static void _TripleDes(uint8_t * pDataSrc, uint8_t * pInitKey, bool flag);

    // 对长度是8倍数的数据进行多次3DES
    static void _TripleDesEx(uint8_t * pData, int nLenData, uint8_t * pKey, uint8_t * pFactor, uint8_t * pMac);

	static void _TripleDesMore(uint8_t * pDataSrc, uint8_t * pInitKey, bool flag, int len, uint8_t * pOutSrc);

};

