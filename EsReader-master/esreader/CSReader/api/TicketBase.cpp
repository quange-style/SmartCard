#include "TicketBase.h"
#include "TimesEx.h"
#include "Api.h"
#include "TicketUl.h"
#include "TicketSvt.h"
#include "TicketBus.h"
#include "TicketBM.h"
#include "Publics.h"
#include "Structs.h"
#include "CmdSort.h"
#include "Errors.h"
#include "../link/linker.h"
#include "DataSecurity.h"
#include <stdarg.h>
#include <unistd.h>

CONFIRM_POINT TicketBase::cfm_point = {0};

TicketBase::TicketBase(void)
{
}

TicketBase::TicketBase(char * p_current_sam, char * p_sam_posid, uint8_t * physic_info)
	: m_p_current_sam(p_current_sam), m_p_sam_posid()
{

}

uint16_t TicketBase::search_card(TicketBase ** pp_ticket, uint8_t * p_forbid_ant, ETPMDM * p_forbid_type, uint8_t * p_current_id, ETYTKOPER oper_type)
{
	uint16_t ret			= 0;
	uint8_t recv[32]		= {0};
	ETPMDM physical_type	= mediNone;
	unsigned char type		= 0;
	char mode_app			= '0';

	do
	{
		ret = rf_modify(Api::current_device_type, CmdSort::m_antenna_mode, oper_type, &g_rfmain_status, &g_rfsub_status);
		if (ret != 0)
		{
		    //g_Record.log_out(ret, level_error, "rf_modify");
			ret = ERR_CARD_NONE;
			break;
		}

		ret = search_card_ex(recv, type);
		if (ret != 0)	break;

		physical_type = (ETPMDM)type;

		//physical_type = (ETPMDM)searchcard(recv);
		if ((p_forbid_ant != NULL && *p_forbid_ant == CmdSort::m_antenna_mode) && (p_forbid_type != NULL && *p_forbid_type == physical_type))
		{
		    //g_Record.log_out(ret, level_error, "p_forbid_ant");
			ret = ERR_CARD_NONE;
			break;
		}

		memset(PTR_TICKET, 0, SIZE_TICKET);

		switch (physical_type)
		{
		case mediNone:
		    //g_Record.log_out(ret, level_error, "mediNone");
			ret = ERR_CARD_NONE;
			break;
		case mediUltraLight:
			*pp_ticket = new(PTR_TICKET) TicketUl(Api::sock_sam_id(SAM_SOCK_1), Api::sock_terminal_id(SAM_SOCK_1), recv);
			mode_app = '2';
			ret = 0;
			break;
		case mediMetroCpu:
			*pp_ticket = new(PTR_TICKET) TicketSvt(Api::sock_sam_id(SAM_SOCK_1), Api::sock_terminal_id(SAM_SOCK_1), recv);
			mode_app = '1';
			ret = 0;
			break;

		case mediBusCpu:
			*pp_ticket = new(PTR_TICKET) TicketBus(Api::sock_sam_id(SAM_SOCK_2), Api::sock_terminal_id(SAM_SOCK_2), recv);
			mode_app = '1';
			ret = 0;
			break;

		case mediBusMifare:
			*pp_ticket = new(PTR_TICKET) TicketBM(Api::sock_sam_id(SAM_SOCK_3), Api::sock_terminal_id(SAM_SOCK_3), recv);
			mode_app = '2';
			ret = 0;
			break;

		case mediBusMfEX:
			*pp_ticket = new(PTR_TICKET) TicketBM(Api::sock_sam_id(SAM_SOCK_3), Api::sock_terminal_id(SAM_SOCK_3), recv);
			mode_app = '3';
			ret = 0;
			break;

		default:
		    //g_Record.log_out(ret, level_error, "default");
			ret = ERR_CARD_NONE;
			break;
		}
	}
	while (0);

	if (ret == 0)
	{
		if (p_current_id != NULL)
			memcpy(p_current_id, recv, 7);
		(*pp_ticket)->m_ticket_data.physical_type = physical_type;
		(*pp_ticket)->m_ticket_data.ticket_app_mode = mode_app;
	}
	return ret;
}

void TicketBase::transfer_data_for_out(ETYTKOPER operType, void * pTradeStruct)
{
    uint8_t * pTimeNow	= NULL;
    uint8_t * pTime1	= NULL;		// 用于将非法的时间转换为合法的时间
    uint8_t * pTime2	= NULL;

	char szTemp[32]		= {0};

	switch (operType)
	{
	case operAnalyse:
		{
			PBOMANALYZE pAnalyze = (PBOMANALYZE)pTradeStruct;

			memcpy(pAnalyze->cPhysicalID, m_ticket_data.physical_id, 20);
			memcpy(pAnalyze->cLogicalID, m_ticket_data.logical_id, 20);
			memcpy(pAnalyze->bTicketType, m_ticket_data.logical_type, 2);
			pAnalyze->lBalance			= m_ticket_data.wallet_value;
			pAnalyze->lDepositorCost	= m_ticket_data.deposit;
			pAnalyze->lLimitedBalance	= m_ticket_data.wallet_value_max;
			memcpy(pAnalyze->bIssueData, m_ticket_data.date_issue, 4);
			memcpy(pAnalyze->bExpiry, m_ticket_data.phy_peroidE, 4);
			memcpy(pAnalyze->bStartDate, m_ticket_data.logical_peroidS, 7);
			memcpy(pAnalyze->bEndDate, m_ticket_data.logical_peroidE, 7);

			pAnalyze->bLastStationID[0] = (uint8_t)((m_ticket_data.read_last_info.station_id >> 8) & 0xFF);
			pAnalyze->bLastStationID[1] = (uint8_t)(m_ticket_data.read_last_info.station_id & 0xFF);
			pAnalyze->bLastDeviceID[0] = (uint8_t)((m_ticket_data.read_last_info.dvc_id >> 8) & 0xFF);
			pAnalyze->bLastDeviceID[1] = (uint8_t)(m_ticket_data.read_last_info.dvc_id & 0xFF);
			pAnalyze->bLastDeviceType	= m_ticket_data.read_last_info.dvc_type;

			memcpy(pAnalyze->dtLastDate, m_ticket_data.read_last_info.time, 7);

			pAnalyze->bEntrySationID[0] = (uint8_t)((m_ticket_data.p_entry_info->station_id >> 8) & 0xFF);
			pAnalyze->bEntrySationID[1] = (uint8_t)(m_ticket_data.p_entry_info->station_id & 0xFF);
			memcpy(pAnalyze->dtEntryDate, m_ticket_data.p_entry_info->time, 7);

			memcpy(pAnalyze->cCityCode, m_ticket_data.city_code, sizeof(pAnalyze->cCityCode));
			memcpy(pAnalyze->cSellerCode, m_ticket_data.issue_code, sizeof(pAnalyze->cSellerCode));
			memcpy(pAnalyze->cCertificateCode, m_ticket_data.certificate_code, sizeof(pAnalyze->cCertificateCode));
			memcpy(pAnalyze->cCertificateName, m_ticket_data.certificate_name, sizeof(pAnalyze->cCertificateName));
			pAnalyze->bCertificateType = m_ticket_data.certificate_type;

			pAnalyze->lChargeCount = m_ticket_data.charge_counter;
			pAnalyze->lTradeCount = m_ticket_data.total_trade_counter;
			pAnalyze->cTestFlag = m_ticket_data.key_flag + 0x30;
			pAnalyze->bActiveStatus = 0;

			pAnalyze->bLimitMode = m_ticket_data.limit_mode;
			memcpy(pAnalyze->bLimitEntryID, m_ticket_data.limit_entry_station, 2);
			memcpy(pAnalyze->bLimitExitID, m_ticket_data.limit_exit_station, 2);

			pAnalyze->cTkAppMode = m_ticket_data.ticket_app_mode;
			memcpy(pAnalyze->bCertExpire, m_ticket_data.cert_peroidE, 4);
		}

		break;
	case operEntry:
		{
			PENTRYGATE pEntry = (PENTRYGATE)pTradeStruct;

			memcpy(pEntry->cTradeType, "53", 2);

			Api::get_current_device_info(pEntry->cStationID, pEntry->cDevType, pEntry->cDeviceID);

			memcpy(pEntry->cSAMID, m_p_current_sam, 16);
			memcpy(pEntry->bTicketType, m_ticket_data.logical_type, 2);
			memcpy(pEntry->cLogicalID, m_ticket_data.logical_id, 20);
			memcpy(pEntry->cPhysicalID, m_ticket_data.physical_id, 20);
			memcpy(pEntry->dtDate, CmdSort::m_time_now, 7);
			pEntry->lBalance = m_ticket_data.wallet_value;
			pEntry->cTestFlag = m_ticket_data.key_flag + 0x30;

			sprintf(szTemp, "%03d", m_ticket_data.limit_mode);
			memcpy(pEntry->cLimitMode, szTemp, sizeof(pEntry->cLimitMode));
			memcpy(pEntry->bLimitEntryID, m_ticket_data.limit_entry_station, 2);
			memcpy(pEntry->bLimitExitID, m_ticket_data.limit_exit_station, 2);

			if (pEntry->cEntryMode == 0)
				pEntry->cEntryMode = '0';

			pEntry->lTradeCount = m_ticket_data.total_trade_counter;
			pEntry->cTkAppMode = m_ticket_data.ticket_app_mode;
		}

		break;
	case operExit:
	case operDecr:
	case operIncr:
		{
			PPURSETRADE pPurse = (PPURSETRADE)pTradeStruct;
			memcpy(pPurse->cTradeType, "54", 2);
			Api::get_current_device_info(pPurse->cStationID, pPurse->cDevType, pPurse->cDeviceID);
			memcpy(pPurse->cSAMID, m_p_current_sam, 16);
			memcpy(pPurse->dtDate, CmdSort::m_time_now, 7);
			memcpy(pPurse->bTicketType, m_ticket_data.logical_type, 2);
			memcpy(pPurse->cLogicalID, m_ticket_data.logical_id, 20);
			memcpy(pPurse->cPhysicalID, m_ticket_data.physical_id, 20);
			pPurse->lBalance = m_ticket_data.wallet_value;
			pPurse->lChargeCount = m_ticket_data.charge_counter;
			pPurse->lTradeCount = m_ticket_data.total_trade_counter;
			memset(pPurse->cReceiptID, '0', sizeof(pPurse->cReceiptID));
			pPurse->bEntryStationID[0] = (uint8_t)((m_ticket_data.p_entry_info->station_id >> 8) & 0xFF);
			pPurse->bEntryStationID[1] = (uint8_t)(m_ticket_data.p_entry_info->station_id & 0xFF);
			memcpy(pPurse->cEntrySAMID, m_ticket_data.p_entry_info->sam, 16);
			memcpy(pPurse->dtEntryDate, m_ticket_data.p_entry_info->time, 7);
			memcpy(pPurse->cOperatorID, Api::operator_id, 6);
			pPurse->bBOMShiftID	= Api::bom_shift_id;
			memcpy(pPurse->cSamLast, m_ticket_data.read_last_info.sam, 16);
			memcpy(pPurse->dtLast, m_ticket_data.read_last_info.time, 7);
			//pPurse->lTradeWallet
			pPurse->cTestFlag = m_ticket_data.key_flag + 0x30;

			sprintf(szTemp, "%03d", m_ticket_data.limit_mode);
			memcpy(pPurse->cLimitMode, szTemp, sizeof(pPurse->cLimitMode));
			memcpy(pPurse->bLimitEntryID, m_ticket_data.limit_entry_station, 2);
			memcpy(pPurse->bLimitExitID, m_ticket_data.limit_exit_station, 2);

			if (pPurse->cExitMode == 0)
				pPurse->cExitMode = '0';

			memcpy(pPurse->cCityCode, m_ticket_data.city_code, sizeof(pPurse->cCityCode));					// 城市代码
			memcpy(pPurse->cIndustryCode, m_ticket_data.industry_code, sizeof(pPurse->cIndustryCode));		// 行业代码
			memcpy(pPurse->cSamPosId, m_p_sam_posid, sizeof(pPurse->cSamPosId));							// SAM卡终端编码

			pPurse->cTkAppMode = m_ticket_data.ticket_app_mode;

			pTimeNow = pPurse->dtDate;
			pTime1 = pPurse->dtLast;
			pTime2 = pPurse->dtEntryDate;
		}
		break;
	case operUpdate:
		{
			PTICKETUPDATE pUpdate = (PTICKETUPDATE)pTradeStruct;

			memcpy(pUpdate->cTradeType, "56", 2);
			Api::get_current_device_info(pUpdate->cStationID, pUpdate->cDevType, pUpdate->cDeviceID);

			memcpy(pUpdate->cSAMID, m_p_current_sam, 16);
			memcpy(pUpdate->bTicketType, m_ticket_data.logical_type, 2);
			memcpy(pUpdate->cLogicalID, m_ticket_data.logical_id, 20);
			memcpy(pUpdate->cPhysicalID, m_ticket_data.physical_id, 20);
			pUpdate->lTradeCount = m_ticket_data.total_trade_counter;

			memcpy(pUpdate->dtUpdateDate, CmdSort::m_time_now, 7);
			memset(pUpdate->cReceiptID, '0', sizeof(pUpdate->cReceiptID));
			memcpy(pUpdate->cOperatorID, Api::operator_id, 6);
			pUpdate->bEntryStationID[0] = (uint8_t)((m_ticket_data.p_entry_info->station_id >> 8) & 0xFF);
			pUpdate->bEntryStationID[1] = (uint8_t)(m_ticket_data.p_entry_info->station_id & 0xFF);

			pUpdate->bBOMShfitID = Api::bom_shift_id;
			pUpdate->cTestFlag = m_ticket_data.key_flag + 0x30;

			sprintf(szTemp, "%03d", m_ticket_data.limit_mode);
			memcpy(pUpdate->cLimitMode, szTemp, sizeof(pUpdate->cLimitMode));
			memcpy(pUpdate->bLimitEntryID, m_ticket_data.limit_entry_station, 2);
			memcpy(pUpdate->bLimitExitID, m_ticket_data.limit_exit_station, 2);

			pUpdate->cTkAppMode = m_ticket_data.ticket_app_mode;
		}
		break;
	case operSjtSale:
		{
			PSJTSALE pSale = (PSJTSALE)pTradeStruct;

			memcpy(pSale->cTradeType, "50", 2);
			Api::get_current_device_info(pSale->cStationID, pSale->cDevType, pSale->cDeviceID);

			memcpy(pSale->cSAMID, m_p_current_sam, 16);
			memcpy(pSale->dtDate, CmdSort::m_time_now, 7);
			memcpy(pSale->cLogicalID, m_ticket_data.logical_id, 20);
			memcpy(pSale->cPhysicalID, m_ticket_data.physical_id, 20);
			memset(pSale->cPaymentTKLogicalID, '0', sizeof(pSale->cPaymentTKLogicalID));
			pSale->lTradeCount = m_ticket_data.total_trade_counter;
			memcpy(pSale->bTicketType, m_ticket_data.logical_type, 2);
			pSale->bDepositorCost = 0x02;
			pSale->nAmountCost = 0;
			memcpy(pSale->cOperatorID, Api::operator_id, 6);
			pSale->bBOMShiftID = Api::bom_shift_id;
			//pSale->lBrokerage;
			pSale->cTestFlag = m_ticket_data.key_flag + 0x30;
			memcpy(pSale->cSamPosId, m_p_sam_posid, sizeof(pSale->cSamPosId));								// SAM卡终端编码
		}
		break;
	case operSvtSale:
		{
			POTHERSALE pSvtSale = (POTHERSALE)pTradeStruct;

			memcpy(pSvtSale->cTradeType, "51", 2);
			Api::get_current_device_info(pSvtSale->cStationID, pSvtSale->cDevType, pSvtSale->cDeviceID);
			memcpy(pSvtSale->bTicketType, m_ticket_data.logical_type, 2);
			memcpy(pSvtSale->cLogicalID, m_ticket_data.logical_id, 20);
			memcpy(pSvtSale->cPhysicalID, m_ticket_data.physical_id, 20);
			memset(pSvtSale->cBusinessseqno, '0', sizeof(pSvtSale->cBusinessseqno));
			memcpy(pSvtSale->cSAMID, m_p_current_sam, 16);
			pSvtSale->nAmount = (short)m_ticket_data.deposit;
			memcpy(pSvtSale->dtDate, CmdSort::m_time_now, 7);
			memset(pSvtSale->cReceiptID, '0', sizeof(pSvtSale->cReceiptID));
			memcpy(pSvtSale->cOperatorID, Api::operator_id, 6);
			pSvtSale->bBOMShfitID = Api::bom_shift_id;
			//pSvtSale->lBrokerage
			pSvtSale->cTestFlag = m_ticket_data.key_flag + 0x30;

		}
		break;
	case operDeffer:
		{
			PTICKETDEFER pDeffer = (PTICKETDEFER)pTradeStruct;

			memcpy(pDeffer->cTradeType, "55", 2);
			Api::get_current_device_info(pDeffer->cStationID, pDeffer->cDevType, pDeffer->cDeviceID);

			memcpy(pDeffer->cSAMID, m_p_current_sam, 16);
			memcpy(pDeffer->bTicketType, m_ticket_data.logical_type, 2);
			memcpy(pDeffer->cLogicalID, m_ticket_data.logical_id, 20);
			memcpy(pDeffer->cPhysicalID, m_ticket_data.physical_id, 20);
			memcpy(pDeffer->dtOperateDate, CmdSort::m_time_now, 7);
			memcpy(pDeffer->cOperatorID, Api::operator_id, 6);
			pDeffer->bBOMShfitID = Api::bom_shift_id;
			pDeffer->cTestFlag = m_ticket_data.key_flag + 0x30;
		}
		break;
	case operRefund:
		{
			PDIRECTREFUND pRefund = (PDIRECTREFUND)pTradeStruct;

			memcpy(pRefund->cTradeType, "57", 2);
			Api::get_current_device_info(pRefund->cStationID, pRefund->cDevType, pRefund->cDeviceID);

			memcpy(pRefund->cSAMID, m_p_current_sam, 16);
			memcpy(pRefund->bTicketType, m_ticket_data.logical_type, 2);
			memcpy(pRefund->cLogicalID, m_ticket_data.logical_id, 20);
			memcpy(pRefund->cPhysicalID, m_ticket_data.physical_id, 20);
			pRefund->lTradeCount = m_ticket_data.total_trade_counter;
			memcpy(pRefund->dtDate, CmdSort::m_time_now, 7);
			memset(pRefund->cReceiptID, '0', sizeof(pRefund->cReceiptID));
			memcpy(pRefund->dtApplyDate, pRefund->dtDate, 7);
			memcpy(pRefund->cOperatorID, Api::operator_id, 6);
			pRefund->bBOMShfitID = Api::bom_shift_id;
			pRefund->cTestFlag = m_ticket_data.key_flag + 0x30;
			memcpy(pRefund->cSamPosId, m_p_sam_posid, sizeof(pRefund->cSamPosId));								// SAM卡终端编码
		}

		break;
	case operLock:
		{
			PTICKETLOCK pLock = (PTICKETLOCK)pTradeStruct;
			memcpy(pLock->cTradeType, "59", 2);
			Api::get_current_device_info(pLock->cStationID, pLock->cDevType, pLock->cDeviceID);
			memcpy(pLock->cSAMID, m_p_current_sam, 16);
			memcpy(pLock->bTicketType, m_ticket_data.logical_type, 2);
			memcpy(pLock->cLogicalID, m_ticket_data.logical_id, 20);
			memcpy(pLock->cPhysicalID, m_ticket_data.physical_id, 20);
			memcpy(pLock->dtDate, CmdSort::m_time_now, 7);
			memcpy(pLock->cOperatorID, Api::operator_id, 6);
			pLock->bBOMShfitID = Api::bom_shift_id;
			pLock->cTestFlag = m_ticket_data.key_flag + 0x30;

			pLock->cTkAppMode = m_ticket_data.ticket_app_mode;
		}
		break;

	case operTkInf:
		{
			PTICKETINFO pInf = (PTICKETINFO)pTradeStruct;

			memcpy(pInf->cPhysicalID, m_ticket_data.physical_id, 20);
			memcpy(pInf->cLogicalID, m_ticket_data.logical_id, 20);
			memcpy(pInf->bTicketType, m_ticket_data.logical_type, 2);
			pInf->lBalance			= m_ticket_data.wallet_value;
			pInf->lDepositorCost	= m_ticket_data.deposit;
			pInf->lLimitedBalance	= m_ticket_data.wallet_value_max;
			memcpy(pInf->bIssueData, m_ticket_data.date_issue, 4);
			memcpy(pInf->bExpiry, m_ticket_data.phy_peroidE, 4);
			memcpy(pInf->bStartDate, m_ticket_data.logical_peroidS, 7);
			memcpy(pInf->bEndDate, m_ticket_data.logical_peroidE, 7);

			pInf->bLastStationID[0] = (uint8_t)((m_ticket_data.read_last_info.station_id >> 8) & 0xFF);
			pInf->bLastStationID[1] = (uint8_t)(m_ticket_data.read_last_info.station_id & 0xFF);
			pInf->bLastDeviceID[0] = (uint8_t)((m_ticket_data.read_last_info.dvc_id >> 8) & 0xFF);
			pInf->bLastDeviceID[1] = (uint8_t)(m_ticket_data.read_last_info.dvc_id & 0xFF);
			pInf->bLastDeviceType	= m_ticket_data.read_last_info.dvc_type;

			memcpy(pInf->dtLastDate, m_ticket_data.read_last_info.time, 7);

			pInf->bEntrySationID[0] = (uint8_t)((m_ticket_data.p_entry_info->station_id >> 8) & 0xFF);
			pInf->bEntrySationID[1] = (uint8_t)(m_ticket_data.p_entry_info->station_id & 0xFF);
			memcpy(pInf->dtEntryDate, m_ticket_data.p_entry_info->time, 7);

			memcpy(pInf->cCityCode, m_ticket_data.city_code, sizeof(pInf->cCityCode));
			memcpy(pInf->cSellerCode, m_ticket_data.issue_code, sizeof(pInf->cSellerCode));
			memcpy(pInf->cCertificateCode, m_ticket_data.certificate_code, sizeof(pInf->cCertificateCode));
			memcpy(pInf->cCertificateName, m_ticket_data.certificate_name, sizeof(pInf->cCertificateName));
			pInf->bCertificateType = m_ticket_data.certificate_type;
			pInf->lChargeCount = m_ticket_data.charge_counter;
			pInf->lTradeCount = m_ticket_data.total_trade_counter;
			pInf->cTestFlag = m_ticket_data.key_flag + 0x30;
			pInf->bActiveStatus = 0;

			pInf->bLimitMode = m_ticket_data.limit_mode;
			memcpy(pInf->bLimitEntryID, m_ticket_data.limit_entry_station, 2);
			memcpy(pInf->bLimitExitID, m_ticket_data.limit_exit_station, 2);

			pInf->cTkAppMode = m_ticket_data.ticket_app_mode;
			memcpy(pInf->bCertExpire, m_ticket_data.cert_peroidE, 4);
		}
		break;
    default:
        break;
	}

    invalid_time_change_current(pTime1);
    invalid_time_change_current(pTime2);
}

void TicketBase::invalid_time_change_current(uint8_t * p_time_invalid)
{
    if (p_time_invalid != NULL && !TimesEx::bcd_time_valid(p_time_invalid, T_TIME))
        memcpy(p_time_invalid, CmdSort::m_time_now, 7);
}

void TicketBase::get_ticket_infos(TICKET_DATA& ticket_data)
{
    memcpy(&ticket_data, &m_ticket_data, sizeof(TICKET_DATA));
}

uint8_t TicketBase::unified_status()
{
    return 0;
}

RETINFO TicketBase::sale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, POTHERSALE p_svt_sale, uint16_t& len_svt, TICKET_PARAM& ticket_prm)
{
	RETINFO ret			= {0};
	ret.wErrCode = ERR_CALL_INVALID;
	return ret;
}

RETINFO TicketBase::bom_refund(PDIRECTREFUND p_refund)
{
	RETINFO ret			= {0};
	ret.wErrCode = ERR_CALL_INVALID;
	return ret;
}

RETINFO TicketBase::exit_sjt_sale(PSJTSALE p_exit_sjt_sale)
{
	RETINFO ret			= {0};
	ret.wErrCode = ERR_CALL_INVALID;
	return ret;
}
RETINFO TicketBase::sjt_decrease()
{
	RETINFO ret			= {0};
	ret.wErrCode = ERR_CALL_INVALID;
	return ret;
}

RETINFO TicketBase::svt_increase(PPURSETRADE p_purse, uint8_t * p_time, uint8_t * p_mac2)
{
	RETINFO ret			= {0};
	ret.wErrCode = ERR_CALL_INVALID;
	return ret;
}

RETINFO TicketBase::svt_decrease(PPURSETRADE p_purse, uint8_t u_pay_flag)
{
	RETINFO ret			= {0};
	ret.wErrCode = ERR_CALL_INVALID;
	return ret;
}

RETINFO TicketBase::format_history(uint8_t&	his_count, PHSSVT p_his_array, int his_max)
{
	RETINFO ret			= {0};
	his_count = 0;
	return ret;
}

RETINFO TicketBase::lock(PTICKETLOCK p_lock)
{
	RETINFO ret			= {0};
	ret.wErrCode = ERR_CALL_INVALID;
	return ret;
}

RETINFO TicketBase::deffer(PTICKETDEFER p_deffer, TICKET_PARAM& ticket_prm)
{
	RETINFO ret			= {0};
	ret.wErrCode = ERR_CALL_INVALID;
	return ret;
}

#define _MaxBitPos		7
// 函数配合宏_pair使用，以免参数漏掉参数造成异常
//uint8_t TicketBase::cmd_sprintf(int start_byte_pos, uint8_t * p_dst_buf, ...)
//{
//    // 指向可变参数列表的指针
//    va_list arg_ptr;
//
//    // 当前可变参数的值
//    int value_arg = 0;
//
//    // 当前可变参数的位数
//    char bits_arg = 0;
//
//    // 需要取当前参数的位数，也即需要填充到数组当前元素的位数
//    char bits_to_get = 0;
//
//    // 指示数组当前元素的位置
//    uint8_t arg_pos_in_buf = 0;
//
//    // 占位符标记
//    bool placeholder_falg;
//
//    // 屏蔽码
//    uint8_t sheild_code;
//
//    va_start(arg_ptr, p_dst_buf);
//
//    while(true)
//    {
//        placeholder_falg = false;
//        //bits_arg = va_arg( arg_ptr, char);
//        bits_arg = va_arg( arg_ptr, int);
//        value_arg = va_arg( arg_ptr, int);
//
//        // 处理占位符
//        if (bits_arg == 0)
//        {
//            if (value_arg == 0)
//                break;
//            bits_arg = (char)value_arg;
//            placeholder_falg = true;
//        }
//
//        do
//        {
//
//            // 计算填入当前BYTE空缺的BIT数
//            if (bits_arg > start_byte_pos + 1)
//                bits_to_get = start_byte_pos + 1;
//            else
//                bits_to_get = bits_arg;
//
//            if (!placeholder_falg)
//            {
//                sheild_code = Publics::bits_get_align_right((uint8_t)0xFF, start_byte_pos, start_byte_pos + 1 - bits_to_get);
//                sheild_code = (uint8_t)~(sheild_code << (start_byte_pos + 1 - bits_to_get));
//
//                // 保留缓冲中原有的数据，并屏蔽当前需要填入数据的地方
//                p_dst_buf[arg_pos_in_buf] &= sheild_code;
//
//                // 取元素的高bits_to_get位填入当前缓冲的空缺中
//                p_dst_buf[arg_pos_in_buf] |=
//                    (uint8_t)Publics::bits_get_align_right(value_arg, bits_arg, bits_arg - bits_to_get) << (start_byte_pos + 1 - bits_to_get);
//            }
//            bits_arg -= bits_to_get;
//            start_byte_pos -= bits_to_get;
//            if (start_byte_pos < 0)
//            {
//                arg_pos_in_buf ++;
//                start_byte_pos = 7;
//            }
//
//        }
//        while (bits_arg > 0);
//
//    }
//
//    va_end(arg_ptr);
//
//    return arg_pos_in_buf;
//}

// 函数配合宏_thr使用，以免参数漏掉参数造成异常
//void TicketBase::cmd_scanf(int start_byte_pos, uint8_t * p_src_buf, ...)
//{
//    // 指向可变参数列表的指针
//    va_list arg_ptr;
//
//    // 当前可变参数的指针
//    char * p_value_arg;
//
//    // 可变参数的临时值（只能给临时值再赋值，否运算则会造成内存泄露）
//    int value_arg;
//
//    // 当前可变参数的总bits数、实际定义的类型占用的BYTES数
//    int bits_arg, nSizeInByte;
//
//    // 应该取当前字节的起始(参数start_byte_pos)、截至位
//    int nEnd;
//
//    // 使指针指向可变参数的第一个参数
//    va_start(arg_ptr, p_src_buf);
//
//    while ((bits_arg = va_arg(arg_ptr, int)) > 0)
//    {
//        p_value_arg = va_arg(arg_ptr, char*);
//        nSizeInByte = va_arg(arg_ptr, int);
//
//        value_arg = 0;
//
//        while (bits_arg > 0)
//        {
//            // 计算应取当前字节的截止位
//            if (start_byte_pos + 1 >= bits_arg)
//                nEnd = start_byte_pos + 1 - bits_arg;
//            else
//                nEnd = 0;
//
//            value_arg = (value_arg << (start_byte_pos - nEnd + 1)) + Publics::bits_get_align_right(p_src_buf[0], start_byte_pos, nEnd);
//
//            bits_arg -= start_byte_pos - nEnd + 1;
//            start_byte_pos = nEnd - 1;
//            if (start_byte_pos < 0)
//            {
//                start_byte_pos = _MaxBitPos;
//                p_src_buf ++;
//            }
//        }
//
//        if (p_value_arg != NULL)
//        {
//            memset(p_value_arg, 0, nSizeInByte);
//            // 高字节在前，低字节在后才可以如此拷贝
//            memcpy(p_value_arg, &value_arg, nSizeInByte);
//        }
//    }
//
//    va_end(arg_ptr);	// 清空参数列表，并置指针为无效
//}

bool TicketBase::station_is_current(uint16_t station_id)
{
    return g_Parameter.same_station(Api::current_station_id, station_id);
}

RETINFO TicketBase::analyse_work_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PPURSETRADE p_purse)
{
    RETINFO ret					= {0};
	uint16_t result_prm			= 0;
	uint16_t err				= 0;
	uint16_t err1				= 0;
    long trade_val				= 0;
	long penalty_val			= 0;
	long penalty_val1			= 0;
    uint32_t operation_status	= 0;
	char exit_mode				= 0;
				
	char temp = 0;

    do
    {
		// 如果发生列车故障，直接返回正常
		if (p_degrade_falgs->train_trouble)
		{
			exit_mode = (char)(degrade_Trouble + 0x30);
			break;
		}

		// 逻辑有效期
		// 当票价类型和实际类型不符时，说明已经转入普通卡，不再检查逻辑有效期
		if (p_ticket_prm->check_logic_expire && need_check_period(1, m_ticket_data.simple_status) &&
			memcmp(m_ticket_data.logical_type, m_ticket_data.fare_type, 2) == 0)
		{
			if (memcmp(CmdSort::m_time_now, m_ticket_data.logical_peroidS, 7) < 0)
			{
				if (p_degrade_falgs->uncheck_datetime)
				{
					exit_mode = (char)(degrade_Uncheck_DateTime + 0x30);
				}
				else
				{
					ret.wErrCode = ERR_OVER_PERIOD_L;
					break;
				}
			}
			if (memcmp(CmdSort::m_time_now, m_ticket_data.logical_peroidE, 7) > 0)
			{
				if (p_degrade_falgs->uncheck_datetime)
				{
					exit_mode = (char)(degrade_Uncheck_DateTime + 0x30);
				}
				else
				{
					if (p_ticket_prm->permit_deffer)
						operation_status |= ALLOW_DIFFER;

					ret.wErrCode = ERR_OVER_PERIOD_L;
					break;
				}
			}
		}

        // 无进站码,10分钟内已出站
        if (m_ticket_data.simple_status == SStatus_Exit || m_ticket_data.simple_status == SStatus_Exit_Tt)
        {
			if (p_degrade_falgs->uncheck_order)			// 进出次序免检模式，找出离本站最近的车站
			{
				memset(m_ticket_data.p_entry_info, 0, sizeof(TradeInfo));
				memcpy(m_ticket_data.p_entry_info->time, CmdSort::m_time_now, 7);
				memset(m_ticket_data.p_entry_info->sam, '0', 16);
				m_ticket_data.p_entry_info->station_id = g_Parameter.nearest_uncheckorder_station();

				exit_mode = (char)(degrade_Uncheck_Order + 0x30);
			}
            else if (p_ticket_prm->check_exit_order)
            {
                ret.wErrCode = err = ERR_WITHOUT_ENTRY;

                if (!TimesEx::timeout(CmdSort::m_time_now, m_ticket_data.p_exit_info->time, 10) &&
                        station_is_current(m_ticket_data.p_exit_info->station_id))
                {
                    ret.wErrCode = err = ERR_LAST_EXIT_NEAR;
                }

                operation_status |= ALLOW_UPDATE;
                break;
            }
            else
            {
                current_trade_info(*m_ticket_data.p_entry_info);
            }
        }
        else if (m_ticket_data.simple_status == SStatus_Upd_FZone || m_ticket_data.simple_status == SStatus_Sale ||
                 m_ticket_data.simple_status == SStatus_Es || m_ticket_data.simple_status == SStatus_Init)
        {
			if (p_degrade_falgs->uncheck_order)			// 进出次序免检模式，找出离本站最近的车站
			{
				memset(m_ticket_data.p_entry_info, 0, sizeof(TradeInfo));
				memcpy(m_ticket_data.p_entry_info->time, CmdSort::m_time_now, 7);
				memset(m_ticket_data.p_entry_info->sam, '0', 16);
				m_ticket_data.p_entry_info->station_id = g_Parameter.nearest_uncheckorder_station();

				exit_mode = (char)(degrade_Uncheck_Order + 0x30);
			}
            else if (p_ticket_prm->check_exit_order)
            {
                ret.wErrCode = err = ERR_WITHOUT_ENTRY;
                operation_status |= ALLOW_UPDATE;
                break;
            }
            else
            {
                current_trade_info(*m_ticket_data.p_entry_info);
            }
        }

        if (m_ticket_data.simple_status == SStatus_Upd_WZone)
        {
            // 非本站更新
            if (p_ticket_prm->check_update_station && !station_is_current(m_ticket_data.p_update_info->station_id))
            {
                ret.wErrCode = ERR_UPDATE_STATION;
                break;
            }
            // 非本日更新
            if (p_ticket_prm->check_update_day && TimesEx::TimeWorkDayCmp(CmdSort::m_time_now, m_ticket_data.p_update_info->time) != 0)
            {
                ret.wErrCode = ERR_UPDATE_DAY;
                break;
            }
        }

        if (m_ticket_data.simple_status == SStatus_Entry)
        {
			// 本站进出控制
			if (p_ticket_prm->limit_the_Station)
			{
				if (!g_Parameter.same_station(m_ticket_data.p_entry_info->station_id, Api::current_station_id))
				{
					ret.wErrCode = ERR_CARD_USE;
					break;
				}
			}

            // 超时(先检查超时，再检查超程，便于既超时，又超程的判断)
            if (p_ticket_prm->check_exit_timeout && !p_degrade_falgs->uncheck_datetime)
            {
                if (g_Parameter.exit_timeout(m_ticket_data.p_exit_info->station_id,
                                             m_ticket_data.p_entry_info->time, CmdSort::m_time_now, &penalty_val))
                {
					g_Record.log_out(0, level_disaster, "1 analyse_work_zone:query_fare");
                    result_prm = g_Parameter.query_fare(CmdSort::m_time_now, m_ticket_data.fare_type,
                                   m_ticket_data.p_entry_info->station_id, m_ticket_data.p_entry_info->time, &trade_val);
                    if (result_prm)
					{
						ret.wErrCode = result_prm;
						break;
					}

                    if (trade_val == 0)
                        penalty_val = trade_val;

                    ret.wErrCode = err = ERR_EXIT_TIMEOUT;
                    operation_status |= ALLOW_UPDATE;
                    // break;		// 需要检查超时又超乘，所以不能break
                }
            }
        }

        // 查票价
		if (m_ticket_data.simple_status == SStatus_Exit_Only)
		{
			trade_val = m_ticket_data.wallet_value;
		}
        else if (!p_ticket_prm->check_balance || p_degrade_falgs->uncheck_fare)
        {
			g_Record.log_out(0, level_disaster, "1 analyse_work_zone:query_lowest_fare");
			temp = (char)((m_ticket_data.p_entry_info->station_id >> 8) & 0xFF);
			if(temp == 0x60)
				result_prm = g_Parameter.query_cf_lowest_fare(CmdSort::m_time_now, m_ticket_data.fare_type,m_ticket_data.p_entry_info->station_id,trade_val);
            else
				result_prm = g_Parameter.query_lowest_fare(CmdSort::m_time_now, m_ticket_data.fare_type,trade_val);

			if (p_degrade_falgs->uncheck_fare)
			{
				exit_mode = (char)(degrade_Uncheck_Fare + 0x30);
			}
        }
        else
        {
			g_Record.log_out(0, level_disaster, "2 analyse_work_zone:query_fare");
            result_prm = g_Parameter.query_fare(CmdSort::m_time_now, m_ticket_data.fare_type,
				m_ticket_data.p_entry_info->station_id, m_ticket_data.p_entry_info->time, &trade_val);
        }
		if (result_prm)
		{
			ret.wErrCode = result_prm;
			break;
		}

        // 检查超乘
        if (m_ticket_data.simple_status == SStatus_Upd_WZone && m_ticket_data.init_status != MS_Upd_Wzone_Time)
		{
			// 余额足够收取票价，否则将余额减为0
			if (m_ticket_data.wallet_value < trade_val/* - ticket_prm.value_over_draft*/)
			{
				trade_val = m_ticket_data.wallet_value/10*10;//超乘，直接舍掉分
			}
		}
		else
        {
			if (!p_ticket_prm->check_over_travel)
			{
				if (m_ticket_data.wallet_value < trade_val)
				{
					trade_val = m_ticket_data.wallet_value/10*10;
				}
			}
            else if (p_ticket_prm->permit_over_draft)
            {
                if (m_ticket_data.wallet_value < trade_val/* - ticket_prm.value_over_draft*/)
                {
                    //penalty_val1 = ((trade_val - m_ticket_data.wallet_value)/10)*10;/* - ticket_prm.value_over_draft*/;
					penalty_val1 = (trade_val - ((m_ticket_data.wallet_value/10)*10));/* - ticket_prm.value_over_draft*/;

                    ret.wErrCode = err1 = ERR_OVER_MILEAGE;
                    operation_status |= ALLOW_UPDATE;

                    break;
                }
            }
            else if (m_ticket_data.wallet_value < trade_val)
            {
                //penalty_val1 = ((trade_val - m_ticket_data.wallet_value)/10)*10;
				penalty_val1 = (trade_val - ((m_ticket_data.wallet_value/10)*10));
				//penalty_val1 = (trade_val - m_ticket_data.wallet_value);

                ret.wErrCode = err1 = ERR_OVER_MILEAGE;
                operation_status |= ALLOW_UPDATE;

                break;
            }
        }

    } while(0);

	if (p_analyse != NULL)
	{
		p_analyse->dwOperationStauts |= operation_status;
		p_analyse->lPenalty = penalty_val;
		p_analyse->wError = err;
		p_analyse->lPenalty1 = penalty_val1;
		p_analyse->wError1 = err1;
	}
	if (p_purse != NULL)
	{
		p_purse->lTradeAmount = trade_val;
		p_purse->cExitMode = exit_mode;
	}

	if (err == ERR_EXIT_TIMEOUT && err1 == ERR_OVER_MILEAGE)
		ret.wErrCode = ERR_OVER_TIME_MILEAGE;

    return ret;
}

RETINFO TicketBase::analyse_free_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse)
{
    RETINFO ret					= {0};
	uint16_t err				= 0;
	uint16_t err1				= 0;
	long penalty_val			= 0;
	long penalty_val1			= 0;
	long lowest_val				= 0;
    uint32_t operation_status	= 0;
	char entry_mode				= 0;
	char temp				= 0;

    do
    {
		// 当票价类型和实际类型不符时，说明已经转入普通卡，不再检查逻辑有效期
		if (p_ticket_prm->check_logic_expire && memcmp(m_ticket_data.logical_type, m_ticket_data.fare_type, 2) == 0)
		{
			// 逻辑有效期
			if (memcmp(CmdSort::m_time_now, m_ticket_data.logical_peroidS, 7) < 0)
			{
				if (p_degrade_falgs->uncheck_datetime)
				{
					entry_mode = (char)(0x30 + degrade_Uncheck_DateTime);
				}
				else if (p_degrade_falgs->sell_sjt_by_emergency || p_degrade_falgs->es_sjt_by_emergency || p_degrade_falgs->entry_by_emergency ||
					p_degrade_falgs->update_free_by_emergency || p_degrade_falgs->update_work_by_emergency)
				{
					entry_mode = (char)(0x30 + degrade_Green_Light);
				}
				else if (p_degrade_falgs->es_sjt_by_train_trouble || p_degrade_falgs->exit_train_trouble_effect || p_degrade_falgs->sell_sjt_by_train_trouble)
				{
					entry_mode = (char)(0x30 + degrade_Trouble);
				}
				else
				{
					ret.wErrCode = ERR_OVER_PERIOD_L;
					break;
				}
			}
			if (memcmp(CmdSort::m_time_now, m_ticket_data.logical_peroidE, 7) > 0)
			{
				if (p_degrade_falgs->uncheck_datetime)
				{
					entry_mode = (char)(0x30 + degrade_Uncheck_DateTime);
				}
				else if (p_degrade_falgs->sell_sjt_by_emergency || p_degrade_falgs->es_sjt_by_emergency || p_degrade_falgs->entry_by_emergency ||
					p_degrade_falgs->update_free_by_emergency || p_degrade_falgs->update_work_by_emergency)
				{
					entry_mode = (char)(0x30 + degrade_Green_Light);
				}
				else if (p_degrade_falgs->es_sjt_by_train_trouble || p_degrade_falgs->exit_train_trouble_effect || p_degrade_falgs->sell_sjt_by_train_trouble)
				{
					entry_mode = (char)(0x30 + degrade_Trouble);
				}
				else
				{
					if (p_ticket_prm->permit_deffer)
						operation_status |= ALLOW_DIFFER;

					ret.wErrCode = ERR_OVER_PERIOD_L;
					break;
				}
			}
		}
		g_Record.log_out(0, level_disaster, "1 analyse_free_zone:query_lowest_fare");
        //ret.wErrCode = g_Parameter.query_lowest_fare(CmdSort::m_time_now, m_ticket_data.fare_type,lowest_val);
        //if (ret.wErrCode)	break;


		temp = (char)((m_ticket_data.p_entry_info->station_id >> 8) & 0xFF);
		if(temp == 0x60)
			ret.wErrCode = g_Parameter.query_cf_lowest_fare(CmdSort::m_time_now, m_ticket_data.fare_type,m_ticket_data.p_entry_info->station_id,lowest_val);
        else
			ret.wErrCode = g_Parameter.query_lowest_fare(CmdSort::m_time_now, m_ticket_data.fare_type,lowest_val);
		if (ret.wErrCode)	break;


        if (m_ticket_data.simple_status == SStatus_Entry)
        {
            if (!station_is_current(m_ticket_data.p_entry_info->station_id))	// 非本站进站
            {
                ret.wErrCode = err = ERR_ENTRY_STATION;
				penalty_val = lowest_val;
            }
            else if (TimesEx::timeout(CmdSort::m_time_now, m_ticket_data.p_entry_info->time, 20))		// 进站超时
            {
                ret.wErrCode = err = ERR_ENTRY_TIMEOUT;
				penalty_val = lowest_val;
            }
            else	// 有进站码
            {
                ret.wErrCode = err = ERR_ENTRY_EVER;
            }

			if (p_degrade_falgs->entry_by_emergency)
			{
				ret.wErrCode = 0;
				entry_mode = (char)(0x30 + degrade_Green_Light);
			}
			else
			{
				operation_status |= ALLOW_UPDATE;
				break;
			}
        }

        if (m_ticket_data.simple_status == SStatus_Upd_WZone)
        {
			if (p_degrade_falgs->update_work_by_emergency)
			{
				ret.wErrCode = 0;
				entry_mode = (char)(0x30 + degrade_Green_Light);
			}
			else
			{
				ret.wErrCode = err = ERR_ENTRY_STATION;
				operation_status |= ALLOW_UPDATE;
				penalty_val = lowest_val;

				break;
			}
		}

		// 非本站使用的车票
		if (p_ticket_prm->entry_sell_station && m_ticket_data.physical_type == mediUltraLight)
		{
			if (m_ticket_data.simple_status == SStatus_Sale)
			{
				if (!g_Parameter.same_station(m_ticket_data.read_last_info.station_id, Api::current_station_id))
				{
					if (p_degrade_falgs->entry_by_emergency)
					{
						ret.wErrCode = 0;
						entry_mode = (char)(0x30 + degrade_Green_Light);
					}
					else
					{
						ret.wErrCode = ERR_CARD_USE;
						break;
					}
				}
			}
		}

        // 检查最低票价
        if (p_ticket_prm->check_balance && m_ticket_data.wallet_value < lowest_val)
        {
            ret.wErrCode = ERR_LACK_WALLET;
            break;
        }

    }
    while (0);

	if (p_analyse != NULL)
	{
		p_analyse->dwOperationStauts |= operation_status;
		p_analyse->lPenalty = penalty_val;
		p_analyse->wError = err;
		p_analyse->lPenalty1 = penalty_val1;
		p_analyse->wError1 = err1;
	}

    return ret;
}

uint32_t TicketBase::add_oper_status(uint8_t wk_area, RETINFO ret, TICKET_PARAM * p_ticket_prm)
{
	return 0;
}

void TicketBase::current_trade_info(TradeInfo& src_info)
{
    memcpy(src_info.time, CmdSort::m_time_now, 7);
    src_info.dvc_id = Api::current_device_id;
    src_info.dvc_type = (ETPDVC)(Api::current_device_type);
    memcpy(src_info.sam, m_p_current_sam, 16);
    src_info.station_id = Api::current_station_id;
}

bool TicketBase::last_trade_need_continue(uint8_t status_targ, uint16_t err_targ, uint16_t err_src)
{
	bool ret = false;

	return ret;
}

RETINFO TicketBase::continue_last_trade(void * p_trade)
{
	RETINFO ret	= {0};
	return ret;
}

void TicketBase::unlock_reader(void * p_trade)
{
	char tac[10];

	memset(tac, '0', sizeof(tac));
	set_confirm_factor(LIFE_CONFIRM_RM, 0, tac);
	memcpy(p_trade, &cfm_point.trade, cfm_point.size_trade);
}

RETINFO TicketBase::data_version_upgrade()
{
	RETINFO ret	= {0};
	return ret;
}

#define CRC_AFFIX		"CS-METRO-2013@GDMH"
bool TicketBase::metro_area_crc_valid(uint8_t * p_metro, uint16_t len_area)
{
	uint8_t data_src[256] = {0};
	size_t pos = strlen(CRC_AFFIX);

	memcpy(data_src, CRC_AFFIX, pos);
	memcpy(data_src + pos, p_metro, len_area);

	return DataSecurity::CheckCrc16(data_src, pos + len_area - 2, p_metro + len_area - 2);
}

void TicketBase::add_metro_area_crc(uint8_t * p_metro, uint16_t len_area)
{
	uint8_t data_src[256] = {0};
	size_t pos = strlen(CRC_AFFIX);

	memcpy(data_src, CRC_AFFIX, pos);
	memcpy(data_src + pos, p_metro, len_area);

	uint16_t mtr_crc = DataSecurity::Crc16(data_src, pos + len_area - 2);
	memcpy(p_metro + len_area - 2, &mtr_crc, 2);
}

void TicketBase::save_last_stack(ETYTKOPER operType, void * p_trade, size_t size_trade, void * p_written_inf, size_t size_written, bool save_to_file)
{
	cfm_point.operType = operType;
	cfm_point.balance = m_ticket_data.wallet_value;

	memcpy(&cfm_point.trade, p_trade, size_trade);
	cfm_point.size_trade = size_trade;

	memcpy(cfm_point.dataWritten, p_written_inf, size_written);
	cfm_point.sizeWritten = size_written;

	cfm_point.charge_counter = m_ticket_data.charge_counter;
	cfm_point.consume_counter = m_ticket_data.total_trade_counter;

	if (save_to_file)
	{
		save_confirm_to_file();
	}
}

CONFIRM_FACTOR TicketBase::get_confirm_factor()
{
	CONFIRM_FACTOR ret;

	memset(&ret, 0, sizeof(ret));
	ret.tac_type = 0x09;

	uint8_t trade_type = Publics::string_to_hex<uint8_t>((char *)(&cfm_point.trade), 2);
	switch(trade_type)
	{
	case 0x50:	// 单程票发售
		memcpy(ret.logic_id, cfm_point.trade.sjtSale.cLogicalID, 20);
		memcpy(ret.trade_time, cfm_point.trade.sjtSale.dtDate, 7);
		ret.trade_amount	= cfm_point.trade.sjtSale.nChargeValue;

		memcpy(ret.device_inf, cfm_point.trade.sjtSale.cStationID, 4);
		memcpy(ret.device_inf + 4, cfm_point.trade.sjtSale.cDevType, 2);
		memcpy(ret.device_inf + 6, cfm_point.trade.sjtSale.cDeviceID, 3);
		ret.tac_type = Publics::two_char_to_hex(cfm_point.trade.sjtSale.cClassicType);
		break;

	case 0x51:	// 储值票发售
		memcpy(ret.logic_id, cfm_point.trade.svtSale.cLogicalID, 20);
		memcpy(ret.trade_time, cfm_point.trade.svtSale.dtDate, 7);
		ret.trade_amount	= 0;

		memcpy(ret.device_inf, cfm_point.trade.svtSale.cStationID, 4);
		memcpy(ret.device_inf + 4, cfm_point.trade.svtSale.cDevType, 2);
		memcpy(ret.device_inf + 6, cfm_point.trade.svtSale.cDeviceID, 3);
		break;
	case 0x53:	// 进闸
		memcpy(ret.logic_id, cfm_point.trade.entry.cLogicalID, 20);
		memcpy(ret.trade_time, cfm_point.trade.entry.dtDate, 7);
		ret.trade_amount	= 0;

		memcpy(ret.device_inf, cfm_point.trade.entry.cStationID, 4);
		memcpy(ret.device_inf + 4, cfm_point.trade.entry.cDevType, 2);
		memcpy(ret.device_inf + 6, cfm_point.trade.entry.cDeviceID, 3);
		break;
	case 0x54:	// 钱包交易
		memcpy(ret.logic_id, cfm_point.trade.purse.cLogicalID, 20);
		memcpy(ret.trade_time, cfm_point.trade.purse.dtDate, 7);
		ret.trade_amount	= cfm_point.trade.purse.lTradeAmount;

		memcpy(ret.device_inf, cfm_point.trade.purse.cStationID, 4);
		memcpy(ret.device_inf + 4, cfm_point.trade.purse.cDevType, 2);
		memcpy(ret.device_inf + 6, cfm_point.trade.purse.cDeviceID, 3);
		ret.tac_type = Publics::two_char_to_hex(cfm_point.trade.purse.cClassicType);
		break;
	case 0x56:	// 更新
		memcpy(ret.logic_id, cfm_point.trade.update.cLogicalID, 20);
		memcpy(ret.trade_time, cfm_point.trade.update.dtUpdateDate, 7);
		ret.trade_amount	= 0;

		memcpy(ret.device_inf, cfm_point.trade.update.cStationID, 4);
		memcpy(ret.device_inf + 4, cfm_point.trade.update.cDevType, 2);
		memcpy(ret.device_inf + 6, cfm_point.trade.update.cDeviceID, 3);
		break;
	case 0x57:	// 退款
		memcpy(ret.logic_id, cfm_point.trade.refund.cLogicalID, 20);
		memcpy(ret.trade_time, cfm_point.trade.refund.dtDate, 7);
		ret.trade_amount	= cfm_point.trade.refund.lBalanceReturned;

		memcpy(ret.device_inf, cfm_point.trade.refund.cStationID, 4);
		memcpy(ret.device_inf + 4, cfm_point.trade.refund.cDevType, 2);
		memcpy(ret.device_inf + 6, cfm_point.trade.refund.cDeviceID, 3);
		ret.tac_type = Publics::two_char_to_hex(cfm_point.trade.refund.cClassicType);
		break;
	}

	return ret;
}

// 赋值给上次交易未赋值的交易因子
void TicketBase::set_confirm_factor(uint8_t status, long sam_seq, char * p_tac)
{
	uint8_t trade_type = Publics::string_to_hex<uint8_t>((char *)(&cfm_point.trade), 2);
	switch(trade_type)
	{
	case 0x50:	// 单程票发售
		cfm_point.trade.sjtSale.bStatus		= status;
		memcpy(cfm_point.trade.sjtSale.cMACorTAC, p_tac, 10);
		if (sam_seq > 0)
			cfm_point.trade.sjtSale.lSAMTrSeqNo = sam_seq;
		break;
	case 0x51:	// 储值票发售
		cfm_point.trade.svtSale.bStatus		= status;
		if (sam_seq > 0)
			cfm_point.trade.svtSale.lSAMTrSeqNo = sam_seq;
		break;
	case 0x53:	// 进闸
		cfm_point.trade.entry.bStatus		= status;
		if (sam_seq > 0)
			cfm_point.trade.entry.lSAMTrSeqNo = sam_seq;
		break;
	case 0x54:	// 钱包交易
		cfm_point.trade.purse.bStatus		= status;
		memcpy(cfm_point.trade.purse.cMACorTAC, p_tac, 10);
		if (sam_seq > 0)
			cfm_point.trade.purse.lSAMTrSeqNo = sam_seq;
		break;
	case 0x56:	// 更新
		cfm_point.trade.update.bStatus		= status;
		if (sam_seq > 0)
			cfm_point.trade.update.lSAMTrSeqNo = sam_seq;
		break;
	case 0x57:	// 退款
		cfm_point.trade.refund.bStatus		= status;
		memcpy(cfm_point.trade.refund.cMACOrTAC, p_tac, 10);
		if (sam_seq > 0)
			cfm_point.trade.refund.lSAMTrSeqNo = sam_seq;
		break;
	}
}

bool TicketBase::check_confirm(uint8_t status_targ, uint8_t status_src, uint16_t err_targ, uint16_t err_src)
{
	bool ret			= false;

	do
	{
		if (status_targ != 0 && status_src != status_targ)
		{
			memset(&cfm_point, 0, sizeof(cfm_point));
			break;
		}

		if (err_src != 0 && err_targ != 0 && err_src != err_targ)
		{
			memset(&cfm_point, 0, sizeof(cfm_point));
			break;
		}

		ret = true;

	} while (0);

	return ret;
}

uint8_t TicketBase::support_confirm(ETYTKOPER operType)
{
	uint8_t ret = 0;
	switch (operType)
	{
	case operSale:
	case operSvtSale:
	case operSjtSale:
	case operDecr:
	case operIncr:
		if (Api::current_device_type != dvcBOM)
			break;
	case operUpdate:
	case operRefund:
	case operEntry:
	case operExit:
		ret = NTC_MUST_CONFIRM;
		break;
	case operSjtClear:
	case operLock:
	case operDeffer:
	case operNone:
	case operAnalyse:
	case operTkInf:
		break;
	}

	return ret;
}

void TicketBase::clear_confirm(char * p_logic_id, bool check_logic_id, bool save_to_file)
{
	CONFIRM_FACTOR factor = get_confirm_factor();
	if ((check_logic_id && memcmp(p_logic_id, factor.logic_id, 20) == 0) || !check_logic_id)
	{
		memset(&cfm_point, 0, sizeof(cfm_point));

		if (save_to_file)
		{
			save_confirm_to_file();
		}
	}
}

size_t TicketBase::size_of_confirm_trade()
{
	return (size_t)(cfm_point.size_trade);
}

ETYTKOPER TicketBase::confirm_trade_type()
{
	return (ETYTKOPER)(cfm_point.operType);
}

void TicketBase::load_confirm_from_file()
{
	char file_path[_POSIX_PATH_MAX]		= {0};
	FILE * fp							= NULL;
	uint8_t read_buf[2048]				= {0};

	do
	{
		if (Api::current_device_type != dvcBOM)
			break;

		sprintf(file_path, "%s/%s", QFile::running_directory(), NAME_CONFIRM_FILE);

		if (access(file_path, 4) != 0)
			break;

		fp = fopen(file_path, "r");
		if (fp != NULL)
		{
			if (fread(read_buf, 1, sizeof(read_buf), fp) >= sizeof(cfm_point))
				memcpy(&cfm_point, read_buf, sizeof(cfm_point));
		}

		fclose(fp);

	} while (0);
}

bool TicketBase::check_confirm_his(int time_limit_min)
{
	char temp[21]		= {0};
	char device_inf[9]	= {0};
	bool ret			= false;

	do
	{
		CONFIRM_FACTOR ftr	= get_confirm_factor();

		if (memcmp(ftr.logic_id, temp, 20) == 0)
			break;

		memcpy(temp, ftr.logic_id, 20);
		if (strlen(temp) != 20)
			break;

		Api::get_current_device_info(device_inf, device_inf + 4, device_inf + 6);
		if (memcmp(ftr.device_inf, device_inf, sizeof(ftr.device_inf)) != 0)
		{
			memset(&cfm_point, 0, sizeof(cfm_point));
			break;
		}

		if (time_limit_min > 0 && TimesEx::timeout(CmdSort::m_time_now, ftr.trade_time, time_limit_min))
		{
			memset(&cfm_point, 0, sizeof(cfm_point));
			break;
		}

		ret = true;

	} while (0);

	return ret;
}

void TicketBase::save_confirm_to_file()
{
	char file_path[_POSIX_PATH_MAX]		= {0};
	FILE * fp							= NULL;

	do
	{
		sprintf(file_path, "%s/%s", QFile::running_directory(), NAME_CONFIRM_FILE);

		fp = fopen(file_path, "w");
		if (fp != NULL)
		{
			fwrite(&cfm_point, 1, sizeof(cfm_point), fp);
		}

		fclose(fp);

	} while (0);
}

bool TicketBase::confirm_current_card()
{
	CONFIRM_FACTOR ftr = get_confirm_factor();

	return (memcmp(ftr.logic_id, m_ticket_data.logical_id, 20) == 0);
}

bool TicketBase::need_check_period(uint8_t wk_area, SimpleStatus s_status)
{
	bool bCheck = false;

	if (wk_area == 2)
	{
		bCheck = true;
	}
	else if (wk_area == 1)
	{
		if ((s_status != SStatus_Entry && s_status != SStatus_Upd_WZone) || s_status == SStatus_Exit_Only)
			bCheck = true;
	}

	return bCheck;
}

// 获取当前发生的降级模式和敏感期影响信息
void TicketBase::get_degrade_mode(MODE_EFFECT_FLAGS& degrade_falgs, uint8_t * p_time_now)
{
	g_Parameter.query_degrade_mode(degrade_falgs);
	switch (m_ticket_data.simple_status)
	{
	case SStatus_Exit_Tt:
		degrade_falgs.exit_train_trouble_effect =
			g_Parameter.tk_in_mode(degrade_null, false, p_time_now, m_ticket_data.p_exit_info->time, m_ticket_data.p_exit_info->station_id);
		break;
	case SStatus_Entry:
		degrade_falgs.entry_by_emergency =
			g_Parameter.tk_in_mode(degrade_Green_Light, true, p_time_now, m_ticket_data.p_entry_info->time, m_ticket_data.p_entry_info->station_id);
		break;
	case SStatus_Sale:
		degrade_falgs.sell_sjt_by_train_trouble =
			g_Parameter.tk_in_mode(degrade_Trouble, true, p_time_now, m_ticket_data.read_last_info.time, m_ticket_data.read_last_info.station_id);
		degrade_falgs.sell_sjt_by_emergency =
			g_Parameter.tk_in_mode(degrade_Green_Light, true, p_time_now, m_ticket_data.read_last_info.time, m_ticket_data.read_last_info.station_id);
		break;
	case SStatus_Es:
		degrade_falgs.es_sjt_by_train_trouble =
			g_Parameter.EsEffectByAppointedMode(degrade_Trouble, p_time_now, m_ticket_data.logical_peroidE, 0);
		degrade_falgs.es_sjt_by_emergency =
			g_Parameter.EsEffectByAppointedMode(degrade_Green_Light, p_time_now, m_ticket_data.logical_peroidE, 0);
		break;
	case SStatus_Upd_FZone:
		// 主要用于非付费区更新的单程票有效期
		degrade_falgs.update_free_by_emergency =
			g_Parameter.tk_in_mode(degrade_Green_Light, true, p_time_now, m_ticket_data.p_update_info->time, m_ticket_data.p_update_info->station_id);
		break;
	case SStatus_Upd_WZone:
		// 主要用于非付费区更新的单程票有效期
		degrade_falgs.update_work_by_emergency =
			g_Parameter.tk_in_mode(degrade_Green_Light, true, p_time_now, m_ticket_data.p_update_info->time, m_ticket_data.p_update_info->station_id);
		break;
	default:
		break;
	}
}
