#include "Api.h"
#include "Publics.h"
#include "ParamMgr.h"
#include "TicketUl.h"
#include "Declares.h"
#include "../link/linker.h"
#include "Errors.h"
#include "Records.h"

static ETPDVC device_type_file	= dvcTVM;

extern TicketBase * g_p_ticket;
//
// TVM接口
//
void Api::Tvm_Ticket_Sale(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	TicketBase * p_ticket		= NULL;
	TICKET_PARAM ticket_prm		= {0};
	TICKET_DATA ticket_data;

	SJTSALE sjt_sale;
	uint16_t len_sjt_sale		= 0;
	OTHERSALE svt_sale;
	uint16_t len_svt_sale		= 0;

	uint8_t amount_type			= 0;
	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		memset(&ticket_data, 0, sizeof(ticket_data));
        memset(&sjt_sale, 0, sizeof(sjt_sale));
        memset(&svt_sale, 0, sizeof(svt_sale));

		memcpy(&sjt_sale.nChargeValue, param_stack, 2);
		sjt_sale.bPaymentMeans = param_stack[2];
		amount_type = param_stack[3];

		ret.wErrCode = api_match_device(device_type_file);
		if (ret.wErrCode != 0)	break;

		ret.wErrCode = TicketBase::search_card(&p_ticket);
		if (ret.wErrCode != 0)	break;

		ret = p_ticket->read_card(operSale, ticket_prm);
		if (ret.wErrCode != 0)	break;

		p_ticket->get_ticket_infos(ticket_data);

		ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
		if (ret.wErrCode)	break;

		// 设备支持
		ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
		if (ret.wErrCode != 0)	break;

		ret.wErrCode = ParamMgr::device_support_sale(device_type_file, ticket_prm.sell_device);
		if (ret.wErrCode != 0)	break;

		// 成本押金
		if(amount_type == 0)
		{
			svt_sale.nAmount = (short)ticket_prm.deposite;
		}
		else
		{
			svt_sale.nAmount = 0;
		}

		ret = p_ticket->sale(&sjt_sale, len_sjt_sale, &svt_sale, len_svt_sale, ticket_prm);

	}
	while (0);

	m_trade_val = 0;

	g_Record.add_record(sjt_sale.cTradeType, &sjt_sale, len_sjt_sale);
	g_Record.add_record(svt_sale.cTradeType, &svt_sale, len_svt_sale);

	len_data = organize_data_region(data_to_send, &ret, len_sjt_sale, &sjt_sale, len_svt_sale, &svt_sale);

	g_Record.log_out(ret.wErrCode, level_disaster, "Tvm_Ticket_Sale(lid=%s,value=%d,pay_type=%d,amount_type=%d)=(%04X,%02X)",
		ticket_data.logical_id, sjt_sale.nChargeValue, sjt_sale.bPaymentMeans, amount_type, ret.wErrCode, ret.bNoticeCode);
}


void Api::Tvm_GetTopPrice(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	long fare_trade = 0;
	uint8_t ticket_type[2];
	uint16_t entry_station;

	ret.wErrCode = g_Parameter.device_initialized();
	if (ret.wErrCode == 0)
	{
		//TVM售票无优惠，当成单程票即可
		//ticket_type[0] = 0x01;
		//ticket_type[1] = 0x00;
		ret.wErrCode = g_Parameter.query_highest_fare(CmdSort::m_time_now,&fare_trade);
	}

	len_data = organize_data_region(data_to_send, &ret, sizeof(fare_trade), &fare_trade);
	g_Record.log_out(ret.wErrCode, level_error, "Tvm_GetTopPrice(fare_trade=%02d)=(%04X,%02X)",fare_trade, ret.wErrCode, ret.bNoticeCode);
}

void Api::Tvm_SjtClear(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret					= {0};
	TicketBase * p_ticket		= NULL;
	TICKET_PARAM ticket_prm		= {0};
	TICKET_DATA ticket_data;

	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		memset(&ticket_data, 0, sizeof(ticket_data));

		ret.wErrCode = api_match_device(device_type_file);
		if (ret.wErrCode != 0)
		{
			ret.wErrCode = api_match_device(dvcExitAGM);
			if (ret.wErrCode != 0)
				break;
		}

		ret.wErrCode = TicketBase::search_card(&p_ticket);
		if (ret.wErrCode != 0)	break;

		ret = p_ticket->read_card(operExit, ticket_prm);
		if (ret.wErrCode != 0)	break;

		p_ticket->get_ticket_infos(ticket_data);

		// 分析票卡开始
		ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
		if (ret.wErrCode)	break;

		// 设备支持
		ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
		if (ret.wErrCode != 0)	break;

		ret = p_ticket->sjt_decrease();
	}
	while (0);

	len_data = organize_data_region(data_to_send, &ret, 0, NULL, 0, NULL);

	g_Record.log_out(ret.wErrCode, level_invalid, "Tvm_SjtClear(lid=%s)=(%04X,%02X)", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode);
}

void Api::Tvm_TicketAnalyze(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret					= {0};
	TicketBase * p_ticket		= NULL;
	TICKET_PARAM ticket_prm		= {0};
	TICKET_DATA ticket_data;

	MODE_EFFECT_FLAGS md_effect	= {0};
	BOMANALYZE analyse;
	TICKETLOCK locked;
	uint16_t len_analyse		= 0;
	uint16_t len_lock			= 0;

	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		memset(&ticket_data, 0, sizeof(ticket_data));
	    memset(&analyse, 0, sizeof(analyse));
	    memset(&locked, 0, sizeof(locked));

		ret.wErrCode = api_match_device(device_type_file);
		if (ret.wErrCode != 0)	break;

		ret.wErrCode = TicketBase::search_card(&p_ticket);
		if (ret.wErrCode != 0)	break;

		ret = p_ticket->read_card(operAnalyse, ticket_prm);
		if (ret.wErrCode != 0)	break;

		p_ticket->get_ticket_infos(ticket_data);
		p_ticket->transfer_data_for_out(operAnalyse, &analyse);

		len_analyse = sizeof(analyse);

		analyse.bStatus = p_ticket->unified_status();

		// 分析票卡开始
		ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
		if (ret.wErrCode)	break;

		// 设备支持
		ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
		if (ret.wErrCode != 0)	break;

		ret = p_ticket->analyse_common(2, &md_effect, &ticket_prm, &locked, len_lock, &analyse, NULL);
		if (ret.wErrCode != 0)	break;

		ret = p_ticket->analyse_free_zone(&md_effect, &ticket_prm, &analyse);

	}
	while (0);

	g_Record.add_record(locked.cTradeType, &locked, len_lock);

	len_data = organize_data_region(data_to_send, &ret, len_analyse, &analyse, len_lock, &locked);

	g_Record.log_out(ret.wErrCode, level_invalid, "Tvm_TicketAnalyze(lid=%s)=(%04X,%02X)", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode);
}

void Api::Tvm_PurseDecrease(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret					= {0};
	TicketBase * p_ticket		= NULL;
	TICKET_PARAM ticket_prm		= {0};
	TICKET_DATA ticket_data;

	PURSETRADE decr;
	uint16_t len_decr			= 0;

	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		memset(&ticket_data, 0, sizeof(ticket_data));
	    memset(&decr, 0, sizeof(decr));

		memcpy(&decr.lTradeAmount, param_stack, 4);

		if (decr.lTradeAmount <= 0)
		{
			ret.wErrCode = ERR_INPUT_PARAM;
			break;
		}

		ret.wErrCode = api_match_device(device_type_file);
		if (ret.wErrCode != 0)
			break;

		ret.wErrCode = TicketBase::search_card(&p_ticket);
		if (ret.wErrCode != 0)	break;

		ret = p_ticket->read_card(operDecr, ticket_prm);
		if (ret.wErrCode != 0)	break;

		p_ticket->get_ticket_infos(ticket_data);

		ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
		if (ret.wErrCode)	break;

		memcpy(decr.cPaymentType, "19", sizeof(decr.cPaymentType));

		// 设备支持
		ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
		if (ret.wErrCode != 0)	break;

		ret = p_ticket->svt_decrease(&decr, PAY_BUY_TOKEN);
		if (ret.wErrCode)	break;

		len_decr = sizeof(decr);

	} while (0);

	g_Record.add_record(decr.cTradeType, &decr, len_decr);

	len_data = organize_data_region(data_to_send, &ret, len_decr, &decr);

	g_Record.log_out(ret.wErrCode, level_invalid, "Tvm_PurseDecrease(lid=%s,val=%d)=(%04X,%02X)", ticket_data.logical_id, decr.lTradeAmount, ret.wErrCode, ret.bNoticeCode);
}

void Api::Tvm_SvtIncrease1(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO		ret				= {0};
	TICKET_PARAM ticket_prm;
	TICKET_DATA ticket_data;

	CHARGE_INIT	chin;
	uint16_t len_chin			= 0;
	uint8_t init_out[128]		= {0};
	char sz_temp[32]			= {0};

	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		memset(&ticket_data, 0, sizeof(ticket_data));
		memset(&ticket_prm, 0, sizeof(ticket_prm));
		memset(&chin, 0, sizeof(chin));

		chin.cWalletType = param_stack[0] + 0x30;
		memcpy(&chin.lAmount, param_stack + 1, 4);
		m_trade_val = chin.lAmount;
		memcpy(chin.cNetPoint, param_stack + 5, sizeof(chin.cNetPoint));
		memcpy(chin.dtMsgTime, CmdSort::m_time_now, 7);

		if (chin.lAmount <= 0)
		{
			ret.wErrCode = ERR_INPUT_PARAM;
			break;
		}

		ret.wErrCode = api_match_device(device_type_file);
		if (ret.wErrCode != 0)	break;

		ret.wErrCode = TicketBase::search_card(&g_p_ticket);
		if (ret.wErrCode != 0)	break;

		ret = g_p_ticket->read_card(operIncr, ticket_prm);
		if (ret.wErrCode != 0)	break;

		g_p_ticket->get_ticket_infos(ticket_data);

		ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
		if (ret.wErrCode)	break;

		// 设备支持
		ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
		if (ret.wErrCode != 0)	break;

		ret.wErrCode = ParamMgr::device_support_charge(device_type_file, ticket_prm.chargable_device);
		if (ret.wErrCode != 0)	break;

		if (ticket_data.wallet_value + chin.lAmount > ticket_data.wallet_value_max || chin.lAmount > ticket_prm.charge_max)
		{
			ret.wErrCode = ERR_OVER_WALLET;
			break;
		}

		// 部分字段赋值
		memcpy(chin.cMsgType, "51", 2);
		get_current_device_info(chin.cStation, chin.cDevType, chin.cDevId);
		memcpy(chin.cSamId, m_sam[0], 16);

		memcpy(chin.cIssueCodeMain, ticket_data.issue_code, sizeof(chin.cIssueCodeMain));
		memset(chin.cIssueCodeSub, '0', sizeof(chin.cIssueCodeSub));

		memcpy(chin.bTicketType, ticket_data.logical_type, 2);
		memcpy(chin.cLogicalId, ticket_data.logical_id, 20);
		memcpy(chin.cPhysicalId, ticket_data.physical_id, 20);
		chin.cTestFlag = ticket_data.key_flag + '0';
		memcpy(chin.cBusinessType, "14", 2);
		chin.lBalcance = ticket_data.wallet_value;
		memcpy(chin.cLastPosID, chin.cSamId, 16);
		memcpy(chin.dtTimeLast, chin.dtMsgTime, 7);
		memcpy(chin.cOperatorId, operator_id, 6);

		ret.wErrCode = svt_recharge(0x02, CmdSort::m_time_now, SAM_SOCK_1, 0, chin.lAmount, init_out);
		if (ret.wErrCode != 0)
		{
			ret.wErrCode = ERR_CARD_READ;
			break;
		}
		// 票卡返回按moto的字节对其标准
		chin.lChargeCount = ticket_data.charge_counter;
		chin.lTradeCount = ticket_data.total_trade_counter;

		// 随机数和MAC
		sprintf(sz_temp, "%02X%02X%02X%02X%02X%02X%02X%02X",
			init_out[8], init_out[9], init_out[10], init_out[11], init_out[12], init_out[13], init_out[14], init_out[15]);
		memcpy(chin.cFreedom, sz_temp, 8);		// 伪随机数
		memcpy(chin.cMac, sz_temp + 8, 8);		// MAC

		chin.lTradeCount = (init_out[16] << 8) + init_out[17];	// 脱机交易流水
		chin.lPosSeq = (init_out[18] << 24) + (init_out[19] << 16) + (init_out[20] << 8) + init_out[21];	// 终端交易序号

		m_sam_seq_charge = chin.lPosSeq;

		len_chin = sizeof(CHARGE_INIT);
	}
	while (0);

	len_data = organize_data_region(data_to_send, &ret, len_chin, &chin);

	g_Record.log_out(ret.wErrCode, level_error, "Tvm_SvtIncrease1(lid=%s,value=%d,wallet_type=%c)=(%04X,%02X)",
		ticket_data.logical_id, chin.lAmount, chin.cWalletType, ret.wErrCode, ret.bNoticeCode);
}

void Api::Tvm_SvtIncrease2(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO		ret				= {0};
	TICKET_PARAM ticket_prm		= {0};
	TICKET_DATA ticket_data;

	PURSETRADE	pstd;
	uint16_t len_pstd			= 0;
	uint8_t bTime[7]			= {0};
	uint8_t Mac2[4]				= {0};


	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		memset(&ticket_data, 0, sizeof(ticket_data));
		memset(&pstd, 0, sizeof(pstd));

		memcpy(bTime, param_stack + 2, 7);
		Publics::string_to_hexs((char *)(param_stack + 62), 8, Mac2, 4);

		ret.wErrCode = api_match_device(device_type_file);
		if (ret.wErrCode != 0)	break;

		g_p_ticket->get_ticket_infos(ticket_data);

		ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
		if (ret.wErrCode)	break;

		// 设备支持
		ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
		if (ret.wErrCode != 0)	break;

		ret.wErrCode = ParamMgr::device_support_charge(device_type_file, ticket_prm.chargable_device);
		if (ret.wErrCode != 0)	break;

		pstd.lTradeAmount = m_trade_val;
		memcpy(pstd.cPaymentType, "14", sizeof(pstd.cPaymentType));

		ret = g_p_ticket->svt_increase(&pstd, bTime, Mac2);
		if (ret.wErrCode != 0)	break;

		pstd.lSAMTrSeqNo = m_sam_seq_charge;

		len_pstd = sizeof(PURSETRADE);

	} while (0);

	g_Record.add_record(pstd.cTradeType, &pstd, len_pstd);

	len_data = organize_data_region(data_to_send, &ret, len_pstd, &pstd);

	g_Record.log_out(ret.wErrCode, level_error, "Tvm_SvtIncrease2(lid=%s)=(%04X,%02X)", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode);
}
