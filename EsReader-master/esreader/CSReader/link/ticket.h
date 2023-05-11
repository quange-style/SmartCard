#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <stdint.h>

typedef struct
{
	unsigned char physicalnumber[8];
	unsigned char cardtype[2];
	unsigned char testflag;
	unsigned char balance[4];
	unsigned char logicnumber[8];
	unsigned char mtrinf[48];
}TMETROINF,*LPTMETROINF;

extern unsigned char g_ant_type;

#ifdef __cplusplus
extern "C"
{
#endif


	//========================================================
	//����:�Ե���SAM  ���г�ʼ��,  һ���豸�ϵ���빤��״̬ǰ����һ��
	//��ڲ���
	//	 nsamsck=sam����Ӧ����
	//���ڲ���
	//       saminf=6�ֽڵ�sam  ����
	//����ֵ
	//       0=�ɹ�
	//========================================================
	extern int sam_metro_active(int nsamsck, unsigned char * p_len_sam_id, unsigned char * p_sam_id, unsigned char * p_len_terminal_id, unsigned char * p_terminal_id);

	//========================================================
	//����:������Ʊ����
	//��ڲ���
	//	 nsamsck=sam����Ӧ����
	//���ڲ���
	//       lpbuffer=������Ʊ�����ݣ��ӵ�4ҳ����15ҳ(48BYTE)
	//����ֵ
	//       0=�����ɹ�, ������������crc  ����ȷ
	//       1=�����ɹ�, ��һ��������crc  ����
	//       2=�����ɹ�, �ڶ���������crc  ����
	//       3=�����ɹ�, ����������crc  ������
	//      -1=�޿�
	//      -2=������ʧ��
	//      -5=����sam  ��ʧ��
	//========================================================
	extern int readul(int nsamsck, unsigned char *lpbuffer);

	//========================================================
	//����:д����Ʊ����
	//��ڲ���
	//        nsamsck=sam����Ӧ����
	//        nzone=��������0-����������0��1-����������0
	//        lpbuffer=16�ֽڴ��������ݣ�
	//���ڲ���
	//       ��
	//����ֵ
	//       0=�ɹ�
	//      -3=д��ʧ��
	//========================================================
	extern int writeul(int nsamsck, int nzone, unsigned char *lpbuffer);
	extern uint16_t writeul_new(int nsamsck, int nzone, unsigned char *lpbuffer);
	extern uint16_t add_ul_crc8(unsigned char data_point, unsigned char * p_area_data);

	//========================================================
	//����:�Ե���SAM  ���й��ܼ��ֻ�м������Ĳ��ܽ��з������ڽ���
	//��ڲ���
	//	 nsamsck=sam����Ӧ����
	//    direct=���ݷ���, 0=��ʾȡ������Ϣ������Ϣ�跢�͸�������; 1=��ʾ�����������������������Ϣ���͵�SAM  ��
	//    lpinf=��direct=1ʱΪ���� ����
	//���ڲ���
	//       ��
	//����ֵ
	//       0=�ɹ�
	//========================================================
	extern int sam_metro_function_active(int nsamsck, int direct, unsigned char *lpinf);

	//========================================================
	//����:ѡ��Ӧ��Ŀ¼
	//��ڲ���
	//	 aid=Ӧ��Ŀ¼��ʶ, 0x3F00=��Ӧ��Ŀ¼,  0x1001=ADF1Ŀ¼, 0x1002=ADF2Ŀ¼
	//���ڲ���
	//       ��
	//����ֵ
	//       >=0�ɹ�
	//========================================================
	int svt_selectfile(unsigned short aid);


	//========================================================
	//����: ���������ļ�,  Ҫ�Ƚ�����Ӧ��Ӧ��Ŀ¼������ȷ��ȡ
	//��ڲ���
	//	 aid=���ļ���ʶ, 0x05=�����ļ�,  0x11=Ӧ�ÿ����ļ���0x15=������Ϣ�ļ�,  0x16=�ֿ��˸�����Ϣ�ļ�
	//    start_offset:��ȡ�ļ�����ʼƫ�Ƶ�ַ
	//    len:Ҫ��ȡ�ĳ���
	//���ڲ���
	//       lpdata: ��ȡ�����ݻ���
	//����ֵ
	//       >=0�ɹ�
	//========================================================
	int svt_readbinary(unsigned char aid, unsigned char start_offset, unsigned char len, unsigned char *lpdata);

	//========================================================
	//����: ����¼�ļ�,  Ҫ�Ƚ�����Ӧ��Ӧ��Ŀ¼������ȷ��ȡ
	//��ڲ���
	//	 aid=���ļ���ʶ, 0x17=�����ļ���0x18=��ʷ��ϸ
	//    record_no:Ҫ��ȡ�ļ���¼��
	//    len:Ҫ��ȡ�ĳ���
	//���ڲ���
	//       lpdata: ��ȡ�����ݻ���
	//����ֵ
	//       >=0�ɹ�
	//========================================================
	int svt_readrecord(unsigned char aid, unsigned char record_no, unsigned char len, unsigned char *lpdata);

	//========================================================
	//����: ����ʷ��ϸ,  Ҫ�Ƚ�����Ӧ��Ӧ��Ŀ¼������ȷ��ȡ
	//��ڲ���
	//	 aid=���ļ���ʶ, 0x17=�����ļ���0x18=��ʷ��ϸ
	//    record_start: Ҫ��ȡ��ʼ��¼��
	//      record_cnt:Ҫ��ȡ�ļ�¼����
	//���ڲ���
	//       lpdata: ��ȡ�����ݻ���
	//����ֵ
	//       >=0�ɹ�
	//========================================================
	int svt_readhistory(unsigned char aid, unsigned char record_start, unsigned char record_cnt, unsigned char *lpdata);


	//========================================================
	//����: ��Ǯ��ֵ,  Ҫ�Ƚ�����Ӧ��Ӧ��Ŀ¼������ȷ��ȡ
	//��ڲ���
	//	 ��
	//���ڲ���
	//       lpdata: 4�ֽڵ�Ǯ��ֵ,  ���ֽ���ǰ
	//����ֵ
	//       >=0�ɹ�
	//========================================================
	int svt_getbalance(unsigned char *lpdata);

	//========================================================
	//����: ȡ�������׼���
	//��ڲ���
	//	 nsamsck: ����SAM  ����Ӧ�Ŀ���,  ��0  ��ʼ
	//���ڲ���
	//       lpdata: 2�ֽڵ��������к�,  ���ֽ���ǰ
	//����ֵ
	//       >=0�ɹ�
	//========================================================
	int svt_getonlineserial(int nsamsck,unsigned char *lpserial);

	//========================================================
	//����: ȡ�ѻ����׼���
	//��ڲ���
	//	 nsamsck: ����SAM  ����Ӧ�Ŀ���,  ��0  ��ʼ
	//���ڲ���
	//       lpdata: 2�ֽڵĽ������к�,  ���ֽ���ǰ
	//����ֵ
	//       >=0�ɹ�
	//========================================================
	int svt_getofflineserial(int nsamsck,unsigned char *lpserial);



	//=================================================================================
	//�� �� ��: svt_read
	//��    ��: ��ȡ����Ʊ����������
	//��ڲ���:
	//          nmode:������ʽ,=0��ͨ��ʽ, =1ȫ����ʽ����Ҫ��ȡ�ֿ�����Ϣ��������ʷ
	//���ڲ���:
	//          lpcardinf:���ص����ݻ�����
	//          ��nmode=0ʱ
	//            ���л�����Ϣ:40Byte
	//            ������Ϣ:30Byte
	//            ���׸�����Ϣ:48Byte
	//            ������Ϣ:48Byte
	//            Ӧ�ÿ�����Ϣ:32Byte
	//            Ǯ��ֵ:4Byte
	//            ���һ����ʷ��¼:23Byte
	//          ��nmode=1ʱ
	//            ���л�����Ϣ:40Byte
	//            ������Ϣ:30Byte
	//            ���׸�����Ϣ:48Byte
	//            ������Ϣ:48Byte
	//            Ӧ�ÿ�����Ϣ:32Byte
	//            Ǯ��ֵ:4Byte
	//            �ֿ��˸�����Ϣ:23Byte
	//            10����ʷ��¼:230Byte
	//�� �� ֵ:
	//        >0: ���ص����ݳ���
	//        <0: ��������
	//=================================================================================
	extern int svt_read(int opertion, unsigned char *lpcardinf);

	//========================================================
	//����:�Ե���svt �����г�ֵ
	//��ڲ���
	//	 nsamsck=sam����Ӧ����
	//    nstep=��=0ʱ��ʾ��ȡ��ֵ��Ϣ��=1ʱ��ʾ��Ʊ�����г�ֵ����
	//    value=Ҫ�䵽Ʊ���ڵĽ��(  ��)
	//    lpinf =��nstep=1ʱΪ��������
	//            �������ݽṹ:
	//                       ����ʱ��7Byte
	//                        MAC2   4Byte
	//
	//���ڲ���
	//       pinf = �������
	//             ������ݽṹ:
	//                               ���4Byte,
	//                       �����������2Byte,
	//                           ��Կ�汾1Byte,
	//                           �㷨��ʶ1Byte,
	//                           α�����4Byte,
	//                             MAC1  4Byte

	//����ֵ
	//       0=�ɹ�
	//========================================================
	extern int svt_recharge(unsigned char trType, unsigned char * p_timenow, int nsamsck, int nstep, unsigned long value, unsigned char *lpinf);

	//========================================================
	//����:�Ե���Ʊ�����ѻ�����
	//��ڲ���
	//    nsamsck=sam����Ӧ����
	//    value = ���ѽ��
	//    trtype=��������, 06=��ͨ����,09=��������
	//    lpmetrofile=��trtype=09  ʱ����Ϊ�����ļ����µ��������ݣ���ͨ����ʱ��ʹ��
	//���ڲ���
	//       respone_data=SAM�ն˺�(6BYTE) +  SAM�ն���ˮ( 4Byte) + TAC (4Byte)
	//����ֵ
	//       0=�ɹ�
	//========================================================
	extern int svt_consum(unsigned char * p_timenow, int nsamsck, unsigned char *lplogicnumber,unsigned long value,unsigned char trtype, unsigned char *lpmetrofile, unsigned char *respone_data);

	//===============================================================
	//����:��SVTƱ��Ӧ�ÿ����ļ����и���
	//��ڲ���
	//	 nsamsck=sam����Ӧ����
	//       file_id=���ļ���ʶ��Ŀǰֻ֧��0x11(Ӧ�ÿ����ļ�)��0x16(������Ϣ�ļ�)
	//        offset=Ҫ�����ļ�����ʼ��ַ
	//           nle=Ҫ���µ����ݳ���
	//         lpinf=Ҫд�������

	//
	//���ڲ���
	//	��
	//����ֵ
	//       0=�ɹ�
	//===============================================================
	extern int svt_update_binfile(int nsamsck,unsigned char *lplogicnumber,int file_id, int offset, int nlen, unsigned char *lpinf);

	//========================================================
	//����:��Ѱ���߸�Ӧ��Ʊ��
	//��ڲ���
	//	 ��
	//���ڲ���
	//       lpcardno=����
	//����ֵ
	//       0=�޿�
	//       1=UL
	//       2=M1��
	//       3=CPU��
	//      99=�࿨
	//========================================================
	extern int searchcard(unsigned char *lpcardno);
	extern unsigned short search_card_ex(unsigned char *lpcardno, unsigned char& type);
	extern int searchcard_es(unsigned char *lpcardno);

	//=================================
	//����: CPU ����λ,  ��ǰ��������searchcard����
	//��ڲ���:  ��
	//���ڲ���:
	//                         lpinf: ��λ��Ϣ
	//����ֵ:
	//                =0�ɹ�
	//=================================
	extern int svt_active(unsigned char *lpinf);

	//========================================================
	//����:�Ե���Ʊָ���ļ�¼���и���
	//��ڲ���
	//    nsamsck=sam����Ӧ����
	//    lplogicnumber=Ҫ����Ʊ����8�ֽ��߼�����(Ӧ�����к�)
	//    record_id=�ĵ��ж�Ӧ�ļ�¼��
	//    lock=������ʶ,=0������
	//    lpfile=�����ļ����µ���������
	//���ڲ���
	//       respone_data=SAM�ն˺�(6BYTE) +  SAM�ն���ˮ( 4Byte) + TAC (4Byte)
	//����ֵ
	//       0=�ɹ�
	//========================================================
	extern int svt_updata_complex(unsigned char * p_timenow, int nsamsck, unsigned char *lplogicnumber, unsigned char record_id, unsigned char lock, unsigned char *lpfile, unsigned char *respone_data);


	//===============================================================
	//����:��SVTƱ�ĸ����ļ�ָ����¼���и���
	//��ڲ���
	//	 nsamsck=sam����Ӧ����
	//  lplogicnumber=Ʊ���߼�����
	//        record_id=��¼ID
	//           lock=�������
	//         lpinf=�����ļ���ӦҪд�������(Ҫ���ȡʱ��õ���ͬ)

	//
	//���ڲ���
	//	��
	//����ֵ
	//       0=�ɹ�
	//===============================================================
	extern int svt_unlock_recordfile(int nsamsck,unsigned char *lplogicnumber,int record_id, unsigned char lock, unsigned char *lpinf);

	//========================================================
	//����:�Ե���Ʊָ���ļ�¼���и���
	//��ڲ���
	//    nsamsck=sam����Ӧ����
	//    lplogicnumber=Ҫ����Ʊ����8�ֽ��߼�����(Ӧ�����к�)
	//    recordcnt=Ҫ���µļ�¼����
	//    dim_record_id=Ҫ���µĶ�����¼������
	//    dim_lock=��Ӧ������¼��������ʶ����,=0������
	//    dim_lpfile=�����ļ����µĶ�����¼���������ݣ���ֵʱ��Ҫע�ⲻͬ�ļ�¼�Ŷ�Ӧ�����ݳ���( ��������Ϊ������¼ǰ�����ض����ݵ�����)
	//���ڲ���
	//       respone_data=SAM�ն˺�(6BYTE) +  SAM�ն���ˮ( 4Byte) + TAC (4Byte)
	//����ֵ
	//       0=�ɹ�
	//========================================================
	extern uint16_t svt_updata_more_complex(unsigned char tac_type, unsigned char * p_time_now, int nsamsck, 
		unsigned char *lplogicnumber, long lAmount, unsigned char recordcnt,
		unsigned char *dim_record_id, unsigned char *dim_lock, unsigned char *dim_lpfile, unsigned char *respone_data);

	typedef struct
	{
		unsigned char	id;
		unsigned char	lock_flag;
		unsigned char	len;
		unsigned char	data[255];
	}COMPLEX_FILE, * PCOMPLEX_FILE;

	extern uint16_t updata_more_complex(unsigned char * p_time_now, int nsamsck, unsigned char *lplogicnumber, long lAmount, unsigned char recordcnt,
		PCOMPLEX_FILE complex_arr, unsigned char *respone_data);

	//========================================================
	//����:��һ��ͨƱ�����ѻ�����
	//��ڲ���
	//	 nsamsck=sam����Ӧ����
	//    value = ���ѽ��
	//    trtype=��������, 06=��ͨ����,09=��������
	//    lpmetrofile=��trtype=09  ʱ����Ϊ�����ļ����µ��������ݣ���ͨ����ʱ��ʹ��
	//���ڲ���
	//       respone_data=SAM�ն˺�(6BYTE) +  SAM�ն���ˮ( 4Byte) + TAC (4Byte)
	//����ֵ
	//       0=�ɹ�
	//========================================================
	extern int oct_consum(unsigned char * p_timenow, int nsamsck, unsigned char *lplogicnumber, unsigned long value,unsigned char trtype, unsigned char *lpmetrofile, unsigned char *respone_data);

	//========================================================
	//���ܣ�����CPU���ǵ���CPU���ǹ���CPU
	//========================================================
	extern int distinguish_cpu(unsigned char * p_pid);
	extern unsigned short distinguish_cpu_ex(unsigned char * p_pid, unsigned char& type);

	//========================================================
	//����:��OCT�ӽ���
	//��ڲ���
	//   nsamsck=sam����Ӧ����
	//    lplogicnumber = Ʊ��Ӧ�����к�
	//    lpcitycode=���д���
	//    optflag=��������(1-����,0-����)
	//���ڲ���
	//       ��
	//����ֵ
	//       0=�ɹ�
	//ע:�ڿ��ж�
	//��3F00Ӧ��Ŀ¼����00B203D400��ǰ���ֽ�Ϊ0x53,0x4B��Ϊ�������ڿ�
	//========================================================
	extern int oct_cpu_lock(int nsamsck, unsigned char *lplogicnumber, unsigned char *lpcitycode, unsigned char optflag);

	//=============================================================================================================
	//�� �� ��: generate_tac
	//��    ��: ���㵥��ƱTAC��
	//��ڲ���:
	//          nsamsck: sam����Ӧ����
	//   *lplogicnumber: 8byte�߼�����
	//            money: 4byte���׽��,long��
	//   trade_datetime: 7byte����ʱ��
	//���ڲ���:
	//        lpresult:������(samid(6B)+dsn(4B)+tac(4B))��������ֵ=0ʱ��Ч
	//����ֵ:
	//       =0�ɹ�
	//=============================================================================================================
	extern int generate_tac(int nsamsck, unsigned char *lplogicnumber, unsigned long money, unsigned char *trade_datetime, unsigned char *lpresult);

	//========================================================
	//����:ͨ��aid ѡ��Ӧ��Ŀ¼
	//��ڲ���
	//	 aid=Ӧ��Ŀ¼��ʶ, 
	//���ڲ���
	//       ��
	//����ֵ
	//       >=0�ɹ�
	//========================================================
	extern int svt_selecfileaid(unsigned char ulen, unsigned char *lpaid);

	//========================================================
	//����:��ȡCPU���ϴν��׵�TAC��
	//��ڲ���
	//	 aid=Ӧ��Ŀ¼��ʶ, 
	//���ڲ���
	//       ��
	//����ֵ
	//       =0�ɹ�
	//========================================================
	extern int get_cpu_last_tac(uint16_t off_line_counter, uint8_t tac_type, unsigned char * p_recv);

	//========================================================
	//����:CPU��pinУ��
	//���ڲ���
	//       ��
	//����ֵ
	//       =0�ɹ�
	//========================================================
	extern int mtr_cpu_pin_check();

	//========================================================
	//����:��ʼ��SAM��ȫ�������Ϣ
	//���ڲ���
	//       ��
	//����ֵ
	//========================================================
	extern void init_sam_inf();

#ifdef __cplusplus
}
#endif


#endif

