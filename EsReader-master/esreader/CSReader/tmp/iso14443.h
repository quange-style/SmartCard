#ifndef __ISO14443_H__
#define __ISO14443_H__

#ifdef __cplusplus
extern "C"
{
#endif
extern int rfdev_init(void);

extern int antenna_switch(int idx);

extern char RequestA(unsigned char req_code, unsigned char *atq);

extern char ISO14443A_Init(void);

extern char ISO14443A_Request(unsigned char req_code, unsigned char *atq);

extern char  ISO14443A_Anticoll(unsigned char *cPsnr );

extern char ISO14443A_Select( unsigned char *cPsnr, unsigned char *_size );

extern char ISO14443A_Authentication( unsigned char auth_mode, unsigned char *snr, unsigned char *keys, unsigned char block );

extern char ISO14443A_Request_UL(unsigned char req_code, unsigned char *atq);

extern char ISO14443A_Anticoll_UL( unsigned char ucode, unsigned char *cPsnr );

extern char ISO14443A_Select_UL(unsigned char ucode, unsigned char *cPsnr, unsigned char *_size);

extern char ISO14443A_Read_UL(unsigned char addr,unsigned char *readdata);

extern char ISO14443A_Write_UL(unsigned char addr, unsigned char *writedata);

extern char ISO14443A_CWrite_UL(unsigned char addr, unsigned char *writedata);


extern unsigned char MifareProRst(unsigned char cParameter,unsigned char cpLength,unsigned char *cpOutMsg);

extern unsigned char MifareProCom(unsigned char cpLength,unsigned char *lpdata, unsigned short int *respone_sw);

#ifdef __cplusplus
}
#endif

#endif


