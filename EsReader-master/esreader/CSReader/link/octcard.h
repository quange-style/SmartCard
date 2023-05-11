//extern unsigned char YCT_M1_Trade(unsigned char operation, unsigned char *inputData, unsigned char *prev);

extern unsigned char g_lstphysical[8];

#ifdef __cplusplus
extern "C"
{
#endif
//==========================================================================================
//�� �� ��: octm1_saminit
//��    ��: �Թ���M1����Ӧ��SAM�����г�ʼ�� ���ڶ�д���ϵ�ʱִ��һ�μ���
//��ڲ���:
//          idx_sam:��ǰӦ��SAM����Ӧ�Ŀ������к�(��0��ʼ)
//���ڲ���:
//          lpsamcode:6�ֽ��ն˻����/sam����
//�� �� ֵ:
//          0=�ɹ�
//==========================================================================================
extern unsigned char octm1_saminit(unsigned char idx_sam, unsigned char *lpsamcode);

//==========================================================================================
//�� �� ��: octm1_getdata
//��    ��: ��ȡm1���ؼ�����
//��ڲ���:
//          idx_sam:��ǰӦ��SAM����Ӧ�Ŀ������к�(��0��ʼ)
//     lpcardnumber:Ѱ�������л�ȡ��4�ֽڵ�������
//���ڲ���:
//          lprev:ȡ�õ���������,��������
//					//==================================================
//					    unsigned char szbalance[4];
//					//==================================================
//					    unsigned char type_major;//��Ƭ������ 50�����ÿ�
//					    unsigned char type_minjor;//Ӧ��������
//					                              // 01����ͨǮ����
//					                              // A3�����˿�
//					                              // A4��ѧ����
//					                              // A5��������Ʊ��
//					                              // A6���м���Ʊ��
//					                              // A7������ԱƱ��
//					                              // A8������ѧ����
//					    unsigned char appserial[4]; //Ӧ�ÿ���� bcd
//					    unsigned char sale_dt[5];//����ʱ�� bcd, yymmddhhnn
//					    unsigned char sale_addr[2]; //���۵ص��� bcd,
//					    unsigned char deposit;//Ѻ��,Ԫ
//					    unsigned char appflag;//Ӧ�ñ�ʶ,AA��δ���ã�BB�����ã�
//					//===================
//					    unsigned char verify_dt[3]; //������ yymmdd
//					    unsigned char pursemode; //���ڴ�ֵ���
//					                             //0x00������ר��Ǯ��������Ǯ����δ���ã�
//					                             //0x01��ר��Ǯ�������ã�
//					                             //0x02������Ǯ�������ã�
//					                             //0x03��ר��Ǯ��������Ǯ����������
//					    unsigned char city[2];  //���д���
//					    unsigned char industry[2]; //��ҵ����
//					//=================================================
//					    unsigned char common_count[2]; //����Ǯ�����״���,��λ��ǰ
//					    unsigned char other_count[2];  //ר��Ǯ�����״���,��λ��ǰ
//					    unsigned char dt_trdate[4]; //��������yyyy-mm-dd
//					    unsigned char lock_flag; //04��������������
//					//=================================================
//					    unsigned char tr_type;//��������
//					    unsigned char servicer_code[2];//�����̴���
//					    unsigned char terminal_no[4];   //POS��� 4BYTE;
//					    unsigned char dt_trdate2[3]; //������ʷ��������yy-mm-dd
//					    unsigned char banlance[3];  //����ǰ���
//					    unsigned char tr_amount[2];    //���׽��
//					//==================================================
//						unsigned char metro_inf[14]; //����ҪCRC16;


//�� �� ֵ:
//          0=�ɹ�
//==========================================================================================
//extern unsigned char octm1_getdata(unsigned char idx_sam, unsigned char *lpcardnumber, unsigned char *lprev);

//==========================================================================================
//�� �� ��: octm1_getdata2
//��    ��: ��ȡm1���ؼ�����
//��ڲ���:
//          idx_sam:��ǰӦ��SAM����Ӧ�Ŀ������к�(��0��ʼ)
//     lpcardnumber:Ѱ�������л�ȡ��4�ֽڵ�������
//���ڲ���:
//          lprev:ȡ�õ���������,��������
//					//==================================================
//					    unsigned char szbalance[4];   // ����Ǯ��
//					    unsigned char szbalance2[4]; // ר��Ǯ��
//					//==================================================
//					    unsigned char type_major;//��Ƭ������ 50�����ÿ�
//					    unsigned char type_minjor;//Ӧ��������
//					                              // 01����ͨǮ����
//					                              // A3�����˿�
//					                              // A4��ѧ����
//					                              // A5��������Ʊ��
//					                              // A6���м���Ʊ��
//					                              // A7������ԱƱ��
//					                              // A8������ѧ����
//					    unsigned char appserial[4]; //Ӧ�ÿ���� bcd
//					    unsigned char sale_dt[5];//����ʱ�� bcd, yymmddhhnn
//					    unsigned char sale_addr[2]; //���۵ص��� bcd,
//					    unsigned char deposit;//Ѻ��,Ԫ
//					    unsigned char appflag;//Ӧ�ñ�ʶ,AA��δ���ã�BB�����ã�=02��CPU������
//					//===================
//					    unsigned char verify_dt[3]; //������ yymmdd
//					    unsigned char pursemode; //���ڴ�ֵ���
//					                             //0x00������ר��Ǯ��������Ǯ����δ���ã�
//					                             //0x01��ר��Ǯ�������ã�
//					                             //0x02������Ǯ�������ã�
//					                             //0x03��ר��Ǯ��������Ǯ����������
//					    unsigned char city[2];  //���д���
//					    unsigned char industry[2]; //��ҵ����
//					//=================================================
//					    unsigned char common_count[2]; //����Ǯ�����״���,��λ��ǰ
//					    unsigned char other_count[2];  //ר��Ǯ�����״���,��λ��ǰ
//					    unsigned char dt_trdate[4]; //��������yyyy-mm-dd
//					    unsigned char lock_flag; //04��������������
//					//=================================================
//					    unsigned char tr_type;//��������
//					    unsigned char servicer_code[2];//�����̴���
//					    unsigned char terminal_no[4];   //POS��� 4BYTE;
//					    unsigned char dt_trdate2[3]; //������ʷ��������yy-mm-dd
//					    unsigned char banlance[3];  //����ǰ���
//					    unsigned char tr_amount[2];    //���׽��
//					//==================================================
//						unsigned char metro_inf[14]; //����ҪCRC16;


//�� �� ֵ:
//          0=�ɹ�
//==========================================================================================
unsigned char octm1_getdata2(unsigned char idx_sam, unsigned char *lpcardnumber, unsigned char *lprev);


//==========================================================================================
//�� �� ��: octm1_writedata
//��    ��: ��M1������д����
//��ڲ���:
//            idx_sam:��ǰӦ��SAM����Ӧ�Ŀ������к�(��0��ʼ)
//          nopertion:������ʽ, 0=����������,1=����������(��д������Ϣ��Ҳ���۷ѣ�ֻд���������), 2=ֻ���µ�����Ϣ����=3ֻ�۷Ѳ�����������Ϣ��, =4�۷ѵ�ͬʱд������Ϣ��,
//          tr_dttime:����ʱ�䣬ֻ����Ҫ��ֵʱ��ʹ��, bcd��, yyyymmddhhnnss
//             lvalue:Ҫ���ѵ�ֵ
//          metro_inf:������Ϣ�� 16byte
//
//���ڲ���:
//          ��
//�� �� ֵ:
//          0=�ɹ�
//       0xEE=д��ʧ�ܣ���Ҫʱ�н���ȷ��
//       ����=д��ʧ�ܣ����账��
//==========================================================================================
//extern unsigned char octm1_writedata(unsigned char idx_sam, unsigned char nopertion, unsigned char *tr_dttime, unsigned long lvalue, unsigned char *metro_inf);

//==========================================================================================
//�� �� ��: octm1_writedata2
//��    ��: ��M1������д����
//��ڲ���:
//            idx_sam:��ǰӦ��SAM����Ӧ�Ŀ������к�(��0��ʼ)
//          nopertion:������ʽ, 0=����������,1=����������(��д������Ϣ��Ҳ���۷ѣ�ֻд���������), 2=ֻ���µ�����Ϣ����=3ֻ�۷Ѳ�����������Ϣ��, =4�۷ѵ�ͬʱд������Ϣ��,
//          tr_dttime:����ʱ�䣬ֻ����Ҫ��ֵʱ��ʹ��, bcd��, yyyymmddhhnnss
//       npursetype:Ҫ�����Ǯ�����, 0=����Ǯ��, 1���0ֵ=ר��Ǯ��
//             lvalue:Ҫ���ѵ�ֵ
//          metro_inf:������Ϣ�� 16byte
//
//���ڲ���:
//          ��
//�� �� ֵ:
//          0=�ɹ�
//       0xEE=д��ʧ�ܣ���Ҫʱ�н���ȷ��
//       ����=д��ʧ�ܣ����账��
//==========================================================================================
unsigned char octm1_writedata2(unsigned char idx_sam, unsigned char nopertion, unsigned char *tr_dttime, unsigned char npursetype, unsigned long lvalue, unsigned char *metro_inf);


//==========================================================================================
//�� �� ��: octm1_gettac
//��    ��: ����TAC��
//��ڲ���:
//          idx_sam:��ǰӦ��SAM����Ӧ�Ŀ������к�(��0��ʼ)
//      lpappnumber:��ǰ��Ƭ��Ӧ�ÿ����, ��octm1_getdata�������ѻ�ȡ
//           lpdata:Ҫ����TAC������������� 16byte
//       
//���ڲ���:
//             lptac:Ҫд��ĵ�����Ϣ�� 4byte
//�� �� ֵ:
//          0=�ɹ�
//==========================================================================================
extern unsigned char octm1_gettac(unsigned char idx_sam, unsigned char *lpappnumber, unsigned char *lpdata, unsigned char *lptac);

//==========================================================================================
//�� �� ��: octm1_judge
//��    ��: ��ɳ�������ж�,��m1����ѡ��select����ֵsakΪCPU��ʱ��Ҫ�Դ˿��ж��Ƿ�ɳ��CPU��M1��,
//          ����ܲ���ȡM1���ݿ�����Ϊ�Ǵ�CPU�����������أ����ں������ú�����ж���ʲô��ҵ��CPU��;
//          ����ܶ�ȡM1���ݿ���Ϊ����cpu��m1,�ٶ�m1���ķ��������ñ�־�����ж�,=0x01/0xBB�����Թ���M1����
//          ���̽��д���,���=2��ֱ���߹���CPU�����̣���Ϊ������־�������أ����ں������ú�����ж���ʲô��ҵ��CPU��;
//��ڲ���:
//      lpcardnumber:Ʊ����������, ��searchcard�������ѻ�ȡ
//       
//���ڲ���:
//             ��
//�� �� ֵ:
//          0=�޿�
//		    2=����������m1���̴���
//          4=����������cpu�����̴���
//          3=������Ҫ�����ж����ĸ���ҵ��CPU��
//===========================================================================
unsigned char octm1_judge(unsigned char *lpcardnumber);
unsigned short octm1_judge_ex(unsigned char *lpcardnumber, unsigned char& type);



#ifdef __cplusplus
};
#endif
