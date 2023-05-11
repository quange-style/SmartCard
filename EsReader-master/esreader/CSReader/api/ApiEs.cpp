#include "Api.h"
#include "Publics.h"
#include "TicketUl.h"
#include "Declares.h"
#include "Errors.h"
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdarg.h>

#include "CmdSort.h"




#include "../link/linker.h"
#include "../link/ticketes.h"
#include "../link/myprintf.h"



//static ETPDVC device_type_file	= dvcES;



//add by shiyulong in 2011-10-11, 用于调试信息输出
void Api::Es_Ticket_Analyse(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret					= {0};
    //TicketBase * p_ticket		= NULL;
    //TICKET_PARAM ticket_prm		= {0};
    //TICKET_DATA ticket_data;
    //uint8_t physical_id[8]={0};
    uint8_t buf[500];

    ESANALYZE analyse;
    uint16_t len_analyse		= 0;
//
//	    do
//	    {
//	        memset(&analyse, 0, sizeof(analyse));
//
//	//        ret.wErrCode = TicketBase::search_card(&p_ticket);
//	        ret.wErrCode = TicketBase::search_card(&p_ticket, NULL, NULL, physical_id);
//	        if (ret.wErrCode != 0)	break;
//	        dbg_dumpmemory("cardno=",physical_id,7);
//
//	        ret = p_ticket->read_card(operAnalyse, ticket_prm);
//	        dbg_formatvar("read=%d",ret.wErrCode);
//	        if (ret.wErrCode != 0)
//	        {
//
//	        	if (ret.wErrCode != 2)
//	        	{
//	            //未发行的卡通过上方式取物理卡号
//				p_ticket->get_ticket_infos(ticket_data);
//				memcpy(analyse.cLogicalID, ticket_data.physical_id, 20);
//	            p_ticket->get_physical_id(analyse.cLogicalID);
//
//	            len_analyse = sizeof(analyse);
//	            analyse.bIssueStatus = 0;
//	            ret.wErrCode=0;
//	            }
//	            break;
//	        }
//	        p_ticket->get_ticket_infos(ticket_data);
//
//
//	//			if (!Check_Issued())
//	//			{
//	//				len_analyse = sizeof(analyse);
//	//				analyse.bIssueStatus = 0;
//	//				break;
//	//			}
//
//	//
//	//			dbg_formatvar("ticket_data.certificate_iscompany=%d",ticket_data.certificate_iscompany);
//	//			dbg_formatvar("ticket_data.certificate_ismetro=%d",ticket_data.certificate_iscompany);
//	//			dbg_formatvar("ticket_data.certificate_type=%d",ticket_data.certificate_type);
//	//			dbg_formatvar("ticket_data.certificate_sex=%d",ticket_data.certificate_sex);
//
//
//	        len_analyse = sizeof(analyse);
//
//	        analyse.bStatus = p_ticket->unified_status();
//
//	        // 赋值
//	        analyse.bIssueStatus = 1;
//	        Publics::hexs_to_string(ticket_data.logical_type, sizeof(ticket_data.logical_type), analyse.cTicketType, sizeof(analyse.cTicketType));
//	        memcpy(analyse.cLogicalID, ticket_data.logical_id, sizeof(analyse.cLogicalID));
//	        memcpy(analyse.cPhysicalID, ticket_data.physical_id, sizeof(analyse.cPhysicalID));
//	        //analyse.bCharacter;					// 票卡物理类型 1：OCT；2：020；3：080；4：UL；5：FM；F:其他
//	        Publics::bcds_to_string(CmdSort::m_time_now, 7, analyse.cIssueDate, 14);
//	        Publics::bcds_to_string(ticket_data.phy_peroidE, 4, analyse.cExpire, 8);
//	        memset(analyse.RFU, '0', sizeof(analyse.RFU));
//	        analyse.lBalance = ticket_data.wallet_value;
//	        analyse.lDeposite = ticket_data.deposit;
//	        memset(analyse.cLine, '0', 2);
//	        memset(analyse.cStationNo, '0', 2);
//	        Publics::bcds_to_string(ticket_data.logical_peroidS, 4, analyse.cDateStart, 8);
//	        Publics::bcds_to_string(ticket_data.logical_peroidE, 4, analyse.cDateEnd, 8);
//	        memset(analyse.dtDaliyActive, 0, sizeof(analyse.dtDaliyActive));
//	        analyse.bEffectDay = 0;
//	        Publics::hex_to_two_char(ticket_data.limit_entry_station[0], analyse.cLimitEntryLine);
//	        Publics::hex_to_two_char(ticket_data.limit_entry_station[1], analyse.cLimitEntryStation);
//	        Publics::hex_to_two_char(ticket_data.limit_exit_station[0], analyse.cLimitExitLine);
//	        Publics::hex_to_two_char(ticket_data.limit_exit_station[1], analyse.cLimitExitStation);
//
//	        analyse.certificate_iscompany = ticket_data.certificate_iscompany;
//			analyse.certificate_ismetro= ticket_data.certificate_ismetro;
//			analyse.certificate_sex= ticket_data.certificate_sex;
//			analyse.certificate_type = ticket_data.certificate_type;
//			memcpy(analyse.certificate_code, ticket_data.certificate_code, 32);
//			memcpy(analyse.certificate_name, ticket_data.certificate_name, 20);
//
//	//			dbg_formatvar("analyse.certificate_iscompany=%x",analyse.certificate_iscompany);
//	//			dbg_formatvar("analyse.certificate_ismetro=%x",analyse.certificate_ismetro);
//	//			dbg_formatvar("analyse.certificate_type=%x",analyse.certificate_type);
//	//			dbg_formatvar("analyse.certificate_sex=%x",analyse.certificate_sex);
//	//			dbg_dumpmemory("certificate_name=",ticket_data.certificate_name, 20);
//	//			dbg_dumpmemory("certificate_code=",ticket_data.certificate_code, 32);
//
//
//
//
//	        char szTemp[8] = {0};
//	        sprintf(szTemp, "%03d", ticket_data.limit_mode);
//	        memcpy(analyse.cLimitMode, szTemp, 3);
//
//	    }
//	    while (0);
//	    if (ret.wErrCode != 2)
//	    {
//	    	dbg_formatvar((char *)"p_ticket->get_physical_type()=%d",p_ticket->get_physical_type());
//	    	set_function_idx(p_ticket->get_physical_type());
//	    }
    //dbg_formatvar("ret=%4Xd",ret.wErrCode);
    antenna_switch(0);
    ISO14443A_Init(1);
	len_analyse = sizeof(analyse);
//    ret.wErrCode = esapi_analyse(buf);
	ret = esapi_analyse(buf);
	len_data = organize_data_region(data_to_send, &ret, len_analyse, buf,0,NULL);

	g_Record.log_out(ret.wErrCode, level_error, "Es_Ticket_Analyse=(%04X,%02X)",ret.wErrCode, ret.bNoticeCode);

//    len_data = organize_data_region(data_to_send, &ret, len_analyse, &analyse);
}

void Api::Es_Ticket_Init(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret = {0};
    //uint16_t nresult;
    uint8_t sztmp[500];

	dbg_formatvar("Es_Ticket_Init");

	//g_Record.log_out(0, level_error, "Es_Ticket_Init");

    ret = esapi_init(param_stack, sztmp);

    len_data = organize_data_region(data_to_send, &ret);

	g_Record.log_out(ret.wErrCode, level_error, "Es_Ticket_Init=(%04X,%02X)",ret.wErrCode, ret.bNoticeCode);

}

//ES_Evaluate
void Api::Es_Ticket_Evaluate(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{

    RETINFO ret = {0};
    //uint16_t nresult;
    uint8_t sztmp[500];

	//g_Record.log_out(0, level_error, "Es_Ticket_Evaluate");

    ret = esapi_evaluate(param_stack, sztmp);

    len_data = organize_data_region(data_to_send, &ret);

	g_Record.log_out(ret.wErrCode, level_error, "Es_Ticket_Evaluate=(%04X,%02X)",ret.wErrCode, ret.bNoticeCode);

}

//ES_Destroy
void Api::Es_Ticket_Destroy(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret = {0};
    //uint16_t nresult;
    uint8_t sztmp[500];

	//g_Record.log_out(0, level_error, "Es_Ticket_Destroy");

    ret = esapi_destroy(param_stack, sztmp);
    len_data = organize_data_region(data_to_send, &ret);
	g_Record.log_out(ret.wErrCode, level_error, "Es_Ticket_Destroy=(%04X,%02X)",ret.wErrCode, ret.bNoticeCode);
}
//ES_Recode
void Api::Es_Ticket_Recode(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret = {0};
    //uint16_t nresult;
    uint8_t sztmp[500];

	//g_Record.log_out(0, level_error, "Es_Ticket_Recode");

    ret = esapi_recode(param_stack, sztmp);

    len_data = organize_data_region(data_to_send, &ret);

	g_Record.log_out(ret.wErrCode, level_error, "Es_Ticket_Recode=(%04X,%02X)",ret.wErrCode, ret.bNoticeCode);
}

void Api::Es_Ticket_Private(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    //unsigned char ulen;
    //unsigned char respone_len;
    unsigned char sztmp[256];
    //unsigned char szcmd[256];
    //unsigned char szdata[8];
    //unsigned char tmp_len;
    //unsigned char sam_len;
    //unsigned short respone_sw;
    //unsigned char inf[100];
    //int nresult = 0;

    RETINFO ret = {0};
//	    do
//	    {
//
//	        memset(inf, 0x00, sizeof(inf));
//	        memcpy(inf, param_stack+16, 56);
//
//	        if (svt_selectfile(0x1001)<0)
//	        {
//	            ret.wErrCode = ERR_CARD_READ;
//	            break;
//	        };
//
//	        memcpy(g_input.key.ADF1_MAMK_02, l_key.ADF1_MAMK_02, 16);
//
//	        //更新0016创建持卡人基本信息
//	        if (apdu_getrandom(szdata,4)==0)
//	        {
//
//	            memset(szcmd, 0x00, sizeof(szcmd));
//	            memcpy(szcmd, "\x04\xD6\x96\x00\x3C",5);
//	            ulen = 5;
//	            memcpy(szcmd+5, inf, 56);
//	            ulen += 56; //0x38  //数据为全 00
//	            MAC_3(g_input.key.ADF1_MAMK_02, ulen, szdata, szcmd, szcmd+ulen);
//	            ulen += 4;
//	            //dbg_dumpmemory("update adf1 file_0016 =",szcmd,ulen);
//	            respone_len = MifareProCom(ulen, szcmd, &respone_sw);
//	            //dbg_dumpmemory("update adf1 file_0016 |=",szcmd,respone_len);
//	            //dbg_formatvar("update adf1 file_0016 |= %04X",sam_sw);
//
//	            if ((respone_len==0) || (0x9000!=respone_sw))
//	            {
//	                ret.wErrCode = ERR_TICKET_WRITE ;
//	                break;
//	            }
//	            Beep_Sleep(1);
//
//	        }
//
//	    }
//	    while (0);
	//dbg_dumpmemory("private=",param_stack, 72);
	
	//g_Record.log_out(0, level_error, "Es_Ticket_Private");
    ret = esapi_private(param_stack, sztmp);

    len_data = organize_data_region(data_to_send, &ret);

	g_Record.log_out(ret.wErrCode, level_error, "Es_Ticket_Private=(%04X,%02X)",ret.wErrCode, ret.bNoticeCode);
}

void Api::Es_Ticket_Clear(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret = {0};
    //uint16_t nresult;
    uint8_t sztmp[500];

	//g_Record.log_out(0, level_error, "Es_Ticket_Clear");

    ret = esapi_clear(param_stack, sztmp);


    len_data = organize_data_region(data_to_send, &ret);

	g_Record.log_out(ret.wErrCode, level_error, "Es_Ticket_Clear=(%04X,%02X)",ret.wErrCode, ret.bNoticeCode);
}



void estest(void)
{

    //unsigned char sztmp[200];
    int nresult=-1;



    do
    {

//	        antenna_switch(0);
//	        ISO14443A_Init(1);
//	//dbg_formatvar("searchcard=%d",searchcard(sztmp));
//	//dbg_formatvar("active=%d",svt_active(sztmp));
//	        issue_setvalue(sztmp);
//
//
//	//删除结构，正式环境不用
//	        if (ticket_reset() != 0)
//	        {
//	            nresult = ERR_CLEAR_FILE;
//	            break;
//	        }
//
//	        if (ticket_create_mf() != 0)
//	        {
//	            nresult = ERR_CREATE_MF;
//	            break;
//	        }
//
//	        if (ticket_create_adf1() != 0)
//	        {
//	            nresult = ERR_CREATE_ADF;
//	            break;
//	        }
        nresult = 0;
    }
    while (0);
//dbg_formatvar("nresult=%04X",nresult);

}


