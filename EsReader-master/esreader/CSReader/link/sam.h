#ifndef __SAM_H__
#define __SAM_H__

//SAM  ������
#define SAM_BAUDRATE_9600 0
#define SAM_BAUDRATE_19200 1
#define SAM_BAUDRATE_38400 2
#define SAM_BAUDRATE_57600 3
#define SAM_BAUDRATE_115200 4

//SAM  �������ţ���0  ��ʼ
#define SAM_IDX_METRO 0
#define SAM_IDX_OCT   1

#ifdef __cplusplus
extern "C"
{
#endif

//========================================================
//����:��sam  �豸���г�ʼ��,  ��Ӧ������ʱ����Ҫ��
//��ڲ���
//	                ��
//���ڲ���
//                   ��
//����ֵ
//       0=�ɹ�
//      -1=���豸�������
//========================================================
extern int samdev_init(void);

//========================================================
//����:�豸sam  ����������
//��ڲ���
//	 nsamsck=sam����Ӧ����
//    baudrate=sam���������0=9600,1=19200,2=38400,3=57600,4=115200
//���ڲ���
//       ��
//����ֵ
//       ��
//========================================================
extern void sam_setbaud(unsigned char socket,unsigned char baudrate);

//========================================================
//����:��ָ���ۺŵ�sam  �����и�λ
//��ڲ���
//	 nsamsck=sam����Ӧ����
//���ڲ���
//       saminf=6�ֽڵ�sam  ����
//       lpdata=��λ������Ϣ
//����ֵ
//       0=��λʧ��
//       >0��λ�ɹ��󷵻ص�����
//========================================================
extern unsigned char sam_rst(unsigned char socket,unsigned char *data);

//========================================================
//����:����ָ����sam  ��ִ��
//��ڲ���
//	 nsamsck=sam����Ӧ����
//    nlen=Ҫ���͵�ָ���(����ָ��ṹ������)
//    indata=���͵�ָ��������
//���ڲ���
//       outdata=sam��ִ����ɺ󷵻ص�����
//       sam_sw=samִ��״̬��,  һ��0x9000  �ǳɹ�
//����ֵ
//       0=ִ���޷���
//       >0ִ���з��أ�����sam_sw  �ж��Ƿ�ɹ�ִ��
//========================================================
extern unsigned char sam_cmd(unsigned char socket, unsigned char inlen,unsigned char *indata,unsigned char *outdata, unsigned short int *sam_sw);

extern char sam_version(unsigned char *version);

extern void samdev_destory(void);

#ifdef __cplusplus
}
#endif

#endif





