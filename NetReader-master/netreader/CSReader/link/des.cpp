#include <stdio.h>              /* printf */
#include "string.h"
#include "des.h"



/*unsigned char key[8];        //密码原码
unsigned char key_buff[64];  //64为二进制数据
////////////////////////
unsigned char data[8];
unsigned char data_buff[64];
unsigned char data_out[64];*/

void key_64(unsigned char *power_in,unsigned char *power_out);
void key_select(unsigned char *power_in);
void key_loop(unsigned char *power_in,unsigned char *power_out);
void data_64(unsigned char *data_in,unsigned char *data_out1);
void data_selsect(unsigned char *data_in);
void key_data_loop(unsigned char *key_buff,unsigned char* data_buff,unsigned char*data_out,unsigned char flag,unsigned char *data);
void des(unsigned char *key,unsigned char *data,unsigned char flag) ;

const unsigned char key_ip[56]={
56, 48, 40, 32, 24, 16,  8,
0, 57, 49, 41, 33, 25, 17,
9,  1, 58, 50, 42, 34, 26,
18, 10,  2, 59, 51, 43, 35,    //49
62, 54, 46, 38, 30, 22, 14,
6, 61, 53, 45, 37, 29, 21,
13,  5, 60, 52, 44, 36, 28,
20, 12,  4, 27, 19, 11, 3
};
const unsigned char key_ip_2[48]={
13,16,10,23,0,4,2,27,
14,5,20,9,22,18,11,3,
25,7,15,6,26,19,12,1,
40,51,30,36,46,54,29,39,
50,44,32,47,43,48,38,55,
33,52,45,41,49,35,28,31
};
const unsigned char  shift[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

const unsigned char  data_ip[64]={
57,49,41,33,25,17,9,1,
59,51,43,35,27,19,11,3,
61,53,45,37,29,21,13,5,
63,55,47,39,31,23,15,7,
56,48,40,32,24,16,8,0,
58,50,42,34,26,18,10,2,
60,52,44,36,28,20,12,4,
62,54,46,38,30,22,14,6
};
const unsigned char data_ip_1[48]={
31,0,1,2,3,4,3,4,
5,6,7,8,7,8,9,10,
11,12,11,12,13,14,15,16,
15,16,17,18,19,20,19,20,
21,22,23,24,23,24,25,26,
27,28,27,28,29,30,31,0
};
const unsigned char s1[4][16] = {
14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 };

/* Table - s2 */
const unsigned char s2[4][16] = {
15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 };

/* Table - s3 */
const unsigned char s3[4][16] = {
10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 };

/* Table - s4 */
const unsigned char s4[4][16] = {
7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 };

/* Table - s5 */
const unsigned char s5[4][16] = {
2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 };

/* Table - s6 */
const unsigned char s6[4][16] = {
12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 };

/* Table - s7 */
const unsigned char s7[4][16] = {
4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 };

/* Table - s8 */
const unsigned char s8[4][16] = {
13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 };
const unsigned char binary[64] = {
0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1,
0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1,
1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1,
1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1 };
const unsigned char data_ip_2[32]={
15,6,19,20,28,11,27,16,0,14,22,25,4,17,30,9,1,7,23,13,31,26,2,8,18,12,29,5,21,10,3,24
};
const unsigned char data_ip_3[64]={
  39,7,47,15,55,23,63,31,38,6,46,14,54,22,62,30,37,5,45,13,53,21,61,29,36,4,44,12,52,20,60,
  28,35,3,43,11,51,19,59,27,34,2,42,10,50,18,58,26,33,1,41,9,49,17,57,25,32,0,40,8,48,16,56,24
};
  
 
/*
函数       void des(unsigned char *key,unsigned char *data,unsigned char flag)：
功能说明:  DES 算法
入参说明： 
		   InBuff:输入数据
		   OutBuff:输出数据
 		   Inkey：密码 8个字节  
 		   flag：0 加密，1：解密
函数输出： data:加解密结果均放入其中	
*/															 
void Des(unsigned char *InBuff,unsigned char *OutBuff,unsigned char *Inkey,unsigned char flag)
{
		unsigned char key_buff[64];  //64为二进制数据
		unsigned char data_buff[64];
		unsigned char data_out[64];

		key_64(Inkey,key_buff);
		key_select(key_buff);  
		data_64(InBuff,data_buff);
		data_selsect(data_buff);
		memcpy(data_out,data_buff,64);
		key_data_loop(key_buff,data_buff,data_out,flag,OutBuff);
}
/*
函数       void DataDes(unsigned char *InBuff,unsigned short InData,unsigned char *OutBuff,unsigned char *Inkey,unsigned char flag)：
功能说明:  对数据进行DES加密
入参说明： 
		   InBuff:输入数据 包含LC数据
		   OutBuff:输出数据
 		   Inkey：密码 8个字节  
 		   flag：0 加密，1：解密
函数输出： data:加解密结果均放入其中
注意事项：InBuff和OutBuff最好不是同一个组数，
          是同一个数组，地址偏移量不能超过8，否则数据被修改	
*/															 
unsigned short DataDes(unsigned char *InBuff,unsigned short InData,unsigned char *OutBuff,unsigned char *Inkey,unsigned char flag)
{
		unsigned char DesBuff[8];
		unsigned char DesBuff2[8];
		unsigned short i;
		unsigned char  Ss;
		Ss=0;
		i=0;
		while(InData)
		{
			memset(DesBuff,0x00,8);
			if(InData>=8)
			{
				memcpy(DesBuff,InBuff,8);
				if(Ss)
				{
					memcpy(OutBuff,DesBuff2,8);
					OutBuff+=8;
					Ss=0;
				}
				InData-=8;
				InBuff+=8;
				Des(DesBuff,DesBuff2,Inkey,flag);
				Ss=1;
 				i++;
			}
			else
			{
			   memcpy(DesBuff,InBuff,InData);
			   if(Ss)
			   {
					memcpy(OutBuff,DesBuff2,8);
					OutBuff+=8;
					Ss=0;
			   }

			   DesBuff[InData]=0x80;
			   Des(DesBuff,OutBuff,Inkey,flag);
			   InData=0;
			   i++;
			}

		}
		if(Ss)
		{
			memcpy(OutBuff,DesBuff2,8);
 			Ss=0;
		}

		i<<=3;
		return (i);
}
/*
函数       void DataThreeDes(unsigned char *InBuff,unsigned short InData,unsigned char *OutBuff,unsigned char *Inkey,unsigned char flag)：
功能说明:  对数据进行DES加密
入参说明： 
		   InBuff:输入数据 包含LC数据
		   OutBuff:输出数据
 		   Inkey：密码 16个字节  
 		   flag：0 加密，1：解密
函数输出： data:加解密结果均放入其中
注意事项：InBuff和OutBuff最好不是同一个组数，
          是同一个数组，地址偏移量不能超过8，否则数据被修改	
*/															 
unsigned short DataThreeDes(unsigned char *InBuff,unsigned short InData,unsigned char *OutBuff,unsigned char *Inkey,unsigned char flag)
{
		unsigned char DesBuff[8];
		unsigned char DesBuff2[8];
		unsigned short i;
		unsigned char  Ss;
		Ss=0;
		i=0;
		while(InData)
		{
			memset(DesBuff,0x00,8);
			if(InData>=8)
			{
				memcpy(DesBuff,InBuff,8);
				InData-=8;
				InBuff+=8;
				Des(DesBuff,DesBuff2,Inkey,flag);
				Des(DesBuff2,DesBuff2,&Inkey[8],flag^0x01);
				Des(DesBuff2,DesBuff2,Inkey,flag);
				memcpy(&OutBuff[i*8],DesBuff2,8);
				//OutBuff+=8;
				i++;
			}
			else
			{
			   memcpy(DesBuff,InBuff,InData);
 			   DesBuff[InData]=0x80;
			   Des(DesBuff,DesBuff2,Inkey,flag);
			   Des(DesBuff2,DesBuff2,&Inkey[8],flag^0x01);
			   Des(DesBuff2,DesBuff2,Inkey,flag);
			   InData=0;
			   memcpy(&OutBuff[i*8],DesBuff2,8);
			   //OutBuff+=8;
			   i++;
			}

		}
		i<<=3;
		return (i);
}

/*----------------------------------------------------------------------------
  说明: 计算一字节串的分散／反分散计算结果，密钥长度为16字节
  参数: Key:  [I ]密钥( 16字节 )
        Data: [I ]字节串( 分散为 8字节; 反分散为16字节 )
        Ont:  [IO]结果字节串( 分散为16字节; 反分散为 8字节 )
        Type: [I ]运算方式   0: 分散   1: 反分散
  返回: 参考值    参考值说明
        0         成功
    非0       失败
  ----------------------------------------------------------------------------*/
unsigned char  Diversify( unsigned char* Key, unsigned char* Data, unsigned char* Out, unsigned char Type )
{
    unsigned char szNotData[9] = { 0 };                                                  /* 输入数据的反 */
    unsigned char i;                                                                    /* 临时变量 */

    if( Type == 0 )
    {
        for( i = 0; i < 8; i++ )
        {
            szNotData[i] = ~Data[i];
        }

        memset( Out, 0, 16 );
        ThreeDES( &Key[0], &Data[0], &Out[0], 0 );
        ThreeDES( &Key[0], &szNotData[0], &Out[8], 0 );
    }
    else
    {
        ThreeDES( &Key[0], &Data[0], &Out[0], 1 );
        ThreeDES( &Key[0], &Data[8], &szNotData[0], 1 );

        for( i = 0; i < 8; i++ )
        {
            if( szNotData[i] != ( unsigned char )~Out[i] ){ return 0x01; }
        }
    }

    return 0;

}
/*
函数       void MAC(unsigned char *InBuff,unsigned char inlen,unsigned char *ChallengeBuff,unsigned char *Inkey,unsigned char flag)
：
功能说明:  计算MAC数据 密码为8个字节
入参说明： 
		   InBuff:输入数据  inlen 输入数据长度<=24
		   ChallengeBuff：随机数  8个字节 
 		   Inkey：密码 8个字节  
 		   OutBuff：输出MAC码 4个字节
		        
函数输出： 1:失败，0：成功 	
*/	
unsigned char MAC(unsigned char *InBuff,unsigned short inlen,unsigned char *ChallengeBuff,unsigned char *Inkey,unsigned char *OutBuff)
{
	unsigned char DesBuff[8];
 	unsigned short i;
 	while(inlen)
	{
		memset(DesBuff,0x00,8);	
	   	if(inlen>8)
		{
			 memcpy(DesBuff,InBuff,8);
			 for(i=0;i<8;i++)ChallengeBuff[i]^=DesBuff[i];
 			 Des(ChallengeBuff,ChallengeBuff,Inkey,ENCRYPT);
 			 InBuff+=8;
			 inlen-=8;
 		}
		else if (inlen==8)
		{
			 memcpy(DesBuff,InBuff,8);
			 for(i=0;i<8;i++)ChallengeBuff[i]^=DesBuff[i];
 			 Des(ChallengeBuff,ChallengeBuff,Inkey,ENCRYPT);
 			 memset(DesBuff,0x00,8);
			 DesBuff[0]=0x80;
			 for(i=0;i<8;i++)ChallengeBuff[i]^=DesBuff[i];
			 Des(ChallengeBuff,ChallengeBuff,Inkey,ENCRYPT);
			 inlen=0;
 		}
		else
 		{
			memcpy(DesBuff,InBuff,inlen);
			DesBuff[inlen]=0x80;
			for(i=0;i<8;i++)ChallengeBuff[i]^=DesBuff[i];
			Des(ChallengeBuff,ChallengeBuff,Inkey,ENCRYPT);
			inlen=0;
 		}
	}
 	memcpy(OutBuff,ChallengeBuff,4);
	return 0;
}

/*
函数       void ThreeMAC(unsigned char *InBuff,unsigned char inlen,unsigned char *ChallengeBuff,unsigned char *Inkey,unsigned char flag)
：
功能说明: 计算MAC数据 密码为16个字节
入参说明： 
		   InBuff:输入数据  inlen 输入数据长度<=24
		   ChallengeBuff：随机数  8个字节 
 		   Inkey：密码 16个字节  
 		   OutBuff：输出MAC码 4个字节
		        
函数输出： 1:失败，0：成功 	
*/	
unsigned char ThreeMAC(unsigned char *InBuff,unsigned short inlen,unsigned char *ChallengeBuff,unsigned char *Inkey,unsigned char *OutBuff)
{
	unsigned char DesBuff[8];
 	unsigned short i;
 	while(inlen)
	{
		memset(DesBuff,0x00,8);	
	   	if(inlen>8)
		{
			 memcpy(DesBuff,InBuff,8);
			 for(i=0;i<8;i++)ChallengeBuff[i]^=DesBuff[i];
 			 Des(ChallengeBuff,ChallengeBuff,Inkey,ENCRYPT);
 			 InBuff+=8;
			 inlen-=8;
 		}
		else if (inlen==8)
		{
			 memcpy(DesBuff,InBuff,8);
			 for(i=0;i<8;i++)ChallengeBuff[i]^=DesBuff[i];
 			 Des(ChallengeBuff,ChallengeBuff,Inkey,ENCRYPT);
 			 memset(DesBuff,0x00,8);
			 DesBuff[0]=0x80;
			 for(i=0;i<8;i++)ChallengeBuff[i]^=DesBuff[i];
			 Des(ChallengeBuff,ChallengeBuff,Inkey,ENCRYPT);
			 inlen=0;
 		}
		else
 		{
			memcpy(DesBuff,InBuff,inlen);
			DesBuff[inlen]=0x80;
			for(i=0;i<8;i++)ChallengeBuff[i]^=DesBuff[i];
			Des(ChallengeBuff,ChallengeBuff,Inkey,ENCRYPT);
			inlen=0;
 		}
	}
	Des(ChallengeBuff,ChallengeBuff,&Inkey[8],DECRYPT);
	Des(ChallengeBuff,ChallengeBuff,Inkey,ENCRYPT);

	memcpy(OutBuff,ChallengeBuff,4);
	return 0;
}

/*
*************************************************************************************************************
- 函数名称 : void ThreeDES(unsigned char *DoubleKeyStr,unsigned char *Data,unsigned char *Out)
- 函数说明 : 3DES算法函数
- 输入参数 : 
-           DoubleKeyStr: 16unsigned char的加密密钥
-           Data:          8unsigned char的明文数据
-           Out :          8unsigned char的加密结果
	        Type  : 0-->加密  1-->解密
- 输出参数 : 无
*************************************************************************************************************
*/
void ThreeDES(unsigned char *DoubleKeyStr,unsigned char *Data,unsigned char *Out,unsigned char Type)//3DES算法
{
 	  unsigned char Buf1[8],Buf2[8]; 
	  if (Type)//解密
	  {
		  	Des(Data,Buf1,DoubleKeyStr,DECRYPT); 
		  	Des(Buf1,Buf2,DoubleKeyStr+8,ENCRYPT);
		  	Des(Buf2,Out ,DoubleKeyStr,DECRYPT); 
	  }
	  else    //加密
	  {
	  	  	Des(Data,Buf1,DoubleKeyStr,ENCRYPT); 
		  	Des(Buf1,Buf2,DoubleKeyStr+8,DECRYPT);
		  	Des(Buf2,Out ,DoubleKeyStr,ENCRYPT); 
 	  }
}			

/*
*************************************************************************************************************
- 函数名称 : void ThreeDESWordData(unsigned char *DoubleKeyStr,unsigned char *Data,unsigned char *Out)
- 函数说明 : 3DES算法函数
- 输入参数 : 
-           DoubleKeyStr: 16unsigned char的加密密钥
-           Data:          16unsigned char的明文数据
-           Out :          16unsigned char的加密结果
	        Type  : 0-->加密  1-->解密
- 输出参数 : 无
*************************************************************************************************************
*/
void ThreeDESWordData(unsigned char *DoubleKeyStr,unsigned char *Data,unsigned char *Out,unsigned char Type)//DES算法
{
	unsigned char OutTmp[16];
	  if (Type)//解密
	  {
		  	ThreeDES(DoubleKeyStr,Data,OutTmp,DECRYPT);
			memcpy(Out,OutTmp,8);
			ThreeDES(DoubleKeyStr,Data+8,OutTmp,DECRYPT);
			memcpy(Out+8,OutTmp,8);
			
	  }
	  else    //加密
	  {
	  	  	ThreeDES(DoubleKeyStr,Data,OutTmp,ENCRYPT);
			memcpy(Out,OutTmp,8);
			ThreeDES(DoubleKeyStr,Data+8,OutTmp,ENCRYPT);
			memcpy(Out+8,OutTmp,8);
 	  }
}			
/**********************************8
数据循环移位
************************************/
void key_data_loop(unsigned char *key_buff,unsigned char* data_buff,unsigned char*data_out,unsigned char flag,unsigned char *data)
{

  unsigned char i,y,x;
  unsigned char temp1,temp2;
 // unsigned char kwork[48];
  unsigned char right_data_buff[48];     
  unsigned char right_data_buff1[32];
 // unsigned char right_data_buff2[32];
  for(i=0;i<16;i++)// 总次数循环
  {
      for(y=0;y<32;y++)
       {
          data_buff[y]=data_out[32+y];    //shu ru wei data_out
       }
      
      if(flag)
     // if(0)             //解密
      {
        if(i)
        {
         for(y=0;y<shift[16-i];y++)   //密码迭位
         {
          temp1=key_buff[27];
          temp2=key_buff[55];
          for(x=27;x>0;x--)
          {
              key_buff[x]=key_buff[x-1];
              key_buff[x+28]=key_buff[x+27];
           }
           key_buff[0]=temp1;
           key_buff[28]=temp2;
          }
        }
      }
      else                     //加密
      {
      for(y=0;y<shift[i];y++)   //密码迭位
      {
          temp1=key_buff[0];
          temp2=key_buff[28];
          for(x=0;x<27;x++)
          {
              key_buff[x]=key_buff[x+1];
              key_buff[x+28]=key_buff[x+29];
          }
          key_buff[27]=temp1;
          key_buff[55]=temp2;
      }
      }
    /*  for(y=0;y<48;y++)
      {
             kwork[y]=key_buff[key_ip_2[y]];
      }  //密码的迭代 完成
        //数据的迭代  
      for(x=0;x<48;x++)    //32--->48
      {
            right_data_buff[i]=data_buff[32+data_ip_1[i]];
      }*/
      for(y=0;y<48;y++)
      {
         // kwork[y]=key_buff[key_ip_2[y]];  
          right_data_buff[y]=data_buff[data_ip_1[y]];
        //  right_data_buff[y]^=kwork[y];
          right_data_buff[y]^=key_buff[key_ip_2[y]];
      }
      temp1=s1[2*right_data_buff[0]+right_data_buff[5]][2*(2*(2*right_data_buff[ 1]+right_data_buff[2])+right_data_buff[3])+right_data_buff[4]];
      temp1 = temp1 * 4;    //?
      right_data_buff1[0]=binary[0+temp1];
      right_data_buff1[1]=binary[1+temp1];
      right_data_buff1[2]=binary[2+temp1];
      right_data_buff1[3]=binary[3+temp1];
      temp1=s2[2*right_data_buff[6]+right_data_buff[11]][2*(2*(2*right_data_buff[7]+right_data_buff[8])+right_data_buff[9])+right_data_buff[10]];
      temp1 = temp1 * 4;    //?
      right_data_buff1[4]=binary[0+temp1];
      right_data_buff1[5]=binary[1+temp1];
      right_data_buff1[6]=binary[2+temp1];
      right_data_buff1[7]=binary[3+temp1];
       temp1=s3[2*right_data_buff[12]+right_data_buff[17]][2*(2*(2*right_data_buff[13]+right_data_buff[14])+right_data_buff[15])+right_data_buff[16]];
      temp1 = temp1 * 4;    //?
      right_data_buff1[8]=binary[0+temp1];
      right_data_buff1[9]=binary[1+temp1];
      right_data_buff1[10]=binary[2+temp1];
      right_data_buff1[11]=binary[3+temp1];
      temp1=s4[2*right_data_buff[18]+right_data_buff[23]][2*(2*(2*right_data_buff[19]+right_data_buff[20])+right_data_buff[21])+right_data_buff[22]];
      temp1 = temp1 * 4;    //?
      right_data_buff1[12]=binary[0+temp1];
      right_data_buff1[13]=binary[1+temp1];
      right_data_buff1[14]=binary[2+temp1];
      right_data_buff1[15]=binary[3+temp1];
       temp1=s5[2*right_data_buff[24]+right_data_buff[29]][2*(2*(2*right_data_buff[25]+right_data_buff[26])+right_data_buff[27])+right_data_buff[28]];
      temp1 = temp1 * 4;    //?
      right_data_buff1[16]=binary[0+temp1];
      right_data_buff1[17]=binary[1+temp1];
      right_data_buff1[18]=binary[2+temp1];
      right_data_buff1[19]=binary[3+temp1];
       temp1=s6[2*right_data_buff[30]+right_data_buff[35]][2*(2*(2*right_data_buff[31]+right_data_buff[32])+right_data_buff[33])+right_data_buff[34]];
      temp1 = temp1 * 4;    //?
      right_data_buff1[20]=binary[0+temp1];
      right_data_buff1[21]=binary[1+temp1];
      right_data_buff1[22]=binary[2+temp1];
      right_data_buff1[23]=binary[3+temp1];
      temp1=s7[2*right_data_buff[36]+right_data_buff[ 41]][2*(2*(2*right_data_buff[ 37]+right_data_buff[ 38])+right_data_buff[ 39])+right_data_buff[ 40]];
      temp1 = temp1 * 4;    //?
      right_data_buff1[24]=binary[0+temp1];
      right_data_buff1[25]=binary[1+temp1];
      right_data_buff1[26]=binary[2+temp1];
      right_data_buff1[27]=binary[3+temp1];
       temp1=s8[2*right_data_buff[ 42]+right_data_buff[47]][2*(2*(2*right_data_buff[43]+right_data_buff[44])+right_data_buff[45])+right_data_buff[46]];
      temp1 = temp1 * 4;    //?
      right_data_buff1[28]=binary[0+temp1];
      right_data_buff1[29]=binary[1+temp1];
      right_data_buff1[30]=binary[2+temp1];
      right_data_buff1[31]=binary[3+temp1];
       for(y=0;y<32;y++)
       {
          //right_data_buff2[y]=right_data_buff1[data_ip_2[y]];
          //data_out[32+y]=data_out[y]^right_data_buff2[y];
          data_out[32+y]=data_out[y]^right_data_buff1[data_ip_2[y]];
          data_out[y]=data_buff[y];
       }
  }
    for (i = 0; i < 32; i++) {
    temp1 = data_out[i];
    data_out[i] = data_out[32+i];
    data_out[32+i] = temp1;}
    for(i=0;i<64;i++)
    {
        key_buff[i]=data_out[data_ip_3[i]];    //最近数据
    }
       //data,输出数据。密文
     x=0;
    memset(data,0,8);
    for(i=0;i<8;i++)
    {
      for(y=0;y<8;y++)
      {
          data[i]<<=1;
         if(key_buff[y+x])  data[i]|=1;
        }
        x+=8;
    }
}

/*********************************
把输入数据变为64位数据。
***********************************/
void key_64(unsigned char *power_in,unsigned char *power_out)
{
      unsigned char i,j;
      for(i=0;i<8;i++)
      {
          j=*(power_in+i);
          power_out[i*8]=(j>>7)%2;
          power_out[i*8+1]=(j>>6)%2;
          power_out[i*8+2]=(j>>5)%2;
          power_out[i*8+3]=(j>>4)%2;
          power_out[i*8+4]=(j>>3)%2;
          power_out[i*8+5]=(j>>2)%2;
          power_out[i*8+6]=(j>>1)%2;
          power_out[i*8+7]=j%2;
      }
}

/*******************************
选择对应的位，和按顺序排列密码,去处校验位
*******************************/
void key_select(unsigned char *power_in)
{
    unsigned char i;
    unsigned char power_out[64];
    for(i=0;i<56;i++)
    {
      power_out[i]=power_in[key_ip[i]];
    }
    memcpy(power_in,power_out,64);
}

/***********************************
把输入数据变为64位
************************************/
void data_64(unsigned char *data_in,unsigned char *data_out1)
{
      unsigned char i,j;
      for(i=0;i<8;i++)
      {
          j=*(data_in+i);
          data_out1[i*8]=(j>>7)%2;
          data_out1[i*8+1]=(j>>6)%2;
          data_out1[i*8+2]=(j>>5)%2;
          data_out1[i*8+3]=(j>>4)%2;
          data_out1[i*8+4]=(j>>3)%2;
          data_out1[i*8+5]=(j>>2)%2;
          data_out1[i*8+6]=(j>>1)%2;
          data_out1[i*8+7]=j%2;
      }
 
}
/*********************************
数据按规定顺序选择排列
*********************************/
void data_selsect(unsigned char *data_in)
{
      unsigned char i;
      unsigned char data_out1[64];
      for(i=0;i<64;i++)
      {
         data_out1[i]=data_in[data_ip[i]];
      }
       memcpy(data_in,data_out1,64);
}

