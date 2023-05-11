#include "Api.h"
#include "Publics.h"
#include "ParamMgr.h"
#include "TicketUl.h"
#include "Declares.h"
#include "Errors.h"
#include "Records.h"

static ETPDVC device_type_file	= dvcBidirectAGM;


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
    TICKETLOCK locked;
    uint16_t len_entry			= 0;
    uint16_t len_lock			= 0;
	uint8_t current_id[7]		= {0};

    do
    {
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		memset(&ticket_data, 0, sizeof(ticket_data));
        memset(&entry, 0, sizeof(entry));
        memset(&locked, 0, sizeof(locked));

        ret.wErrCode = api_match_device(device_type_file);
        if (ret.wErrCode != 0)
        {
        	ret.wErrCode = api_match_device(dvcEntryAGM);
        	if (ret.wErrCode != 0)
        		break;
        }

        ret.wErrCode = TicketBase::search_card(&p_ticket, NULL, NULL, current_id, operEntry);
        if (ret.wErrCode != 0)
		{
			//g_Record.log_out(ret.wErrCode, level_normal, "Gate_EntryFlow-search_card=(%04X)", ret.wErrCode);
			memset(locked_entry_id, 0, sizeof(locked_entry_id));
			break;
		}

		if (memcmp(current_id, locked_entry_id, sizeof(current_id)) == 0)
		{
			//g_Record.log_out(ret.wErrCode, level_normal, "Gate_EntryFlow-current_id");
			ret.wErrCode = ERR_CARD_NONE;
			break;
		}

		memcpy(locked_entry_id, current_id, sizeof(locked_entry_id));

        ret = p_ticket->read_card(operEntry, ticket_prm);
        if (ret.wErrCode != 0)	break;

        p_ticket->get_ticket_infos(ticket_data);

		//g_Record.log_out(ret.wErrCode, level_normal, "Gate_EntryFlow(lid=%s)=(%04X,%02X)", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode);

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

        ret = p_ticket->analyse_common(2, &md_effect, &ticket_prm, &locked, len_lock, NULL, &entry.cEntryMode);
        if (ret.wErrCode != 0)	break;

        ret = p_ticket->analyse_free_zone(&md_effect, &ticket_prm, NULL);

		// 交易确认
		if (ret.wErrCode != 0)
		{
			if (!p_ticket->confirm_current_card())
				break;

			if (!p_ticket->check_confirm_his(10))
				break;

			if (!p_ticket->last_trade_need_continue(TicketBase::MS_Entry, ERR_ENTRY_EVER, ret.wErrCode))
				break;

			ret = p_ticket->continue_last_trade(&entry);
		}
		else
		{
			ret = p_ticket->entry_gate(&entry);
		}

		if (ret.wErrCode)	break;

        len_entry = sizeof(entry);

		p_ticket->clear_confirm(entry.cLogicalID, true, false);

    }
    while (0);

	g_Record.add_record(locked.cTradeType, &locked, len_lock);
	g_Record.add_record(entry.cTradeType, &entry, len_entry);

	g_Record.log_out(ret.wErrCode, level_normal, "Gate_EntryFlow(lid=%s)=(%04X,%02X)", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode);

	if (ret.wErrCode)
	{
		if (ret.wErrCode == ERR_CARD_READ/* || ret.bNoticeCode == NTC_MUST_CONFIRM*/)
		{
			ret.wErrCode = ERR_CARD_NONE;
			ret.bNoticeCode = 0;
			memset(locked_entry_id, 0, sizeof(locked_entry_id));
		}
	}

	len_data = organize_data_region(data_to_send, &ret, len_entry, &entry, len_lock, &locked);
}

void Api::Gate_ExitFlow(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret					= {0};
    TicketBase * p_ticket		= NULL;
    TICKET_PARAM ticket_prm		= {0};
    TICKET_DATA ticket_data;

    MODE_EFFECT_FLAGS md_effect	= {0};
    PURSETRADE exit;
    TICKETLOCK locked;
    uint16_t len_exit			= 0;
    uint16_t len_lock			= 0;
	uint8_t forbid_ant			= 0;
	ETPMDM forbid_type			= mediUltraLight;
	uint8_t current_id[7]		= {0};

    do
    {
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

        memset(&ticket_data, 0, sizeof(ticket_data));
        memset(&exit, 0, sizeof(exit));
        memset(&locked, 0, sizeof(locked));

        ret.wErrCode = api_match_device(device_type_file);
        if (ret.wErrCode != 0)
        {
        	ret.wErrCode = api_match_device(dvcExitAGM);
        	if (ret.wErrCode != 0)
        		break;
        }

		ret.wErrCode = TicketBase::search_card(&p_ticket, &forbid_ant, &forbid_type, current_id, operExit);
		if (ret.wErrCode != 0)
		{
			memset(locked_exit_id, 0, sizeof(locked_entry_id));
			break;
		}

		if (memcmp(current_id, locked_exit_id, sizeof(current_id)) == 0)
		{
			ret.wErrCode = ERR_CARD_NONE;
			break;
		}

		memcpy(locked_exit_id, current_id, sizeof(locked_exit_id));


		ret = p_ticket->read_card(operExit, ticket_prm);
        if (ret.wErrCode != 0)	break;

        p_ticket->get_ticket_infos(ticket_data);

		p_ticket->get_degrade_mode(md_effect, CmdSort::m_time_now);
		if (md_effect.emergency)
		{
			len_exit = 0;
			break;
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

        ret = p_ticket->analyse_work_zone(&md_effect, &ticket_prm, NULL, &exit);

		// 交易确认
		if (ret.wErrCode != 0)
		{
			if (!p_ticket->confirm_current_card())
				break;

			if (!p_ticket->check_confirm_his(10))
				break;

			if (!p_ticket->last_trade_need_continue(TicketBase::MS_Exit, ERR_LAST_EXIT_NEAR, ret.wErrCode))
				break;

			ret = p_ticket->continue_last_trade(&exit);
		}
		else
		{
			ret = p_ticket->exit_gate(&exit, &md_effect);
		}

        if (ret.wErrCode)		break;

		len_exit = sizeof(exit);

		p_ticket->clear_confirm(exit.cLogicalID, true, false);

    } while (0);

	g_Record.add_record(locked.cTradeType, &locked, len_lock);
	g_Record.add_record(exit.cTradeType, &exit, len_exit);

	g_Record.log_out(ret.wErrCode, level_normal, "Gate_ExitFlow(lid=%s)=(%04X,%02X)", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode);

	if (ret.wErrCode)
	{
		if (ret.wErrCode == ERR_CARD_READ/* || ret.bNoticeCode == NTC_MUST_CONFIRM*/)
		{
			ret.wErrCode = ERR_CARD_NONE;
			ret.bNoticeCode = 0;
			memset(locked_exit_id, 0, sizeof(locked_exit_id));
		}
	}

    len_data = organize_data_region(data_to_send, &ret, len_exit, &exit, len_lock, &locked);
}
