#include <stdint.h>
#include <string.h>
#include "../api/Structs.h"

#ifndef _TICKETES_HEADER_
#define _TICKETES_HEADER_
#pragma pack(push, 1)

//
///////////// ES��ؽṹ /////////////
//
//ES����
typedef struct ES_ANALYZE {
	uint8_t		bIssueStatus;				// ����״̬ 0 �Cδ����;1 �C�ѷ���;2 �C ע��
	uint8_t		bStatus;					// Ʊ��״̬ �� �Ը�¼5����Ʊ״̬��Ϊ׼
	char		cTicketType[4];				// Ʊ������
	char		cLogicalID[20];				// Ʊ���߼�����
	char		cPhysicalID[20];			// Ʊ��������
	uint8_t		bCharacter;					// Ʊ���������� 1��OCT��2��020��3��080��4��UL��5��FM��F:����
	char		cIssueDate[14];				// ��Ʊ����ʱ������ʱ��
	//char		cStartExpire[8];			// ����ʼ��Ч��, YYYYMMDDHHMMSS��
	//char		cEndExpire[8];			    // ���������Ч��, YYYYMMDDHHMMSS�� 
	char		cExpire[8];					// ������Ч��, YYYYMMDDHHMMSS��
	char		RFU[16];					// E/S SAM�߼�����, Ĭ��ֵ:��0000000000��
	long		lBalance;					// ���, ��λΪ��/��(Ĭ��Ϊ0)
	long 		lDeposite;					// Ѻ��
	char		cLine[2];
	char		cStationNo[2];
	char		cDateStart[8];				// �߼���Ч�ڿ�ʼʱ��
	char		cDateEnd[8];				// �߼���Ч�ڽ���ʱ��
	uint8_t		dtDaliyActive[7];			// ����Ʊ����ʱ��
	uint16_t		bEffectDay;					// ����Ʊ��Ч����
	char		cLimitEntryLine[2];			// ���ƽ�վ��·
	char		cLimitEntryStation[2];		// ���ƽ�վվ��
	char		cLimitExitLine[2];			// ���Ƴ�վ��·
	char		cLimitExitStation[2];		// ���Ƴ�վվ��
	char		cLimitMode[3];			    // ����ģʽ

	uint8_t			certificate_iscompany;  //00-���˿�, 01-��λ��
	uint8_t			certificate_ismetro;  //������λ��
	char			certificate_name[20];	// ֤������������
	char			certificate_code[32];	// ֤������
	uint8_t			certificate_type;		// ֤������
	uint8_t			certificate_sex;		// ֤������

	uint16_t		trade_count;		// ���׼�����add by shiyulong in 2013-10-14

	// add by shiyulong on 2016-02-19

	unsigned char cardfactory[4];


}ESANALYZE, * PESANALYZE;

#pragma pack(pop)

#endif

#ifndef _TICKETES_
#define _TICKETES_

extern RETINFO esapi_analyse(uint8_t *lpoutput);

extern RETINFO esapi_init(uint8_t *lpinput, uint8_t *lpoutput);

extern RETINFO esapi_evaluate(uint8_t *lpinput, uint8_t *lpoutput);


extern RETINFO esapi_destroy(uint8_t *lpinput, uint8_t *lpoutput);


extern RETINFO esapi_recode(uint8_t *lpinput, uint8_t *lpoutput);

extern RETINFO esapi_private(uint8_t *lpinput, uint8_t *lpoutput);

extern RETINFO esapi_clear(uint8_t *lpinput, uint8_t *lpoutput);

extern void set_function_idx(int idx );


#endif

unsigned char es_get_issue_status(unsigned char globle_status);
unsigned char es_standard_status(unsigned char globle_status, unsigned char metro_status);