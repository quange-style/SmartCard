#include "Api.h"
#include "Publics.h"
#include "ParamMgr.h"
#include "Declares.h"
#include "Errors.h"
#include "Records.h"
#include "TicketBase.h"
#include "../link/myprintf.h"
#include "../link/SerialFile.h"

static ETPDVC device_type_file	= dvcBidirectAGM;

extern CLoopList	g_LoopList;
//
// 闸机接口
//
void Api::Gate_AisleModel(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret					= {0};

	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		if (param_stack[0] < 1 || param_stack[0] > 3)
		{
			ret.wErrCode = ERR_INPUT_PARAM;
			break;
		}

		ret.wErrCode = api_match_device(device_type_file);
		if (ret.wErrCode != 0)
		{
			ret.wErrCode = api_match_device(dvcEntryAGM);
			if (ret.wErrCode != 0)
			{
				ret.wErrCode = api_match_device(dvcExitAGM);
				if (ret.wErrCode != 0)
					break;
			}
		}

		door_type = param_stack[0];

	}
	while (0);

	len_data = organize_data_region(data_to_send, &ret);
}

void Api::Gate_EntryFlow(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret					= {0};
    TicketBase * p_ticket		= NULL;
    TICKET_PARAM ticket_prm		= {0};
    TICKET_DATA ticket_data;

    MODE_EFFECT_FLAGS md_effect	= {0};
    ENTRYGATE entry;
	ETICKETDEALINFO eticketdealinfo;
	ETICKETAUTHINFO eticketauthinfo;
	char cOnlineRes = '1';			//联机验票结果，0为成功 非0为验证失败
    TICKETLOCK locked;
    uint16_t len_entry			= 0;
    uint16_t len_lock			= 0;
	uint16_t len_eticket		= 0;
	uint8_t current_id[7]		= {0};

	long time_use=0;
	struct timeval start;
	struct timeval end;
	long msec = 0;

	gettimeofday(&start,NULL);

    do
    {
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0) break;

		memset(&ticket_data, 0, sizeof(ticket_data));
        memset(&entry, 0, sizeof(entry));
        memset(&locked, 0, sizeof(locked));
		memset(&eticketdealinfo, 0, sizeof(eticketdealinfo));
		memset(&eticketauthinfo, 0, sizeof(eticketauthinfo));

        ret.wErrCode = api_match_device(device_type_file);
        if (ret.wErrCode != 0)
        {
        	ret.wErrCode = api_match_device(dvcEntryAGM);
        	if (ret.wErrCode != 0) break;
        }

        ret.wErrCode = TicketBase::search_card(&p_ticket, NULL, NULL, current_id, operEntry);
        if (ret.wErrCode != 0)
		{
			memset(locked_entry_id, 0, sizeof(locked_entry_id));
			break;
		}

		g_Record.log_buffer("Gate_EntryFlow testTmpData = ", Api::testTmpData, 14,level_normal);

		p_ticket->get_ticket_infos(ticket_data);

		if(ticket_data.qr_flag == 0x00)
		{
			if (memcmp(current_id, locked_entry_id, sizeof(current_id)) == 0)
			{
				ret.wErrCode = ERR_CARD_NONE;
				break;
			}

			memcpy(locked_entry_id, current_id, sizeof(locked_entry_id));
		}

        ret = p_ticket->read_card(operEntry, ticket_prm);
        if (ret.wErrCode != 0)	break;

        p_ticket->get_ticket_infos(ticket_data);

		p_ticket->get_degrade_mode(md_effect, CmdSort::m_time_now);
		if (md_effect.emergency)
		{
			len_entry = 0;
			break;
		}

        // 分析票卡开始
        ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
        if (ret.wErrCode)	break;

		// 检查专用通道
		ret.wErrCode = g_Parameter.passageway_allow_pass(ticket_data.logical_type, door_type);
		if (ret.wErrCode)	break;

        // 设备支持
        ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
        if (ret.wErrCode != 0)	break;

		//检查是否重复过闸 如果是电子票卡的话，需要检查重复使用二维吗

		if(ticket_data.qr_flag != 0x00)
		{
			if(g_LoopList.QueryList((char *)ticket_data.timeAndcenterCode,operEntry))
			{
                g_Record.log_out(0, level_error,"this is the same qr code");
				ret.wErrCode = ERR_CARD_NONE;
				memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
				memset(Api::clear_qr_readbuf,0x00,sizeof(Api::clear_qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
				break;
			}else{
				g_Record.log_out(0, level_error,"this is the same qr code else");
			}
		}

        ret = p_ticket->analyse_common(2, &md_effect, &ticket_prm, &locked, len_lock, NULL, &entry.cEntryMode);
        if (ret.wErrCode != 0)	break;

        ret = p_ticket->analyse_free_zone(&md_effect, &ticket_prm, NULL, NULL);

		// 交易确认
		if (ret.wErrCode != 0)
		{

			if(ticket_data.qr_flag == 0x00)
			{
				if (!p_ticket->confirm_current_card())
					break;

				if (!p_ticket->check_confirm_his(10))
					break;
			}

			if (!p_ticket->last_trade_need_continue(TicketBase::MS_Entry, ERR_ENTRY_EVER, ret.wErrCode)) break;

			ret = p_ticket->continue_last_trade(&entry);
		}
		else
		{
			if(ticket_data.qr_flag == 0x00){
				ret = p_ticket->entry_gate(&entry,&eticketdealinfo,cOnlineRes);
			}
			else{
				ret = p_ticket->check_online(operEntry,&eticketauthinfo);
			}
		}

		if (ret.wErrCode)	break;

		if(ticket_data.qr_flag == 0x00)
		{
			len_entry = sizeof(entry);
			p_ticket->clear_confirm(entry.cLogicalID, true, false);
		}
		else
		{
			len_eticket = sizeof(eticketauthinfo);
		}

    }
    while (0);

	if(ret.wErrCode == ERR_NEED_CONTINUE_LAST)//交易确认
	{
		p_ticket->get_ticket_infos(ticket_data);
		ret = p_ticket->continue_last_trade(&eticketdealinfo);
		//g_LoopList.UpdateList((char *)ticket_data.timeAndcenterCode,operEntry);
		len_eticket = sizeof(eticketdealinfo);
		p_ticket->clear_confirm_e(eticketdealinfo.cCenterCode, true, false);
		ret.bRfu[0] = 0x11;
		ret.bRfu[1] = 0x00;
	}
	//FIXME: 不管成功与否，都添加如队列 如果是重复的码就直接返回无卡
	if(ticket_data.qr_flag != 0x00&&ret.wErrCode != ERR_SUCCEED ){
		g_LoopList.UpdateList((char *)ticket_data.timeAndcenterCode,operEntry);
	}
	g_Record.add_record(locked.cTradeType, &locked, len_lock);
	if(ticket_data.qr_flag == 0x00){
		g_Record.add_record(entry.cTradeType, &entry, len_entry);
	}
	else{
		g_Record.add_record("60", &eticketauthinfo, len_eticket);
	}

	if (ret.wErrCode != ERR_CARD_NONE)
	{
		memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
		memset(Api::clear_qr_readbuf,0x00,sizeof(Api::clear_qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
	}

	if (ret.wErrCode)
	{
		if (ret.wErrCode == ERR_CARD_READ/* || ret.bNoticeCode == NTC_MUST_CONFIRM*/)
		{
			ret.wErrCode = ERR_CARD_NONE;
			ret.bNoticeCode = 0;
			memset(locked_entry_id, 0, sizeof(locked_entry_id));
		}
	}

	if(ticket_data.qr_flag == 0x00){
		len_data = organize_data_region(data_to_send, &ret, len_entry, &entry, len_lock, &locked);
	}
	else{
		len_data = organize_data_region(data_to_send, &ret, len_eticket, &eticketauthinfo, len_lock, &locked);
	}

	gettimeofday(&end,NULL);
	time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
	msec = time_use / 1000;

	if(ret.wErrCode != ERR_CARD_NONE){
		g_Record.log_out(ret.wErrCode, level_disaster,"Gate_EntryFlow(lid=%s)=(%04X,%02X) use time = [%ld]", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode,msec);
	}

}

void Api::Gate_ExitFlow(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret					= {0};
    TicketBase * p_ticket		= NULL;
    TICKET_PARAM ticket_prm		= {0};
    TICKET_DATA ticket_data;

    MODE_EFFECT_FLAGS md_effect	= {0};
    PURSETRADE exit;
	ETICKETDEALINFO eticketdealinfo;
	ETICKETAUTHINFO eticketauthinfo;
    TICKETLOCK locked;
    uint16_t len_exit			= 0;
    uint16_t len_lock			= 0;
	uint16_t len_eticket		= 0;
	uint8_t forbid_ant			= 0;
	ETPMDM forbid_type			= mediUltraLight;
	uint8_t current_id[7]		= {0};
	char cOnlineRes = '1';		//联机验票结果，0为成功 非0为验证失败

    long time_use=0;
	struct timeval start;
	struct timeval end;
	long msec = 0;
	gettimeofday(&start,NULL);

    do
    {
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0) break;

        memset(&ticket_data, 0, sizeof(ticket_data));
        memset(&exit, 0, sizeof(exit));
        memset(&locked, 0, sizeof(locked));
		memset(&eticketdealinfo, 0, sizeof(eticketdealinfo));
		memset(&eticketauthinfo, 0, sizeof(eticketauthinfo));

        ret.wErrCode = api_match_device(device_type_file);
        if (ret.wErrCode != 0)
        {
        	ret.wErrCode = api_match_device(dvcExitAGM);
        	if (ret.wErrCode != 0) break;
        }

		ret.wErrCode = TicketBase::search_card(&p_ticket, &forbid_ant, &forbid_type, current_id, operExit);
		if (ret.wErrCode != 0)
		{
			if(Api::antFlag == 0)
				memset(locked_exit_id, 0, sizeof(locked_entry_id));
			break;
		}

		g_Record.log_buffer("Gate_EntryFlow testTmpData = ", Api::testTmpData, 14,level_normal);

		p_ticket->get_ticket_infos(ticket_data);

		if(ticket_data.qr_flag == 0x00)
		{
            if (memcmp(current_id, locked_exit_id, sizeof(current_id)) == 0)
            {
                ret.wErrCode = ERR_CARD_NONE;
                break;
            }

            memcpy(locked_exit_id, current_id, sizeof(locked_exit_id));

		}
		ret = p_ticket->read_card(operExit, ticket_prm);
        if (ret.wErrCode != 0)	break;

        p_ticket->get_ticket_infos(ticket_data);
		p_ticket->get_degrade_mode(md_effect, CmdSort::m_time_now);
		if (md_effect.emergency)
		{
			len_exit = 0;
			break;
		}

		//检查是否重复过闸 如果是电子票卡的话，需要检查重复使用二维码
		if(ticket_data.qr_flag != 0x00)
		{
			if(g_LoopList.QueryList((char *)ticket_data.timeAndcenterCode,operExit))
			{
                g_Record.log_out(0, level_error,"this is the same qr code");
				ret.wErrCode = ERR_CARD_NONE;
				memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
				memset(Api::clear_qr_readbuf,0x00,sizeof(Api::clear_qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
				break;
			}else{
				g_Record.log_out(0, level_error,"this is the same qr code else");
			}
		}

        // 分析票卡开始
        ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
        if (ret.wErrCode)	break;
        // 设备支持
        ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
        if (ret.wErrCode != 0)	break;
		// 检查专用通道
		ret.wErrCode = g_Parameter.passageway_allow_pass(ticket_data.logical_type, door_type);
		if (ret.wErrCode)	break;
        ret = p_ticket->analyse_common(1, &md_effect, &ticket_prm, &locked, len_lock, NULL, &exit.cExitMode);
		if (ret.wErrCode != 0)	break;
        ret = p_ticket->analyse_work_zone(&md_effect, &ticket_prm, NULL, &exit, NULL);
		// 交易确认
		if (ret.wErrCode != 0)
		{
			if(ticket_data.qr_flag == 0x00)
			{
				if (!p_ticket->confirm_current_card()) break;

				if (!p_ticket->check_confirm_his(10)) break;
			}
			if (!p_ticket->last_trade_need_continue(TicketBase::MS_Exit, ERR_LAST_EXIT_NEAR, ret.wErrCode)) break;
			ret = p_ticket->continue_last_trade(&exit);
		}
		else
		{
			if(ticket_data.qr_flag == 0x00){
				ret = p_ticket->exit_gate(&exit, &eticketdealinfo,&md_effect,cOnlineRes);
			}
			else{
				ret = p_ticket->check_online(operExit,&eticketauthinfo);
			}
		}

        if (ret.wErrCode)		break;

		if(ticket_data.qr_flag == 0x00)
		{
			len_exit = sizeof(exit);
			p_ticket->clear_confirm(exit.cLogicalID, true, false);
		}
		else
		{
			len_eticket = sizeof(eticketauthinfo);
		}

    } while (0);

	if(ret.wErrCode == ERR_NEED_CONTINUE_LAST)//交易确认
	{
		p_ticket->get_ticket_infos(ticket_data);
		ret = p_ticket->continue_last_trade(&eticketdealinfo);
		//g_LoopList.UpdateList((char *)ticket_data.timeAndcenterCode,operEntry);
		len_eticket = sizeof(eticketdealinfo);
		p_ticket->clear_confirm_e(eticketdealinfo.cCenterCode, true, false);
		ret.bRfu[0] = 0x11;
		ret.bRfu[1] = 0x00;
	}

	//FIXME: 不管成功与否，都添加如队列 如果是重复的码就直接返回无卡
	if(ticket_data.qr_flag != 0x00&&ret.wErrCode != ERR_SUCCEED ){
		g_LoopList.UpdateList((char *)ticket_data.timeAndcenterCode,operEntry);
	}
	g_Record.add_record(locked.cTradeType, &locked, len_lock);

	if(ticket_data.qr_flag == 0x00){
		g_Record.add_record(exit.cTradeType, &exit, len_exit);
	}
	else{
		g_Record.add_record("60", &eticketauthinfo, len_eticket);
	}


	if (ret.wErrCode != ERR_CARD_NONE)
	{
		memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
		memset(Api::clear_qr_readbuf,0x00,sizeof(Api::clear_qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
	}

	if (ret.wErrCode)
	{
		if (ret.wErrCode == ERR_CARD_READ/* || ret.bNoticeCode == NTC_MUST_CONFIRM*/)
		{
			ret.wErrCode = ERR_CARD_NONE;
			ret.bNoticeCode = 0;
			memset(locked_exit_id, 0, sizeof(locked_exit_id));
		}
	}

	if(ticket_data.qr_flag == 0x00){
		len_data = organize_data_region(data_to_send, &ret, len_exit, &exit, len_lock, &locked);
	}
	else{
		len_data = organize_data_region(data_to_send, &ret, len_eticket, &eticketauthinfo, len_lock, &locked);
	}

	gettimeofday(&end,NULL);
	time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
	msec = time_use / 1000;

	if(ret.wErrCode != ERR_CARD_NONE){
		g_Record.log_out(ret.wErrCode, level_disaster,"Gate_ExitFlow(lid=%s)=(%04X,%02X) use time = [%ld]", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode,msec);
	}

}

void Api::Gate_OpenFlow(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret					= {0};
    TicketBase * p_ticket		= NULL;
    TICKET_PARAM ticket_prm		= {0};
    TICKET_DATA ticket_data;

    MODE_EFFECT_FLAGS md_effect	= {0};
	ENTRYGATE entry;
    PURSETRADE exit;
	ETICKETDEALINFO eticketdealinfo;
    TICKETLOCK locked;
    uint16_t len_exit			= 0;
    uint16_t len_lock			= 0;
	uint16_t len_eticket		= 0;
	uint8_t forbid_ant			= 0;
	ETPMDM forbid_type			= mediUltraLight;
	uint8_t current_id[7]		= {0};

    long time_use=0;
	struct timeval start;
	struct timeval end;
	long msec = 0;
	char cCenterCodeIn[32 + 1] = {0};	//传入的中心票号
	char cOnlineRes = '1';			    //联机验票结果
	char dealType[2 + 1] = {0};
	ETYTKOPER oper_type;

	gettimeofday(&start,NULL);

	g_Record.log_out(0, level_normal,"Gate_OpenFlow");

    do
    {
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		memcpy(cCenterCodeIn, param_stack, 32);
		cOnlineRes = param_stack[32];
		g_Record.log_out(0, level_error,"cOnlineRes=[%c]",cOnlineRes);


        memset(&ticket_data, 0, sizeof(ticket_data));
		memset(&entry, 0, sizeof(entry));
        memset(&exit, 0, sizeof(exit));
        memset(&locked, 0, sizeof(locked));
		memset(&eticketdealinfo, 0, sizeof(eticketdealinfo));

		ret.wErrCode = TicketBase::search_card_div(cCenterCodeIn,dealType);
		if (ret.wErrCode != 0)
		{
			break;
		}

        ret.wErrCode = api_match_device(device_type_file);
        if (ret.wErrCode != 0)
        {
			if(memcmp(dealType,"07",2) == 0){ //如果不是双向闸机，需要根据类型来判断进闸还是出闸
				ret.wErrCode = api_match_device(dvcEntryAGM);
			}else{
				ret.wErrCode = api_match_device(dvcExitAGM);
			}
        	if (ret.wErrCode != 0)
        		break;
        }

		if(memcmp(dealType,"07",2) == 0){
			oper_type = operEntry;
		}else if(memcmp(dealType,"08",2) == 0){
			oper_type = operExit;
		}

		ret.wErrCode = TicketBase::search_card(&p_ticket, &forbid_ant, &forbid_type, current_id, oper_type);
		if (ret.wErrCode != 0)
		{
			if(Api::antFlag == 0){
				memset(locked_exit_id, 0, sizeof(locked_entry_id));
			}
			break;
		}

		g_Record.log_buffer("Gate_EntryFlow testTmpData = ", Api::testTmpData, 14,level_normal);

		p_ticket->get_ticket_infos(ticket_data);

		if(ticket_data.qr_flag == 0x00)
		{
			//if(Api::antFlag == 0)
			{
				if (memcmp(current_id, locked_exit_id, sizeof(current_id)) == 0)
				{
					ret.wErrCode = ERR_CARD_NONE;
					break;
				}

				memcpy(locked_exit_id, current_id, sizeof(locked_exit_id));
			}

		}
		ret = p_ticket->read_card(oper_type, ticket_prm);
        if (ret.wErrCode != 0)	break;

        p_ticket->get_ticket_infos(ticket_data);
		p_ticket->get_degrade_mode(md_effect, CmdSort::m_time_now);
		if (md_effect.emergency)
		{
			len_exit = 0;
			break;
		}

		//检查是否重复过闸 如果是电子票卡的话，需要检查重复使用二维吗
		if(ticket_data.qr_flag == 0x01)
		{
			if(g_LoopList.QueryList((char *)ticket_data.timeAndcenterCode,oper_type))
			{
                g_Record.log_out(0, level_error,"this is the same qr code");
				ret.wErrCode = ERR_CARD_NONE;
				memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
				memset(Api::clear_qr_readbuf,0x00,sizeof(Api::clear_qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
				break;
			}
		}

        // 分析票卡开始
        ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
        if (ret.wErrCode)	break;
        // 设备支持
        ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
        if (ret.wErrCode != 0)	break;
		// 检查专用通道
		ret.wErrCode = g_Parameter.passageway_allow_pass(ticket_data.logical_type, door_type);
		if (ret.wErrCode)	break;
        ret = p_ticket->analyse_common(1, &md_effect, &ticket_prm, &locked, len_lock, NULL, &exit.cExitMode);
		if (ret.wErrCode != 0)	break;
		if(oper_type == operEntry){
			ret = p_ticket->analyse_free_zone(&md_effect, &ticket_prm, NULL,NULL);
		}
		else{
			ret = p_ticket->analyse_work_zone(&md_effect, &ticket_prm, NULL, &exit,NULL);
		}

		// 交易确认
		if (ret.wErrCode != 0)
		{
			if(ticket_data.qr_flag == 0x00)
			{
				if (!p_ticket->confirm_current_card()) break;

				if (!p_ticket->check_confirm_his(10)) break;
			}

			if (!p_ticket->last_trade_need_continue(TicketBase::MS_Exit, ERR_LAST_EXIT_NEAR, ret.wErrCode))  break;

			ret = p_ticket->continue_last_trade(&exit);
		}
		else {
            if (oper_type == operEntry) {
                ret = p_ticket->entry_gate(&entry, &eticketdealinfo, cOnlineRes);
            } else {
                ret = p_ticket->exit_gate(&exit, &eticketdealinfo, &md_effect, cOnlineRes);
            }
        }

        if (ret.wErrCode)		break;

		if(ticket_data.qr_flag == 0x00)
		{
			len_exit = sizeof(exit);
			p_ticket->clear_confirm(exit.cLogicalID, true, false);
		}
		else
		{
			len_eticket = sizeof(eticketdealinfo);
			p_ticket->clear_confirm_e(eticketdealinfo.cCenterCode, true, false);
		}

    } while (0);

	if(ret.wErrCode == ERR_NEED_CONTINUE_LAST)//交易确认
	{
		p_ticket->get_ticket_infos(ticket_data);
		ret = p_ticket->continue_last_trade(&eticketdealinfo);
		//g_LoopList.UpdateList((char *)ticket_data.timeAndcenterCode,oper_type);
		len_eticket = sizeof(eticketdealinfo);
		p_ticket->clear_confirm_e(eticketdealinfo.cCenterCode, true, false);
		ret.bRfu[0] = 0x11;
		ret.bRfu[1] = 0x00;
	}

	//最后关键openFlow接口 不管是否成功都需要添加队列去判断重复二维码
	if(ticket_data.qr_flag != 0x00){
		g_LoopList.UpdateList((char *)ticket_data.timeAndcenterCode,oper_type);
		g_Record.log_buffer("timeAndcenterCode=", ticket_data.timeAndcenterCode,27);
	}

	g_Record.add_record(locked.cTradeType, &locked, len_lock);

	if(ticket_data.qr_flag == 0x00){
		g_Record.add_record(exit.cTradeType, &exit, len_exit);
	}
	else{
		g_Record.add_record("60", &eticketdealinfo, len_eticket);
	}

	if (ret.wErrCode != ERR_CARD_NONE)
	{
		memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
		memset(Api::clear_qr_readbuf,0x00,sizeof(Api::clear_qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
	}

	if (ret.wErrCode)
	{
		if (ret.wErrCode == ERR_CARD_READ/* || ret.bNoticeCode == NTC_MUST_CONFIRM*/)
		{
			ret.wErrCode = ERR_CARD_NONE;
			ret.bNoticeCode = 0;
			memset(locked_exit_id, 0, sizeof(locked_exit_id));
		}
	}

	if(ticket_data.qr_flag == 0x00){
		len_data = organize_data_region(data_to_send, &ret, len_exit, &exit, len_lock, &locked);
	}
	else{
		len_data = organize_data_region(data_to_send, &ret, len_eticket, &eticketdealinfo, len_lock, &locked);
	}

	gettimeofday(&end,NULL);
	time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
	msec = time_use / 1000;

	if(ret.wErrCode != ERR_CARD_NONE){
		g_Record.log_out(ret.wErrCode, level_disaster,"Gate_OpenFlow(lid=%s)=(%04X,%02X) use time = [%ld]", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode,msec);
	}

}
