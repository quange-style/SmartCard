#include "Api.h"
#include "Publics.h"
#include "ParamMgr.h"
#include "TicketUl.h"
#include "TicketElecT.h"
#include "Declares.h"
#include "../link/linker.h"
#include "Errors.h"
#include "Records.h"
#include "../link/myprintf.h"
#include "../link/SerialFile.h"

static ETPDVC device_type_file	= dvcTVM;

extern TicketBase * g_p_ticket;

extern CLoopList	g_LoopList;
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

		//首先清掉二维码数据//
		memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码

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

	g_Record.log_out(ret.wErrCode, level_disaster,
		"Tvm_Ticket_Sale(lid=%s,value=%d,pay_type=%d,amount_type=%d)=(%04X,%02X)",
		ticket_data.logical_id, sjt_sale.nChargeValue, sjt_sale.bPaymentMeans, amount_type, ret.wErrCode, ret.bNoticeCode);
}


void Api::Tvm_GetTopPrice(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	long fare_trade = 0;
	//uint8_t ticket_type[2];
	//uint16_t entry_station;

	ret.wErrCode = g_Parameter.device_initialized();
	if (ret.wErrCode == 0)
	{
		//TVM售票无优惠，当成单程票即可
		//ticket_type[0] = 0x01;
		//ticket_type[1] = 0x00;
		ret.wErrCode = g_Parameter.query_highest_fare(CmdSort::m_time_now,&fare_trade);
	}

	len_data = organize_data_region(data_to_send, &ret, sizeof(fare_trade), &fare_trade);

	g_Record.log_out(ret.wErrCode, level_error,
		"Tvm_GetTopPrice(fare_trade=%d)=(%04X,%02X),",fare_trade, ret.wErrCode, ret.bNoticeCode);
}



void Api::Tvm_IGetTicket(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	long fare_trade = 0;
	TicketBase * p_ticket		= NULL;
	uint8_t current_id[7]		= {0};
	ETICKETDEALINFO eticketdealinfo;
	GETETICKETRESPINFO geteticketrespinfo;
	TICKETLOCK locked;
	uint16_t len_eticket		= 0;
	uint16_t len_lock			= 0;

	MODE_EFFECT_FLAGS md_effect	= {0};

	TICKET_PARAM ticket_prm		= {0};
	TICKET_DATA ticket_data;

	long time_use=0;
	struct timeval start;
	struct timeval end;
	long msec = 0;

	gettimeofday(&start,NULL);

	do
    {

		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		//首先清掉二维码数据//
		//memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码

		ret.wErrCode = TicketBase::search_card(&p_ticket, NULL, NULL, current_id, operGetETicket);
		dbg_formatvar("ret.wErrCode=%d",ret.wErrCode);
		if (ret.wErrCode != 0)
		{
			//memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//寻不到卡的时候，清掉QR缓存数据
			//memset(Api::last_qr_readbuf,0x00,sizeof(Api::last_qr_readbuf));
			//dbg_formatvar("wErrCode!=0");
			break;
		}

		ret = p_ticket->read_card(operGetETicket, ticket_prm);
		if (ret.wErrCode != 0)	break;

		p_ticket->get_ticket_infos(ticket_data);


		//检查是否重复过闸 如果是电子票卡的话，需要检查重复使用二维吗
		if(ticket_data.qr_flag == 0x01)
		{
			if(g_LoopList.QueryList((char *)ticket_data.qr_ticketdata.centerCode,operGetETicket))
			{
				dbg_formatvar("this is the same qr code");
				ret.wErrCode = ERR_CARD_NONE;
				memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
				memset(Api::clear_qr_readbuf,0x00,sizeof(Api::clear_qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
				//g_Record.log_out(0, level_error,"QueryList:clear the same qr");
				break;
			}
		}

		ret = p_ticket->analyse_free_zone(&md_effect, &ticket_prm, NULL, NULL);
		// 交易确认
		if (ret.wErrCode != 0)
		{

		}
		else
		{
			ret = p_ticket->getETicket(&geteticketrespinfo);
		}

		//ret = p_ticket->getETicket(&eticketdealinfo);
		//ret = p_ticket->getETicket(&geteticketrespinfo);
		if (ret.wErrCode)	break;

		g_LoopList.UpdateList((char *)ticket_data.qr_ticketdata.centerCode,operGetETicket);

		//len_eticket = sizeof(eticketdealinfo);
		len_eticket = sizeof(geteticketrespinfo);

	}while (0);

	if (ret.wErrCode)
	{
		if (ret.wErrCode == ERR_CARD_READ/* || ret.bNoticeCode == NTC_MUST_CONFIRM*/)
		{
			ret.wErrCode = ERR_CARD_NONE;
			ret.bNoticeCode = 0;
		}
	}

	if (ret.wErrCode != ERR_CARD_NONE)
	{
		memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
		memset(Api::clear_qr_readbuf,0x00,sizeof(Api::clear_qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
	}

	//memset(Api::last_qr_readbuf,0x00,sizeof(Api::last_qr_readbuf));

	//len_data = organize_data_region(data_to_send, &ret, len_eticket, &eticketdealinfo, len_lock, &locked);
	len_data = organize_data_region(data_to_send, &ret, len_eticket, &geteticketrespinfo, len_lock, &locked);

	gettimeofday(&end,NULL);

	time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
	msec = time_use / 1000;

	g_Record.log_out(ret.wErrCode, level_disaster,"Tvm_IGetTicket(lid = %s)=(%04X,%02X) use time = [%ld]",ticket_data.qr_ticketdata.cardNo,ret.wErrCode, ret.bNoticeCode,msec);
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

		//首先清掉二维码数据//
		memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码

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

	g_Record.log_out(ret.wErrCode, level_invalid,
		"Tvm_SjtClear(lid=%s)=(%04X,%02X)", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode);
}

void Api::Tvm_TicketAnalyze(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret					= {0};
	TicketBase * p_ticket		= NULL;
	TICKET_PARAM ticket_prm		= {0};
	TICKET_DATA ticket_data;

	MODE_EFFECT_FLAGS md_effect	= {0};
	BOMETICANALYZE eticanalyse;
	BOMANALYZE analyse;
	TICKETLOCK locked;
	uint16_t len_analyse		= 0;
	uint16_t len_lock			= 0;
	uint16_t len_eticanalyse	= 0;

	long time_use=0;
	struct timeval start;
	struct timeval end;
	long msec = 0;

	gettimeofday(&start,NULL);

	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		//首先清掉二维码数据//
		memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码

		memset(&ticket_data, 0, sizeof(ticket_data));
	    memset(&analyse, 0, sizeof(analyse));
	    memset(&locked, 0, sizeof(locked));
		memset(&eticanalyse, 0, sizeof(eticanalyse));

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

		ret = p_ticket->analyse_free_zone(&md_effect, &ticket_prm, &analyse, &eticanalyse);

	}
	while (0);

	g_Record.add_record(locked.cTradeType, &locked, len_lock);

	len_data = organize_data_region(data_to_send, &ret, len_analyse, &analyse, len_lock, &locked);

	gettimeofday(&end,NULL);

	time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
	msec = time_use / 1000;

	g_Record.log_out(ret.wErrCode, level_disaster,
		"Tvm_TicketAnalyze(lid=%s)=(%04X,%02X) use time = [%ld]", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode, msec);
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

		//首先清掉二维码数据//
		memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码

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

	g_Record.log_out(ret.wErrCode, level_invalid,
		"Tvm_PurseDecrease(lid=%s,val=%d)=(%04X,%02X)",
		ticket_data.logical_id, decr.lTradeAmount, ret.wErrCode, ret.bNoticeCode);
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

		//首先清掉二维码数据//
		memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码

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

	g_Record.log_out(ret.wErrCode, level_error,
		"Tvm_SvtIncrease1(lid=%s,value=%d,wallet_type=%c)=(%04X,%02X)",
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

		//首先清掉二维码数据//
		memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码

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

	g_Record.log_out(ret.wErrCode, level_error,
		"Tvm_SvtIncrease2(lid=%s)=(%04X,%02X)", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode);
}


void Api::Tvm_GetTicketInfo(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
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

		//首先清掉二维码数据//
		memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码

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



void Api::Tvm_GetETicket_Sale(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	TicketBase * p_ticket		= NULL;
	TICKET_PARAM ticket_prm		= {0};
	TICKET_DATA ticket_data;

	SJTSALE sjt_sale;
	uint16_t len_sjt_sale		= 0;

	OTHERSALE svt_sale;
	uint16_t len_svt_sale		= 0;

	ETICKETDEALINFO eticketdealinfo;
	uint16_t len_eticket_sale		= 0;

	uint8_t amount_type			= 0;
	char tempAmount[6] = {0};

	long time_use=0;
	struct timeval start;
	struct timeval end;
	long msec = 0;

	gettimeofday(&start,NULL);


	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;
		//首先清掉二维码数据//
		memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码

		memset(&ticket_data, 0, sizeof(ticket_data));
		memset(&sjt_sale, 0, sizeof(sjt_sale));
		memset(&eticketdealinfo, 0, sizeof(eticketdealinfo));

		memcpy(&sjt_sale.nChargeValue, param_stack, 2);
		sjt_sale.bPaymentMeans = param_stack[2];
		amount_type = param_stack[3];

		sprintf(tempAmount,"%06d",sjt_sale.nChargeValue);

		memcpy(&eticketdealinfo.cTicketLogicalNo, param_stack + 36, 20);//增加逻辑卡号
		memcpy(&eticketdealinfo.cCenterCode, param_stack + 4, 32);//增加中心票号
		sprintf(eticketdealinfo.cPayType, "%02x",param_stack[2]);
		//memcpy(&eticketdealinfo.cTxnAmount, tempAmount, 6);//交易金额
		//g_Record.log_out_debug(0, level_disaster,",cTxnAmount=%s",eticketdealinfo.cTxnAmount);


		g_Record.log_out_debug(0, level_disaster,"Tvm_GetETicket_Sale(cTicketLogicalNo=%s,cCenterCode=%s",eticketdealinfo.cTicketLogicalNo, eticketdealinfo.cCenterCode);
		//g_Record.log_buffer_debug("param_stack:",param_stack,56);
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

		ret = p_ticket->esale(&sjt_sale, len_sjt_sale, &eticketdealinfo, len_eticket_sale, ticket_prm);

	}
	while (0);

	m_trade_val = 0;

	g_Record.add_record(sjt_sale.cTradeType, &sjt_sale, len_sjt_sale);
	//g_Record.add_record(svt_sale.cTradeType, &svt_sale, len_svt_sale);

	len_data = organize_data_region(data_to_send, &ret, len_sjt_sale, &sjt_sale, len_eticket_sale, &eticketdealinfo);

	gettimeofday(&end,NULL);

	time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
	msec = time_use / 1000;

	g_Record.log_out(ret.wErrCode, level_disaster,
		"Tvm_Ticket_Sale(lid=%s,value=%d,pay_type=%d,amount_type=%d)=(%04X,%02X)  use time = [%ld]",
		ticket_data.logical_id, sjt_sale.nChargeValue, sjt_sale.bPaymentMeans, amount_type, ret.wErrCode, ret.bNoticeCode,msec);
}


void Api::Tvm_UploadGetTicketInfo(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	long fare_trade = 0;
	TicketBase * p_ticket		= NULL;
	uint8_t current_id[7]		= {0};
	char succMount[3] = {0};	// 成功张数
	char failMount[3] = {0};	// 失败张数
	uint16_t length = 0;
	uint8_t uploadTicket[128];			// 进站信息
	int sysFlag = 0;

	do
    {

		memcpy(succMount, param_stack, 2);
		memcpy(failMount, param_stack + 2, 2);

		dbg_dumpmemory("succMount:",succMount,2);
		dbg_dumpmemory("failMount:",failMount,2);

		//g_Record.log_out(0, level_disaster,"param_stack(%02x%02x%02x%02x)",param_stack[0],param_stack[1], param_stack[2],param_stack[3]);

		//g_Record.log_out(0, level_disaster,"succMount=(%02x%02x)  failMount=(%02x%02x)",succMount[0],succMount[1], failMount[0],failMount[1]);

		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		TicketBase::getDealDataUpload(operUnloadTicketInfo, length, uploadTicket, succMount, failMount);

		ret = TicketElecT::write_card_back(operUnloadTicketInfo, uploadTicket, length);


	}while (0);

	if (ret.wErrCode)
	{
		if (ret.wErrCode == ERR_CARD_READ/* || ret.bNoticeCode == NTC_MUST_CONFIRM*/)
		{
			ret.wErrCode = ERR_CARD_NONE;
			ret.bNoticeCode = 0;
		}
	}

	//len_data = organize_data_region(data_to_send, &ret, len_eticket, &eticketdealinfo, len_lock, &locked);
	len_data = organize_data_region(data_to_send, &ret);

	g_Record.log_out(ret.wErrCode, level_disaster,"Tvm_UploadGetTicketInfo=(%04X,%02X)",ret.wErrCode, ret.bNoticeCode);
}


void Api::Tvm_ThirdPayGetETicket(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	long fare_trade = 0;
	TicketBase * p_ticket		= NULL;
	uint8_t current_id[7]		= {0};

	MODE_EFFECT_FLAGS md_effect	= {0};

	TICKET_PARAM ticket_prm		= {0};
	TICKET_DATA ticket_data;

	do
    {
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		//首先清掉二维码数据//
		//memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码

		ret.wErrCode = TicketBase::search_card(&p_ticket, NULL, NULL, current_id, operThirdGetETicket);
		dbg_formatvar("ret.wErrCode=%d",ret.wErrCode);
		if (ret.wErrCode != 0)
		{
			//memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//寻不到卡的时候，清掉QR缓存数据
			//memset(Api::last_qr_readbuf,0x00,sizeof(Api::last_qr_readbuf));
			//dbg_formatvar("wErrCode!=0");
			break;
		}

		ret = p_ticket->read_card(operThirdGetETicket, ticket_prm);
		if (ret.wErrCode != 0)	break;

		p_ticket->get_ticket_infos(ticket_data);


		//检查是否重复上送授权码
		if(ticket_data.qr_flag == 0x01)
		{
			dbg_formatvar("third_qr_info:%s",ticket_data.third_qr_info);
			dbg_dumpmemory("third_qr_info:",ticket_data.third_qr_info,18);
			if(g_LoopList.QueryList((char *)ticket_data.third_qr_info,operThirdGetETicket))
			{
				dbg_formatvar("this is the same qr code");
				ret.wErrCode = ERR_CARD_NONE;
				memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
				//g_Record.log_out(0, level_error,"QueryList:clear the same qr");
				break;
			}
		}

		//ret = p_ticket->getETicket(&geteticketrespinfo);


		//ret = p_ticket->getETicket(&eticketdealinfo);
		//ret = p_ticket->getETicket(&geteticketrespinfo);
		//if (ret.wErrCode)	break;m_ticket_data.third_qr_info
		dbg_dumpmemory("third_qr_info:",ticket_data.third_qr_info,18);
		g_LoopList.UpdateList((char *)ticket_data.third_qr_info,operThirdGetETicket);

		//len_eticket = sizeof(eticketdealinfo);
		//len_eticket = sizeof(geteticketrespinfo);

	}while (0);

	if (ret.wErrCode)
	{
		if (ret.wErrCode == ERR_CARD_READ/* || ret.bNoticeCode == NTC_MUST_CONFIRM*/)
		{
			ret.wErrCode = ERR_CARD_NONE;
			ret.bNoticeCode = 0;
		}
	}

	if (ret.wErrCode != ERR_CARD_NONE)
	{
		memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
	}

	//memset(Api::last_qr_readbuf,0x00,sizeof(Api::last_qr_readbuf));

	//len_data = organize_data_region(data_to_send, &ret, len_eticket, &eticketdealinfo, len_lock, &locked);
	if(ret.wErrCode != 0)
		len_data = organize_data_region(data_to_send, &ret, 0, NULL);
	else
		len_data = organize_data_region(data_to_send, &ret, 64, ticket_data.third_qr_info);
	if(ret.wErrCode != ERR_CARD_NONE)
		g_Record.log_out(0, level_disaster,"Tvm_ThirdPayGetETicket = %s",ticket_data.third_qr_info);
}


void Api::Tvm_ThirdPay_Sale(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	TicketBase * p_ticket		= NULL;
	TICKET_PARAM ticket_prm		= {0};
	TICKET_DATA ticket_data;

	SJTSALE sjt_sale;
	uint16_t len_sjt_sale		= 0;

	OTHERSALE svt_sale;
	uint16_t len_svt_sale		= 0;

	ETICKETDEALINFO eticketdealinfo;
	uint16_t len_eticket_sale		= 0;

	uint8_t amount_type			= 0;
	char tempAmount[6] = {0};

	long time_use=0;
	struct timeval start;
	struct timeval end;
	long msec = 0;

	gettimeofday(&start,NULL);


	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;
		//首先清掉二维码数据//
		memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码

		memset(&ticket_data, 0, sizeof(ticket_data));
		memset(&sjt_sale, 0, sizeof(sjt_sale));
		memset(&eticketdealinfo, 0, sizeof(eticketdealinfo));

		memcpy(&sjt_sale.nChargeValue, param_stack, 2);
		sjt_sale.bPaymentMeans = param_stack[2];
		amount_type = param_stack[3];

		sprintf(tempAmount,"%06d",sjt_sale.nChargeValue);

		memcpy(&eticketdealinfo.cNetTranNumber, param_stack + 4, 64);//增加互联网订单号
		sprintf(eticketdealinfo.cPayType, "%02x",param_stack[2]);
		//memcpy(&eticketdealinfo.cTxnAmount, tempAmount, 6);//交易金额
		//g_Record.log_out_debug(0, level_disaster,",cTxnAmount=%s",eticketdealinfo.cTxnAmount);


		//g_Record.log_out_debug(0, level_disaster,"Tvm_ThirdPay_Sale(cNetTranNumber=%s",eticketdealinfo.cNetTranNumber);
		//g_Record.log_buffer_debug("param_stack:",param_stack,68);
		ret.wErrCode = api_match_device(device_type_file);
		if (ret.wErrCode != 0)	break;

		gettimeofday(&start,NULL);
		rfdev_get_rfstatus(1);//在发售前首先对主天线板进行复位
		gettimeofday(&end,NULL);

		time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
		msec = time_use / 1000;
		g_Record.log_out(ret.wErrCode, level_disaster,"rfdev_get_rfstatus use time = [%ld]",msec);

		ret.wErrCode = TicketBase::search_card(&p_ticket);
		if (ret.wErrCode != 0)	break;

		ret = p_ticket->read_card(operSale, ticket_prm);
		///ret.wErrCode = ERR_CARD_READ;
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

		ret = p_ticket->thirdesale(&sjt_sale, len_sjt_sale, &eticketdealinfo, len_eticket_sale, ticket_prm,eticketdealinfo.cNetTranNumber);

	}
	while (0);

	m_trade_val = 0;

	g_Record.add_record(sjt_sale.cTradeType, &sjt_sale, len_sjt_sale);
	//g_Record.add_record(svt_sale.cTradeType, &svt_sale, len_svt_sale);

	len_data = organize_data_region(data_to_send, &ret, len_sjt_sale, &sjt_sale, len_eticket_sale, &eticketdealinfo);

	gettimeofday(&end,NULL);

	time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
	msec = time_use / 1000;

	g_Record.log_out(ret.wErrCode, level_disaster,
		"Tvm_ThirdPay_Sale(lid=%s,value=%d,pay_type=%d,amount_type=%d)=(%04X,%02X)  use time = [%ld]",
		ticket_data.logical_id, sjt_sale.nChargeValue, sjt_sale.bPaymentMeans, amount_type, ret.wErrCode, ret.bNoticeCode,msec);
}

