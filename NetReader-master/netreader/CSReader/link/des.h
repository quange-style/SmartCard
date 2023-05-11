#define ENCRYPT 0  //加密 

#define DECRYPT 1  //解密

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
void Des(unsigned char *InBuff,unsigned char *OutBuff,unsigned char *Inkey,unsigned char flag);
/*
函数       void DataDes(unsigned char *InBuff,unsigned short InData,unsigned char *OutBuff,unsigned char *Inkey,unsigned char flag)：
功能说明:  对数据进行DES加密
入参说明： 
		   InBuff:输入数据 包含LC数据
		   OutBuff:输出数据
 		   Inkey：密码 8个字节  
 		   flag：0 加密，1：解密
函数输出： data:加解密结果均放入其中	
*/															 
unsigned short DataDes(unsigned char *InBuff,unsigned short InData,unsigned char *OutBuff,unsigned char *Inkey,unsigned char flag);
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
unsigned short DataThreeDes(unsigned char *InBuff,unsigned short InData,unsigned char *OutBuff,unsigned char *Inkey,unsigned char flag);
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
unsigned char  Diversify( unsigned char Key[16], unsigned char* Data, unsigned char* Out, unsigned char Type );

unsigned char MAC(unsigned char *InBuff,unsigned short inlen,unsigned char *ChallengeBuff,unsigned char *Inkey,unsigned char *OutBuff);
/*
函数       void ThreeMAC(unsigned char *InBuff,unsigned char inlen,unsigned char *ChallengeBuff,unsigned char *Inkey,unsigned char flag)
：
功能说明: 计算MAC数据 密码为16个字节
入参说明： 
		   InBuff:输入数据  inlen 输入数据长度<=24
		   ChallengeBuff：随机数  8个字节 
 		   Inkey：密码 8个字节  
 		   OutBuff：输出MAC码 4个字节
		        
函数输出： 1:失败，0：成功 	
*/	
unsigned char ThreeMAC(unsigned char *InBuff,unsigned short inlen,unsigned char *ChallengeBuff,unsigned char *Inkey,unsigned char *OutBuff);
/*
*************************************************************************************************************
- 函数名称 : void ThreeDES(unsigned char *DoubleKeyStr,unsigned char *Data,unsigned char *Out)
- 函数说明 : 3DES算法函数
- 输入参数 : 
-           DoubleKeyStr: 16BYTE的加密密钥
-           Data:          8BYTE的明文数据
-           Out :          8BYTE的加密结果
	        Type  : 0-->加密  1-->解密
- 输出参数 : 无
*************************************************************************************************************
*/
void ThreeDES(unsigned char *DoubleKeyStr,unsigned char *Data,unsigned char *Out,unsigned char Type);

void ThreeDESWordData(unsigned char *DoubleKeyStr,unsigned char *Data,unsigned char *Out,unsigned char Type);
#endif

