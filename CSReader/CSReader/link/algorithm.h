#pragma once

#define ENCRYPT				1
#define DECRYPT				0

#ifndef _ALGORITHM_
#define _ALGORITHM_

unsigned char generate_crc8(unsigned char *ptr, unsigned int len, unsigned char init);

unsigned int generate_crc16(unsigned char *ptr, unsigned int len);

void des(unsigned char *source, const unsigned char * inkey, int flg);

void TripleDes( unsigned char *pOut, unsigned char *pIn, unsigned char *pKey, unsigned char Type );

void MAC(const unsigned char *lpkey, int nlen, const unsigned char *lpvector, const unsigned char *lpdata, unsigned char *lpresult);

void MAC_3(unsigned char *lpkey, int nlen, unsigned char *vector, const unsigned char *lpdata, unsigned char *lpresult);


#endif

