#include "TicketUl.h"
#include "TimesEx.h"
#include "Publics.h"
#include "DataSecurity.h"
#include "Errors.h"
#include "../link/linker.h"


TicketUl::TicketUl(void)
{
}

TicketUl::TicketUl(char * p_current_sam, char * p_sam_posid, uint8_t * physic_info)
{
    m_p_current_sam = p_current_sam;
	m_p_sam_posid = p_sam_posid;

    memset(&m_ticket_data, 0, sizeof(m_ticket_data));

    sprintf(m_ticket_data.physical_id, "%02X%02X%02X%02X%02X%02X%02X%-6C",
            physic_info[0], physic_info[1], physic_info[2], physic_info[3], physic_info[4], physic_info[5], physic_info[6], 0x20);

    uint8_t * pMemoryPos = ((uint8_t *)this) + sizeof(TicketUl);
    m_ticket_data.p_entry_info = new(pMemoryPos) TradeInfo;

    memset(m_ticket_data.p_entry_info, 0, sizeof(TradeInfo));
    memset(&m_ticket_data.read_last_info, 0, sizeof(TradeInfo));
}

uint16_t TicketUl::read_ul(uint8_t * p_ul_read_buf)
{
	return readul(SAM_SOCK_1, p_ul_read_buf);
}

uint16_t TicketUl::write_ul(uint8_t valid_area_ptr, uint8_t * p_ul_area_data)
{
	uint16_t ret = 0;
	do 
	{
		//#####################
		g_Record.log_out(0, level_disaster, "5:%d", valid_area_ptr);

		ret = add_ul_crc8(valid_area_ptr, p_ul_area_data);
		if (ret != 0)	break;

		//#####################
		g_Record.log_out(0, level_disaster, "6:%d", valid_area_ptr);

		ret = writeul_new(SAM_SOCK_1, valid_area_ptr, p_ul_area_data);

		//#####################
		g_Record.log_out(0, level_disaster, "7 end:%d", valid_area_ptr);

	} while (0);

	return ret;
}

RETINFO TicketUl::read_card(ETYTKOPER operType, TICKET_PARAM& ticket_prm, uint8_t * p_read_init/* = NULL*/)
{
    RETINFO ret				= {0};
    uint8_t ul_data[256]	= {0};
    int result_read			= 0;
	uint16_t counter[2]		= {0};
    UL_DEAL_AREA area_read;
	uint16_t tmp			= 0;

    do
    {
        memset(&area_read, 0, sizeof(area_read));

        result_read = read_ul(ul_data);

		counter[0] = (uint16_t)((ul_data[45] << 8)  + ul_data[46]);
		counter[1] = (uint16_t)((ul_data[61] << 8)  + ul_data[62]);

        switch (result_read)
        {
        case 0:			// 两个crc都正确
			if (counter[0] > counter[1])
			{
				if (counter[0] != 0xffff)
					m_valid_area_ptr = 0;
				else
					m_valid_area_ptr = 1;
			}
			else if (counter[0] < counter[1])
			{
				if (counter[1] != 0xffff)
					m_valid_area_ptr = 1;
				else
					m_valid_area_ptr = 0;
			}
			if (counter[0] == counter[1])		// 交易计数相等
			{
				if (Api::current_device_type == dvcTVM || operType == operSale)
					m_valid_area_ptr = 0;
				else
					ret.wErrCode = ERR_CARD_INVALID;
			}

            break;

        case 1:			// 只有0区crc错误
            m_valid_area_ptr = 1;
            break;

        case 2:			// 只有1区crc错误
            m_valid_area_ptr = 0;
            break;

        case 3:			// 两个crc都错误
			if (Api::current_device_type == dvcTVM || operType == operSale)
				m_valid_area_ptr = 0;
			else
				ret.wErrCode = ERR_CARD_INVALID;
           break;

        default:
			g_Record.log_out(0, level_error, "read_card-default");
            ret.wErrCode = ERR_CARD_READ;
            break;
        }

        if (ret.wErrCode != 0)	break;

		if (p_read_init != NULL)
			memcpy(p_read_init, ul_data, 64);

        sprintf(m_ticket_data.logical_id, "0000%04X%04X%02X%02X%02X%02X", 0x0000, 0x0003, ul_data[16], ul_data[17], ul_data[18], ul_data[19]);

		//#####################
		g_Record.log_out(0, level_disaster, "%s-1:%d", m_ticket_data.logical_id, m_valid_area_ptr);

        TimesEx::tm2_bcd4_exchange(ul_data + 24, m_ticket_data.date_issue, true);
        m_ticket_data.key_flag = ul_data[25] & 0x01;

        memcpy(m_ticket_data.logical_type, ul_data + 26, 2);
		memcpy(m_ticket_data.fare_type, m_ticket_data.logical_type, 2);

		for (int nCheck=0;nCheck<2;nCheck++)
		{
			memcpy(m_read_area, ul_data + 32 + 16 * m_valid_area_ptr, 16);
			get_deal_area(area_read, ul_data + 32 + 16 * m_valid_area_ptr);

			if (Api::current_device_type == dvcTVM || operType == operSale)
				break;

			// 根据票卡余额和交易计数校正票卡有效的处理信息区
			// 余额有分出现可能票卡数据存在异常
			if (area_read.last_wallet_val % 10 != 0 && result_read == 0)
			{
				if (counter[m_valid_area_ptr] > counter[1 - m_valid_area_ptr])		// 有效区域的计数大于无效区域的计数
				{
					if (counter[m_valid_area_ptr] - counter[1 - m_valid_area_ptr] != 1)
					{
						m_valid_area_ptr = 1 - m_valid_area_ptr;
						continue;
					}
				}
				else	// 有效区域的计数小于无效区域的计数，说明无效区域的计数应该为0xffff
				{
					if (counter[m_valid_area_ptr] != 0)
					{
						m_valid_area_ptr = 1 - m_valid_area_ptr;
						continue;
					}
				}
			}
		}

        memcpy(m_ticket_data.read_last_info.time, area_read.last_time, 7);

		tmp = area_read.last_line * 100 + area_read.last_station;
        m_ticket_data.read_last_info.station_id = Publics::val_to_bcd(tmp);

        m_ticket_data.read_last_info.dvc_type = (ETPDVC)area_read.last_dvc_type;
        m_ticket_data.read_last_info.dvc_id = Publics::val_to_bcd(area_read.last_dvc_id);
		g_Parameter.sam_from_device(m_ticket_data.read_last_info.station_id, m_ticket_data.read_last_info.dvc_type,
			m_ticket_data.read_last_info.dvc_id, SAM_SOCK_1, m_ticket_data.read_last_info.sam);

        m_ticket_data.init_status = area_read.last_status;
        m_ticket_data.wallet_value = area_read.last_wallet_val;
		tmp = area_read.entry_line * 100 + area_read.entry_station;
        m_ticket_data.p_entry_info->station_id = Publics::val_to_bcd(tmp);
        memcpy(m_ticket_data.p_entry_info->time, area_read.last_time, 7);
		memcpy(m_ticket_data.p_entry_info->sam, m_ticket_data.read_last_info.sam, 16);
        m_ticket_data.total_trade_counter = area_read.trade_counter;

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

		//#####################
		g_Record.log_out(0, level_disaster, "2:%d", m_valid_area_ptr);

    }
    while (0);

    return ret;
}

void TicketUl::get_deal_area(UL_DEAL_AREA& deal_info, uint8_t * p_deal_area)
{
    TimesEx::tm4_bcd7_exchange(p_deal_area, deal_info.last_time, true);

    //cmd_scanf(7, p_deal_area + 4,
    //          _IN_P(6, deal_info.last_line),
    //          _IN_P(6, deal_info.last_station),
    //          _IN_P(4, deal_info.last_dvc_type),
    //          _IN_P(10, deal_info.last_dvc_id),
    //          _IN_P(14, deal_info.last_wallet_val),
    //          _IN_P(5, deal_info.last_status),
    //          _IN_P(3, deal_info.last_status_special),
    //          _IN_P(6, deal_info.entry_line),
    //          _IN_P(6, deal_info.entry_station),
    //          ENDLE);

	deal_info.last_line = (uint8_t)((p_deal_area[4] >> 2) & 0x3F);
	deal_info.last_station = (uint8_t)(((p_deal_area[4] & 0x03) << 4) + ((p_deal_area[5] >> 4) & 0x0F));
	deal_info.last_dvc_type = (uint8_t)(p_deal_area[5] & 0x0F);
	deal_info.last_dvc_id = (uint16_t)((p_deal_area[6] << 2) + (p_deal_area[7] >> 6));
	deal_info.last_wallet_val = (uint16_t)(((p_deal_area[7] & 0x3F) << 8) + p_deal_area[8]);
	deal_info.last_status = (uint8_t)(p_deal_area[9] >> 3);
	deal_info.last_status_special = (uint8_t)(p_deal_area[9] & 0x07);
	deal_info.entry_line = (uint8_t)((p_deal_area[10] >> 2) & 0x3F);
	deal_info.entry_station = (uint8_t)(((p_deal_area[10] & 0x03) << 4) + ((p_deal_area[11] >> 4) & 0x0F));

    deal_info.trade_counter = (uint16_t)((p_deal_area[13] << 8) + p_deal_area[14]);

}

void TicketUl::set_deal_area(UL_DEAL_AREA& deal_info, uint8_t * p_deal_area)
{
    TimesEx::tm4_bcd7_exchange(p_deal_area, deal_info.last_time, false);
    //cmd_sprintf(7, p_deal_area + 4,
    //            _OUT_B(6, deal_info.last_line),
    //            _OUT_B(6, deal_info.last_station),
    //            _OUT_B(4, deal_info.last_dvc_type),
    //            _OUT_B(10, deal_info.last_dvc_id),
    //            _OUT_B(14, deal_info.last_wallet_val),
    //            _OUT_B(5, deal_info.last_status),
    //            _OUT_B(3, deal_info.last_status_special),
    //            _OUT_B(6, deal_info.entry_line),
    //            _OUT_B(6, deal_info.entry_station),
    //            ENDLE);

	p_deal_area[4] = (uint8_t)((deal_info.last_line << 2) + ((deal_info.last_station >> 4) &0x03));
	p_deal_area[5] = (uint8_t)(((deal_info.last_station & 0x0F) << 4) + (deal_info.last_dvc_type & 0x0F));
	p_deal_area[6] = (uint8_t)((deal_info.last_dvc_id >> 2) & 0xFF);
	p_deal_area[7] = (uint8_t)(((deal_info.last_dvc_id & 0x03) << 6) + ((deal_info.last_wallet_val >> 8) & 0x3F));
	p_deal_area[8] = (uint8_t)(deal_info.last_wallet_val & 0xFF);
	p_deal_area[9] = (uint8_t)(((deal_info.last_status & 0x1F) << 3) + (deal_info.last_status_special & 0x07));
	p_deal_area[10] = (uint8_t)((deal_info.entry_line << 2) + ((deal_info.entry_station >> 4) &0x03));
	p_deal_area[11] = (uint8_t)(((deal_info.entry_station & 0x0F) << 4) + (p_deal_area[11] & 0x0F));

    p_deal_area[13] = (uint8_t)(deal_info.trade_counter >> 8);
    p_deal_area[14] = (uint8_t)(deal_info.trade_counter & 0xFF);

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
		//if (p_ticket_prm->check_phy_expire && need_check_period(wk_area, m_ticket_data.simple_status))
		//{
		//	if (memcmp(CmdSort::m_time_now, m_ticket_data.phy_peroidE, 4) > 0)
		//	{
		//		if (p_degrade_falgs->train_trouble && wk_area == 1)
		//		{
		//			mode = (char)(0x30 + degrade_Trouble);
		//		}
		//		else
		//		{
		//			ret.wErrCode = ERR_OVER_PERIOD_P;
		//			break;
		//		}
		//	}
		//}

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

RETINFO TicketUl::analyse_work_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PPURSETRADE p_purse)
{
	RETINFO ret = {0};

	ret = TicketBase::analyse_work_zone(p_degrade_falgs, p_ticket_prm, p_analyse, p_purse);

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

RETINFO TicketUl::analyse_free_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse)
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

        ret = TicketBase::analyse_free_zone(p_degrade_falgs, p_ticket_prm, p_analyse);

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
	uint8_t ul_data[256]	= {0};
	uint8_t ul_temp[16]	= {0};

	int iret = read_ul(ul_data);

	if (iret == 0)
	{
		//g_Record.log_out(0, level_disaster, "ul_data:[%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x]", 
			//ul_data[32],ul_data[33],ul_data[34],ul_data[35],ul_data[36],ul_data[37],ul_data[38],ul_data[39],
			//ul_data[40],ul_data[41],ul_data[42],ul_data[43],ul_data[44],ul_data[45],ul_data[46],ul_data[47],
			//ul_data[48],ul_data[49],ul_data[50],ul_data[51],ul_data[52],ul_data[53],ul_data[54],ul_data[55],
			//ul_data[56],ul_data[57],ul_data[58],ul_data[59],ul_data[60],ul_data[61],ul_data[62],ul_data[63]);

		if (memcmp(p_area_data, ul_data + 32 + 16 * m_valid_area_ptr, 15) != 0)
		{
			g_Record.log_out(0, level_disaster, "read_equals_write:4:%d, Reverse", m_valid_area_ptr);
			g_Record.log_out(0, level_disaster, "read_equals_write:p_area_data:[%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x]",
				p_area_data[0],p_area_data[1],p_area_data[2],p_area_data[3],p_area_data[4],p_area_data[5],p_area_data[6],p_area_data[7],
				p_area_data[8],p_area_data[9],p_area_data[10],p_area_data[11],p_area_data[12],p_area_data[13],p_area_data[14],p_area_data[15]);
			memcpy(ul_temp,ul_data + 32 + 16 * m_valid_area_ptr, 15);
			g_Record.log_out(0, level_disaster, "read_equals_write:ul_temp:[02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x]",
				ul_temp[0],ul_temp[1],ul_temp[2],ul_temp[3],ul_temp[4],ul_temp[5],ul_temp[6],ul_temp[7],
				ul_temp[8],ul_temp[9],ul_temp[10],ul_temp[11],ul_temp[12],ul_temp[13],ul_temp[14]);
			ret = ERR_CARD_WRITE;
		}
	}
	else
	{
		g_Record.log_out(0, level_disaster, "ERR_CARD_READ");
		g_Record.log_out(0, level_disaster, "read_ul:%d",iret);
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
    UL_DEAL_AREA area_write;

	//#####################
	g_Record.log_out(0, level_disaster, "3:%d", m_valid_area_ptr);

    memset(&area_write, 0, sizeof(area_write));
    // 组织处理信息区数据
    memcpy(area_write.last_time, m_ticket_data.write_last_info.time, 7);

    area_write.last_line = Publics::bcd_to_val((uint8_t)(m_ticket_data.write_last_info.station_id >> 8));
    area_write.last_station = Publics::bcd_to_val((uint8_t)(m_ticket_data.write_last_info.station_id & 0xFF));

    area_write.last_dvc_type = m_ticket_data.write_last_info.dvc_type;

    area_write.last_dvc_id = Publics::bcd_to_val(m_ticket_data.write_last_info.dvc_id);

    area_write.last_status = m_ticket_data.init_status;
    area_write.last_wallet_val = (uint16_t)m_ticket_data.wallet_value;

    area_write.entry_line = Publics::bcd_to_val((uint8_t)(m_ticket_data.p_entry_info->station_id >> 8));
    area_write.entry_station = Publics::bcd_to_val((uint8_t)(m_ticket_data.p_entry_info->station_id & 0xFF));

	m_ticket_data.total_trade_counter ++;
    area_write.trade_counter = (uint16_t)m_ticket_data.total_trade_counter;

    m_valid_area_ptr = 1 - m_valid_area_ptr;

	//#####################
	g_Record.log_out(0, level_disaster, "4:%d, Reverse", m_valid_area_ptr);

    set_deal_area(area_write, m_write_area);


	do
	{
		// 如果票卡被移走，回读重写的次数太多，浪费时间
		for (write_count=0;write_count<2;write_count++)
		{
			//g_Record.log_out(0, level_disaster, "write_ul_begin[%d]:m_valid_area_ptr[%d]:[%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x]"
			//,write_count+1,m_valid_area_ptr,m_write_area[0],m_write_area[1],m_write_area[2],m_write_area[3]
			//,m_write_area[4],m_write_area[5],m_write_area[6],m_write_area[7],m_write_area[8],m_write_area[9]
			//,m_write_area[10],m_write_area[11],m_write_area[12],m_write_area[13],m_write_area[14]);
			ret.wErrCode = write_ul(m_valid_area_ptr, m_write_area);
			if (ret.wErrCode == 0)
			{
				ret.wErrCode = read_equals_write(m_write_area);
				if (ret.wErrCode == 0)	break;
				rfdev_init();
			}
		}
		if (ret.wErrCode != 0)
		{
			ret.wErrCode = ERR_CARD_WRITE;
			if (Api::current_device_type != dvcTVM)
			{
				ret.bNoticeCode = NTC_MUST_CONFIRM;
			}
				
			break;
		}

		Publics::string_to_bcds(m_ticket_data.logical_id + 4, 16, logic_id, sizeof(logic_id));
		if (generate_tac(SAM_SOCK_1, logic_id, trade_amount, CmdSort::m_time_now, recv_buf) != 0)
		{
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

RETINFO TicketUl::entry_gate(PENTRYGATE p_entry)
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
		save_last_stack(operEntry, p_entry, sizeof(ENTRYGATE), m_write_area, sizeof(m_write_area), false);
	}

    return ret;
}

RETINFO TicketUl::exit_gate(PPURSETRADE p_exit, MODE_EFFECT_FLAGS * p_degrade_falgs)
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
		save_last_stack(operExit, p_exit, sizeof(PURSETRADE), m_write_area, sizeof(m_write_area), false);
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
			save_last_stack(operSjtSale, p_sjt_sale, sizeof(SJTSALE), m_write_area, sizeof(m_write_area), true);
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
		save_last_stack(operSjtSale, p_exit_sjt_sale, sizeof(SJTSALE), m_write_area, sizeof(m_write_area), true);
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
			save_last_stack(operUpdate, p_update, sizeof(TICKETUPDATE), m_write_area, sizeof(m_write_area), true);
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
		save_last_stack(operRefund, p_refund, sizeof(DIRECTREFUND), m_write_area, sizeof(m_write_area), true);
	}

    return ret;

}

bool TicketUl::last_trade_need_continue(uint8_t status_targ, uint16_t err_targ, uint16_t err_src)
{
	CONFIRM_FACTOR factor = get_confirm_factor();

	bool ret = check_confirm(status_targ, m_ticket_data.init_status, err_targ, err_src);
	if (ret)
	{
		if (memcmp(m_read_area, cfm_point.dataWritten, 16) != 0)
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
