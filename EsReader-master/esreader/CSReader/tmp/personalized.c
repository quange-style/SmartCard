
#include <string.h>
#include "sam.h"
#include "iso14443.h"
#include "algorithm.h"
#include "public.h"
#include "globalvar.h"

//=====================================
//�ļ���Ϣ
//���л�����Ϣ-- ���ļ���
typedef struct{
	bcd issuecode[2];               //���д���
	bcd citycode[2];                //���д���
	bcd industrycode[2];            //��ҵ����
	unsigned char testflag;         //���Ա��
	unsigned char rfu;              //��������
	unsigned char logicnumber[8];   //Ӧ�����к�
	unsigned char cardtype[2];      //������
	bcd issuedate[4];//yyyymmdd         //��������
	unsigned char issuedevice[6];   //�����豸��Ϣ
	unsigned char  cardversion[2];  //���汾��
	unsigned char effectivefrom[4]; //����������yyyymmdd
	unsigned char effectiveto[4];   //����������yyyymmdd
	unsigned char rfu2[2];          //����
}TISSUE,*LPTISSUE;//40Byte

//����Ӧ�û��������ļ�-- ADF1Ŀ¼��
typedef struct{
	bcd issuecode[2];               //����������
	bcd citycode[2];                //���д���
	bcd industrycode[2];            //��ҵ����
	unsigned char rfu[2];           //��������
	bcd appstatus;                  //���ñ�ʶ
	bcd appversion;                 //Ӧ�ð汾
	unsigned char testflag;         //���Ա��
	unsigned char rfu2;             //
	unsigned char logicnumber[8];   //Ӧ�����к�
	unsigned char effectivefrom[4]; //Ӧ����������yyyymmdd
	unsigned char effectiveto[4];   //Ӧ�ý�������yyyymmdd
	unsigned char rfu3[2];          //����
}TPUBLICINF,*LPTPUBLICINF; //�ܳ�30 BYTE


//�ֿ��˻��������ļ�
typedef struct{

}TPERSONALIZED,*LPPERSONALIZED;

//Ӧ�ÿ����ļ�
typedef struct{
	unsigned char saleflag;     //���ۼ�����
	unsigned char deposit;      //����Ѻ��
	unsigned char saledevice[6];   //�����豸��Ϣ
	unsigned char passmode;     //����ģʽ�ж�
	unsigned char entryline;    //������·
	unsigned char entrystation; //����վ��
	unsigned char exitline;     //�ɳ���·
	unsigned char exitstation;  //�ɳ�վ��
	unsigned char pursecaps[2];    //Ǯ������
	unsigned char rfu[17];      //Ԥ��
}TAPPCTRL,*LPTAPPCTRL;
//�ļ���Ϣ�ṹ
//==========================================

//=====================================================
//CPU����Կ
typedef struct{
	//MF��Կ
	unsigned char TK_MF[16];  //������Կ
	unsigned char MF_MKEY[16];//������Կ
	unsigned char MF_MAMK[16];//ά����Կ
	//ADF1��Կ
	unsigned char ADF1_TKEY[16];      //������Կ
	unsigned char ADF1_MKEY[16];      //������Կ
	unsigned char MLK_ADF1_0201[16];  //Ȧ�潻����Կ
	unsigned char MPK_ADF1_0101[16];  //���ѽ�����Կ01
	unsigned char MPK_ADF1_0102[16];  //���ѽ�����Կ02
	unsigned char MPK_ADF1_0103[16];  //���ѽ�����Կ03
	unsigned char MPK_ADF1_0104[16];  //���ѽ�����Կ04
	unsigned char MPK_ADF1_0105[16];  //���ѽ�����Կ05
	unsigned char MTK_ADF1[16];       //TAC ��Կ
	unsigned char MPUK_ADF1[16];      //����PIN ��Կ
	unsigned char MRPK_ADF1[16];      //��װPIN ��Կ
	unsigned char MAMK_ADF1_00[16];   //Ӧ��ά����Կ01
	unsigned char MAMK_ADF1_01[16];   //Ӧ��ά����Կ02
	unsigned char MAMK_ADF1_02[16];   //Ӧ��ά����Կ03
	unsigned char MAMK_ADF1_03[16];   //Ӧ��ά����Կ04
	unsigned char PIN_ADF1[3];       //����
	//ADF2��Կ
	unsigned char ADF2_TKEY[16];      //������Կ
	unsigned char ADF2_MKEY[16];      //������Կ
	unsigned char MLK_ADF2_0201[16];  //Ȧ�潻����Կ
	unsigned char MPK_ADF2_0101[16];  //���ѽ�����Կ01
	unsigned char MPK_ADF2_0102[16];  //���ѽ�����Կ02
	unsigned char MPK_ADF2_0103[16];  //���ѽ�����Կ03
	unsigned char MPK_ADF2_0104[16];  //���ѽ�����Կ04
	unsigned char MPK_ADF2_0105[16];  //���ѽ�����Կ05
	unsigned char MTK_ADF2[16];       //TAC ��Կ
	unsigned char MPUK_ADF2[16];      //����PIN ��Կ
	unsigned char MRPK_ADF2[16];      //��װPIN ��Կ
	unsigned char MAMK_ADF2_00[16];   //Ӧ��ά����Կ01
	unsigned char MAMK_ADF2_01[16];   //Ӧ��ά����Կ02
	unsigned char MAMK_ADF2_02[16];   //Ӧ��ά����Կ03
	unsigned char MAMK_ADF2_03[16];   //Ӧ��ά����Կ04
	unsigned char PIN_ADF2[3];       //����
}TTICK_KEY,*LPTTICKK_KEY;


typedef struct{
	bcd issuecode[2];               //����������
	bcd citycode[2];                //���д���
	bcd industrycode[2];            //��ҵ����	
	bcd testflag;                   //��Ӧ�ú�ADF1 Ӧ�ù���
	unsigned char logicnumber[8];   //Ӧ�����к���Ӧ�ú�ADF1 Ӧ�ù���
	unsigned char cardtype[2];      //������
	//��Ӧ����
	bcd issuedate[4];//yyyymmdd         //��������
	unsigned char issuedevice[6];   //�����豸��Ϣ
	unsigned char  cardversion[2];  //���汾��
	unsigned char effectivefrom[4]; //����������yyyymmdd
	unsigned char effectiveto[4];   //����������yyyymmdd
	//ADF1
	unsigned char appstatus;      //���ñ�ʶ
	bcd appversion;  //���汾��
	unsigned char appeffectivefrom[4]; //Ӧ����������yyyymmdd
	unsigned char appeffectiveto[4];   //Ӧ�ý�������yyyymmdd	
	//Ӧ�ÿ����ļ�
	unsigned char saleflag;     //���ۼ�����
	unsigned char deposit;      //����Ѻ��
	unsigned char saledevice[6];   //�����豸��Ϣ
	unsigned char passmode;     //����ģʽ�ж�
	unsigned char entryline;    //������·
	unsigned char entrystation; //����վ��
	unsigned char exitline;     //�ɳ���·
	unsigned char exitstation;  //�ɳ�վ��	
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
//������: issue_setvalue
//����     :  Ʊ����ʼ�����ݵ�����
//��ڲ���:
//                      lpdata: ����TINPUT �ṹ���������
//���ڲ���: ��
//����ֵ:      ��
//============================================================
void issue_setvalue(void *lpdata)
{

//===����ʱǿ�Ƹ�
  memcpy(g_input.logicnumber, l_logicnumber, 8);
  memcpy((unsigned char *)&g_input.key, (unsigned char *)&l_key, sizeof(l_key));

  //���л�����Ϣ�ļ�  
  memcpy(g_input.inf.issuecode, "\x53\x20", 2);//����������
  memcpy(g_input.inf.citycode, "\x41\x00", 2);//���д���
  memcpy(g_input.inf.industrycode, "\x00\x00", 2);//���д���
  g_input.inf.testflag = 0x01;				// ���Ա��
  memcpy(g_input.logicnumber, "\x90\x00\x00\x00\x10\x00\x00\x10", 8); //�߼�����
  memcpy(g_input.inf.cardtype,"\x02\x00", 2);
  memcpy(g_input.inf.issuedate, "\x20\x13\x04\x16", 4);
  memcpy(g_input.inf.issuedevice, "\x11\x22\x33\x44\x55\x66", 6);
  memcpy(g_input.inf.cardversion, "\x00\x01", 2);
  
  
  memcpy(g_input.inf.effectivefrom, "\x20\x13\x06\x01", 4); //����������
  memcpy(g_input.inf.effectiveto, "\x20\x19\x12\x31", 4);   //����Ч����

  //����Ӧ�û�������

  g_input.inf.appstatus=0x00;
  g_input.inf.appversion=0x01;
  
  memcpy(g_input.inf.appeffectivefrom, "\x20\x13\x06\x01", 4); //����������
  memcpy(g_input.inf.appeffectiveto, "\x20\x19\x12\x31", 4);   //����Ч����  

//Ӧ�ÿ���
  g_input.inf.saleflag = 0;	  //���ۼ�����
  g_input.inf.deposit = 0;	  //����Ѻ��
  memcpy(g_input.inf.saledevice, "\x00\x00\x00\x00\x00\x00", 6);	 //�����豸��Ϣ
  g_input.inf.passmode = 0;	  //����ģʽ�ж�
  g_input.inf.entryline = 0;	  //������·
  g_input.inf.entrystation = 0; //����վ��
  g_input.inf.exitline = 0;	  //�ɳ���·
  g_input.inf.exitstation = 0;  //�ɳ�վ��  

  //====================================
  //memcpy(&g_input, lpdata, sizeof(g_input));//��ʽʹ��ʱ��ֵ
  
}

//===============================================================
//����: MAC_3
//����: �������������·mac  ����
//��ڲ���:
//					  lpkey=��Կ
//					   nlen=Ҫ���м�������ݳ���
//					 lpdata=Ҫ���м������������
//���ڲ���:
//					lpresult=�����4�ֽ�mac  ����
//��  ��   ֵ:
//					��
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
    memcpy( sztmpnot, vector, 8 );                                              /* ��ʼֵ���� */
	do{

		if (block_len==1)
		{
			////dbg_dumpmemory("data=",lstblkdata, 8);
			for( j = 0; j < 8; j++ )
			{
				sztmpnot[j] ^= lstblkdata[j];									  /* �õ��Ľ���ͺ���8 �ֽ����  */
			}
		}
		else
		{
			////dbg_dumpmemory("data=",lpdata+offset, 8);
			for( j = 0; j < 8; j++ )
			{
				sztmpnot[j] ^= lpdata[offset++];									  /* �õ��Ľ���ͺ���8 �ֽ����  */
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
//����: data_encrypt
//����: ����������м���
//��ڲ���:
//                      nlen=Ҫ���м��ܵ����ݳ���
//                   lpdata=Ҫ���м��ܵ���������
//                    lpkey=��Կ
//���ڲ���:
//                  lpdata=���ܺ����������
//��  ��   ֵ:
//                  ���ܺ�����ݳ���
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
		//��Ϊ8  ������
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



//ɾ��Ʊ��ԭ�нṹ
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

		//��֤������Կ
		if (apdu_getrandom(szdata,8)==0)
		{
			if (apdu_exteralauth(szdata,g_input.key.TK_MF))
			{
				nresult = -2;
				break;
			}
		}	

		//����MF(PBOC MF:1PAY.SYS.DDF01)(MF���ļ�������Ϊ0E�������޸�)
		//AID=3F00
		//MF�ռ�=1A00
		//�ļ�����=5E:
		//DF��Ч��=01����Ч��
		//Ǯ����ʼ��ʹ��λ=0������ʼ����
		//ɾ��DFʹ��=1��DF��ɾ����
		//DF���Ƿ����FCI�ļ�=1������FCI�ļ���
		//����DF���ļ��Ƿ���Ҫ��MAC=1����Ҫ����
		//�Ƿ�PBOCӦ��=1��ǿ��Ϊ1��
		//Ǯ�������Ƿ�׷�ӽ��׼�¼=0(��׷��)��
		//����Ȩ��=91(����MF�µ�EFǰ��Ҫ�����ⲿ��֤)
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

		//��֤������Կ
		//��֤������Կ
		if (apdu_getrandom(szdata,8)==0)
		{
			if (apdu_exteralauth(szdata,g_input.key.TK_MF))
			{
				nresult = -5;
				break;
			}
		}

		//FID=0000 ����MF�µ�KEY�ļ�
		//�ɴ���18h����Կ��ÿ����Կ��¼����14h
		//�ļ�����=C0��д������ҪMAC��д������Ҫ���ܣ�	
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

		//DCCK  ��Ƭ������Կ
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

		//DCMK  ��ƬӦ��ά����Կ		
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


		//FID=0001 ����Ŀ¼��Ϣ�ļ�	DCMK
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

		//����Ŀ¼��Ϣ0001
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

		
		//;FID=0005 ����������������Ϣ�ļ�  DCMK����
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

		//���·�������Ϣ�ļ�0005
		if (apdu_getrandom(szdata,4)==0)
		{

			memset(szcmd, 0x00, sizeof(szcmd));
			
			memcpy(szcmd, "\x04\xD6\x85\x00\x2C", 5);
			ulen = 5;
			lpmfissueinf = (LPTISSUE)(szcmd+ulen);

			memcpy(lpmfissueinf->issuecode, g_input.inf.issuecode, 2);//����������
			memcpy(lpmfissueinf->citycode, g_input.inf.citycode, 2);//���д���
			memcpy(lpmfissueinf->industrycode, g_input.inf.citycode, 2);//���д���
			lpmfissueinf->testflag = g_input.inf.testflag;				  // ���Ա��
			memcpy(lpmfissueinf->logicnumber, g_input.logicnumber, 8); //�߼�����
			memcpy(lpmfissueinf->cardtype,g_input.inf.cardtype, 2);
			memcpy(lpmfissueinf->issuedate, g_input.inf.issuedate, 2);
			memcpy(lpmfissueinf->issuedevice, g_input.inf.issuedevice, 6);
			memcpy(lpmfissueinf->cardversion, g_input.inf.cardversion, 2);
			
			
			memcpy(lpmfissueinf->effectivefrom, g_input.inf.effectivefrom, 4);
			memcpy(lpmfissueinf->effectiveto, g_input.inf.effectiveto, 4);

			
//				memcpy(lpmfissueinf->issuecode, "\x53\x20", 2);//����������
//				memcpy(lpmfissueinf->citycode, "\x41\x00", 2);//���д���
//				memcpy(lpmfissueinf->industrycode, "\x00\x00", 2);//���д���
//				lpmfissueinf->testflag = 0x01;                // ���Ա��
//				memcpy(lpmfissueinf->logicnumber, "\x90\x00\x00\x00\x10\x00\x00\x10", 8); //�߼�����
//				memcpy(lpmfissueinf->cardtype,"\x02\x00", 2);
//				memcpy(lpmfissueinf->issuedate, "\x20\x13\x04\x16", 4);
//				memcpy(lpmfissueinf->issuedevice, "\x11\x22\x33\x44\x55\x66", 6);
//				memcpy(lpmfissueinf->cardversion, "\x00\x01", 2);
//				
//				
//				memcpy(lpmfissueinf->effectivefrom, "\x20\x13\x06\x01", 4);
//				memcpy(lpmfissueinf->effectiveto, "\x20\x19\x12\x31", 4);
//				
//				memcpy(lpmfissueinf->effectivefrom, "\x20\x13\x06\x01", 4);//Ӧ����������
//				memcpy(lpmfissueinf->effectiveto, "\x20\x19\x12\x31", 4);//Ӧ�ý�������
			
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

		
		//FID=EF0D������FCI�ļ�
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

		//����FCI�ļ�EF0D
		//88(Ŀ¼�����ļ���SFI��ǩ) 01(����) 01(Ŀ¼�����ļ���SFI)

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

	//��Ӧ7 �����ϼ�¼, ���ݷֱ�Ϊ��ʶ�ͳ���
	unsigned char complex_record[7][2]={
		{0x01,0x30},{0x02,0x30},{0x03,0x30},{0x04,0x30},{0x05,0x30},{0x06,0x30},{0x11,0x18}
	};
	
    int nresult = 0;


	do{
		//ע��ADF1 
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

		//��֤ADF1�Ĵ�����Կ
		if (apdu_getrandom(szdata,8)==0)
		{
			if (apdu_exteralauth(szdata,g_input.key.ADF1_TKEY))
			{
				nresult = -2;
				break;
			}
		}

		
		//����ADF1 
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

		//������Կ�ļ�0000
		memcpy(szcmd, "\x00\xE0\x00\x08\x0B\x00\x00\x08\x18\x14\xC0\x00\x00\x00\x01\x01", 16);
		ulen = 16;
		//dbg_dumpmemory("create key0000 =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("create key0000 |=",szcmd,respone_len);

		//����adf1 ������Կ10 (�ⲿ��֤��Կ)
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

		// ADF1 ����������Կ=====================
		//��Կ����: 40 (������Կ)
		//�汾: 01
		//���: 01-05
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

		// ADF1 ����Ȧ����Կ=====================
		//��Կ����: E0 (Ȧ����Կ)
		//�汾: 02
		//���: 01
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

		
		// ADF1 ����TAC ��Կ=====================
		//��Կ����: 60 (Ȧ����Կ)
		//�汾: 01
		//���: 01
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

		// ADF1 ����ά�� ��Կ=====================
		//��Կ����: 30 (ά����Կ)
		//�汾: 01
		//���: 00 - 03
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


		// ADF1 ����PIN =====================
		//����̶�: 08 00 01
		//��������: 05
		//���볤��: 03
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

		
		// ADF1 ����PIN  ������Կ=====================
		//��Կ�̶�: 20
		//�汾: 01
		//���: 02 ����1108��WRITE KEY/PINָ��˵����
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


		// ADF1 ����PIN ��װ��Կ=====================
		//��Կ�̶�: 20
		//�汾: 01
		//���: 03 ����1108��WRITE KEY/PINָ��˵����
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


		//0011	 Ӧ�ÿ����ļ�
		//�������ļ�(01)���ļ�����=0A
		//�����ɣ�ά����Կ00���£��ļ�����=0A����Ȩ��=00��дȨ��=00
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
		

		//����0011Ӧ�ÿ����ļ�
		if (apdu_getrandom(szdata,4)==0)
		{
			memset(szcmd, 0x00, sizeof(szcmd));
			memcpy(szcmd, "\x04\xD6\x91\x00\x24",5);
			ulen = 5;
			ulen +=32; //����Ϊȫ 00
			MAC_3(g_input.key.MAMK_ADF1_02, ulen, szdata, szcmd, szcmd+ulen);
			ulen += 4;
			//dbg_dumpmemory("update adf1 file_0011 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("update adf1 file_0011 |=",szcmd,respone_len);				
	
		}

		//0012   �����ļ�
		//�������ļ�(01)���ļ�����=08
		//�����ɣ�ά����Կ00���£��ļ�����=08����Ȩ��=00��дȨ��=00	
		memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x12\x01\x00\x20\x0A\x00\x00\x00\x00\x00", 16);
		//dbg_dumpmemory("create adf1 file_0012 =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("create adf1 file_0012 |=",szcmd,respone_len);	
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -18;
			break;
		}
		

//	//0015	 ��������Ӧ�û����ļ�
//	//�������ļ�(01)���ļ�����=09
//	//�����ɣ�ά����Կ00���£��ļ�����=09����Ȩ��=00��дȨ��=00 	
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
//	//����0015����Ӧ�û����ļ�
//	//ע������߼����� sDData = 9000000010000010
//	if (apdu_getrandom(szdata,4)==0)
//	{
//		memset(szcmd, 0x00, sizeof(szcmd));
//		memcpy(szcmd, "\x00\xD6\x95\x00\x1e",5);//22
//		ulen = 5;
//	
//		lppublic = (LPTPUBLICINF)(szcmd+ulen);
//		memcpy(lppublic->issuecode, "\x53\x20", 2);//����������
//		memcpy(lppublic->citycode, "\x41\x00", 2);//���д���
//		lppublic->appstatus = 0x00; 			  //���ñ�ʶ
//		lppublic->appversion = 0x01;			  // Ӧ�ð汾
//		lppublic->testflag = 0x01;				  // ���Ա��
//		memcpy(lppublic->logicnumber, "\x90\x00\x00\x00\x10\x00\x00\x10", 8); //�߼�����
//		memcpy(lppublic->effectivefrom, "\x20\x13\x06\x01", 4);//Ӧ����������
//		memcpy(lppublic->effectiveto, "\x20\x19\x12\x31", 4);//Ӧ�ý�������
//		
//		ulen +=sizeof(TPUBLICINF); //����Ϊȫ 00
//	//	MAC_3(g_input.key.MAMK_ADF1_01, ulen, szdata, szcmd, szcmd+ulen);
//	//	ulen += 4;
//		//dbg_dumpmemory("update adf1 file_0015 =",szcmd,ulen);
//		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
//		//dbg_dumpmemory("update adf1 file_0015 |=",szcmd,respone_len);				
//	
//	}	
//	
//	//0016	 �����ֿ��˻�����Ϣ
//	//�������ļ�(01)���ļ�����=0A
//	//�����ɣ�ά����Կ02���£��ļ�����=0A����Ȩ��=00��дȨ��=00
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
//	//����0016�����ֿ��˻�����Ϣ
//	if (apdu_getrandom(szdata,4)==0)
//	{
//		memset(szcmd, 0x00, sizeof(szcmd));
//		memcpy(szcmd, "\x00\xD6\x96\x00\x38",5);
//		ulen = 5;
//		ulen += 56; //0x38	//����Ϊȫ 00
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



		//0015   ��������Ӧ�û����ļ�
		//�������ļ�(01)���ļ�����=09
		//�����ɣ�ά����Կ00���£��ļ�����=09����Ȩ��=00��дȨ��=00		
		memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x15\x01\x00\x1E\x09\x00\x00\x00\x00\x00", 16);
		//dbg_dumpmemory("create adf1 file_0015 =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("create adf1 file_0015 |=",szcmd,respone_len);
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -19;
			break;
		}
		
	
		//����0015����Ӧ�û����ļ�
		//ע������߼����� sDData = 9000000010000010
		if (apdu_getrandom(szdata,4)==0)
		{
			memset(szcmd, 0x00, sizeof(szcmd));
			memcpy(szcmd, "\x04\xD6\x95\x00\x22",5);
			ulen = 5;
	
			lppublic = (LPTPUBLICINF)(szcmd+ulen);
			memcpy(lppublic->issuecode, g_input.inf.issuecode, 2);//����������
			memcpy(lppublic->citycode, g_input.inf.issuecode, 2);//���д���
			lppublic->appstatus = g_input.inf.appstatus;               //���ñ�ʶ
			lppublic->appversion = g_input.inf.appversion;              // Ӧ�ð汾
			lppublic->testflag = g_input.inf.testflag;                // ���Ա��
			memcpy(lppublic->logicnumber, g_input.logicnumber, 8); //�߼�����
			memcpy(lppublic->effectivefrom, g_input.inf.appeffectivefrom, 4);//Ӧ����������
			memcpy(lppublic->effectiveto, g_input.inf.appeffectiveto, 4);//Ӧ�ý�������

			
//				memcpy(lppublic->issuecode, "\x53\x20", 2);//����������
//				memcpy(lppublic->citycode, "\x41\x00", 2);//���д���
//				lppublic->appstatus = 0x00;               //���ñ�ʶ
//				lppublic->appversion = 0x01;              // Ӧ�ð汾
//				lppublic->testflag = 0x01;                // ���Ա��
//				memcpy(lppublic->logicnumber, "\x90\x00\x00\x00\x10\x00\x00\x10", 8); //�߼�����
//				memcpy(lppublic->effectivefrom, "\x20\x13\x06\x01", 4);//Ӧ����������
//				memcpy(lppublic->effectiveto, "\x20\x19\x12\x31", 4);//Ӧ�ý�������
			
			ulen +=sizeof(TPUBLICINF); //����Ϊȫ 00
			MAC_3(g_input.key.MAMK_ADF1_01, ulen, szdata, szcmd, szcmd+ulen);
			ulen += 4;
			//dbg_dumpmemory("update adf1 file_0015 =",szcmd,ulen);
			respone_len = MifareProCom(ulen, szcmd, &sam_sw);
			//dbg_dumpmemory("update adf1 file_0015 |=",szcmd,respone_len);				
	
		}		

		//0016   �����ֿ��˻�����Ϣ
		//�������ļ�(01)���ļ�����=0A
		//�����ɣ�ά����Կ02���£��ļ�����=0A����Ȩ��=00��дȨ��=00
		memcpy(szcmd, "\x00\xE0\x00\x01\x0B\x00\x16\x01\x00\x38\x0A\x00\x00\x00\x00\x00", 16);
		//dbg_dumpmemory("create adf1 file_0016 =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("create adf1 file_0016 |=",szcmd,respone_len);	
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -20;
			break;
		}

	
		//����0016�����ֿ��˻�����Ϣ
		if (apdu_getrandom(szdata,4)==0)
		{

			memset(szcmd, 0x00, sizeof(szcmd));
			memcpy(szcmd, "\x04\xD6\x96\x00\x3C",5);
			ulen = 5;
			ulen += 56; //0x38  //����Ϊȫ 00
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
		
		//0002   ����Ǯ���ļ�
		//Ǯ���ļ�(02)
		memcpy(szcmd, "\x00\xE0\x00\x22\x0B\x00\x02\x22\x00\x43\x00\x60\x60\x60\x60\x60", 16);
		//dbg_dumpmemory("create adf1 purse =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("create adf1 purse |=",szcmd,respone_len);	
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -20;
			break;
		}

		//0018  ����������ϸ�ļ�
		//�ļ�����=������ϸ�ļ�(32)
		//Ӧ������=00��PBOCӦ�ã�		
		memcpy(szcmd, "\x00\xE0\x00\x32\x0B\x00\x18\x32\x0A\x17\x00\x00\x00\x00\x00\x00", 16);
		//dbg_dumpmemory("create adf1 consume record =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("create adf1 consume record |=",szcmd,respone_len);	
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -20;
			break;
		}

		//0017	ȫ�����������ļ�
		//�ļ�����=TLV��ʽ���Ա䳤��¼�ļ�(05)���ļ�����=0A
		//�����ɣ�ά����Կ02���£��ļ�����=0A����Ȩ��=00��дȨ��=00
		//�ļ�����=0200		
		memcpy(szcmd, "\x00\xE0\x00\x05\x0B\x00\x17\x05\x02\x00\x0A\x00\x00\x00\x00\x00", 16);
		//dbg_dumpmemory("create adf1 complex file =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("create adf1 complex file |=",szcmd,respone_len);	
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -20;
			break;
		}



		//���Ӽ�¼01
		for (i=0;i<7;i++)
		{
			if (apdu_getrandom(szdata,4)==0)
			{
				memset(szcmd, 0x00, sizeof(szcmd));
				memcpy(szcmd, "\x04\xE2\x00\xB8\x34",5);
				szcmd[4] = complex_record[i][1]+4; //�������ݳ���+  MAC 4 �ֽڳ���
				szcmd[5] = complex_record[i][0];   //��¼��ʶ
				szcmd[6] = complex_record[i][1]-2; //��¼ȥ��2�ֽڱ�ͷ�ĳ���
				if (complex_record[i][0]==0x02)
				{
					szcmd[9] =0x09; //�����ͨ�ļ�, e/s��ʼ��+ ����
				}
				
				ulen = 5;
				ulen += complex_record[i][1]; //0x30  //����Ϊȫ 00
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

		//EF0D	����FCI�ļ�
		memcpy(szcmd, "\x00\xE0\x00\x01\x0B\xEF\x0D\x01\x00\x04\x00\x00\x00\x00\x00\x00", 16);
		//dbg_dumpmemory("create adf1 FCI =",szcmd,ulen);
		respone_len = MifareProCom(ulen, szcmd, &sam_sw);
		//dbg_dumpmemory("create adf1 FCI |=",szcmd,respone_len);	
		if ((respone_len==0) || (0x9000!=sam_sw))
		{
			nresult = -20;
			break;
		}


		//���� FCI File
		//9F08(��ǰ�汾�ű�ǩ) 01(����) 02(��ǰ�汾��)		
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
//����: ticket_esanalyze
//����: ��Ʊ�������ļ��ṹ
//��ڲ���:
//                      ��
//���ڲ���:
//                     lpoutdata = Ʊ����������
//����ֵ:
//               0=�ɹ�
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
			//δ����
			break;
		}
		//�ѷ���
		//����bom  ������ȡ����

		nresult = 0;
	
	}while(0);


	return nresult;

	

}



//==============================================================
//����: ticket_init
//����: ��Ʊ�������ļ��ṹ
//��ڲ���:
//                       lpindata =  ����TINPUT �ṹ���������
//���ڲ���:
//                     lpoutdata = Ԥ��ֵ����
//����ֵ:
//               0=�ɹ�
//==============================================================
int ticket_esinit(void *lpindata, void *lpoutdata)
{
	int nresult=0;
	
	issue_setvalue(lpindata);

	nresult = ticket_reset();//ɾ���ṹ����ʽ��������

	if (nresult==0)
	{
		nresult = ticket_create_mf();

		if (nresult)
		{
			nresult = 4041; //����ʵ�������ֵ
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
