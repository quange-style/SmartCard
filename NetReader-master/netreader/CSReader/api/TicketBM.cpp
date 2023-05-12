#include "TicketBM.h"
#include "Errors.h"
#include "Publics.h"
#include "TimesEx.h"
#include "DataSecurity.h"

TicketBM::TicketBM(void)
{
}

TicketBM::~TicketBM(void)
{
}

TicketBM::TicketBM(char * p_current_sam, char * p_sam_posid, uint8_t * physic_info)
{
	m_p_current_sam = p_current_sam;
	m_p_sam_posid = p_sam_posid;

	memset(&m_ticket_data, 0, sizeof(m_ticket_data));

	memcpy(m_pid, physic_info, 4);
	sprintf(m_ticket_data.physical_id, "%02X%02X%02X%02X%-12C",
		physic_info[0], physic_info[1], physic_info[2], physic_info[3], 0x20);

	pMemoryPos = ((uint8_t *)this) + sizeof(TicketBM);
	m_ticket_data.p_entry_info = new(pMemoryPos) TradeInfo;

	m_ticket_data.p_exit_info = &m_ticket_data.read_last_info;
	m_ticket_data.p_update_info = &m_ticket_data.read_last_info;

	memset(&m_ticket_data.read_last_info, 0, sizeof(TradeInfo));

	m_start_use_flag = 0x00;

	m_ticket_data.qr_flag = 0x00;
}

// 统一票卡状态，分析和交易中的状态都用此状态
uint8_t TicketBM::unified_status()
{
	uint8_t ret = USTATUS_UNDEFINED;
	switch (m_ticket_data.init_status)
	{
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
	case MS_Upd_Wzone_Free:
		ret = SStatus_Upd_WZone;
		break;
	default:
		ret = USTATUS_SALE;
		break;
	}
	return ret;
}

RETINFO TicketBM::read_card(ETYTKOPER operType, TICKET_PARAM& ticket_prm, uint8_t * p_read_init)
{
	RETINFO ret			= {0};
	uint8_t u_recv[512]	= {0};
	int pos				= 0;
	long wallet_normal	= 0;
	long wallet_special	= 0;

	do
	{
		memset(&m_ReadInf, 0, sizeof(m_ReadInf));

		if (octm1_getdata2(SAM_SOCK_3, (unsigned char *)(m_pid), (unsigned char *)(u_recv)) != 0)
		{
			ret.wErrCode = ERR_CARD_READ;
			break;
		}

		// 公共钱包
		Publics::make_val(u_recv + pos, 4, wallet_normal, true);
		pos += 4;

		// 专用钱包
		Publics::make_val(u_recv + pos, 4, wallet_special, true);
		pos += 4;

		// 卡类型(4)
		memcpy(m_ticket_data.logical_type, u_recv + pos, 2);
		pos += 2;
		contrast_ticket_type(m_ticket_data.logical_type);

		if (use_wallet_special(m_ticket_data.logical_type))
			m_ticket_data.wallet_value = wallet_special;
		else
			m_ticket_data.wallet_value = wallet_normal;

		// 逻辑卡号(6)
		sprintf(m_ticket_data.logical_id, "%012x%02x%02x%02x%02x", 0, u_recv[pos], u_recv[pos + 1], u_recv[pos + 2], u_recv[pos + 3]);
		pos += 4;

		// 发行时间 ＋ 发售地点(10)
		memcpy(m_ticket_data.date_issue + 1, u_recv + pos, 3);
		TimesEx::century_fill(m_ticket_data.date_issue);
		pos += 7;			// 时间为yymmddhhmm

		// 押金(17)
		m_ticket_data.deposit = 100 * u_recv[pos];
		pos += 1;

		// 应用标识
		m_start_use_flag = u_recv[pos];
		pos += 1;

		// 审核情况(为逻辑有效期)
		memcpy(m_ticket_data.logical_peroidE + 1, u_recv + pos, 3);
		TimesEx::century_fill(m_ticket_data.logical_peroidE);
		memcpy(m_ticket_data.logical_peroidE + 4, "\x23\x59\x59", 3);
		if (TimesEx::bcd_time_valid(m_ticket_data.logical_peroidE, T_TIME))
			memcpy(m_ticket_data.logical_peroidS, CmdSort::m_time_now, 4);
		pos += 3;

		// 卡启用标识(22)
		m_ticket_data.globle_status = u_recv[pos];
		pos += 1;

		// 城市代码，行业代码(23)
		Publics::bcds_to_string(u_recv + pos, 2, m_ticket_data.city_code, 4);
		pos += 2;
		Publics::bcds_to_string(u_recv + pos, 2, m_ticket_data.industry_code, 4);
		pos += 2;

		// 共用交易计数(27)
		Publics::make_val(u_recv + pos, 2, m_ticket_data.total_trade_counter, true);
		pos += 2;

		// 专用钱包交易计数 ＋ 交易日期(29)
		pos += 2 + 4;

		// 名单标识(35)
		m_ticket_data.list_falg = u_recv[pos];
		pos += 1;

		// 基本信息备份
		memcpy(m_ReadInf.base_public, u_recv, pos);

		// 交易历史备份(36)
		memcpy(m_ReadInf.his_all, u_recv + pos, 15);
		pos += 15;

		// 地铁信息区
		// 地铁信息区备份(52)
		memcpy(m_ReadInf.metro_inf, u_recv + pos, 16);

		if (Publics::every_equal<uint8_t>(0, m_ReadInf.metro_inf, 16))
		{
			// 状态
			m_ticket_data.init_status = MS_Sale;
		}
		else
		{
			// 最后一次交易信息
			m_ticket_data.read_last_info.dvc_type = (ETPDVC)(m_ReadInf.metro_inf[0] & 0x0F);
			m_ticket_data.read_last_info.dvc_id = ((m_ReadInf.metro_inf[1] & 0x0F) << 8) + m_ReadInf.metro_inf[2];
			m_ticket_data.read_last_info.station_id = (m_ReadInf.metro_inf[3] << 8) + m_ReadInf.metro_inf[4];
			TimesEx::tm4_bcd7_exchange(m_ReadInf.metro_inf + 5, m_ticket_data.read_last_info.time, true);
			g_Parameter.sam_from_device(m_ticket_data.read_last_info.station_id,
				m_ticket_data.read_last_info.dvc_type, m_ticket_data.read_last_info.dvc_id, SAM_SOCK_3, m_ticket_data.read_last_info.sam);

			// 状态
			m_ticket_data.init_status = (m_ReadInf.metro_inf[1] >> 4) & 0x0F;

			// 进站信息
			memcpy(m_ticket_data.p_entry_info, &m_ticket_data.read_last_info, sizeof(TradeInfo));
			m_ticket_data.p_entry_info->station_id = (m_ReadInf.metro_inf[9] << 8) + m_ReadInf.metro_inf[10];
		}

		m_ticket_data.simple_status = get_simple_status(m_ticket_data.init_status);

		memset(m_ticket_data.issue_code, '0', sizeof(m_ticket_data.issue_code));
		memset(m_ticket_data.certificate_code, '0', sizeof(m_ticket_data.certificate_code));
		memset(m_ticket_data.certificate_name, '0', sizeof(m_ticket_data.certificate_name));

		// 学生票有效期过期时变成普通票
		ret.wErrCode = g_Parameter.query_ticket_prm(m_ticket_data.logical_type, ticket_prm);
		if (ret.wErrCode != 0)	break;

		m_ticket_data.wallet_value_max = ticket_prm.balance_max;

		memcpy(m_ticket_data.cert_peroidE, m_ticket_data.logical_peroidE, 4);
		if (ticket_prm.check_logic_expire && memcmp(m_ticket_data.logical_type, "\x06\x01", 2) == 0 &&
			memcmp(CmdSort::m_time_now, m_ticket_data.cert_peroidE, 4) > 0)
		{
			memcpy(m_ticket_data.fare_type, "\x06\x00", 2);
		}
		else
		{
			memcpy(m_ticket_data.fare_type, m_ticket_data.logical_type, 2);
		}

	} while (0);

	return ret;
}

RETINFO TicketBM::analyse_common(uint8_t wk_area, MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PTICKETLOCK p_lock, uint16_t& lenlock, PBOMANALYZE p_analyse, char * p_mode)
{
	RETINFO ret				= {0};
	char mode				= '0';

	do
	{
		// 公交M1无物理有效期信息
		//// 物理有效期
		//if (p_ticket_prm->check_phy_expire && need_check_period(wk_area, m_ticket_data.simple_status))
		//{
		//	if (memcmp(CmdSort::m_time_now, m_ticket_data.phy_peroidE, 4) > 0)
		//	{
		//		ret.wErrCode = ERR_OVER_PERIOD_P;
		//		break;
		//	}
		//}

		// 启用标识
		if (need_check_use_flag(m_ticket_data.logical_type))
		{
			if (m_start_use_flag != 0x01 && m_start_use_flag != 0xBB)
			{
				if (p_degrade_falgs->train_trouble)
				{
					mode = (char)(0x30 + degrade_Trouble);
				}
				else
				{
					ret.wErrCode = ERR_CARD_DISABLED;
					break;
				}
			}
		}

		// 黑名单
		bool in_prm = g_Parameter.query_black_oct(m_ticket_data.logical_id);
		if (m_ticket_data.list_falg == 0x04)
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

RETINFO TicketBM::analyse_work_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PPURSETRADE p_purse, PBOMETICANALYZE p_eticanalyse)
{
	RETINFO ret				= {0};

	ret = TicketBase::analyse_work_zone(p_degrade_falgs, p_ticket_prm, p_analyse, p_purse, p_eticanalyse);

	return ret;
}

RETINFO TicketBM::analyse_free_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PBOMETICANALYZE p_eticanalyse)
{
	RETINFO ret				= {0};

	ret = TicketBase::analyse_free_zone(p_degrade_falgs, p_ticket_prm, p_analyse, p_eticanalyse);

	return ret;
}

RETINFO TicketBM::entry_gate(PENTRYGATE p_entry,PETICKETDEALINFO p_eticketdealinfo,char cOnlineRes)
{
	RETINFO ret				= {0};
	WRITE_BUS_M1 wrt_inf	= {0};

	m_ticket_data.init_status = MS_Entry;
	m_ticket_data.p_entry_info = &m_ticket_data.write_last_info;
	current_trade_info(*m_ticket_data.p_entry_info);

	ret = write_card(operEntry, 0, NULL, NULL, &wrt_inf);

	transfer_data_for_out(operEntry, p_entry);
	p_entry->bStatus = LIFE_ENTRY;

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
	{
		save_last_stack(operEntry, p_entry, sizeof(ENTRYGATE), &wrt_inf, sizeof(wrt_inf), false);
	}

	if (ret.wErrCode == 0 && memcmp(m_ticket_data.logical_type, m_ticket_data.fare_type, 2) != 0)
	{
		ret.bNoticeCode = NTC_TYPE_OVER_PERIOD;
	}

	return ret;
}

RETINFO TicketBM::exit_gate(PPURSETRADE p_exit,PETICKETDEALINFO p_eticketdealinfo, MODE_EFFECT_FLAGS * p_degrade_falgs,char cOnlineRes)
{
	RETINFO ret				= {0};
	WRITE_BUS_M1 wrt_inf	= {0};

	if (p_degrade_falgs->train_trouble)
		m_ticket_data.init_status = MS_Exit_Tt;
	else
		m_ticket_data.init_status = MS_Exit;
	m_ticket_data.p_exit_info = &m_ticket_data.write_last_info;
	m_ticket_data.wallet_value -= p_exit->lTradeAmount;

	current_trade_info(*m_ticket_data.p_exit_info);

	ret = write_card(operExit, p_exit->lTradeAmount, p_exit->cClassicType, p_exit->cMACorTAC, &wrt_inf);

	transfer_data_for_out(operExit, p_exit);
	if (!need_check_use_flag(m_ticket_data.logical_type))
	{
		if (m_ticket_data.simple_status == SStatus_Upd_WZone)
			memcpy(p_exit->cPaymentType, "9C", sizeof(p_exit->cPaymentType));
		else
			memcpy(p_exit->cPaymentType, "92", sizeof(p_exit->cPaymentType));
	}
	else
	{
		if (m_ticket_data.simple_status == SStatus_Upd_WZone)
			memcpy(p_exit->cPaymentType, "4C", sizeof(p_exit->cPaymentType));
		else
			memcpy(p_exit->cPaymentType, "42", sizeof(p_exit->cPaymentType));
	}

	p_exit->bStatus = LIFE_EXIT;

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
	{
		save_last_stack(operExit, p_exit, sizeof(PURSETRADE), &wrt_inf, sizeof(wrt_inf), false);
	}

	if (ret.wErrCode == 0 && memcmp(m_ticket_data.logical_type, m_ticket_data.fare_type, 2) != 0)
	{
		ret.bNoticeCode = NTC_TYPE_OVER_PERIOD;
	}

	return ret;
}

RETINFO TicketBM::check_online(ETYTKOPER operType,PETICKETAUTHINFO p_eticketauthinfo)
{
	RETINFO ret				= {0};
	return ret;
}

RETINFO TicketBM::bom_update(PTICKETUPDATE p_update, uint8_t * p_entry_station)
{
	RETINFO ret				= {0};
	WRITE_BUS_M1 wrt_inf	= {0};
	long fare				= 0;

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
		m_ticket_data.p_entry_info->station_id = (p_entry_station[0] << 8) + p_entry_station[1];
		m_ticket_data.init_status = MS_Upd_Wzone_Entry;
		p_update->bStatus = LIFE_UPD_WO_STA;

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
		m_ticket_data.p_update_info = &m_ticket_data.write_last_info;
		current_trade_info(*m_ticket_data.p_update_info);

		ret = write_card(operUpdate, 0, NULL, NULL, &wrt_inf);

		// 交易记录赋值
		transfer_data_for_out(operUpdate, p_update);

		if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		{
			save_last_stack(operExit, p_update, sizeof(TICKETUPDATE), &wrt_inf, sizeof(wrt_inf), true);
		}
	}

	return ret;
}

RETINFO TicketBM::svt_decrease(PPURSETRADE p_purse, uint8_t u_pay_flag,uint8_t paytype)
{
	RETINFO ret				= {0};
	WRITE_BUS_M1 wrt_inf	= {0};

	if (p_purse->lTradeAmount > m_ticket_data.wallet_value)
	{
		ret.wErrCode = ERR_LACK_WALLET;
	}
	else
	{
		m_ticket_data.wallet_value -= p_purse->lTradeAmount;
		m_ticket_data.write_last_info = m_ticket_data.read_last_info;

		ret = write_card(operDecr, p_purse->lTradeAmount, p_purse->cClassicType, p_purse->cMACorTAC, &wrt_inf);
		p_purse->cPayType = paytype;

		transfer_data_for_out(operDecr, p_purse);
		if (!need_check_use_flag(m_ticket_data.logical_type))
		{
			if (u_pay_flag == PAY_UPD_PENALTY)
				memcpy(p_purse->cPaymentType, "9A", sizeof(p_purse->cPaymentType));
			else if (u_pay_flag == PAY_BUY_TOKEN)
				memcpy(p_purse->cPaymentType, "99", sizeof(p_purse->cPaymentType));
		}
		else
		{
			if (u_pay_flag == PAY_UPD_PENALTY)
				memcpy(p_purse->cPaymentType, "4A", sizeof(p_purse->cPaymentType));
			else if (u_pay_flag == PAY_BUY_TOKEN)
				memcpy(p_purse->cPaymentType, "49", sizeof(p_purse->cPaymentType));
		}

		p_purse->bStatus = LIFE_DECREASE;

		if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		{
			save_last_stack(operDecr, p_purse, sizeof(PURSETRADE), &wrt_inf, sizeof(wrt_inf), true);
		}
	}

	return ret;
}

RETINFO TicketBM::format_history(uint8_t&	his_count, PHSSVT p_his_array, int his_max)
{
	RETINFO ret	= {0};
	HSSVT his_all[30];

	his_count = 0;
	memset(his_all, 0, sizeof(his_all));

	if (format_history(m_ReadInf.his_all, his_all[his_count]))
		his_count++;

	memcpy(p_his_array, his_all, sizeof(HSSVT) * his_count);

	return ret;
}

RETINFO TicketBM::lock(PTICKETLOCK p_lock)
{
	RETINFO ret				= {0};

	m_ticket_data.list_falg = '2'- p_lock->cLockFlag;

	ret = write_card(operLock, 0, NULL, NULL, NULL);
	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		ret.bNoticeCode = 0;

	transfer_data_for_out(operLock, p_lock);
	if (m_ticket_data.list_falg == '1')
		p_lock->bStatus = LIFE_LOCKED;
	else
		p_lock->bStatus = LIFE_UNLOCKED;

	return ret;
}

bool TicketBM::last_trade_need_continue(uint8_t status_targ, uint16_t err_targ, uint16_t err_src)
{
	bool ret = false;

	P_WRITE_BUS_M1 p_write_inf = (P_WRITE_BUS_M1)cfm_point.dataWritten;

	do
	{
		CONFIRM_FACTOR factor = get_confirm_factor();

		if (!check_confirm(status_targ, m_ticket_data.init_status, err_targ, err_src))
			break;

		//
		// 根据写卡类型确定是否需要交易确认
		//
		// 只写地铁信息区
		if (p_write_inf->write_type == 2 && memcmp(p_write_inf->write_data, m_ReadInf.metro_inf, 16) != 0)
			break;

		// 只扣费
		if (p_write_inf->write_type == 3 && factor.trade_amount > 0 && m_ticket_data.wallet_value != cfm_point.balance)
			break;

		// 写地铁信息区同时扣费
		if (p_write_inf->write_type == 4 && factor.trade_amount > 0 &&
			m_ticket_data.wallet_value != cfm_point.balance && memcmp(p_write_inf->write_data, m_ReadInf.metro_inf, 16) != 0)
			break;

		ret = true;

	} while (0);

	return ret;
}

RETINFO TicketBM::continue_last_trade(void * p_trade)
{
	RETINFO ret				= {0};
	uint8_t u_recv[32]		= {0};
	char sz_tac[16]			= {0};
	uint8_t wallet_type		= 0;

	P_WRITE_BUS_M1 p_write_inf = (P_WRITE_BUS_M1)cfm_point.dataWritten;
	CONFIRM_FACTOR factor = get_confirm_factor();

	do
	{
		if (use_wallet_special(m_ticket_data.logical_type))
		{
			wallet_type = 1;
		}

		// 只要钱包扣值未成功就再写一次
		if (factor.trade_amount > 0 && m_ticket_data.wallet_value == cfm_point.balance + factor.trade_amount)
		{
			ret.wErrCode = octm1_writedata2(SAM_SOCK_3, p_write_inf->write_type, factor.trade_time, wallet_type, factor.trade_amount, p_write_inf->write_data);
			if (ret.wErrCode != 0)
			{
				ret.wErrCode = ERR_CARD_WRITE;
				break;
			}
		}

		// 获取TAC和SAM流水
		if (!Publics::every_equal<uint8_t>(0, p_write_inf->tac_data, 16))
		{
			if (octm1_gettac(SAM_SOCK_3, m_ReadInf.base_public + 10, p_write_inf->tac_data, u_recv) == 0)
			{
				sprintf(sz_tac, "%02X%02X%02X%02X  ",u_recv[0], u_recv[1], u_recv[2], u_recv[3]);
			}
		}

		set_confirm_factor(LIFE_CONFIRM, 0, sz_tac);

		memcpy(p_trade, &cfm_point.trade, cfm_point.size_trade);

	} while (0);

	return ret;
}

RETINFO TicketBM::write_card(ETYTKOPER operType, long trade_amount, char * p_tac_type, char * p_trade_tac, P_WRITE_BUS_M1 p_write_inf)
{
	RETINFO ret				= {0};
	uint8_t u_write_type	= 0;
	uint8_t u_tac_data[16]	= {0};
	uint8_t u_recv[32]		= {0};
	char sz_temp[16]		= {0};
	uint8_t wallet_type		= 0;

	do
	{
		if (p_tac_type != NULL)
			memset(p_tac_type, '0', 2);

		if (use_wallet_special(m_ticket_data.logical_type))
		{
			wallet_type = 1;
		}

		set_tac_factor(u_tac_data, CmdSort::m_time_now, m_ticket_data.wallet_value + trade_amount, trade_amount, (uint16_t)m_ticket_data.total_trade_counter);

		ret.wErrCode = get_write_type(operType, u_write_type, trade_amount, CmdSort::m_time_now, m_ReadInf.metro_inf);
		if (ret.wErrCode != 0)
			break;

		set_metro_area();

		ret.wErrCode = octm1_writedata2(SAM_SOCK_3, u_write_type, CmdSort::m_time_now, wallet_type, trade_amount, m_ReadInf.metro_inf);
		if (p_write_inf != NULL)
		{
			p_write_inf->write_type = u_write_type;
			memcpy(p_write_inf->write_data, m_ReadInf.metro_inf, 16);
			memcpy(p_write_inf->tac_data, u_tac_data, 16);
		}

		if (ret.wErrCode != 0)
		{
			if (ret.wErrCode == 0xEE)
				ret.bNoticeCode = NTC_MUST_CONFIRM;

			ret.wErrCode = ERR_CARD_WRITE;
			break;
		}

		// 获取TAC和SAM流水
		if (p_trade_tac != NULL)
		{
			if (octm1_gettac(SAM_SOCK_3, m_ReadInf.base_public + 10, u_tac_data, u_recv) == 0)
			{
				sprintf(sz_temp, "%02X%02X%02X%02X  ",u_recv[0], u_recv[1], u_recv[2], u_recv[3]);
				memcpy(p_trade_tac, sz_temp, 10);
			}
		}

	} while (0);

	return ret;
}

//void TicketBM::save_last_stack(ETYTKOPER operType, void * p_trade, size_t size_trade, void * p_written_inf, size_t size_written, bool save_to_file)
//{
//}

// 赋值给上次交易未赋值的交易因子
//void TicketBM::set_confirm_factor(uint8_t status, long sam_seq, char * p_tac)
//{
//}

SimpleStatus TicketBM::get_simple_status(uint8_t init_status)
{
	SimpleStatus ret = SStatus_Invalid;

	switch (init_status)
	{
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
	}
	return ret;
}

void TicketBM::contrast_ticket_type(uint8_t * p_ticket_type)
{
	uint8_t type_table[][4] =
	{
		{0x50, 0x01, 0x06, 0x00},			// 电子钱包卡
		{0x50, 0xA3, 0x06, 0x00},			// 成人钱包卡
		{0x50, 0xA4, 0x06, 0x01},			// 学生钱包卡
		{0x50, 0xA5, 0x06, 0x03},			// 老年人票卡
		{0x50, 0xB4, 0x06, 0x01},			// 家属学生卡
		{0x50, 0xA7, 0x06, 0x01},			// 内部学生卡
		{0x50, 0xA8, 0x06, 0x04}			// 残疾人票
	};

	for (size_t i=0;i<sizeof(type_table)/sizeof(type_table[0]);i++)
	{
		if (memcmp(type_table[i], p_ticket_type, 2) == 0)
		{
			memcpy(p_ticket_type, type_table[i] + 2, 2);
			break;
		}
	}
}

bool TicketBM::format_history(uint8_t * p_his_buffer, HSSVT& his)
{
	// 交易类型
	his.bStatus = p_his_buffer[0];
	if (his.bStatus < 100)
	{
		switch (his.bStatus)
		{
		case 0x01:
		case 0x03:
			his.bStatus = LIFE_CHARGE;
		case 0x02:
		case 0x04:
			his.bStatus = LIFE_CONSUME_COMMON;
			break;
		}
	}

	// 商户代码

	// POS 机编号
	char szTemp[32] = {0};
	sprintf(szTemp, "%08X%02X%02X%02X%02X", 0, p_his_buffer[3], p_his_buffer[4], p_his_buffer[5], p_his_buffer[6]);
	memcpy(his.cSAMID, szTemp, 16);

	// 交易日期
	memcpy(his.dtDate + 1, p_his_buffer + 7, 3);
	TimesEx::century_fill(his.dtDate);

	// 交易前余额

	//本次交易金额
	his.lTradeAmount = (p_his_buffer[13] << 8) + p_his_buffer[14];

	return true;
}

// 获取写卡类型，并判断传入参数是否正确
uint16_t TicketBM::get_write_type(ETYTKOPER operType, uint8_t& write_type, long trade_amount, uint8_t * p_time, uint8_t * p_metro)
{
#define WRT_UNLOCK			0x00
#define WRT_LOCK			0x01
#define WRT_MTR				0x02
#define WRT_WALLET			0x03
#define WRT_MTR_WALLET		0x04

	uint16_t u_ret = 0;

	write_type = 0xFF;

	switch(operType)
	{
	case operEntry:
		write_type = WRT_MTR;
		break;
	case operExit:
		write_type = WRT_MTR_WALLET;
		break;
	case operDecr:
		write_type = WRT_WALLET;
		break;
	case operUpdate:
		write_type = WRT_MTR;
		break;
	case operLock:
		if (m_ticket_data.list_falg == 0)
			write_type = WRT_UNLOCK;
		else
			write_type = WRT_LOCK;
		break;
	default:
		u_ret = ERR_INPUT_INVALID;
		break;
	}

	if (u_ret == 0)
	{
		if (write_type > WRT_MTR_WALLET)
			u_ret = ERR_INPUT_INVALID;
		else if ((write_type == WRT_MTR || WRT_MTR_WALLET) && p_metro == NULL)
			u_ret = ERR_INPUT_INVALID;
		else if ((write_type == WRT_WALLET || WRT_MTR_WALLET) && trade_amount < 0)
			u_ret = ERR_INPUT_INVALID;
	}

	return u_ret;
}

void TicketBM::set_metro_area()
{
	// 最后一次交易信息
	m_ReadInf.metro_inf[0] = (m_ReadInf.metro_inf[0] & 0xF0) | m_ticket_data.write_last_info.dvc_type;
	m_ReadInf.metro_inf[1] = ((m_ticket_data.write_last_info.dvc_id >> 8) & 0x0F) | (m_ticket_data.init_status << 4);
	m_ReadInf.metro_inf[2] = m_ticket_data.write_last_info.dvc_id & 0xFF;
	m_ReadInf.metro_inf[3] = (m_ticket_data.write_last_info.station_id >> 8) & 0xFF;
	m_ReadInf.metro_inf[4] = m_ticket_data.write_last_info.station_id & 0xFF;
	TimesEx::tm4_bcd7_exchange(m_ReadInf.metro_inf + 5, m_ticket_data.write_last_info.time, false);
	m_ReadInf.metro_inf[9] = (m_ticket_data.p_entry_info->station_id >> 8) & 0xFF;
	m_ReadInf.metro_inf[10] = m_ticket_data.p_entry_info->station_id & 0xFF;

	add_metro_area_crc(m_ReadInf.metro_inf, 16);
}

void TicketBM::set_tac_factor(uint8_t * p_tac_buf, uint8_t * p_time, long balance_after, uint16_t trade_amount, uint16_t trade_counter)
{
	p_tac_buf[0] = m_ticket_data.logical_type[1];

	memcpy(p_tac_buf + 1, p_time, 7);
	p_tac_buf[8] = (uint8_t)((balance_after >> 24) & 0xFF);
	p_tac_buf[9] = (uint8_t)((balance_after >> 16) & 0xFF);
	p_tac_buf[10] = (uint8_t)((balance_after >> 8) & 0xFF);
	p_tac_buf[11] = (uint8_t)(balance_after & 0xFF);

	p_tac_buf[12] = (uint8_t)((trade_amount >> 8) & 0xFF);
	p_tac_buf[13] = (uint8_t)(trade_amount & 0xFF);

	p_tac_buf[14] = (uint8_t)((trade_counter >> 8) & 0xFF);
	p_tac_buf[15] = (uint8_t)(trade_counter & 0xFF);

}

bool TicketBM::use_wallet_special(uint8_t * p_logic_type)
{
	static uint8_t special_wallet_type[][2] =
	{
		{0x06, 0x01}
	};

	for (uint8_t i=0;i<sizeof(special_wallet_type)/sizeof(special_wallet_type[0]);i++)
	{
		if (memcmp(p_logic_type, special_wallet_type[i], 2) == 0)
			return true;
	}

	return false;
}

// 是否检查启用标识
bool TicketBM::need_check_use_flag(uint8_t * p_logic_type)
{
	static uint8_t uncheck_falg_type[][2] =
	{
		{0x06, 0x00},
		{0x06, 0x01}
	};

	for (uint8_t i=0;i<sizeof(uncheck_falg_type)/sizeof(uncheck_falg_type[0]);i++)
	{
		if (memcmp(p_logic_type, uncheck_falg_type[i], 2) == 0)
			return true;
	}

	return false;
}
