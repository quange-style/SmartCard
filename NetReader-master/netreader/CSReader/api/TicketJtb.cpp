#include "TicketJtb.h"
#include "Errors.h"
#include "Publics.h"
#include "TimesEx.h"
#include "../link/myprintf.h"

TicketJtb::TicketJtb(void)
{

}

TicketJtb::TicketJtb(char * p_current_sam, char * p_sam_posid, uint8_t * physic_info)
{
	m_p_current_sam = p_current_sam;
	m_p_sam_posid = p_sam_posid;

	memset(&m_ticket_data, 0, sizeof(m_ticket_data));
	memset(&m_ReadInf, 0, sizeof(m_ReadInf));

	sprintf(m_ticket_data.physical_id, "%02X%02X%02X%02X%02X%02X%02X%-6C",
		physic_info[0], physic_info[1], physic_info[2], physic_info[3], physic_info[4], physic_info[5], physic_info[6], 0x20);

	pMemoryPos = ((uint8_t *)this) + sizeof(TicketJtb);
	m_ticket_data.p_entry_info = new(pMemoryPos) TradeInfo;

	pMemoryPos1 = ((uint8_t *)this) + sizeof(TicketJtb) + sizeof(TradeInfo);
	m_ticket_data.p_jtb_entry_info = new(pMemoryPos1) JTBTradeInfo;

	m_ticket_data.p_exit_info = &m_ticket_data.read_last_info;
	m_ticket_data.p_update_info = &m_ticket_data.read_last_info;

	memset(&m_ticket_data.read_last_info, 0, sizeof(TradeInfo));

	m_ticket_data.qr_flag = 0x00;
}

TicketJtb::~TicketJtb(void)
{

}

uint16_t TicketJtb::metro_jtb_read(P_METRO_JTBCPU_INF p_mtr_inf)
{
	uint16_t u_ret			= 0;
	//unsigned short nresult		= 0;
	unsigned short response_sw	= 0;
	unsigned char ulen			= 0;
	unsigned char sztmp[256]	= {0};

	do
	{

		//dbg_formatvar("metro_jtb_read");

		// 选择主目录
		/*
		if (MifareProRst(0x40,ulen,sztmp) == 0)
		{
			u_ret = ERR_CARD_READ;
			break;
		}


 		ulen = 19;
 		memcpy(sztmp, "\x00\xA4\x04\x00\x0E\x32\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", ulen);
		dbg_dumpmemory("ppse=",sztmp,19);
 		ulen = MifareProCom(ulen, sztmp, &response_sw);
		dbg_formatvar("ulen = %d |response_sw = %04X",ulen,response_sw);
		if( (ulen == 0) || (response_sw != 0x9000))
		{
			u_ret = ERR_CARD_READ;
			break;
		}*/

		//通过AID的方式选择互联互通电子钱包应用
		ulen = 13;
		memcpy(sztmp, "\x00\xA4\x04\x00\x08\xA0\x00\x00\x06\x32\x01\x01\x05", ulen);
		dbg_dumpmemory("aid=",sztmp,13);
 		ulen = MifareProCom(ulen, sztmp, &response_sw);
		dbg_formatvar("ulen = %d |response_sw = %04X",ulen,response_sw);
		if( (ulen == 0) || (response_sw != 0x9000))
		{
			g_Record.log_out(u_ret, level_error, "select interconnecte elec wallet failed");
			u_ret = ERR_CARD_READ;
			break;
		}

		// 读公共应用文件
		if (p_mtr_inf->p_public_base != NULL)
		{
			if (svt_readbinary(0x15, 0, LENM_PUBLIC_BASE, p_mtr_inf->p_public_base) < 0)
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read binary file 15 failed");
				break;
			}
		}
		dbg_dumpmemory("p_public_base=",p_mtr_inf->p_public_base,30);

		// 读持卡人信息
		if (p_mtr_inf->p_owner_base != NULL)
		{
			dbg_formatvar("p_owner_base");
			if (svt_readbinary(0x16, 0, LENB_OWNER_BASE, p_mtr_inf->p_owner_base) < 0)
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read binary file 16 failed");
				break;
			}
		}
		//dbg_dumpmemory("p_owner_base=",p_mtr_inf->p_owner_base,55);

		// 读管理信息文件
		if (p_mtr_inf->p_manage_info != NULL)
		{
			if (svt_readbinary(0x17, 0, LENM_JTBMANAGE, p_mtr_inf->p_manage_info) < 0)
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read binary file 17 failed");
				break;
			}
		}
		dbg_dumpmemory("p_manage_info=",p_mtr_inf->p_manage_info,60);


		// 轨道交通
		if (p_mtr_inf->p_metro != NULL)
		{
			if (svt_jtbreadrecord(0x1A, 1, 0x00, p_mtr_inf->p_metro) < 0)
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read complex file 1A-1 failed");
				break;
			}
		}

		dbg_dumpmemory("p_metro=",p_mtr_inf->p_metro,128);

		// 钱包
		if (p_mtr_inf->p_wallet != NULL)
		{
			if (svt_getbalance(p_mtr_inf->p_wallet,0x03) < 0)
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read wallet failed");
				break;
			}
		}

		// 所有历史-公共交通过程信息循环记录文件
		if (p_mtr_inf->p_his_all != NULL)
		{
			if (svt_readhistory(0x1E, 1, 1, p_mtr_inf->p_his_all) < 0)
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read history 18 failed");
				break;
			}
		}

		m_ticket_data.read_last_info.station_id = (p_mtr_inf->p_his_all[10] << 8) + p_mtr_inf->p_his_all[12];


		/*
		// 充值计数
		if (p_mtr_inf->p_charge_count != NULL)
		{
			if (svt_getonlineserial(SAM_SOCK_4, p_mtr_inf->p_charge_count) < 0)
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read on-line counter failed");
				break;
			}
		}

		dbg_dumpmemory("p_charge_count=",p_mtr_inf->p_charge_count,2);

		// 交易计数
		if (p_mtr_inf->p_consume_count != NULL)
		{
			if (svt_getofflineserial(SAM_SOCK_4, p_mtr_inf->p_consume_count) < 0)
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read off-line counter failed");
				break;
			}
		}

		dbg_dumpmemory("p_consume_count=",p_mtr_inf->p_consume_count,2);
		*/

	} while (0);

	return u_ret;
}

RETINFO TicketJtb::read_card(ETYTKOPER operType, TICKET_PARAM& ticket_prm, uint8_t * p_read_init/* = NULL*/)
{
	RETINFO ret				= {0};

	do
	{
		memset(&m_u_buffer, 0, sizeof(m_u_buffer));
		//dbg_formatvar("file_need_read");
		m_ReadInf = file_need_read(operType, m_u_buffer);

		ret.wErrCode = metro_jtb_read(&m_ReadInf);
		if (ret.wErrCode != 0)	break;

		//
		// 公共信息
		//
		get_public_base(m_ReadInf.p_public_base);
		//
		// 持卡人信息
		//
		get_owner_base(m_ReadInf.p_owner_base);

		//
		// 管理信息
		//
		get_manage_info(m_ReadInf.p_manage_info);
		//
		// 钱包
		//
		get_wallet(m_ReadInf.p_wallet);
		//
		// 充值计数
		//
		get_change_count(m_ReadInf.p_charge_count);
		//
		// 交易计数
		//
		get_consume_count(m_ReadInf.p_consume_count);
		//
		// 地铁信息
		//
		get_metro(m_ReadInf.p_metro);
		//
		// 所有历史
		//
		get_his_all(m_ReadInf.p_his_all);

		m_ticket_data.simple_status = get_simple_status(m_ticket_data.init_status);

		dbg_formatvar("simple_status |= %d",m_ticket_data.simple_status);

	} while (0);

	return ret;
}

uint8_t TicketJtb::unified_status()
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

RETINFO TicketJtb::analyse_common(uint8_t wk_area, MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PTICKETLOCK p_lock, uint16_t& lenlock, PBOMANALYZE p_analyse, char * p_mode)
{
	RETINFO ret				= {0};
	char mode				= '0';
	char p_jtb_card_issue_code[11];	// 发卡机构标识

	do
	{
		// 物理有效期
		if (p_ticket_prm->check_phy_expire && need_check_period(wk_area, m_ticket_data.simple_status))
		{
			if (memcmp(CmdSort::m_time_now, m_ticket_data.phy_peroidE, 4) > 0)
			{
				if (p_degrade_falgs->train_trouble && wk_area == 1)
				{
					mode = (char)(0x30 + degrade_Trouble);
				}
				else if (p_degrade_falgs->uncheck_datetime)
				{
					mode = (char)(0x30 + degrade_Uncheck_DateTime);
				}
				else
				{
					ret.wErrCode = ERR_OVER_PERIOD_P;
					break;
				}
			}
		}

		memset(p_jtb_card_issue_code, 0, 11);
		memcpy(p_jtb_card_issue_code,m_ticket_data.issue_code,4);
		memcpy(&p_jtb_card_issue_code[4],m_ticket_data.city_code,4);

		//dbg_formatvar("p_jtb_card_issue_code:%s",p_jtb_card_issue_code);

	    // 检查白名单
		bool in_prm = g_Parameter.query_white_jtb(p_jtb_card_issue_code);
		if(!in_prm)
		{
			ret.wErrCode = ERR_NOINT_WHITE;
			break;
		}

	    // 检查是否是互联互通票卡
		in_prm = g_Parameter.is_interconnect(m_ticket_data.interCardFlag,m_ticket_data.city_code_17);
		if(!in_prm)
		{
			ret.wErrCode = ERR_INTER_CARD;
			break;
		}

		// 检查黑名单
		in_prm = g_Parameter.query_black_jtb(m_ticket_data.logical_id);
		if (m_ticket_data.list_falg > 0)
		{
			if (p_degrade_falgs->train_trouble)
			{
				mode = (char)(0x30 + degrade_Trouble);
			}
			else
			{
				if (!in_prm)
				{
					if (p_analyse != NULL)
						p_analyse->dwOperationStauts |= ALLOW_UNLOCK;
				}
				ret.wErrCode = ERR_CARD_LOCKED;
				break;
			}
		}
		else if (in_prm)
		{
			if (p_degrade_falgs->train_trouble)
			{
				mode = (char)(0x30 + degrade_Trouble);
			}
			else
			{
				p_lock->cLockFlag = '1';
				ret = lock(p_lock);

				if (ret.wErrCode == 0)
				{
					lenlock = sizeof(TICKETLOCK);
					//lenlock = 0;
					ret.wErrCode = ERR_CARD_LOCKED;
				}
				break;
			}
		}

		// 车票状态检查
		if (m_ticket_data.simple_status == SStatus_Invalid)
		{
			ret.wErrCode = ERR_CARD_STATUS;
			break;
		}

		if (m_ticket_data.simple_status == SStatus_Refund)
		{
			ret.wErrCode = ERR_CARD_REFUND;
			break;
		}

	} while (0);

	if (p_mode != NULL)
		*p_mode = mode;

	return ret;
}

RETINFO TicketJtb::analyse_work_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PPURSETRADE p_purse, PBOMETICANALYZE p_eticanalyse)
{
	RETINFO ret				= {0};
	ret = TicketBase::analyse_work_zone(p_degrade_falgs, p_ticket_prm, p_analyse, p_purse, p_eticanalyse);
	return ret;
}

RETINFO TicketJtb::analyse_free_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PBOMETICANALYZE p_eticanalyse)
{
	RETINFO ret				= {0};

	//增加交通票卡的判断
	//1A文件的交易状态为01、03即为未完成交易
	//if(m_ticket_data)

	ret = TicketBase::analyse_free_zone(p_degrade_falgs, p_ticket_prm, p_analyse, p_eticanalyse);

	if (ret.wErrCode == 0 || ret.wErrCode == ERR_LACK_WALLET)
	{
		if (m_ticket_data.wallet_value <= p_ticket_prm->refund_max_value)
		{
			if (p_ticket_prm->pemite_refund && m_ticket_data.wallet_value > 0)
			{
				if (p_analyse != NULL)
					p_analyse->dwOperationStauts |= ALLOW_REFUND;
			}

			if (p_ticket_prm->permit_refund_deposite && m_ticket_data.deposit > 0)
			{
				if (p_analyse != NULL)
					p_analyse->dwOperationStauts |= ALLOW_REFUND_DE;
			}
		}
	}

	return ret;
}

uint32_t TicketJtb::add_oper_status(uint8_t wk_area, RETINFO ret, TICKET_PARAM * p_ticket_prm)
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

RETINFO TicketJtb::entry_gate(PENTRYGATE p_entry,PETICKETDEALINFO p_eticketdealinfo,char cOnlineRes)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;

	METRO_JTBCPU_INF write_inf = file_need_write(operEntry, &m_ReadInf);

	m_ticket_data.init_status = MS_Entry;
	m_ticket_data.p_jtb_entry_info = &m_ticket_data.jtb_write_last_info;
	current_jtbtrade_info(*m_ticket_data.p_jtb_entry_info);

	m_ticket_data.p_jtb_update_info = &m_ticket_data.jtb_write_last_info;
	current_jtbtrade_info(*m_ticket_data.p_jtb_update_info);

	ret = write_card(operEntry, write_inf, 0x09, 0, lSamSeq, NULL);
	p_entry->lSAMTrSeqNo = lSamSeq;

	transfer_jtb_data_for_out(operEntry, p_entry);
	p_entry->bStatus = LIFE_ENTRY;

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
	{
		save_last_stack(operEntry, p_entry, sizeof(ENTRYGATE), &write_inf, sizeof(write_inf), false);
	}
	return ret;
}

RETINFO TicketJtb::exit_gate(PPURSETRADE p_exit,PETICKETDEALINFO p_eticketdealinfo, MODE_EFFECT_FLAGS * p_degrade_falgs,char cOnlineRes)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;
	uint8_t tacType			= 0x09;

	METRO_JTBCPU_INF write_inf = file_need_write(operExit, &m_ReadInf);

	if (p_degrade_falgs->train_trouble)
		m_ticket_data.init_status = MS_Exit_Tt;
	else
		m_ticket_data.init_status = MS_Exit;
	m_ticket_data.p_jtb_exit_info = &m_ticket_data.jtb_write_last_info;
	m_ticket_data.wallet_value -= p_exit->lTradeAmount;

	current_jtbtrade_info(*m_ticket_data.p_jtb_exit_info);

	//p_exit->lTradeAmount = 1;//测试用

	ret = write_card(operExit, write_inf, tacType, p_exit->lTradeAmount, lSamSeq, p_exit->cMACorTAC);
	p_exit->lSAMTrSeqNo = lSamSeq;

	transfer_jtb_data_for_out(operExit, p_exit);
	if (m_ticket_data.simple_status == SStatus_Upd_WZone)
		memcpy(p_exit->cPaymentType, "2C", sizeof(p_exit->cPaymentType));
	else
		memcpy(p_exit->cPaymentType, "22", sizeof(p_exit->cPaymentType));
	Publics::hex_to_two_char(tacType, p_exit->cClassicType);

	p_exit->bStatus = LIFE_EXIT;

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
	{
		save_last_stack(operExit, p_exit, sizeof(PURSETRADE), &write_inf, sizeof(write_inf), false);
	}

	return ret;
}

RETINFO TicketJtb::check_online(ETYTKOPER operType,PETICKETAUTHINFO p_eticketauthinfo)
{
	RETINFO ret				= {0};
	return ret;
}

RETINFO TicketJtb::esale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, PETICKETDEALINFO p_eticketdealinfo, uint16_t& len_eticket_sale, TICKET_PARAM& ticket_prm)
{
	RETINFO ret				= {0};
	return ret;
}

RETINFO TicketJtb::thirdesale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, PETICKETDEALINFO p_eticketdealinfo, uint16_t& len_eticket_sale, TICKET_PARAM& ticket_prm , char * cNetTranNumber)
{
	RETINFO ret				= {0};
	return ret;
}

RETINFO TicketJtb::sale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, POTHERSALE p_svt_sale, uint16_t& len_svt, TICKET_PARAM& ticket_prm)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;
	len_sjt					= 0;
	len_svt					= 0;

	METRO_JTBCPU_INF write_inf = file_need_write(operSvtSale, &m_ReadInf);

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

	ret = write_card(operSvtSale, write_inf, 0x09, 0, lSamSeq, NULL);
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

RETINFO TicketJtb::bom_update(PTICKETUPDATE p_update, uint8_t * p_entry_station)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;
	long fare				= 0;

	METRO_JTBCPU_INF write_inf = file_need_write(operUpdate, &m_ReadInf);

	switch (p_update->bUpdateReasonCode)
	{
	case 0x01:		// 付费区超时
		m_ticket_data.init_status = MS_Upd_Wzone_Time;
		p_update->bStatus = LIFE_UPD_TM_OUT;
		break;
	case 0x02:		// 付费区超乘
		m_ticket_data.init_status = MS_Upd_Wzone_Trip;
		p_update->bStatus = LIFE_UPD_TP_OUT;
		break;
	case 0x03:		// 付费区无进站码
		m_ticket_data.p_jtb_entry_info->station_id = (p_entry_station[0] << 8) + p_entry_station[1];
		m_ticket_data.init_status = MS_Upd_Wzone_Entry;
		p_update->bStatus = LIFE_UPD_WO_STA;
		ret.wErrCode = g_Parameter.query_fare(p_update->dtUpdateDate,
			m_ticket_data.fare_type, m_ticket_data.p_jtb_entry_info->station_id, p_update->dtUpdateDate, &fare);
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
	//如果参数没有传错
	if (ret.wErrCode == 0)
	{
		m_ticket_data.p_jtb_update_info = &m_ticket_data.jtb_write_last_info;
		current_jtbtrade_info(*m_ticket_data.p_jtb_update_info);

		ret = write_card(operUpdate, write_inf, 0x09, 0, lSamSeq, NULL);
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

RETINFO TicketJtb::bom_refund(PDIRECTREFUND p_refund)
{
	RETINFO ret				= {0};
	return ret;
}

RETINFO TicketJtb::svt_increase(PPURSETRADE p_purse, uint8_t * p_time, uint8_t * p_mac2,uint8_t paytype)
{
	RETINFO ret					= {0};
	return ret;
}

RETINFO TicketJtb::svt_decrease(PPURSETRADE p_purse, uint8_t u_pay_flag,uint8_t paytype)
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
		METRO_JTBCPU_INF write_inf = file_need_write(operDecr, &m_ReadInf);
		m_ticket_data.wallet_value -= p_purse->lTradeAmount;
		m_ticket_data.p_jtb_exit_info = &m_ticket_data.jtb_write_last_info;

		current_jtbtrade_info(*m_ticket_data.p_jtb_exit_info);

		ret = write_card(operDecr, write_inf, tacType, p_purse->lTradeAmount, lSamSeq, p_purse->cMACorTAC);
		p_purse->lSAMTrSeqNo = lSamSeq;
		p_purse->cPayType = paytype;

		transfer_jtb_data_for_out(operDecr, p_purse);

		if (u_pay_flag == PAY_UPD_PENALTY)
			memcpy(p_purse->cPaymentType, "2A", sizeof(p_purse->cPaymentType));
		else if (u_pay_flag == PAY_INC_DESIND)
			memcpy(p_purse->cPaymentType, "28", sizeof(p_purse->cPaymentType));
		else if (u_pay_flag == PAY_BUY_TOKEN)
			memcpy(p_purse->cPaymentType, "29", sizeof(p_purse->cPaymentType));

		p_purse->bStatus = LIFE_DECREASE;
		Publics::hex_to_two_char(tacType, p_purse->cClassicType);

		if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		{
			save_last_stack(operDecr, p_purse, sizeof(PURSETRADE), &write_inf, sizeof(write_inf), true);
		}
	}

	return ret;
}

METRO_JTBCPU_INF TicketJtb::file_need_read(ETYTKOPER type, uint8_t * p_buffer)
{
	int ptr_buffer			= 0;
	METRO_JTBCPU_INF ret_inf	= {0};

	ret_inf.p_public_base = p_buffer + ptr_buffer;
	ptr_buffer += LENM_PUBLIC_BASE;

	ret_inf.p_wallet = p_buffer + ptr_buffer;
	ptr_buffer += LENM_WALLET;

	ret_inf.p_metro = p_buffer + ptr_buffer;
	ptr_buffer += LENM_JTBMETRO;

	ret_inf.p_manage_info = p_buffer + ptr_buffer;
	ptr_buffer += LENM_JTBMANAGE;

	ret_inf.p_charge_count = p_buffer + ptr_buffer;
	ptr_buffer += LENM_CHANGE_CNT;

	ret_inf.p_consume_count = p_buffer + ptr_buffer;
	ptr_buffer += LENM_CONSUME_CNT;

	ret_inf.p_his_all = p_buffer + ptr_buffer;
	ptr_buffer += LENM_HIS_ALL;

	switch (type)
	{
	case operUpdate:
	case operAnalyse:
	case operSale:
	case operEntry:
	case operExit:
	case operDecr:
		ret_inf.p_owner_base = p_buffer + ptr_buffer;
		ptr_buffer += LENM_JTBOWNER_BASE;

		break;

	case operTkInf:
		ret_inf.p_owner_base = p_buffer + ptr_buffer;
		ptr_buffer += LENM_OWNER_BASE;

		//ret_inf.p_his_all = p_buffer + ptr_buffer;
		//ptr_buffer += LENM_HIS_ALL;
		break;


	case operSvtSale:
	case operLock:
	case operDeffer:
	case operIncr:
	case operRefund:
	//case operDecr:
		break;
    default:
        break;
	}

	return ret_inf;
}

METRO_JTBCPU_INF TicketJtb::file_need_write(ETYTKOPER type, P_METRO_JTBCPU_INF p_read_inf)
{
	METRO_JTBCPU_INF ret_inf = {0};

	switch (type)
	{
	case operSvtSale:
		ret_inf.p_app_ctrl = p_read_inf->p_app_ctrl;
		ret_inf.p_ctrl_record = p_read_inf->p_ctrl_record;
		ret_inf.p_metro = p_read_inf->p_metro;
		break;
	case operLock:
		ret_inf.p_trade_assist = p_read_inf->p_trade_assist;
		ret_inf.p_metro = p_read_inf->p_metro;
		break;
	case operDeffer:
		ret_inf.p_app_ctrl = p_read_inf->p_app_ctrl;
		ret_inf.p_ctrl_record = p_read_inf->p_ctrl_record;
		break;
	case operDecr:
	case operIncr:
		break;
	case operRefund:
		ret_inf.p_app_ctrl = p_read_inf->p_app_ctrl;
		ret_inf.p_metro = p_read_inf->p_metro;
		break;
	case operUpdate:
	case operEntry:
	case operExit:
		ret_inf.p_his_all = p_read_inf->p_his_all;
		ret_inf.p_metro = p_read_inf->p_metro;
		break;
    default:
        break;
	}

	return ret_inf;
}

void TicketJtb::get_issue_base(uint8_t * p_issue_base)
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

void TicketJtb::get_public_base(uint8_t * p_public_base)
{
	if (p_public_base != NULL)
	{
		char cardNo[21];
		memset(cardNo, 0x20, 21);

		// 发卡方代码
		Publics::bcds_to_string(p_public_base, 2, m_ticket_data.issue_code, 4);

		//dbg_formatvar("issue_code = %s",m_ticket_data.issue_code);

		// 城市代码、行业代码
		Publics::bcds_to_string(p_public_base + 2, 2, m_ticket_data.city_code, 4);
		memcpy(m_ticket_data.industry_code, "0000", 4);

		// 应用标识
		//m_ticket_data.key_flag = 0x01;

		//dbg_formatvar("key_flag = %02x",m_ticket_data.key_flag);

		// 逻辑卡号
		sprintf(cardNo, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
			p_public_base[10], p_public_base[11],p_public_base[12], p_public_base[13],p_public_base[14],
			p_public_base[15], p_public_base[16], p_public_base[17], p_public_base[18], p_public_base[19]);

		memcpy(m_ticket_data.logical_id, cardNo+1, 19);
		m_ticket_data.logical_id[19] = 0x20;

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

void TicketJtb::get_owner_base(uint8_t * p_owner_base)
{
	if (p_owner_base != NULL)
	{
		int i = 0;
		memcpy(m_ticket_data.certificate_name, p_owner_base + 2, sizeof(m_ticket_data.certificate_name));
		memcpy(m_ticket_data.certificate_code, p_owner_base + 22, sizeof(m_ticket_data.certificate_code));
		for(i = 0; i < 20; i++)
		{
			if(m_ticket_data.certificate_name[i] == 0x00)
				m_ticket_data.certificate_name[i] = 0x20;
		}

		for(i = 0; i < 32; i++)
		{
			if(m_ticket_data.certificate_code[i] == 0x00)
				m_ticket_data.certificate_code[i] = 0x20;
		}
		m_ticket_data.certificate_type = p_owner_base[54];
	}
}

void TicketJtb::get_manage_info(uint8_t * p_manage_info)
{
	if (p_manage_info != NULL)
	{
		memcpy(m_ticket_data.city_code_17, p_manage_info + 6, sizeof(m_ticket_data.city_code_17));
		memcpy(m_ticket_data.interCardFlag, p_manage_info + 8, sizeof(m_ticket_data.interCardFlag));
		m_ticket_data.jtbCardType = p_manage_info[10];

		Publics::bcds_to_string(p_manage_info, 4, m_ticket_data.international_code, 8);
		Publics::bcds_to_string(p_manage_info + 4, 2, m_ticket_data.province_code, 4);

		//dbg_dumpmemory("international_code = ",m_ticket_data.international_code,4);
		//dbg_dumpmemory("province_code = ",m_ticket_data.province_code,2);
		//dbg_dumpmemory("city_code_17 = ",m_ticket_data.city_code_17,2);
		//dbg_dumpmemory("interCardFlag = ",m_ticket_data.interCardFlag,2);
		//dbg_formatvar("jtbCardType = %02x",m_ticket_data.jtbCardType);
	}
}

void TicketJtb::get_wallet(uint8_t * p_wallet)
{
	if (p_wallet != NULL)
	{
		m_ticket_data.wallet_value = (p_wallet[0] << 24) + (p_wallet[1] << 16) + (p_wallet[2] << 8) + p_wallet[3];
	}
}

void TicketJtb::get_his_all(uint8_t * p_his_all)
{
	if (p_his_all != NULL)
	{

	}
}

void TicketJtb::get_trade_assist(uint8_t * p_trade_assist)
{
	if (p_trade_assist != NULL)
	{
		m_ticket_data.list_falg = p_trade_assist[2];
	}
}

void TicketJtb::get_metro(uint8_t * p_metro)
{
	if (p_metro != NULL)
	{
		if(p_metro[14] == 0x01)
		{
			m_ticket_data.init_status = MS_Entry;
			m_ticket_data.p_entry_info->dvc_type = (ETPDVC)(4);
		}
		else if(p_metro[14] == 0x02)
		{
			m_ticket_data.init_status = MS_Exit;
			m_ticket_data.p_entry_info->dvc_type = (ETPDVC)(5);
		}
		else if(p_metro[14] == 0x03)
		{
			m_ticket_data.init_status = MS_Upd_FZone_Free;
			m_ticket_data.p_entry_info->dvc_type = (ETPDVC)(3);
		}
		else if(p_metro[14] == 0x04)
		{
			m_ticket_data.init_status = MS_Upd_Wzone_Trip;
			m_ticket_data.p_entry_info->dvc_type = (ETPDVC)(3);
		}else if(p_metro[14] == 0x00)
		{
			m_ticket_data.init_status = MS_Init;
			m_ticket_data.p_entry_info->dvc_type = (ETPDVC)(3);
		}
		//dbg_formatvar("init_status |= %02X",m_ticket_data.init_status);


		m_ticket_data.p_jtb_entry_info->station_id = (p_metro[35] << 8) + p_metro[36];
		memcpy(&m_ticket_data.p_jtb_entry_info->institutionCode, &p_metro[19], 8);
		Publics::bcds_to_string(&p_metro[51], 8, m_ticket_data.p_jtb_entry_info->sam, 16);
		Publics::bcds_to_string(&p_metro[15], 2, m_ticket_data.p_jtb_entry_info->city_code, 4);
		memcpy(&m_ticket_data.p_jtb_entry_info->time, p_metro + 67, 7);
		m_ticket_data.p_jtb_entry_info->line_id = p_metro[85];
		m_ticket_data.p_jtb_entry_info->deal_fee = (p_metro[87] << 24) + (p_metro[88] << 16) + (p_metro[89] << 8) + p_metro[90];
		m_ticket_data.p_jtb_entry_info->balance = (p_metro[91] << 24) + (p_metro[92] << 16) + (p_metro[93] << 8) + p_metro[94];

		m_ticket_data.p_entry_info->station_id = (p_metro[35] << 8) + p_metro[36];
		memcpy(&m_ticket_data.p_entry_info->time, p_metro + 67, 7);
		Publics::bcds_to_string(&p_metro[51], 8, m_ticket_data.p_entry_info->sam, 16);
		//新增测试
		//m_ticket_data.p_exit_info->station_id = (p_metro[43] << 8) + p_metro[44];
		//memcpy(&m_ticket_data.p_exit_info->time, p_metro + 74, 7);
		//Publics::bcds_to_string(&p_metro[59], 8, m_ticket_data.p_exit_info->sam, 16);

		if(p_metro[14] == 0x03)
		{
			//m_ticket_data.read_last_info.station_id = (p_metro[43] << 8) + p_metro[44];

			memcpy(m_ticket_data.read_last_info.time, p_metro + 74, sizeof(m_ticket_data.read_last_info.time));

			memcpy(m_ticket_data.read_last_info.sam,m_ticket_data.p_jtb_entry_info->sam, 16);
		}
		else if (p_metro[14] == 0x04)
		{
			//m_ticket_data.read_last_info.station_id = (p_metro[35] << 8) + p_metro[36];

			memcpy(m_ticket_data.read_last_info.time, p_metro + 67, sizeof(m_ticket_data.read_last_info.time));

			memcpy(m_ticket_data.read_last_info.sam,m_ticket_data.p_jtb_entry_info->sam, 16);
		}
		else
		{
			m_ticket_data.read_last_info.station_id = (p_metro[35] << 8) + p_metro[36];

			memcpy(m_ticket_data.read_last_info.time, p_metro + 67, sizeof(m_ticket_data.read_last_info.time));

			memcpy(m_ticket_data.read_last_info.sam,m_ticket_data.p_jtb_entry_info->sam, 16);
		}
	}
}

void TicketJtb::get_ctrl_record(uint8_t * p_ctrl_record)
{
	if (p_ctrl_record != NULL)
	{
		m_ticket_data.app_lock_flag = p_ctrl_record[2];
		memcpy(m_ticket_data.logical_peroidS, p_ctrl_record + 4, 7);
		memcpy(m_ticket_data.logical_peroidE, m_ticket_data.logical_peroidS, 7);
	}
}

void TicketJtb::get_app_ctrl(uint8_t * p_app_ctrl)
{
	// 由于涉及有效期，调用本函数必须在调用get_ctrl_record后面
	if (p_app_ctrl != NULL)
	{
		m_ticket_data.globle_status = p_app_ctrl[0];
		m_ticket_data.effect_mins = (p_app_ctrl[1] << 24) + (p_app_ctrl[2] << 16) + (p_app_ctrl[3] << 8) + p_app_ctrl[4];
		TimesEx::bcd_time_calculate(m_ticket_data.logical_peroidE, T_TIME, 0, 0, m_ticket_data.effect_mins);

		m_ticket_data.deposit = p_app_ctrl[5] * 100;

		m_ticket_data.wallet_value_max = (p_app_ctrl[12] << 8) + p_app_ctrl[13];
		m_ticket_data.wallet_value_max *= 100;

		m_ticket_data.limit_mode = p_app_ctrl[14];
		memcpy(m_ticket_data.limit_entry_station, p_app_ctrl + 15, 2);
		memcpy(m_ticket_data.limit_exit_station, p_app_ctrl + 17, 2);

		// 未锁定表示未发售或者多日票未激活，有效期尚未开始
		if (m_ticket_data.app_lock_flag == 0)
		{
			memset(m_ticket_data.logical_peroidS, 0x00, 7);
			memset(m_ticket_data.logical_peroidE, 0x00, 7);
			m_ticket_data.globle_status = EXTERN_STATUS_INIT;
		}
	}
}

void TicketJtb::get_change_count(uint8_t * p_charge_count)
{
	if (p_charge_count != NULL)
	{
		m_ticket_data.charge_counter = (p_charge_count[0] << 8) + p_charge_count[1];
	}
}

void TicketJtb::get_consume_count(uint8_t * p_consume_count)
{
	if (p_consume_count != NULL)
	{
		m_ticket_data.total_trade_counter = (p_consume_count[0] << 8) + p_consume_count[1];
	}
}


RETINFO TicketJtb::lock_card(unsigned char *lplogicnumber, unsigned char *issueno, long& sam_seq, char * p_trade_tac)
{
	RETINFO ret			= {0};
	uint8_t u_recv[64]	= {0};
	char sz_tac[11]		= {0};
	int nresult=-1;

	dbg_formatvar("TicketJtb::lock_card");

	do
	{
		ret.wErrCode = ERR_CARD_WRITE;

		// 选择ADF1
		if (jtb_selectppse() != 0)
			break;

		nresult = jtb_lock_card(SAM_SOCK_4,lplogicnumber,issueno);
		if(nresult == 0)
			ret.wErrCode = ERR_SUCCEED;
		else
			ret.wErrCode = ERR_CARD_WRITE;

	} while (0);

	return ret;
}

RETINFO TicketJtb::write_card(ETYTKOPER operType, METRO_JTBCPU_INF write_inf, uint8_t tac_type, long trade_amount, long& sam_seq, char * p_trade_tac)
{
	RETINFO ret			= {0};
	uint8_t u_recv[64]	= {0};
	char sz_tac[11]		= {0};

	do
	{
		ret.wErrCode = ERR_CARD_WRITE;

		// 选择ADF1
		if (jtb_selectppse() != 0)
			break;
		ret = set_complex_file(operType,tac_type, trade_amount, write_inf.p_metro,u_recv);

		if (ret.wErrCode != 0)
		{
			if (ret.bNoticeCode == NTC_MUST_CONFIRM)
			{
				sam_seq = (u_recv[6] << 24) + (u_recv[7] << 16) + (u_recv[8] << 8) + u_recv[9];
			}
		}
		else
		{
			sam_seq = (u_recv[6] << 24) + (u_recv[7] << 16) + (u_recv[8] << 8) + u_recv[9];
			if (p_trade_tac != NULL)
			{
				sprintf(sz_tac, "%02X%02X%02X%02X  ", u_recv[10], u_recv[11], u_recv[12], u_recv[13]);
				memcpy(p_trade_tac, sz_tac, 10);
			}
		}

	} while (0);

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		ret.bNoticeCode = support_confirm(operType);

	return ret;
}

uint16_t TicketJtb::write_card(uint8_t tac_type, uint8_t * p_app_ctrl, int record_id, uint8_t lock_flag, uint8_t * p_file_data, long& sam_seq)
{
	uint16_t ret				= ERR_CARD_WRITE;
	unsigned char u_recv[512]	= {0};

	do
	{
		if (set_app_ctrl(p_app_ctrl) != 0)
			break;

		if (svt_unlock_recordfile(SAM_SOCK_4, m_ReadInf.p_issue_base + 8, record_id, lock_flag, p_file_data + 3) != 0)
			break;

		if (svt_consum(CmdSort::m_time_now, SAM_SOCK_4, m_ReadInf.p_issue_base + 8, 0, tac_type, NULL , u_recv) != 0)
			break;

		sam_seq = (u_recv[6] << 24) + (u_recv[7] << 16) + (u_recv[8] << 8) + u_recv[9];

		ret = 0;

	} while (0);

	return ret;
}

uint16_t TicketJtb::set_trade_assist(uint8_t * p_trade_assist)
{
	uint16_t ret = 0;
	unsigned char u_recv[512] = {0};

	if (p_trade_assist != NULL)
	{
		p_trade_assist[2] = m_ticket_data.list_falg;

		if (m_ticket_data.list_falg == 0)
			ret = svt_unlock_recordfile(SAM_SOCK_4, m_ReadInf.p_issue_base + 8, 0x01, p_trade_assist[2], p_trade_assist + 3);
		else
			ret = svt_updata_complex(CmdSort::m_time_now, SAM_SOCK_4, m_ReadInf.p_issue_base + 8, 0x01, p_trade_assist[2], p_trade_assist + 3, u_recv);

		if (ret != 0)
			ret = ERR_CARD_WRITE;
	}

	return ret;
}

uint16_t TicketJtb::set_metro(uint8_t tac_type, uint8_t * p_metro, uint32_t lValue, uint8_t * p_respond)
{
	if (p_metro != NULL)
	{
		p_metro[4] = (p_metro[4] & 0x07) | (m_ticket_data.init_status << 3);

		p_metro[5] = (uint8_t)(m_ticket_data.p_entry_info->station_id >> 8);
		p_metro[6] = (uint8_t)(m_ticket_data.p_entry_info->station_id & 0xFF);

		p_metro[7] = (uint8_t)((m_ticket_data.p_entry_info->dvc_type << 4) + (m_ticket_data.p_entry_info->dvc_id >> 8));
		p_metro[8] = (uint8_t)(m_ticket_data.p_entry_info->dvc_id & 0x0F);

		memcpy(p_metro + 9, m_ticket_data.p_entry_info->time, sizeof(m_ticket_data.p_entry_info->time));


		p_metro[16] = (uint8_t)(m_ticket_data.write_last_info.station_id >> 8);
		p_metro[17] = (uint8_t)(m_ticket_data.write_last_info.station_id & 0xFF);

		p_metro[18] = (uint8_t)((m_ticket_data.write_last_info.dvc_type << 4) + (m_ticket_data.write_last_info.dvc_id >> 8));
		p_metro[19] = (uint8_t)(m_ticket_data.write_last_info.dvc_id & 0x0F);

		memcpy(p_metro + 20, m_ticket_data.write_last_info.time, sizeof(m_ticket_data.write_last_info.time));

		p_metro[27] = m_ticket_data.daliy_trade_counter;

		add_metro_area_crc(p_metro, LENM_METRO);
	}

	if (svt_consum(CmdSort::m_time_now, SAM_SOCK_4, m_ReadInf.p_issue_base + 8, lValue, tac_type, p_metro + 3, p_respond) != 0)
		return ERR_CARD_WRITE;

	return 0;
}

uint16_t TicketJtb::set_ctrl_record(uint8_t * p_ctrl_record)
{
	if (p_ctrl_record != NULL)
	{
		uint8_t u_recv[64] = {0};

		p_ctrl_record[2] = m_ticket_data.app_lock_flag;
		memcpy(p_ctrl_record + 4, m_ticket_data.logical_peroidS, 7);

		if (svt_updata_complex(CmdSort::m_time_now, SAM_SOCK_4, m_ReadInf.p_issue_base + 8, 0x11, 0, p_ctrl_record + 3, u_recv) != 0)
			return ERR_CARD_WRITE;
	}

	return 0;
}

uint16_t TicketJtb::set_app_ctrl(uint8_t * p_app_ctrl)
{
	uint16_t wallet_max = 0;

	if (p_app_ctrl != NULL)
	{
		p_app_ctrl[0] = m_ticket_data.globle_status;

		p_app_ctrl[1] = (uint8_t)((m_ticket_data.effect_mins >> 24) & 0xFF);
		p_app_ctrl[2] = (uint8_t)((m_ticket_data.effect_mins >> 16) & 0xFF);
		p_app_ctrl[3] = (uint8_t)((m_ticket_data.effect_mins >> 8) & 0xFF);
		p_app_ctrl[4] = (uint8_t)(m_ticket_data.effect_mins & 0xFF);

		p_app_ctrl[5] = (uint8_t)(m_ticket_data.deposit / 100);

		wallet_max = (uint16_t)(m_ticket_data.wallet_value_max / 100);
		p_app_ctrl[12] = (uint8_t)((wallet_max >> 8) & 0xFF);
		p_app_ctrl[13] = (uint8_t)(wallet_max & 0xFF);

		p_app_ctrl[14] = m_ticket_data.limit_mode;
		memcpy(p_app_ctrl + 15, m_ticket_data.limit_entry_station, 2);
		memcpy(p_app_ctrl + 17, m_ticket_data.limit_exit_station, 2);

		if (svt_update_binfile(SAM_SOCK_4, m_ReadInf.p_issue_base + 8, 0x11, 0, LENM_APP_CTRL, p_app_ctrl) != 0)
			return ERR_CARD_WRITE;
	}

	return 0;
}

// 将状态格式化为简化状态
SimpleStatus TicketJtb::get_simple_status(uint8_t init_status)
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

RETINFO TicketJtb::format_history(uint8_t&	his_count, PHSSVT p_his_array, int his_max)
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

bool TicketJtb::format_history(uint8_t * p_his_buffer, HSSVT& his)
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

RETINFO TicketJtb::lock(PTICKETLOCK p_lock)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;
	unsigned char lplogicnumber[10] = {0};
	unsigned char lissuenumber[8] = {0};

	METRO_JTBCPU_INF write_inf = file_need_write(operLock, &m_ReadInf);

	m_ticket_data.list_falg = '2'- p_lock->cLockFlag;

	memcpy(lplogicnumber, m_ReadInf.p_public_base + 10, 10);
	memcpy(lissuenumber, m_ReadInf.p_public_base, 8);

	if (m_ticket_data.list_falg == 0)
	{
		ret.wErrCode = write_card(0x06, NULL, 0x01, m_ticket_data.list_falg, write_inf.p_metro, lSamSeq);
	}
	else
	{
		ret = lock_card(lplogicnumber, lissuenumber, lSamSeq, NULL);
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

RETINFO TicketJtb::deffer(PTICKETDEFER p_deffer, TICKET_PARAM& ticket_prm)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;

	METRO_JTBCPU_INF write_inf = file_need_write(operDeffer, &m_ReadInf);

	memcpy(m_ticket_data.logical_peroidS, CmdSort::m_time_now, 7);
	m_ticket_data.effect_mins = ticket_prm.deffer_days * 24 * 60;

	if (write_inf.p_ctrl_record != NULL)
		memcpy(write_inf.p_ctrl_record + 4, m_ticket_data.logical_peroidS, 7);

	ret.wErrCode = write_card(0x06, write_inf.p_app_ctrl, 0x11, write_inf.p_ctrl_record[2], write_inf.p_ctrl_record, lSamSeq);
	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		ret.bNoticeCode = 0;

	p_deffer->lSAMTrSeqNo = lSamSeq;

	transfer_jtb_data_for_out(operDeffer, p_deffer);
	p_deffer->bStatus = LIFE_DEFFER;

	return ret;
}

RETINFO TicketJtb::set_complex_file(ETYTKOPER operType,uint8_t tac_type, long lAmount,uint8_t * p_metro,uint8_t * p_recv)
{
	int pos_file_buf			= 0;
	unsigned char u_count		= 0;
	unsigned char u_rid[8]		= {0};
	unsigned char u_lock[8]		= {0};
	unsigned char u_send[512]	= {0};
	RETINFO ret					= {0};
	uint8_t his_record[48]       = {0};
	long label_val				= 0;

	if (p_metro != NULL)
	{

		switch (operType)
		{
			case operUpdate:
				{
					if(m_ticket_data.init_status == MS_Upd_Wzone_Trip)
					{
						p_metro[14] = 0x04;//交易状态为出站更新;
						Publics::string_to_bcds(m_ticket_data.p_jtb_update_info->city_code, 4, &p_metro[15], 2);
						memcpy(p_metro + 19, m_ticket_data.p_jtb_update_info->institutionCode, 8);
						//memcpy(p_metro + 67, m_ticket_data.p_jtb_update_info->time, sizeof(m_ticket_data.p_jtb_update_info->time));
						//获取本地最低票价
						g_Parameter.query_lowest_fare(CmdSort::m_time_now, m_ticket_data.logical_type, label_val);
						p_metro[85] = m_ticket_data.p_jtb_entry_info->line_id;
						p_metro[87] = ((lAmount >> 24) & 0xFF);
						p_metro[88] = ((lAmount >> 16) & 0xFF);
						p_metro[89] = ((lAmount >> 8) & 0xFF);
						p_metro[90] = (lAmount & 0xFF);
						p_metro[91] = ((m_ticket_data.wallet_value >> 24) & 0xFF);
						p_metro[92] = ((m_ticket_data.wallet_value >> 16) & 0xFF);
						p_metro[93] = ((m_ticket_data.wallet_value >> 8) & 0xFF);
						p_metro[94] = (m_ticket_data.wallet_value & 0xFF);

						m_ticket_data.p_jtb_entry_info->station_id = Api::current_station_id;

						dbg_dumpmemory("p_metro(MS_Upd_Wzone_Time)|=",p_metro,128);
					}
					else if(m_ticket_data.init_status == MS_Upd_FZone_Fare)
					{
						p_metro[14] = 0x03;//交易状态为进站更新;
						Publics::string_to_bcds(m_ticket_data.p_jtb_update_info->city_code, 4, &p_metro[15], 2);
						memcpy(p_metro + 19, m_ticket_data.p_jtb_update_info->institutionCode, 8);
						//p_metro[35] = (uint8_t)(m_ticket_data.p_jtb_update_info->station_id >> 8);
						//p_metro[36] = (uint8_t)(m_ticket_data.p_jtb_update_info->station_id & 0xFF);
						p_metro[35] = (uint8_t)(m_ticket_data.p_jtb_entry_info->station_id >> 8);
						p_metro[36] = (uint8_t)(m_ticket_data.p_jtb_entry_info->station_id & 0xFF);
						memcpy(p_metro + 67, m_ticket_data.p_jtb_update_info->time, sizeof(m_ticket_data.p_jtb_update_info->time));
						//获取本地最低票价
						g_Parameter.query_lowest_fare(CmdSort::m_time_now, m_ticket_data.logical_type, label_val);
						p_metro[85] = m_ticket_data.p_jtb_entry_info->line_id;
						p_metro[87] = ((lAmount >> 24) & 0xFF);
						p_metro[88] = ((lAmount >> 16) & 0xFF);
						p_metro[89] = ((lAmount >> 8) & 0xFF);
						p_metro[90] = (lAmount & 0xFF);
						p_metro[91] = ((m_ticket_data.wallet_value >> 24) & 0xFF);
						p_metro[92] = ((m_ticket_data.wallet_value >> 16) & 0xFF);
						p_metro[93] = ((m_ticket_data.wallet_value >> 8) & 0xFF);
						p_metro[94] = (m_ticket_data.wallet_value & 0xFF);

						m_ticket_data.p_jtb_entry_info->station_id = Api::current_station_id;

						dbg_dumpmemory("p_metro(MS_Upd_FZone_Free)|=",p_metro,128);
					}
					else if(m_ticket_data.init_status == MS_Upd_FZone_Free)
					{
						p_metro[14] = 0x03;//交易状态为进站更新;
						Publics::string_to_bcds(m_ticket_data.p_jtb_update_info->city_code, 4, &p_metro[15], 2);
						memcpy(p_metro + 19, m_ticket_data.p_jtb_update_info->institutionCode, 8);
						p_metro[35] = (uint8_t)(m_ticket_data.p_jtb_entry_info->station_id >> 8);
						p_metro[36] = (uint8_t)(m_ticket_data.p_jtb_entry_info->station_id & 0xFF);
						memcpy(p_metro + 67, m_ticket_data.p_jtb_update_info->time, sizeof(m_ticket_data.p_jtb_update_info->time));
						//获取本地最低票价
						g_Parameter.query_lowest_fare(CmdSort::m_time_now, m_ticket_data.logical_type, label_val);
						p_metro[85] = m_ticket_data.p_jtb_entry_info->line_id;
						p_metro[87] = ((lAmount >> 24) & 0xFF);
						p_metro[88] = ((lAmount >> 16) & 0xFF);
						p_metro[89] = ((lAmount >> 8) & 0xFF);
						p_metro[90] = (lAmount & 0xFF);
						p_metro[91] = ((m_ticket_data.wallet_value >> 24) & 0xFF);
						p_metro[92] = ((m_ticket_data.wallet_value >> 16) & 0xFF);
						p_metro[93] = ((m_ticket_data.wallet_value >> 8) & 0xFF);
						p_metro[94] = (m_ticket_data.wallet_value & 0xFF);

						m_ticket_data.p_jtb_entry_info->station_id = Api::current_station_id;

						dbg_dumpmemory("p_metro(MS_Upd_FZone_Fare)|=",p_metro,128);
					}
					else if((m_ticket_data.init_status == MS_Upd_Wzone_Entry))
					{
						p_metro[14] = 0x01;//交易状态为已经进站;
						Publics::string_to_bcds(m_ticket_data.p_jtb_update_info->city_code, 4, &p_metro[15], 2);
						memcpy(p_metro + 19, m_ticket_data.p_jtb_update_info->institutionCode, 8);
						//p_metro[35] = (uint8_t)(m_ticket_data.p_jtb_update_info->station_id >> 8);
						//p_metro[36] = (uint8_t)(m_ticket_data.p_jtb_update_info->station_id & 0xFF);
						p_metro[35] = (uint8_t)(m_ticket_data.p_jtb_entry_info->station_id >> 8);
						p_metro[36] = (uint8_t)(m_ticket_data.p_jtb_entry_info->station_id & 0xFF);
						memcpy(p_metro + 67, m_ticket_data.p_jtb_update_info->time, sizeof(m_ticket_data.p_jtb_update_info->time));
						//获取本地最低票价
						g_Parameter.query_lowest_fare(CmdSort::m_time_now, m_ticket_data.logical_type, label_val);
						p_metro[85] = m_ticket_data.p_jtb_entry_info->line_id;
						p_metro[87] = ((lAmount >> 24) & 0xFF);
						p_metro[88] = ((lAmount >> 16) & 0xFF);
						p_metro[89] = ((lAmount >> 8) & 0xFF);
						p_metro[90] = (lAmount & 0xFF);
						p_metro[91] = ((m_ticket_data.wallet_value >> 24) & 0xFF);
						p_metro[92] = ((m_ticket_data.wallet_value >> 16) & 0xFF);
						p_metro[93] = ((m_ticket_data.wallet_value >> 8) & 0xFF);
						p_metro[94] = (m_ticket_data.wallet_value & 0xFF);
						dbg_dumpmemory("p_metro(MS_Upd_Wzone_Entry)|=",p_metro,128);
					}else if(m_ticket_data.init_status == MS_Upd_Wzone_Time)
					{
						p_metro[14] = 0x01;//交易状态为已经进站;
						memcpy(p_metro + 67, m_ticket_data.p_jtb_update_info->time, sizeof(m_ticket_data.p_jtb_update_info->time));
						g_Parameter.query_lowest_fare(CmdSort::m_time_now, m_ticket_data.logical_type, label_val);
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

					dbg_dumpmemory("p_metro(operEntry)|=",p_metro,128);
					//g_Record.log_buffer("p_metro(operEntry)|=",p_metro,128);
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

	if(p_metro != NULL)
	{

		if (lAmount == 0)
		{
			his_record[0] = 0x03;
			Publics::string_to_bcds(m_ticket_data.p_jtb_update_info->sam, 16, &his_record[1], 8);
			//g_Record.log_buffer("his_record(9)|= ", his_record,9);
			dbg_dumpmemory("his_record(9)|= ",his_record,9);
			his_record[9] = 0x01;//行业代码，01-地铁
			//g_Record.log_buffer("his_record(10)|= ", his_record,10);
			dbg_dumpmemory("his_record(10)|= ",his_record,10);
			his_record[10] = (uint8_t)(m_ticket_data.p_jtb_entry_info->station_id >> 8);
			//g_Record.log_buffer("his_record(12)|= ", his_record,12);
			dbg_dumpmemory("his_record(12)|= ",his_record,12);
			his_record[12] = (uint8_t)(m_ticket_data.p_jtb_entry_info->station_id & 0xFF);
			//g_Record.log_buffer("his_record(14)|= ", his_record,14);
			dbg_dumpmemory("his_record(14)|= ",his_record,14);

			his_record[17] = ((lAmount >> 24) & 0xFF);
			his_record[18] = ((lAmount >> 16) & 0xFF);
			his_record[19] = ((lAmount >> 8) & 0xFF);
			his_record[20] = (lAmount & 0xFF);
			//g_Record.log_buffer("his_record(21)|= ", his_record,21);

			his_record[21] = ((m_ticket_data.wallet_value >> 24) & 0xFF);
			his_record[22] = ((m_ticket_data.wallet_value >> 16) & 0xFF);
			his_record[23] = ((m_ticket_data.wallet_value >> 8) & 0xFF);
			his_record[24] = (m_ticket_data.wallet_value & 0xFF);

			//g_Record.log_buffer("his_record(25)|= ", his_record,25);

			memcpy(his_record + 25, m_ticket_data.p_jtb_update_info->time, sizeof(m_ticket_data.p_jtb_update_info->time));

			//g_Record.log_buffer("his_record(33)|= ", his_record,33);

			Publics::string_to_bcds(m_ticket_data.p_jtb_update_info->city_code, 4, &his_record[32], 2);
			//g_Record.log_buffer("his_record(35)|= ", his_record,35);
			memcpy(his_record + 34, m_ticket_data.p_jtb_update_info->institutionCode, 8);
			//g_Record.log_buffer("his_record(43)|= ", his_record,43);
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
		// 消费并产生TAC时出错
		if (ret.wErrCode == 4)		ret.bNoticeCode = NTC_MUST_CONFIRM;
		ret.wErrCode = ERR_CARD_WRITE;
	}

	return ret;
}

void TicketJtb::save_last_stack(ETYTKOPER operType, void * p_trade, size_t size_trade, void * p_written_inf, size_t size_written, bool save_to_file)
{
	METRO_CPU_INF_W written_info = {0};
	METRO_JTBCPU_INF * p_old_write = (METRO_JTBCPU_INF *)p_written_inf;

	if (p_written_inf == NULL)
	{
		TicketBase::save_last_stack(operType, p_trade, size_trade, NULL, 0, save_to_file);
	}
	else
	{
		if (p_old_write->p_trade_assist != NULL)
		{
			written_info.flag_trade_assist = 1;
			memcpy(written_info.w_trade_assist, p_old_write->p_trade_assist, LENM_TRADE_ASSIST);
		}

		if (p_old_write->p_metro != NULL)
		{
			written_info.flag_metro = 1;
			memcpy(written_info.w_metro, p_old_write->p_metro, LENM_METRO);
		}

		if (p_old_write->p_ctrl_record != NULL)
		{
			written_info.flag_ctrl_record = 1;
			memcpy(written_info.w_ctrl_record, p_old_write->p_ctrl_record, LENM_CTRL_RECORD);
		}

		if (p_old_write->p_app_ctrl != NULL)
		{
			written_info.flag_app_ctrl = 1;
			memcpy(written_info.w_app_ctrl, p_old_write->p_app_ctrl, LENM_APP_CTRL);
		}

		TicketBase::save_last_stack(operType, p_trade, size_trade, &written_info, sizeof(written_info), save_to_file);
	}
}

bool TicketJtb::last_trade_need_continue(uint8_t status_targ, uint16_t err_targ, uint16_t err_src)
{
	bool ret = false;

	METRO_CPU_INF_W * p_old_write = (METRO_CPU_INF_W *)cfm_point.dataWritten;

	do
	{
		CONFIRM_FACTOR factor = get_confirm_factor();

		if (!check_confirm(status_targ, m_ticket_data.init_status, err_targ, err_src)){
			g_Record.log_out(0, level_disaster,"last_trade_need_continue check_confirm not need continue");
			break;
		}

		// 直接判断复合消费是否成功，如果成功则需要继续去流水和TAC，否则认为失败
		if (p_old_write->flag_trade_assist > 0)
		{
			if (memcmp(p_old_write->w_trade_assist, m_ReadInf.p_trade_assist, LENM_TRADE_ASSIST) != 0)	break;
		}

		if (p_old_write->flag_metro > 0)
		{
			if (memcmp(p_old_write->w_metro, m_ReadInf.p_metro, LENM_METRO) != 0)	break;
		}

		if (p_old_write->flag_ctrl_record > 0)
		{
			if (memcmp(p_old_write->w_ctrl_record, m_ReadInf.p_ctrl_record, LENM_CTRL_RECORD) != 0)	break;
		}

		if (cfm_point.balance != m_ticket_data.wallet_value)	break;

		if (cfm_point.operType == operIncr)
		{
			if (m_ticket_data.charge_counter != cfm_point.charge_counter + 1 ||	cfm_point.consume_counter != m_ticket_data.total_trade_counter)	break;
		}
		else
		{
			if (m_ticket_data.charge_counter != cfm_point.charge_counter ||	m_ticket_data.total_trade_counter != cfm_point.consume_counter + 1)	break;
		}

		ret = true;
		g_Record.log_out(0, level_normal, "need confirm");

	} while (0);
	g_Record.log_out(0, level_normal, "need confirm? (%d)",ret);
    return ret;
}

RETINFO TicketJtb::continue_last_trade(void * p_trade)
{
	RETINFO ret			= {0};
	char sz_tac[11]		= {0};
	uint8_t u_recv[32]	= {0};
	uint16_t counter	= 0;

	do
	{
		CONFIRM_FACTOR factor = get_confirm_factor();

		if (cfm_point.operType == operIncr)
		{
			counter = (uint16_t)(m_ticket_data.charge_counter);
		}
		else if (cfm_point.operType == operDecr)
		{
			counter = (uint16_t)(m_ticket_data.total_trade_counter);
		}
		else
		{
			counter = (uint16_t)(m_ticket_data.total_trade_counter);
		}

		// 要用本次的交易计数
		if (get_cpu_last_tac(counter, factor.tac_type, u_recv) != 0)
		{
			ret.wErrCode = ERR_CARD_WRITE;
			break;
		}

		sprintf(sz_tac, "%02X%02X%02X%02X  ", u_recv[0], u_recv[1], u_recv[2], u_recv[3]);

		set_confirm_factor(LIFE_CONFIRM, 0, sz_tac);

		memcpy(p_trade, &cfm_point.trade, cfm_point.size_trade);

	} while (0);

	g_Record.log_out(ret.wErrCode, level_error, "continue_last_trade(void * p_trade)=%d", ret.wErrCode);

	return ret;
}

// 赋值给上次交易未赋值的交易因子
//void TicketSvt::set_confirm_factor(uint8_t status, long sam_seq, char * p_tac)
//{
//	uint8_t trade_type = Publics::string_to_hex<uint8_t>((char *)(&cfm_point.trade), 2);
//	switch(trade_type)
//	{
//	//case 0x50:	// 单程票发售
//	//	cfm_point.trade.sjtSale.bStatus		= status;
//	//	memcpy(cfm_point.trade.sjtSale.cMACorTAC, p_tac, 10);
//	//	break;
//	case 0x51:	// 储值票发售
//		cfm_point.trade.svtSale.bStatus		= status;
//		break;
//	case 0x53:	// 进闸
//		cfm_point.trade.entry.bStatus		= status;
//		break;
//	case 0x54:	// 钱包交易
//		cfm_point.trade.purse.bStatus		= status;
//		memcpy(cfm_point.trade.purse.cMACorTAC, p_tac, 10);
//		break;
//	case 0x56:	// 更新
//		cfm_point.trade.update.bStatus		= status;
//		break;
//	case 0x57:	// 退款
//		cfm_point.trade.refund.bStatus		= status;
//		memcpy(cfm_point.trade.refund.cMACOrTAC, p_tac, 10);
//		break;
//	}
//}

RETINFO TicketJtb::data_version_upgrade()
{
	RETINFO ret		            = {0};
	unsigned char u_recv[256]	= {0};

	if (svt_unlock_recordfile(SAM_SOCK_1, m_ReadInf.p_issue_base + 8, 0x11, 0x00, u_recv) != 0)
		ret.wErrCode = ERR_CARD_WRITE;

	if (svt_consum(CmdSort::m_time_now, SAM_SOCK_1, m_ReadInf.p_issue_base + 8, m_ticket_data.wallet_value, 0x06, u_recv, u_recv) != 0)
		ret.wErrCode = ERR_CARD_WRITE;

	return ret;
}

uint16_t TicketJtb::read_wallet(long * p_wallet)
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

		if (svt_getbalance(tmp,0x03) < 0)
		{
			ret = ERR_CARD_READ;
			break;
		}

		*p_wallet = (tmp[0] << 24) + (tmp[1] << 16) + (tmp[2] << 8) + tmp[3];

	} while (0);

	return ret;
}
