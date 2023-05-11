#include "TicketSvt.h"
#include "Errors.h"
#include "Publics.h"
#include "TimesEx.h"


TicketSvt::TicketSvt(void)
{

}

TicketSvt::TicketSvt(char * p_current_sam, char * p_sam_posid, uint8_t * physic_info)
{
	m_p_current_sam = p_current_sam;
	m_p_sam_posid = p_sam_posid;

	memset(&m_ticket_data, 0, sizeof(m_ticket_data));
	memset(&m_ReadInf, 0, sizeof(m_ReadInf));

	sprintf(m_ticket_data.physical_id, "%02X%02X%02X%02X%02X%02X%02X%-6C",
		physic_info[0], physic_info[1], physic_info[2], physic_info[3], physic_info[4], physic_info[5], physic_info[6], 0x20);

	uint8_t * pMemoryPos = ((uint8_t *)this) + sizeof(TicketSvt);
	m_ticket_data.p_entry_info = new(pMemoryPos) TradeInfo;

	m_ticket_data.p_exit_info = &m_ticket_data.read_last_info;
	m_ticket_data.p_update_info = &m_ticket_data.read_last_info;

	memset(&m_ticket_data.read_last_info, 0, sizeof(TradeInfo));
}

TicketSvt::~TicketSvt(void)
{

}

uint16_t TicketSvt::metro_svt_read(P_METRO_CPU_INF p_mtr_inf)
{
	uint16_t u_ret			= 0;

	do
	{
		// 选择主目录
		if (svt_selectfile(0x3F00) < 0)
		{
			u_ret = ERR_CARD_READ;
			g_Record.log_out(u_ret, level_error, "select file 3F00 failed");
			break;
		}

		if (p_mtr_inf->p_issue_base != NULL)
		{
			// 读发行基本信息
			if (svt_readbinary(0x05, 0, LENM_ISSUE_BASE, p_mtr_inf->p_issue_base) < 0)
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read binary file 05 failed");
				break;
			}
		}

		// 选择ADF1
		if (svt_selectfile(0x1001) < 0)
		{
			if (svt_selecfileaid(9, (unsigned char *)"\xA0\x00\x00\x00\x03\x86\x98\x07\x01") < 0)
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "select file 3F00 failed and select file aid failed");
				break;
			}
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

		// 读持卡人信息
		if (p_mtr_inf->p_owner_base != NULL)
		{
			if (svt_readbinary(0x16, 0, LENM_OWNER_BASE, p_mtr_inf->p_owner_base) < 0)
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read binary file 16 failed");
				break;
			}
		}

		// 钱包
		if (p_mtr_inf->p_wallet != NULL)
		{
			if (svt_getbalance(p_mtr_inf->p_wallet) < 0)
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read wallet failed");
				break;
			}
		}

		// 所有历史
		if (p_mtr_inf->p_his_all != NULL)
		{
			if (svt_readhistory(0x18, 1, 10, p_mtr_inf->p_his_all) < 0)		// 本地
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read history 18 failed");
				break;
			}
			if (svt_readhistory(0x10, 1, 10, p_mtr_inf->p_his_all + 0x17 * 10) < 0)		// 异地
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read history 18 failed");
				break;
			}
			if (svt_readhistory(0x1A, 1, 10, p_mtr_inf->p_his_all + 0x17 * 20) < 0)		// 充值
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read history 1a failed");
				break;
			}
		}

		// 交易辅助
		if (p_mtr_inf->p_trade_assist != NULL)
		{
			if (svt_readrecord(0x17, 1, LENM_TRADE_ASSIST, p_mtr_inf->p_trade_assist) < 0)
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read complex file 17-1 failed");
				break;
			}
		}

		// 轨道交通
		if (p_mtr_inf->p_metro != NULL)
		{
			if (svt_readrecord(0x17, 2, LENM_METRO, p_mtr_inf->p_metro) < 0)
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read complex file 17-2 failed");
				break;
			}
		}

		// 应用控制记录
		if (p_mtr_inf->p_ctrl_record != NULL)
		{
			if (svt_readrecord(0x17, 0x11, LENM_CTRL_RECORD, p_mtr_inf->p_ctrl_record) < 0)
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read complex file 17-11 failed");
				break;
			}
		}

		// 读应用控制文件
		if (p_mtr_inf->p_app_ctrl != NULL)
		{
			if (svt_readbinary(0x11, 0, LENM_APP_CTRL, p_mtr_inf->p_app_ctrl) < 0)
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read binary file 11 failed");
				break;
			}
		}

		// 充值计数
		if (p_mtr_inf->p_charge_count != NULL)
		{
			if (svt_getonlineserial(SAM_SOCK_1, p_mtr_inf->p_charge_count) < 0)
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read on-line counter failed");
				break;
			}
		}

		// 交易计数
		if (p_mtr_inf->p_consume_count != NULL)
		{
			if (svt_getofflineserial(SAM_SOCK_1, p_mtr_inf->p_consume_count) < 0)
			{
				u_ret = ERR_CARD_READ;
				g_Record.log_out(u_ret, level_error, "read off-line counter failed");
				break;
			}
		}

	} while (0);

	return u_ret;
}

RETINFO TicketSvt::read_card(ETYTKOPER operType, TICKET_PARAM& ticket_prm, uint8_t * p_read_init/* = NULL*/)
{
	RETINFO ret				= {0};

	do
	{
		memset(&m_u_buffer, 0, sizeof(m_u_buffer));
		m_ReadInf = file_need_read(operType, m_u_buffer);

		//g_Record.log_out(ret.wErrCode, level_error, "metro_svt_read");

		ret.wErrCode = metro_svt_read(&m_ReadInf);
		if (ret.wErrCode != 0)	break;

		//
		// 发行基本信息
		//
		get_issue_base(m_ReadInf.p_issue_base);
		//
		// 公共信息
		//
		get_public_base(m_ReadInf.p_public_base);
		//
		// 持卡人信息
		//
		get_owner_base(m_ReadInf.p_owner_base);
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
		// 所有历史
		//
		get_his_all(m_ReadInf.p_his_all);
		//
		// 交易辅助信息
		//
		get_trade_assist(m_ReadInf.p_trade_assist);
		//
		// 地铁信息
		//
		get_metro(m_ReadInf.p_metro);
		//
		// 应用控制记录
		//
		get_ctrl_record(m_ReadInf.p_ctrl_record);
		//
		// 应用控制信息
		//
		get_app_ctrl(m_ReadInf.p_app_ctrl);

		// 当应用控制纪录未锁定，且不是多日票，状态格式化为初始化
		if (m_ticket_data.app_lock_flag == 0)
		{
			m_ticket_data.init_status = MS_Init;
		}

		m_ticket_data.simple_status = get_simple_status(m_ticket_data.init_status);

	} while (0);

	return ret;
}

uint8_t TicketSvt::unified_status()
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

RETINFO TicketSvt::analyse_common(uint8_t wk_area, MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PTICKETLOCK p_lock, uint16_t& lenlock, PBOMANALYZE p_analyse, char * p_mode)
{
	RETINFO ret				= {0};
	char mode				= '0';

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

		// 黑名单
		bool in_prm = g_Parameter.query_black_mtr(m_ticket_data.logical_id);
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

		if (g_Parameter.permit_sale(p_ticket_prm->sell_device))
		{
			if (m_ticket_data.simple_status == SStatus_Init)
			{
				ret.wErrCode = ERR_WITHOUT_SELL;
				if (wk_area == 2 && p_analyse != NULL && m_ticket_data.wallet_value == 0)
					p_analyse->dwOperationStauts |= ALLOW_SALE;

				break;
			}
		}

		// 检查余额
		if (m_ticket_data.wallet_value > p_ticket_prm->balance_max)
		{
			if (p_degrade_falgs->train_trouble)
			{
				mode = (char)(0x30 + degrade_Trouble);
			}
			else
			{
				ret.wErrCode = ERR_OVER_WALLET;
				break;
			}
		}

	} while (0);

	if (p_mode != NULL)
		*p_mode = mode;

	return ret;
}

RETINFO TicketSvt::analyse_work_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PPURSETRADE p_purse)
{
	RETINFO ret				= {0};
	ret = TicketBase::analyse_work_zone(p_degrade_falgs, p_ticket_prm, p_analyse, p_purse);
	return ret;
}

RETINFO TicketSvt::analyse_free_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse)
{
	RETINFO ret				= {0};

	ret = TicketBase::analyse_free_zone(p_degrade_falgs, p_ticket_prm, p_analyse);

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

uint32_t TicketSvt::add_oper_status(uint8_t wk_area, RETINFO ret, TICKET_PARAM * p_ticket_prm)
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

RETINFO TicketSvt::entry_gate(PENTRYGATE p_entry)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;

	METRO_CPU_INF write_inf = file_need_write(operEntry, &m_ReadInf);

	m_ticket_data.init_status = MS_Entry;
	m_ticket_data.p_entry_info = &m_ticket_data.write_last_info;
	current_trade_info(*m_ticket_data.p_entry_info);

	ret = write_card(operEntry, write_inf, 0x09, 0, lSamSeq, NULL);
	p_entry->lSAMTrSeqNo = lSamSeq;

	transfer_data_for_out(operEntry, p_entry);
	p_entry->bStatus = LIFE_ENTRY;

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
	{
		save_last_stack(operEntry, p_entry, sizeof(ENTRYGATE), &write_inf, sizeof(write_inf), false);
	}
	return ret;
}

RETINFO TicketSvt::exit_gate(PPURSETRADE p_exit, MODE_EFFECT_FLAGS * p_degrade_falgs)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;
	uint8_t tacType			= 0x09;

	METRO_CPU_INF write_inf = file_need_write(operExit, &m_ReadInf);

	if (p_degrade_falgs->train_trouble)
		m_ticket_data.init_status = MS_Exit_Tt;
	else
		m_ticket_data.init_status = MS_Exit;
	m_ticket_data.p_exit_info = &m_ticket_data.write_last_info;
	m_ticket_data.wallet_value -= p_exit->lTradeAmount;

	current_trade_info(*m_ticket_data.p_exit_info);

	ret = write_card(operExit, write_inf, tacType, p_exit->lTradeAmount, lSamSeq, p_exit->cMACorTAC);
	p_exit->lSAMTrSeqNo = lSamSeq;

	transfer_data_for_out(operExit, p_exit);
	if (m_ticket_data.simple_status == SStatus_Upd_WZone)
		memcpy(p_exit->cPaymentType, "1C", sizeof(p_exit->cPaymentType));
	else
		memcpy(p_exit->cPaymentType, "12", sizeof(p_exit->cPaymentType));
	Publics::hex_to_two_char(tacType, p_exit->cClassicType);

	p_exit->bStatus = LIFE_EXIT;

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
	{
		save_last_stack(operExit, p_exit, sizeof(PURSETRADE), &write_inf, sizeof(write_inf), false);
	}

	return ret;
}

RETINFO TicketSvt::sale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, POTHERSALE p_svt_sale, uint16_t& len_svt, TICKET_PARAM& ticket_prm)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;
	len_sjt					= 0;
	len_svt					= 0;

	METRO_CPU_INF write_inf = file_need_write(operSvtSale, &m_ReadInf);

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
	transfer_data_for_out(operSvtSale, p_svt_sale);
	p_svt_sale->bStatus = LIFE_SALE;

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
	{
		save_last_stack(operSvtSale, p_svt_sale, sizeof(OTHERSALE), &write_inf, sizeof(write_inf), true);
	}
	return ret;
}

RETINFO TicketSvt::bom_update(PTICKETUPDATE p_update, uint8_t * p_entry_station)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;
	long fare				= 0;

	METRO_CPU_INF write_inf = file_need_write(operUpdate, &m_ReadInf);

	switch (p_update->bUpdateReasonCode)
	{
	case 0x01:		// 付费区超时
		m_ticket_data.init_status = MS_Upd_Wzone_Time;
		p_update->bStatus = LIFE_UPD_TM_OUT;
		g_Record.log_out(0, level_disaster, "1 MS_Upd_Wzone_Time");
		break;
	case 0x02:		// 付费区超乘
		m_ticket_data.init_status = MS_Upd_Wzone_Trip;
		p_update->bStatus = LIFE_UPD_TP_OUT;
		g_Record.log_out(0, level_disaster, "2 MS_Upd_Wzone_Trip");
		break;
	case 0x03:		// 付费区无进站码
		m_ticket_data.p_entry_info->station_id = (p_entry_station[0] << 8) + p_entry_station[1];
		m_ticket_data.init_status = MS_Upd_Wzone_Entry;
		p_update->bStatus = LIFE_UPD_WO_STA;
		g_Record.log_out(0, level_disaster, "3 MS_Upd_Wzone_Entry");
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
		g_Record.log_out(0, level_disaster, "4 MS_Upd_FZone_Free");
		break;
	case 0x11:
	case 0x12:		// 非付费区付费更新
		m_ticket_data.init_status = MS_Upd_FZone_Fare;
		p_update->bStatus = LIFE_UPD_OUT_FARE;
		g_Record.log_out(0, level_disaster, "5 MS_Upd_FZone_Fare");
		break;
	default:
		ret.wErrCode = ERR_INPUT_PARAM;
		break;
	}

	if (ret.wErrCode == 0)
	{
		m_ticket_data.p_update_info = &m_ticket_data.write_last_info;
		current_trade_info(*m_ticket_data.p_update_info);

		ret = write_card(operUpdate, write_inf, 0x09, 0, lSamSeq, NULL);
		p_update->lSAMTrSeqNo = lSamSeq;

		// 交易记录赋值
		transfer_data_for_out(operUpdate, p_update);

		if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		{
			save_last_stack(operUpdate, p_update, sizeof(TICKETUPDATE), &write_inf, sizeof(write_inf), true);
		}
	}

	return ret;
}

RETINFO TicketSvt::bom_refund(PDIRECTREFUND p_refund)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;
	uint8_t tacType			= 0x09;

	METRO_CPU_INF write_inf = file_need_write(operRefund, &m_ReadInf);

	m_ticket_data.init_status = MS_Refund;
	m_ticket_data.wallet_value -= p_refund->lBalanceReturned;
	m_ticket_data.deposit -= p_refund->nDepositReturned;
	current_trade_info(m_ticket_data.write_last_info);

	m_ticket_data.globle_status = EXTERN_STATUS_RFND;	//已退款

	ret = write_card(operRefund, write_inf, tacType, p_refund->lBalanceReturned, lSamSeq, p_refund->cMACOrTAC);
	p_refund->lSAMTrSeqNo = lSamSeq;

	transfer_data_for_out(operRefund, p_refund);
	p_refund->bReturnTypeCode = '0';
	p_refund->bStatus = LIFE_REFUND;
	Publics::hex_to_two_char(tacType, p_refund->cClassicType);

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
	{
		save_last_stack(operRefund, p_refund, sizeof(DIRECTREFUND), &write_inf, sizeof(write_inf), true);
	}

	return ret;
}

RETINFO TicketSvt::svt_increase(PPURSETRADE p_purse, uint8_t * p_time, uint8_t * p_mac2)
{
	RETINFO ret					= {0};
	uint8_t time_mac[16]		= {0};
	long wallet_after			= 0;
	uint8_t tacType				= 0x02;
	uint8_t u_recv[32]	= {0};
	uint16_t counter	= 0;
	uint16_t read_ret = 0;

	do
	{
		memcpy(time_mac, p_time, 7);
		memcpy(time_mac + 7, p_mac2, 4);

		m_ticket_data.wallet_value += p_purse->lTradeAmount;


		transfer_data_for_out(operIncr, p_purse);
		memcpy(p_purse->dtDate, p_time, 7);
		p_purse->bStatus = LIFE_CHARGE;
		Publics::hex_to_two_char(tacType, p_purse->cClassicType);

		g_Record.log_out(0, level_error, "tacType() = %02X  lTradeAmount = %ld ", tacType,p_purse->lTradeAmount);
		g_Record.log_out(0, level_error, "time_mac() = %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", time_mac[0], time_mac[1], time_mac[2], time_mac[3], time_mac[4], time_mac[5], time_mac[6], time_mac[7], time_mac[8], time_mac[9], time_mac[10]);

		ret.wErrCode = svt_recharge(tacType, p_time, SAM_SOCK_1, 1, p_purse->lTradeAmount, time_mac);
		g_Record.log_out(ret.wErrCode, level_error, "svt_recharge() = %04X", ret.wErrCode);

		g_Record.log_out(0, level_error, "time_mac(svt_recharge_end) = %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", time_mac[0], time_mac[1], time_mac[2], time_mac[3], time_mac[4], time_mac[5], time_mac[6], time_mac[7], time_mac[8], time_mac[9], time_mac[10]);

		for (int i=0;i<5;i++)
		{
			read_ret = read_wallet(&wallet_after);
			if (read_ret == 0)
				break;

			usleep(500 * 1000);
		}

		if (read_ret == 0)	// 回读余额正确的时候需要比较余额
		{
			if (m_ticket_data.wallet_value != wallet_after)		// 月不相等返回失败
			{
				ret.wErrCode = ERR_CARD_WRITE;
				break;
			}
			else
			{
				char sz_mac[11] = {0};
				if(ret.wErrCode!=0)
				{
					//回读余额与预期充值后余额相同，认为充值成功，重取一次TAC
					counter = (uint16_t)(m_ticket_data.total_trade_counter) + 1;
					if (get_cpu_last_tac(counter, tacType, u_recv)==0)
						sprintf(sz_mac, "%02X%02X%02X%02X  ", u_recv[0], u_recv[1], u_recv[2], u_recv[3]);
					else
						sprintf(sz_mac, "%02X%02X%02X%02X  ", time_mac[0], time_mac[1], time_mac[2], time_mac[3]);

					g_Record.log_out(0, level_error, "sz_mac1 = %02X%02X%02X%02X", sz_mac[0], sz_mac[1], sz_mac[2], sz_mac[3]);
				}
				else 
				{
					sprintf(sz_mac, "%02X%02X%02X%02X  ", time_mac[0], time_mac[1], time_mac[2], time_mac[3]);
					g_Record.log_out(0, level_error, "sz_mac2 = %02X%02X%02X%02X", sz_mac[0], sz_mac[1], sz_mac[2], sz_mac[3]);
				}
				memcpy(p_purse->cMACorTAC, sz_mac, 10);
			}
		}
		else	// 回读余额错误就需要外部人为干涉，应返回交易确认
		{
			g_Record.log_out(ret.wErrCode, level_error, "after charge, svt_getbalance failed");

			ret.wErrCode = ERR_CARD_WRITE;
			if (Api::current_device_type == dvcBOM)
				ret.bNoticeCode = NTC_MUST_CONFIRM;
		}

	} while (0);

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
	{
		save_last_stack(operIncr, p_purse, sizeof(PURSETRADE), NULL, 0, true);
	}

	return ret;
}

RETINFO TicketSvt::svt_decrease(PPURSETRADE p_purse, uint8_t u_pay_flag)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;
	uint8_t tacType			= 0x06;

	if (p_purse->lTradeAmount > m_ticket_data.wallet_value)
	{
		ret.wErrCode = ERR_LACK_WALLET;
	}
	else
	{
		METRO_CPU_INF write_inf = file_need_write(operDecr, &m_ReadInf);
		m_ticket_data.wallet_value -= p_purse->lTradeAmount;

		ret = write_card(operDecr, write_inf, tacType, p_purse->lTradeAmount, lSamSeq, p_purse->cMACorTAC);
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

METRO_CPU_INF TicketSvt::file_need_read(ETYTKOPER type, uint8_t * p_buffer)
{
	int ptr_buffer			= 0;
	METRO_CPU_INF ret_inf	= {0};

	ret_inf.p_issue_base = p_buffer + ptr_buffer;
	ptr_buffer += LENM_ISSUE_BASE;

	//p_mtr_inf->p_public_base = p_buffer + ptr_buffer;
	//ptr_buffer += LENM_PUBLIC_BASE;

	ret_inf.p_wallet = p_buffer + ptr_buffer;
	ptr_buffer += LENM_WALLET;

	ret_inf.p_trade_assist = p_buffer + ptr_buffer;
	ptr_buffer += LENM_TRADE_ASSIST;

	ret_inf.p_metro = p_buffer + ptr_buffer;
	ptr_buffer += LENM_METRO;

	ret_inf.p_ctrl_record = p_buffer + ptr_buffer;
	ptr_buffer += LENM_CTRL_RECORD;

	ret_inf.p_app_ctrl = p_buffer + ptr_buffer;
	ptr_buffer += LENM_APP_CTRL;

	ret_inf.p_charge_count = p_buffer + ptr_buffer;
	ptr_buffer += LENM_CHANGE_CNT;

	ret_inf.p_consume_count = p_buffer + ptr_buffer;
	ptr_buffer += LENM_CONSUME_CNT;

	switch (type)
	{
	case operAnalyse:
	case operSale:
		ret_inf.p_owner_base = p_buffer + ptr_buffer;
		ptr_buffer += LENM_OWNER_BASE;

		break;

	case operTkInf:
		ret_inf.p_owner_base = p_buffer + ptr_buffer;
		ptr_buffer += LENM_OWNER_BASE;

		ret_inf.p_his_all = p_buffer + ptr_buffer;
		ptr_buffer += LENM_HIS_ALL;
		break;

	case operUpdate:
	case operSvtSale:
	case operLock:
	case operDeffer:
	case operIncr:
	case operRefund:
	case operEntry:
	case operExit:
	case operDecr:
		break;
    default:
        break;
	}

	return ret_inf;
}

METRO_CPU_INF TicketSvt::file_need_write(ETYTKOPER type, P_METRO_CPU_INF p_read_inf)
{
	METRO_CPU_INF ret_inf = {0};

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
		ret_inf.p_metro = p_read_inf->p_metro;
		break;
    default:
        break;
	}

	return ret_inf;
}

void TicketSvt::get_issue_base(uint8_t * p_issue_base)
{
	if (p_issue_base != NULL)
	{
		// 发卡方代码
		Publics::bcds_to_string(p_issue_base, 2, m_ticket_data.issue_code, 4);

		// 城市代码、行业代码
		Publics::bcds_to_string(p_issue_base + 2, 2, m_ticket_data.city_code, 4);
		Publics::bcds_to_string(p_issue_base + 4, 2, m_ticket_data.industry_code, 4);

		// 应用标识
		m_ticket_data.key_flag = p_issue_base[6];

		// 逻辑卡号
		sprintf(m_ticket_data.logical_id, "0000%02X%02X%02X%02X%02X%02X%02X%02X",
			p_issue_base[8], p_issue_base[9], p_issue_base[10], p_issue_base[11],
			p_issue_base[12], p_issue_base[13], p_issue_base[14], p_issue_base[15]);

		// 卡类型
		memcpy(m_ticket_data.logical_type, p_issue_base + 16, 2);
		memcpy(m_ticket_data.fare_type, m_ticket_data.logical_type, 2);

		// 发行日期
		memcpy(m_ticket_data.date_issue, p_issue_base + 18, 4);

		// 物理有效期
		memcpy(m_ticket_data.phy_peroidE, p_issue_base + 34, 4);
	}
}

void TicketSvt::get_public_base(uint8_t * p_public_base)
{
	//if (p_public_base != NULL)
	//{

	//}
}

void TicketSvt::get_owner_base(uint8_t * p_owner_base)
{
	if (p_owner_base != NULL)
	{
		memcpy(m_ticket_data.certificate_name, p_owner_base + 2, sizeof(m_ticket_data.certificate_name));//姓名128
		memcpy(m_ticket_data.certificate_code, p_owner_base + 130, sizeof(m_ticket_data.certificate_code));
		m_ticket_data.certificate_type = p_owner_base[162];
	}
}

void TicketSvt::get_wallet(uint8_t * p_wallet)
{
	if (p_wallet != NULL)
	{
		m_ticket_data.wallet_value = (p_wallet[0] << 24) + (p_wallet[1] << 16) + (p_wallet[2] << 8) + p_wallet[3];
	}
}

void TicketSvt::get_his_all(uint8_t * p_his_all)
{
	if (p_his_all != NULL)
	{

	}
}

void TicketSvt::get_trade_assist(uint8_t * p_trade_assist)
{
	if (p_trade_assist != NULL)
	{
		m_ticket_data.list_falg = p_trade_assist[2];
	}
}

void TicketSvt::get_metro(uint8_t * p_metro)
{
	if (p_metro != NULL)
	{
		if (!metro_area_crc_valid(p_metro, LENM_METRO))
		{
			memset(p_metro + 3, 0, LENM_METRO - 3);		// 前3字节不能变动
			p_metro[4] = (uint8_t)(MS_Sale << 3);
		}

		m_ticket_data.init_status = p_metro[4] >> 3;

		m_ticket_data.p_entry_info->station_id = (p_metro[5] << 8) + p_metro[6];

		m_ticket_data.p_entry_info->dvc_type = (ETPDVC)(p_metro[7] >> 4);

		m_ticket_data.p_entry_info->dvc_id = ((p_metro[7] & 0x0F) << 8) + p_metro[8];

		memcpy(m_ticket_data.p_entry_info->time, p_metro + 9, sizeof(m_ticket_data.p_entry_info->time));

		g_Parameter.sam_from_device(m_ticket_data.p_entry_info->station_id, m_ticket_data.p_entry_info->dvc_type,
			m_ticket_data.p_entry_info->dvc_id, SAM_SOCK_1, m_ticket_data.p_entry_info->sam);

		m_ticket_data.read_last_info.station_id = (p_metro[16] << 8) + p_metro[17];

		m_ticket_data.read_last_info.dvc_type = (ETPDVC)(p_metro[18] >> 4);

		m_ticket_data.read_last_info.dvc_id = ((p_metro[18] & 0x0F) << 8) + p_metro[19];

		memcpy(m_ticket_data.read_last_info.time, p_metro + 20, sizeof(m_ticket_data.read_last_info.time));

		g_Parameter.sam_from_device(m_ticket_data.read_last_info.station_id, m_ticket_data.read_last_info.dvc_type,
			m_ticket_data.read_last_info.dvc_id, SAM_SOCK_1, m_ticket_data.read_last_info.sam);

		m_ticket_data.daliy_trade_counter = p_metro[27];
	}
}

void TicketSvt::get_ctrl_record(uint8_t * p_ctrl_record)
{
	if (p_ctrl_record != NULL)
	{
		m_ticket_data.app_lock_flag = p_ctrl_record[2];
		memcpy(m_ticket_data.logical_peroidS, p_ctrl_record + 4, 7);
		memcpy(m_ticket_data.logical_peroidE, m_ticket_data.logical_peroidS, 7);
	}
}

void TicketSvt::get_app_ctrl(uint8_t * p_app_ctrl)
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

void TicketSvt::get_change_count(uint8_t * p_charge_count)
{
	if (p_charge_count != NULL)
	{
		m_ticket_data.charge_counter = (p_charge_count[0] << 8) + p_charge_count[1];
	}
}

void TicketSvt::get_consume_count(uint8_t * p_consume_count)
{
	if (p_consume_count != NULL)
	{
		m_ticket_data.total_trade_counter = (p_consume_count[0] << 8) + p_consume_count[1];
	}
}

RETINFO TicketSvt::write_card(ETYTKOPER operType, METRO_CPU_INF write_inf, uint8_t tac_type, long trade_amount, long& sam_seq, char * p_trade_tac)
{
	RETINFO ret			= {0};
	uint8_t u_recv[64]	= {0};
	char sz_tac[11]		= {0};

	do
	{
		ret.wErrCode = ERR_CARD_WRITE;

		// 选择ADF1
		//if (svt_selectfile(0x1001) < 0)
		//{
		//	if (svt_selecfileaid(9, (unsigned char *)"\xA0\x00\x00\x00\x03\x86\x98\x07\x01") < 0)
		//	{
		//		break;
		//	}
		//}

		// PIN校验，替换选择ADF1
		if (mtr_cpu_pin_check() != 0)
		{
		    g_Record.log_out(ret.wErrCode, level_error, "mtr_cpu_pin_check");
			ret.wErrCode = ERR_CARD_READ;
			break;
		}
        //g_Record.log_out(ret.wErrCode, level_error, "set_app_ctrl");
		ret.wErrCode = set_app_ctrl(write_inf.p_app_ctrl);
		if (ret.wErrCode != 0)		break;
//g_Record.log_out(ret.wErrCode, level_error, "set_complex_file");
		ret = set_complex_file(tac_type, trade_amount, write_inf.p_trade_assist, write_inf.p_metro, write_inf.p_ctrl_record, u_recv);

		if (ret.wErrCode != 0)
		{
			if (ret.bNoticeCode == NTC_MUST_CONFIRM)
			{
			    //g_Record.log_out(ret.wErrCode, level_error, "NTC_MUST_CONFIRM");
				sam_seq = (u_recv[6] << 24) + (u_recv[7] << 16) + (u_recv[8] << 8) + u_recv[9];
			}
		}
		else
		{
			sam_seq = (u_recv[6] << 24) + (u_recv[7] << 16) + (u_recv[8] << 8) + u_recv[9];
			if (p_trade_tac != NULL)
			{
			    //g_Record.log_out(ret.wErrCode, level_error, "NTC_MUST_CONFIRM1111");
				sprintf(sz_tac, "%02X%02X%02X%02X  ", u_recv[10], u_recv[11], u_recv[12], u_recv[13]);
				memcpy(p_trade_tac, sz_tac, 10);
			}
		}

	} while (0);

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		ret.bNoticeCode = support_confirm(operType);

	return ret;
}

uint16_t TicketSvt::write_card(uint8_t tac_type, uint8_t * p_app_ctrl, int record_id, uint8_t lock_flag, uint8_t * p_file_data, long& sam_seq)
{
	uint16_t ret				= ERR_CARD_WRITE;
	unsigned char u_recv[512]	= {0};

	do
	{
		if (set_app_ctrl(p_app_ctrl) != 0)
			break;

		if (svt_unlock_recordfile(SAM_SOCK_1, m_ReadInf.p_issue_base + 8, record_id, lock_flag, p_file_data + 3) != 0)
			break;

		if (svt_consum(CmdSort::m_time_now, SAM_SOCK_1, m_ReadInf.p_issue_base + 8, 0, tac_type, NULL , u_recv) != 0)
			break;

		sam_seq = (u_recv[6] << 24) + (u_recv[7] << 16) + (u_recv[8] << 8) + u_recv[9];

		ret = 0;

	} while (0);

	return ret;
}

uint16_t TicketSvt::set_trade_assist(uint8_t * p_trade_assist)
{
	uint16_t ret = 0;
	unsigned char u_recv[512] = {0};

	if (p_trade_assist != NULL)
	{
		p_trade_assist[2] = m_ticket_data.list_falg;

		if (m_ticket_data.list_falg == 0)
			ret = svt_unlock_recordfile(SAM_SOCK_1, m_ReadInf.p_issue_base + 8, 0x01, p_trade_assist[2], p_trade_assist + 3);
		else
			ret = svt_updata_complex(CmdSort::m_time_now, SAM_SOCK_1, m_ReadInf.p_issue_base + 8, 0x01, p_trade_assist[2], p_trade_assist + 3, u_recv);

		if (ret != 0)
			ret = ERR_CARD_WRITE;
	}

	return ret;
}

uint16_t TicketSvt::set_metro(uint8_t tac_type, uint8_t * p_metro, uint32_t lValue, uint8_t * p_respond)
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

	if (svt_consum(CmdSort::m_time_now, SAM_SOCK_1, m_ReadInf.p_issue_base + 8, lValue, tac_type, p_metro + 3, p_respond) != 0)
		return ERR_CARD_WRITE;

	return 0;
}

uint16_t TicketSvt::set_ctrl_record(uint8_t * p_ctrl_record)
{
	if (p_ctrl_record != NULL)
	{
		uint8_t u_recv[64] = {0};

		p_ctrl_record[2] = m_ticket_data.app_lock_flag;
		memcpy(p_ctrl_record + 4, m_ticket_data.logical_peroidS, 7);

		if (svt_updata_complex(CmdSort::m_time_now, SAM_SOCK_1, m_ReadInf.p_issue_base + 8, 0x11, 0, p_ctrl_record + 3, u_recv) != 0)
			return ERR_CARD_WRITE;
	}

	return 0;
}

uint16_t TicketSvt::set_app_ctrl(uint8_t * p_app_ctrl)
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

		if (svt_update_binfile(SAM_SOCK_1, m_ReadInf.p_issue_base + 8, 0x11, 0, LENM_APP_CTRL, p_app_ctrl) != 0)
			return ERR_CARD_WRITE;
	}

	return 0;
}

// 将状态格式化为简化状态
SimpleStatus TicketSvt::get_simple_status(uint8_t init_status)
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

RETINFO TicketSvt::format_history(uint8_t&	his_count, PHSSVT p_his_array, int his_max)
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

bool TicketSvt::format_history(uint8_t * p_his_buffer, HSSVT& his)
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

RETINFO TicketSvt::lock(PTICKETLOCK p_lock)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;

	METRO_CPU_INF write_inf = file_need_write(operLock, &m_ReadInf);

	m_ticket_data.list_falg = '2'- p_lock->cLockFlag;

	if (m_ticket_data.list_falg == 0)
	{
		ret.wErrCode = write_card(0x06, NULL, 0x01, m_ticket_data.list_falg, write_inf.p_metro, lSamSeq);
	}
	else
	{
		ret = write_card(operLock, write_inf, 0x09, 0, lSamSeq, NULL);
	}

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		ret.bNoticeCode = 0;

	p_lock->lSAMTrSeqNo = lSamSeq;

	transfer_data_for_out(operLock, p_lock);
	if (m_ticket_data.list_falg == 1)
		p_lock->bStatus = LIFE_LOCKED;
	else
		p_lock->bStatus = LIFE_UNLOCKED;

	return ret;
}

RETINFO TicketSvt::deffer(PTICKETDEFER p_deffer, TICKET_PARAM& ticket_prm)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;

	METRO_CPU_INF write_inf = file_need_write(operDeffer, &m_ReadInf);

	memcpy(m_ticket_data.logical_peroidS, CmdSort::m_time_now, 7);
	m_ticket_data.effect_mins = ticket_prm.deffer_days * 24 * 60;

	if (write_inf.p_ctrl_record != NULL)
		memcpy(write_inf.p_ctrl_record + 4, m_ticket_data.logical_peroidS, 7);

	ret.wErrCode = write_card(0x06, write_inf.p_app_ctrl, 0x11, write_inf.p_ctrl_record[2], write_inf.p_ctrl_record, lSamSeq);
	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		ret.bNoticeCode = 0;

	p_deffer->lSAMTrSeqNo = lSamSeq;

	transfer_data_for_out(operDeffer, p_deffer);
	p_deffer->bStatus = LIFE_DEFFER;

	return ret;
}

RETINFO TicketSvt::set_complex_file(uint8_t tac_type, long lAmount, uint8_t * p_trade_assist, uint8_t * p_metro, uint8_t * p_ctrl_record, uint8_t * p_recv)
{
	int pos_file_buf			= 0;
	unsigned char u_count		= 0;
	unsigned char u_rid[8]		= {0};
	unsigned char u_lock[8]		= {0};
	unsigned char u_send[512]	= {0};
	RETINFO ret					= {0};

	if (p_trade_assist != NULL)
	{
		u_lock[u_count] = m_ticket_data.list_falg;
		u_rid[u_count] = 0x01;

		memcpy(u_send + pos_file_buf, p_trade_assist + 3, LENM_TRADE_ASSIST - 3);
		pos_file_buf += LENM_TRADE_ASSIST - 3;
		u_count ++;
	}

	if (p_metro != NULL)
	{
		p_metro[4] = (p_metro[4] & 0x07) | (m_ticket_data.init_status << 3);

		p_metro[5] = (uint8_t)(m_ticket_data.p_entry_info->station_id >> 8);
		p_metro[6] = (uint8_t)(m_ticket_data.p_entry_info->station_id & 0xFF);

		p_metro[7] = (uint8_t)((m_ticket_data.p_entry_info->dvc_type << 4) + (m_ticket_data.p_entry_info->dvc_id >> 8));
		p_metro[8] = (uint8_t)(m_ticket_data.p_entry_info->dvc_id & 0xFF);

		memcpy(p_metro + 9, m_ticket_data.p_entry_info->time, sizeof(m_ticket_data.p_entry_info->time));


		p_metro[16] = (uint8_t)(m_ticket_data.write_last_info.station_id >> 8);
		p_metro[17] = (uint8_t)(m_ticket_data.write_last_info.station_id & 0xFF);

		p_metro[18] = (uint8_t)((m_ticket_data.write_last_info.dvc_type << 4) + (m_ticket_data.write_last_info.dvc_id >> 8));
		p_metro[19] = (uint8_t)(m_ticket_data.write_last_info.dvc_id & 0xFF);

		memcpy(p_metro + 20, m_ticket_data.write_last_info.time, sizeof(m_ticket_data.write_last_info.time));

		p_metro[27] = m_ticket_data.daliy_trade_counter;

		u_rid[u_count] = 0x02;

		add_metro_area_crc(p_metro, LENM_METRO);

		memcpy(u_send + pos_file_buf, p_metro + 3, LENM_METRO - 3);
		pos_file_buf += LENM_METRO - 3;
		u_count ++;
	}

	if (p_ctrl_record != NULL)
	{
		u_lock[u_count] = m_ticket_data.app_lock_flag;
		memcpy(p_ctrl_record + 4, m_ticket_data.logical_peroidS, 7);

		u_rid[u_count] = 0x11;

		memcpy(u_send + pos_file_buf, p_ctrl_record + 3, LENM_CTRL_RECORD - 3);
		pos_file_buf += LENM_CTRL_RECORD - 3;
		u_count ++;

	}

	ret.wErrCode = svt_updata_more_complex(tac_type, CmdSort::m_time_now,
		SAM_SOCK_1, m_ReadInf.p_issue_base + 8, lAmount, u_count, u_rid, u_lock, u_send, (unsigned char *)p_recv);
	if (ret.wErrCode != 0)
	{
		if (ret.wErrCode == 4)	// 消费并产生TAC时出错
			ret.bNoticeCode = NTC_MUST_CONFIRM;
		ret.wErrCode = ERR_CARD_WRITE;
	}

	return ret;
}

void TicketSvt::save_last_stack(ETYTKOPER operType, void * p_trade, size_t size_trade, void * p_written_inf, size_t size_written, bool save_to_file)
{
	METRO_CPU_INF_W written_info = {0};
	METRO_CPU_INF * p_old_write = (METRO_CPU_INF *)p_written_inf;

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

bool TicketSvt::last_trade_need_continue(uint8_t status_targ, uint16_t err_targ, uint16_t err_src)
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

RETINFO TicketSvt::continue_last_trade(void * p_trade)
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

RETINFO TicketSvt::data_version_upgrade()
{
	RETINFO ret		            = {0};
	unsigned char u_recv[256]	= {0};

	if (svt_unlock_recordfile(SAM_SOCK_1, m_ReadInf.p_issue_base + 8, 0x11, 0x00, u_recv) != 0)
		ret.wErrCode = ERR_CARD_WRITE;

	if (svt_consum(CmdSort::m_time_now, SAM_SOCK_1, m_ReadInf.p_issue_base + 8, m_ticket_data.wallet_value, 0x06, u_recv, u_recv) != 0)
		ret.wErrCode = ERR_CARD_WRITE;

	return ret;
}

uint16_t TicketSvt::read_wallet(long * p_wallet)
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
			g_Record.log_out(0, level_error, "read_wallet-svt_selectfile");
			ret = ERR_CARD_READ;
			break;
		}

		// 读发行基本信息
		if (svt_readbinary(0x05, 0, LENM_ISSUE_BASE, tmp) < 0)
		{
			g_Record.log_out(0, level_error, "read_wallet-svt_readbinary-05");
			ret = ERR_CARD_READ;
			break;
		}

		// 选择ADF1
		if (svt_selectfile(0x1001) < 0)
		{
			if (svt_selecfileaid(9, (unsigned char *)"\xA0\x00\x00\x00\x03\x86\x98\x07\x01") < 0)
			{
				g_Record.log_out(0, level_error, "read_wallet-svt_selecfileaid-ADF1");
				ret = ERR_CARD_READ;
				break;
			}
		}

		if (svt_getbalance(tmp) < 0)
		{
			g_Record.log_out(0, level_error, "read_wallet-svt_getbalance");
			ret = ERR_CARD_READ;
			break;
		}

		*p_wallet = (tmp[0] << 24) + (tmp[1] << 16) + (tmp[2] << 8) + tmp[3];

	} while (0);

	return ret;
}
