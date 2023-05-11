
#include <string.h>
#include "sam.h"
#include "iso14443.h"
#include "algorithm.h"
#include "public.h"
#include "globalvar.h"

//=====================================
//文件信息
//发行基本信息-- 主文件下
typedef struct{
	bcd issuecode[2];               //城市代码
	bcd citycode[2];                //城市代码
	bcd industrycode[2];            //行业代码
	unsigned char testflag;         //测试标记
	unsigned char rfu;              //保留数据
	unsigned char logicnumber[8];   //应用序列号
	unsigned char cardtype[2];      //卡类型
	bcd issuedate[4];//yyyymmdd         //发行日期
	unsigned char issuedevice[6];   //发行设备信息
	unsigned char  cardversion[2];  //卡版本号
	unsigned char effectivefrom[4]; //卡启用日期yyyymmdd
	unsigned char effectiveto[4];   //卡截至日期yyyymmdd
	unsigned char rfu2[2];          //保留
}TISSUE,*LPTISSUE;//40Byte

//公共应用基本数据文件-- ADF1目录下
typedef struct{
	bcd issuecode[2];               //发卡方代码
	bcd citycode[2];                //城市代码
	bcd industrycode[2];            //行业代码
	unsigned char rfu[2];           //保留数据
	bcd appstatus;                  //启用标识
	bcd appversion;                 //应用版本
	unsigned char testflag;         //测试标记
	unsigned char rfu2;             //
	unsigned char logicnumber[8];   //应用序列号
	unsigned char effectivefrom[4]; //应用启用日期yyyymmdd
	unsigned char effectiveto[4];   //应用截至日期yyyymmdd
	unsigned char rfu3[2];          //保留
}TPUBLICINF,*LPTPUBLICINF; //总长30 BYTE


//持卡人基本数据文件
typedef struct{

}TPERSONALIZED,*LPPERSONALIZED;

//应用控制文件
typedef struct{
	unsigned char saleflag;     //发售激活标记
	unsigned char deposit;      //发售押金
	unsigned char saledevice[6];   //发售设备信息
	unsigned char passmode;     //出入模式判断
	unsigned char entryline;    //可入线路
	unsigned char entrystation; //可入站点
	unsigned char exitline;     //可出线路
	unsigned char exitstation;  //可出站点
	unsigned char pursecaps[2];    //钱包上限
	unsigned char rfu[17];      //预留
}TAPPCTRL,*LPTAPPCTRL;
//文件信息结构
//==========================================

//=====================================================
//CPU卡密钥
typedef struct{
	//MF密钥
	unsigned char TK_MF[16];  //传输密钥
	unsigned char MF_MKEY[16];//主控密钥
	unsigned char MF_MAMK[16];//维护密钥
	//ADF1密钥
	unsigned char ADF1_TKEY[16];      //传输密钥
	unsigned char ADF1_MKEY[16];      //主控密钥
	unsigned char MLK_ADF1_0201[16];  //圈存交易密钥
	unsigned char MPK_ADF1_0101[16];  //消费交易密钥01
	unsigned char MPK_ADF1_0102[16];  //消费交易密钥02
	unsigned char MPK_ADF1_0103[16];  //消费交易密钥03
	unsigned char MPK_ADF1_0104[16];  //消费交易密钥04
	unsigned char MPK_ADF1_0105[16];  //消费交易密钥05
	unsigned char MTK_ADF1[16];       //TAC 密钥
	unsigned char MPUK_ADF1[16];      //解锁PIN 密钥
	unsigned char MRPK_ADF1[16];      //重装PIN 密钥
	unsigned char MAMK_ADF1_00[16];   //应用维护密钥01
	unsigned char MAMK_ADF1_01[16];   //应用维护密钥02
	unsigned char MAMK_ADF1_02[16];   //应用维护密钥03
	unsigned char MAMK_ADF1_03[16];   //应用维护密钥04
	unsigned char PIN_ADF1[3];       //密码
	//ADF2密钥
	unsigned char ADF2_TKEY[16];      //传输密钥
	unsigned char ADF2_MKEY[16];      //主控密钥
	unsigned char MLK_ADF2_0201[16];  //圈存交易密钥
	unsigned char MPK_ADF2_0101[16];  //消费交易密钥01
	unsigned char MPK_ADF2_0102[16];  //消费交易密钥02
	unsigned char MPK_ADF2_0103[16];  //消费交易密钥03
	unsigned char MPK_ADF2_0104[16];  //消费交易密钥04
	unsigned char MPK_ADF2_0105[16];  //消费交易密钥05
	unsigned char MTK_ADF2[16];       //TAC 密钥
	unsigned char MPUK_ADF2[16];      //解锁PIN 密钥
	unsigned char MRPK_ADF2[16];      //重装PIN 密钥
	unsigned char MAMK_ADF2_00[16];   //应用维护密钥01
	unsigned char MAMK_ADF2_01[16];   //应用维护密钥02
	unsigned char MAMK_ADF2_02[16];   //应用维护密钥03
	unsigned char MAMK_ADF2_03[16];   //应用维护密钥04
	unsigned char PIN_ADF2[3];       //密码
}TTICK_KEY,*LPTTICKK_KEY;


typedef struct{
	bcd issuecode[2];               //发卡方代码
	bcd citycode[2];                //城市代码
	bcd industrycode[2];            //行业代码	
	bcd testflag;                   //主应用和ADF1 应用共用
	unsigned char logicnumber[8];   //应用序列号主应用和ADF1 应用共用
	unsigned char cardtype[2];      //卡类型
	//主应用区
	bcd issuedate[4];//yyyymmdd         //发行日期
	unsigned char issuedevice[6];   //发行设备信息
	unsigned char  cardversion[2];  //卡版本号
	unsigned char effectivefrom[4]; //卡启用日期yyyymmdd
	unsigned char effectiveto[4];   //卡截至日期yyyymmdd
	//ADF1
	unsigned char appstatus;      //启用标识
	bcd appversion;  //卡版本号
	unsigned char appeffectivefrom[4]; //应用启用日期yyyymmdd
	unsigned char appeffectiveto[4];   //应用截至日期yyyymmdd	
	//应用控制文件
	unsigned char saleflag;     //发售激活标记
	unsigned char deposit;      //发售押金
	unsigned char saledevice[6];   //发售设备信息
	unsigned char passmode;     //出入模式判断
	unsigned char entryline;    //可入线路
	unsigned char entrystation; //可入站点
	unsigned char exitline;     //可出线路
	unsigned char exitstation;  //可出站点	
} TTICKETINF, *LPTTICKETINF;

typedef struct{
	unsigned char logicnumber[16];
	unsigned int purse;//
	TTICKETINF inf;
	TTICK_KEY key;
} TINPUT, *LPTINPUT;


TTICK_KEY l_key=
{
	//MF
	{"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"},
	{"\x88\x88\x88\x88\x88\x88\x88\x99\x99\x99\x99\x99\x99\x99\x99\x99"},
	{"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"},
	//ADF1
	{"\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"},
	{"\x01\x88\x88\x88\x88\x88\x88\x99\x99\x99\x99\x99\x12\x34\x56\x78"},
	{"\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x23\x45\x67\x81"},
	{"\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x34\x56\x78\x12"},
	{"\x1B\x2B\x1B\x2B\x1B\x2B\x1B\x2B\x1B\x2B\x1B\x2B\x45\x67\x81\x23"},
	{"\x1B\x3B\x1B\x3B\x1B\x3B\x1B\x3B\x1B\x3B\x1B\x3B\x56\x78\x12\x34"},
	{"\x1B\x4B\x1B\x4B\x1B\x4B\x1B\x4B\x1B\x4B\x1B\x4B\x67\x81\x23\x45"},
	{"\x1B\x5B\x1B\x5B\x1B\x5B\x1B\x5B\x1B\x5B\x1B\x5B\x78\x12\x34\x56"},
	{"\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x81\x23\x45\x67"},
	{"\x1C\x1D\x1C\x1D\x1C\x1D\x1C\x1D\x1C\x1D\x1C\x1D\x12\x34\x56\x78"},
	{"\x1C\x2D\x1C\x2D\x1C\x2D\x1C\x2D\x1C\x2D\x1C\x2D\x23\x45\x67\x81"},
	{"\x1C\x1E\x1C\x1E\x1C\x1E\x1C\x1E\x1C\x1E\x1C\x1E\x34\x56\x78\x12"},
	{"\x1C\x2E\x1C\x2E\x1C\x2E\x1C\x2E\x1C\x2E\x1C\x2E\x45\x67\x81\x23"},
	{"\x1C\x3E\x1C\x3E\x1C\x3E\x1C\x3E\x1C\x3E\x1C\x3E\x56\x78\x12\x34"},
	{"\x1C\x4E\x1C\x4E\x1C\x4E\x1C\x4E\x1C\x4E\x1C\x4E\x67\x81\x23\x45"},
	{"\x12\x34\x56"},
	//ADF2
	{"\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"},
	{"\x01\x88\x88\x88\x88\x88\x88\x99\x99\x99\x99\x99\x12\x34\x56\x78"},
	{"\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x1A\x23\x45\x67\x81"},
	{"\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x34\x56\x78\x12"},
	{"\x1B\x2B\x1B\x2B\x1B\x2B\x1B\x2B\x1B\x2B\x1B\x2B\x45\x67\x81\x23"},
	{"\x1B\x3B\x1B\x3B\x1B\x3B\x1B\x3B\x1B\x3B\x1B\x3B\x56\x78\x12\x34"},
	{"\x1B\x4B\x1B\x4B\x1B\x4B\x1B\x4B\x1B\x4B\x1B\x4B\x67\x81\x23\x45"},
	{"\x1B\x5B\x1B\x5B\x1B\x5B\x1B\x5B\x1B\x5B\x1B\x5B\x78\x12\x34\x56"},
	{"\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x1C\x81\x23\x45\x67"},
	{"\x1C\x1D\x1C\x1D\x1C\x1D\x1C\x1D\x1C\x1D\x1C\x1D\x12\x34\x56\x78"},
	{"\x1C\x2D\x1C\x2D\x1C\x2D\x1C\x2D\x1C\x2D\x1C\x2D\x23\x45\x67\x81"},
	{"\x1C\x1E\x1C\x1E\x1C\x1E\x1C\x1E\x1C\x1E\x1C\x1E\x34\x56\x78\x12"},
	{"\x1C\x2E\x1C\x2E\x1C\x2E\x1C\x2E\x1C\x2E\x1C\x2E\x45\x67\x81\x23"},
	{"\x1C\x3E\x1C\x3E\x1C\x3E\x1C\x3E\x1C\x3E\x1C\x3E\x56\x78\x12\x34"},
	{"\x1C\x4E\x1C\x4E\x1C\x4E\x1C\x4E\x1C\x4E\x1C\x4E\x67\x81\x23\x45"},
	{"\x12\x34\x56"}
};



unsigned char l_logicnumber[16]={0x90,0x00,0x00,0x00,0x10,0x00,0x00,0x10};

TINPUT g_input;

//============================================================
//函数名: issue_setvalue
//功能     :  票卡初始化数据的设置
//入口参数:
//                      lpdata: 按照TINPUT 结构排序的数据
//出口参数: 无
//返回值:      无
//============================================================
void issue_setvalue(void *lpdata)
{

//===调试时强制赋
  memcpy(g_input.logicnumber, l_logicnumber, 8);
  memcpy((unsigned char *)&g_input.key, (unsigned char *)&l_key, sizeof(l_key));

  //发行基本信息文件  
  memcpy(g_input.inf.issuecode, "\x53\x20", 2);//发卡方代码
  memcpy(g_input.inf.citycode, "\x41\x00", 2);//城市代码
  memcpy(g_input.inf.industrycode, "\x00\x00", 2);//城市代码
  g_input.inf.testflag = 0x01;				// 测试标记
  memcpy(g_input.logicnumber, "\x90\x00\x00\x00\x10\x00\x00\x10", 8); //逻辑卡号
  memcpy(g_input.inf.cardtype,"\x02\x00", 2);
  memcpy(g_input.inf.issuedate, "\x20\x13\x04\x16", 4);
  memcpy(g_input.inf.issuedevice, "\x11\x22\x33\x44\x55\x66", 6);
  memcpy(g_input.inf.cardversion, "\x00\x01", 2);
  
  
  memcpy(g_input.inf.effectivefrom, "\x20\x13\x06\x01", 4); //卡启用日期
  memcpy(g_input.inf.effectiveto, "\x20\x19\x12\x31", 4);   //卡有效日期

  //公共应用基本数据

  g_input.inf.appstatus=0x00;
  g_input.inf.appversion=0x01;
  
  memcpy(g_input.inf.appeffectivefrom, "\x20\x13\x06\x01", 4); //卡启用日期
  memcpy(g_input.inf.appeffectiveto, "\x20\x19\x12\x31", 4);   //卡有效日期  

//应用控制
  g_input.inf.saleflag = 0;	  //发售激活标记
  g_input.inf.deposit = 0;	  //发售押金
  memcpy(g_input.inf.saledevice, "\x00\x00\x00\x00\x00\x00", 6);	 //发售设备信息
  g_input.inf.passmode = 0;	  //出入模式判断
  g_input.inf.entryline = 0;	  //可入线路
  g_input.inf.entrystation = 0; //可入站点
  g_input.inf.exitline = 0;	  //可出线路
  g_input.inf.exitstation = 0;  //可出站点  

  //====================================
  //memcpy(&g_input, lpdata, sizeof(g_input));//正式使用时赋值
  
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

void MAC_3( const unsigned char *lpkey, int nlen, unsigned char *vector, const unsigned char *lpdata, unsigned char *lpresult )
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
	do{

		if (block_len==1)
		{
			////dbg_dumpmemory("data=",lstblkdata, 8);
			for( j = 0; j < 8; j++ )
			{
				sztmpnot[j] ^= lstblkdata[j];									  /* 得到的结果和后续8 字节异或  */
			}
		}
		else
		{
			////dbg_dumpmemory("data=",lpdata+offset, 8);
			for( j = 0; j < 8; j++ )
			{
				sztmpnot[j] ^= lpdata[offset++];									  /* 得到的结果和后续8 字节异或  */
			}
		
		}

		
        des(sztmpnot, lpkey, ENCRYPT);

		block_len--;
		

	}while (block_len>0);

	des(sztmpnot, lpkey+8, DECRYPT);
	des(sztmpnot, lpkey, ENCRYPT);
	memcpy(lpresult, sztmpnot, 8);	

}


//===============================================================
//函数: data_encrypt
//功能: 对数据项进行加密
//入口参数:
//                      nlen=要进行加密的数据长度
//                   lpdata=要进行加密的数据内容
//                    lpkey=密钥
//出口参数:
//                  lpdata=加密后的数据内容
//返  回   值:
//                  加密后的数据长度
//===============================================================
int data_encrypt(unsigned char nlen, unsigned char *lpdata, unsigned char *lpkey)
{
	const unsigned char filldata[8]={0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	unsigned char szdata[256];
	int data_len=0;
	unsigned char fill_len=0;
	unsigned char block_len=0;
	unsigned char i,offset;

	memset(szdata, 0x00, sizeof(szdata));
	szdata[0] = nlen;
	data_len = 1;
	memcpy(szdata+data_len, lpdata, nlen);
	data_len += nlen;

	fill_len = data_len % 8;
	block_len = data_len / 8; 
	if (fill_len)
	{
		fill_len = 8 - fill_len;
		//不为8  的整倍
		memcpy(szdata+data_len, filldata, fill_len);
		block_len++;
	
	}
	else
	{
		fill_len = 8;
		memcpy(szdata+data_len, filldata, 8);
	
	}
	data_len += fill_len;

	//dbg_dumpmemory("szdata=",szdata, data_len);

	
	for(i=0, offset=0; i<block_len; i++, offset+=8)
	{
		TripleDes(szdata+offset, szdata+offset, lpkey, ENCRYPT);
		
	}

	//dbg_dumpmemory("szdata result =",szdata, data_len);

	//data_len = block_len<<3;
	memcpy(lpdata, szdata, data_len); 

	return (data_len);
	
}


int apdu_getrandom(unsigned char *lpdata, int ngetlen)
{

    unsigned char ulen;
    unsigned char respone_len;
	unsigned char sztmp[256];	
    unsigned short sam_sw;
    int nresult = -2;
    

	memcpy(sztmp, "\x00\x84\x00\x00\x08", 5);
	sztmp[4]=ngetlen;
	ulen = 5;
	respone_len = MifareProCom(ulen, sztmp, &sam_sw);
	if ((respone_len==0) || (0x9000!=sam_sw))
	{
		nresult = -2;
	}
	memcpy(lpdata,"\x00\x00\x00\x00\x00\x00\x00\x00",8);

	memcpy(lpdata, sztmp, respone_len-2); 
	nresult = 0;
	return 0;

}

int apdu_exteralauth(unsigned char *lprandom, unsigned char *lpkey)
{

	unsigned char ulen;
	unsigned char respone_len;
	unsigned char sztmp[256];
	unsigned char szdata[8];
	unsigned short sam_sw;
	int nresult = -2;


	memcpy(szdata, lprandom, 8);
	
	
	TripleDes(szdata, szdata, lpkey, ENCRYPT);

	memcpy(sztmp, "\x00\x82\x00\x00\x08",5);
	ulen = 5;
	memcpy(sztmp+5, szdata, 8);
	ulen += 8;
	//dbg_dumpmemory("externalauth=",sztmp,ulen);
	respone_len = MifareProCom(ulen, sztmp, &sam_sw);
	//dbg_dumpmemory("externalauth=",sztmp,respone_len);
	if ((respone_len==0) || (0x9000!=sam_sw))
	{
		nresult = -2;
	}

	nresult = 0;
	return 0;


}



//删除票卡原有结构
int ticket_reset(void)
{
	unsigned char szdata[8];
	unsigned char szcmd[256];
	unsigned char respone_len;
	unsigned short sam_sw;
	unsigned char ulen;
	int nresult = -2;
	

	if (apdu_getrandom(szdata,8)==0)
	{
		if (apdu_exteralauth(szdata,g_input.key.MF_MKEY)==0)
		{
			nresult = 0;
		}
	}


	if (apdu_getrandom(szdata,4)==0)
	{
	
		memset(szcmd, 0x00, sizeof(szcmd));
		memcpy(szcmd, "\x84\x0E\x00\x00\x18",5);
		ulen = 5;
		memcpy(szcmd+ulen, "\xF0\x02\x91\xFF",4);
		ulen += 4;
		memcpy(szcmd+ulen, g_input.key.TK_MF, 16);
		ulen += 16;
		MAC_3(g_input.key.MF_MKEY, ulen, szdata, szcmd, szcmd+ulen);
		ulen += 4;
		//dbg_dumpmemory("delete mf =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("delete mf |=",szcmd,respone_len);
		
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -1;
		}

		if (0x9000==sam_sw)
		{
			nresult = 0;
		}
		
	}		



	return nresult;

}


int ticket_create_mf(void)
{
    unsigned char ulen;
    unsigned char respone_len;
	unsigned char sztmp[256];	
	unsigned char szcmd[256];
	unsigned char szdata[8];
	unsigned char tmp_len;
    unsigned short sam_sw;
	LPTPUBLICINF lppublic;
	LPTISSUE lpmfissueinf;
    int nresult = 0;

	int i;

	do{

		//认证传输密钥
		if (apdu_getrandom(szdata,8)==0)
		{
			if (apdu_exteralauth(szdata,g_input.key.TK_MF))
			{
				nresult = -2;
				break;
			}
		}	

		//创建MF(PBOC MF:1PAY.SYS.DDF01)(MF的文件名长度为0E，不能修改)
		//AID=3F00
		//MF空间=1A00
		//文件属性=5E:
		//DF有效性=01（有效）
		//钱包初始化使能位=0（不初始化）
		//删除DF使能=1（DF可删除）
		//DF下是否具有FCI文件=1（具有FCI文件）
		//创建DF下文件是否需要加MAC=1（需要）；
		//是否PBOC应用=1（强制为1）
		//钱包消费是否追加交易记录=0(不追加)；
		//创建权限=91(创建MF下的EF前需要进行外部认证)
		if (apdu_getrandom(szdata,4)==0)
		{
		
			memcpy(szcmd, "\x04\xE0\x00\x38\x1E\x3F\x00\x38\x1A\x00\x5E\x00\x00\x00\x00\x91\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", 31);
			ulen = 31;
			MAC_3(g_input.key.TK_MF, ulen, szdata, szcmd, szcmd+ulen);	
			ulen += 4;
			//dbg_dumpmemory("create mf =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create mf |=",szcmd,respone_len);
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -3;
				break;
			}			
		
		}

		memcpy(szcmd, "\x00\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", 19);
		ulen = 19;
		//dbg_dumpmemory("select mf =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("select mf |=",szcmd,respone_len);
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -4;
			break;
		}	

		//认证传输密钥
		//认证传输密钥
		if (apdu_getrandom(szdata,8)==0)
		{
			if (apdu_exteralauth(szdata,g_input.key.TK_MF))
			{
				nresult = -5;
				break;
			}
		}

		//FID=0000 创建MF下的KEY文件
		//可创建18h条密钥，每条密钥记录长度14h
		//文件属性=C0（写至少需要MAC，写至少需要加密）	
		if (apdu_getrandom(szdata,4)==0)
		{
		
			memcpy(szcmd, "\x04\xE0\x00\x08\x0F\x00\x00\x08\x18\x14\xC0\x00\x00\x00\x01\x01", 16);
			ulen = 16;
			MAC_3(g_input.key.TK_MF, ulen, szdata, szcmd, szcmd+ulen);	
			ulen += 4;
			//dbg_dumpmemory("create mfkey0000=",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create mfkey0000 |=",szcmd,respone_len);
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -6;
				break;
			}			
		
		}

		//DCCK  卡片主控密钥
		if (apdu_getrandom(szdata,4)==0)
		{
		
			memcpy(szcmd,"\x84\xF0\x00\x01\x1C", 5);
			ulen = 5;
			memcpy(sztmp,"\x10\x01\x01\x33",4);
			memcpy(sztmp+4, g_input.key.MF_MKEY, 16);
			tmp_len = data_encrypt(20, sztmp, g_input.key.TK_MF);
			

			memcpy(szcmd+ulen, sztmp, tmp_len);
			ulen += tmp_len;

			MAC_3(g_input.key.TK_MF, ulen, szdata, szcmd, szcmd+ulen);	

			ulen += 4;
			//dbg_dumpmemory("create  mf dcck =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create  mf dcck |=",szcmd,respone_len);
		
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -7;
				break;
			}
		}		

		//DCMK  卡片应用维护密钥		
		if (apdu_getrandom(szdata,4)==0)
		{
		
			memcpy(szcmd,"\x84\xF0\x00\x02\x1C", 5);
			ulen = 5;
			memcpy(sztmp,"\x30\x01\x00\x33",4);
			memcpy(sztmp+4, g_input.key.MF_MAMK, 16);
			tmp_len = data_encrypt(20, sztmp, g_input.key.MF_MKEY);
			

			memcpy(szcmd+ulen, sztmp, tmp_len);
			ulen += tmp_len;

			MAC_3(g_input.key.MF_MKEY, ulen, szdata, szcmd, szcmd+ulen);	

			ulen += 4;
			//dbg_dumpmemory("create mf dcmk =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create mf dcmk |=",szcmd,respone_len);
		
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -8;
				break;
			}
		}	


		//FID=0001 创建目录信息文件	DCMK
		if (apdu_getrandom(szdata,4)==0)
		{
		
			memcpy(szcmd, "\x04\xE0\x00\x05\x0F\x00\x01\x05\x01\x00\x08\x00\x00\x00\x00\x00", 16);
			ulen = 16;
			MAC_3(g_input.key.MF_MKEY, ulen, szdata, szcmd, szcmd+ulen);	
			ulen += 4;
			//dbg_dumpmemory("create FID0001=",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create FID0001 |=",szcmd,respone_len);
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -9;
				break;
			}			
		
		}

		//更新目录信息0001
		if (apdu_getrandom(szdata,4)==0)
		{
		
			memcpy(szcmd, "\x04\xDC\x01\x0C\x1C", 5);
			ulen = 5;
			memcpy(szcmd+ulen, "\x61\x11\x4F\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01\x50\x09\x43\x53\x5F\x4D\x54\x52\x5F\x30\x31", 24);
			ulen += 24;
			MAC_3(g_input.key.MF_MAMK, ulen, szdata, szcmd, szcmd+ulen);	
			ulen += 4;
			//dbg_dumpmemory("update FID0000=",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("update FID0000 |=",szcmd,respone_len);
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -10;
				break;
			}			
		
		}

		
		//;FID=0005 创建发卡方基本信息文件  DCMK控制
		if (apdu_getrandom(szdata,4)==0)
		{
		
			memcpy(szcmd, "\x04\xE0\x00\x01\x0F\x00\x05\x01\x00\x28\x08\x00\x00\x00\x00\x00", 16);
			ulen = 16;
			MAC_3(g_input.key.MF_MKEY, ulen, szdata, szcmd, szcmd+ulen);	
			ulen += 4;
			//dbg_dumpmemory("create FID0005=",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create FID0005 |=",szcmd,respone_len);
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -11;
				break;
			}			
		
		}

		//更新发卡方信息文件0005
		if (apdu_getrandom(szdata,4)==0)
		{

			memset(szcmd, 0x00, sizeof(szcmd));
			
			memcpy(szcmd, "\x04\xD6\x85\x00\x2C", 5);
			ulen = 5;
			lpmfissueinf = (LPTISSUE)(szcmd+ulen);

			memcpy(lpmfissueinf->issuecode, g_input.inf.issuecode, 2);//发卡方代码
			memcpy(lpmfissueinf->citycode, g_input.inf.citycode, 2);//城市代码
			memcpy(lpmfissueinf->industrycode, g_input.inf.citycode, 2);//城市代码
			lpmfissueinf->testflag = g_input.inf.testflag;				  // 测试标记
			memcpy(lpmfissueinf->logicnumber, g_input.logicnumber, 8); //逻辑卡号
			memcpy(lpmfissueinf->cardtype,g_input.inf.cardtype, 2);
			memcpy(lpmfissueinf->issuedate, g_input.inf.issuedate, 2);
			memcpy(lpmfissueinf->issuedevice, g_input.inf.issuedevice, 6);
			memcpy(lpmfissueinf->cardversion, g_input.inf.cardversion, 2);
			
			
			memcpy(lpmfissueinf->effectivefrom, g_input.inf.effectivefrom, 4);
			memcpy(lpmfissueinf->effectiveto, g_input.inf.effectiveto, 4);

			
//				memcpy(lpmfissueinf->issuecode, "\x53\x20", 2);//发卡方代码
//				memcpy(lpmfissueinf->citycode, "\x41\x00", 2);//城市代码
//				memcpy(lpmfissueinf->industrycode, "\x00\x00", 2);//城市代码
//				lpmfissueinf->testflag = 0x01;                // 测试标记
//				memcpy(lpmfissueinf->logicnumber, "\x90\x00\x00\x00\x10\x00\x00\x10", 8); //逻辑卡号
//				memcpy(lpmfissueinf->cardtype,"\x02\x00", 2);
//				memcpy(lpmfissueinf->issuedate, "\x20\x13\x04\x16", 4);
//				memcpy(lpmfissueinf->issuedevice, "\x11\x22\x33\x44\x55\x66", 6);
//				memcpy(lpmfissueinf->cardversion, "\x00\x01", 2);
//				
//				
//				memcpy(lpmfissueinf->effectivefrom, "\x20\x13\x06\x01", 4);
//				memcpy(lpmfissueinf->effectiveto, "\x20\x19\x12\x31", 4);
//				
//				memcpy(lpmfissueinf->effectivefrom, "\x20\x13\x06\x01", 4);//应用启用日期
//				memcpy(lpmfissueinf->effectiveto, "\x20\x19\x12\x31", 4);//应用结束日期
			
			ulen += sizeof(TISSUE);
			MAC_3(g_input.key.MF_MAMK, ulen, szdata, szcmd, szcmd+ulen);	
			ulen += 4;
			//dbg_dumpmemory("update FID0005=",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("update FID0005 |=",szcmd,respone_len);
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -12;
				break;
			}			
		
		}

		
		//FID=EF0D，创建FCI文件
		if (apdu_getrandom(szdata,4)==0)
		{
		
			memcpy(szcmd, "\x04\xE0\x00\x01\x0F\xEF\x0D\x01\x00\x03\x08\x00\x00\x00\x00\x00", 16);
			ulen = 16;
			MAC_3(g_input.key.MF_MKEY, ulen, szdata, szcmd, szcmd+ulen);	
			ulen += 4;
			//dbg_dumpmemory("create MF FCI  =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create MF FCI |=",szcmd,respone_len);
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -13;
				break;
			}			
		
		}

		//更新FCI文件EF0D
		//88(目录基本文件的SFI标签) 01(长度) 01(目录基本文件的SFI)

		if (apdu_getrandom(szdata,4)==0)
		{
		
			memcpy(szcmd, "\x04\xD6\x8D\x00\x07\x88\x01\x01", 8);
			ulen = 8;
			MAC_3(g_input.key.MF_MAMK, ulen, szdata, szcmd, szcmd+ulen);	
			ulen += 4;
			//dbg_dumpmemory("update MF FCI  =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("update MF FCI |=",szcmd,respone_len);
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -14;
				break;
			}			
		
		}		




	}while(0);

	return nresult;

}



int ticket_create_adf1(void)
{

    unsigned char ulen;
    unsigned char respone_len;
	unsigned char sztmp[256];	
	unsigned char szcmd[256];
	unsigned char szdata[8];
	unsigned char tmp_len;
    unsigned short sam_sw;
	LPTPUBLICINF lppublic;

	int i;

	//对应7 个复合记录, 数据分别为标识和长度
	unsigned char complex_record[7][2]={
		{0x01,0x30},{0x02,0x30},{0x03,0x30},{0x04,0x30},{0x05,0x30},{0x06,0x30},{0x11,0x18}
	};
	
    int nresult = 0;


	do{
		//注册ADF1 
		memcpy(sztmp, "\x00\xEE\x01\x38\x20",5);
		ulen = 5;
		memcpy(sztmp+ulen, "\x10\x01\x38\x07\xC0\x32\x00\x00\x00\x00\x11\x09\xF0\x01\x01\x33", 16);
		ulen += 16;
		memcpy(sztmp+ulen, g_input.key.ADF1_TKEY, 16);
		ulen += 16;
		//dbg_dumpmemory("register adf1 =",sztmp,ulen);
		respone_len = MifareProCom(ulen, sztmp, &sam_sw);
		//dbg_dumpmemory("register adf1 |=",sztmp,respone_len);
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -1;
			//break;
		}

		//认证ADF1的传输密钥
		if (apdu_getrandom(szdata,8)==0)
		{
			if (apdu_exteralauth(szdata,g_input.key.ADF1_TKEY))
			{
				nresult = -2;
				break;
			}
		}

		
		//创建ADF1 
		if (apdu_getrandom(szdata,8)==0)
		{

			//dbg_dumpmemory("random =",szdata,8);
			memcpy(sztmp, "\x04\xE0\x00\x38\x19\x10\x01\x38\x05\xC0\x7B\x00\x00\x00\x00\x11\x09\xF0\x00\x00\x41\x00\x53\x20\xAD\xF1", 26);
			memcpy(szcmd, "\x04\xE0\x00\x38\x19\x10\x01\x38\x05\xC0\x7B\x00\x00\x00\x00\x11\x09\xF0\x00\x00\x41\x00\x53\x20\xAD\xF1", 26);
			ulen = 26;

			MAC_3(g_input.key.ADF1_TKEY, ulen, szdata, szcmd, szcmd+ulen);	

			ulen += 4;
			//dbg_dumpmemory("create adf1 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create adf1 |=",szcmd,respone_len);
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -1;
				break;
			}			

		}

		//创建密钥文件0000
		memcpy(szcmd, "\x00\xE0\x00\x08\x0B\x00\x00\x08\x18\x14\xC0\x00\x00\x00\x01\x01", 16);
		ulen = 16;
		//dbg_dumpmemory("create key0000 =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("create key0000 |=",szcmd,respone_len);

		//创建adf1 主控密钥10 (外部认证密钥)
		if (apdu_getrandom(szdata,4)==0)
		{

			memcpy(szcmd,"\x84\xF0\x00\x01\x1C", 5);
			ulen = 5;
			memcpy(sztmp,"\x10\x01\x01\x33",4);
			memcpy(sztmp+4, g_input.key.ADF1_MKEY, 16);
			tmp_len = data_encrypt(20,sztmp,g_input.key.ADF1_TKEY);

			memcpy(szcmd+ulen, sztmp, tmp_len);
			ulen += tmp_len;

			MAC_3(g_input.key.ADF1_TKEY, ulen, szdata, szcmd, szcmd+ulen);	

			ulen += 4;
			//dbg_dumpmemory("create adf1 key_mkey =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create adf1 key_mkey |=",szcmd,respone_len);
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -2;
				break;
			}			

		}		

		// ADF1 创建消费密钥=====================
		//密钥类型: 40 (消费密钥)
		//版本: 01
		//序号: 01-05
		if (apdu_getrandom(szdata,4)==0)
		{

			memcpy(szcmd,"\x84\xF0\x00\x02\x1C", 5);
			ulen = 5;
			memcpy(sztmp,"\x40\x01\x01\x33",4);
			memcpy(sztmp+4, g_input.key.MPK_ADF1_0101, 16);
			tmp_len = data_encrypt(20,sztmp,g_input.key.ADF1_MKEY);
			

			memcpy(szcmd+ulen, sztmp, tmp_len);
			ulen += tmp_len;

			MAC_3(g_input.key.ADF1_MKEY, ulen, szdata, szcmd, szcmd+ulen);	

			ulen += 4;
			//dbg_dumpmemory("create adf1 MPK_0101 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create adf1 MPK_0101 |=",szcmd,respone_len);

			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -3;
				break;
			}
		}

		if (apdu_getrandom(szdata,4)==0)
		{

			memcpy(szcmd,"\x84\xF0\x00\x03\x1C", 5);
			ulen = 5;
			memcpy(sztmp,"\x40\x01\x02\x33",4);
			memcpy(sztmp+4, g_input.key.MPK_ADF1_0102, 16);
			tmp_len = data_encrypt(20,sztmp,g_input.key.ADF1_MKEY);

			memcpy(szcmd+ulen, sztmp, tmp_len);
			ulen += tmp_len;

			MAC_3(g_input.key.ADF1_MKEY, ulen, szdata, szcmd, szcmd+ulen);	

			ulen += 4;
			//dbg_dumpmemory("create adf1 MPK_0102 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create adf1 MPK_0102 |=",szcmd,respone_len);

			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -4;
				break;
			}
		}

		if (apdu_getrandom(szdata,4)==0)
		{
			memcpy(szcmd,"\x84\xF0\x00\x04\x1C", 5);
			ulen = 5;
			memcpy(sztmp,"\x40\x01\x03\x33",4);
			memcpy(sztmp+4, g_input.key.MPK_ADF1_0103, 16);
			tmp_len = data_encrypt(20,sztmp,g_input.key.ADF1_MKEY);

			memcpy(szcmd+ulen, sztmp, tmp_len);
			ulen += tmp_len;

			MAC_3(g_input.key.ADF1_MKEY, ulen, szdata, szcmd, szcmd+ulen);	

			ulen += 4;
			//dbg_dumpmemory("create adf1 MPK_0103 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create adf1 MPK_0103 |=",szcmd,respone_len);

			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -5;
				break;
			}
		}

		if (apdu_getrandom(szdata,4)==0)
		{
			memcpy(szcmd,"\x84\xF0\x00\x05\x1C", 5);
			ulen = 5;
			memcpy(sztmp,"\x40\x01\x04\x33",4);
			memcpy(sztmp+4, g_input.key.MPK_ADF1_0104, 16);
			tmp_len = data_encrypt(20,sztmp,g_input.key.ADF1_MKEY);

			memcpy(szcmd+ulen, sztmp, tmp_len);
			ulen += tmp_len;

			MAC_3(g_input.key.ADF1_MKEY, ulen, szdata, szcmd, szcmd+ulen);	

			ulen += 4;
			//dbg_dumpmemory("create adf1 MPK_0104 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create adf1 MPK_0104 |=",szcmd,respone_len);

			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -6;
				break;
			}
		}


		if (apdu_getrandom(szdata,4)==0)
		{
			memcpy(szcmd,"\x84\xF0\x00\x06\x1C", 5);
			ulen = 5;
			memcpy(sztmp,"\x40\x01\x05\x33",4);
			memcpy(sztmp+4, g_input.key.MPK_ADF1_0105, 16);
			tmp_len = data_encrypt(20,sztmp,g_input.key.ADF1_MKEY);

			memcpy(szcmd+ulen, sztmp, tmp_len);
			ulen += tmp_len;

			MAC_3(g_input.key.ADF1_MKEY, ulen, szdata, szcmd, szcmd+ulen);	

			ulen += 4;
			//dbg_dumpmemory("create adf1 MPK_0105 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create adf1 MPK_0105 |=",szcmd,respone_len);
			
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -7;
				break;
			}		

			

		}			

		// ADF1 创建圈存密钥=====================
		//密钥类型: E0 (圈存密钥)
		//版本: 02
		//序号: 01
		if (apdu_getrandom(szdata,4)==0)
		{

			memcpy(szcmd,"\x84\xF0\x00\x07\x1C", 5);
			ulen = 5;
			memcpy(sztmp,"\xE0\x02\x01\x33",4);
			memcpy(sztmp+4, g_input.key.MLK_ADF1_0201, 16);
			tmp_len = data_encrypt(20,sztmp,g_input.key.ADF1_MKEY);
			

			memcpy(szcmd+ulen, sztmp, tmp_len);
			ulen += tmp_len;

			MAC_3(g_input.key.ADF1_MKEY, ulen, szdata, szcmd, szcmd+ulen);	

			ulen += 4;
			//dbg_dumpmemory("create adf1 MLK_0101 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create adf1 MLK_0101 |=",szcmd,respone_len);

			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -8;
				break;
			}
		}

		
		// ADF1 创建TAC 密钥=====================
		//密钥类型: 60 (圈存密钥)
		//版本: 01
		//序号: 01
		if (apdu_getrandom(szdata,4)==0)
		{

			memcpy(szcmd,"\x84\xF0\x00\x08\x1C", 5);
			ulen = 5;
			memcpy(sztmp,"\x60\x01\x01\x33",4);
			memcpy(sztmp+4, g_input.key.MTK_ADF1, 16);
			tmp_len = data_encrypt(20,sztmp,g_input.key.ADF1_MKEY);
			


			memcpy(szcmd+ulen, sztmp, tmp_len);
			ulen += tmp_len;

			MAC_3(g_input.key.ADF1_MKEY, ulen, szdata, szcmd, szcmd+ulen);	

			ulen += 4;
			//dbg_dumpmemory("create adf1 MTK_0101 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create adf1 MTK_0101 |=",szcmd,respone_len);

			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -9;
				break;
			}
		}		

		// ADF1 创建维护 密钥=====================
		//密钥类型: 30 (维护密钥)
		//版本: 01
		//序号: 00 - 03
		if (apdu_getrandom(szdata,4)==0)
		{

			memcpy(szcmd,"\x84\xF0\x00\x09\x1C", 5);
			ulen = 5;
			memcpy(sztmp,"\x30\x01\x00\x33",4);
			memcpy(sztmp+4, g_input.key.MAMK_ADF1_00, 16);
			tmp_len = data_encrypt(20,sztmp,g_input.key.ADF1_MKEY);
			

			memcpy(szcmd+ulen, sztmp, tmp_len);
			ulen += tmp_len;

			MAC_3(g_input.key.ADF1_MKEY, ulen, szdata, szcmd, szcmd+ulen);	

			ulen += 4;
			//dbg_dumpmemory("create adf1 MAMK_0100 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create adf1 MAMK_0100 |=",szcmd,respone_len);

			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -10;
				break;
			}
		}		


		if (apdu_getrandom(szdata,4)==0)
		{
		
			memcpy(szcmd,"\x84\xF0\x00\x0A\x1C", 5);
			ulen = 5;
			memcpy(sztmp,"\x30\x01\x01\x33",4);
			memcpy(sztmp+4, g_input.key.MAMK_ADF1_01, 16);
			tmp_len = data_encrypt(20,sztmp,g_input.key.ADF1_MKEY);
			

			memcpy(szcmd+ulen, sztmp, tmp_len);
			ulen += tmp_len;

			MAC_3(g_input.key.ADF1_MKEY, ulen, szdata, szcmd, szcmd+ulen);	

			ulen += 4;
			//dbg_dumpmemory("create adf1 MAMK_0101 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create adf1 MAMK_0101 |=",szcmd,respone_len);
		
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -11;
				break;
			}
		}		

		if (apdu_getrandom(szdata,4)==0)
		{
		
			memcpy(szcmd,"\x84\xF0\x00\x0B\x1C", 5);
			ulen = 5;
			memcpy(sztmp,"\x30\x01\x02\x33",4);
			memcpy(sztmp+4, g_input.key.MAMK_ADF1_02, 16);
			tmp_len = data_encrypt(20,sztmp,g_input.key.ADF1_MKEY);
			

			memcpy(szcmd+ulen, sztmp, tmp_len);
			ulen += tmp_len;

			MAC_3(g_input.key.ADF1_MKEY, ulen, szdata, szcmd, szcmd+ulen);	

			ulen += 4;
			//dbg_dumpmemory("create adf1 MAMK_0102 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create adf1 MAMK_0102 |=",szcmd,respone_len);
		
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -12;
				break;
			}
		}	

		if (apdu_getrandom(szdata,4)==0)
		{
		
			memcpy(szcmd,"\x84\xF0\x00\x0C\x1C", 5);
			ulen = 5;
			memcpy(sztmp,"\x30\x01\x03\x33",4);
			memcpy(sztmp+4, g_input.key.MAMK_ADF1_03, 16);
			tmp_len = data_encrypt(20,sztmp,g_input.key.ADF1_MKEY);

			memcpy(szcmd+ulen, sztmp, tmp_len);
			ulen += tmp_len;

			MAC_3(g_input.key.ADF1_MKEY, ulen, szdata, szcmd, szcmd+ulen);	

			ulen += 4;
			//dbg_dumpmemory("create adf1 MAMK_0103 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create adf1 MAMK_0103 |=",szcmd,respone_len);
		
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -13;
				break;
			}
		}		


		// ADF1 创建PIN =====================
		//密码固定: 08 00 01
		//解锁次数: 05
		//密码长度: 03
		if (apdu_getrandom(szdata,4)==0)
		{

			memcpy(szcmd,"\x84\xF0\x00\x0D\x14", 5);
			ulen = 5;
			memcpy(sztmp,"\x08\x00\x00\x55\x03",5);
			memcpy(sztmp+5, g_input.key.PIN_ADF1, 3);
			tmp_len = data_encrypt(8,sztmp,g_input.key.ADF1_MKEY);
			

			memcpy(szcmd+ulen, sztmp, tmp_len);
			ulen += tmp_len;

			MAC_3(g_input.key.ADF1_MKEY, ulen, szdata, szcmd, szcmd+ulen);	

			ulen += 4;
			//dbg_dumpmemory("create adf1 PIN =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create adf1 PIN |=",szcmd,respone_len);

			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -14;
				break;
			}
		}			

		
		// ADF1 创建PIN  解锁密钥=====================
		//密钥固定: 20
		//版本: 01
		//序号: 02 （见1108的WRITE KEY/PIN指令说明）
		if (apdu_getrandom(szdata,4)==0)
		{

			memcpy(szcmd,"\x84\xF0\x00\x0E\x1C", 5);
			ulen = 5;
			memcpy(sztmp,"\x20\x01\x02\x33",4);
			memcpy(sztmp+4, g_input.key.MPUK_ADF1, 16);
			tmp_len = data_encrypt(20,sztmp,g_input.key.ADF1_MKEY);
			

			memcpy(szcmd+ulen, sztmp, tmp_len);
			ulen += tmp_len;

			MAC_3(g_input.key.ADF1_MKEY, ulen, szdata, szcmd, szcmd+ulen);	

			ulen += 4;
			//dbg_dumpmemory("create adf1 MPUK_0100 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create adf1 MPUK_0100 |=",szcmd,respone_len);

			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -15;
				break;
			}
		}	


		// ADF1 创建PIN 重装密钥=====================
		//密钥固定: 20
		//版本: 01
		//序号: 03 （见1108的WRITE KEY/PIN指令说明）
		if (apdu_getrandom(szdata,4)==0)
		{
		
			memcpy(szcmd,"\x84\xF0\x00\x0E\x1C", 5);
			ulen = 5;
			memcpy(sztmp,"\x20\x01\x03\x33",4);
			memcpy(sztmp+4, g_input.key.MRPK_ADF1, 16);
			tmp_len = data_encrypt(20,sztmp,g_input.key.ADF1_MKEY);
			

			memcpy(szcmd+ulen, sztmp, tmp_len);
			ulen += tmp_len;

			MAC_3(g_input.key.ADF1_MKEY, ulen, szdata, szcmd, szcmd+ulen);	

			ulen += 4;
			//dbg_dumpmemory("create adf1 MRPK_0100 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("create adf1 MRPK_0100 |=",szcmd,respone_len);
		
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -16;
				break;
			}
		}	


		//0011	 应用控制文件
		//二进制文件(01)，文件属性=0A
		//读自由，维护密钥00更新：文件属性=0A，读权限=00，写权限=00
		memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x11\x01\x00\x20\x0A\x00\x00\x00\x00\x00", 16);
		ulen = 16;
		//dbg_dumpmemory("create adf1 file_0011 =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("create adf1 file_0011 |=",szcmd,respone_len);

		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -17;
			break;
		}
		

		//更新0011应用控制文件
		if (apdu_getrandom(szdata,4)==0)
		{
			memset(szcmd, 0x00, sizeof(szcmd));
			memcpy(szcmd, "\x04\xD6\x91\x00\x24",5);
			ulen = 5;
			ulen +=32; //数据为全 00
			MAC_3(g_input.key.MAMK_ADF1_02, ulen, szdata, szcmd, szcmd+ulen);
			ulen += 4;
			//dbg_dumpmemory("update adf1 file_0011 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("update adf1 file_0011 |=",szcmd,respone_len);				
	
		}

		//0012   保留文件
		//二进制文件(01)，文件属性=08
		//读自由，维护密钥00更新：文件属性=08，读权限=00，写权限=00	
		memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x12\x01\x00\x20\x0A\x00\x00\x00\x00\x00", 16);
		//dbg_dumpmemory("create adf1 file_0012 =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("create adf1 file_0012 |=",szcmd,respone_len);	
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -18;
			break;
		}
		

//	//0015	 创建公共应用基本文件
//	//二进制文件(01)，文件属性=09
//	//读自由，维护密钥00更新：文件属性=09，读权限=00，写权限=00 	
//	memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x15\x01\x00\x1E\x00\x00\x00\x00\x00\x00", 16);
//	//dbg_dumpmemory("create adf1 file_0015 =",szcmd,ulen);
//	respone_len = MifareProCom(ulen, szcmd, &sam_sw);
//	//dbg_dumpmemory("create adf1 file_0015 |=",szcmd,respone_len);
//	if ((respone_len==0) || (0x9000!=sam_sw))
//	{
//		nresult = -19;
//		break;
//	}
//	
//	
//	//更新0015公共应用基本文件
//	//注意更新逻辑卡号 sDData = 9000000010000010
//	if (apdu_getrandom(szdata,4)==0)
//	{
//		memset(szcmd, 0x00, sizeof(szcmd));
//		memcpy(szcmd, "\x00\xD6\x95\x00\x1e",5);//22
//		ulen = 5;
//	
//		lppublic = (LPTPUBLICINF)(szcmd+ulen);
//		memcpy(lppublic->issuecode, "\x53\x20", 2);//发卡方代码
//		memcpy(lppublic->citycode, "\x41\x00", 2);//城市代码
//		lppublic->appstatus = 0x00; 			  //启用标识
//		lppublic->appversion = 0x01;			  // 应用版本
//		lppublic->testflag = 0x01;				  // 测试标记
//		memcpy(lppublic->logicnumber, "\x90\x00\x00\x00\x10\x00\x00\x10", 8); //逻辑卡号
//		memcpy(lppublic->effectivefrom, "\x20\x13\x06\x01", 4);//应用启用日期
//		memcpy(lppublic->effectiveto, "\x20\x19\x12\x31", 4);//应用结束日期
//		
//		ulen +=sizeof(TPUBLICINF); //数据为全 00
//	//	MAC_3(g_input.key.MAMK_ADF1_01, ulen, szdata, szcmd, szcmd+ulen);
//	//	ulen += 4;
//		//dbg_dumpmemory("update adf1 file_0015 =",szcmd,ulen);
//		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
//		//dbg_dumpmemory("update adf1 file_0015 |=",szcmd,respone_len);				
//	
//	}	
//	
//	//0016	 创建持卡人基本信息
//	//二进制文件(01)，文件属性=0A
//	//读自由，维护密钥02更新：文件属性=0A，读权限=00，写权限=00
//	memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x16\x01\x00\x38\x00\x00\x00\x00\x00\x00", 16);
//	//dbg_dumpmemory("create adf1 file_0016 =",szcmd,ulen);
//	respone_len = MifareProCom(ulen, szcmd, &sam_sw);
//	//dbg_dumpmemory("create adf1 file_0016 |=",szcmd,respone_len);	
//	if ((respone_len==0) || (0x9000!=sam_sw))
//	{
//		nresult = -20;
//		break;
//	}
//	
//	
//	//更新0016创建持卡人基本信息
//	if (apdu_getrandom(szdata,4)==0)
//	{
//		memset(szcmd, 0x00, sizeof(szcmd));
//		memcpy(szcmd, "\x00\xD6\x96\x00\x38",5);
//		ulen = 5;
//		ulen += 56; //0x38	//数据为全 00
//	//		MAC_3(g_input.key.MAMK_ADF1_02, ulen, szdata, szcmd, szcmd+ulen);
//	//		ulen += 4;
//		//dbg_dumpmemory("update adf1 file_0016 =",szcmd,ulen);
//		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
//		//dbg_dumpmemory("update adf1 file_0016 |=",szcmd,respone_len);
//		if ((respone_len==0) || (0x9000!=sam_sw))
//		{
//			nresult = -21;
//			break;
//		}
//		
//	}		



		//0015   创建公共应用基本文件
		//二进制文件(01)，文件属性=09
		//读自由，维护密钥00更新：文件属性=09，读权限=00，写权限=00		
		memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x15\x01\x00\x1E\x09\x00\x00\x00\x00\x00", 16);
		//dbg_dumpmemory("create adf1 file_0015 =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("create adf1 file_0015 |=",szcmd,respone_len);
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -19;
			break;
		}
		
	
		//更新0015公共应用基本文件
		//注意更新逻辑卡号 sDData = 9000000010000010
		if (apdu_getrandom(szdata,4)==0)
		{
			memset(szcmd, 0x00, sizeof(szcmd));
			memcpy(szcmd, "\x04\xD6\x95\x00\x22",5);
			ulen = 5;
	
			lppublic = (LPTPUBLICINF)(szcmd+ulen);
			memcpy(lppublic->issuecode, g_input.inf.issuecode, 2);//发卡方代码
			memcpy(lppublic->citycode, g_input.inf.issuecode, 2);//城市代码
			lppublic->appstatus = g_input.inf.appstatus;               //启用标识
			lppublic->appversion = g_input.inf.appversion;              // 应用版本
			lppublic->testflag = g_input.inf.testflag;                // 测试标记
			memcpy(lppublic->logicnumber, g_input.logicnumber, 8); //逻辑卡号
			memcpy(lppublic->effectivefrom, g_input.inf.appeffectivefrom, 4);//应用启用日期
			memcpy(lppublic->effectiveto, g_input.inf.appeffectiveto, 4);//应用结束日期

			
//				memcpy(lppublic->issuecode, "\x53\x20", 2);//发卡方代码
//				memcpy(lppublic->citycode, "\x41\x00", 2);//城市代码
//				lppublic->appstatus = 0x00;               //启用标识
//				lppublic->appversion = 0x01;              // 应用版本
//				lppublic->testflag = 0x01;                // 测试标记
//				memcpy(lppublic->logicnumber, "\x90\x00\x00\x00\x10\x00\x00\x10", 8); //逻辑卡号
//				memcpy(lppublic->effectivefrom, "\x20\x13\x06\x01", 4);//应用启用日期
//				memcpy(lppublic->effectiveto, "\x20\x19\x12\x31", 4);//应用结束日期
			
			ulen +=sizeof(TPUBLICINF); //数据为全 00
			MAC_3(g_input.key.MAMK_ADF1_01, ulen, szdata, szcmd, szcmd+ulen);
			ulen += 4;
			//dbg_dumpmemory("update adf1 file_0015 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("update adf1 file_0015 |=",szcmd,respone_len);				
	
		}		

		//0016   创建持卡人基本信息
		//二进制文件(01)，文件属性=0A
		//读自由，维护密钥02更新：文件属性=0A，读权限=00，写权限=00
		memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x16\x01\x00\x38\x0A\x00\x00\x00\x00\x00", 16);
		//dbg_dumpmemory("create adf1 file_0016 =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("create adf1 file_0016 |=",szcmd,respone_len);	
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -20;
			break;
		}

	
		//更新0016创建持卡人基本信息
		if (apdu_getrandom(szdata,4)==0)
		{

			memset(szcmd, 0x00, sizeof(szcmd));
			memcpy(szcmd, "\x04\xD6\x96\x00\x3C",5);
			ulen = 5;
			ulen += 56; //0x38  //数据为全 00
			MAC_3(g_input.key.MAMK_ADF1_02, ulen, szdata, szcmd, szcmd+ulen);
			ulen += 4;
			//dbg_dumpmemory("update adf1 file_0016 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("update adf1 file_0016 |=",szcmd,respone_len);
			//dbg_formatvar("update adf1 file_0016 |= %04X",sam_sw);
			
			if ((respone_len==0) || (0x9000!=sam_sw))
			{
				nresult = -21;
				break;
			}
			
		}		
		
		//0002   创建钱包文件
		//钱包文件(02)
		memcpy(szcmd, "\x00\xE0\x00\x22\x0B\x00\x02\x22\x00\x43\x00\x60\x60\x60\x60\x60", 16);
		//dbg_dumpmemory("create adf1 purse =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("create adf1 purse |=",szcmd,respone_len);	
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -20;
			break;
		}

		//0018  创建交易明细文件
		//文件类型=交易明细文件(32)
		//应用类型=00（PBOC应用）		
		memcpy(szcmd, "\x00\xE0\x00\x32\x0B\x00\x18\x32\x0A\x17\x00\x00\x00\x00\x00\x00", 16);
		//dbg_dumpmemory("create adf1 consume record =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("create adf1 consume record |=",szcmd,respone_len);	
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -20;
			break;
		}

		//0017	全国复合消费文件
		//文件类型=TLV格式线性变长记录文件(05)，文件属性=0A
		//读自由，维护密钥02更新：文件属性=0A，读权限=00，写权限=00
		//文件长度=0200		
		memcpy(szcmd, "\x00\xE0\x00\x05\x0B\x00\x17\x05\x02\x00\x0A\x00\x00\x00\x00\x00", 16);
		//dbg_dumpmemory("create adf1 complex file =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("create adf1 complex file |=",szcmd,respone_len);	
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -20;
			break;
		}



		//增加记录01
		for (i=0;i<7;i++)
		{
			if (apdu_getrandom(szdata,4)==0)
			{
				memset(szcmd, 0x00, sizeof(szcmd));
				memcpy(szcmd, "\x04\xE2\x00\xB8\x34",5);
				szcmd[4] = complex_record[i][1]+4; //发送数据长度+  MAC 4 字节长度
				szcmd[5] = complex_record[i][0];   //记录标识
				szcmd[6] = complex_record[i][1]-2; //记录去掉2字节标头的长度
				if (complex_record[i][0]==0x02)
				{
					szcmd[9] =0x09; //轨道交通文件, e/s初始化+ 正常
				}
				
				ulen = 5;
				ulen += complex_record[i][1]; //0x30  //数据为全 00
				MAC_3(g_input.key.MAMK_ADF1_02, ulen, szdata, szcmd, szcmd+ulen);
				ulen += 4;
				//dbg_dumpmemory("update adf1 complex record 1 =",szcmd,ulen);
				respone_len = MifareProCom(ulen, szcmd, &sam_sw);
				//dbg_dumpmemory("update adf1 complex record 1 |=",szcmd,respone_len);
				if ((respone_len==0) || (0x9000!=sam_sw))
				{
					nresult = -21;
					break;
				}
				
			}
		}

		if (sam_sw!=0x9000)
		{
			nresult = -21;
			break;
		}

		//EF0D	创建FCI文件
		memcpy(szcmd, "\x00\xE0\x00\x01\x0B\xEF\x0D\x01\x00\x04\x00\x00\x00\x00\x00\x00", 16);
		//dbg_dumpmemory("create adf1 FCI =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("create adf1 FCI |=",szcmd,respone_len);	
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -20;
			break;
		}


		//更新 FCI File
		//9F08(当前版本号标签) 01(长度) 02(当前版本号)		
		memcpy(szcmd, "\x00\xD6\x8D\x00\x04\x9F\x08\x01\x02", 16);
		//dbg_dumpmemory("update adf1 FCI =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("update adf1 FCI |=",szcmd,respone_len);	
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -20;
			break;
		}


		nresult = 0;
		
		
	}while(0);


	return nresult;

}


//==============================================================
//函数: ticket_esanalyze
//功能: 对票卡创建文件结构
//入口参数:
//                      无
//出口参数:
//                     lpoutdata = 票卡分析数据
//返回值:
//               0=成功
//==============================================================
int ticket_esanalyze(void *lpoutdata)
{
    unsigned char ulen;
    unsigned char respone_len;
	unsigned char sztmp[256];	
	unsigned char szcmd[256];
	unsigned char szdata[8];
	unsigned char tmp_len;
    unsigned short sam_sw;
	LPTPUBLICINF lppublic;
	LPTISSUE lpmfissueinf;
    int nresult = 0;


	do{

		memcpy(szcmd, "\x00\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", 19);
		ulen = 19;
		//dbg_dumpmemory("select mf =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("select mf |=",szcmd,respone_len);
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -4;
			//未发行
			break;
		}
		//已发行
		//参照bom  分析获取数据

		nresult = 0;
	
	}while(0);


	return nresult;

	

}



//==============================================================
//函数: ticket_init
//功能: 对票卡创建文件结构
//入口参数:
//                       lpindata =  按照TINPUT 结构排序的数据
//出口参数:
//                     lpoutdata = 预赋值交易
//返回值:
//               0=成功
//==============================================================
int ticket_esinit(void *lpindata, void *lpoutdata)
{
	int nresult=0;
	
	issue_setvalue(lpindata);

	nresult = ticket_reset();//删除结构，正式环境不用

	if (nresult==0)
	{
		nresult = ticket_create_mf();

		if (nresult)
		{
			nresult = 4041; //根据实际情况赋值
		}
		
	}

	if (nresult==0)
	{
		nresult = ticket_create_adf1();

		if (nresult)
		{
			nresult = 4042; 
		}
		
	}

	return nresult;
	

	
}
