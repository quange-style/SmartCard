#ifndef _TOKEN_H_
#define _TOKEN_H_

#ifdef __cplusplus
extern "C"
{
#endif


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
extern int ticket_esanalyze(void *lpoutdata);


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
extern int ticket_esinit(void *lpindata, void *lpoutdata);

#ifdef __cplusplus
}
#endif

#endif



