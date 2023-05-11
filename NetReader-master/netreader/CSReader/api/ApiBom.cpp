#include "Api.h"
#include "Publics.h"
#include "ParamMgr.h"
#include "TicketUl.h"
#include "Declares.h"
#include "../link/linker.h"
#include "TimesEx.h"
#include "Errors.h"
#include "Records.h"
#include "../link/myprintf.h"
#include "TimeCalcUtils.h"

char Api::operator_id[7]			= "000000";
uint8_t Api::bom_shift_id			= 0;
uint8_t Api::work_area				= 1;

static ETPDVC device_type_file		= dvcBOM;

TicketBase * g_p_ticket				= NULL;
uint8_t locked_analyse_id[20]		= {0};

//
// BOM接口
//
void Api::Bom_Login(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret = {0};
	ret.wErrCode = g_Parameter.device_initialized();
	if (ret.wErrCode == 0)
	{
		ret.wErrCode = api_match_device(device_type_file);
		if (ret.wErrCode == 0)
		{
			memcpy(operator_id, param_stack, 6);
			bom_shift_id = param_stack[6];
		}
	}

    len_data = organize_data_region(param_stack, &ret);

	g_Record.log_out(ret.wErrCode, level_invalid,
		"Bom_Login(operator_id=%s)=(%04X,%02X)", operator_id, ret.wErrCode, ret.bNoticeCode);
}

void Api::Bom_TicketAnalyze(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret					= {0};
    TicketBase * p_ticket		= NULL;
	TICKET_PARAM ticket_prm		= {0};
    TICKET_DATA ticket_data;

    MODE_EFFECT_FLAGS md_effect;
    BOMANALYZE analyse;
	BOMETICANALYZE eticanalyse;
    TICKETLOCK locked;
    uint16_t len_analyse		= 0;
	uint16_t len_eticanalyse	= 0;
    uint16_t len_lock			= 0;

    uint8_t use_degrade_flag	= 0; //启用降级模式 0 为不启用  其他为启用
	bool add_opers				= false;

	struct timeval start;
	struct timeval end;
	long msec = 0;
	gettimeofday(&start,NULL);

    do
    {
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		if (TicketBase::check_confirm_his(1440))
		{
			ret.bNoticeCode = NTC_MUST_CONFIRM;
			break;
		}

		memset(&ticket_data, 0, sizeof(ticket_data));
        memset(&analyse, 0, sizeof(analyse));
        memset(&locked, 0, sizeof(locked));
		memset(&md_effect, 0, sizeof(md_effect));
		memset(&eticanalyse, 0, sizeof(eticanalyse));

        use_degrade_flag = param_stack[0];
        work_area = param_stack[1];// 1为付费区  2为非付费区

        ret.wErrCode = api_match_device(device_type_file);
        if (ret.wErrCode != 0)	break;
        //1，寻卡
        ret.wErrCode = TicketBase::search_card(&p_ticket, NULL, NULL, NULL, operAnalyse);
        if (ret.wErrCode != 0)	break;
        //2，读卡
        ret = p_ticket->read_card(operAnalyse, ticket_prm);
        if (ret.wErrCode != 0)	break;
		p_ticket->get_ticket_infos(ticket_data);

		if(ticket_data.qr_flag == 0x00)//传统票卡
		{
			len_analyse = sizeof(analyse);
            analyse.bStatus = p_ticket->unified_status();
            memcpy(locked_analyse_id, ticket_data.logical_id, 20);
            g_Record.log_buffer("analyse:locked_analyse_id:",locked_analyse_id,20);
		}
		else
		{
			len_eticanalyse = sizeof(eticanalyse);
		}


		if (use_degrade_flag > 0) {
            p_ticket->get_degrade_mode(md_effect, CmdSort::m_time_now);
        }
        ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
        if (ret.wErrCode)	break;
        ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
        if (ret.wErrCode != 0)	break;

		add_opers = true;

		if(ticket_data.qr_flag == 0x00){//传统票卡
            ret = p_ticket->analyse_common(work_area, &md_effect, &ticket_prm, &locked, len_lock, &analyse, NULL);
            if (ret.wErrCode != 0)	break;
		}

        if (work_area == 1)	// 付费区
        {
			if(ticket_data.qr_flag == 0x00)
			{
				ret = p_ticket->analyse_work_zone(&md_effect, &ticket_prm, &analyse, NULL, NULL);
			}
			else
			{
				ret = p_ticket->analyse_work_zone(&md_effect, &ticket_prm, NULL, NULL, &eticanalyse);
			}
        }else				// 非付费区
        {
			if(ticket_data.qr_flag == 0x00)
			{
				ret = p_ticket->analyse_free_zone(&md_effect, &ticket_prm, &analyse,NULL);
			}
			else
			{
				ret = p_ticket->analyse_free_zone(&md_effect, &ticket_prm, NULL, &eticanalyse);
			}
        }


    } while (0);
    //组分析数据包
	if(ticket_data.qr_flag == 0x00)
	{
		if (len_analyse > 0 && add_opers)
		{
			analyse.dwOperationStauts |= p_ticket->add_oper_status(work_area, ret, &ticket_prm);
			p_ticket->transfer_data_for_out(operAnalyse, &analyse);
		}
	}else{
		if (len_eticanalyse > 0 && add_opers)
		{
			ret.bRfu[0] = 0x11;
			ret.bRfu[1] = 0x00;
			p_ticket->transfer_edata_for_out(operAnalyse, &eticanalyse);
		}
	}

	g_Record.add_record(locked.cTradeType, &locked, len_lock);
    //组给上位机的包
	if(ticket_data.qr_flag == 0x00) {
        len_data = organize_data_region(data_to_send, &ret, len_analyse, &analyse, len_lock, &locked);
    }else {
        len_data = organize_data_region(data_to_send, &ret, len_eticanalyse, &eticanalyse, len_lock, &locked);
    }

	if (ret.wErrCode != ERR_CARD_NONE)
	{
		memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
		memset(Api::clear_qr_readbuf,0x00,sizeof(Api::clear_qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
	}

    gettimeofday(&end,NULL);
    msec=TimeCalcUtils::calcMsec(start,end);
	g_Record.log_out(ret.wErrCode, level_error,
		"Bom_TicketAnalyze(lid=%s,work_area=%d)=(%04X,%02X) use time=[%ld]",
		ticket_data.logical_id, work_area, ret.wErrCode, ret.bNoticeCode,msec);
}

void Api::Bom_Ticket_Sale(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
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

		if (TicketBase::check_confirm_his(1440))
		{
			ret.bNoticeCode = NTC_MUST_CONFIRM;
			break;
		}

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

		dbg_dumpmemory("locked_analyse_id:",locked_analyse_id,20);
		dbg_dumpmemory("logical_id:",ticket_data.logical_id,20);

		if (memcmp(locked_analyse_id, ticket_data.logical_id, 20) != 0)
		{
			ret.wErrCode = ERR_DIFFRENT_CARD;
			break;
		}

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

	g_Record.add_record(sjt_sale.cTradeType, &sjt_sale, len_sjt_sale);
	g_Record.add_record(svt_sale.cTradeType, &svt_sale, len_svt_sale);

	len_data = organize_data_region(data_to_send, &ret, len_sjt_sale, &sjt_sale, len_svt_sale, &svt_sale);

	g_Record.log_out(ret.wErrCode, level_error,
		"Bom_Ticket_Sale(lid=%s,value=%d,pay_type=%d,amount_type=%d)=(%04X,%02X)",
		ticket_data.logical_id, sjt_sale.nChargeValue, sjt_sale.bPaymentMeans, amount_type, ret.wErrCode, ret.bNoticeCode);
}

void Api::Bom_SvtIncrease1(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO		ret				= {0};
	TICKET_PARAM ticket_prm;
	TICKET_DATA ticket_data;

	CHARGE_INIT	chin;
	uint16_t len_chin			= 0;
	uint8_t init_out[128]		= {0};
	char sz_temp[32]			= {0};

	//dbg_formatvar("Bom_SvtIncrease1");

	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		if (TicketBase::check_confirm_his(1440))
		{
			ret.bNoticeCode = NTC_MUST_CONFIRM;
			break;
		}

		memset(&ticket_data, 0, sizeof(ticket_data));
	    memset(&ticket_prm, 0, sizeof(ticket_prm));
	    memset(&chin, 0, sizeof(chin));

		chin.cWalletType = param_stack[0] + 0x30;
		memcpy(&chin.lAmount, param_stack + 1, 4);
		m_trade_val = chin.lAmount;
		memcpy(chin.cNetPoint, param_stack + 5, sizeof(chin.cNetPoint));
		memcpy(chin.dtMsgTime, CmdSort::m_time_now, 7);

		//g_Record.log_out(0, level_error, "Bom_SvtIncrease1(value=%d)",chin.lAmount);

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

		if (memcmp(locked_analyse_id, ticket_data.logical_id, 20) != 0)
		{
			ret.wErrCode = ERR_DIFFRENT_CARD;
			break;
		}

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

	dbg_formatvar("Bom_SvtIncrease1(lid=%s,value=%d,wallet_type=%c)=(%04X,%02X)",
		ticket_data.logical_id, chin.lAmount, chin.cWalletType, ret.wErrCode, ret.bNoticeCode);

	g_Record.log_out(ret.wErrCode, level_error, "Bom_SvtIncrease1(lid=%s,value=%d,wallet_type=%c)=(%04X,%02X)",
		ticket_data.logical_id, chin.lAmount, chin.cWalletType, ret.wErrCode, ret.bNoticeCode);
}

void Api::Bom_SvtIncrease2(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO		ret				= {0};
	TICKET_PARAM ticket_prm		= {0};
	TICKET_DATA ticket_data;

	PURSETRADE	pstd;
	uint16_t len_pstd			= 0;
	uint8_t bTime[7]			= {0};
	uint8_t Mac2[4]				= {0};
    uint8_t paytype = 0;
	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		if (TicketBase::check_confirm_his(1440))
		{
			ret.bNoticeCode = NTC_MUST_CONFIRM;
			break;
		}

		memset(&ticket_data, 0, sizeof(ticket_data));
	    memset(&pstd, 0, sizeof(pstd));

		memcpy(bTime, param_stack + 2, 7);
		Publics::string_to_hexs((char *)(param_stack + 62), 8, Mac2, 4);
		paytype = param_stack[77];

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

		//ret = g_p_ticket->svt_increase(&pstd, bTime, Mac2);
		//if (ret.wErrCode != 0)	break;
		ret = g_p_ticket->svt_increase(&pstd, bTime, Mac2 , paytype);
		if((ret.wErrCode==ERR_CARD_WRITE)&&(ret.bNoticeCode == NTC_MUST_CONFIRM))
		{
			///////////////////////////////////////////////////////////////////
			/////////////在读写器内部进行交易确认//////////////////////////////
			///////////////////////////////////////////////////////////////////

			ret.wErrCode = TicketBase::search_card(&g_p_ticket);
			if (ret.wErrCode != 0)	break;

			ret = g_p_ticket->read_card(operIncr, ticket_prm);
			if (ret.wErrCode != 0)	break;

			g_p_ticket->get_ticket_infos(ticket_data);

			if (!g_p_ticket->confirm_current_card())
				break;

			if (!g_p_ticket->check_confirm_his(10))
				break;

			if (!g_p_ticket->last_trade_need_continue(0, 0, 0))
				break;

			ret = g_p_ticket->continue_last_trade(&pstd);
			if(ret.wErrCode != 0)
			{
				ret.wErrCode = ERR_CARD_WRITE;
				ret.bNoticeCode = NTC_MUST_CONFIRM;
				break;
			}
			g_p_ticket->clear_confirm(ticket_data.logical_id, false, true);
			ret.wErrCode = ERR_SUCCEED;
			ret.bNoticeCode = NTC_NONE;

		}else if (ret.wErrCode != 0)	break;

		pstd.lSAMTrSeqNo = m_sam_seq_charge;

		len_pstd = sizeof(PURSETRADE);
	}
	while (0);

	m_trade_val = 0;

	g_Record.add_record(pstd.cTradeType, &pstd, len_pstd);

	len_data = organize_data_region(data_to_send, &ret, len_pstd, &pstd);

	dbg_formatvar("Bom_SvtIncrease2(lid=%s)=(%04X,%02X)", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode);

	g_Record.log_out(ret.wErrCode, level_error,
		"Bom_SvtIncrease2(lid=%s)=(%04X,%02X)", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode);
}

void Api::Bom_TransactConfirm(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret = {0};
	TicketBase * p_ticket		= NULL;
	TICKET_PARAM ticket_prm		= {0};
	TICKET_DATA ticket_data;

	uint16_t len_trans			= 0;
	size_t size_trade			= 0;
	uint8_t oper_select			= 0;
	TRANSACTCONFIRM tc;
	do
	{
		memset(&tc, 0, sizeof(tc));

		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)	break;

		memset(&ticket_data, 0, sizeof(ticket_data));
		oper_select = param_stack[0];
		if (oper_select != 0 && oper_select != 1)
		{
			ret.wErrCode = ERR_INPUT_PARAM;
			break;
		}

		if (oper_select == 1)//如果为解锁指令，则解除读写器锁定
		{
			p_ticket->unlock_reader(tc.bData);
		}
		else
		{
			ret.wErrCode = api_match_device(device_type_file);
			if (ret.wErrCode != 0)	break;

			ret.wErrCode = TicketBase::search_card(&p_ticket);
			if (ret.wErrCode != 0)	break;

			ret = p_ticket->read_card(p_ticket->confirm_trade_type(), ticket_prm);
			if (ret.wErrCode != 0)	break;

			p_ticket->get_ticket_infos(ticket_data);

			ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
			if (ret.wErrCode)	break;

			// 设备支持
			ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
			if (ret.wErrCode != 0)	break;

			if (!p_ticket->confirm_current_card())
			{
				ret.wErrCode = ERR_DIFFRENT_CARD;
				break;
			}
			g_Record.log_out(0, level_normal,"Bom_TransactConfirm confirm_trade tc.bTradeType=(%d)",tc.bTradeType);
			//FIXME:判断上一笔交易是否继续，去掉判断基本实现想法
			if (!p_ticket->last_trade_need_continue(0, 0, 0))
			{
				g_Record.log_out(0, level_normal,"Bom_TransactConfirm not need continue");
				ret.wErrCode = ERR_NEED_NOT_CONFIRM;
				p_ticket->clear_confirm(ticket_data.logical_id, false, true);
				break;
			}
			ret = p_ticket->continue_last_trade(tc.bData);
			if (ret.wErrCode != 0)	break;
			size_trade = p_ticket->size_of_confirm_trade();
			g_Record.log_out(0, level_normal,"Bom_TransactConfirm confirm_trade size=(%x)",size_trade);
		}

		tc.bTradeType = Publics::string_to_hex<uint8_t>((char *)(tc.bData), 2);
		g_Record.log_out(0, level_normal,"Bom_TransactConfirm confirm_trade tc.bTradeType=(%d)",tc.bTradeType);
		
		len_trans = sizeof(tc);
		p_ticket->clear_confirm(ticket_data.logical_id, false, true);

	} while (0);

	//保存交易到本地
	if(ret.wErrCode!=0){
		g_Record.add_record((char *)(tc.bData), tc.bData, size_trade);
	}

	len_data = organize_data_region(data_to_send, &ret, len_trans, &tc);
	dbg_formatvar("Bom_TransactConfirm(lid=%s)=(%04X,%02X)", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode);
	g_Record.log_out(ret.wErrCode, level_error,
		"Bom_TransactConfirm(lid=%s)=(%04X,%02X)", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode);
}

void Api::Bom_ExitSale(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret					= {0};
    TicketBase * p_ticket		= NULL;
    TICKET_PARAM ticket_prm;
    TICKET_DATA ticket_data;

    SJTSALE sjt_sale;
    uint16_t len_sjt_sale		= 0;

    //uint8_t free_fare			= 0;
    //uint8_t policy_code			= 0;
	short policy_penalty		= 0;

    do
    {
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		if (TicketBase::check_confirm_his(1440))
		{
			ret.bNoticeCode = NTC_MUST_CONFIRM;
			break;
		}

		memset(&ticket_data, 0, sizeof(ticket_data));
		memset(&ticket_prm, 0, sizeof(ticket_prm));
		memset(&sjt_sale, 0, sizeof(sjt_sale));

		//free_fare = param_stack[0];
		//policy_code = param_stack[1];
		memcpy(&policy_penalty, param_stack, 2);

		ret.wErrCode = api_match_device(device_type_file);
		if (ret.wErrCode != 0)	break;

        ret.wErrCode = TicketBase::search_card(&p_ticket);
        if (ret.wErrCode != 0)	break;

        ret = p_ticket->read_card(operAnalyse, ticket_prm);
        if (ret.wErrCode != 0)	break;

        p_ticket->get_ticket_infos(ticket_data);

		if (memcmp(locked_analyse_id, ticket_data.logical_id, 20) != 0)
		{
			ret.wErrCode = ERR_DIFFRENT_CARD;
			break;
		}

        ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
        if (ret.wErrCode)	break;

        // 设备支持
        ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
        if (ret.wErrCode != 0)	break;

        // 判断是否允许发售
        if(ticket_data.simple_status == SStatus_Exit_Only)
        {
			if (g_Parameter.same_station(ticket_data.read_last_info.station_id, Api::current_station_id))
        	{
				if (memcmp(CmdSort::m_time_now, ticket_data.logical_peroidS, 7) >= 0 && memcmp(CmdSort::m_time_now, ticket_data.logical_peroidE, 7) <= 0)
        		{
        			ret.wErrCode = ERR_CALLING_INVALID;
        			break;
        		}
			}
        }

        // 部分字段赋值
		//if (free_fare == 0)	// 免费出站票
		//{
		//	sjt_sale.nChargeValue = 0;
		//}
		//else if (free_fare == 1)
		//{
		//	ret.wErrCode = g_Parameter.query_policy_penalty(sjt_sale.bTicketType, policy_code, sjt_sale.nChargeValue);
		//	if (ret.wErrCode != 0)		break;
		//}
		//else
		//{
		//	ret.wErrCode = ERR_INPUT_PARAM;
		//	break;
		//}

		sjt_sale.nChargeValue = policy_penalty;

        ret = p_ticket->exit_sjt_sale(&sjt_sale);
        if (ret.wErrCode != 0)		break;

        len_sjt_sale = sizeof(sjt_sale);

    }
    while (0);

	g_Record.add_record(sjt_sale.cTradeType, &sjt_sale, len_sjt_sale);

    len_data = organize_data_region(data_to_send, &ret, len_sjt_sale, &sjt_sale);

	g_Record.log_out(ret.wErrCode, level_error,
		"Bom_ExitSale(lid=%s,policy_penalty=%d)=(%04X,%02X)",
		ticket_data.logical_id, policy_penalty, ret.wErrCode, ret.bNoticeCode);
}

void Api::Bom_TicketUpdate(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret					= {0};
    TicketBase * p_ticket		= NULL;
    TICKET_PARAM ticket_prm;
    TICKET_DATA ticket_data;

    TICKETUPDATE ticket_update;
    uint16_t len_ticket_update	= 0;

    uint8_t reason_update = 0;
    uint8_t entry_station[2];
    short val_update = 0;
    uint8_t pay_mode = 0;

    do
    {
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		if (TicketBase::check_confirm_his(1440))
		{
			ret.bNoticeCode = NTC_MUST_CONFIRM;
			break;
		}

		memset(&ticket_data, 0, sizeof(ticket_data));
        memset(&ticket_prm, 0, sizeof(ticket_prm));
        memset(&ticket_update, 0, sizeof(ticket_update));

        reason_update = param_stack[0];
        memcpy(entry_station, param_stack + 1, 2);
        memcpy(&val_update, param_stack + 3, 2);
        pay_mode = param_stack[5];

        ret.wErrCode = api_match_device(device_type_file);
        if (ret.wErrCode != 0)	break;

        ret.wErrCode = TicketBase::search_card(&p_ticket);
        if (ret.wErrCode != 0)	break;


        ret = p_ticket->read_card(operUpdate, ticket_prm);
        if (ret.wErrCode != 0)	break;

        p_ticket->get_ticket_infos(ticket_data);

		if (memcmp(locked_analyse_id, ticket_data.logical_id, 20) != 0)
		{
			ret.wErrCode = ERR_DIFFRENT_CARD;
			break;
		}

        ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
        if (ret.wErrCode)	break;

        // 设备支持
        ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
        if (ret.wErrCode != 0)	break;

        // 部分字段赋值
        ticket_update.bPaymentMode = pay_mode;
        ticket_update.bUpdateReasonCode = Publics::val_to_bcd(reason_update);
        ticket_update.cUpdateZone = Api::work_area + '0';
        ticket_update.nForfeiture = val_update;

        ret = p_ticket->bom_update(&ticket_update, entry_station);
		ticket_update.cUpdateZone = Api::work_area + '0';			// 赋值偶尔有非法字符,此处再次赋值,解决问题?
        //FIXME:如果出现了异常，直接Break掉了，由于size为0，故无法写入文件
		if (ret.wErrCode != 0)	break;

        len_ticket_update = sizeof(ticket_update);

    }
    while (0);

	g_Record.add_record(ticket_update.cTradeType, &ticket_update, len_ticket_update);

    len_data = organize_data_region(data_to_send, &ret, len_ticket_update, &ticket_update);

	g_Record.log_out(ret.wErrCode, level_error,
		"Bom_TicketUpdate(lid=%s,reason=%x,entryid=%02x%02x,penalty=%d,pay_mode=%d)=(%04X,%02X)",
		ticket_data.logical_id, reason_update, entry_station[0], entry_station[1], val_update, pay_mode, ret.wErrCode, ret.bNoticeCode);
}

void Api::Bom_EticketUpdate(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret					= {0};
    TicketBase * p_ticket		= NULL;
    TICKET_PARAM ticket_prm;
    TICKET_DATA ticket_data;
	ETICKETDEALINFO eticketdealinfo;
    uint16_t len_eticketdealinfo = 0;
    ETICKET_ABNORML_HANDLE abnormlHandle;
	uint8_t  p_eTicket_info[200]	= {0};
	uint16_t len_eTicket_info		= 0;
	long penalty = 0;
	do
    {
		ret.bRfu[0] = 0x11;
		ret.bRfu[1] = 0x00;
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0) break;
		//todo: 需要交易确认？
		if (TicketBase::check_confirm_his(1440))//24*60
		{
			ret.bNoticeCode = NTC_MUST_CONFIRM;
			break;
		}

		memset(&ticket_data, 0, sizeof(ticket_data));
        memset(&ticket_prm, 0, sizeof(ticket_prm));
        memset(&eticketdealinfo, 0, sizeof(eticketdealinfo));
        memset(&abnormlHandle, 0, sizeof(abnormlHandle));

        memcpy(abnormlHandle.trad_type, param_stack, 2);
		memcpy(abnormlHandle.center_code, param_stack + 2, 32);
        memcpy(&abnormlHandle.amount_timeout, param_stack + 34, 4);
        memcpy(&abnormlHandle.amount_over_take, param_stack + 38, 4);
        memcpy(abnormlHandle.station_code, param_stack + 42, 4);
        memcpy(abnormlHandle.net_result, param_stack + 46, 1);
		g_Record.log_out(0, level_error,"bom net_result=[%s]",abnormlHandle.net_result);
		g_Record.log_out(0, level_disaster, "center_code = %s ", abnormlHandle.center_code);
		g_Record.log_out(0, level_error, "station = %s ", abnormlHandle.station_code);

		ret.wErrCode = api_match_device(device_type_file);
        if (ret.wErrCode != 0)	break;
        //1，寻卡，读卡，此处只做电子票的寻卡分析
		//1.1从之前电子票分析接口中获取码体信息，这样就不需要重新扫码了
		memcpy(Api::qr_readbuf, Api::qr_elect_analy_readbuf, sizeof(Api::qr_elect_analy_readbuf));
        ret.wErrCode = TicketBase::search_card(&p_ticket, NULL, NULL, NULL, operGetETicket);
		g_Record.log_out(0, level_disaster, "search_card Bom_EticketUpdate");
        if (ret.wErrCode != 0)	break;
        ret = p_ticket->read_card(operUpdate, ticket_prm);
        if (ret.wErrCode != 0)	break;
        p_ticket->get_ticket_infos(ticket_data);

        ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
        if (ret.wErrCode)	break;
        // 设备支持
        ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
        if (ret.wErrCode != 0)	break;

        memcpy(eticketdealinfo.cCenterCode,abnormlHandle.center_code,32);
		switch (abnormlHandle.trad_type[1])
		{
		case '0':
			memcpy(eticketdealinfo.cTransCode, "03", 2);//超程更新
			break;
		case '1':
			memcpy(eticketdealinfo.cTransCode, "02", 2);//超时更新
			break;
		case '3':
		case '5':
			memcpy(eticketdealinfo.cTransCode, "04", 2);//未出站(清出站，补出站)
			break;
		case '2':
		case '4':
			memcpy(eticketdealinfo.cTransCode, "05", 2);//未入站(清进站，补进站)
			break;
		default:
			break;
		}
		g_Record.log_out(0, level_warning, "abnormlHandle  trad_type= %d ", abnormlHandle.trad_type[1]);
		g_Record.log_out(0, level_disaster, "cTransCode = %s ", eticketdealinfo.cTransCode);
        memcpy(eticketdealinfo.cTicketMainType,"11",2);//车票类型  电子票11
        memcpy(eticketdealinfo.cTicketSubType,ticket_data.qr_ticketdata.subType,2);//车票子类型对应票种：00 电子单程票 01 电子后付费储值票 02电子预付费储值票
		len_eticketdealinfo = sizeof(eticketdealinfo);
        ret = p_ticket->bom_elect_update(&abnormlHandle, p_eTicket_info, len_eTicket_info);

    }
    while (0);

	if(ret.wErrCode==ERR_SUCCEED){
		p_ticket->transfer_edata_for_out(operUpdate, &eticketdealinfo);
		//1,交易日志记录到本地
		g_Record.add_record("60", &eticketdealinfo, len_eticketdealinfo);
	}
	g_Record.log_out(0, level_warning, "bom_elect_update2");
	if (ret.wErrCode != ERR_CARD_NONE)
	{
		memset(Api::qr_readbuf, 0x00, sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
		memset(Api::clear_qr_readbuf, 0x00, sizeof(Api::clear_qr_readbuf));//交易结束，不管是否成功，再次清掉二维码

	}else if(ret.wErrCode==ERR_SUCCEED){
		memset(Api::qr_elect_analy_readbuf, 0x00, sizeof(Api::qr_elect_analy_readbuf));//更新完毕之后，清除缓存的信息

	}
	g_Record.log_out(0, level_warning, "bom_elect_update3");

    //2 组包发送给上位机
    len_data = organize_data_region(data_to_send, &ret, len_eticketdealinfo, &eticketdealinfo);
	g_Record.log_out(0, level_warning, "bom_elect_update1");
	g_Record.log_out(ret.wErrCode, level_error,
		"Bom_EticketUpdate(trad_type=%d,station_code=%s,amount_over_take=%d,amount_timeout=%d)=(%04X,%02X)",
		abnormlHandle.trad_type[1], abnormlHandle.station_code,abnormlHandle.amount_over_take, abnormlHandle.amount_timeout, ret.wErrCode, ret.bNoticeCode);
}

void Api::Bom_PurseDecrease(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret					= {0};
	TicketBase * p_ticket		= NULL;
	TICKET_PARAM ticket_prm;
	TICKET_DATA ticket_data;

	PURSETRADE decr;
	uint16_t len_decr			= 0;

	dbg_formatvar("Bom_PurseDecrease");

	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		if (TicketBase::check_confirm_his(1440))
		{
			ret.bNoticeCode = NTC_MUST_CONFIRM;
			break;
		}

		memset(&ticket_data, 0, sizeof(ticket_data));
	    memset(&ticket_prm, 0, sizeof(ticket_prm));
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

		if (memcmp(locked_analyse_id, ticket_data.logical_id, 20) != 0)
		{
			ret.wErrCode = ERR_DIFFRENT_CARD;
			break;
		}

		ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
		if (ret.wErrCode)	break;

		// 设备支持
		ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
		if (ret.wErrCode != 0)	break;
		//默认传卡扣
		ret = p_ticket->svt_decrease(&decr, PAY_UPD_PENALTY,2);
		if (ret.wErrCode)	break;

		dbg_formatvar("len_decr");

		len_decr = sizeof(PURSETRADE);

		dbg_formatvar("len_decr = %d",len_decr);

	}
	while (0);

	g_Record.add_record(decr.cTradeType, &decr, len_decr);

	len_data = organize_data_region(data_to_send, &ret, len_decr, &decr, 0, NULL);

	g_Record.log_out(ret.wErrCode, level_error,
		"Bom_PurseDecrease(lid=%s,val=%d)=(%04X,%02X)", ticket_data.logical_id, decr.lTradeAmount, ret.wErrCode, ret.bNoticeCode);
}

void Api::Bom_TicketDeffer(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret					= {0};
	TicketBase * p_ticket		= NULL;
	TICKET_PARAM ticket_prm;
	TICKET_DATA ticket_data;

	TICKETDEFER td;
	uint16_t len_td				= 0;

	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		if (TicketBase::check_confirm_his(1440))
		{
			ret.bNoticeCode = NTC_MUST_CONFIRM;
			break;
		}

		memset(&ticket_data, 0, sizeof(ticket_data));
	    memset(&ticket_prm, 0, sizeof(ticket_prm));
	    memset(&td, 0, sizeof(td));

		ret.wErrCode = api_match_device(device_type_file);
		if (ret.wErrCode != 0)	break;

		ret.wErrCode = TicketBase::search_card(&p_ticket);
		if (ret.wErrCode != 0)	break;


		ret = p_ticket->read_card(operDeffer, ticket_prm);
		if (ret.wErrCode != 0)	break;

		p_ticket->get_ticket_infos(ticket_data);

		if (memcmp(locked_analyse_id, ticket_data.logical_id, 20) != 0)
		{
			ret.wErrCode = ERR_DIFFRENT_CARD;
			break;
		}

		ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
		if (ret.wErrCode)	break;

		// 设备支持
		ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
		if (ret.wErrCode != 0)	break;

		// 部分字段赋值
		memcpy(td.dtOldExpiryDate, ticket_data.logical_peroidE, 7);
		memcpy(td.dtNewExpiryDate, CmdSort::m_time_now, 7);
		TimesEx::bcd_time_calculate(td.dtNewExpiryDate, T_DATE, ticket_prm.deffer_days);

		ret = p_ticket->deffer(&td, ticket_prm);
		if (ret.wErrCode)	break;

		len_td = sizeof(td);

	}
	while (0);

	g_Record.add_record(td.cTradeType, &td, len_td);

	len_data = organize_data_region(data_to_send, &ret, len_td, &td);

	g_Record.log_out(ret.wErrCode, level_error,
		"Bom_TicketDeffer(lid=%s)=(%04X,%02X)", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode);
}

void Api::Bom_TicketUnLock(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret					= {0};
	TicketBase * p_ticket		= NULL;
	TICKET_PARAM ticket_prm;
	TICKET_DATA ticket_data;

	TICKETLOCK tl;
	uint16_t len_tl				= 0;

	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		if (TicketBase::check_confirm_his(1440))
		{
			ret.bNoticeCode = NTC_MUST_CONFIRM;
			break;
		}

		memset(&ticket_data, 0, sizeof(ticket_data));
	    memset(&ticket_prm, 0, sizeof(ticket_prm));
	    memset(&tl, 0, sizeof(tl));

		ret.wErrCode = api_match_device(device_type_file);
		if (ret.wErrCode != 0)	break;

		ret.wErrCode = TicketBase::search_card(&p_ticket);
		if (ret.wErrCode != 0)	break;


		ret = p_ticket->read_card(operLock, ticket_prm);
		if (ret.wErrCode != 0)	break;

		p_ticket->get_ticket_infos(ticket_data);

		if (memcmp(locked_analyse_id, ticket_data.logical_id, 20) != 0)
		{
			ret.wErrCode = ERR_DIFFRENT_CARD;
			break;
		}

		ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
		if (ret.wErrCode)	break;

		// 设备支持
		ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
		if (ret.wErrCode != 0)	break;

		// 部分字段赋值
		tl.cLockFlag = '2';
		ret = p_ticket->lock(&tl);
		if (ret.wErrCode)	break;

		len_tl = sizeof(tl);

	}
	while (0);

	g_Record.add_record(tl.cTradeType, &tl, len_tl);

	len_data = organize_data_region(data_to_send, &ret, len_tl, &tl);

	g_Record.log_out(ret.wErrCode, level_error,
		"Bom_TicketUnLock(lid=%s)=(%04X,%02X)", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode);
}

void Api::Bom_DirectRefund(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret					= {0};
    TicketBase * p_ticket		= NULL;
    TICKET_PARAM ticket_prm;
    TICKET_DATA ticket_data;

    DIRECTREFUND refund;
    uint16_t len_refund			= 0;

    uint8_t return_mode			= 0;
    uint16_t penalty_refund		= 0;
    uint8_t penalty_reason		= 0;

    do
    {
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		memset(&ticket_data, 0, sizeof(ticket_data));
        memset(&ticket_prm, 0, sizeof(ticket_prm));
        memset(&refund, 0, sizeof(refund));

        return_mode = param_stack[0];
        memcpy(&penalty_refund, param_stack + 1, 2);
        penalty_reason = param_stack[3];

        ret.wErrCode = api_match_device(device_type_file);
        if (ret.wErrCode != 0)	break;

        ret.wErrCode = TicketBase::search_card(&p_ticket);
        if (ret.wErrCode != 0)	break;


        ret = p_ticket->read_card(operRefund, ticket_prm);
        if (ret.wErrCode != 0)	break;

        p_ticket->get_ticket_infos(ticket_data);

		if (memcmp(locked_analyse_id, ticket_data.logical_id, 20) != 0)
		{
			ret.wErrCode = ERR_DIFFRENT_CARD;
			break;
		}

        ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
        if (ret.wErrCode)	break;

        // 设备支持
        ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
        if (ret.wErrCode != 0)	break;

        // 部分字段赋值
        refund.bForfeitReason = penalty_reason;
        refund.nForfeiture = penalty_refund;
        if (return_mode == 1)
        {
        	refund.lBalanceReturned = ticket_data.wallet_value;
        }
        else if (return_mode == 2)
        {
        	refund.nDepositReturned = (short)(ticket_data.deposit);
			if (!ticket_prm.permit_refund_deposite)
			{
				ret.wErrCode = ERR_INPUT_INVALID;
				break;
			}
        }
        else if (return_mode == 3)
        {
        	refund.lBalanceReturned = ticket_data.wallet_value;
        	refund.nDepositReturned = (short)(ticket_data.deposit);
			if (!ticket_prm.permit_refund_deposite)
			{
				ret.wErrCode = ERR_INPUT_INVALID;
				break;
			}
        }
        else
        {
        	ret.wErrCode = ERR_INPUT_INVALID;
        	break;
        }
		refund.lBrokerage = ticket_prm.refund_brokerage;
        ret = p_ticket->bom_refund(&refund);
        if (ret.wErrCode)	break;

        len_refund = sizeof(refund);

    }
    while (0);

	g_Record.add_record(refund.cTradeType, &refund, len_refund);

    len_data = organize_data_region(data_to_send, &ret, len_refund, &refund);

	g_Record.log_out(ret.wErrCode, level_error,
		"Bom_DirectRefund(lid=%s,return_mode=%d)=(%04X,%02X)",
		ticket_data.logical_id, return_mode, ret.wErrCode, ret.bNoticeCode);
}

void Api::Bom_GetTicketInfo(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
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
	if (ret.wErrCode != ERR_CARD_NONE)
	{
		memset(Api::qr_readbuf, 0x00, sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
		memset(Api::clear_qr_readbuf, 0x00, sizeof(Api::clear_qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
	}
}

void Api::Bom_Charge_Descind1(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO		ret				= {0};
	TICKET_PARAM ticket_prm;
	TICKET_DATA ticket_data;

	DESCIND_INIT	dcin;
	uint16_t len_dcin			= 0;
	uint8_t init_out[128]		= {0};

	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		if (TicketBase::check_confirm_his(1440))
		{
			ret.bNoticeCode = NTC_MUST_CONFIRM;
			break;
		}

		memset(&ticket_data, 0, sizeof(ticket_data));
		memset(&ticket_prm, 0, sizeof(ticket_prm));
		memset(&dcin, 0, sizeof(dcin));

		dcin.cWalletType = param_stack[0] + 0x30;
		memcpy(&dcin.lAmount, param_stack + 1, 4);
		memcpy(&dcin.lBalcance, param_stack + 5, 4);
		memcpy(dcin.cNetPoint, param_stack + 9, sizeof(dcin.cNetPoint));
		memcpy(dcin.cLogicalId, param_stack + 25, 20);
		memcpy(dcin.dtMsgTime, CmdSort::m_time_now, 7);

		ret.wErrCode = api_match_device(device_type_file);
		if (ret.wErrCode != 0)	break;

		ret.wErrCode = TicketBase::search_card(&g_p_ticket);
		if (ret.wErrCode != 0)	break;

		ret = g_p_ticket->read_card(operDecr, ticket_prm);
		if (ret.wErrCode != 0)	break;

		g_p_ticket->get_ticket_infos(ticket_data);

		ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
		if (ret.wErrCode)	break;

		// 设备支持
		ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
		if (ret.wErrCode != 0)	break;

		if (memcmp(dcin.cLogicalId, ticket_data.logical_id, 20) != 0 || dcin.lBalcance != ticket_data.wallet_value)
		{
			ret.wErrCode = ERR_CALLING_INVALID;
			break;
		}

		// 部分字段赋值
		memcpy(dcin.cMsgType, "53", 2);
		get_current_device_info(dcin.cStation, dcin.cDevType, dcin.cDevId);
		memcpy(dcin.cSamId, m_sam[0], 16);

		memcpy(dcin.cIssueCodeMain, ticket_data.issue_code, sizeof(dcin.cIssueCodeMain));
		memset(dcin.cIssueCodeSub, '0', sizeof(dcin.cIssueCodeSub));

		memcpy(dcin.bTicketType, ticket_data.logical_type, 2);
		memcpy(dcin.cLogicalId, ticket_data.logical_id, 20);
		memcpy(dcin.cPhysicalId, ticket_data.physical_id, 20);
		dcin.cTestFlag = ticket_data.key_flag + '0';
		memcpy(dcin.cBusinessType, "18", 2);
		dcin.lBalcance = ticket_data.wallet_value;
		memcpy(dcin.cLastPosID, dcin.cSamId, 16);
		memcpy(dcin.dtTimeLast, dcin.dtMsgTime, 7);
		memcpy(dcin.cOperatorId, operator_id, 6);

		m_trade_val = dcin.lAmount;

		ret.wErrCode = svt_recharge(0x02, CmdSort::m_time_now, SAM_SOCK_1, 0, dcin.lAmount, init_out);
		if (ret.wErrCode != 0)
		{
			ret.wErrCode = ERR_CARD_READ;
			break;
		}
		// 票卡返回按moto的字节对其标准
		dcin.lChargeCount = ticket_data.charge_counter;
		dcin.lTradeCount = ticket_data.total_trade_counter;

		dcin.lPosSeq = (init_out[18] << 24) + (init_out[19] << 16) + (init_out[20] << 8) + init_out[21];	// 终端交易序号

		len_dcin = sizeof(DESCIND_INIT);
	}
	while (0);

	len_data = organize_data_region(data_to_send, &ret, len_dcin, &dcin);

	g_Record.log_out(ret.wErrCode, level_error,
		"Bom_Charge_Descind1(lid=%s)=(%04X,%02X)", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode);
}

void Api::Bom_Charge_Descind2(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret					= {0};
	TICKET_PARAM ticket_prm;
	TICKET_DATA ticket_data;

	PURSETRADE decr;
	uint16_t len_decr			= 0;
    uint8_t paytype = 0;

	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		if (TicketBase::check_confirm_his(1440))
		{
			ret.bNoticeCode = NTC_MUST_CONFIRM;
			break;
		}

		memset(&ticket_data, 0, sizeof(ticket_data));
		memset(&ticket_prm, 0, sizeof(ticket_prm));
		memset(&decr, 0, sizeof(decr));
		paytype = param_stack[0];

		ret.wErrCode = api_match_device(device_type_file);
		if (ret.wErrCode != 0)
			break;

		g_p_ticket->get_ticket_infos(ticket_data);

		ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
		if (ret.wErrCode)	break;

		// 设备支持
		ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
		if (ret.wErrCode != 0)	break;

		decr.lTradeAmount = m_trade_val;
		ret = g_p_ticket->svt_decrease(&decr, PAY_INC_DESIND,paytype);
		if (ret.wErrCode)	break;

		len_decr = sizeof(PURSETRADE);

	}
	while (0);

	g_Record.add_record(decr.cTradeType, &decr, len_decr);

	len_data = organize_data_region(data_to_send, &ret, len_decr, &decr);

	g_Record.log_out(ret.wErrCode, level_error,
		"Bom_Charge_Descind2(lid=%s)=(%04X,%02X)", ticket_data.logical_id, ret.wErrCode, ret.bNoticeCode);
}

void Api::Bom_Query_Policy_Penalty(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret				= {0};
	short policy_penalty	= 0;
	uint8_t ticket_type[2]	= {0};
	uint8_t policy_code		= 0;

	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)	break;

		memcpy(ticket_type, param_stack, 2);
		policy_code = param_stack[2];

		ret.wErrCode = g_Parameter.query_policy_penalty(ticket_type, policy_code, policy_penalty);
		if (ret.wErrCode != 0)		break;

	} while (0);

	len_data = organize_data_region(data_to_send, &ret, sizeof(policy_penalty), &policy_penalty);

	g_Record.log_out(ret.wErrCode, level_error,
		"Bom_Query_Policy_Penalty(ticket_type=%2x%02x,code=%02d)=(%04X,%02X)",
		ticket_type[0], ticket_type[1], policy_code, ret.wErrCode, ret.bNoticeCode);
}

void Api::Bom_Query_OverTrip_Val(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret			= {0};
	long over_trip_var	= 0;
	long balance		= 0;
	uint8_t ticket_type[2];
	uint16_t entry_station;

	ret.wErrCode = g_Parameter.device_initialized();
	if (ret.wErrCode == 0)
	{
		memcpy(&ticket_type, param_stack, 2);
		memcpy(&balance, param_stack + 2, 4);
		entry_station = (uint16_t)((param_stack[6] << 8) + param_stack[7]);

		//g_Record.log_out(0, level_disaster, "Bom_Query_OverTrip_Val:query_fare");
		ret.wErrCode = g_Parameter.query_fare(CmdSort::m_time_now, ticket_type, entry_station, CmdSort::m_time_now, &over_trip_var);
		if (ret.wErrCode == 0)
		{
			if (over_trip_var <= balance)
				over_trip_var = 0;
			else
				over_trip_var = over_trip_var - balance;
		}
	}

	len_data = organize_data_region(data_to_send, &ret, sizeof(over_trip_var), &over_trip_var);
}

void Api::bom_elect_ticket_analyze(uint8_t* param_stack, uint8_t* data_to_send, uint16_t& len_data)//BOM传ITP票卡数据的分析
{
	RETINFO ret = { 0 };
	TicketBase* p_ticket = NULL;
	TICKET_PARAM ticket_prm = { 0 };
	TICKET_DATA ticket_data;
	MODE_EFFECT_FLAGS md_effect;
	BOMETICANALYZE eticanalyse;
	uint16_t len_eticanalyse = 0;
	uint8_t use_degrade_flag = 0; //启用降级模式 0 为不启用  其他为启用

	BOMESJTRETURN ReturnSJT = { 0 };
	BOMESVTRETURN ReturnSVT = { 0 };
	uint16_t len_eticket = 0;

	struct timeval start;
	struct timeval end;
	long msec = 0;
	gettimeofday(&start, NULL);

	do
	{
		ret.wErrCode = g_Parameter.device_initialized();
		if (ret.wErrCode != 0)
			break;

		if (TicketBase::check_confirm_his(1440))
		{
			ret.bNoticeCode = NTC_MUST_CONFIRM;
			break;
		}
		//解析数据
		memset(&ticket_data, 0, sizeof(ticket_data));
		memset(&md_effect, 0, sizeof(md_effect));
		memset(&eticanalyse, 0, sizeof(eticanalyse));
		use_degrade_flag = param_stack[0];
		work_area = param_stack[1];	// 1为付费区  2为非付费区

		//开始对票卡操作
		ret.wErrCode = api_match_device(device_type_file);
		if (ret.wErrCode != 0)	break;

		//1，寻卡
		//1.1寻卡前把扫到的二维码缓存起来，用于电子票更新，如果没有就没有
		memcpy(Api::qr_elect_analy_readbuf,Api::qr_readbuf,sizeof(Api::qr_elect_analy_readbuf));
		ret.wErrCode = TicketBase::search_card(&p_ticket, NULL, NULL, NULL, operGetETicket);
		if (ret.wErrCode != 0)	break;
		//2，读卡
		ret = p_ticket->read_card(operAnalyse, ticket_prm);
		if (ret.wErrCode != 0)	break;
		p_ticket->get_ticket_infos(ticket_data);

		//3，校验票卡，对各种参数进行比对校验
		if (use_degrade_flag > 0) {
			p_ticket->get_degrade_mode(md_effect, CmdSort::m_time_now);
		}
		//todo:降级模式处理
		ret.wErrCode = g_Parameter.query_ticket_prm(ticket_data.logical_type, ticket_prm);
		if (ret.wErrCode)	break;
		ret.wErrCode = ParamMgr::device_support_ticket(device_type_file, ticket_prm.usable_device);
		if (ret.wErrCode != 0)	break;

		//4.1 关于电子票版本号为99的特殊处理
		if(memcmp(Api::qr_headversion, "99", 2) == 0){
			break;
		}
		//4.2,开始分析票卡
		if (work_area == 1)	// 付费区
		{
			ret = p_ticket->analyse_work_zone(&md_effect, &ticket_prm, NULL, NULL, &eticanalyse);
		}
		else
		{// 非付费区
			ret = p_ticket->analyse_free_zone(&md_effect, &ticket_prm, NULL, &eticanalyse);
		}

		len_eticanalyse = sizeof(BOMETICANALYZE);
        if(memcmp(ticket_data.qr_ticketdata.subType, "00", 2) == 0){
            len_eticket = sizeof(ReturnSJT);
        }
        else{
            len_eticket = sizeof(ReturnSVT);
        }

	} while (0);

	ret.bRfu[0] = 0x11;
	ret.bRfu[1] = 0x00;
	//版本号为99，为降级二维码，需进行特殊处理
	g_Record.log_out(0,level_normal,"qr_headversion = %02s", Api::qr_headversion);
	if(memcmp(Api::qr_headversion, "99", 2) == 0&&ret.wErrCode == ERR_SUCCEED){
		BOMEOFFLINERETURN offline_return={0};
		len_eticket = sizeof(BOMEOFFLINERETURN);
		g_Record.log_out(0,level_normal,"len_eticket = %d", len_eticket);
		if (len_eticket > 0)	p_ticket->transfer_edata_for_out(operEOffline, &offline_return);
		len_data = organize_data_region(data_to_send, &ret, len_eticket, &offline_return);
	}else{
		//组数据2：分析数据包
		if (len_eticanalyse > 0)    p_ticket->transfer_edata_for_out(operAnalyse, &eticanalyse);
		//组数据1：二维码信息数据
		if(memcmp(ticket_data.qr_ticketdata.subType, "00", 2) == 0)
		{//电子单程票
			if (len_eticket > 0)	p_ticket->transfer_edata_for_out(operESJT, &ReturnSJT);
			len_data = organize_data_region(data_to_send, &ret, len_eticket, &ReturnSJT, len_eticanalyse, &eticanalyse);
		}
		else
		{//电子储值票
			if (len_eticket > 0)    p_ticket->transfer_edata_for_out(operESVT, &ReturnSVT);

			len_data = organize_data_region(data_to_send, &ret, len_eticket, &ReturnSVT, len_eticanalyse, &eticanalyse);
		}
	}

	if (ret.wErrCode != ERR_CARD_NONE)
	{
		memset(Api::qr_readbuf, 0x00, sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
		memset(Api::clear_qr_readbuf, 0x00, sizeof(Api::clear_qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
		memcpy(Api::qr_headversion, "00", 2);//清除唯一维护的版本号
	}

	gettimeofday(&end, NULL);
	msec = TimeCalcUtils::calcMsec(start, end);

    g_Record.log_out(ret.wErrCode, level_error,
		"bom_elect_ticket_analyze(lid=%s,work_area=%d)=(%04X,%02X,%02X) use time=[%ld]",
		ticket_data.logical_id, work_area, ret.wErrCode, ret.bNoticeCode, ret.bRfu[0], msec);
}

void Api::Bom_ETicketAnalyze(uint8_t* param_stack, uint8_t* data_to_send, uint16_t& len_data){
    bom_elect_ticket_analyze(param_stack, data_to_send, len_data);
}



