#include <string.h>
#include "algorithm.h"

//Poly   : 0x04c11db7
//Init   : parameter, typically 0xffffffff

const unsigned long crc_table[256] = {
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


const unsigned long * get_crc_table()
{ return (const unsigned long *)crc_table;
}

#define CRC_DO1(buf) ncrc = crc_table[((int)ncrc ^ (*buf++)) & 0xff] ^ (ncrc >> 8);
#define CRC_DO2(buf)  CRC_DO1(buf); CRC_DO1(buf);
#define CRC_DO4(buf)  CRC_DO2(buf); CRC_DO2(buf);
#define CRC_DO8(buf)  CRC_DO4(buf); CRC_DO4(buf);

unsigned long ucrc32(unsigned long crc, const unsigned char *buf, unsigned int len)
{
	unsigned long ncrc=crc;
	unsigned int nLen = len;
	if (buf == NULL) return 0L;
 	ncrc = ncrc ^ 0xffffffffL;
 	while (nLen >= 8)  {CRC_DO8(buf); nLen -= 8;}
 	if (nLen) do {CRC_DO1(buf);} while (--nLen);
 	return ncrc ^ 0xffffffffL;
}

//==================================================================
//函数名称: generate_crc16
//实现功能: 对过半字查表计算crc16  值, 多项式为0x1021, 预置值为0
//入口参数:
//                      ptr - 要进行计算的数据缓冲首地址
//                      ltn - 要进行计算的数据长度
//返回参数:
//                      无
//返回结果: crc16 计算值
//=================================================================
unsigned int generate_crc16(unsigned char *ptr, unsigned int len)
{
	static unsigned short int const crc_ta[16]=
	{
		0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
		0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef
	};

    unsigned short int crc;
    unsigned char da;


    crc=0;
    while (len--!=0)
    {
        da=crc>>12;
        crc<<=4;
        crc^=crc_ta[da^(*ptr>>4)];

        da=crc>>12;
        crc<<=4;
        crc^=crc_ta[da^(*ptr&0x0f)];
        ptr++;
    }
    return(crc);
}


//==============================================================
//函数名称: generate_table_crc16
//实现功能: 通过查表法计算crc16  值, 多项式为0x1021, 预置值为0
//入口参数:
//                      ptr - 要进行计算的数据缓冲首地址
//                      ltn - 要进行计算的数据长度
//返回参数:
//                      无
//返回结果: crc16 计算值
//==============================================================
unsigned short int generate_table_crc16(unsigned char *ptr, unsigned int len)
{
	//static unsigned short int const wCRC16Table[256] =
	//{
	//	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	//	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	//	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
	//	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	//	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
	//	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	//	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	//	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	//	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	//	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
	//	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	//	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	//	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	//	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	//	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
	//	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	//	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	//	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	//	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	//	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
	//	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	//	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	//	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	//	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	//	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
	//	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	//	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	//	0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	//	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	//	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
	//	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	//	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
	//};

    unsigned short int wTemp = 0;
    unsigned short int wCRC = 0x0;

    unsigned int i,j;

    for (i = 0; i < len; i++)
    {
        for (j = 0; j < 8; j++)
        {
            wTemp = ((ptr[i] << j) & 0x80 ) ^ ((wCRC & 0x8000) >> 8);

            wCRC <<= 1;

            if (wTemp != 0)
            {
                wCRC ^= 0x1021;
            }
        }
    }


    return(wCRC);
}



//====================================================================
//
//====================================================================
unsigned char generate_crc8(unsigned char *ptr, unsigned int len, unsigned char initval)
{
//多项式0x8C
    unsigned char i;
    unsigned char crc=initval;
    while (len--)
    {

        for (i=1; i!=0; i<<=1)
        {
            if ((crc&1)!=0)
            {
                crc>>=1;
                crc^=0x8C;
            }
            else crc>>=1;
            if ((*ptr&i)!=0) crc^=0x8C;
        }
        ptr++;

    }
    return(crc);
}


void des(unsigned char *source, const unsigned char * inkey, int flg)
{
    unsigned char bufout[64] = {0};
    unsigned char kwork[56] = {0};
    unsigned char worka[48] = {0};
    unsigned char kn[48] = {0};
    unsigned char buffer[64] = {0};
    unsigned char key[64] = {0};
    unsigned char nbrofshift, temp1, temp2;
    int valindex;
    unsigned char i, j, k, iter;

    /* INITIALIZE THE TABLES */
    /* Table - s1 */
    const unsigned char s1[4][16] =
    {
        {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
        {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
        {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
        {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
    };

    /* Table - s2 */
    const unsigned char s2[4][16] =
    {
        {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
        {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
        {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
        {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 }
    };

    /* Table - s3 */
    const unsigned char s3[4][16] =
    {
        {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
        {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
        {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
        {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 }
    };

    /* Table - s4 */
    const unsigned char s4[4][16] =
    {
        {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
        {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
        {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
        {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 }
    };

    /* Table - s5 */
    const unsigned char s5[4][16] =
    {
        {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
        {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
        {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
        {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 }
    };

    /* Table - s6 */
    const unsigned char s6[4][16] =
    {
        {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
        {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
        {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
        {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 }
    };

    /* Table - s7 */
    const unsigned char s7[4][16] =
    {
        {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
        {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
        {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
        {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 }
    };

    /* Table - s8 */
    const unsigned char s8[4][16] =
    {
        {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
        {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
        {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
        {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 }
    };


    /* Table - Shift */
    const unsigned char shift[16] =
    {
        1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
    };


    /* Table - Binary */
    const unsigned char binary[64] =
    {
        0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1,
        0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1,
        1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1,
        1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1
    };

    // MAIN PROCESS
    // Convert from 64-bit key into 64-byte key
    /* MAIN PROCESS */
    /* Convert from 64-bit key into 64-byte key */
    for (i = 0; i < 8; i++)
    {
        key[8*i] = ((j = *(inkey + i)) >> 7) & 0x01;//((j = *(inkey + i)) >> 7) % 2;
        key[8*i+1] = (j >> 6 ) &0x01;
        key[8*i+2] = (j >> 5)&0x01;
        key[8*i+3] = (j >> 4) & 0x01;
        key[8*i+4] = (j >>3 ) & 0x01;
        key[8*i+5] = (j >>2 ) &0x01;
        key[8*i+6] = (j >>1 ) & 0x01;
        key[8*i+7] = j & 0x01;
    }

    /* Convert from 64-bit data into 64-byte data */
    for (i = 0; i < 8; i++)
    {
        buffer[8*i] = ((j = *(source + i)) / 128) % 2;
        buffer[8*i+1] = (j >> 6) & 0x01;
        buffer[8*i+2] = (j >> 5) & 0x01;
        buffer[8*i+3] = (j >> 4) & 0x01;
        buffer[8*i+4] = (j >> 3) & 0x01;
        buffer[8*i+5] = (j >> 2) & 0x01;
        buffer[8*i+6] = (j >> 1) & 0x01;
        buffer[8*i+7] = j & 0x01;//j % 2;
    }

    /* Initial Permutation of Data */
    bufout[ 0] = buffer[57];
    bufout[ 1] = buffer[49];
    bufout[ 2] = buffer[41];
    bufout[ 3] = buffer[33];
    bufout[ 4] = buffer[25];
    bufout[ 5] = buffer[17];
    bufout[ 6] = buffer[ 9];
    bufout[ 7] = buffer[ 1];
    bufout[ 8] = buffer[59];
    bufout[ 9] = buffer[51];
    bufout[10] = buffer[43];
    bufout[11] = buffer[35];
    bufout[12] = buffer[27];
    bufout[13] = buffer[19];
    bufout[14] = buffer[11];
    bufout[15] = buffer[ 3];
    bufout[16] = buffer[61];
    bufout[17] = buffer[53];
    bufout[18] = buffer[45];
    bufout[19] = buffer[37];
    bufout[20] = buffer[29];
    bufout[21] = buffer[21];
    bufout[22] = buffer[13];
    bufout[23] = buffer[ 5];
    bufout[24] = buffer[63];
    bufout[25] = buffer[55];
    bufout[26] = buffer[47];
    bufout[27] = buffer[39];
    bufout[28] = buffer[31];
    bufout[29] = buffer[23];
    bufout[30] = buffer[15];
    bufout[31] = buffer[ 7];
    bufout[32] = buffer[56];
    bufout[33] = buffer[48];
    bufout[34] = buffer[40];
    bufout[35] = buffer[32];
    bufout[36] = buffer[24];
    bufout[37] = buffer[16];
    bufout[38] = buffer[ 8];
    bufout[39] = buffer[ 0];
    bufout[40] = buffer[58];
    bufout[41] = buffer[50];
    bufout[42] = buffer[42];
    bufout[43] = buffer[34];
    bufout[44] = buffer[26];
    bufout[45] = buffer[18];
    bufout[46] = buffer[10];
    bufout[47] = buffer[ 2];
    bufout[48] = buffer[60];
    bufout[49] = buffer[52];
    bufout[50] = buffer[44];
    bufout[51] = buffer[36];
    bufout[52] = buffer[28];
    bufout[53] = buffer[20];
    bufout[54] = buffer[12];
    bufout[55] = buffer[ 4];
    bufout[56] = buffer[62];
    bufout[57] = buffer[54];
    bufout[58] = buffer[46];
    bufout[59] = buffer[38];
    bufout[60] = buffer[30];
    bufout[61] = buffer[22];
    bufout[62] = buffer[14];
    bufout[63] = buffer[ 6];

    /* Initial Permutation of Key */
    kwork[ 0] = key[56];
    kwork[ 1] = key[48];
    kwork[ 2] = key[40];
    kwork[ 3] = key[32];
    kwork[ 4] = key[24];
    kwork[ 5] = key[16];
    kwork[ 6] = key[ 8];
    kwork[ 7] = key[ 0];
    kwork[ 8] = key[57];
    kwork[ 9] = key[49];
    kwork[10] = key[41];
    kwork[11] = key[33];
    kwork[12] = key[25];
    kwork[13] = key[17];
    kwork[14] = key[ 9];
    kwork[15] = key[ 1];
    kwork[16] = key[58];
    kwork[17] = key[50];
    kwork[18] = key[42];
    kwork[19] = key[34];
    kwork[20] = key[26];
    kwork[21] = key[18];
    kwork[22] = key[10];
    kwork[23] = key[ 2];
    kwork[24] = key[59];
    kwork[25] = key[51];
    kwork[26] = key[43];
    kwork[27] = key[35];
    kwork[28] = key[62];
    kwork[29] = key[54];
    kwork[30] = key[46];
    kwork[31] = key[38];
    kwork[32] = key[30];
    kwork[33] = key[22];
    kwork[34] = key[14];
    kwork[35] = key[ 6];
    kwork[36] = key[61];
    kwork[37] = key[53];
    kwork[38] = key[45];
    kwork[39] = key[37];
    kwork[40] = key[29];
    kwork[41] = key[21];
    kwork[42] = key[13];
    kwork[43] = key[ 5];
    kwork[44] = key[60];
    kwork[45] = key[52];
    kwork[46] = key[44];
    kwork[47] = key[36];
    kwork[48] = key[28];
    kwork[49] = key[20];
    kwork[50] = key[12];
    kwork[51] = key[ 4];
    kwork[52] = key[27];
    kwork[53] = key[19];
    kwork[54] = key[11];
    kwork[55] = key[ 3];

    /* 16 Iterations */
    for (iter=1; iter<17; iter++)
    {
        for (i=0; i<32; i++)
            buffer[i] = bufout[32+i];

        /* Calculation of F(R, K) */
        /* Permute - E */
        worka[ 0] = buffer[31];
        worka[ 1] = buffer[ 0];
        worka[ 2] = buffer[ 1];
        worka[ 3] = buffer[ 2];
        worka[ 4] = buffer[ 3];
        worka[ 5] = buffer[ 4];
        worka[ 6] = buffer[ 3];
        worka[ 7] = buffer[ 4];
        worka[ 8] = buffer[ 5];
        worka[ 9] = buffer[ 6];
        worka[10] = buffer[ 7];
        worka[11] = buffer[ 8];
        worka[12] = buffer[ 7];
        worka[13] = buffer[ 8];
        worka[14] = buffer[ 9];
        worka[15] = buffer[10];
        worka[16] = buffer[11];
        worka[17] = buffer[12];
        worka[18] = buffer[11];
        worka[19] = buffer[12];
        worka[20] = buffer[13];
        worka[21] = buffer[14];
        worka[22] = buffer[15];
        worka[23] = buffer[16];
        worka[24] = buffer[15];
        worka[25] = buffer[16];
        worka[26] = buffer[17];
        worka[27] = buffer[18];
        worka[28] = buffer[19];
        worka[29] = buffer[20];
        worka[30] = buffer[19];
        worka[31] = buffer[20];
        worka[32] = buffer[21];
        worka[33] = buffer[22];
        worka[34] = buffer[23];
        worka[35] = buffer[24];
        worka[36] = buffer[23];
        worka[37] = buffer[24];
        worka[38] = buffer[25];
        worka[39] = buffer[26];
        worka[40] = buffer[27];
        worka[41] = buffer[28];
        worka[42] = buffer[27];
        worka[43] = buffer[28];
        worka[44] = buffer[29];
        worka[45] = buffer[30];
        worka[46] = buffer[31];
        worka[47] = buffer[ 0];

        /* KS Function Begin */
        if (flg)
        {
            nbrofshift = shift[iter-1];
            for (i=0; i < (int)nbrofshift; i++)
            {
                temp1 = kwork[0];
                temp2 = kwork[28];
                for (j=0; j<27; j++)
                {
                    kwork[j] = kwork[j+1];
                    kwork[j+28] = kwork[j+29];
                }
                kwork[27] = temp1;
                kwork[55] = temp2;
            }
        }
        else if (iter > 1)
        {
            nbrofshift = shift[17-iter];
            for (i=0; i<(int)nbrofshift; i++)
            {
                temp1 = kwork[27];
                temp2 = kwork[55];
                for (j=27; j>0; j--)
                {
                    kwork[j] = kwork[j-1];
                    kwork[j+28] = kwork[j+27];
                }
                kwork[0] = temp1;
                kwork[28] = temp2;
            }
        }

        /* Permute kwork - PC2 */
        kn[ 0] = kwork[13];
        kn[ 1] = kwork[16];
        kn[ 2] = kwork[10];
        kn[ 3] = kwork[23];
        kn[ 4] = kwork[ 0];
        kn[ 5] = kwork[ 4];
        kn[ 6] = kwork[ 2];
        kn[ 7] = kwork[27];
        kn[ 8] = kwork[14];
        kn[ 9] = kwork[ 5];
        kn[10] = kwork[20];
        kn[11] = kwork[ 9];
        kn[12] = kwork[22];
        kn[13] = kwork[18];
        kn[14] = kwork[11];
        kn[15] = kwork[ 3];
        kn[16] = kwork[25];
        kn[17] = kwork[ 7];
        kn[18] = kwork[15];
        kn[19] = kwork[ 6];
        kn[20] = kwork[26];
        kn[21] = kwork[19];
        kn[22] = kwork[12];
        kn[23] = kwork[ 1];
        kn[24] = kwork[40];
        kn[25] = kwork[51];
        kn[26] = kwork[30];
        kn[27] = kwork[36];
        kn[28] = kwork[46];
        kn[29] = kwork[54];
        kn[30] = kwork[29];
        kn[31] = kwork[39];
        kn[32] = kwork[50];
        kn[33] = kwork[44];
        kn[34] = kwork[32];
        kn[35] = kwork[47];
        kn[36] = kwork[43];
        kn[37] = kwork[48];
        kn[38] = kwork[38];
        kn[39] = kwork[55];
        kn[40] = kwork[33];
        kn[41] = kwork[52];
        kn[42] = kwork[45];
        kn[43] = kwork[41];
        kn[44] = kwork[49];
        kn[45] = kwork[35];
        kn[46] = kwork[28];
        kn[47] = kwork[31];
        /* KS Function End */

        /* worka XOR kn */
        for (i=0; i<48; i++)
            worka[i] = worka[i]^kn[i];

        /* 8 s-functions */
        valindex = s1[2*worka[0]+worka[5]][2*(2*(2*worka[1]+worka[2])+worka[3])+worka[4]];
        valindex = valindex*4;
        kn[0] = binary[0+valindex];
        kn[1] = binary[1+valindex];
        kn[2] = binary[2+valindex];
        kn[3] = binary[3+valindex];
        valindex = s2[2*worka[6]+worka[11]][2*(2*(2*worka[7]+worka[8])+worka[9])+worka[10]];
        valindex = valindex*4;
        kn[ 4] = binary[0+valindex];
        kn[ 5] = binary[1+valindex];
        kn[ 6] = binary[2+valindex];
        kn[ 7] = binary[3+valindex];
        valindex = s3[2*worka[12]+worka[17]][2*(2*(2*worka[13]+worka[14])+worka[15])+worka[16]];
        valindex = valindex*4;
        kn[ 8] = binary[0+valindex];
        kn[ 9] = binary[1+valindex];
        kn[10] = binary[2+valindex];
        kn[11] = binary[3+valindex];
        valindex = s4[2*worka[18]+worka[23]][2*(2*(2*worka[19]+worka[20])+worka[21])+worka[22]];
        valindex = valindex*4;
        kn[12] = binary[0+valindex];
        kn[13] = binary[1+valindex];
        kn[14] = binary[2+valindex];
        kn[15] = binary[3+valindex];
        valindex = s5[2*worka[24]+worka[29]][2*(2*(2*worka[25]+worka[26])+worka[27])+worka[28]];
        valindex = valindex * 4;
        kn[16] = binary[0+valindex];
        kn[17] = binary[1+valindex];
        kn[18] = binary[2+valindex];
        kn[19] = binary[3+valindex];
        valindex = s6[2*worka[30]+worka[35]][2*(2*(2*worka[31]+worka[32])+worka[33])+worka[34]];
        valindex = valindex*4;
        kn[20] = binary[0+valindex];
        kn[21] = binary[1+valindex];
        kn[22] = binary[2+valindex];
        kn[23] = binary[3+valindex];
        valindex = s7[2*worka[36]+worka[41]][2*(2*(2*worka[37]+worka[38])+worka[39])+worka[40]];
        valindex = valindex*4;
        kn[24] = binary[0+valindex];
        kn[25] = binary[1+valindex];
        kn[26] = binary[2+valindex];
        kn[27] = binary[3+valindex];
        valindex = s8[2*worka[42]+worka[47]][2*(2*(2*worka[43]+worka[44])+worka[45])+worka[46]];
        valindex = valindex*4;
        kn[28] = binary[0+valindex];
        kn[29] = binary[1+valindex];
        kn[30] = binary[2+valindex];
        kn[31] = binary[3+valindex];

        /* Permute - P */
        worka[ 0] = kn[15];
        worka[ 1] = kn[ 6];
        worka[ 2] = kn[19];
        worka[ 3] = kn[20];
        worka[ 4] = kn[28];
        worka[ 5] = kn[11];
        worka[ 6] = kn[27];
        worka[ 7] = kn[16];
        worka[ 8] = kn[ 0];
        worka[ 9] = kn[14];
        worka[10] = kn[22];
        worka[11] = kn[25];
        worka[12] = kn[ 4];
        worka[13] = kn[17];
        worka[14] = kn[30];
        worka[15] = kn[ 9];
        worka[16] = kn[ 1];
        worka[17] = kn[ 7];
        worka[18] = kn[23];
        worka[19] = kn[13];
        worka[20] = kn[31];
        worka[21] = kn[26];
        worka[22] = kn[ 2];
        worka[23] = kn[ 8];
        worka[24] = kn[18];
        worka[25] = kn[12];
        worka[26] = kn[29];
        worka[27] = kn[ 5];
        worka[28] = kn[21];
        worka[29] = kn[10];
        worka[30] = kn[ 3];
        worka[31] = kn[24];

        /* bufout XOR worka */
        for (i=0; i<32; i++)
        {
            bufout[i+32] = bufout[i]^worka[i];
            bufout[i] = buffer[i];
        }
    } /* End of Iter */

    /* Prepare Output */
    for (i = 0; i < 32; i++)
    {
        j = bufout[i];
        bufout[i] = bufout[32+i];
        bufout[32+i] = j;
    }

    /* Inverse Initial Permutation */
    buffer[ 0] = bufout[39];
    buffer[ 1] = bufout[ 7];
    buffer[ 2] = bufout[47];
    buffer[ 3] = bufout[15];
    buffer[ 4] = bufout[55];
    buffer[ 5] = bufout[23];
    buffer[ 6] = bufout[63];
    buffer[ 7] = bufout[31];
    buffer[ 8] = bufout[38];
    buffer[ 9] = bufout[ 6];
    buffer[10] = bufout[46];
    buffer[11] = bufout[14];
    buffer[12] = bufout[54];
    buffer[13] = bufout[22];
    buffer[14] = bufout[62];
    buffer[15] = bufout[30];
    buffer[16] = bufout[37];
    buffer[17] = bufout[ 5];
    buffer[18] = bufout[45];
    buffer[19] = bufout[13];
    buffer[20] = bufout[53];
    buffer[21] = bufout[21];
    buffer[22] = bufout[61];
    buffer[23] = bufout[29];
    buffer[24] = bufout[36];
    buffer[25] = bufout[ 4];
    buffer[26] = bufout[44];
    buffer[27] = bufout[12];
    buffer[28] = bufout[52];
    buffer[29] = bufout[20];
    buffer[30] = bufout[60];
    buffer[31] = bufout[28];
    buffer[32] = bufout[35];
    buffer[33] = bufout[ 3];
    buffer[34] = bufout[43];
    buffer[35] = bufout[11];
    buffer[36] = bufout[51];
    buffer[37] = bufout[19];
    buffer[38] = bufout[59];
    buffer[39] = bufout[27];
    buffer[40] = bufout[34];
    buffer[41] = bufout[ 2];
    buffer[42] = bufout[42];
    buffer[43] = bufout[10];
    buffer[44] = bufout[50];
    buffer[45] = bufout[18];
    buffer[46] = bufout[58];
    buffer[47] = bufout[26];
    buffer[48] = bufout[33];
    buffer[49] = bufout[ 1];
    buffer[50] = bufout[41];
    buffer[51] = bufout[ 9];
    buffer[52] = bufout[49];
    buffer[53] = bufout[17];
    buffer[54] = bufout[57];
    buffer[55] = bufout[25];
    buffer[56] = bufout[32];
    buffer[57] = bufout[ 0];
    buffer[58] = bufout[40];
    buffer[59] = bufout[ 8];
    buffer[60] = bufout[48];
    buffer[61] = bufout[16];
    buffer[62] = bufout[56];
    buffer[63] = bufout[24];

    j = 0;
    for (i=0; i<8; i++)
    {
        *(source + i) = 0x00;
        for (k=0; k<7; k++)
            *(source + i) = ((*(source + i)) + buffer[j+k]) * 2;
        *(source + i) = *(source + i) + buffer[j+7];
        j += 8;
    }
}

void TripleDes( unsigned char *pOut, unsigned char *pIn, unsigned char *pKey, unsigned char Type )
{

    unsigned char Key1[8] = { 0 };
    unsigned char Key2[8] = { 0 };
    unsigned char tmp1[8] = { 0 };
//	unsigned char tmp2[8] = { 0 };

    memcpy( Key1, pKey, 8 );
    memcpy( Key2, pKey + 8, 8 );
    memcpy(tmp1, pIn, 8);

    des(tmp1, Key1, Type);
    des(tmp1, Key2, !Type);
    des(tmp1, Key1, Type);

    memcpy(pOut, tmp1, 8);

}


//================================================================================
//	函数名:  MAC
//	功      能:  用输入数据计算输出8 字节的认证码
//	入口参数:
//				lpkey : 8 字节的密钥
//	                        nlen: 参与计算的数据长度
//	                  lpvector: 8 字节计算预置向量值
//	                     lpdata: 输入数据
//	出口参数:
//                         lpdata: 8 字节的计算结果
//   返回值
//                无
//================================================================================
void MAC(const unsigned char *lpkey, int nlen, const unsigned char *lpvector, const unsigned char *lpdata, unsigned char *lpresult)
{

    const unsigned char filldata[8]={0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char lstblkdata[8];
    unsigned char sztmpnot[8];
    int data_len=0;
    unsigned char fill_len=0;
    unsigned char block_len=0;
    unsigned char j,offset;


    data_len = nlen;
    fill_len = data_len % 8;
    block_len = data_len / 8;

    if (fill_len)
    {
        offset = block_len << 3;
        memcpy(lstblkdata, lpdata+offset, fill_len);
        offset = fill_len;
        fill_len = 8 - fill_len;
        memcpy(lstblkdata+offset, filldata, fill_len);
    }
    else
    {
        fill_len = 8;
        memcpy(lstblkdata, filldata, 8);

    }

    data_len += fill_len;
    block_len++;
    //	//dbg_formatvar("block_len=%d, data_len = %d",block_len,data_len);
    //	//dbg_dumpmemory("block_len",lpdata, nlen);

    offset = 0;
    memcpy( sztmpnot, lpvector, 8 );                                              /* 初始值付入 */
    do
    {

        if (block_len==1)
        {
            ////dbg_dumpmemory("data=",lstblkdata, 8);
            for ( j = 0; j < 8; j++ )
            {
                sztmpnot[j] ^= lstblkdata[j];									  /* 得到的结果和后续8 字节异或  */
            }
        }
        else
        {
            ////dbg_dumpmemory("data=",lpdata+offset, 8);
            for ( j = 0; j < 8; j++ )
            {
                sztmpnot[j] ^= lpdata[offset++];									  /* 得到的结果和后续8 字节异或  */
            }

        }


        des(sztmpnot, lpkey, ENCRYPT);

        block_len--;


    }
    while (block_len>0);

    memcpy(lpresult, sztmpnot, 8);

}


//===============================================================
//函数: MAC_3
//功能: 对数据项进行线路mac  计算
//入口参数:
//					  lpkey=密钥
//					   nlen=要进行计算的数据长度
//					 lpdata=要进行计算的数据内容
//出口参数:
//					lpresult=计算的4字节mac  数据
//返  回   值:
//					无
//===============================================================
void MAC_3(unsigned char *lpkey, int nlen, unsigned char *vector, const unsigned char *lpdata, unsigned char *lpresult )
{

    const unsigned char filldata[8]={0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char lstblkdata[8];
    unsigned char sztmpnot[8];
    int data_len=0;
    unsigned char fill_len=0;
    unsigned char block_len=0;
    unsigned char j,offset;


    data_len = nlen;
    fill_len = data_len % 8;
    block_len = data_len / 8;

    if (fill_len)
    {
        offset = block_len << 3;
        memcpy(lstblkdata, lpdata+offset, fill_len);
        offset = fill_len;
        fill_len = 8 - fill_len;
        memcpy(lstblkdata+offset, filldata, fill_len);
    }
    else
    {
        fill_len = 8;
        memcpy(lstblkdata, filldata, 8);

    }

    data_len += fill_len;
    block_len++;
    //	//dbg_formatvar("block_len=%d, data_len = %d",block_len,data_len);
    //	//dbg_dumpmemory("block_len",lpdata, nlen);

    offset = 0;
    memcpy( sztmpnot, vector, 8 );                                              /* 初始值付入 */
    do
    {

        if (block_len==1)
        {
            ////dbg_dumpmemory("data=",lstblkdata, 8);
            for ( j = 0; j < 8; j++ )
            {
                sztmpnot[j] ^= lstblkdata[j];									  /* 得到的结果和后续8 字节异或  */
            }
        }
        else
        {
            ////dbg_dumpmemory("data=",lpdata+offset, 8);
            for ( j = 0; j < 8; j++ )
            {
                sztmpnot[j] ^= lpdata[offset++];									  /* 得到的结果和后续8 字节异或  */
            }

        }


        des(sztmpnot, lpkey, ENCRYPT);

        block_len--;


    }
    while (block_len>0);

    des(sztmpnot, lpkey+8, DECRYPT);
    des(sztmpnot, lpkey, ENCRYPT);
    memcpy(lpresult, sztmpnot, 8);

}


