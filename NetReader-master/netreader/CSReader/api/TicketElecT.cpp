#include "TicketElecT.h"
#include "Errors.h"
#include "Publics.h"
#include "TimesEx.h"
#include "../link/myprintf.h"
#include "../json/json.h"
#include "../ble/ble.h"
#include "TicketsStructs.h"
#include "EnumTypes.h"
#include "Extra.h"
#include <string>

TicketElecT::TicketElecT(void)
{

}

TicketElecT::TicketElecT(char * p_current_sam, char * p_sam_posid, uint8_t * physic_info)
{
	m_p_current_sam = p_current_sam;
	m_p_sam_posid = p_sam_posid;

	memset(&m_ticket_data, 0, sizeof(m_ticket_data));
	memset(&m_ReadInf, 0, sizeof(m_ReadInf));

	sprintf(m_ticket_data.physical_id, "%02X%02X%02X%02X%02X%02X%02X%-6C",
		physic_info[0], physic_info[1], physic_info[2], physic_info[3], physic_info[4], physic_info[5], physic_info[6], 0x20);

	pMemoryPos = ((uint8_t *)this) + sizeof(TicketElecT);
	m_ticket_data.p_entry_info = new(pMemoryPos) TradeInfo;

	m_ticket_data.p_exit_info = &m_ticket_data.read_last_info;
	m_ticket_data.p_update_info = &m_ticket_data.read_last_info;

	memset(&m_ticket_data.read_last_info, 0, sizeof(TradeInfo));

	m_ticket_data.qr_flag = 0x01;
}

TicketElecT::~TicketElecT(void)
{

}

uint16_t TicketElecT::metro_elec_read(P_METRO_ELECT_INF p_mtr_inf)
{
	uint16_t u_ret			= 0;
	//unsigned short nresult		= 0;
	unsigned short response_sw	= 0;
	unsigned char ulen			= 0;
	unsigned char sztmp[256]	= {0};

	do
	{
		//memcpy(p_mtr_inf->p_eTicket_entry_info->cTxnCode,"12",2);

	} while (0);

	return u_ret;
}



uint8_t TicketElecT::unified_status()
{
	uint8_t ret = USTATUS_UNDEFINED;

	switch (m_ticket_data.init_status)
	{
	case MS_Init:
		ret = USTATUS_INIT;
		break;
	case MS_Es:
		ret = USTATUS_ES;
		break;
	case MS_Sale:
		ret = USTATUS_SALE;
		break;
	case MS_Entry:
		ret = USTATUS_ENTRY;
		break;
	case MS_Exit:
		ret = USTATUS_EXIT;
		break;
	case MS_Exit_Only:
		ret = USTATUS_EXIT_ONLY;
		break;
	case MS_Exit_Tt:
		ret = USTATUS_EXIT_T;
		break;
	case MS_Upd_FZone_Free:
		ret = USTATUS_UPD_OUT_FREE;
		break;
	case MS_Upd_FZone_Fare:
		ret = USTATUS_UPD_OUT_FARE;
		break;
	case MS_Upd_Wzone_Entry:
		ret = USTATUS_UPD_WO_STA;
		break;
	case MS_Upd_Wzone_Exit:
		ret = USTATUS_UPD_IN;
		break;
	case MS_Upd_Wzone_Time:
		ret = USTATUS_UPD_TM_OUT;
		break;
	case MS_Upd_Wzone_Trip:
		ret = USTATUS_UPD_TP_OUT;
		break;
	//case MS_Upd_Wzone_Free:
	//	ret = SStatus_Upd_WZone;
	//	break;
	case MS_Refund:
		ret = USTATUS_REFUND;
	}

	return ret;
}

RETINFO TicketElecT::analyse_common(uint8_t wk_area, MODE_EFFECT_FLAGS * p_degrade_falgs,
        TICKET_PARAM * p_ticket_prm, PTICKETLOCK p_lock, uint16_t& lenlock,
        PBOMANALYZE p_analyse, char * p_mode)
{
    RETINFO ret				= {0};
    return ret;
}

RETINFO TicketElecT::analyse_free_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PBOMETICANALYZE p_eticanalyse){
	RETINFO ret = {0};
	uint8_t validDate[4]		= {0};

	do{
		//单程票判断逻辑有效期
		Publics::string_to_bcds(m_ticket_data.qr_ticketdata.validDate, 8, validDate, 4);
		if (memcmp(CmdSort::m_time_now, validDate, 4) > 0){
			g_Record.log_out(0, level_disaster,"analyse_free_zone:ERR_OVER_PERIOD_L3");
			ret.wErrCode = ERR_OVER_PERIOD_L;
			break;
		}

		ret = TicketElectBase::analyse_free_zone(p_degrade_falgs,p_ticket_prm, p_analyse, p_eticanalyse);
	}
	while(0);

	return ret;
}

RETINFO TicketElecT::analyse_work_zone(MODE_EFFECT_FLAGS * p_degrade_falgs,
        TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PPURSETRADE p_purse,
        PBOMETICANALYZE p_eticanalyse)
{
	RETINFO ret				    = {0};
	char exit_mode				= 0;
	uint16_t result_prm			= 0;
	long trade_val				= 0;    //实际交易价格
	long init_val				= 0;    //原有价格
	long penalty_val_overtime			= 0;
	long penalty_val_override			= 0;
	uint8_t validOutTime[7]		= {0};	//进出闸有效时间
	uint8_t validDate[4]		= {0};  //单程票逻辑有效期

	g_Record.log_out(0, level_error,"analyse_work_zone:[m_time_now = %02x%02x%02x%02x%02x%02x%02x]",CmdSort::m_time_now[0],CmdSort::m_time_now[1],CmdSort::m_time_now[2],CmdSort::m_time_now[3],CmdSort::m_time_now[4],CmdSort::m_time_now[5],CmdSort::m_time_now[6]);
    if (p_degrade_falgs->train_trouble)//FIXME: 出闸时需要分析紧急模式
    {
        exit_mode = (char)(degrade_Trouble + 0x30);
        return ret;
    }
    g_Record.log_out(0, level_disaster,"analyse_work_zone:[status = %01x]",m_ticket_data.simple_status);
	//单程票判断逻辑有效期
	Publics::string_to_bcds(m_ticket_data.qr_ticketdata.validDate, 8, validDate, 4);
	if (memcmp(CmdSort::m_time_now, validDate, 4) > 0){
		g_Record.log_out(0, level_disaster,"analyse_work_zone:ERR_OVER_PERIOD_L3");
		ret.wErrCode = ERR_OVER_PERIOD_L;
		return ret;
	}

    switch (m_ticket_data.simple_status){
		//已经进站
        case SStatus_Entry:
			//0,查询车费
            result_prm = g_Parameter.query_fare(CmdSort::m_time_now, m_ticket_data.fare_type,
                                                m_ticket_data.p_entry_info->station_id,
                                                m_ticket_data.p_entry_info->time, &trade_val, &init_val);
            if (result_prm){
                ret.wErrCode = result_prm;
                break;
            }
			g_Record.log_out(0, level_error, "analyse_work_zone:[trade_val = %ld]", trade_val);

            // 1，超时 比较有效出站时间 罚金计算是根据当前站点的计算
            Publics::string_to_bcds(m_ticket_data.qr_ticketdata.validOutTime, 14, validOutTime, 7);
            if(memcmp(CmdSort::m_time_now, validOutTime, 7) > 0)
            {
                g_Parameter.exit_timeout(m_ticket_data.p_entry_info->station_id,
                                         m_ticket_data.p_entry_info->time, CmdSort::m_time_now, &penalty_val_overtime);
                ret.wErrCode = ERR_EXIT_TIMEOUT;
                if (p_eticanalyse != NULL)
                {
                    memcpy(p_eticanalyse->cPenaltyType,"01",2);
					//罚款金额  线网最高票价-车资(实际购买的车票金额)
                    p_eticanalyse->lPenalty1=penalty_val_overtime-((m_ticket_data.wallet_value/10)*10);
				}
				// break;		// 需要检查超时又超乘，所以不能break
            }
			g_Record.log_out(0, level_error, "analyse_work_zone:[wallet_value = %ld]", m_ticket_data.wallet_value);
			g_Record.log_out(0, level_error, "analyse_work_zone:[penalty_val_overtime = %ld]", penalty_val_overtime);


			//2，超乘 比较实际站点的价格与单程票中的价格
            if (m_ticket_data.wallet_value < trade_val)
            {
                penalty_val_override = (trade_val - ((m_ticket_data.wallet_value/10)*10));//超乘罚金，补差额
                ret.wErrCode = ERR_OVER_MILEAGE;
                if (p_eticanalyse != NULL)
                {
                    memcpy(p_eticanalyse->cPenaltyType,"02",2);
                    p_eticanalyse->lPenalty2=penalty_val_override;
                }
				if (penalty_val_override && penalty_val_overtime)
				{
					if(p_eticanalyse!=NULL){
						memcpy(p_eticanalyse->cPenaltyType,"03",2);//既超时又超程
					}
					ret.wErrCode = ERR_OVER_TIME_MILEAGE;
				}
                break;
            }

			if (penalty_val_overtime)
			{//因前面需要判断罚金，超时的时候未能break，故在判断完成之后，进行break
				break;
			}
            //金额的计算，开闸时需要计算金额
            if (p_purse != NULL)
            {
                p_purse->lTradeAmount = trade_val;
                p_purse->cExitMode = exit_mode;
				g_Record.log_out(0, level_disaster, "analyse_work_zone:[trade_val = %ld]", p_purse->lTradeAmount);
			}
            g_Record.log_out(0, level_error,"analyse_work_zone:[penaltyType = %02x]",p_eticanalyse->cPenaltyType);
			break;
        case SStatus_Exit:
		case SStatus_Sale://单程票发售完成  刚买票后就直接在付费区
            ret.wErrCode  = ERR_WITHOUT_ENTRY; //付费区默认无进站
            if (station_is_current(m_ticket_data.p_exit_info->station_id))//如果有出站信息，说明有出站
            {
				if(!TimesEx::timeout(CmdSort::m_time_now, m_ticket_data.p_exit_info->time, 10)){
					ret.wErrCode = ERR_LAST_EXIT_NEAR;//已于10分钟之内出站
				}else{
				    ret.wErrCode = ERR_EXIT_ALREADY;//已出站
				}
            }else{
                if(m_ticket_data.simple_status==SStatus_Exit){
                    ret.wErrCode = ERR_EXIT_OTHER;//已出站
                }
				g_Record.log_out(0, level_warning,"station_is_current others");
			}
            break;
        case SStatus_Invalid:
        default://对于禁止使用以及取票成功的状态，分析时报票卡状态异常
        	ret.wErrCode = ERR_ETICKET_UNKNOEWN_STATUS;
            break;
    }

    if(p_eticanalyse != NULL){
        //zanshi fushu jiaoyan
		if(p_eticanalyse->lPenalty1<0||p_eticanalyse->lPenalty2<0){
            g_Record.log_out(0, level_error,"analyse_work penalty < 0");
            ret.wErrCode = ERR_DEVICE_UNINIT;
		}

    }

    if(Api::API_IS_RELEASE){
        if(m_ticket_data.qr_ticker_isTest=='2'){
            g_Record.log_out(0, level_error, "reader is release but  qr is test ");
            ret.wErrCode=ERR_ETICKET_QR_INVALID;
        }
    }else{
        if(m_ticket_data.qr_ticker_isTest=='1'){
            g_Record.log_out(0, level_error, "reader is test but qr is release");
            ret.wErrCode=ERR_ETICKET_QR_INVALID;
        }
    }

	return ret;
}

uint32_t TicketElecT::add_oper_status(uint8_t wk_area, RETINFO ret, TICKET_PARAM * p_ticket_prm)
{
	uint32_t oper_status = 0;

	if (ret.wErrCode != ERR_OVER_PERIOD_L && ret.wErrCode != ERR_CARD_STATUS &&
		ret.wErrCode != ERR_CARD_REFUND && ret.wErrCode != ERR_OVER_WALLET &&
		ret.wErrCode != ERR_CARD_LOCKED && ret.wErrCode != ERR_WITHOUT_SELL &&
		ret.wErrCode != ERR_OVER_PERIOD_P)
	{
		if (p_ticket_prm->permit_charge && m_ticket_data.wallet_value < m_ticket_data.wallet_value_max)
			oper_status |= ALLOW_CHARGE;
	}
	return oper_status;
}

RETINFO TicketElecT::entry_gate(PENTRYGATE p_entry,PETICKETDEALINFO p_eticketdealinfo,char cOnlineRes)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;
	uint16_t length = 0;

	getDealData(operEntry,length,m_ReadInf.p_eTicket_entry_info,m_ticket_data.qr_ticketdata.price);

	METRO_ELECT_INF write_inf = file_need_write(operEntry, &m_ReadInf);

	m_ticket_data.init_status = MS_Entry;
	m_ticket_data.p_jtb_entry_info = &m_ticket_data.jtb_write_last_info;
	current_jtbtrade_info(*m_ticket_data.p_jtb_entry_info);

	if(cOnlineRes != '0'){
		ret = write_card(operEntry, write_inf.p_eTicket_entry_info, length, 0x09, 0, lSamSeq, NULL);
	}
	else{
		Extra::ble_seq_add(operEntry,m_ticket_data.qr_passcode, write_inf.p_eTicket_entry_info, length);
	}

	transfer_edata_for_out(operEntry, p_eticketdealinfo);

	ret.bRfu[0] = 0x11;
	ret.bRfu[1] = 0x00;

	if ((ret.wErrCode == ERR_BLE_WRITEBACK) || (ret.wErrCode == ERR_BLE_OTHER))
	{
		save_last_stack(operEntry, p_eticketdealinfo, sizeof(ETICKETDEALINFO), write_inf.p_eTicket_entry_info + 2, sizeof(write_inf.p_eTicket_entry_info) - 2, false);
	}

	return ret;
}

RETINFO TicketElecT::exit_gate(PPURSETRADE p_exit,PETICKETDEALINFO p_eticketdealinfo, MODE_EFFECT_FLAGS * p_degrade_falgs,char cOnlineRes)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;
	uint8_t tacType			= 0x09;
	uint16_t length = 0;

	getDealData(operExit,length,m_ReadInf.p_eTicket_exit_info,m_ticket_data.qr_ticketdata.price);

	METRO_ELECT_INF write_inf = file_need_write(operExit, &m_ReadInf);

	m_ticket_data.init_status = MS_Exit;

	m_ticket_data.p_exit_info = &m_ticket_data.write_last_info;
	m_ticket_data.wallet_value -= p_exit->lTradeAmount;

	current_trade_info(*m_ticket_data.p_exit_info);
	//p_exit->lTradeAmount = 1;//测试用
	if(cOnlineRes != '0'){
		ret = write_card(operExit, write_inf.p_eTicket_exit_info, length,tacType, p_exit->lTradeAmount, lSamSeq, p_exit->cMACorTAC);
	}
	else{
		Extra::ble_seq_add(operExit,m_ticket_data.qr_passcode, write_inf.p_eTicket_exit_info, length);
	}

	transfer_edata_for_out(operExit, p_eticketdealinfo);

	ret.bRfu[0] = 0x11;
	ret.bRfu[1] = 0x00;

	if ((ret.wErrCode == ERR_BLE_WRITEBACK) || (ret.wErrCode == ERR_BLE_OTHER))
	{
		save_last_stack(operExit, p_eticketdealinfo, sizeof(ETICKETDEALINFO), write_inf.p_eTicket_exit_info + 2, sizeof(write_inf.p_eTicket_exit_info) - 2, false);
	}

	return ret;
}


RETINFO TicketElecT::esale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, PETICKETDEALINFO p_eticketdealinfo, uint16_t& len_eticket_sale, TICKET_PARAM& ticket_prm)
{
	RETINFO ret				= {0};
	return ret;
}

RETINFO TicketElecT::thirdesale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, PETICKETDEALINFO p_eticketdealinfo, uint16_t& len_eticket_sale, TICKET_PARAM& ticket_prm , char * cNetTranNumber)
{
	RETINFO ret				= {0};
	return ret;
}

RETINFO TicketElecT::sale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, POTHERSALE p_svt_sale, uint16_t& len_svt, TICKET_PARAM& ticket_prm)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;
	len_sjt					= 0;
	len_svt					= 0;

	METRO_ELECT_INF write_inf = file_need_write(operSvtSale, &m_ReadInf);

	m_ticket_data.init_status = MS_Sale;
	current_trade_info(m_ticket_data.write_last_info);

	// 锁定应用
	m_ticket_data.app_lock_flag = 0x01;
	// 有效期其实时间
	memcpy(m_ticket_data.logical_peroidS, CmdSort::m_time_now, sizeof(m_ticket_data.logical_peroidS));

	// 发售
	m_ticket_data.globle_status = EXTERN_STATUS_SALE;
	// 有效期时间
	m_ticket_data.effect_mins = ticket_prm.effect_time_from_sale;
	// 押金
	m_ticket_data.deposit = p_svt_sale->nAmount;
	// 余额上限
	m_ticket_data.wallet_value_max = ticket_prm.balance_max;

	//ret = write_card(operSvtSale, write_inf, 0x09, 0, lSamSeq, NULL);
	p_svt_sale->lSAMTrSeqNo = lSamSeq;
	if (ret.wErrCode == 0)
		len_svt = sizeof(*p_svt_sale);

	p_svt_sale->lBrokerage = ticket_prm.sale_brokerage;
	transfer_jtb_data_for_out(operSvtSale, p_svt_sale);
	p_svt_sale->bStatus = LIFE_SALE;

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
	{
		save_last_stack(operSvtSale, p_svt_sale, sizeof(OTHERSALE), &write_inf, sizeof(write_inf), true);
	}
	return ret;
}


RETINFO TicketElecT::getETicket(PGETETICKETRESPINFO p_geteticketrespinfo)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;
	uint16_t length = 0;
	uint8_t tacType			= 0x09;
	char		cMACorTAC[10] ={0};

	getDealData(operGetETicket,length,m_ReadInf.p_eTicket_get_info,m_ticket_data.qr_ticketdata.price);

	//m_ReadInf.p_eTicket_get_info = (uint8_t *)(ble::ble_getDealJson(operGetETicket,length).c_str());
	//dbg_formatvar("length:%d",length);
	//dbg_dumpmemory("p_eTicket_get_info:",m_ReadInf.p_eTicket_get_info,length);

	METRO_ELECT_INF write_inf = file_need_write(operGetETicket, &m_ReadInf);

	ret = write_card(operGetETicket, write_inf.p_eTicket_get_info, length, tacType,0, lSamSeq, cMACorTAC);

	transfer_edata_for_out(operGetETicket, p_geteticketrespinfo);

	ret.bRfu[0] = 0x11;
	ret.bRfu[1] = 0x00;

	return ret;
}


RETINFO TicketElecT::bom_update(PTICKETUPDATE p_update, uint8_t * p_entry_station)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;
	long fare				= 0;

    //todo: m_ticket_data.qr_ticketdata 根据需求更新
	METRO_ELECT_INF write_inf = file_need_write(operUpdate, &m_ReadInf);
	switch (p_update->bUpdateReasonCode)
	{
	case 0x01:		// 付费区超时
		m_ticket_data.init_status = MS_Upd_Wzone_Time;
		p_update->bStatus = LIFE_UPD_TM_OUT;
		dbg_formatvar("MS_Upd_Wzone_Time");
		break;
	case 0x02:		// 付费区超乘
		m_ticket_data.init_status = MS_Upd_Wzone_Trip;
		p_update->bStatus = LIFE_UPD_TP_OUT;
		dbg_formatvar("MS_Upd_Wzone_Trip");
		break;
	case 0x03:		// 付费区无进站码
		m_ticket_data.p_entry_info->station_id = (p_entry_station[0] << 8) + p_entry_station[1];
		m_ticket_data.init_status = MS_Upd_Wzone_Entry;
		p_update->bStatus = LIFE_UPD_WO_STA;
		dbg_formatvar("MS_Upd_Wzone_Entry");
		ret.wErrCode = g_Parameter.query_fare(p_update->dtUpdateDate,
			m_ticket_data.fare_type, m_ticket_data.p_entry_info->station_id, p_update->dtUpdateDate, &fare);
		if (ret.wErrCode == 0)
		{
			if (fare > m_ticket_data.wallet_value && p_update->nForfeiture < fare - m_ticket_data.wallet_value)
				ret.wErrCode = ERR_INPUT_PARAM;
		}

		break;
	case 0x10:		// 非付费免费更新
		m_ticket_data.init_status = MS_Upd_FZone_Free;
		p_update->bStatus = LIFE_UPD_OUT_FREE;
		break;
	case 0x11:
	case 0x12:		// 非付费区付费更新
		m_ticket_data.init_status = MS_Upd_FZone_Fare;
		p_update->bStatus = LIFE_UPD_OUT_FARE;
		break;
	default:
		ret.wErrCode = ERR_INPUT_PARAM;
		break;
	}

	if (ret.wErrCode == 0)
	{
		m_ticket_data.p_jtb_update_info = &m_ticket_data.jtb_write_last_info;
		current_jtbtrade_info(*m_ticket_data.p_jtb_update_info);

		m_ticket_data.p_update_info = &m_ticket_data.write_last_info;
		current_trade_info(*m_ticket_data.p_update_info);
//		ret = write_card(operUpdate, write_inf.p_eTicket_base_info, 0x09, 0, 0,lSamSeq, NULL);
		p_update->lSAMTrSeqNo = lSamSeq;

		// 交易记录赋值
		transfer_jtb_data_for_out(operUpdate, p_update);

		if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		{
			save_last_stack(operUpdate, p_update, sizeof(TICKETUPDATE), &write_inf, sizeof(write_inf), true);
		}
	}

	return ret;
}

RETINFO TicketElecT::bom_refund(PDIRECTREFUND p_refund)
{
	RETINFO ret				= {0};
	return ret;
}

RETINFO TicketElecT::svt_increase(PPURSETRADE p_purse, uint8_t * p_time, uint8_t * p_mac2,uint8_t paytype)
{
	RETINFO ret					= {0};
	return ret;
}

RETINFO TicketElecT::svt_decrease(PPURSETRADE p_purse, uint8_t u_pay_flag,uint8_t paytype)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;
	uint8_t tacType			= 0x06;

	dbg_formatvar("TicketJtb::svt_decrease");

	if (p_purse->lTradeAmount > m_ticket_data.wallet_value)
	{
		ret.wErrCode = ERR_LACK_WALLET;
	}
	else
	{
		METRO_ELECT_INF write_inf = file_need_write(operDecr, &m_ReadInf);
		m_ticket_data.wallet_value -= p_purse->lTradeAmount;

		//ret = write_card(operDecr, write_inf, tacType, p_purse->lTradeAmount, lSamSeq, p_purse->cMACorTAC);
		p_purse->lSAMTrSeqNo = lSamSeq;

		transfer_data_for_out(operDecr, p_purse);

		if (u_pay_flag == PAY_UPD_PENALTY)
			memcpy(p_purse->cPaymentType, "1A", sizeof(p_purse->cPaymentType));
		else if (u_pay_flag == PAY_INC_DESIND)
			memcpy(p_purse->cPaymentType, "18", sizeof(p_purse->cPaymentType));
		else if (u_pay_flag == PAY_BUY_TOKEN)
			memcpy(p_purse->cPaymentType, "19", sizeof(p_purse->cPaymentType));

		p_purse->bStatus = LIFE_DECREASE;
		Publics::hex_to_two_char(tacType, p_purse->cClassicType);

		if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		{
			save_last_stack(operDecr, p_purse, sizeof(PURSETRADE), &write_inf, sizeof(write_inf), true);
		}
	}

	return ret;
}

METRO_ELECT_INF TicketElecT::file_need_read(ETYTKOPER type, uint8_t * p_buffer)
{
	int ptr_buffer			= 0;
	METRO_ELECT_INF ret_inf	= {0};

	ret_inf.p_eTicket_base_info = p_buffer + ptr_buffer;
	ptr_buffer += LENM_ETICKETBASE;

	switch (type)
	{
	case operEntry:
		ret_inf.p_eTicket_entry_info = p_buffer + ptr_buffer;
		ptr_buffer += LENM_ETICKETENTRY;
		break;
	case operExit:
		ret_inf.p_eTicket_exit_info = p_buffer + ptr_buffer;
		ptr_buffer += LENM_ETICKETEXIT;
		break;
	case operGetETicket:
		ret_inf.p_eTicket_get_info = p_buffer + ptr_buffer;
		ptr_buffer += LENM_ETICKETGET;

		break;

    default:
        break;
	}

	return ret_inf;
}

METRO_ELECT_INF TicketElecT::file_need_write(ETYTKOPER type, P_METRO_ELECT_INF p_read_inf)
{
	METRO_ELECT_INF ret_inf = {0};

	switch (type)
	{
	case operEntry:
		{
			ret_inf.p_eTicket_entry_info = p_read_inf->p_eTicket_entry_info;
		}
		break;
	case operExit:
		{
			ret_inf.p_eTicket_exit_info = p_read_inf->p_eTicket_exit_info;
		}
		break;
	case operGetETicket:
		{
			ret_inf.p_eTicket_get_info = p_read_inf->p_eTicket_get_info;
		}
		break;
	default:
		break;
	}

	return ret_inf;
}

void TicketElecT::get_issue_base(uint8_t * p_issue_base)
{
	if (p_issue_base != NULL)
	{
		// 发卡方代码
		Publics::bcds_to_string(p_issue_base, 2, m_ticket_data.issue_code, 4);

		//dbg_formatvar("issue_code = %s",m_ticket_data.issue_code);

		// 城市代码、行业代码
		Publics::bcds_to_string(p_issue_base + 2, 2, m_ticket_data.city_code, 4);
		Publics::bcds_to_string(p_issue_base + 4, 2, m_ticket_data.industry_code, 4);

		// 应用标识
		m_ticket_data.key_flag = p_issue_base[8];

		//dbg_formatvar("key_flag = %02x",m_ticket_data.key_flag);

		// 逻辑卡号
		sprintf(m_ticket_data.logical_id, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
			p_issue_base[10], p_issue_base[11],p_issue_base[12], p_issue_base[13],p_issue_base[14],
			p_issue_base[15], p_issue_base[16], p_issue_base[17], p_issue_base[18], p_issue_base[19]);

		//dbg_formatvar("logical_id = %s",m_ticket_data.logical_id);

		// 卡类型
		memcpy(m_ticket_data.logical_type, p_issue_base + 28, 2);
		memcpy(m_ticket_data.fare_type, m_ticket_data.logical_type, 2);

		//dbg_formatvar("logical_type = %02x%02x",m_ticket_data.logical_type[0],m_ticket_data.logical_type[1]);

		// 发行日期
		memcpy(m_ticket_data.date_issue, p_issue_base + 20, 4);

		//dbg_dumpmemory("date_issue = ",m_ticket_data.date_issue,4);

		// 物理有效期
		memcpy(m_ticket_data.phy_peroidE, p_issue_base + 24, 4);
		//dbg_dumpmemory("phy_peroidE = ",m_ticket_data.phy_peroidE,4);
	}
}

void TicketElecT::get_public_base(uint8_t * p_public_base)
{
	if (p_public_base != NULL)
	{
		memcpy(p_public_base,"\x00\x01\x00\x01\xFF\xFF\xFF\xFF\x02\x01\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\x20\x18\x06\x15\x20\x20\x06\x15\x11\x00",30);

		// 发卡方代码
		Publics::bcds_to_string(p_public_base, 2, m_ticket_data.issue_code, 4);

		//dbg_formatvar("issue_code = %s",m_ticket_data.issue_code);

		// 城市代码、行业代码
		Publics::bcds_to_string(p_public_base + 2, 2, m_ticket_data.city_code, 4);
		Publics::bcds_to_string(p_public_base + 4, 2, m_ticket_data.industry_code, 4);

		// 应用标识
		m_ticket_data.key_flag = p_public_base[8];

		//dbg_formatvar("key_flag = %02x",m_ticket_data.key_flag);

		// 逻辑卡号
		sprintf(m_ticket_data.logical_id, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
			p_public_base[10], p_public_base[11],p_public_base[12], p_public_base[13],p_public_base[14],
			p_public_base[15], p_public_base[16], p_public_base[17], p_public_base[18], p_public_base[19]);

		//dbg_formatvar("logical_id = %s",m_ticket_data.logical_id);

		// 卡类型
		memcpy(m_ticket_data.logical_type, p_public_base + 28, 2);
		memcpy(m_ticket_data.logical_type, "\x10\x00", 2);//正式的时候需要改成1000
		memcpy(m_ticket_data.fare_type, m_ticket_data.logical_type, 2);

		//dbg_formatvar("logical_type = %02x%02x",m_ticket_data.logical_type[0],m_ticket_data.logical_type[1]);

		// 发行日期
		memcpy(m_ticket_data.date_issue, p_public_base + 20, 4);

		//dbg_dumpmemory("date_issue = ",m_ticket_data.date_issue,4);

		m_ticket_data.globle_status = EXTERN_STATUS_SALE;

		// 物理有效期
		memcpy(m_ticket_data.phy_peroidE, p_public_base + 24, 4);
		//dbg_dumpmemory("phy_peroidE = ",m_ticket_data.phy_peroidE,4);

		memcpy(m_ticket_data.logical_peroidS, p_public_base + 20, 4);

		memcpy(m_ticket_data.logical_peroidE, p_public_base + 24, 4);
	}
}

void TicketElecT::get_owner_base(uint8_t * p_owner_base)
{
	if (p_owner_base != NULL)
	{
		memset(p_owner_base,0x00,55);
		memcpy(m_ticket_data.certificate_name, p_owner_base + 2, sizeof(m_ticket_data.certificate_name));
		memcpy(m_ticket_data.certificate_code, p_owner_base + 22, sizeof(m_ticket_data.certificate_code));
		m_ticket_data.certificate_type = p_owner_base[54];
	}
}

void TicketElecT::get_manage_info(uint8_t * p_manage_info)
{
	if (p_manage_info != NULL)
	{
		memcpy(p_manage_info,"\x00\x00\x01\x56\x00\x00\x41\x00\x00\x01\x11",11);

		memcpy(m_ticket_data.international_code, p_manage_info, sizeof(m_ticket_data.international_code));
		memcpy(m_ticket_data.province_code, p_manage_info + 4, sizeof(m_ticket_data.province_code));
		memcpy(m_ticket_data.city_code_17, p_manage_info + 6, sizeof(m_ticket_data.city_code_17));
		memcpy(m_ticket_data.interCardFlag, p_manage_info + 8, sizeof(m_ticket_data.interCardFlag));
		m_ticket_data.jtbCardType = p_manage_info[10];

		//dbg_dumpmemory("international_code = ",m_ticket_data.international_code,4);
		//dbg_dumpmemory("province_code = ",m_ticket_data.province_code,2);
		//dbg_dumpmemory("city_code_17 = ",m_ticket_data.city_code_17,2);
		//dbg_dumpmemory("interCardFlag = ",m_ticket_data.interCardFlag,2);
		//dbg_formatvar("jtbCardType = %02x",m_ticket_data.jtbCardType);
	}
}

void TicketElecT::get_wallet(uint8_t * p_wallet)
{
	if (p_wallet != NULL)
	{
		m_ticket_data.wallet_value = 1000;
	}
}

void TicketElecT::get_his_all(uint8_t * p_his_all)
{
	if (p_his_all != NULL)
	{

	}
}

void TicketElecT::get_trade_assist(uint8_t * p_trade_assist)
{
	if (p_trade_assist != NULL)
	{
		m_ticket_data.list_falg = p_trade_assist[2];
	}
}

void TicketElecT::get_metro(uint8_t * p_metro)
{
	if (p_metro != NULL)
	{
	}
}

void TicketElecT::get_ctrl_record(uint8_t * p_ctrl_record)
{
	if (p_ctrl_record != NULL)
	{

		m_ticket_data.app_lock_flag = 0x01;
		memcpy(m_ticket_data.logical_peroidS, "\x20\x18\x06\x15\x10\x47\x00", 7);
		memcpy(m_ticket_data.logical_peroidE, "\x20\x20\x06\x15\x10\x47\x00", 7);
	}
}

void TicketElecT::get_app_ctrl(uint8_t * p_app_ctrl)
{
	// 由于涉及有效期，调用本函数必须在调用get_ctrl_record后面
	if (p_app_ctrl != NULL)
	{
	}
}

void TicketElecT::get_change_count(uint8_t * p_charge_count)
{
	if (p_charge_count != NULL)
	{
		m_ticket_data.charge_counter = 0;
	}
}

void TicketElecT::get_consume_count(uint8_t * p_consume_count)
{
	if (p_consume_count != NULL)
	{
		m_ticket_data.total_trade_counter = 0;
	}
}

RETINFO TicketElecT::write_card_back(ETYTKOPER operType, uint8_t * p_eTicket_info, uint16_t length)
{
    RETINFO ret			= {0};
    uint8_t u_recv[64]	= {0};
    char sz_tac[11]		= {0};
    int result = -1;

    do
    {

        //ble::ble_handshake();

        result = ble::ble_all_oper(operType,Api::qr_passcode, p_eTicket_info, length);
        if(result != 0)
        {
            if (result == -3)
            {
                g_Record.log_out(0, level_error,"============================if ble_writeBack match ble failed==================================");
                ret.wErrCode = ERR_BLE_ADDRESS_MATCH;
                break;
            }
            g_Record.log_out(0, level_error,"============================if ble_writeBack faile ble close==================================");
            //ble::ble_close();
            ret.wErrCode = ERR_BLE_WRITEBACK;
            break;
        }


    } while (0);

    return ret;
}

RETINFO TicketElecT::read_card(ETYTKOPER operType, TICKET_PARAM& ticket_prm, uint8_t * p_read_init/* = NULL*/)
{
    RETINFO ret				= {0};
    uint8_t	eticket_info[512]  = {0};

    do
    {
        memset(&m_u_buffer, 0, sizeof(m_u_buffer));
        m_ReadInf = file_need_read(operType, m_u_buffer);

        memcpy(m_ticket_data.qr_passcode, Api::qr_readbuf + 10 + 7, 32);
        memcpy(Api::qr_passcode, m_ticket_data.qr_passcode, 32);

        //首先解密二维码转成明文
        //校验票卡合法性
        //解析电子票卡明文数据
        //解析电子票卡明文数据
        memcpy(eticket_info, Api::clear_qr_readbuf/* + 42*/, 512);
        get_qrInfo(eticket_info);
        // 逻辑卡号
        memcpy(m_ticket_data.logical_id, m_ticket_data.qr_ticketdata.cardNo, 20);
        // 钱包余额
        // m_ticket_data.wallet_value = m_ticket_data.qr_ticketdata.balance;
    } while (0);

    return ret;
}

uint16_t TicketElecT::set_trade_assist(uint8_t * p_trade_assist)
{
	uint16_t ret = 0;
	unsigned char u_recv[512] = {0};

	if (p_trade_assist != NULL)
	{

	}

	return ret;
}

uint16_t TicketElecT::set_metro(uint8_t tac_type, uint8_t * p_metro, uint32_t lValue, uint8_t * p_respond)
{
	if (p_metro != NULL)
	{

	}


	return 0;
}

uint16_t TicketElecT::set_ctrl_record(uint8_t * p_ctrl_record)
{
	if (p_ctrl_record != NULL)
	{
	}

	return 0;
}

uint16_t TicketElecT::set_app_ctrl(uint8_t * p_app_ctrl)
{
	uint16_t wallet_max = 0;

	if (p_app_ctrl != NULL)
	{
	}

	return 0;
}

void TicketElecT::get_qrInfo(uint8_t * p_grInfo)
{
	uint8_t temp[4]		= {0};
	uint8_t temp_entry[4]		= {0};
	int len_getData=0;  //用于记录解析二维码的当前位置
    getQRHeadInfo(p_grInfo);
    getQRDataComInfo(p_grInfo);

	len_getData+=29;
    //余额
	Publics::string_to_hexs((char *)(p_grInfo + len_getData), 8, temp, 4);
	len_getData+=8;
	m_ticket_data.qr_ticketdata.balance = temp[0];
	m_ticket_data.qr_ticketdata.balance <<= 8;
	m_ticket_data.qr_ticketdata.balance += temp[1];
	m_ticket_data.qr_ticketdata.balance <<= 8;
	m_ticket_data.qr_ticketdata.balance += temp[2];
	m_ticket_data.qr_ticketdata.balance <<= 8;
	m_ticket_data.qr_ticketdata.balance += temp[3];
	g_Record.log_out(0, level_error,"get_qrInfo(balance=%ld)", m_ticket_data.qr_ticketdata.balance);
    //计数器
	Publics::string_to_hexs((char *)(p_grInfo + len_getData), 8, temp, 4);
	len_getData+=8;
	m_ticket_data.qr_ticketdata.counter = temp[0];
	m_ticket_data.qr_ticketdata.counter <<= 8;
	m_ticket_data.qr_ticketdata.counter += temp[1];
	m_ticket_data.qr_ticketdata.counter <<= 8;
	m_ticket_data.qr_ticketdata.counter += temp[2];
	m_ticket_data.qr_ticketdata.counter <<= 8;
	m_ticket_data.qr_ticketdata.counter += temp[3];
    g_Record.log_out(0, level_error,"get_qrInfo(counter=%ld)", m_ticket_data.qr_ticketdata.counter);
    //票号
	memcpy(m_ticket_data.qr_ticketdata.centerCode, p_grInfo + len_getData, 32);
	len_getData+=32;
	g_Record.log_out(0, level_error,"get_qrInfo(centerCode=%s)", m_ticket_data.qr_ticketdata.centerCode);
	//中心校验码
	memcpy(m_ticket_data.qr_ticketdata.mac, p_grInfo + len_getData, 8);
	len_getData+=8;
	memcpy(m_ticket_data.qr_ticketdata.saleTime, p_grInfo + len_getData, 14);
	len_getData+=14;
	memcpy(m_ticket_data.qr_ticketdata.validDate, p_grInfo + len_getData, 8);
	len_getData+=8;

	//闸机只用前7位当前时间+加上卡号
	memcpy(m_ticket_data.timeAndcenterCode, CmdSort::m_time_now, 7);
	memcpy(m_ticket_data.timeAndcenterCode + 7, m_ticket_data.qr_ticketdata.cardNo, 20);

	if(m_ticket_data.qr_ticket_status == STATUS_FINISH_SALE)//售卡完成状态，代表后面结构是进站或者取票
	{

		memcpy(m_ticket_data.qr_ticketdata.startStation, p_grInfo + len_getData, 4);
		len_getData+=4;
		memcpy(m_ticket_data.qr_ticketdata.terminalStation, p_grInfo + len_getData, 4);
		len_getData+=4;
		g_Record.log_out(0, level_error,"get_qrInfo(startStation=%s)", m_ticket_data.qr_ticketdata.startStation);
		g_Record.log_out(0, level_error,"get_qrInfo(terminalStation=%s)", m_ticket_data.qr_ticketdata.terminalStation);
        //单价
		Publics::string_to_hexs((char *)(p_grInfo + len_getData), 8, temp, 4);
		len_getData+=8;
		m_ticket_data.qr_ticketdata.price = temp[0];
		m_ticket_data.qr_ticketdata.price <<= 8;
		m_ticket_data.qr_ticketdata.price += temp[1];
		m_ticket_data.qr_ticketdata.price <<= 8;
		m_ticket_data.qr_ticketdata.price += temp[2];
		m_ticket_data.qr_ticketdata.price <<= 8;
		m_ticket_data.qr_ticketdata.price += temp[3];
		g_Record.log_out(0, level_error,"get_qrInfo(price=%ld)", m_ticket_data.qr_ticketdata.price);
        //张数
		memcpy(m_ticket_data.qr_ticketdata.amount, p_grInfo + len_getData, 2);
		len_getData+=2;
		g_Record.log_out(0, level_error,"get_qrInfo(amount  counts=%s)", m_ticket_data.qr_ticketdata.amount);
        //总价
		Publics::string_to_hexs((char *)(p_grInfo + len_getData), 8, temp, 4);
		len_getData+=8;
		//FIXME:解析金额总金额的时候报错了，读写器直接奔溃，故暂时注释掉
		//m_ticket_data.qr_ticketdata.sum = temp[0];
		// m_ticket_data.qr_ticketdata.sum <<= 8;
		// m_ticket_data.qr_ticketdata.sum += temp[1];
		// m_ticket_data.qr_ticketdata.sum <<= 8;
		// m_ticket_data.qr_ticketdata.sum += temp[2];
		// m_ticket_data.qr_ticketdata.sum <<= 8;
		// m_ticket_data.qr_ticketdata.sum += temp[3];
       	// g_Record.log_out(0, level_error,"get_qrInfo(sum price=%s)", m_ticket_data.qr_ticketdata.sum);
		//验证码
		memcpy(m_ticket_data.qr_ticketdata.vervifyCode, p_grInfo + len_getData, 8);
		len_getData+=8;
		//////////////////////类似于单程票来处理结构///////////////////////////
		Publics::string_to_bcds(m_ticket_data.qr_ticketdata.startStation, 4, temp, 2);
		m_ticket_data.p_entry_info->station_id = (temp[0] << 8) + temp[1];
		m_ticket_data.wallet_value = m_ticket_data.qr_ticketdata.price;

		m_ticket_data.simple_status = SStatus_Sale;

	}else if(m_ticket_data.qr_ticket_status == STATUS_ENTRY){
		memcpy(m_ticket_data.qr_ticketdata.startStation, p_grInfo + len_getData, 4);
		len_getData+=4;
		memcpy(m_ticket_data.qr_ticketdata.entryTime, p_grInfo + len_getData, 14);
		len_getData+=14;
		memcpy(m_ticket_data.qr_ticketdata.terminalStation, p_grInfo + len_getData, 4);
		len_getData+=4;

		g_Record.log_out(0, level_error,"get_qrInfo(startStation=%s)", m_ticket_data.qr_ticketdata.startStation);
		g_Record.log_out(0, level_error,"get_qrInfo(entryTime=%s)", m_ticket_data.qr_ticketdata.entryTime);
		g_Record.log_out(0, level_error,"get_qrInfo(terminalStation=%s)", m_ticket_data.qr_ticketdata.terminalStation);
        //票价
		Publics::string_to_hexs((char *)(p_grInfo + len_getData), 8, temp, 4);
		len_getData+=8;
		m_ticket_data.qr_ticketdata.price = temp[0];
		m_ticket_data.qr_ticketdata.price <<= 8;
		m_ticket_data.qr_ticketdata.price += temp[1];
		m_ticket_data.qr_ticketdata.price <<= 8;
		m_ticket_data.qr_ticketdata.price += temp[2];
		m_ticket_data.qr_ticketdata.price <<= 8;
		m_ticket_data.qr_ticketdata.price += temp[3];
		g_Record.log_out(0, level_error,"get_qrInfo(price=%ld)", m_ticket_data.qr_ticketdata.price);
        //验证码
		memcpy(m_ticket_data.qr_ticketdata.vervifyCode, p_grInfo + len_getData, 8);
		len_getData+=8;
		//有效出站时间
		memcpy(m_ticket_data.qr_ticketdata.validOutTime, p_grInfo + len_getData, 14);
		len_getData+=14;
		g_Record.log_out(0, level_error,"get_qrInfo(validOutTime=%s)", m_ticket_data.qr_ticketdata.validOutTime);

		//////////////////////类似于单程票来处理结构///////////////////////////
		Publics::string_to_bcds(m_ticket_data.qr_ticketdata.startStation, 4, temp, 2);
		m_ticket_data.p_entry_info->station_id = (temp[0] << 8) + temp[1];
		m_ticket_data.read_last_info.station_id = (temp[0] << 8) + temp[1];

		Publics::string_to_bcds(m_ticket_data.qr_ticketdata.entryTime, 14, m_ticket_data.read_last_info.time, 7);
		Publics::string_to_bcds(m_ticket_data.qr_ticketdata.entryTime, 14, m_ticket_data.p_entry_info->time, 7);

		m_ticket_data.wallet_value = m_ticket_data.qr_ticketdata.price;
		m_ticket_data.qr_ticketdata.TxnAmount = m_ticket_data.qr_ticketdata.balance;
		m_ticket_data.simple_status = SStatus_Entry;
	}else if(m_ticket_data.qr_ticket_status == STATUS_EXIT){
		//进站信息
		memcpy(m_ticket_data.qr_ticketdata.startStation, p_grInfo + len_getData, 4);
		len_getData+=4;
		memcpy(m_ticket_data.qr_ticketdata.entryTime, p_grInfo + len_getData, 14);
		len_getData+=14;
		//出站信息
		memcpy(m_ticket_data.qr_ticketdata.exitStation, p_grInfo + len_getData, 4);
		len_getData+=4;
		memcpy(m_ticket_data.qr_ticketdata.exitTime, p_grInfo + len_getData, 14);
		len_getData+=14;
		//票价
		Publics::string_to_hexs((char *)(p_grInfo + len_getData), 8, temp, 4);
		len_getData+=4;
		m_ticket_data.qr_ticketdata.price = temp[0];
		m_ticket_data.qr_ticketdata.price <<= 8;
		m_ticket_data.qr_ticketdata.price += temp[1];
		m_ticket_data.qr_ticketdata.price <<= 8;
		m_ticket_data.qr_ticketdata.price += temp[2];
		m_ticket_data.qr_ticketdata.price <<= 8;
		m_ticket_data.qr_ticketdata.price += temp[3];
		//验证码
		memcpy(m_ticket_data.qr_ticketdata.vervifyCode, p_grInfo + len_getData, 8);
		len_getData+=8;

		//1.1进站转换
        Publics::string_to_bcds(m_ticket_data.qr_ticketdata.startStation, 4, temp_entry, 2);
        m_ticket_data.p_exit_info->station_id = (temp_entry[0] << 8) + temp_entry[1];
        Publics::string_to_bcds(m_ticket_data.qr_ticketdata.entryTime, 14, m_ticket_data.p_entry_info->time, 7);
        //2.2出站转换
	    Publics::string_to_bcds(m_ticket_data.qr_ticketdata.exitStation, 4, temp, 2);
        m_ticket_data.p_exit_info->station_id = (temp[0] << 8) + temp[1];
        Publics::string_to_bcds(m_ticket_data.qr_ticketdata.exitTime, 14, m_ticket_data.p_exit_info->time, 7);
		g_Record.log_out(0, level_error,"get_qrInfo(price=%ld)", m_ticket_data.qr_ticketdata.price);
		g_Record.log_out(0, level_error,"get_qrInfo(entryTime=%s)", m_ticket_data.qr_ticketdata.entryTime);
		g_Record.log_out(0, level_error,"get_qrInfo(entryStation=%s)", m_ticket_data.qr_ticketdata.startStation);
		g_Record.log_out(0, level_error,"get_qrInfo(exitTime=%s)", m_ticket_data.qr_ticketdata.exitTime);
		g_Record.log_out(0, level_error,"get_qrInfo(exitStation=%s)", m_ticket_data.qr_ticketdata.exitStation);
		m_ticket_data.simple_status = SStatus_Exit;
	}else{
        m_ticket_data.simple_status = SStatus_Invalid;//对未知的电子票状态进行处理
	}

	memcpy(m_ticket_data.logical_type, "\x01\x00", 2);//单程票
	memcpy(m_ticket_data.fare_type, m_ticket_data.logical_type, 2);
}

void TicketElecT::get_bleInfo(uint8_t * p_bleInfo)
{
	Json::Reader reader;
	Json::Value json_object;
	Json::Value json_object1;

	dbg_formatvar("create json");

	if (!reader.parse((char *)p_bleInfo, json_object))//字符串inbuf，json格式化
	{
		dbg_formatvar("json_object error!");
	}
	else
	{

		dbg_formatvar("json_object sucss");
		string jsonEncMode=json_object["encMode"].asString();
		dbg_formatvar("jsonEncMode=%s",jsonEncMode.c_str());
		string jsonLength=json_object["length"].asString();
		dbg_formatvar("length=%s",jsonLength.c_str());
		string jsonResult=json_object["result"].asString();
		dbg_formatvar("result=%s",jsonResult.c_str());
		string jsonType=json_object["type"].asString();
		dbg_formatvar("type=%s",jsonType.c_str());
		string jsonVersion=json_object["version"].asString();
		dbg_formatvar("version=%s",jsonVersion.c_str());


		json_object1 = json_object["ticket"];
		dbg_formatvar("ticket");

		m_ticket_data.ble_ticketdata.balance=(long)json_object1["balance"].asUInt();
		dbg_formatvar("balance=%ld",m_ticket_data.ble_ticketdata.balance);
		dbg_formatvar("cardNo=%s",json_object1["cardNo"].asString().c_str());

		strcpy(m_ticket_data.ble_ticketdata.cardNo,json_object1["cardNo"].asString().c_str());
		//memcpy(&m_ticket_data.ble_ticketdata.cardNo[0], (char *)(json_object1["cardNo"].asString().c_str()), 20);
		dbg_formatvar("cardNo=%s",m_ticket_data.ble_ticketdata.cardNo);

		m_ticket_data.ble_ticketdata.subType=(int)json_object1["subType"].asInt();
		dbg_formatvar("subType=%d",m_ticket_data.ble_ticketdata.subType);

		m_ticket_data.ble_ticketdata.mainType=(int)json_object1["mainType"].asInt();
		dbg_formatvar("mainType=%d",m_ticket_data.ble_ticketdata.mainType);

		m_ticket_data.ble_ticketdata.payAmount=(long)json_object1["payAmount"].asUInt();
		dbg_formatvar("payAmount=%ld",m_ticket_data.ble_ticketdata.payAmount);

		memcpy(&m_ticket_data.ble_ticketdata.terminalStation, json_object1["terminalStation"].asString().c_str(), 4);
		dbg_formatvar("terminalStation=%s",m_ticket_data.ble_ticketdata.terminalStation);

		memcpy(&m_ticket_data.ble_ticketdata.startStation, json_object1["startStation"].asString().c_str(), 4);
		dbg_formatvar("startStation=%s",m_ticket_data.ble_ticketdata.startStation);

		memcpy(m_ticket_data.logical_type, "\x10\x00", 2);//先当成1000的票卡获取票卡参数
		memcpy(m_ticket_data.fare_type, m_ticket_data.logical_type, 2);

    }


	Json::FastWriter fast_writer;
	string jsonRoot = fast_writer.write(json_object);
	dbg_formatvar("jsonRoot=%s",jsonRoot.c_str());

}

// 将状态格式化为简化状态
SimpleStatus TicketElecT::get_simple_status(uint8_t init_status)
{
	SimpleStatus ret = SStatus_Invalid;

	if (m_ticket_data.globle_status == EXTERN_STATUS_SALE)
	{
		switch (init_status)
		{
		case MS_Init:
			ret = SStatus_Init;
			break;
		case MS_Es:
			ret = SStatus_Es;
			break;
		case MS_Sale:
			ret = SStatus_Sale;
			break;
		case MS_Entry:
			ret = SStatus_Entry;
			break;
		case MS_Exit:
			ret = SStatus_Exit;
			break;
		case MS_Exit_Tt:
			ret = SStatus_Exit_Tt;
			break;
		case MS_Upd_FZone_Free:
		case MS_Upd_FZone_Fare:
			ret = SStatus_Upd_FZone;
			break;
		case MS_Upd_Wzone_Entry:
		case MS_Upd_Wzone_Exit:
		case MS_Upd_Wzone_Time:
		case MS_Upd_Wzone_Trip:
		case MS_Upd_Wzone_Free:
			ret = SStatus_Upd_WZone;
			break;
		case MS_Refund:
			ret = SStatus_Refund;
			break;
		}
	}
	else if (m_ticket_data.globle_status == EXTERN_STATUS_INIT)
	{
		ret = SStatus_Init;
	}
	else if (m_ticket_data.globle_status == EXTERN_STATUS_RFND)
	{
		ret = SStatus_Refund;
	}

	return ret;
}

RETINFO TicketElecT::format_history(uint8_t&	his_count, PHSSVT p_his_array, int his_max)
{
	RETINFO ret	= {0};
	HSSVT his_all[30];
	HSSVT his_temp;

	his_count = 0;
	memset(his_all, 0, sizeof(his_all));

	if (m_ReadInf.p_his_all != NULL)
	{
		for (int i=0;i<30;i++)
		{
			if (format_history(m_ReadInf.p_his_all + 23 * i, his_all[his_count]))
				his_count++;
		}

		for (int i=0;i<his_count;i++)
		{
			for (int j=i+1;j<his_count;j++)
			{
				if (memcmp(his_all[i].dtDate, his_all[j].dtDate, 7) < 0)
				{
					memcpy(&his_temp, his_all + i, sizeof(HSSVT));
					memcpy(his_all + i, his_all + j, sizeof(HSSVT));
					memcpy(his_all + j, &his_temp, sizeof(HSSVT));
				}
			}
		}

		if (his_count > his_max)
			his_count = his_max;

		memcpy(p_his_array, his_all, sizeof(HSSVT) * his_count);
	}
	return ret;
}

bool TicketElecT::format_history(uint8_t * p_his_buffer, HSSVT& his)
{
	char szSam[32]		= {0};
	char szTerminal[32] = {0};

	// 交易时间（BCD码)
	memcpy(his.dtDate, p_his_buffer + 16, sizeof(his.dtDate));

	// 票卡生命周期索引,具体定义见附录五
	if (p_his_buffer[9] == 0x02)
		his.bStatus = LIFE_CHARGE;
	else if (p_his_buffer[9] == 0x06)
		his.bStatus = LIFE_CONSUME_COMMON;
	else
		his.bStatus = LIFE_CONSUME_COMPOUND;

	// 交易金额,单位分
	his.lTradeAmount = (p_his_buffer[5] << 24) + (p_his_buffer[6] << 16) + (p_his_buffer[7] << 8) + p_his_buffer[8];

	// sam终端id
	sprintf(szTerminal, "0000%02X%02X%02X%02X%02X%02X",
		p_his_buffer[10], p_his_buffer[11], p_his_buffer[12], p_his_buffer[13], p_his_buffer[14], p_his_buffer[15]);
	memcpy(his.cSAMID, szTerminal, sizeof(his.cSAMID));

	// sam卡号
	sprintf(szSam, "%02X%02X0003%02X%02X%02X%02X",
		p_his_buffer[10], p_his_buffer[11], p_his_buffer[12], p_his_buffer[13], p_his_buffer[14], p_his_buffer[15]);

	g_Parameter.device_from_sam(szSam, SAM_SOCK_1, his.bStationID, &his.bDeviceType, his.bDeviceID);

	return TimesEx::bcd_time_valid(his.dtDate, T_TIME);
}

RETINFO TicketElecT::lock(PTICKETLOCK p_lock)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;

	METRO_ELECT_INF write_inf = file_need_write(operLock, &m_ReadInf);

	m_ticket_data.list_falg = '2'- p_lock->cLockFlag;

	if (m_ticket_data.list_falg == 0)
	{
		//ret.wErrCode = write_card(0x06, NULL, 0x01, m_ticket_data.list_falg, write_inf.p_metro, lSamSeq);
	}
	else
	{
		//ret = write_card(operLock, write_inf, 0x09, 0, lSamSeq, NULL);
	}

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		ret.bNoticeCode = 0;

	p_lock->lSAMTrSeqNo = lSamSeq;

	transfer_jtb_data_for_out(operLock, p_lock);
	if (m_ticket_data.list_falg == 1)
		p_lock->bStatus = LIFE_LOCKED;
	else
		p_lock->bStatus = LIFE_UNLOCKED;

	return ret;
}

RETINFO TicketElecT::deffer(PTICKETDEFER p_deffer, TICKET_PARAM& ticket_prm)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;

	METRO_ELECT_INF write_inf = file_need_write(operDeffer, &m_ReadInf);

	memcpy(m_ticket_data.logical_peroidS, CmdSort::m_time_now, 7);
	m_ticket_data.effect_mins = ticket_prm.deffer_days * 24 * 60;

	if (write_inf.p_ctrl_record != NULL)
		memcpy(write_inf.p_ctrl_record + 4, m_ticket_data.logical_peroidS, 7);

	//ret.wErrCode = write_card(0x06, write_inf.p_app_ctrl, 0x11, write_inf.p_ctrl_record[2], write_inf.p_ctrl_record, lSamSeq);
	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		ret.bNoticeCode = 0;

	p_deffer->lSAMTrSeqNo = lSamSeq;

	transfer_jtb_data_for_out(operDeffer, p_deffer);
	p_deffer->bStatus = LIFE_DEFFER;

	return ret;
}

RETINFO TicketElecT::set_complex_file(ETYTKOPER operType,uint8_t tac_type, long lAmount,uint8_t * p_metro,uint8_t * p_recv)
{
	int pos_file_buf			= 0;
	unsigned char u_count		= 0;
	unsigned char u_rid[8]		= {0};
	unsigned char u_lock[8]		= {0};
	unsigned char u_send[512]	= {0};
	RETINFO ret					= {0};
	uint8_t his_record[48]       = {0};
	long label_val				= 0;

	dbg_formatvar("TicketJtb::set_complex_file - 1");

	if (p_metro != NULL)
	{
		dbg_formatvar("TicketJtb::set_complex_file - 2");

		switch (operType)
		{
			case operUpdate:
				{
					if((m_ticket_data.init_status == MS_Upd_Wzone_Time)||(m_ticket_data.init_status == MS_Upd_Wzone_Trip))
					{
						p_metro[14] = 0x04;//交易状态为出站更新;
						p_metro[43] = (uint8_t)(Api::current_station_id >> 8);
						p_metro[44] = (uint8_t)(Api::current_station_id & 0xFF);
					}
					else if((m_ticket_data.init_status == MS_Upd_FZone_Free)||(m_ticket_data.init_status == MS_Upd_FZone_Fare))
					{
						p_metro[14] = 0x03;//交易状态为进站更新;
					}
					else if((m_ticket_data.init_status == MS_Upd_Wzone_Entry))
					{
						p_metro[14] = 0x04;//交易状态为出站更新;
						p_metro[35] = (uint8_t)(m_ticket_data.p_entry_info->station_id >> 8);
						p_metro[36] = (uint8_t)(m_ticket_data.p_entry_info->station_id & 0xFF);
					}
					p_metro[81] = ((label_val >> 24) & 0xFF);
					p_metro[82] = ((label_val >> 16) & 0xFF);
					p_metro[83] = ((label_val >> 8) & 0xFF);
					p_metro[84] = (label_val & 0xFF);
				}
				break;
			case operEntry:
				{
					p_metro[14] = 0x01;//交易状态为进闸;
					Publics::string_to_bcds(m_ticket_data.p_jtb_entry_info->city_code, 4, &p_metro[15], 2);
					memcpy(p_metro + 19, m_ticket_data.p_jtb_entry_info->institutionCode, 8);
					p_metro[35] = (uint8_t)(m_ticket_data.p_jtb_entry_info->station_id >> 8);
					p_metro[36] = (uint8_t)(m_ticket_data.p_jtb_entry_info->station_id & 0xFF);

					//Publics::string_to_bcds(m_ticket_data.p_jtb_entry_info->sam, 16, &p_metro[51], 8);
					memcpy(p_metro + 67, m_ticket_data.p_jtb_entry_info->time, sizeof(m_ticket_data.p_jtb_entry_info->time));

					//获取本地最低票价
					g_Parameter.query_lowest_fare(CmdSort::m_time_now, m_ticket_data.logical_type, label_val);
					//dbg_formatvar("label_val |= %d",label_val);
					p_metro[81] = ((label_val >> 24) & 0xFF);
					p_metro[82] = ((label_val >> 16) & 0xFF);
					p_metro[83] = ((label_val >> 8) & 0xFF);
					p_metro[84] = (label_val & 0xFF);

					p_metro[85] = m_ticket_data.p_jtb_entry_info->line_id;
					p_metro[87] = ((lAmount >> 24) & 0xFF);
					p_metro[88] = ((lAmount >> 16) & 0xFF);
					p_metro[89] = ((lAmount >> 8) & 0xFF);
					p_metro[90] = (lAmount & 0xFF);
					p_metro[91] = ((m_ticket_data.wallet_value >> 24) & 0xFF);
					p_metro[92] = ((m_ticket_data.wallet_value >> 16) & 0xFF);
					p_metro[93] = ((m_ticket_data.wallet_value >> 8) & 0xFF);
					p_metro[94] = (m_ticket_data.wallet_value & 0xFF);
				}
				break;
			case operExit:
				{
					p_metro[14] = 0x02;//交易状态为出闸;
					Publics::string_to_bcds(m_ticket_data.p_jtb_exit_info->city_code, 4, &p_metro[17], 2);
					memcpy(p_metro + 27, m_ticket_data.p_jtb_exit_info->institutionCode, 8);
					p_metro[43] = (uint8_t)(m_ticket_data.p_jtb_exit_info->station_id >> 8);
					p_metro[44] = (uint8_t)(m_ticket_data.p_jtb_exit_info->station_id & 0xFF);

					//Publics::string_to_bcds(m_ticket_data.p_jtb_exit_info->sam, 16, &p_metro[59], 8);

					memcpy(p_metro + 74, m_ticket_data.p_jtb_exit_info->time, sizeof(m_ticket_data.p_jtb_exit_info->time));

					p_metro[81] = ((label_val >> 24) & 0xFF);
					p_metro[82] = ((label_val >> 16) & 0xFF);
					p_metro[83] = ((label_val >> 8) & 0xFF);
					p_metro[84] = (label_val & 0xFF);

					p_metro[86] = m_ticket_data.p_jtb_exit_info->line_id;
					p_metro[95] = ((lAmount >> 24) & 0xFF);
					p_metro[96] = ((lAmount >> 16) & 0xFF);
					p_metro[97] = ((lAmount >> 8) & 0xFF);
					p_metro[98] = (lAmount & 0xFF);
				}
				break;
		}
	}

	dbg_formatvar("TicketJtb::set_complex_file - 3");

	if(p_metro != NULL)
	{

		dbg_formatvar("TicketJtb::set_complex_file - 4");

		if (lAmount == 0)
		{
			his_record[0] = 0x03;
			Publics::string_to_bcds(m_ticket_data.p_jtb_entry_info->sam, 16, &his_record[1], 8);
			his_record[9] = 0x01;//行业代码，01-地铁
			his_record[10] = (uint8_t)(m_ticket_data.p_jtb_entry_info->station_id >> 8);
			his_record[12] = (uint8_t)(m_ticket_data.p_jtb_entry_info->station_id & 0xFF);

			his_record[17] = ((lAmount >> 24) & 0xFF);
			his_record[18] = ((lAmount >> 16) & 0xFF);
			his_record[19] = ((lAmount >> 8) & 0xFF);
			his_record[20] = (lAmount & 0xFF);

			his_record[21] = ((m_ticket_data.wallet_value >> 24) & 0xFF);
			his_record[22] = ((m_ticket_data.wallet_value >> 16) & 0xFF);
			his_record[23] = ((m_ticket_data.wallet_value >> 8) & 0xFF);
			his_record[24] = (m_ticket_data.wallet_value & 0xFF);

			memcpy(his_record + 25, m_ticket_data.p_jtb_entry_info->time, sizeof(m_ticket_data.p_jtb_entry_info->time));

			Publics::string_to_bcds(m_ticket_data.p_jtb_entry_info->city_code, 4, &his_record[32], 2);
			memcpy(his_record + 34, m_ticket_data.p_jtb_entry_info->institutionCode, 8);
		}else{
			his_record[0] = 0x04;
			Publics::string_to_bcds(m_ticket_data.p_jtb_exit_info->sam, 16, &his_record[1], 8);
			his_record[9] = 0x01;//行业代码，01-地铁
			his_record[10] = (uint8_t)(m_ticket_data.p_jtb_exit_info->station_id >> 8);
			his_record[12] = (uint8_t)(m_ticket_data.p_jtb_exit_info->station_id & 0xFF);

			his_record[17] = ((lAmount >> 24) & 0xFF);
			his_record[18] = ((lAmount >> 16) & 0xFF);
			his_record[19] = ((lAmount >> 8) & 0xFF);
			his_record[20] = (lAmount & 0xFF);

			//dbg_formatvar("wallet_value |= %ld",m_ticket_data.wallet_value);
			//dbg_formatvar("lAmount |= %ld",lAmount);

			his_record[21] = ((m_ticket_data.wallet_value >> 24) & 0xFF);
			his_record[22] = ((m_ticket_data.wallet_value >> 16) & 0xFF);
			his_record[23] = ((m_ticket_data.wallet_value >> 8) & 0xFF);
			his_record[24] = (m_ticket_data.wallet_value & 0xFF);

			memcpy(his_record + 25, m_ticket_data.p_jtb_exit_info->time, sizeof(m_ticket_data.p_jtb_exit_info->time));

			Publics::string_to_bcds(m_ticket_data.p_jtb_exit_info->city_code, 4, &his_record[32], 2);
			memcpy(his_record + 34, m_ticket_data.p_jtb_exit_info->institutionCode, 8);
		}
	}

	ret.wErrCode = jtb_updata_more_complex(operType,tac_type, CmdSort::m_time_now,
		SAM_SOCK_4, m_ReadInf.p_public_base, p_metro,his_record, lAmount, 0,(unsigned char *)p_recv);
	if (ret.wErrCode != 0)
	{
		if (ret.wErrCode == 4)	// 消费并产生TAC时出错
			ret.bNoticeCode = NTC_MUST_CONFIRM;
		ret.wErrCode = ERR_CARD_WRITE;
	}

	return ret;
}

void TicketElecT::save_last_stack(ETYTKOPER operType, void * p_trade, size_t size_trade, void * p_written_inf, size_t size_written, bool save_to_file)
{
	METRO_CPU_INF_W written_info = {0};
	METRO_ELECT_INF * p_old_write = (METRO_ELECT_INF *)p_written_inf;

	TicketBase::save_last_stack_eticket(operType, p_trade, size_trade, &p_written_inf, sizeof(written_info), save_to_file);
}

bool TicketElecT::last_trade_need_continue(uint8_t status_targ, uint16_t err_targ, uint16_t err_src)
{
	bool ret = false;

	METRO_CPU_INF_W * p_old_write = (METRO_CPU_INF_W *)cfm_point.dataWritten;

	do
	{
		CONFIRM_FACTOR factor = get_confirm_factor();

		if (!check_confirm(status_targ, m_ticket_data.init_status, err_targ, err_src))
			break;

		// 直接判断复合消费是否成功，如果成功则需要继续去流水和TAC，否则认为失败
		if (p_old_write->flag_trade_assist > 0)
		{
			if (memcmp(p_old_write->w_trade_assist, m_ReadInf.p_trade_assist, LENM_TRADE_ASSIST) != 0)
				break;
		}

		if (p_old_write->flag_metro > 0)
		{
			if (memcmp(p_old_write->w_metro, m_ReadInf.p_metro, LENM_METRO) != 0)
				break;
		}

		if (p_old_write->flag_ctrl_record > 0)
		{
			if (memcmp(p_old_write->w_ctrl_record, m_ReadInf.p_ctrl_record, LENM_CTRL_RECORD) != 0)
				break;
		}

		if (cfm_point.balance != m_ticket_data.wallet_value)
			break;

		if (cfm_point.operType == operIncr)
		{
			if (m_ticket_data.charge_counter != cfm_point.charge_counter + 1 ||	cfm_point.consume_counter != m_ticket_data.total_trade_counter)
				break;
		}
		else
		{
			if (m_ticket_data.charge_counter != cfm_point.charge_counter ||	m_ticket_data.total_trade_counter != cfm_point.consume_counter + 1)
				break;
		}

		ret = true;

	} while (0);

    return ret;
}

RETINFO TicketElecT::continue_last_trade(void * p_trade)
{
	RETINFO ret			= {0};
	char sz_tac[11]		= {0};
	uint8_t u_recv[32]	= {0};
	uint16_t counter	= 0;

	do
	{
		memcpy(p_trade, &cfm_point_e.eticketdealinfo, sizeof(ETICKETDEALINFO));

	} while (0);

	g_Record.log_out(ret.wErrCode, level_error, "continue_last_trade(void * p_trade)=%d", ret.wErrCode);

	return ret;
}

RETINFO TicketElecT::data_version_upgrade()
{
	RETINFO ret		            = {0};
	unsigned char u_recv[256]	= {0};

	if (svt_unlock_recordfile(SAM_SOCK_1, m_ReadInf.p_issue_base + 8, 0x11, 0x00, u_recv) != 0)
		ret.wErrCode = ERR_CARD_WRITE;

	if (svt_consum(CmdSort::m_time_now, SAM_SOCK_1, m_ReadInf.p_issue_base + 8, m_ticket_data.wallet_value, 0x06, u_recv, u_recv) != 0)
		ret.wErrCode = ERR_CARD_WRITE;

	return ret;
}

uint16_t TicketElecT::read_wallet(long * p_wallet)
{
	uint16_t ret			= 0;
	unsigned char tmp[256]	= {0};
	ETPMDM physical_type	= mediNone;
	unsigned char type		= 0;

	do
	{
		if (ISO14443A_Init(g_ant_type) != 0)
		{
			ret = ERR_CARD_NONE;
			break;
		}

		ret = search_card_ex(tmp, type);
		if (ret != 0)	break;

		physical_type = (ETPMDM)type;
		if (physical_type != mediMetroCpu)
		{
			ret = ERR_DIFFRENT_CARD;
			break;
		}

		// 选择主目录
		if (svt_selectfile(0x3F00) < 0)
		{
			ret = ERR_CARD_READ;
			break;
		}

		// 读发行基本信息
		if (svt_readbinary(0x05, 0, LENM_ISSUE_BASE, tmp) < 0)
		{
			ret = ERR_CARD_READ;
			break;
		}

		// 选择ADF1
		if (svt_selectfile(0x1001) < 0)
		{
			if (svt_selecfileaid(9, (unsigned char *)"\xA0\x00\x00\x00\x03\x86\x98\x07\x01") < 0)
			{
				ret = ERR_CARD_READ;
				break;
			}
		}

		if (svt_getbalance(tmp,0x00) < 0)
		{
			ret = ERR_CARD_READ;
			break;
		}

		*p_wallet = (tmp[0] << 24) + (tmp[1] << 16) + (tmp[2] << 8) + tmp[3];

	} while (0);

	return ret;
}

ElectTicketType TicketElecT::getCurrentTicketType(){
    return ELECT_TICKET_SJ;//电子票单程票
}

void TicketElecT::getDealData(ETYTKOPER operType,uint16_t& len_data,uint8_t * dataBack,long amount){
    int length = 0;
    char StationCode[2+1] = {0};
    char LineCode[2+1] = {0};
    char DeviceType[2+1] = {0};
    char DeviceCode[3+1] = {0};
    char time[14+1] = {0};
    char exittime[14+1] = {0};
    long penalty_val	= 0;
    uint8_t tempTime[7] = {0};
    uint8_t validexitTime[7] = {0};

    char dealData[256] = {0};
    sprintf(LineCode, "%02x", ((Api::current_station_id >>8) & 0x00FF));
    sprintf(StationCode, "%02x", (Api::current_station_id & 0xFF));
    sprintf(DeviceType, "%02x", (uint8_t)(Api::current_device_type));
    sprintf(DeviceCode, "%03x", Api::current_device_id);

	memcpy(tempTime, CmdSort::m_time_now, 7);
    sprintf(time, "%02x%02x%02x%02x%02x%02x%02x", tempTime[0], tempTime[1], tempTime[2], tempTime[3], tempTime[4], tempTime[5], tempTime[6]);
	g_Parameter.get_validexittime(Api::current_station_id,tempTime, validexitTime, &penalty_val);
	sprintf(exittime, "%02x%02x%02x%02x%02x%02x%02x", validexitTime[0], validexitTime[1], validexitTime[2], validexitTime[3], validexitTime[4], validexitTime[5], validexitTime[6]);
    memcpy(m_ticket_data.qr_ticketdata.validOutTime, exittime, 14);

    switch (operType)
    {
        case operEntry:
        {
            memcpy(dealData,"01",2);
            length += 2;
            memcpy(dealData + length, time, 14);
            length += 14;
            memcpy(dealData + length, exittime, 14);
            length += 14;
            memcpy(dealData + length, LineCode, 2);//线路编码
            length += 2;
            memcpy(dealData + length, StationCode, 2);//站点编码
            length += 2;
            memcpy(dealData + length, DeviceType, 2);//设备类型
            length += 2;
            memcpy(dealData + length, DeviceCode, 3);//设备编号
            length += 3;
            memcpy(dealData + length, m_p_current_sam + 4, 12);
            length += 12;
            memcpy(dealData + length, "00000000", 8);//暂时不写
            length += 8;
            memcpy(dealData + length, "00", 2);
            length += 2;
            memcpy(dealData + length, "00000000", 8);
            length += 8;
            len_data = length;

        }
            break;
        case operExit:
        {
            memcpy(dealData,"02",2);
            length += 2;
            memcpy(dealData + length, time, 14);
            length += 14;
            memcpy(dealData + length, LineCode, 2);//线路编码
            length += 2;
            memcpy(dealData + length, StationCode, 2);//站点编码
            length += 2;
            memcpy(dealData + length, DeviceType, 2);//设备类型
            length += 2;
            memcpy(dealData + length, DeviceCode, 3);//设备编号
            length += 3;
            memcpy(dealData + length, m_p_current_sam + 4, 12);
            length += 12;
            memcpy(dealData + length, "20180702", 8);
            length += 8;
            sprintf(dealData + length,"%08X",amount);
            length += 8;
            memcpy(dealData + length, "00", 2);
            length += 2;
            memcpy(dealData + length, "00000000", 8);
            length += 8;
            len_data = length;

        }
            break;
        case operGetETicket:
        {
            memcpy(dealData,"05",2);
            length += 2;
            memcpy(dealData + length, time, 14);
            length += 14;
            memcpy(dealData + length, LineCode, 2);//线路编码
            length += 2;
            memcpy(dealData + length, StationCode, 2);//站点编码
            length += 2;
            memcpy(dealData + length, DeviceType, 2);//设备类型
            length += 2;
            memcpy(dealData + length, DeviceCode, 3);//设备编号
            length += 3;
            memcpy(dealData + length, m_p_current_sam + 4, 12);
            length += 12;
            memcpy(dealData + length, "00000000", 8);
            length += 8;
            memcpy(dealData + length, "00", 2);
            length += 2;
            memcpy(dealData + length, "00000000", 8);
            length += 8;
            len_data = length;
        }
            break;
        case operAddTicketResult:									//补票超时 结果 交易
		case operPenaltyRes:										//罚款通知
        {
            memcpy(dealData, "06", 2);								//交易码 06补票
            length += 2;
            memcpy(dealData + length, time, 14);					//时间
            length += 14;
            memcpy(dealData + length, LineCode, 2);					//线路编码
            length += 2;
            memcpy(dealData + length, StationCode, 2);				//站点编码
            length += 2;
            memcpy(dealData + length, DeviceType, 2);				//设备类型
            length += 2;
            memcpy(dealData + length, DeviceCode, 3);				//设备编号
            length += 3;
            memcpy(dealData + length, m_p_current_sam + 4, 12);		//终端号
            length += 12;
            memcpy(dealData + length, "00000000", 8);				//PSAM流水号
            length += 8;
            memcpy(dealData + length, "00", 2);						//状态码0—正常；1—失败
            length += 2;
            sprintf(dealData + length, "%08X", amount);				//超时金额
            length += 8;
			sprintf(dealData + length, "%08X", m_ticket_data.qr_ticketdata.lPenalty2);//超程金额
			length += 8;
            memcpy(dealData + length, exittime, 14);				//有效出站时间
            length += 14;
            memcpy(dealData + length, "00000000", 8);				//验证码
            length += 8;
            len_data = length;
        }
            break;

		case operClearEntry:
		{
			memcpy(dealData, "21", 2);								//交易码
			length += 2;
			memcpy(dealData + length, time, 14);					//进站时间
			length += 14;
			memcpy(dealData + length, exittime, 14);				//有效出站时间
			length += 14;
			memcpy(dealData + length, LineCode, 2);					//线路编码
			length += 2;
			memcpy(dealData + length, StationCode, 2);				//站点编码
			length += 2;
			memcpy(dealData + length, DeviceType, 2);				//设备类型
			length += 2;
			memcpy(dealData + length, DeviceCode, 3);				//设备编号
			length += 3;
			memcpy(dealData + length, m_p_current_sam + 4, 12);		//终端号
			length += 12;
			memcpy(dealData + length, "00000000", 8);				//PSAM流水号 暂时不写
			length += 8;
			memcpy(dealData + length, "00", 2);						//状态码
			length += 2;
			memcpy(dealData + length, "00000000", 8);				//验证码
			length += 8;
			len_data = length;

		}
		break;
		case operAddEntry:  //补进站
		{
			memcpy(dealData, "23", 2);								//交易码
			length += 2;
			memcpy(dealData + length, time, 14);					//进站时间
			length += 14;
			memcpy(dealData + length, exittime, 14);				//有效出站时间
			length += 14;

            memcpy(dealData + length, m_ticket_data.qr_ticketdata.startStation, 4);//进站的车站id 根据华腾师工所述
            length += 4;
            //进站设备类型  为固定值00
            memcpy(dealData + length, "00", 2);
            length += 2;
            //进站设备编号   固定值为000
            memcpy(dealData + length, "000", 3);
            length += 3;

			memcpy(dealData + length, m_p_current_sam + 4, 12);		//终端号
			length += 12;
			memcpy(dealData + length, "00000000", 8);				//PSAM流水号 暂时不写
			length += 8;
			memcpy(dealData + length, "00", 2);						//状态码
			length += 2;
			memcpy(dealData + length, "00000000", 8);				//验证码
			length += 8;
			len_data = length;

		}
		break;
		case operClearExit:
		{
			memcpy(dealData, "22", 2);								//交易码
			length += 2;
			memcpy(dealData + length, time, 14);					//出站时间
			length += 14;
			memcpy(dealData + length, LineCode, 2);					//线路编码
			length += 2;
			memcpy(dealData + length, StationCode, 2);				//站点编码
			length += 2;
			memcpy(dealData + length, DeviceType, 2);				//设备类型
			length += 2;
			memcpy(dealData + length, DeviceCode, 3);				//设备编号
			length += 3;
			memcpy(dealData + length, m_p_current_sam + 4, 12);		//PSAM终端号
			length += 12;
			memcpy(dealData + length, "20180702", 8);				//PSAM流水号
			length += 8;
			sprintf(dealData + length, "%08X", amount);				//实扣金额
			length += 8;
			memcpy(dealData + length, "00", 2);						//状态码
			length += 2;
			memcpy(dealData + length, "00000000", 8);				//验证码
			length += 8;
			len_data = length;

		}
		break;
		case operAddExit:
		{
			memcpy(dealData, "24", 2);								//交易码
			length += 2;
			memcpy(dealData + length, time, 14);					//出站时间
			length += 14;
			memcpy(dealData + length, LineCode, 2);					//线路编码
			length += 2;
			memcpy(dealData + length, StationCode, 2);				//站点编码
			length += 2;
			memcpy(dealData + length, DeviceType, 2);				//设备类型
			length += 2;
			memcpy(dealData + length, DeviceCode, 3);				//设备编号
			length += 3;
			memcpy(dealData + length, m_p_current_sam + 4, 12);		//PSAM终端号
			length += 12;
			memcpy(dealData + length, "20180702", 8);				//PSAM流水号
			length += 8;
			sprintf(dealData + length, "%08X", amount);				//实扣金额
			length += 8;
			memcpy(dealData + length, "00", 2);						//状态码
			length += 2;
			memcpy(dealData + length, "00000000", 8);				//验证码
			length += 8;
			len_data = length;

		}
		break;
        default:
            break;
    }
    memcpy(dataBack, (uint8_t * )dealData, length);
}
