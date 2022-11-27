#include <stdint.h>
#include <string.h>
#include "../api/Structs.h"

#ifndef _TICKETES_HEADER_
#define _TICKETES_HEADER_
#pragma pack(push, 1)

//
///////////// ES相关结构 /////////////
//
//ES分析
typedef struct ES_ANALYZE {
	uint8_t		bIssueStatus;				// 发行状态 0 –未发行;1 –已发行;2 – 注销
	uint8_t		bStatus;					// 票卡状态 － 以附录5“车票状态”为准
	char		cTicketType[4];				// 票卡类型
	char		cLogicalID[20];				// 票卡逻辑卡号
	char		cPhysicalID[20];			// 票卡物理卡号
	uint8_t		bCharacter;					// 票卡物理类型 1：OCT；2：020；3：080；4：UL；5：FM；F:其他
	char		cIssueDate[14];				// 制票日期时间日期时间
	//char		cStartExpire[8];			// 物理开始有效期, YYYYMMDDHHMMSS，
	//char		cEndExpire[8];			    // 物理结束有效期, YYYYMMDDHHMMSS， 
	char		cExpire[8];					// 物理有效期, YYYYMMDDHHMMSS，
	char		RFU[16];					// E/S SAM逻辑卡号, 默认值:’0000000000’
	long		lBalance;					// 余额, 单位为分/次(默认为0)
	long 		lDeposite;					// 押金
	char		cLine[2];
	char		cStationNo[2];
	char		cDateStart[8];				// 逻辑有效期开始时间
	char		cDateEnd[8];				// 逻辑有效期结束时间
	uint8_t		dtDaliyActive[7];			// 多日票激活时间
	uint16_t		bEffectDay;					// 多日票有效天数
	char		cLimitEntryLine[2];			// 限制进站线路
	char		cLimitEntryStation[2];		// 限制进站站点
	char		cLimitExitLine[2];			// 限制出站线路
	char		cLimitExitStation[2];		// 限制出站站点
	char		cLimitMode[3];			    // 限制模式

	uint8_t			certificate_iscompany;  //00-个人卡, 01-单位卡
	uint8_t			certificate_ismetro;  //地铁单位卡
	char			certificate_name[20];	// 证件持有人姓名
	char			certificate_code[32];	// 证件代码
	uint8_t			certificate_type;		// 证件类型
	uint8_t			certificate_sex;		// 证件类型

	uint16_t		trade_count;		// 交易计数，add by shiyulong in 2013-10-14

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