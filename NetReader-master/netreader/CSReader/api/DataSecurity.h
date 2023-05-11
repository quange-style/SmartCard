#pragma once

#include <stdint.h>

class DataSecurity
{
public:
    DataSecurity(void);


    // ����Crc
    static uint16_t Crc16(uint8_t * lpData, uint16_t nLen);

    // ����CRC32
    static uint32_t Crc32(uint8_t * pSrcData, uint32_t nLenData);

    // ����CRC32�ķ�����Ҫ����У���ļ����߶��ļ��߼���
    static uint32_t Crc32Reverse(uint32_t dwInitCrc, uint8_t * pSrcData, uint32_t nLenData);

    // У��CRC32
    static bool CheckCrc32(uint8_t * pSrcData, uint32_t nLenData, uint8_t * pCrcTarg);

	// У��CRC32
	static bool CheckCrc16(uint8_t * pSrcData, uint16_t nLenData, uint8_t * pCrcTarg);

    // ���������ļ����ݵ�CRC32У��
    static uint32_t crc32_of_file(char * p_path_file);

    // �ļ�ĩβ
    static bool crc32_in_file_end_valid(const char * p_path_file);
    /////////////////////////////////////////////////////////////////////////////////////////////////
    // 3DES�㷨��غ���
    // 64bit���ݵ�64uint8_t����
    static void _64_Bit2Bytes(uint8_t * pBits64, uint8_t * puint8_t64);

    // ���ݷ�ɢ�仯��������ԭʼ����
    static void _Disperse(uint8_t * pBuffer, const uint8_t pRule[], int nLen);

    // ���ݷ�ɢ�仯������ԭʼ����
    static void _Disperse(uint8_t * pTarg, uint8_t * pSrc, const uint8_t pRule[], int nLen);

    // 3DES
    static void _TripleDes(uint8_t * pDataSrc, uint8_t * pInitKey, bool flag);

    // �Գ�����8���������ݽ��ж��3DES
    static void _TripleDesEx(uint8_t * pData, int nLenData, uint8_t * pKey, uint8_t * pFactor, uint8_t * pMac);

	static void _TripleDesMore(uint8_t * pDataSrc, uint8_t * pInitKey, bool flag, int len, uint8_t * pOutSrc);

};

