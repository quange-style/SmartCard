#include "Api.h"
#include "Publics.h"
#include "ParamMgr.h"
#include "TicketUl.h"
#include "Declares.h"
#include "Records.h"

static ETPDVC device_type_file	= dvcTCM;


//
// TCM接口
//
void Api::Tcm_GetTicketInfo(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret					= {0};
	TicketBase * p_ticket		= NULL;
	TICKET_PARAM ticket_prm;
	TICKET_DATA ticket_data;

	TICKETINFO tkInf;
	uint16_t len_inf			= 0;

	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		memset(&ticket_data, 0, sizeof(ticket_data));
	    memset(&ticket_prm, 0, sizeof(ticket_prm));
	    memset(&tkInf, 0, sizeof(tkInf));

		ret.wErrCode = api_match_device(device_type_file);
		if (ret.wErrCode != 0)	break;

		ret.wErrCode = TicketBase::search_card(&p_ticket);
		if (ret.wErrCode != 0)	break;

		ret = p_ticket->read_card(operTkInf, ticket_prm);
		if (ret.wErrCode != 0)	break;

		p_ticket->get_ticket_infos(ticket_data);
		p_ticket->transfer_data_for_out(operTkInf, &tkInf);
		tkInf.bStatus = p_ticket->unified_status();

		ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
		if (ret.wErrCode)	break;

		// 设备支持
		ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
		if (ret.wErrCode != 0)	break;

		p_ticket->format_history(tkInf.bUsefulCount, tkInf.bhs, sizeof(tkInf.bhs)/sizeof(tkInf.bhs[0]));

		len_inf = sizeof(tkInf);

	}
	while (0);

	len_data = organize_data_region(data_to_send, &ret, len_inf, &tkInf, 0, NULL);
}
