#include "TicketBus.h"
#include "Errors.h"
#include "Publics.h"
#include "TimesEx.h"
#include "../link/myprintf.h"


TicketBus::TicketBus(void)
{

}

TicketBus::TicketBus(char * p_current_sam, char * p_sam_posid, uint8_t * physic_info)
{
	m_p_current_sam = p_current_sam;
	m_p_sam_posid = p_sam_posid;

	memset(&m_ticket_data, 0, sizeof(m_ticket_data));

	sprintf(m_ticket_data.physical_id, "%02X%02X%02X%02X%02X%02X%02X%-6C",
		physic_info[0], physic_info[1], physic_info[2], physic_info[3], physic_info[4], physic_info[5], physic_info[6], 0x20);

	uint8_t * pMemoryPos = ((uint8_t *)this) + sizeof(TicketBus);
	m_ticket_data.p_entry_info = new(pMemoryPos) TradeInfo;

	m_ticket_data.p_exit_info = &m_ticket_data.read_last_info;
	m_ticket_data.p_update_info = &m_ticket_data.read_last_info;

	memset(&m_ticket_data.read_last_info, 0, sizeof(TradeInfo));

	m_start_use_flag = 0x00;

	m_ticket_data.qr_flag = 0x00;
}

TicketBus::~TicketBus(void)
{

}

uint8_t TicketBus::unified_status()
{
	uint8_t ret = USTATUS_UNDEFINED;
	switch (m_ticket_data.init_status)
	{
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
	case MS_Upd_Wzone_Free:
		ret = SStatus_Upd_WZone;
		break;
	}
	return ret;
}

uint16_t TicketBus::bus_svt_read(P_BUS_CPU_INF p_bus_inf)
{
	uint16_t u_ret			= 0;
	uint8_t temp_type[2]	= {0};
	uint16_t file_no		= 0x3F01;

	do
	{
		// 选择主目录
		if (svt_selectfile(0x3F00) < 0)
		{
			u_ret = ERR_CARD_READ;
			break;
		}

		u_ret = oct_get_lock_flag();
		if (u_ret != 0)	break;

		if (p_bus_inf->p_issue_base != NULL)
		{
			// 读发行基本信息
			if (svt_readbinary(0x05, 0, LENB_ISSUE_BASE, p_bus_inf->p_issue_base) < 0)
			{
				u_ret = ERR_CARD_READ;
				break;
			}

			// 卡类型
			memcpy(temp_type, p_bus_inf->p_issue_base + 16, 2);
			contrast_ticket_type(temp_type);

			if (use_wallet_special(temp_type))
				file_no = 0xDF03;
			else
				file_no = 0x3F01;
		}

		// 读应用索引文件
		if (p_bus_inf->p_app_index != NULL)
		{
			// 读发行基本信息
			if (svt_readbinary(0x06, 0, LENB_APP_INDEX, p_bus_inf->p_app_index) < 0)
			{
				u_ret = ERR_CARD_READ;
				break;
			}
		}

		// 选择ADF1
		if (svt_selectfile(file_no) < 0)
		{
			u_ret = ERR_CARD_READ;
			break;
		}

		if (p_bus_inf->p_public_base != NULL)
		{
			// 读发行基本信息
			if (svt_readbinary(0x15, 0, LENB_PUBLIC_BASE, p_bus_inf->p_public_base) < 0)
			{
				u_ret = ERR_CARD_READ;
				break;
			}
		}

		// 读持卡人信息

		if (p_bus_inf->p_owner != NULL)
		{
			if (svt_readbinary(0x16, 0, LENB_OWNER_BASE, p_bus_inf->p_owner) < 0)
			{
				//u_ret = ERR_CARD_READ;
				//break;
			}
		}

		// 钱包
		if (p_bus_inf->p_wallet != NULL)
		{
			if (svt_getbalance(p_bus_inf->p_wallet,0x00) < 0)
			{
				u_ret = ERR_CARD_READ;
				break;
			}
		}

		// 所有历史
		if (p_bus_inf->p_his_all != NULL)
		{
			if (svt_readhistory(0x18, 1, 10, p_bus_inf->p_his_all) < 0)					// 本地
			{
				u_ret = ERR_CARD_READ;
				break;
			}

			if (svt_readhistory(0x10, 1, 10, p_bus_inf->p_his_all + 0x17 * 10) < 0)		// 异地
			{
				u_ret = ERR_CARD_READ;
				break;
			}
			if (svt_readhistory(0x1A, 1, 10, p_bus_inf->p_his_all + 0x17 * 20) < 0)		// 圈存
			{
				u_ret = ERR_CARD_READ;
				break;
			}
		}

		// 轨道交通
		if (p_bus_inf->p_metro != NULL)
		{
			if (svt_readrecord(0x17, 3, LENB_METRO, p_bus_inf->p_metro) < 0)
			{
				u_ret = ERR_CARD_READ;
				break;
			}
		}

		//// 充值计数
		//if (p_bus_inf->p_charge_count != NULL)
		//{
		//	if (svt_getonlineserial(SAM_SOCK_2, p_bus_inf->p_charge_count) < 0)
		//	{
		//		u_ret = ERR_CARD_READ;
		//		break;
		//	}
		//}

		// 交易计数
		if (p_bus_inf->p_consume_count != NULL)
		{
			if (svt_getofflineserial(SAM_SOCK_2, p_bus_inf->p_consume_count) < 0)
			{
				u_ret = ERR_CARD_READ;
				break;
			}
		}

	} while (0);

	return u_ret;
}

RETINFO TicketBus::read_card(ETYTKOPER operType, TICKET_PARAM& ticket_prm, uint8_t * p_read_init/* = NULL*/)
{
	RETINFO ret				= {0};

	do
	{
		memset(&m_u_buffer, 0, sizeof(m_u_buffer));
		m_ReadInf = file_need_read(operType, m_u_buffer);

		ret.wErrCode = bus_svt_read(&m_ReadInf);
		if (ret.wErrCode != 0)	break;

		//
		// 发行基本信息
		//
		get_issue_base(m_ReadInf.p_issue_base);
		ret.wErrCode = g_Parameter.query_ticket_prm(m_ticket_data.logical_type, ticket_prm);
		if (ret.wErrCode != 0)	break;

		//
		// 应用索引文件
		//
		get_app_index(m_ReadInf.p_app_index, ticket_prm);
		//
		// 发行基本信息
		//
		get_public_base(m_ReadInf.p_public_base);
		//
		// 持卡人信息
		//
		get_owner_base(m_ReadInf.p_owner);
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
		// 地铁信息
		//
		get_metro(m_ReadInf.p_metro);

		m_ticket_data.wallet_value_max = ticket_prm.balance_max;

		m_ticket_data.simple_status = get_simple_status(m_ticket_data.init_status);

	} while (0);


	return ret;
}

void TicketBus::get_issue_base(uint8_t * p_issue_base)
{
	if (p_issue_base != NULL)
	{
		// 发卡方代码
		Publics::bcds_to_string(p_issue_base, 2, m_ticket_data.issue_code, 4);

		// 城市代码、行业代码
		Publics::bcds_to_string(p_issue_base + 2, 2, m_ticket_data.city_code, 4);
		Publics::bcds_to_string(p_issue_base + 4, 2, m_ticket_data.industry_code, 4);

		// 逻辑卡号
		sprintf(m_ticket_data.logical_id, "0000%02X%02X%02X%02X%02X%02X%02X%02X",
			p_issue_base[8], p_issue_base[9], p_issue_base[10], p_issue_base[11],
			p_issue_base[12], p_issue_base[13], p_issue_base[14], p_issue_base[15]);

		// 卡类型
		memcpy(m_ticket_data.logical_type, p_issue_base + 16, 2);
		contrast_ticket_type(m_ticket_data.logical_type);

		// 发行日期
		memcpy(m_ticket_data.date_issue, p_issue_base + 18, 4);

		// 启用标识
		m_start_use_flag = p_issue_base[27];

		// 押金
		m_ticket_data.deposit = p_issue_base[43] * 100;

		// 物理有效期
		memcpy(m_ticket_data.phy_peroidE, p_issue_base + 44, 4);
	}
}

void TicketBus::get_app_index(uint8_t * p_issue_base, TICKET_PARAM& ticket_prm)
{
	memcpy(m_ticket_data.cert_peroidE + 1, p_issue_base + 12, 3);
	TimesEx::century_fill(m_ticket_data.cert_peroidE);

	// 学生票有效期过期时变成普通票
	if (ticket_prm.check_logic_expire && memcmp(m_ticket_data.logical_type, "\x06\x01", 2) == 0 &&
		memcmp(CmdSort::m_time_now, m_ticket_data.cert_peroidE, 4) > 0)
	{
		memcpy(m_ticket_data.fare_type, "\x06\x00", 2);
	}
	else
	{
		memcpy(m_ticket_data.fare_type, m_ticket_data.logical_type, 2);
	}
}

void TicketBus::get_public_base(uint8_t * p_public_base)
{
	if (p_public_base != NULL)
	{
		// 逻辑有效期
		memcpy(m_ticket_data.logical_peroidS, p_public_base + 20, 4);
		memcpy(m_ticket_data.logical_peroidE, p_public_base + 24, 4);
		memcpy(m_ticket_data.logical_peroidE + 4, "\x23\x59\x59", 3);
	}
}

void TicketBus::get_owner_base(uint8_t * p_owner_base)
{
	if (p_owner_base != NULL)
	{
		memcpy(m_ticket_data.certificate_code, p_owner_base + 24, 20);
		m_ticket_data.certificate_type = p_owner_base[44];
		get_certificate_name(m_ticket_data.certificate_type, m_ticket_data.certificate_name);
	}
}

void TicketBus::get_wallet(uint8_t * p_wallet)
{
	if (p_wallet != NULL)
	{
		m_ticket_data.wallet_value = (p_wallet[0] << 24) + (p_wallet[1] << 16) + (p_wallet[2] << 8) + p_wallet[3];
	}
}

void TicketBus::get_his_all(uint8_t * p_his_all)
{
	if (p_his_all != NULL)
	{

	}
}

void TicketBus::get_metro(uint8_t * p_metro)
{
	if (p_metro != NULL)
	{
		if (!metro_area_crc_valid(p_metro, LENB_METRO))
		{
			memset(p_metro + 3, 0, LENB_METRO - 3);		// 前3字节不能变动
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

void TicketBus::get_change_count(uint8_t * p_charge_count)
{
	if (p_charge_count != NULL)
	{
		m_ticket_data.charge_counter = (p_charge_count[0] << 8) + p_charge_count[1];
	}
}

void TicketBus::get_consume_count(uint8_t * p_consume_count)
{
	if (p_consume_count != NULL)
	{
		m_ticket_data.total_trade_counter = (p_consume_count[0] << 8) + p_consume_count[1];
	}
}
// 将状态格式化为简化状态
SimpleStatus TicketBus::get_simple_status(uint8_t init_status)
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

BUS_CPU_INF TicketBus::file_need_read(ETYTKOPER type, uint8_t * p_buffer)
{
	int ptr_buffer			= 0;
	BUS_CPU_INF ret_inf	= {0};

	ret_inf.p_wallet = p_buffer + ptr_buffer;
	ptr_buffer += LENB_WALLET;

	ret_inf.p_metro = p_buffer + ptr_buffer;
	ptr_buffer += LENB_METRO;

	ret_inf.p_issue_base = p_buffer + ptr_buffer;
	ptr_buffer += LENB_ISSUE_BASE;

	ret_inf.p_public_base = p_buffer + ptr_buffer;
	ptr_buffer += LENB_PUBLIC_BASE;

	ret_inf.p_app_index = p_buffer + ptr_buffer;
	ptr_buffer += LENB_APP_INDEX;

	switch (type)
	{
	case operAnalyse:
		ret_inf.p_owner = p_buffer + ptr_buffer;
		ptr_buffer += LENB_OWNER_BASE;

		ret_inf.p_charge_count = p_buffer + ptr_buffer;
		ptr_buffer += LENB_CHANGE_CNT;

		ret_inf.p_consume_count = p_buffer + ptr_buffer;
		ptr_buffer += LENB_CONSUME_CNT;
		break;

	case operTkInf:
		ret_inf.p_owner = p_buffer + ptr_buffer;
		ptr_buffer += LENB_OWNER_BASE;

		ret_inf.p_charge_count = p_buffer + ptr_buffer;
		ptr_buffer += LENB_CHANGE_CNT;

		ret_inf.p_consume_count = p_buffer + ptr_buffer;
		ptr_buffer += LENB_CONSUME_CNT;

		ret_inf.p_his_all = p_buffer + ptr_buffer;
		ptr_buffer += LENB_HIS_ALL;
		break;

	case operUpdate:
	case operLock:
	case operEntry:
		// 由于公交其他文件读取都不需要密钥，此处只是为了在无SAM卡时读卡产生错误
		ret_inf.p_consume_count = p_buffer + ptr_buffer;
		ptr_buffer += LENB_CONSUME_CNT;
		break;
	case operDecr:
	case operExit:
		ret_inf.p_charge_count = p_buffer + ptr_buffer;
		ptr_buffer += LENB_CHANGE_CNT;

		ret_inf.p_consume_count = p_buffer + ptr_buffer;
		ptr_buffer += LENB_CONSUME_CNT;

		break;
	default:
		break;
	}

	return ret_inf;
}
BUS_CPU_INF TicketBus::file_need_write(ETYTKOPER type, P_BUS_CPU_INF p_read_inf)
{
	BUS_CPU_INF ret_inf = {0};

	switch (type)
	{
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

RETINFO TicketBus::analyse_common(uint8_t wk_area, MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PTICKETLOCK p_lock, uint16_t& lenlock, PBOMANALYZE p_analyse, char * p_mode)
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

		// 启用标识
		if (need_check_use_flag(m_ticket_data.logical_type))
		{
			if (m_start_use_flag != 0x01)
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

RETINFO TicketBus::analyse_work_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PPURSETRADE p_purse, PBOMETICANALYZE p_eticanalyse)
{
	RETINFO ret				= {0};

	ret = TicketBase::analyse_work_zone(p_degrade_falgs, p_ticket_prm, p_analyse, p_purse, p_eticanalyse);

	return ret;
}

RETINFO TicketBus::analyse_free_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PBOMETICANALYZE p_eticanalyse)
{
	RETINFO ret				= {0};

	ret = TicketBase::analyse_free_zone(p_degrade_falgs, p_ticket_prm, p_analyse, p_eticanalyse);

	return ret;
}

RETINFO TicketBus::entry_gate(PENTRYGATE p_entry,PETICKETDEALINFO p_eticketdealinfo)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;

	BUS_CPU_INF write_inf = file_need_write(operEntry, &m_ReadInf);

	m_ticket_data.init_status = MS_Entry;
	m_ticket_data.p_entry_info = &m_ticket_data.write_last_info;
	current_trade_info(*m_ticket_data.p_entry_info);

	ret = write_card(operEntry, write_inf, 0x09, 0, lSamSeq, NULL);
	p_entry->lSAMTrSeqNo = lSamSeq;

	transfer_data_for_out(operEntry, p_entry);
	p_entry->bStatus = LIFE_ENTRY;

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
	{
		save_last_stack(operEntry, p_entry, sizeof(ENTRYGATE), &write_inf, LENB_METRO, false);
	}

	if (ret.wErrCode == 0 && memcmp(m_ticket_data.logical_type, m_ticket_data.fare_type, 2) != 0)
	{
		ret.bNoticeCode = NTC_TYPE_OVER_PERIOD;
	}

	return ret;
}

RETINFO TicketBus::exit_gate(PPURSETRADE p_exit,PETICKETDEALINFO p_eticketdealinfo, MODE_EFFECT_FLAGS * p_degrade_falgs)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;
	uint8_t tacType			= 0x09;

	BUS_CPU_INF write_inf = file_need_write(operExit, &m_ReadInf);

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
	if (m_ticket_data.logical_type[1] == 0x02 || m_ticket_data.logical_type[1] == 0x03)
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
	Publics::hex_to_two_char(tacType, p_exit->cClassicType);

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
	{
		save_last_stack(operExit, p_exit, sizeof(PURSETRADE), &write_inf, LENB_METRO, false);
	}

	if (ret.wErrCode == 0 && memcmp(m_ticket_data.logical_type, m_ticket_data.fare_type, 2) != 0)
	{
		ret.bNoticeCode = NTC_TYPE_OVER_PERIOD;
	}

	return ret;
}

RETINFO TicketBus::bom_update(PTICKETUPDATE p_update, uint8_t * p_entry_station)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;
	long fare				= 0;

	BUS_CPU_INF write_inf = file_need_write(operUpdate, &m_ReadInf);

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
		BUS_CPU_INF write_inf = file_need_write(operUpdate, &m_ReadInf);

		m_ticket_data.p_update_info = &m_ticket_data.write_last_info;
		current_trade_info(*m_ticket_data.p_update_info);

		ret = write_card(operUpdate, write_inf, 0x09, 0, lSamSeq, NULL);
		p_update->lSAMTrSeqNo = lSamSeq;

		// 交易记录赋值
		transfer_data_for_out(operUpdate, p_update);

		if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		{
			save_last_stack(operExit, p_update, sizeof(TICKETUPDATE), &write_inf, LENB_METRO, false);
		}
	}

	return ret;
}

RETINFO TicketBus::svt_decrease(PPURSETRADE p_purse, uint8_t u_pay_flag)
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
		BUS_CPU_INF write_inf = file_need_write(operEntry, &m_ReadInf);
		m_ticket_data.wallet_value -= p_purse->lTradeAmount;

		ret = write_card(operDecr, write_inf, tacType, p_purse->lTradeAmount, lSamSeq, p_purse->cMACorTAC);
		p_purse->lSAMTrSeqNo = lSamSeq;

		transfer_data_for_out(operDecr, p_purse);
		if (m_ticket_data.logical_type[1] == 0x02 || m_ticket_data.logical_type[1] == 0x03)
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
		Publics::hex_to_two_char(tacType, p_purse->cClassicType);

		if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		{
			save_last_stack(operExit, p_purse, sizeof(PURSETRADE), &write_inf, LENB_METRO, false);
		}
	}

	return ret;
}

RETINFO TicketBus::format_history(uint8_t&	his_count, PHSSVT p_his_array, int his_max)
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

bool TicketBus::format_history(uint8_t * p_his_buffer, HSSVT& his)
{
	char szTemp[32] = {0};

	// 交易金额,单位分
	his.lTradeAmount = (p_his_buffer[5] << 24) + (p_his_buffer[6] << 16) + (p_his_buffer[7] << 8) + p_his_buffer[8];

	// 票卡生命周期索引,具体定义见附录五
	his.bStatus = p_his_buffer[9];
	if (his.bStatus == 0x02)
		his.bStatus = LIFE_CHARGE;
	else if (his.bStatus == 0x06)
		his.bStatus = LIFE_CONSUME_COMMON;
	else if (his.bStatus == 0x09)
		his.bStatus = LIFE_CONSUME_COMPOUND;

	// sam id
	sprintf(szTemp, "0000%02X%02X%02X%02X%02X%02X",
		p_his_buffer[10], p_his_buffer[11], p_his_buffer[12], p_his_buffer[13], p_his_buffer[14], p_his_buffer[15]);
	memcpy(his.cSAMID, szTemp, sizeof(his.cSAMID));

	// 交易时间（BCD码)
	memcpy(his.dtDate, p_his_buffer + 16, sizeof(his.dtDate));

	// g_Parameter.device_from_sam(his.cSAMID, SAM_SOCK_2, his.bStationID, &his.bDeviceType, his.bDeviceID);

	return TimesEx::bcd_time_valid(his.dtDate, T_TIME);
}

RETINFO TicketBus::lock(PTICKETLOCK p_lock)
{
	RETINFO ret				= {0};
	long lSamSeq			= 0;

	BUS_CPU_INF write_inf = file_need_write(operLock, &m_ReadInf);

	m_ticket_data.list_falg = '2'- p_lock->cLockFlag;

	ret = write_card(0x06, m_ticket_data.list_falg, lSamSeq);
	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		ret.bNoticeCode = 0;

	p_lock->lSAMTrSeqNo = lSamSeq;

	transfer_data_for_out(operLock, p_lock);
	if (m_ticket_data.list_falg == '1')
		p_lock->bStatus = LIFE_LOCKED;
	else
		p_lock->bStatus = LIFE_UNLOCKED;

	return ret;
}

RETINFO TicketBus::write_card(ETYTKOPER operType, BUS_CPU_INF write_inf, uint8_t tac_type, long trade_amount, long& sam_seq, char * p_trade_tac)
{
	RETINFO ret			= {0};
	uint8_t u_recv[64]	= {0};
	uint16_t file_no	= 0x3F01;

	do
	{
		ret.wErrCode = ERR_CARD_WRITE;

		set_metro(write_inf.p_metro);

		if (use_wallet_special(m_ticket_data.logical_type))
			file_no = 0xDF03;
		else
			file_no = 0x3F01;

		// 选择ADF1
		if (svt_selectfile(file_no) < 0)
			break;

		ret.wErrCode = (uint16_t)oct_consum(CmdSort::m_time_now, SAM_SOCK_2, m_ReadInf.p_issue_base + 8, trade_amount, tac_type, write_inf.p_metro + 3, u_recv);
		if (ret.wErrCode != 0)
		{
			ret.wErrCode = ERR_CARD_WRITE;
			break;
		}

		sam_seq = (u_recv[6] << 24) + (u_recv[7] << 16) + (u_recv[8] << 8) + u_recv[9];

		if (p_trade_tac != NULL)
		{
			char tac[16] = {0};
			sprintf(tac, "%02X%02X%02X%02X  ", u_recv[10], u_recv[11], u_recv[12], u_recv[13]);
			memcpy(p_trade_tac, tac, 10);
		}

	} while (0);

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		ret.bNoticeCode = support_confirm(operType);

	return ret;

}

RETINFO TicketBus::write_card(uint8_t tac_type, uint8_t lock_flag, long& sam_seq)
{
	RETINFO ret					= {0};
	unsigned char u_recv[64]	= {0};
	uint16_t file_no			= 0x3F01;

	do
	{
		if (use_wallet_special(m_ticket_data.logical_type))
			file_no = 0xDF03;
		else
			file_no = 0x3F01;

		// 选择ADF1
		if (svt_selectfile(0x3F00) < 0)
		{
			ret.wErrCode = ERR_CARD_WRITE;
			break;
		}

		if (oct_cpu_lock(SAM_SOCK_2, m_ReadInf.p_issue_base + 8, m_ReadInf.p_issue_base + 2, lock_flag) != 0)
		{
			ret.wErrCode = ERR_CARD_WRITE;
			break;
		}

		// 选择ADF1
		if (svt_selectfile(file_no) < 0)
		{
			ret.wErrCode = ERR_CARD_WRITE;
			break;
		}

		ret.wErrCode = (uint16_t)oct_consum(CmdSort::m_time_now, SAM_SOCK_2, m_ReadInf.p_issue_base + 8, 0, tac_type, m_ReadInf.p_metro + 3, u_recv);
		if (ret.wErrCode != 0)
		{
			ret.wErrCode = ERR_CARD_WRITE;
			break;
		}

		sam_seq = (u_recv[6] << 24) + (u_recv[7] << 16) + (u_recv[8] << 8) + u_recv[9];

	} while (0);

	return ret;
}

void TicketBus::set_metro(uint8_t * p_metro)
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

		add_metro_area_crc(p_metro, LENB_METRO);

	}
}

void TicketBus::save_last_stack(ETYTKOPER operType, void * p_trade, size_t size_trade, void * p_written_inf, size_t size_written, bool save_to_file)
{
	BUS_CPU_INF_W written_info = {0};
	BUS_CPU_INF * p_old_write = (BUS_CPU_INF *)p_written_inf;

	if (p_old_write->p_metro != NULL)
	{
		written_info.flag_metro = 1;
		memcpy(written_info.w_metro, p_old_write->p_metro, LENB_METRO);
	}

	TicketBase::save_last_stack(operType, p_trade, size_trade, &written_info, sizeof(written_info), save_to_file);
}

bool TicketBus::last_trade_need_continue(uint8_t status_targ, uint16_t err_targ, uint16_t err_src)
{
	bool ret = true;

	BUS_CPU_INF_W * p_old_write = (BUS_CPU_INF_W *)cfm_point.dataWritten;

	do
	{
		CONFIRM_FACTOR factor = get_confirm_factor();

		if (!check_confirm(status_targ, m_ticket_data.init_status, err_targ, err_src))
		{
			ret = false;
			break;
		}

		if (p_old_write->flag_metro > 0)
		{
			if (memcmp(p_old_write->w_metro, m_ReadInf.p_metro, LENB_METRO) == 0)
			{
				break;
			}
		}

		if (factor.trade_amount > 0 && cfm_point.balance == m_ticket_data.wallet_value)
		{
			break;
		}

		ret = false;

	} while (0);

	return ret;
}

RETINFO TicketBus::continue_last_trade(void * p_trade)
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
//void TicketBus::set_confirm_factor(uint8_t status, long sam_seq, char * p_tac)
//{
//	uint8_t trade_type = Publics::string_to_hex<uint8_t>((char *)(&cfm_point.trade), 2);
//	switch(trade_type)
//	{
//	//case 0x50:	// 单程票发售
//	//	cfm_point.trade.sjtSale.bStatus		= status;
//	//	memcpy(cfm_point.trade.sjtSale.cMACorTAC, p_tac, 10);
//	//	break;
//	//case 0x51:	// 储值票发售
//	//	cfm_point.trade.svtSale.bStatus		= status;
//	//	break;
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
//	//case 0x57:	// 退款
//	//	cfm_point.trade.refund.bStatus		= status;
//	//	memcpy(cfm_point.trade.refund.cMACOrTAC, p_tac, 10);
//	//	break;
//	}
//}

void TicketBus::get_certificate_name(uint8_t cert_type, char * p_name)
{
	switch (cert_type)
	{
	case 0x00:
		memcpy(p_name, "本地居民身份证", 14);
		break;
	case 0x01:
		memcpy(p_name, "异地居民身份证", 14);
		break;
	case 0x02:
		memcpy(p_name, "暂居证", 6);
		break;
	case 0x03:
		memcpy(p_name, "学生证", 6);
		break;
	case 0x04:
		memcpy(p_name, "军官证", 6);
		break;
	case 0x05:
		memcpy(p_name, "士兵证", 6);
		break;
	case 0x06:
		memcpy(p_name, "警官证", 6);
		break;
	case 0x07:
		memcpy(p_name, "护照", 4);
		break;
	case 0x08:
		memcpy(p_name, "港澳通行证", 10);
		break;
	case 0x09:
		memcpy(p_name, "回乡证", 6);
		break;
	case 0x0A:
		memcpy(p_name, "台胞证", 6);
		break;
	case 0x0B:
		memcpy(p_name, "驾驶证", 6);
		break;
	}
}

void TicketBus::contrast_ticket_type(uint8_t * p_ticket_type)
{
	uint8_t type_table[][4] =
	{
		{0x80, 0x01, 0x06, 0x00},			// 电子钱包卡
		{0x80, 0xA3, 0x06, 0x00},			// 成人钱包卡
		{0x80, 0xA4, 0x06, 0x01},			// 学生钱包卡
		{0x80, 0xA5, 0x06, 0x03},			// 老年人票卡
		{0x80, 0xB4, 0x06, 0x01},			// 家属学生卡
		{0x80, 0xA7, 0x06, 0x01},			// 内部学生卡
		{0x80, 0xA8, 0x06, 0x04}			// 残疾人票
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

uint16_t TicketBus::oct_get_lock_flag()
{
	unsigned char ulen;
	unsigned char sztmp[256];
	unsigned char response_len;
	unsigned short response_sw;

	ulen = 5;
	memcpy(sztmp, "\x00\xB2\x03\xD4\x00", ulen);
	dbg_dumpmemory("oct_get_lock_flag=",sztmp,5);
	response_len = MifareProCom(ulen, sztmp, &response_sw);
	dbg_dumpmemory("oct_get_lock_flag |=",sztmp,response_len);
	if (response_len == 0 || response_sw != 0x9000)
	{
		return ERR_CARD_READ;
	}

	if (memcmp(sztmp, "\x53\x4B", 2) == 0)
		m_ticket_data.list_falg = 1;

	return 0;
}

bool TicketBus::use_wallet_special(uint8_t * p_logic_type)
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
bool TicketBus::need_check_use_flag(uint8_t * p_logic_type)
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
