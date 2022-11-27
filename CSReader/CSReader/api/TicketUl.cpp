#include "TicketUl.h"
#include "TimesEx.h"
#include "Publics.h"
#include "DataSecurity.h"
#include "Errors.h"
#include "../link/linker.h"
#include "../link/myprintf.h"

ULINF TicketUl::m_ulInfo	= {0};

TicketUl::TicketUl(void)
{
}

TicketUl::TicketUl(char * p_current_sam, char * p_sam_posid, uint8_t * physic_info)
{
    m_p_current_sam = p_current_sam;
	m_p_sam_posid = p_sam_posid;

    memset(&m_ticket_data, 0, sizeof(m_ticket_data));
	memset(&m_ulInfo, 0, sizeof(ULINF));

    sprintf(m_ticket_data.physical_id, "%02X%02X%02X%02X%02X%02X%02X%-6C",
            physic_info[0], physic_info[1], physic_info[2], physic_info[3], physic_info[4], physic_info[5], physic_info[6], 0x20);

    uint8_t * pMemoryPos = ((uint8_t *)this) + sizeof(TicketUl);
    m_ticket_data.p_entry_info = new(pMemoryPos) TradeInfo;

    memset(m_ticket_data.p_entry_info, 0, sizeof(TradeInfo));
    memset(&m_ticket_data.read_last_info, 0, sizeof(TradeInfo));
	m_ticket_data.qr_flag = 0x00;
}

uint16_t TicketUl::read_ul(uint8_t * p_ul_read_buf)
{
	return readul(SAM_SOCK_1, p_ul_read_buf);
}

uint16_t TicketUl::get_valid_ptr(ETYTKOPER operType, int read_result)
{
	uint16_t ret	= 0;
	short sub_count = (short)(m_ulInfo.area_data[0].trade_counter - m_ulInfo.area_data[1].trade_counter);

	switch (read_result)
	{
	case 0:			// 两个crc都正确
		if (sub_count > 0)
		{
			if (sub_count == 1)
			{
				m_ulInfo.area_ptr = 0;
				dbg_formatvar("m_ulInfo.area_ptr1:%d",m_ulInfo.area_ptr);
				g_Record.log_out(0, level_error, "m_ulInfo.area_ptr1:%d",m_ulInfo.area_ptr);
			}
			else
			{
				// 上次交易计数发生跳变就看交易时间
				if (memcmp(m_ulInfo.area_data[0].last_time, m_ulInfo.area_data[1].last_time, 7) >= 0)
				{
					m_ulInfo.area_ptr = 0;
					dbg_formatvar("m_ulInfo.area_ptr2:%d",m_ulInfo.area_ptr);
					g_Record.log_out(0, level_error, "m_ulInfo.area_ptr2:%d",m_ulInfo.area_ptr);
				}
				else
				{
					m_ulInfo.area_ptr = 1;
					dbg_formatvar("m_ulInfo.area_ptr3:%d",m_ulInfo.area_ptr);
					g_Record.log_out(0, level_error, "m_ulInfo.area_ptr3:%d",m_ulInfo.area_ptr);
				}
			}
		}
		else if (sub_count < 0)
		{
			if (sub_count == -1)
			{
				m_ulInfo.area_ptr = 1;
				dbg_formatvar("m_ulInfo.area_ptr4:%d",m_ulInfo.area_ptr);
				g_Record.log_out(0, level_error, "m_ulInfo.area_ptr4:%d",m_ulInfo.area_ptr);
			}
			else
			{
				// 上次交易计数发生跳变就看交易时间
				if (memcmp(m_ulInfo.area_data[0].last_time, m_ulInfo.area_data[1].last_time, 7) <= 0)
				{
					m_ulInfo.area_ptr = 1;
					dbg_formatvar("m_ulInfo.area_ptr5:%d",m_ulInfo.area_ptr);
					g_Record.log_out(0, level_error, "m_ulInfo.area_ptr5:%d",m_ulInfo.area_ptr);
				}
				else
				{
					m_ulInfo.area_ptr = 0;
					dbg_formatvar("m_ulInfo.area_ptr6:%d",m_ulInfo.area_ptr);
					g_Record.log_out(0, level_error, "m_ulInfo.area_ptr6:%d",m_ulInfo.area_ptr);
				}
			}
		}
		else	// 计数相等，直接看交易时间
		{
			if (memcmp(m_ulInfo.area_data[0].last_time, m_ulInfo.area_data[1].last_time, 7) >= 0)
			{
				m_ulInfo.area_ptr = 0;
				dbg_formatvar("m_ulInfo.area_ptr7:%d",m_ulInfo.area_ptr);
				g_Record.log_out(0, level_error, "m_ulInfo.area_ptr7:%d",m_ulInfo.area_ptr);
			}
			else
			{
				m_ulInfo.area_ptr = 1;
				dbg_formatvar("m_ulInfo.area_ptr8:%d",m_ulInfo.area_ptr);
				g_Record.log_out(0, level_error, "m_ulInfo.area_ptr8:%d",m_ulInfo.area_ptr);
			}
		}
		break;

	case 1:			// 只有0区crc错误
		m_ulInfo.area_ptr = 1;
		dbg_formatvar("m_ulInfo.area_ptr9:%d",m_ulInfo.area_ptr);
		g_Record.log_out(0, level_error, "m_ulInfo.area_ptr9:%d",m_ulInfo.area_ptr);
		break;

	case 2:			// 只有1区crc错误
		m_ulInfo.area_ptr = 0;
		dbg_formatvar("m_ulInfo.area_ptr10:%d",m_ulInfo.area_ptr);
		g_Record.log_out(0, level_error, "m_ulInfo.area_ptr10:%d",m_ulInfo.area_ptr);
		break;

	case 3:			// 两个crc都错误
		if (operType == operSale)
		{
			m_ulInfo.area_ptr = 0;
			dbg_formatvar("m_ulInfo.area_ptr11:%d",m_ulInfo.area_ptr);
			g_Record.log_out(0, level_error, "m_ulInfo.area_ptr11:%d",m_ulInfo.area_ptr);
		}
		else
		{
			ret = ERR_CARD_INVALID;
			g_Record.log_out(0, level_error, "get_valid_ptr(ERR_CARD_INVALID)");
			dbg_formatvar("get_valid_ptr(ERR_CARD_INVALID)");
		}
		break;

	default:
		ret = ERR_CARD_READ;
		break;
	}

	return ret;
}

RETINFO TicketUl::read_card(ETYTKOPER operType, TICKET_PARAM& ticket_prm, uint8_t * p_read_init/* = NULL*/)
{
    RETINFO ret				= {0};
    uint8_t ul_data[256]	= {0};
    int result_read			= 0;
	uint16_t tmp			= 0;

    do
    {
        result_read = read_ul(ul_data);

		dbg_dumpmemory("UL_card:",ul_data,64);

		//g_Record.log_buffer("单程票：", ul_data, 64);

		// 先不判断读返回结果
		memcpy(m_ulInfo.area_buffer[0], ul_data + 32, 16);
		memcpy(m_ulInfo.area_buffer[1], ul_data + 48, 16);

		dbg_dumpmemory("area_buffer[0]:",m_ulInfo.area_buffer[0],16);
		dbg_dumpmemory("area_buffer[1]:",m_ulInfo.area_buffer[1],16);
		g_Record.log_buffer("area_buffer[0]：", m_ulInfo.area_buffer[0],16);
		g_Record.log_buffer("area_buffer[1]：", m_ulInfo.area_buffer[1],16);

		get_deal_area(&m_ulInfo.area_data[0], m_ulInfo.area_buffer[0]);
		get_deal_area(&m_ulInfo.area_data[1], m_ulInfo.area_buffer[1]);

		ret.wErrCode = get_valid_ptr(operType, result_read);
		if (ret.wErrCode != 0)	
		{
			g_Record.log_out(0, level_error, "get_valid_ptr err = [%04X]",ret.wErrCode);
			break;
		}

		if (p_read_init != NULL)
			memcpy(p_read_init, ul_data, 64);

		//m_ulInfo.area_ptr = 0;//测试

        sprintf(m_ticket_data.logical_id, "000000000003%02X%02X%02X%02X", ul_data[16], ul_data[17], ul_data[18], ul_data[19]);

		//#####################################################################
		g_Record.log_out(0, level_disaster, "%s,1:%d", m_ticket_data.logical_id, m_ulInfo.area_ptr);

		dbg_formatvar("m_ulInfo.area_ptr:%d",m_ulInfo.area_ptr);

        TimesEx::tm2_bcd4_exchange(ul_data + 24, m_ticket_data.date_issue, true);
        m_ticket_data.key_flag = ul_data[25] & 0x01;

        memcpy(m_ticket_data.logical_type, ul_data + 26, 2);
		memcpy(m_ticket_data.fare_type, m_ticket_data.logical_type, 2);

		memcpy(m_ticket_data.read_last_info.time, m_ulInfo.area_data[m_ulInfo.area_ptr].last_time, 7);

		tmp = m_ulInfo.area_data[m_ulInfo.area_ptr].last_line * 100 + m_ulInfo.area_data[m_ulInfo.area_ptr].last_station;
        m_ticket_data.read_last_info.station_id = Publics::val_to_bcd(tmp);

        m_ticket_data.read_last_info.dvc_type = (ETPDVC)m_ulInfo.area_data[m_ulInfo.area_ptr].last_dvc_type;
        m_ticket_data.read_last_info.dvc_id = Publics::val_to_bcd(m_ulInfo.area_data[m_ulInfo.area_ptr].last_dvc_id);
		g_Parameter.sam_from_device(m_ticket_data.read_last_info.station_id, m_ticket_data.read_last_info.dvc_type,
			m_ticket_data.read_last_info.dvc_id, SAM_SOCK_1, m_ticket_data.read_last_info.sam);

        m_ticket_data.init_status = m_ulInfo.area_data[m_ulInfo.area_ptr].last_status;
        m_ticket_data.wallet_value = m_ulInfo.area_data[m_ulInfo.area_ptr].last_wallet_val;
		tmp = m_ulInfo.area_data[m_ulInfo.area_ptr].entry_line * 100 + m_ulInfo.area_data[m_ulInfo.area_ptr].entry_station;
        m_ticket_data.p_entry_info->station_id = Publics::val_to_bcd(tmp);
        memcpy(m_ticket_data.p_entry_info->time, m_ulInfo.area_data[m_ulInfo.area_ptr].last_time, 7);
		memcpy(m_ticket_data.p_entry_info->sam, m_ticket_data.read_last_info.sam, 16);
        m_ticket_data.total_trade_counter = m_ulInfo.area_data[m_ulInfo.area_ptr].trade_counter;

        m_ticket_data.simple_status = get_simple_status(m_ticket_data.init_status);

        m_ticket_data.p_update_info = &m_ticket_data.read_last_info;
        m_ticket_data.p_exit_info = &m_ticket_data.read_last_info;

		g_Parameter.query_ticket_prm(m_ticket_data.logical_type, ticket_prm);
		if (m_ticket_data.simple_status == SStatus_Es)
		{
			memcpy(m_ticket_data.logical_peroidS, CmdSort::m_time_now, 7);
			memcpy(m_ticket_data.logical_peroidE, m_ticket_data.read_last_info.time, 7);
		}
		else
		{
			memcpy(m_ticket_data.logical_peroidS, m_ticket_data.read_last_info.time, 7);
			memcpy(m_ticket_data.logical_peroidE, m_ticket_data.read_last_info.time, 7);

			TimesEx::bcd_time_calculate(m_ticket_data.logical_peroidE, T_TIME, 0, 0, ticket_prm.effect_time_from_sale);
		}

		m_ticket_data.wallet_value_max = ticket_prm.balance_max;

		memset(m_ticket_data.city_code, '0', sizeof(m_ticket_data.city_code));
		memset(m_ticket_data.issue_code, '0', sizeof(m_ticket_data.issue_code));
		memset(m_ticket_data.certificate_code, '0', sizeof(m_ticket_data.certificate_code));
		memset(m_ticket_data.certificate_name, '0', sizeof(m_ticket_data.certificate_name));

		memcpy(m_ticket_data.industry_code, "0000", 4);
    }
    while (0);

    return ret;
}

void TicketUl::get_deal_area(P_UL_AREA p_deal_info, uint8_t * p_deal_area)
{
    TimesEx::tm4_bcd7_exchange(p_deal_area, p_deal_info->last_time, true);

	p_deal_info->last_line = (uint8_t)((p_deal_area[4] >> 2) & 0x3F);
	p_deal_info->last_station = (uint8_t)(((p_deal_area[4] & 0x03) << 4) + ((p_deal_area[5] >> 4) & 0x0F));
	p_deal_info->last_dvc_type = (uint8_t)(p_deal_area[5] & 0x0F);
	p_deal_info->last_dvc_id = (uint16_t)((p_deal_area[6] << 2) + (p_deal_area[7] >> 6));
	p_deal_info->last_wallet_val = (uint16_t)(((p_deal_area[7] & 0x3F) << 8) + p_deal_area[8]);
	p_deal_info->last_status = (uint8_t)(p_deal_area[9] >> 3);
	p_deal_info->last_status_special = (uint8_t)(p_deal_area[9] & 0x07);
	p_deal_info->entry_line = (uint8_t)((p_deal_area[10] >> 2) & 0x3F);
	p_deal_info->entry_station = (uint8_t)(((p_deal_area[10] & 0x03) << 4) + ((p_deal_area[11] >> 4) & 0x0F));

    p_deal_info->trade_counter = (uint16_t)((p_deal_area[13] << 8) + p_deal_area[14]);

}

void TicketUl::set_deal_area(P_UL_AREA p_deal_info, uint8_t * p_deal_area)
{
    TimesEx::tm4_bcd7_exchange(p_deal_area, p_deal_info->last_time, false);

	p_deal_area[4] = (uint8_t)((p_deal_info->last_line << 2) + ((p_deal_info->last_station >> 4) &0x03));
	p_deal_area[5] = (uint8_t)(((p_deal_info->last_station & 0x0F) << 4) + (p_deal_info->last_dvc_type & 0x0F));
	p_deal_area[6] = (uint8_t)((p_deal_info->last_dvc_id >> 2) & 0xFF);
	p_deal_area[7] = (uint8_t)(((p_deal_info->last_dvc_id & 0x03) << 6) + ((p_deal_info->last_wallet_val >> 8) & 0x3F));
	p_deal_area[8] = (uint8_t)(p_deal_info->last_wallet_val & 0xFF);
	p_deal_area[9] = (uint8_t)(((p_deal_info->last_status & 0x1F) << 3) + (p_deal_info->last_status_special & 0x07));
	p_deal_area[10] = (uint8_t)((p_deal_info->entry_line << 2) + ((p_deal_info->entry_station >> 4) &0x03));
	p_deal_area[11] = (uint8_t)(((p_deal_info->entry_station & 0x0F) << 4) + (p_deal_area[11] & 0x0F));

    p_deal_area[13] = (uint8_t)(p_deal_info->trade_counter >> 8);
    p_deal_area[14] = (uint8_t)(p_deal_info->trade_counter & 0xFF);

}

// 将状态格式化为简化状态
SimpleStatus TicketUl::get_simple_status(uint8_t init_status)
{
    SimpleStatus ret = SStatus_Invalid;
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
    case MS_Exit_Only:
        ret = SStatus_Exit_Only;
        break;
    case MS_Exit_Tt:
        ret = SStatus_Exit_Tt;
        break;
    case MS_Upd_FZone_Free:
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
    return ret;
}

uint8_t TicketUl::unified_status()
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
		ret = USTATUS_UPD_IN;
		break;
	case MS_Refund:
		ret = USTATUS_REFUND;
		break;
	}
	return ret;
}

RETINFO TicketUl::analyse_common(uint8_t wk_area, MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PTICKETLOCK p_lock, uint16_t& lenlock, PBOMANALYZE p_analyse, char * p_mode)
{
    RETINFO ret				= {0};
	char mode				= '0';

    do
    {
		// 物理有效期，单程票没有物理有效期

		// 车票状态检查
		if (m_ticket_data.simple_status == SStatus_Invalid)
		{
			ret.wErrCode = ERR_CARD_STATUS;
			break;
		}

		if (m_ticket_data.simple_status == SStatus_Refund)
		{
			ret.wErrCode = ERR_CARD_REFUND;
			ret.bNoticeCode = NTC_TOKEN_RECLAIM;
			break;
		}

		if (g_Parameter.permit_sale(p_ticket_prm->sell_device))
		{
			if (m_ticket_data.simple_status == SStatus_Init)
			{
				ret.wErrCode = ERR_WITHOUT_SELL;
				if (p_analyse != NULL)
					p_analyse->dwOperationStauts |= ALLOW_SALE;

				break;
			}
		}

		// 检查余额(出站票的余额可能超出上限)
		if (m_ticket_data.simple_status != SStatus_Exit_Only && m_ticket_data.wallet_value > p_ticket_prm->balance_max)
		{
			if (p_degrade_falgs->train_trouble)
			{
				mode = (char)(0x30 + degrade_Trouble);
			}
			else
			{
				ret.wErrCode = ERR_OVER_WALLET;
				if (wk_area == 2 && p_analyse != NULL && g_Parameter.permit_sale(p_ticket_prm->sell_device))
					p_analyse->dwOperationStauts |= ALLOW_SALE;

				break;
			}
        }

    } while (0);

	if (p_mode != NULL)
		*p_mode = mode;

    return ret;
}

RETINFO TicketUl::analyse_work_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PPURSETRADE p_purse, PBOMETICANALYZE p_eticanalyse)
{
	RETINFO ret = {0};

	ret = TicketBase::analyse_work_zone(p_degrade_falgs, p_ticket_prm, p_analyse, p_purse, p_eticanalyse);

	if (ret.wErrCode == ERR_WITHOUT_ENTRY && m_ticket_data.simple_status == SStatus_Exit)
		ret.bNoticeCode = NTC_TOKEN_RECLAIM;
	else if (ret.wErrCode == ERR_UPDATE_DAY || ret.wErrCode == ERR_UPDATE_STATION || ret.wErrCode == ERR_OVER_PERIOD_L)
		ret.bNoticeCode = NTC_TOKEN_RECLAIM;
	else if (ret.wErrCode == ERR_LAST_EXIT_NEAR)
		ret.bNoticeCode = NTC_TOKEN_RECLAIM;

	if (ret.wErrCode == 0)
	{
		if (p_purse != NULL && !p_degrade_falgs->train_trouble && !p_degrade_falgs->emergency)
			p_purse->lTradeAmount = m_ticket_data.wallet_value;
	}
	else if (ret.bNoticeCode == NTC_TOKEN_RECLAIM)
	{
		if (p_analyse != NULL)
		{
			p_analyse->dwOperationStauts &= ~ALLOW_UPDATE;
			p_analyse->lPenalty = 0;
		}
	}

	return ret;
}

RETINFO TicketUl::analyse_free_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PBOMETICANALYZE p_eticanalyse)
{
    RETINFO ret = {0};
	uint32_t oper_status = 0;

    do
    {
        if (m_ticket_data.simple_status == SStatus_Exit_Only || m_ticket_data.simple_status == SStatus_Exit)
        {
            ret.wErrCode = ERR_CARD_INVALID;
            ret.bNoticeCode = NTC_TOKEN_RECLAIM;
            break;
        }

        ret = TicketBase::analyse_free_zone(p_degrade_falgs, p_ticket_prm, p_analyse, p_eticanalyse);

		if (ret.wErrCode != 0)
		{
			if (ret.wErrCode == ERR_ENTRY_STATION || ret.wErrCode == ERR_ENTRY_TIMEOUT || ret.wErrCode == ERR_OVER_PERIOD_L)
			{
				ret.bNoticeCode = NTC_TOKEN_RECLAIM;
			}
			break;
		}

		if (p_ticket_prm->entry_sell_station)
		{
			if (m_ticket_data.simple_status == SStatus_Es)
			{
				if (!g_Parameter.same_station(m_ticket_data.read_last_info.station_id, Api::current_station_id) && m_ticket_data.read_last_info.station_id != 0)
				{
					ret.wErrCode = ERR_CARD_USE;
					break;
				}
			}
		}
	}
    while (0);

	if (p_analyse != NULL)
	{
		p_analyse->dwOperationStauts |= oper_status;

		// 回收票不允许更新
		if (ret.bNoticeCode == NTC_TOKEN_RECLAIM)
		{
			p_analyse->dwOperationStauts &= ~ALLOW_UPDATE;
			p_analyse->lPenalty = 0;
		}
	}

    return ret;
}

uint32_t TicketUl::add_oper_status(uint8_t wk_area, RETINFO ret, TICKET_PARAM * p_ticket_prm)
{
	uint32_t oper_status = 0;

	if (g_Parameter.permit_sale(p_ticket_prm->sell_device))
	{
		// 零值回收票可发售
		if (ret.bNoticeCode == NTC_TOKEN_RECLAIM/* && m_ticket_data.wallet_value == 0*/)
			oper_status |= ALLOW_SALE;


		if (ret.wErrCode == ERR_CARD_REFUND)
			oper_status |= ALLOW_SALE;
	}

	// 可退款
	if (wk_area != 1 && p_ticket_prm->pemite_refund)
	{
		if (ret.wErrCode == 0 && m_ticket_data.wallet_value > 0 &&
			m_ticket_data.wallet_value <= p_ticket_prm->refund_max_value)
		{
			oper_status |= ALLOW_REFUND;
		}
	}

	return oper_status;
}

uint16_t TicketUl::read_equals_write(uint8_t * p_area_data)
{
	uint16_t ret			= 0;
	uint8_t ul_data[128]	= {0};

	int iret = read_ul(ul_data);

	if (iret == 0)
	{
		if (memcmp(p_area_data, ul_data + 32 + 16 * m_ulInfo.area_ptr, 16) != 0)
		{
			//#####################################################################
			g_Record.log_out(0, level_disaster, "3:%d", m_ulInfo.area_ptr);
			g_Record.log_buffer("area_write:", p_area_data, 16);
			g_Record.log_buffer("area_reread:", ul_data + 32 + 16 * m_ulInfo.area_ptr, 16);
			g_Record.log_buffer("area_read0:", m_ulInfo.area_buffer[0], 16);
			g_Record.log_buffer("area_read1:", m_ulInfo.area_buffer[1], 16);
			ret = ERR_CARD_WRITE;
		}
	}
	else
	{
		ret = ERR_CARD_READ;
	}

	return ret;
}

RETINFO TicketUl::write_card(ETYTKOPER operType, long trade_amount, long& sam_seq, char * p_trade_tac)
{
	RETINFO ret					= {0};
	int write_count				= 0;
	unsigned char logic_id[8]	= {0};
	unsigned char recv_buf[64]	= {0};

	m_ulInfo.area_ptr = 1 - m_ulInfo.area_ptr;
    // 组织处理信息区数据
    memcpy(m_ulInfo.area_data[m_ulInfo.area_ptr].last_time, m_ticket_data.write_last_info.time, 7);

    m_ulInfo.area_data[m_ulInfo.area_ptr].last_line =
		Publics::bcd_to_val((uint8_t)(m_ticket_data.write_last_info.station_id >> 8));
    m_ulInfo.area_data[m_ulInfo.area_ptr].last_station =
		Publics::bcd_to_val((uint8_t)(m_ticket_data.write_last_info.station_id & 0xFF));
    m_ulInfo.area_data[m_ulInfo.area_ptr].last_dvc_type =
		m_ticket_data.write_last_info.dvc_type;

    m_ulInfo.area_data[m_ulInfo.area_ptr].last_dvc_id =
		Publics::bcd_to_val(m_ticket_data.write_last_info.dvc_id);

    m_ulInfo.area_data[m_ulInfo.area_ptr].last_status = m_ticket_data.init_status;
    m_ulInfo.area_data[m_ulInfo.area_ptr].last_wallet_val = (uint16_t)m_ticket_data.wallet_value;

    m_ulInfo.area_data[m_ulInfo.area_ptr].entry_line =
		Publics::bcd_to_val((uint8_t)(m_ticket_data.p_entry_info->station_id >> 8));
    m_ulInfo.area_data[m_ulInfo.area_ptr].entry_station =
		Publics::bcd_to_val((uint8_t)(m_ticket_data.p_entry_info->station_id & 0xFF));

	m_ticket_data.total_trade_counter ++;
    m_ulInfo.area_data[m_ulInfo.area_ptr].trade_counter = (uint16_t)m_ticket_data.total_trade_counter;

	g_Record.log_out(0, level_disaster, "total_trade_counter=[%d]|trade_counter=[%ld]", (short)m_ulInfo.area_data[m_ulInfo.area_ptr].trade_counter,m_ticket_data.total_trade_counter);

	do
	{
		set_deal_area(&m_ulInfo.area_data[m_ulInfo.area_ptr], m_ulInfo.area_write);
		ret.wErrCode = add_ul_crc8(m_ulInfo.area_ptr, m_ulInfo.area_write);
		if (ret.wErrCode != 0)	
		{
			g_Record.log_out(0, level_error, "add_ul_crc8 err = [%04X]",ret.wErrCode);
			break;
		}

		//#####################################################################
		g_Record.log_out(0, level_disaster, "2:%d", m_ulInfo.area_ptr);

		// 如果票卡被移走，回读重写的次数太多，浪费时间
		for (write_count=0;write_count<2;write_count++)
		{
			ret.wErrCode = writeul_new(SAM_SOCK_1, m_ulInfo.area_ptr, m_ulInfo.area_write);
			if (ret.wErrCode == 0)
			{
				ret.wErrCode = read_equals_write(m_ulInfo.area_write);
				if (ret.wErrCode == 0)	
				{
					break;
				}
				g_Record.log_out(0, level_error, "read_equals_write = [%d] [%04X]",(write_count+1), ret.wErrCode);
				//rfdev_init();
				//rfdev_get_rfstatus(1);
				//rfdev_get_rfstatus(2);
				ISO14443A_Init(g_ant_type);
			}
		}
		if (ret.wErrCode != 0)
		{
			g_Record.log_out(0, level_error, "writeul_new err = [%04X]",ret.wErrCode);
			ret.wErrCode = ERR_CARD_WRITE;
			if (Api::current_device_type != dvcTVM)
			{
				ret.bNoticeCode = NTC_MUST_CONFIRM;
			}
			//rfdev_get_rfstatus(1);
			//rfdev_get_rfstatus(2);
			ISO14443A_Init(g_ant_type);
			break;
		}

		Publics::string_to_bcds(m_ticket_data.logical_id + 4, 16, logic_id, sizeof(logic_id));
		if (generate_tac(SAM_SOCK_1, logic_id, trade_amount, CmdSort::m_time_now, recv_buf) != 0)
		{
			g_Record.log_out(0, level_error, "generate_tac err");
			ret.wErrCode = ERR_CARD_WRITE;
			ret.bNoticeCode = NTC_MUST_CONFIRM;
			break;
		}

		sam_seq = (recv_buf[6] << 24) + (recv_buf[7] << 16) + (recv_buf[8] << 8) + recv_buf[9];

		if (p_trade_tac != NULL)
		{
			char sz_temp[16] = {0};
			sprintf(sz_temp, "%02X%02X%02X%02X  ", recv_buf[10], recv_buf[11], recv_buf[12], recv_buf[13]);
			memcpy(p_trade_tac, sz_temp, 10);
		}

	} while (0);

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		ret.bNoticeCode = support_confirm(operType);

	if (write_count > 1)
	{
		g_Record.log_out(ret.wErrCode, level_error, "write %s for %d times, result=(%04x,%02x)",
			m_ticket_data.logical_id, write_count, ret.wErrCode, ret.bNoticeCode);
	}

	return ret;
}

RETINFO TicketUl::entry_gate(PENTRYGATE p_entry,PETICKETDEALINFO p_eticketdealinfo)
{
    RETINFO ret		= {0};
	long lsamseq	= 0;

    m_ticket_data.init_status	= MS_Entry;

    current_trade_info(*m_ticket_data.p_entry_info);
    current_trade_info(m_ticket_data.write_last_info);

    ret = write_card(operEntry, 0, lsamseq, NULL);
	p_entry->lSAMTrSeqNo = lsamseq;

    // 交易记录赋值
    transfer_data_for_out(operEntry, p_entry);
	p_entry->bStatus = LIFE_ENTRY;

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
	{
		g_Record.log_out(0, level_error,"TicketUl::entry_gate-- save_last_stack");
		save_last_stack(operEntry, p_entry, sizeof(ENTRYGATE), m_ulInfo.area_write, sizeof(m_ulInfo.area_write), false);
	}

    return ret;
}

RETINFO TicketUl::exit_gate(PPURSETRADE p_exit,PETICKETDEALINFO p_eticketdealinfo, MODE_EFFECT_FLAGS * p_degrade_falgs)
{
    RETINFO ret		= {0};
	long lsamseq	= 0;

	if (p_degrade_falgs->train_trouble)
		m_ticket_data.init_status = MS_Exit_Tt;
	else
	    m_ticket_data.init_status	= MS_Exit;
    m_ticket_data.wallet_value -= p_exit->lTradeAmount;

    m_ticket_data.p_exit_info = &m_ticket_data.write_last_info;
    current_trade_info(*m_ticket_data.p_exit_info);

    ret = write_card(operExit, p_exit->lTradeAmount, lsamseq, p_exit->cMACorTAC);
	p_exit->lSAMTrSeqNo = lsamseq;

    // 交易记录赋值
    transfer_data_for_out(operExit, p_exit);
	if (m_ticket_data.simple_status == SStatus_Upd_WZone)
		memcpy(p_exit->cPaymentType, "1C", sizeof(p_exit->cPaymentType));
	else
		memcpy(p_exit->cPaymentType, "12", sizeof(p_exit->cPaymentType));

	memset(p_exit->cClassicType, '0', sizeof(p_exit->cClassicType));

	if (m_ticket_data.simple_status == SStatus_Exit_Only)
		p_exit->bStatus = LIFE_EXIT_TK_EXIT;
	else
		p_exit->bStatus = LIFE_EXIT;

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
	{
		save_last_stack(operExit, p_exit, sizeof(PURSETRADE), m_ulInfo.area_write, sizeof(m_ulInfo.area_write), false);
	}

	if (p_degrade_falgs->train_trouble)
	{
		if (m_ticket_data.simple_status != SStatus_Exit_Only && m_ticket_data.wallet_value != 0)
		{
			ret.bNoticeCode = NTC_EXIT_TRAIN_TROUBLE;
		}
	}

    return ret;
}

RETINFO TicketUl::sale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, POTHERSALE p_svt_sale, uint16_t& len_svt, TICKET_PARAM& ticket_prm)
{
    RETINFO ret		= {0};
	long lsamseq	= 0;

	len_svt = 0;

	do
	{
		if (p_sjt_sale->nChargeValue > ticket_prm.balance_max)
		{
			ret.wErrCode = ERR_OVER_WALLET;
			break;
		}

		ret.wErrCode = g_Parameter.fare_zone_by_sjt_fare(m_ticket_data.logical_type, p_sjt_sale->nChargeValue, p_sjt_sale->bZoneID);
		if (ret.wErrCode != 0)
			break;

		m_ticket_data.init_status	= MS_Sale;
		m_ticket_data.wallet_value = p_sjt_sale->nChargeValue;

		current_trade_info(m_ticket_data.write_last_info);

		ret = write_card(operSjtSale, p_sjt_sale->nChargeValue, lsamseq, p_sjt_sale->cMACorTAC);
		p_sjt_sale->lSAMTrSeqNo = lsamseq;

		if (ret.wErrCode == 0)
			len_sjt = sizeof(SJTSALE);

		// 交易记录赋值
		transfer_data_for_out(operSjtSale, p_sjt_sale);
		p_sjt_sale->bStatus = LIFE_SALE;
		memset(p_sjt_sale->cClassicType, '0', sizeof(p_sjt_sale->cClassicType));

		if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		{
			g_Record.log_out(0, level_error,"TicketUl::sale -- save_last_stack");
			save_last_stack(operSjtSale, p_sjt_sale, sizeof(SJTSALE), m_ulInfo.area_write, sizeof(m_ulInfo.area_write), true);
		}

	} while (0);

    return ret;
}


RETINFO TicketUl::esale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, PETICKETDEALINFO p_eticketdealinfo, uint16_t& len_eticket_sale, TICKET_PARAM& ticket_prm)
{
	RETINFO ret		= {0};
	long lsamseq	= 0;

	//len_svt = 0;

	do
	{
		if (p_sjt_sale->nChargeValue > ticket_prm.balance_max)
		{
			ret.wErrCode = ERR_OVER_WALLET;
			break;
		}

		ret.wErrCode = g_Parameter.fare_zone_by_sjt_fare(m_ticket_data.logical_type, p_sjt_sale->nChargeValue, p_sjt_sale->bZoneID);
		if (ret.wErrCode != 0)
			break;

		m_ticket_data.init_status	= MS_Sale;
		m_ticket_data.wallet_value = p_sjt_sale->nChargeValue;

		memcpy(m_ticket_data.qr_ticketdata.cardNo, p_eticketdealinfo->cTicketLogicalNo, 20);//逻辑卡号
		memcpy(m_ticket_data.qr_ticketdata.centerCode, p_eticketdealinfo->cCenterCode, 32);//中心票号
		m_ticket_data.qr_ticketdata.balance = p_sjt_sale->nChargeValue;//交易金额

		current_trade_info(m_ticket_data.write_last_info);

		ret = write_card(operSjtSale, p_sjt_sale->nChargeValue, lsamseq, p_sjt_sale->cMACorTAC);
		p_sjt_sale->lSAMTrSeqNo = lsamseq;

		if (ret.wErrCode == 0)
		{
			len_sjt = sizeof(SJTSALE);
			len_eticket_sale = sizeof(ETICKETDEALINFO);
		}

		// 交易记录赋值
		transfer_data_for_out(operSjtSale, p_sjt_sale);
		p_sjt_sale->bStatus = LIFE_SALE;
		memset(p_sjt_sale->cClassicType, '0', sizeof(p_sjt_sale->cClassicType));

		transfer_edata_for_out(operGetETicketRecord, p_eticketdealinfo/*,p_sjt_sale->bPaymentMeans*/);

		ret.bRfu[0] = 0x11;
		ret.bRfu[1] = 0x00;

		if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		{
			g_Record.log_out(0, level_error,"TicketUl::esale -- save_last_stack");
			save_last_stack(operSjtSale, p_sjt_sale, sizeof(SJTSALE), m_ulInfo.area_write, sizeof(m_ulInfo.area_write), true);
		}

	} while (0);

	return ret;
}


RETINFO TicketUl::thirdesale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, PETICKETDEALINFO p_eticketdealinfo, uint16_t& len_eticket_sale, TICKET_PARAM& ticket_prm, char * cNetTranNumber)
{
	RETINFO ret		= {0};
	long lsamseq	= 0;

	//len_svt = 0;

	do
	{
		if (p_sjt_sale->nChargeValue > ticket_prm.balance_max)
		{
			ret.wErrCode = ERR_OVER_WALLET;
			break;
		}

		ret.wErrCode = g_Parameter.fare_zone_by_sjt_fare(m_ticket_data.logical_type, p_sjt_sale->nChargeValue, p_sjt_sale->bZoneID);
		if (ret.wErrCode != 0)
			break;

		m_ticket_data.init_status	= MS_Sale;
		m_ticket_data.wallet_value = p_sjt_sale->nChargeValue;
		//memcpy(m_ticket_data.qr_ticketdata.cardNo, p_eticketdealinfo->cTicketLogicalNo, 20);//逻辑卡号
		//memcpy(m_ticket_data.qr_ticketdata.cardNo, m_ticket_data.logical_id, 20);
		memcpy(m_ticket_data.qr_ticketdata.NetTranNumber, cNetTranNumber, 64);//互联网订单号
		memcpy(m_ticket_data.qr_ticketdata.mac, "00000000", 8);//mac
		m_ticket_data.qr_ticketdata.balance = p_sjt_sale->nChargeValue;//交易金额

		current_trade_info(m_ticket_data.write_last_info);

		ret = write_card(operSjtSale, p_sjt_sale->nChargeValue, lsamseq, p_sjt_sale->cMACorTAC);
		p_sjt_sale->lSAMTrSeqNo = lsamseq;

		if (ret.wErrCode == 0)
		{
			len_sjt = sizeof(SJTSALE);
			len_eticket_sale = sizeof(ETICKETDEALINFO);
		}

		// 交易记录赋值
		transfer_data_for_out(operSjtSale, p_sjt_sale);
		p_sjt_sale->bStatus = LIFE_SALE;
		memset(p_sjt_sale->cClassicType, '0', sizeof(p_sjt_sale->cClassicType));

		transfer_edata_for_out(operThirdGetETicket, p_eticketdealinfo/*,p_sjt_sale->bPaymentMeans*/);

		ret.bRfu[0] = 0x11;
		ret.bRfu[1] = 0x00;

		if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		{
			g_Record.log_out(0, level_error,"TicketUl::esale -- save_last_stack");
			save_last_stack(operSjtSale, p_sjt_sale, sizeof(SJTSALE), m_ulInfo.area_write, sizeof(m_ulInfo.area_write), true);
		}

	} while (0);

	return ret;
}

RETINFO TicketUl::exit_sjt_sale(PSJTSALE p_exit_sjt_sale)
{
    RETINFO ret		= {0};
	long lsamseq	= 0;

    m_ticket_data.init_status	= MS_Exit_Only;
	m_ticket_data.wallet_value	= p_exit_sjt_sale->nChargeValue;

    current_trade_info(m_ticket_data.write_last_info);

    ret = write_card(operSjtSale, p_exit_sjt_sale->nChargeValue, lsamseq, p_exit_sjt_sale->cMACorTAC);
	p_exit_sjt_sale->lSAMTrSeqNo = lsamseq;

    // 交易记录赋值
    transfer_data_for_out(operSjtSale, p_exit_sjt_sale);
	p_exit_sjt_sale->bStatus = LIFE_EXIT_ONLY;
	p_exit_sjt_sale->bPaymentMeans = 0x05;	// 新政处理收费
	memset(p_exit_sjt_sale->cClassicType, '0', sizeof(p_exit_sjt_sale->cClassicType));

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
	{
		save_last_stack(operSjtSale, p_exit_sjt_sale, sizeof(SJTSALE), m_ulInfo.area_write, sizeof(m_ulInfo.area_write), true);
	}

    return ret;
}

RETINFO TicketUl::sjt_decrease()
{
	RETINFO ret		= {0};
	long lsamseq	= 0;

	m_ticket_data.init_status	= MS_Exit;
	m_ticket_data.wallet_value  = 0;

	ret = write_card(operDecr, m_ticket_data.wallet_value, lsamseq, NULL);
	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		ret.bNoticeCode = 0;

	return ret;
}

RETINFO TicketUl::bom_update(PTICKETUPDATE p_update, uint8_t * p_entry_station)
{
    RETINFO ret		= {0};
	long lsamseq	= 0;
	long fare		= 0;

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
    case 0x10:		// 非付费区有进站码
        m_ticket_data.init_status = MS_Upd_FZone_Free;
		p_update->bStatus = LIFE_UPD_OUT_FREE;
        break;
	default:
		ret.wErrCode = ERR_INPUT_PARAM;
		break;
    }

	if (ret.wErrCode == 0)
	{
		m_ticket_data.p_update_info = &m_ticket_data.write_last_info;
		current_trade_info(*m_ticket_data.p_update_info);

		ret = write_card(operUpdate, 0, lsamseq, NULL);
		p_update->lSAMTrSeqNo = lsamseq;

		// 交易记录赋值
		transfer_data_for_out(operUpdate, p_update);

		if (ret.bNoticeCode == NTC_MUST_CONFIRM)
		{
			save_last_stack(operUpdate, p_update, sizeof(TICKETUPDATE), m_ulInfo.area_write, sizeof(m_ulInfo.area_write), true);
		}
	}
    return ret;
}

RETINFO TicketUl::bom_refund(PDIRECTREFUND p_refund)
{
    RETINFO ret		= {0};
	long lsamseq	= 0;

    m_ticket_data.init_status = MS_Refund;
    p_refund->lBalanceReturned = m_ticket_data.wallet_value;
    m_ticket_data.wallet_value -= p_refund->lBalanceReturned;

    current_trade_info(m_ticket_data.write_last_info);

    // 交易记录赋值
    if (p_refund->bForfeitReason == 6)
        p_refund->bReturnTypeCode = 0x33;
    else
        p_refund->bReturnTypeCode = 0x32;

    ret = write_card(operRefund, p_refund->lBalanceReturned, lsamseq, p_refund->cMACOrTAC);
	p_refund->lSAMTrSeqNo = lsamseq;

    transfer_data_for_out(operRefund, p_refund);
	p_refund->bStatus = LIFE_REFUND;
	memset(p_refund->cClassicType, '0', sizeof(p_refund->cClassicType));

	if (ret.bNoticeCode == NTC_MUST_CONFIRM)
	{
		save_last_stack(operRefund, p_refund, sizeof(DIRECTREFUND), m_ulInfo.area_write, sizeof(m_ulInfo.area_write), true);
	}

    return ret;

}

bool TicketUl::last_trade_need_continue(uint8_t status_targ, uint16_t err_targ, uint16_t err_src)
{
	CONFIRM_FACTOR factor = get_confirm_factor();

	bool ret = check_confirm(status_targ, m_ticket_data.init_status, err_targ, err_src);
	if (ret)
	{
		if (memcmp(m_ulInfo.area_buffer[m_ulInfo.area_ptr], cfm_point.dataWritten, 16) != 0)
		{
			memset(&cfm_point, 0, sizeof(cfm_point));
			ret = false;
		}
		else
		{
			ret = true;
		}
	}

	return ret;
}

RETINFO TicketUl::continue_last_trade(void * p_trade)
{
	RETINFO ret					= {0};
	unsigned char recv_buf[64]	= {0};
	unsigned char uc_id[8]		= {0};
	long sam_seq				= 0;
	char tac[11]				= {0};
	uint8_t status				= LIFE_CONFIRM;

	do
	{
		CONFIRM_FACTOR factor = get_confirm_factor();

		Publics::string_to_bcds(factor.logic_id + 4, 16, uc_id, sizeof(uc_id));
		if (generate_tac(SAM_SOCK_1, uc_id, factor.trade_amount, factor.trade_time, recv_buf) != 0)
		{
			ret.wErrCode = ERR_CARD_WRITE;
			break;
		}

		sam_seq = (recv_buf[6] << 24) + (recv_buf[7] << 16) + (recv_buf[8] << 8) + recv_buf[9];
		sprintf(tac, "%02X%02X%02X%02X  ", recv_buf[10], recv_buf[11], recv_buf[12], recv_buf[13]);
		set_confirm_factor(status, sam_seq, tac);

		memcpy(p_trade, &cfm_point.trade, cfm_point.size_trade);

	} while (0);

	g_Record.log_out(ret.wErrCode, level_error, "continue_last_trade(void * p_trade)=%d", ret.wErrCode);

	return ret;
}

// 赋值给上次交易未赋值的交易因子
//void TicketUl::set_confirm_factor(uint8_t status, long sam_seq, char * p_tac)
//{
//	uint8_t trade_type = Publics::string_to_hex<uint8_t>((char *)(&cfm_point.trade), 2);
//	switch(trade_type)
//	{
//	case 0x50:	// 单程票发售
//		cfm_point.trade.sjtSale.bStatus		= status;
//		cfm_point.trade.sjtSale.lSAMTrSeqNo = sam_seq;
//		memcpy(cfm_point.trade.sjtSale.cMACorTAC, p_tac, 10);
//		break;
//	//case 0x51:	// 储值票发售
//	//	cfm_point.trade.svtSale.bStatus		= status;
//	//	cfm_point.trade.svtSale.lSAMTrSeqNo = sam_seq;
//	//	break;
//	case 0x53:	// 进闸
//		cfm_point.trade.entry.bStatus		= status;
//		cfm_point.trade.entry.lSAMTrSeqNo = sam_seq;
//		break;
//	case 0x54:	// 钱包交易
//		cfm_point.trade.purse.bStatus		= status;
//		cfm_point.trade.purse.lSAMTrSeqNo	= sam_seq;
//		memcpy(cfm_point.trade.purse.cMACorTAC, p_tac, 10);
//		break;
//	case 0x56:	// 更新
//		cfm_point.trade.update.bStatus		= status;
//		cfm_point.trade.update.lSAMTrSeqNo	= sam_seq;
//		break;
//	case 0x57:	// 退款
//		cfm_point.trade.refund.bStatus		= status;
//		cfm_point.trade.refund.lSAMTrSeqNo	= sam_seq;
//		memcpy(cfm_point.trade.refund.cMACOrTAC, p_tac, 10);
//		break;
//	}
//}
