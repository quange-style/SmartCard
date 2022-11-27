#include "TicketBase.h"
#include "TimesEx.h"
#include "Api.h"
#include "TicketUl.h"
#include "TicketSvt.h"
#include "TicketBus.h"
#include "TicketBM.h"
#include "TicketTct.h"
#include "TicketJtb.h"
#include "TicketElecT.h"
#include "TicketPostPayElecT.h"
#include "TicketSJTElecT.h"
#include "TicketThirdPay.h"
#include "Publics.h"
#include "Structs.h"
#include "CmdSort.h"
#include "Errors.h"
#include "../link/linker.h"
#include "DataSecurity.h"
#include <stdarg.h>
#include "../link/myprintf.h"
#include <unistd.h>

TICKET_DATA TicketBase::m_ticket_data;
CONFIRM_POINT TicketBase::cfm_point		= {0};
CONFIRM_POINT_E TicketBase::cfm_point_e		= {0};

TicketBase::TicketBase(void)
{
}

TicketBase::TicketBase(char * p_current_sam, char * p_sam_posid, uint8_t * physic_info)
	: m_p_current_sam(p_current_sam), m_p_sam_posid()
{
	memset(&m_ticket_data, 0, sizeof(TICKET_DATA));
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
			Api::search_card_err ++;
			if (Api::search_card_err > SEARCH_CARD_ERR_TIME )
			{
				g_rfmain_status = rfdev_get_rfstatus(1);
				g_rfsub_status |= rfdev_get_rfstatus(2);
				//g_Record.log_out(0, level_error, "if search card err more than 100,rf reset when rf_modify");
				Api::search_card_err = 0;
			}
			dbg_formatvar("---------------------------------rf_modify ERR_CARD_NONE---------------------------------------");
			ret = ERR_CARD_NONE;
			break;
		}

		if( oper_type == operGetETicket)//如果是取票交易，则不用去查看rf的数据
			ret = search_qr_card_ex(recv, type);
		else if (oper_type == operThirdGetETicket)
			ret = search_third_card_ex(recv, type);
		else
			ret = search_card_ex(recv, type);

		if (ret != 0)
		{
			break;
		}

		physical_type = (ETPMDM)type;
		if ((p_forbid_ant != NULL && *p_forbid_ant == CmdSort::m_antenna_mode) && (p_forbid_type != NULL && *p_forbid_type == physical_type))
		{
			ret = ERR_CARD_NONE;
			break;
		}

		memset(PTR_TICKET, 0, SIZE_TICKET);

		switch (physical_type)
		{
		case mediNone:
			ret = ERR_CARD_NONE;
			//g_Record.log_out(0, level_error, "Api::search_card_err = [%d]|SEARCH_CARD_ERR_TIME = [%]",Api::search_card_err,SEARCH_CARD_ERR_TIME);
			Api::search_card_err ++;
			if (Api::search_card_err > SEARCH_CARD_ERR_TIME )
			{
				g_rfmain_status = rfdev_get_rfstatus(1);
				g_rfsub_status |= rfdev_get_rfstatus(2);
				Api::search_card_err = 0;
			}
			break;
		case mediUltraLight:
			*pp_ticket = new(PTR_TICKET) TicketUl(Api::sock_sam_id(SAM_SOCK_1), Api::sock_terminal_id(SAM_SOCK_1), recv);
			mode_app = '2';
			ret = 0;
			break;
		case mediMetroCpu:
			dbg_formatvar("mediMetroCpu");
			*pp_ticket = new(PTR_TICKET) TicketSvt(Api::sock_sam_id(SAM_SOCK_1), Api::sock_terminal_id(SAM_SOCK_1), recv);
			mode_app = '1';
			ret = 0;
			break;

		case mediBusCpu:
			dbg_formatvar("mediBusCpu");
			*pp_ticket = new(PTR_TICKET) TicketBus(Api::sock_sam_id(SAM_SOCK_2), Api::sock_terminal_id(SAM_SOCK_2), recv);
			mode_app = '1';
			ret = 0;
			break;

		case mediBusMifare:
			dbg_formatvar("mediBusMifare");
			*pp_ticket = new(PTR_TICKET) TicketBM(Api::sock_sam_id(SAM_SOCK_3), Api::sock_terminal_id(SAM_SOCK_3), recv);
			mode_app = '2';
			ret = 0;
			break;

		case mediBusMfEX:
			*pp_ticket = new(PTR_TICKET) TicketBM(Api::sock_sam_id(SAM_SOCK_3), Api::sock_terminal_id(SAM_SOCK_3), recv);
			mode_app = '3';
			ret = 0;
			break;

		case mediMetroCpuTct:
			dbg_formatvar("mediMetroCpuTct");
			*pp_ticket = new(PTR_TICKET) TicketTct(Api::sock_sam_id(SAM_SOCK_1), Api::sock_terminal_id(SAM_SOCK_1), recv);
			mode_app = '1';
			ret = 0;
			break;

		case mediJtbCpu:
			dbg_formatvar("mediJtbCpu");
			*pp_ticket = new(PTR_TICKET) TicketJtb(Api::sock_sam_id(SAM_SOCK_4), Api::sock_terminal_id(SAM_SOCK_4), recv);
			mode_app = '1';
			ret = 0;
			break;

		case mediElecULTictet:
			//dbg_formatvar("mediElecTictet");
			*pp_ticket = new(PTR_TICKET) TicketElecT(Api::sock_sam_id(SAM_SOCK_1), Api::sock_terminal_id(SAM_SOCK_1), recv);
			mode_app = '1';
			ret = 0;
			break;

		case mediElecSJTTictet:
			dbg_formatvar("mediElecSJTTictet");
			*pp_ticket = new(PTR_TICKET) TicketSJTElecT(Api::sock_sam_id(SAM_SOCK_1), Api::sock_terminal_id(SAM_SOCK_1), recv);
			mode_app = '1';
			ret = 0;
			break;

		case mediElecPostPayTictet:
			dbg_formatvar("mediElecPostPayTictet");
			*pp_ticket = new(PTR_TICKET) TicketPostPayElecT(Api::sock_sam_id(SAM_SOCK_1), Api::sock_terminal_id(SAM_SOCK_1), recv);
			mode_app = '1';
			ret = 0;
			break;

		case mediThirdPay:
			dbg_formatvar("mediThirdPay");
			*pp_ticket = new(PTR_TICKET) TicketThirdPay(Api::sock_sam_id(SAM_SOCK_1), Api::sock_terminal_id(SAM_SOCK_1), recv);
			mode_app = '1';
			ret = 0;
			break;

		default:
			ret = ERR_CARD_NONE;
			break;
		}
	}
	while (0);

	if (ret == 0)
	{

		Api::search_card_err = 0;//寻卡成功，参数清零

		if (p_current_id != NULL)
			memcpy(p_current_id, recv, 7);
		(*pp_ticket)->m_ticket_data.physical_type = physical_type;
		(*pp_ticket)->m_ticket_data.ticket_app_mode = mode_app;
	}

	return ret;
}


void TicketBase::transfer_edata_for_out(ETYTKOPER operType, void * pTradeStruct/*, uint8_t bPaymentMeans*/)
{
	uint8_t * pTimeNow	= NULL;
	uint8_t * pTime1	= NULL;		// 用于将非法的时间转换为合法的时间
	uint8_t * pTime2	= NULL;

	char szTemp[32]		= {0};

	switch (operType)
	{
	case operEntry:
		{
			PETICKETDEALINFO pGetETicket = (PETICKETDEALINFO)pTradeStruct;
			memcpy(pGetETicket->cTransCode, "07", 2);
			sprintf(szTemp, "%02x%02x%02x%02x", CmdSort::m_time_now[0], CmdSort::m_time_now[1], CmdSort::m_time_now[2], CmdSort::m_time_now[3]);
			memcpy(pGetETicket->cTxnDate, szTemp, 8);

			memcpy(pGetETicket->cTicketMainType, "11", 2);//主类型
			//memcpy(pGetETicket->cTicketSubType, "00", 2);//子类型


			memcpy(pGetETicket->cTicketSubType, m_ticket_data.qr_ticketdata.subType, 2);//子类型

			memcpy(pGetETicket->cChipType, "07", 2);//芯片类型

			memcpy(pGetETicket->cTicketLogicalNo, m_ticket_data.qr_ticketdata.cardNo, 20);//逻辑卡号

			memcpy(pGetETicket->cCenterCode, m_ticket_data.qr_ticketdata.centerCode, 32);//中心票号

			memcpy(pGetETicket->cCenterMac, m_ticket_data.qr_ticketdata.mac, 8);//mac

			memcpy(pGetETicket->cRealTicketNo, "0000000000000000", 16);//实体卡号

			memcpy(pGetETicket->cMark, "00", 2);//取票交易

			memcpy(pGetETicket->cTicketCSN, "0000000000000000", 16);//物理卡号

			sprintf(pGetETicket->cTicketCount,"%010ld",m_ticket_data.qr_ticketdata.counter);
			//memcpy(pGetETicket->cTicketCount, "0000000000", 10);//票卡计数

			memcpy(pGetETicket->cPsamNo, m_p_current_sam, 16);

			memcpy(pGetETicket->cPsamSeq, "0000000000", 10);

			sprintf(szTemp, "%02x%02x%02x%02x%02x%02x%02x", CmdSort::m_time_now[0], CmdSort::m_time_now[1], CmdSort::m_time_now[2], CmdSort::m_time_now[3], CmdSort::m_time_now[4], CmdSort::m_time_now[5], CmdSort::m_time_now[6]);
			memcpy(pGetETicket->cTxnTms, szTemp, 14);

			memcpy(pGetETicket->cValidOutTms, m_ticket_data.qr_ticketdata.validOutTime, 14);

			memcpy(pGetETicket->cPayType, "01", 2);

			sprintf(pGetETicket->cBeforeTxnBalance,"%010ld",m_ticket_data.qr_ticketdata.balance);

			memcpy(pGetETicket->cTxnAmountNo, "000000", 6);

			sprintf(pGetETicket->cTxnAmount,"%06ld",0);

			memcpy(pGetETicket->cDiscountAmt, "000000", 6);

			sprintf(pGetETicket->cTxnStation, "%04x", Api::current_station_id);

			//sprintf(dev_info, "%04x%02x%03x", Api::current_station_id, Api::current_device_type, Api::current_device_id);

			//sprintf(pGetETicket->cLastStation, "%04x", Api::current_station_id);
			memcpy(pGetETicket->cLastStation, "0000", 4);

			//sprintf(pGetETicket->cTxnStation, "%04x", Api::current_station_id);

			memcpy(pGetETicket->cLastTxnTms, "00000000000000", 14);

			sprintf(pGetETicket->cDevNodeId, "%04x%02x%03x", Api::current_station_id, Api::current_device_type, Api::current_device_id);

			memcpy(pGetETicket->cDevSeqNo, "0000000000", 10);

			memcpy(pGetETicket->cTestMark, "00", 2);

			memcpy(pGetETicket->cQRSerialNo, "00000000", 8);

			memcpy(pGetETicket->cOverAmount, "000000", 6);

			memcpy(pGetETicket->cTac, "00000000", 8);

			memcpy(pGetETicket->cNetTranNumber, "0000000000000000000000000000000000000000000000000000000000000000", 64);

			memcpy(pGetETicket->cReserve, "00000000000000000000000000000000", 32);

			//dev_inf[0] = (uint8_t)((current_station_id >> 8) & 0xFF);
			//dev_inf[1] = (uint8_t)(current_station_id & 0xFF);
		}
		break;
	case operExit:
		{
			PETICKETDEALINFO pGetETicket = (PETICKETDEALINFO)pTradeStruct;
			memcpy(pGetETicket->cTransCode, "08", 2);
			sprintf(szTemp, "%02x%02x%02x%02x", CmdSort::m_time_now[0], CmdSort::m_time_now[1], CmdSort::m_time_now[2], CmdSort::m_time_now[3]);
			memcpy(pGetETicket->cTxnDate, szTemp, 8);

			memcpy(pGetETicket->cTicketMainType, "11", 2);//主类型
			//memcpy(pGetETicket->cTicketSubType, "00", 2);//子类型

			memcpy(pGetETicket->cTicketSubType, m_ticket_data.qr_ticketdata.subType, 2);//子类型

			memcpy(pGetETicket->cChipType, "07", 2);//芯片类型

			memcpy(pGetETicket->cTicketLogicalNo, m_ticket_data.qr_ticketdata.cardNo, 20);//逻辑卡号

			memcpy(pGetETicket->cCenterCode, m_ticket_data.qr_ticketdata.centerCode, 32);//中心票号

			memcpy(pGetETicket->cCenterMac, m_ticket_data.qr_ticketdata.mac, 8);//mac

			memcpy(pGetETicket->cRealTicketNo, "0000000000000000", 16);//实体卡号

			memcpy(pGetETicket->cMark, "00", 2);//取票交易

			memcpy(pGetETicket->cTicketCSN, "0000000000000000", 16);//物理卡号

			sprintf(pGetETicket->cTicketCount,"%010ld",m_ticket_data.qr_ticketdata.counter);
			//memcpy(pGetETicket->cTicketCount, "0000000000", 10);//票卡计数

			memcpy(pGetETicket->cPsamNo, m_p_current_sam, 16);

			memcpy(pGetETicket->cPsamSeq, "0000000000", 10);

			sprintf(szTemp, "%02x%02x%02x%02x%02x%02x%02x", CmdSort::m_time_now[0], CmdSort::m_time_now[1], CmdSort::m_time_now[2], CmdSort::m_time_now[3], CmdSort::m_time_now[4], CmdSort::m_time_now[5], CmdSort::m_time_now[6]);
			memcpy(pGetETicket->cTxnTms, szTemp, 14);

			memcpy(pGetETicket->cValidOutTms, m_ticket_data.qr_ticketdata.validOutTime, 14);

			memcpy(pGetETicket->cPayType, "01", 2);

			sprintf(pGetETicket->cBeforeTxnBalance,"%010ld",m_ticket_data.qr_ticketdata.balance);

			memcpy(pGetETicket->cTxnAmountNo, "000000", 6);

			sprintf(pGetETicket->cTxnAmount,"%06ld",m_ticket_data.qr_ticketdata.TxnAmount);

			memcpy(pGetETicket->cDiscountAmt, "000000", 6);

			sprintf(pGetETicket->cTxnStation, "%04x", Api::current_station_id);

			//sprintf(dev_info, "%04x%02x%03x", Api::current_station_id, Api::current_device_type, Api::current_device_id);

			memcpy(pGetETicket->cLastStation, m_ticket_data.qr_ticketdata.startStation, 4);
			memcpy(pGetETicket->cLastTxnTms, m_ticket_data.qr_ticketdata.entryTime, 14);

			//sprintf(pGetETicket->cLastStation, "%04x", Api::current_station_id);

			//sprintf(pGetETicket->cTxnStation, "%04x", Api::current_station_id);

			//memcpy(pGetETicket->cLastTxnTms, "20180725114500", 14);

			sprintf(pGetETicket->cDevNodeId, "%04x%02x%03x", Api::current_station_id, Api::current_device_type, Api::current_device_id);

			memcpy(pGetETicket->cDevSeqNo, "0000000000", 10);

			memcpy(pGetETicket->cTestMark, "00", 2);

			memcpy(pGetETicket->cQRSerialNo, "00000000", 8);

			memcpy(pGetETicket->cOverAmount, "000000", 6);

			memcpy(pGetETicket->cTac, "00000000", 8);

			memcpy(pGetETicket->cNetTranNumber, "0000000000000000000000000000000000000000000000000000000000000000", 64);

			memcpy(pGetETicket->cReserve, "00000000000000000000000000000000", 32);

			//dev_inf[0] = (uint8_t)((current_station_id >> 8) & 0xFF);
			//dev_inf[1] = (uint8_t)(current_station_id & 0xFF);
		}
		break;
	case operGetETicket:
		{
			PGETETICKETRESPINFO pGetETicketResp = (PGETETICKETRESPINFO)pTradeStruct;
			memcpy(pGetETicketResp->cAmount, m_ticket_data.qr_ticketdata.amount, 2);

			sprintf(szTemp, "%08ld",m_ticket_data.qr_ticketdata.sum);

			sprintf(pGetETicketResp->cSum, szTemp,8);

			memcpy(pGetETicketResp->cCenterCode, m_ticket_data.qr_ticketdata.centerCode, 32);
			memcpy(pGetETicketResp->cTicketLogicalNo, m_ticket_data.qr_ticketdata.cardNo, 20);
			memcpy(pGetETicketResp->cStartStation, m_ticket_data.qr_ticketdata.startStation, 4);
			memcpy(pGetETicketResp->cTerminalStation, m_ticket_data.qr_ticketdata.terminalStation, 4);
		}
		break;
	case operGetETicketRecord:
		{
			PETICKETDEALINFO pGetETicket = (PETICKETDEALINFO)pTradeStruct;
			memcpy(pGetETicket->cTransCode, "10", 2);
			sprintf(szTemp, "%02x%02x%02x%02x", CmdSort::m_time_now[0], CmdSort::m_time_now[1], CmdSort::m_time_now[2], CmdSort::m_time_now[3]);
			memcpy(pGetETicket->cTxnDate, szTemp, 8);

			memcpy(pGetETicket->cTicketMainType, "11", 2);//主类型

			memcpy(m_ticket_data.qr_ticketdata.subType, "00", 2);

			memcpy(pGetETicket->cTicketSubType, m_ticket_data.qr_ticketdata.subType, 2);//子类型

			memcpy(pGetETicket->cChipType, "07", 2);//芯片类型

			//memcpy(pGetETicket->cTicketLogicalNo, "00000000000000000000", 20);//逻辑卡号
			memcpy(pGetETicket->cTicketLogicalNo, m_ticket_data.qr_ticketdata.cardNo, 20);//逻辑卡号

			memcpy(pGetETicket->cCenterCode, m_ticket_data.qr_ticketdata.centerCode, 32);//中心票号

			memcpy(pGetETicket->cCenterMac, "00000000", 8);//mac

			memcpy(pGetETicket->cRealTicketNo, m_ticket_data.logical_id + 4, 16);//实体卡号

			memcpy(pGetETicket->cMark, "00", 2);//取票交易

			memcpy(pGetETicket->cTicketCSN, m_ticket_data.physical_id, 16);//物理卡号

			sprintf(pGetETicket->cTicketCount,"%010ld",m_ticket_data.qr_ticketdata.counter);
			//memcpy(pGetETicket->cTicketCount, "0000000000", 10);//票卡计数

			memcpy(pGetETicket->cPsamNo, m_p_current_sam, 16);

			memcpy(pGetETicket->cPsamSeq, "0000000000", 10);

			sprintf(szTemp, "%02x%02x%02x%02x%02x%02x%02x", CmdSort::m_time_now[0], CmdSort::m_time_now[1], CmdSort::m_time_now[2], CmdSort::m_time_now[3], CmdSort::m_time_now[4], CmdSort::m_time_now[5], CmdSort::m_time_now[6]);
			memcpy(pGetETicket->cTxnTms, szTemp, 14);

			memcpy(pGetETicket->cValidOutTms, "00000000000000", 14);

			//memcpy(pGetETicket->cPayType, "01", 2);

			sprintf(pGetETicket->cBeforeTxnBalance,"%010ld",m_ticket_data.qr_ticketdata.balance);

			memcpy(pGetETicket->cTxnAmountNo, "000000", 6);

			sprintf(pGetETicket->cTxnAmount,"%06ld",m_ticket_data.qr_ticketdata.balance);

			memcpy(pGetETicket->cDiscountAmt, "000000", 6);

			sprintf(pGetETicket->cTxnStation, "%04x", Api::current_station_id);

			//sprintf(dev_info, "%04x%02x%03x", Api::current_station_id, Api::current_device_type, Api::current_device_id);

			//sprintf(pGetETicket->cLastStation, "%04x", Api::current_station_id);
			sprintf(pGetETicket->cLastStation, "%04x", Api::current_station_id);
			memcpy(pGetETicket->cLastStation, "0000", 4);

			//sprintf(pGetETicket->cTxnStation, "%04x", Api::current_station_id);

			memcpy(pGetETicket->cLastTxnTms, "00000000000000", 14);

			sprintf(pGetETicket->cDevNodeId, "%04x%02x%03x", Api::current_station_id, Api::current_device_type, Api::current_device_id);

			memcpy(pGetETicket->cDevSeqNo, "0000000000", 10);

			memcpy(pGetETicket->cTestMark, "00", 2);

			memcpy(pGetETicket->cQRSerialNo, "00000000", 8);

			memcpy(pGetETicket->cOverAmount, "000000", 6);

			memcpy(pGetETicket->cTac, "00000000", 8);

			memcpy(pGetETicket->cNetTranNumber, "0000000000000000000000000000000000000000000000000000000000000000", 64);

			memcpy(pGetETicket->cReserve, "00000000000000000000000000000000", 32);

			//dev_inf[0] = (uint8_t)((current_station_id >> 8) & 0xFF);
			//dev_inf[1] = (uint8_t)(current_station_id & 0xFF);
		}
		break;
	case operThirdGetETicket:
		{
			PETICKETDEALINFO pGetETicket = (PETICKETDEALINFO)pTradeStruct;
			memcpy(pGetETicket->cTransCode, "13", 2);
			sprintf(szTemp, "%02x%02x%02x%02x", CmdSort::m_time_now[0], CmdSort::m_time_now[1], CmdSort::m_time_now[2], CmdSort::m_time_now[3]);
			memcpy(pGetETicket->cTxnDate, szTemp, 8);

			memcpy(pGetETicket->cTicketMainType, "01", 2);//主类型

			memcpy(m_ticket_data.qr_ticketdata.subType, "00", 2);

			memcpy(pGetETicket->cTicketSubType, m_ticket_data.qr_ticketdata.subType, 2);//子类型

			memcpy(pGetETicket->cChipType, "07", 2);//芯片类型

			memcpy(pGetETicket->cTicketLogicalNo, "00000000000000000000", 20);//逻辑卡号
			//memcpy(pGetETicket->cTicketLogicalNo, m_ticket_data.qr_ticketdata.cardNo, 20);//逻辑卡号

			memcpy(pGetETicket->cCenterCode, "0000000000000000000000000000000000000000", 32);//中心票号

			memcpy(pGetETicket->cCenterMac, "00000000", 8);//mac

			memcpy(pGetETicket->cRealTicketNo, m_ticket_data.logical_id + 4, 16);//实体卡号

			memcpy(pGetETicket->cMark, "00", 2);//取票交易

			memcpy(pGetETicket->cTicketCSN, m_ticket_data.physical_id, 16);//物理卡号

			//sprintf(pGetETicket->cTicketCount,"%010ld",m_ticket_data.qr_ticketdata.counter);
			memcpy(pGetETicket->cTicketCount, "0000000000", 10);//票卡计数

			memcpy(pGetETicket->cPsamNo, m_p_current_sam, 16);

			memcpy(pGetETicket->cPsamSeq, "0000000000", 10);

			sprintf(szTemp, "%02x%02x%02x%02x%02x%02x%02x", CmdSort::m_time_now[0], CmdSort::m_time_now[1], CmdSort::m_time_now[2], CmdSort::m_time_now[3], CmdSort::m_time_now[4], CmdSort::m_time_now[5], CmdSort::m_time_now[6]);
			memcpy(pGetETicket->cTxnTms, szTemp, 14);

			memcpy(pGetETicket->cValidOutTms, "00000000000000", 14);

			//memcpy(pGetETicket->cPayType, "01", 2);

			sprintf(pGetETicket->cBeforeTxnBalance,"%010ld",m_ticket_data.qr_ticketdata.balance);

			memcpy(pGetETicket->cTxnAmountNo, "000000", 6);

			sprintf(pGetETicket->cTxnAmount,"%06ld",m_ticket_data.qr_ticketdata.balance);

			memcpy(pGetETicket->cDiscountAmt, "000000", 6);

			sprintf(pGetETicket->cTxnStation, "%04x", Api::current_station_id);

			//sprintf(dev_info, "%04x%02x%03x", Api::current_station_id, Api::current_device_type, Api::current_device_id);

			//sprintf(pGetETicket->cLastStation, "%04x", Api::current_station_id);
			sprintf(pGetETicket->cLastStation, "%04x", Api::current_station_id);
			memcpy(pGetETicket->cLastStation, "0000", 4);

			//sprintf(pGetETicket->cTxnStation, "%04x", Api::current_station_id);

			memcpy(pGetETicket->cLastTxnTms, "00000000000000", 14);

			sprintf(pGetETicket->cDevNodeId, "%04x%02x%03x", Api::current_station_id, Api::current_device_type, Api::current_device_id);

			memcpy(pGetETicket->cDevSeqNo, "0000000000", 10);

			memcpy(pGetETicket->cTestMark, "00", 2);

			memcpy(pGetETicket->cQRSerialNo, "00000000", 8);

			memcpy(pGetETicket->cOverAmount, "000000", 6);

			memcpy(pGetETicket->cTac, "00000000", 8);

			memcpy(pGetETicket->cNetTranNumber, m_ticket_data.qr_ticketdata.NetTranNumber, 64);

			memcpy(pGetETicket->cReserve, "00000000000000000000000000000000", 32);

			//dev_inf[0] = (uint8_t)((current_station_id >> 8) & 0xFF);
			//dev_inf[1] = (uint8_t)(current_station_id & 0xFF);
		}
		break;
	case operAnalyse:
		{
			PBOMETICANALYZE pGetETicket = (PBOMETICANALYZE)pTradeStruct;
			memcpy(pGetETicket->cIssueCode, "CSMG", 4);
			memcpy(pGetETicket->cVersion, m_ticket_data.qr_ticketdata.ticketVersion, 2);
			pGetETicket->cCardStatus = m_ticket_data.qr_ticket_status;//票卡状态

			memcpy(pGetETicket->cTicketLogicalNo, m_ticket_data.qr_ticketdata.cardNo, 20);//逻辑卡号

			//sprintf(szTemp, "%02x%02x%02x%02x", CmdSort::m_time_now[0], CmdSort::m_time_now[1], CmdSort::m_time_now[2], CmdSort::m_time_now[3]);
			//memcpy(pGetETicket->cTxnDate, szTemp, 8);

			memcpy(pGetETicket->cTicketSubType, m_ticket_data.qr_ticketdata.subType, 2);//子类型
			pGetETicket->lBalance = m_ticket_data.qr_ticketdata.balance;//余额
			pGetETicket->lTicketCount = m_ticket_data.qr_ticketdata.counter;//计数器

			memcpy(pGetETicket->cCenterCode, m_ticket_data.qr_ticketdata.centerCode, 32);//中心票号

			memcpy(pGetETicket->cSaleDate, "00000000000000", 14);//售票时间
			memcpy(pGetETicket->cValidDate, "00000000", 8);//有效期
			memcpy(pGetETicket->cStartStationId, "0000", 4);//起始站编码
			memcpy(pGetETicket->cEndStationId, "0000", 4);//终点站编码
			memcpy(pGetETicket->cTicketNum,"00", 2);//张数
			dbg_formatvar("sum = %d   %02d   %02ld",m_ticket_data.qr_ticketdata.sum,m_ticket_data.qr_ticketdata.sum,m_ticket_data.qr_ticketdata.sum);

			if(memcmp(m_ticket_data.qr_ticketdata.subType, "00", 2) == 0){
				memcpy(pGetETicket->cSaleDate, m_ticket_data.qr_ticketdata.saleTime, 14);//售票时间
				memcpy(pGetETicket->cValidDate, m_ticket_data.qr_ticketdata.validDate, 8);//有效期
				memcpy(pGetETicket->cStartStationId, m_ticket_data.qr_ticketdata.startStation, 4);//起始站编码
				memcpy(pGetETicket->cEndStationId, m_ticket_data.qr_ticketdata.terminalStation, 4);//终点站编码
				memcpy(pGetETicket->cTicketNum, m_ticket_data.qr_ticketdata.amount, 2);//张数
			}else if(memcmp(m_ticket_data.qr_ticketdata.subType, "01", 2) == 0){
			}else if(memcmp(m_ticket_data.qr_ticketdata.subType, "02", 2) == 0){
			}
			memcpy(pGetETicket->cEntryDate, m_ticket_data.qr_ticketdata.entryTime, 14);//进站时间

			memcpy(pGetETicket->cEntryStationId, m_ticket_data.qr_ticketdata.startStation, 4);//进站站点

			memcpy(pGetETicket->cExitDate, m_ticket_data.qr_ticketdata.exitTime, 14);//出站时间

			memcpy(pGetETicket->cExitStationId, m_ticket_data.qr_ticketdata.terminalStation, 4);//出站站点

			memcpy(pGetETicket->cValidExitDate, m_ticket_data.qr_ticketdata.validOutTime, 14);//有效出站时间

			memcpy(pGetETicket->cLastDealType, "00", 2);//上次交易类型

			memcpy(pGetETicket->cLastDealDate, "00000000000000", 14);//上次交易时间

			memcpy(pGetETicket->cPenaltyType, m_ticket_data.qr_ticketdata.cPenaltyType, 2);//罚款类型

			pGetETicket->lPenalty1 = m_ticket_data.qr_ticketdata.lPenalty1;//罚款金额

			pGetETicket->lPenalty2 = m_ticket_data.qr_ticketdata.lPenalty2;//更新罚金

		}
		break;
	default:
		break;
	}

	invalid_time_change_current(pTime1);
	invalid_time_change_current(pTime2);
}

void TicketBase::getDealDataSjt(ETYTKOPER operType,uint16_t& len_data,uint8_t * dataBack,long amount)
{
	int length = 0;
	char StationCode[2+1] = {0};
	char LineCode[2+1] = {0};
	char DeviceType[2+1] = {0};
	char DeviceCode[3+1] = {0};
	char PsamCode[12+1] = {0};
	char time[14+1] = {0};
	char exittime[14+1] = {0};
	long penalty_val	= 0;
	uint8_t tempTime[7] = {0};
	uint8_t validexitTime[7] = {0};

	char dealData[256] = {0};

	sprintf(StationCode, "%02x", ((Api::current_station_id >>8) & 0x00FF));
	sprintf(LineCode, "%02x", (Api::current_station_id & 0xFF));
	sprintf(DeviceType, "%02x", (uint8_t)(Api::current_device_type));
	sprintf(DeviceCode, "%03x", Api::current_device_id);
	//sprintf(time, "%02x%02x%02x%02x%02x%02x%02x", CmdSort::m_time_now[0], CmdSort::m_time_now[1], CmdSort::m_time_now[2], CmdSort::m_time_now[3], CmdSort::m_time_now[4], CmdSort::m_time_now[5], CmdSort::m_time_now[6]);
	memcpy(tempTime, CmdSort::m_time_now, 7);
	sprintf(time, "%02x%02x%02x%02x%02x%02x%02x", tempTime[0], tempTime[1], tempTime[2], tempTime[3], tempTime[4], tempTime[5], tempTime[6]);

	g_Parameter.get_validexittime(Api::current_station_id,tempTime, validexitTime, &penalty_val);

	dbg_dumpmemory("validexitTime:",validexitTime,7);

	sprintf(exittime, "%02x%02x%02x%02x%02x%02x%02x", validexitTime[0], validexitTime[1], validexitTime[2], validexitTime[3], validexitTime[4], validexitTime[5], validexitTime[6]);

	memcpy(m_ticket_data.qr_ticketdata.validOutTime, exittime, 14);

	switch (operType)
	{
	case operEntry:
		{
			memcpy(dealData,"01",2);
			length += 2;
			//dbg_dumpmemory("dealData1:",dealData,length);
			memcpy(dealData + length, time, 14);
			length += 14;
			memcpy(dealData + length, exittime, 14);
			length += 14;
			//dbg_dumpmemory("dealData2:",dealData,length);
			memcpy(dealData + length, StationCode, 2);//线路编码
			length += 2;
			//dbg_dumpmemory("dealData3:",dealData,length);
			memcpy(dealData + length, LineCode, 2);//站点编码
			length += 2;
			//dbg_dumpmemory("dealData4:",dealData,length);
			memcpy(dealData + length, DeviceType, 2);//设备类型
			length += 2;
			//dbg_dumpmemory("dealData5:",dealData,length);
			memcpy(dealData + length, DeviceCode, 3);//设备编号
			length += 3;
			//dbg_dumpmemory("dealData6:",dealData,length);
			//memcpy(dealData + length, g_saminf[0].sam_terminal_id, 12);
			memcpy(dealData + length, "112233445566", 12);//暂时不写
			length += 12;
			//dbg_dumpmemory("dealData8:",dealData,length);
			memcpy(dealData + length, "00", 2);
			length += 2;
			//dbg_dumpmemory("dealData9:",dealData,length);
			memcpy(dealData + length, "00000000", 8);
			length += 8;
			//dbg_dumpmemory("dealData10:",dealData,length);
			len_data = length;

		}
		break;
	case operExit:
		{
			memcpy(dealData,"02",2);
			length += 2;
			//dbg_dumpmemory("dealData1:",dealData,length);
			memcpy(dealData + length, time, 14);
			length += 14;
			//dbg_dumpmemory("dealData2:",dealData,length);
			memcpy(dealData + length, StationCode, 2);//线路编码
			length += 2;
			//dbg_dumpmemory("dealData3:",dealData,length);
			memcpy(dealData + length, LineCode, 2);//站点编码
			length += 2;
			//dbg_dumpmemory("dealData4:",dealData,length);
			memcpy(dealData + length, DeviceType, 2);//设备类型
			length += 2;
			//dbg_dumpmemory("dealData5:",dealData,length);
			memcpy(dealData + length, DeviceCode, 3);//设备编号
			length += 3;
			//dbg_dumpmemory("dealData6:",dealData,length);
			memcpy(dealData + length, "112233445566", 12);
			length += 12;
			sprintf(dealData + length,"%08X",amount);
			//dbg_formatvar("price:%08X",amount);
			//memcpy(dealData + length, "000001F4", 8);
			length += 8;

			//dbg_dumpmemory("dealData8:",dealData,length);
			memcpy(dealData + length, "00", 2);
			length += 2;
			//dbg_dumpmemory("dealData9:",dealData,length);
			memcpy(dealData + length, "00000000", 8);
			length += 8;
			//dbg_dumpmemory("dealData10:",dealData,length);
			len_data = length;

		}
		break;
    default:
        break;
	}
	memcpy(dataBack, (uint8_t * )dealData, length);

}


void TicketBase::getDealDataPos(ETYTKOPER operType,uint16_t& len_data,uint8_t * dataBack,long amount)
{
	int length = 0;
	char StationCode[2+1] = {0};
	char LineCode[2+1] = {0};
	char DeviceType[2+1] = {0};
	char DeviceCode[3+1] = {0};
	char PsamCode[12+1] = {0};
	char time[14+1] = {0};
	char exittime[14+1] = {0};
	long penalty_val	= 0;
	uint8_t tempTime[7] = {0};
	uint8_t validexitTime[7] = {0};

	char dealData[256] = {0};

	sprintf(StationCode, "%02x", ((Api::current_station_id >>8) & 0x00FF));
	sprintf(LineCode, "%02x", (Api::current_station_id & 0xFF));
	sprintf(DeviceType, "%02x", (uint8_t)(Api::current_device_type));
	sprintf(DeviceCode, "%03x", Api::current_device_id);

	memcpy(tempTime, CmdSort::m_time_now, 7);
	sprintf(time, "%02x%02x%02x%02x%02x%02x%02x", tempTime[0], tempTime[1], tempTime[2], tempTime[3], tempTime[4], tempTime[5], tempTime[6]);
	//sprintf(time, "%02x%02x%02x%02x%02x%02x%02x", CmdSort::m_time_now[0], CmdSort::m_time_now[1], CmdSort::m_time_now[2], CmdSort::m_time_now[3], CmdSort::m_time_now[4], CmdSort::m_time_now[5], CmdSort::m_time_now[6]);

	g_Parameter.get_validexittime(Api::current_station_id,tempTime, validexitTime, &penalty_val);

	dbg_dumpmemory("validexitTime:",validexitTime,7);

	sprintf(exittime, "%02x%02x%02x%02x%02x%02x%02x", validexitTime[0], validexitTime[1], validexitTime[2], validexitTime[3], validexitTime[4], validexitTime[5], validexitTime[6]);

	memcpy(m_ticket_data.qr_ticketdata.validOutTime, exittime, 14);

	switch (operType)
	{
	case operEntry:
		{
			memcpy(dealData,"01",2);
			length += 2;
			//dbg_dumpmemory("dealData1:",dealData,length);
			memcpy(dealData + length, time, 14);
			length += 14;
			memcpy(dealData + length, exittime, 14);
			length += 14;
			//dbg_dumpmemory("dealData2:",dealData,length);
			memcpy(dealData + length, StationCode, 2);//线路编码
			length += 2;
			//dbg_dumpmemory("dealData3:",dealData,length);
			memcpy(dealData + length, LineCode, 2);//站点编码
			length += 2;
			//dbg_dumpmemory("dealData4:",dealData,length);
			memcpy(dealData + length, DeviceType, 2);//设备类型
			length += 2;
			//dbg_dumpmemory("dealData5:",dealData,length);
			memcpy(dealData + length, DeviceCode, 3);//设备编号
			length += 3;
			//dbg_dumpmemory("dealData6:",dealData,length);
			//memcpy(dealData + length, g_saminf[0].sam_terminal_id, 12);
			memcpy(dealData + length, "112233445566", 12);//暂时不写
			length += 12;
			//dbg_dumpmemory("dealData8:",dealData,length);
			memcpy(dealData + length, "00", 2);
			length += 2;
			//dbg_dumpmemory("dealData9:",dealData,length);
			memcpy(dealData + length, "00000000", 8);
			length += 8;
			//dbg_dumpmemory("dealData10:",dealData,length);
			len_data = length;

		}
		break;
	case operExit:
		{
			memcpy(dealData,"02",2);
			length += 2;
			//dbg_dumpmemory("dealData1:",dealData,length);
			memcpy(dealData + length, time, 14);
			length += 14;
			//dbg_dumpmemory("dealData2:",dealData,length);
			memcpy(dealData + length, StationCode, 2);//线路编码
			length += 2;
			//dbg_dumpmemory("dealData3:",dealData,length);
			memcpy(dealData + length, LineCode, 2);//站点编码
			length += 2;
			//dbg_dumpmemory("dealData4:",dealData,length);
			memcpy(dealData + length, DeviceType, 2);//设备类型
			length += 2;
			//dbg_dumpmemory("dealData5:",dealData,length);
			memcpy(dealData + length, DeviceCode, 3);//设备编号
			length += 3;
			//dbg_dumpmemory("dealData6:",dealData,length);
			memcpy(dealData + length, "112233445566", 12);
			length += 12;
			sprintf(dealData + length,"%08X",amount);
			//dbg_formatvar("price:%08X",amount);
			//memcpy(dealData + length, "000001F4", 8);
			length += 8;

			//dbg_dumpmemory("dealData8:",dealData,length);
			memcpy(dealData + length, "00", 2);
			length += 2;
			//dbg_dumpmemory("dealData9:",dealData,length);
			memcpy(dealData + length, "00000000", 8);
			length += 8;
			//dbg_dumpmemory("dealData10:",dealData,length);
			len_data = length;

		}
		break;
    default:
        break;
	}
	memcpy(dataBack, (uint8_t * )dealData, length);

}


void TicketBase::getDealData(ETYTKOPER operType,uint16_t& len_data,uint8_t * dataBack,long amount)
{
	int length = 0;
	char StationCode[2+1] = {0};
	char LineCode[2+1] = {0};
	char DeviceType[2+1] = {0};
	char DeviceCode[3+1] = {0};
	char PsamCode[12+1] = {0};
	char time[14+1] = {0};
	char exittime[14+1] = {0};
	long penalty_val	= 0;
	uint8_t tempTime[7] = {0};
	uint8_t validexitTime[7] = {0};

	char dealData[256] = {0};

	sprintf(StationCode, "%02x", ((Api::current_station_id >>8) & 0x00FF));
	sprintf(LineCode, "%02x", (Api::current_station_id & 0xFF));
	sprintf(DeviceType, "%02x", (uint8_t)(Api::current_device_type));
	sprintf(DeviceCode, "%03x", Api::current_device_id);
	memcpy(tempTime, CmdSort::m_time_now, 7);
	sprintf(time, "%02x%02x%02x%02x%02x%02x%02x", tempTime[0], tempTime[1], tempTime[2], tempTime[3], tempTime[4], tempTime[5], tempTime[6]);

	g_Parameter.get_validexittime(Api::current_station_id,tempTime, validexitTime, &penalty_val);

	dbg_dumpmemory("validexitTime:",validexitTime,7);

	sprintf(exittime, "%02x%02x%02x%02x%02x%02x%02x", validexitTime[0], validexitTime[1], validexitTime[2], validexitTime[3], validexitTime[4], validexitTime[5], validexitTime[6]);

	memcpy(m_ticket_data.qr_ticketdata.validOutTime, exittime, 14);

	switch (operType)
	{
	case operEntry:
		{
			memcpy(dealData,"01",2);
			length += 2;
			//dbg_dumpmemory("dealData1:",dealData,length);
			memcpy(dealData + length, time, 14);
			length += 14;
			memcpy(dealData + length, exittime, 14);
			length += 14;
			dbg_dumpmemory("xxxxxxxxxxxxxxxxxxxxx:",dealData,length);
			memcpy(dealData + length, StationCode, 2);//线路编码
			length += 2;
			//dbg_dumpmemory("dealData3:",dealData,length);
			memcpy(dealData + length, LineCode, 2);//站点编码
			length += 2;
			//dbg_dumpmemory("dealData4:",dealData,length);
			memcpy(dealData + length, DeviceType, 2);//设备类型
			length += 2;
			//dbg_dumpmemory("dealData5:",dealData,length);
			memcpy(dealData + length, DeviceCode, 3);//设备编号
			length += 3;
			//dbg_dumpmemory("dealData6:",dealData,length);
			//memcpy(dealData + length, g_saminf[0].sam_terminal_id, 12);
			//memcpy(dealData + length, "112233445566", 12);//暂时不写
			memcpy(dealData + length, m_p_current_sam + 4, 12);
			length += 12;
			//dbg_dumpmemory("dealData7:",dealData,length);
			memcpy(dealData + length, "00000000", 8);//暂时不写
			length += 8;
			//dbg_dumpmemory("dealData8:",dealData,length);
			memcpy(dealData + length, "00", 2);
			length += 2;
			//dbg_dumpmemory("dealData9:",dealData,length);
			memcpy(dealData + length, "00000000", 8);
			length += 8;
			//dbg_dumpmemory("dealData10:",dealData,length);
			len_data = length;

		}
		break;
	case operExit:
		{
			memcpy(dealData,"02",2);
			length += 2;
			//dbg_dumpmemory("dealData1:",dealData,length);
			memcpy(dealData + length, time, 14);
			length += 14;
			//dbg_dumpmemory("dealData2:",dealData,length);
			memcpy(dealData + length, StationCode, 2);//线路编码
			length += 2;
			//dbg_dumpmemory("dealData3:",dealData,length);
			memcpy(dealData + length, LineCode, 2);//站点编码
			length += 2;
			//dbg_dumpmemory("dealData4:",dealData,length);
			memcpy(dealData + length, DeviceType, 2);//设备类型
			length += 2;
			//dbg_dumpmemory("dealData5:",dealData,length);
			memcpy(dealData + length, DeviceCode, 3);//设备编号
			length += 3;
			//dbg_dumpmemory("dealData6:",dealData,length);
			//memcpy(dealData + length, "112233445566", 12);
			memcpy(dealData + length, m_p_current_sam + 4, 12);
			length += 12;
			//dbg_dumpmemory("dealData7:",dealData,length);
			memcpy(dealData + length, "20180702", 8);
			length += 8;
			sprintf(dealData + length,"%08X",amount);
			//dbg_formatvar("price:%08X",amount);
			//memcpy(dealData + length, "000001F4", 8);
			length += 8;

			//dbg_dumpmemory("dealData8:",dealData,length);
			memcpy(dealData + length, "00", 2);
			length += 2;
			//dbg_dumpmemory("dealData9:",dealData,length);
			memcpy(dealData + length, "00000000", 8);
			length += 8;
			//dbg_dumpmemory("dealData10:",dealData,length);
			len_data = length;

		}
		break;
	case operGetETicket:
		{
			memcpy(dealData,"05",2);
			length += 2;
			//dbg_dumpmemory("dealData1:",dealData,length);
			memcpy(dealData + length, time, 14);
			length += 14;
			//dbg_dumpmemory("dealData2:",dealData,length);
			memcpy(dealData + length, StationCode, 2);//线路编码
			length += 2;
			//dbg_dumpmemory("dealData3:",dealData,length);
			memcpy(dealData + length, LineCode, 2);//站点编码
			length += 2;
			//dbg_dumpmemory("dealData4:",dealData,length);
			memcpy(dealData + length, DeviceType, 2);//设备类型
			length += 2;
			//dbg_dumpmemory("dealData5:",dealData,length);
			memcpy(dealData + length, DeviceCode, 3);//设备编号
			length += 3;
			//dbg_dumpmemory("dealData6:",dealData,length);
			//memcpy(dealData + length, "112233445566", 12);
			memcpy(dealData + length, m_p_current_sam + 4, 12);
			length += 12;
			//dbg_dumpmemory("dealData7:",dealData,length);
			memcpy(dealData + length, "00000000", 8);
			length += 8;
			//dbg_dumpmemory("dealData8:",dealData,length);
			//memcpy(dealData + length, "5510000111223344", 16);
			//length += 16;
			//dbg_dumpmemory("dealData8:",dealData,length);
			memcpy(dealData + length, "00", 2);
			length += 2;
			//dbg_dumpmemory("dealData9:",dealData,length);
			memcpy(dealData + length, "00000000", 8);
			length += 8;
			//dbg_dumpmemory("dealData10:",dealData,length);
			len_data = length;
		}
		break;
	default:
		break;
	}
	memcpy(dataBack, (uint8_t * )dealData, length);

}


void TicketBase::getDealDataUpload(ETYTKOPER operType,uint16_t& len_data,uint8_t * dataBack,char * succMount, char * failMount/*, char * date*/)
{
	int length = 0;
	char StationCode[2+1] = {0};
	char LineCode[2+1] = {0};
	char DeviceType[2+1] = {0};
	char DeviceCode[3+1] = {0};
	char PsamCode[12+1] = {0};
	char time[14+1] = {0};

	char dealData[256] = {0};

	sprintf(StationCode, "%02x", ((Api::current_station_id >>8) & 0x00FF));
	sprintf(LineCode, "%02x", (Api::current_station_id & 0xFF));
	sprintf(DeviceType, "%02x", (uint8_t)(Api::current_device_type));
	sprintf(DeviceCode, "%03x", Api::current_device_id);
	sprintf(time, "%02x%02x%02x%02x%02x%02x%02x", CmdSort::m_time_now[0], CmdSort::m_time_now[1], CmdSort::m_time_now[2], CmdSort::m_time_now[3], CmdSort::m_time_now[4], CmdSort::m_time_now[5], CmdSort::m_time_now[6]);

	switch (operType)
	{
	case operUnloadTicketInfo:
		{
			memcpy(dealData,"03",2);
			length += 2;
			//dbg_dumpmemory("dealData1:",dealData,length);
			memcpy(dealData + length, time, 14);
			length += 14;
			//dbg_dumpmemory("dealData2:",dealData,length);
			memcpy(dealData + length, StationCode, 2);//线路编码
			length += 2;
			//dbg_dumpmemory("dealData3:",dealData,length);
			memcpy(dealData + length, LineCode, 2);//站点编码
			length += 2;
			//dbg_dumpmemory("dealData4:",dealData,length);
			memcpy(dealData + length, DeviceType, 2);//设备类型
			length += 2;
			//dbg_dumpmemory("dealData5:",dealData,length);
			memcpy(dealData + length, DeviceCode, 3);//设备编号
			length += 3;
			//dbg_dumpmemory("dealData6:",dealData,length);
			//memcpy(dealData + length, "112233445566", 12);
			memcpy(dealData + length, "000000000000", 12);
			length += 12;
			//dbg_dumpmemory("dealData7:",dealData,length);
			memcpy(dealData + length, "00000000", 8);
			length += 8;
			if(memcmp(failMount, "00", 2) == 0)
				memcpy(dealData + length, "00", 2);
			else
				memcpy(dealData + length, "01", 2);
			length += 2;
			memcpy(dealData + length, succMount, 2);
			length += 2;
			memcpy(dealData + length, failMount, 2);
			length += 2;
			//dbg_dumpmemory("dealData9:",dealData,length);
			memcpy(dealData + length, "00000000", 8);
			length += 8;
			dbg_dumpmemory("dealData10:",dealData,length);
			len_data = length;
		}
		break;
	default:
		break;
	}
	memcpy(dataBack, (uint8_t * )dealData, length);

	//g_Record.log_buffer_debug("dataBack= ", dataBack,length);

	//dbg_dumpmemory("dataBack:",dataBack,length);

}



void TicketBase::transfer_jtb_data_for_out(ETYTKOPER operType, void * pTradeStruct)
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
			//pAnalyze->bActiveStatus = 0;
			pAnalyze->bActiveStatus = m_ticket_data.app_lock_flag;

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
			//dbg_formatvar("cTradeType = %s",pPurse->cTradeType);
			Api::get_current_device_info(pPurse->cStationID, pPurse->cDevType, pPurse->cDeviceID);
			memcpy(pPurse->cSAMID, m_p_current_sam, 16);
			//dbg_formatvar("cSAMID = %s",pPurse->cSAMID);
			memcpy(pPurse->dtDate, CmdSort::m_time_now, 7);
			memcpy(pPurse->bTicketType, m_ticket_data.logical_type, 2);
			//dbg_dumpmemory("bTicketType = ",pPurse->bTicketType,2);
			memcpy(pPurse->cLogicalID, m_ticket_data.logical_id, 20);
			//dbg_formatvar("cLogicalID = %s",pPurse->cLogicalID);
			memcpy(pPurse->cPhysicalID, m_ticket_data.physical_id, 20);
			//dbg_formatvar("cPhysicalID = %s",pPurse->cPhysicalID);
			pPurse->lBalance = m_ticket_data.wallet_value;
			//dbg_formatvar("lBalance = %ld",pPurse->lBalance);
			pPurse->lChargeCount = m_ticket_data.charge_counter;
			//dbg_formatvar("lChargeCount = %ld",pPurse->lChargeCount);
			pPurse->lTradeCount = m_ticket_data.total_trade_counter;
			//dbg_formatvar("lTradeCount = %ld",pPurse->lTradeCount);
			memset(pPurse->cReceiptID, '0', sizeof(pPurse->cReceiptID));
			pPurse->bEntryStationID[0] = (uint8_t)((m_ticket_data.p_jtb_entry_info->station_id >> 8) & 0xFF);
			pPurse->bEntryStationID[1] = (uint8_t)(m_ticket_data.p_jtb_entry_info->station_id & 0xFF);
			//dbg_dumpmemory("bEntryStationID = ",pPurse->bEntryStationID, 2);
			memcpy(pPurse->cEntrySAMID, m_ticket_data.p_jtb_entry_info->sam, 16);
			//dbg_formatvar("cEntrySAMID = %s",pPurse->cEntrySAMID);
			memcpy(pPurse->dtEntryDate, m_ticket_data.p_jtb_entry_info->time, 7);
			//dbg_dumpmemory("dtEntryDate = ",pPurse->dtEntryDate, 7);
			memcpy(pPurse->cOperatorID, Api::operator_id, 6);
			//dbg_formatvar("cOperatorID = %s",pPurse->cOperatorID);
			pPurse->bBOMShiftID	= Api::bom_shift_id;
			memcpy(pPurse->cSamLast, m_ticket_data.read_last_info.sam, 16);
			//dbg_formatvar("cSamLast = %s",pPurse->cSamLast);
			memcpy(pPurse->dtLast, m_ticket_data.read_last_info.time, 7);
			//dbg_dumpmemory("dtLast = ",pPurse->dtLast, 7);
			pPurse->cTestFlag = m_ticket_data.key_flag + 0x30;

			sprintf(szTemp, "%03d", m_ticket_data.limit_mode);
			memcpy(pPurse->cLimitMode, szTemp, sizeof(pPurse->cLimitMode));

			memcpy(pPurse->bLimitEntryID, m_ticket_data.limit_entry_station, 2);
			//dbg_dumpmemory("bLimitEntryID = ",pPurse->bLimitEntryID, 2);
			memcpy(pPurse->bLimitExitID, m_ticket_data.limit_exit_station, 2);
			//dbg_formatvar("bLimitExitID = ",pPurse->bLimitExitID, 2);

			if (pPurse->cExitMode == 0)
				pPurse->cExitMode = '0';

			memcpy(pPurse->cCityCode, m_ticket_data.city_code, sizeof(pPurse->cCityCode));					// 城市代码
			//dbg_formatvar("cCityCode = %s",pPurse->cCityCode);
			memcpy(pPurse->cIndustryCode, m_ticket_data.industry_code, sizeof(pPurse->cIndustryCode));		// 行业代码
			//dbg_formatvar("cIndustryCode = %s",pPurse->cIndustryCode);
			memcpy(pPurse->cSamPosId, m_p_sam_posid, sizeof(pPurse->cSamPosId));							// SAM卡终端编码
			//dbg_formatvar("cSamPosId = %s",pPurse->cSamPosId);

			pPurse->cTkAppMode = m_ticket_data.ticket_app_mode;

			memcpy(pPurse->cRecordVer, "10", 2);
			memset(pPurse->cIssueCardCode,0x20,16);
			memcpy(pPurse->cIssueCardCode, m_ticket_data.issue_code, 4);
			memcpy(&pPurse->cIssueCardCode[4], m_ticket_data.city_code, 4);
			//memcpy(&pPurse->cIssueCardCode[8], "FFFFFFFF", 8);
			dbg_formatvar("cIssueCardCode = %s",pPurse->cIssueCardCode);

			memcpy(pPurse->cKeyVer, "00", 2); // 密钥版本号
			memcpy(pPurse->cKeyIndex, "00", 2);// 密钥索引
			memcpy(pPurse->cRandom, "00000000", 8);// 伪随机数
			memcpy(pPurse->cAlgFlag, "00", 2); // 算法标识
			memset(pPurse->cCardHolderName,0x20,40);
			memcpy(pPurse->cCardHolderName, m_ticket_data.certificate_name, 20);// 持卡人姓名
			//memcpy(&pPurse->cCardHolderName[20], "                    ", 20);// 持卡人姓名
			sprintf(pPurse->cCertificateType, "%02X", m_ticket_data.certificate_type);// 证件类型
			//memset(pPurse->cCertificateCode,0x20,30);
			memcpy(pPurse->cCertificateCode, m_ticket_data.certificate_code, 30);// 证件号码
			//memset(pPurse->cOriginalCardType,0x20,8);
			sprintf(pPurse->cOriginalCardType, "%02X%-6C", m_ticket_data.jtbCardType,0x20);// 原来票卡类型
			memcpy(pPurse->cCardLocation, "0000", 4);// 卡属地

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
			//pAnalyze->bActiveStatus = 0;
			pAnalyze->bActiveStatus = m_ticket_data.app_lock_flag;

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

			memcpy(pPurse->cRecordVer, "10", 2); // 记录版本，暂时默认为10
			memcpy(pPurse->cIssueCardCode, "0000000000000000", 16); // 发卡机构代码
			memcpy(pPurse->cKeyVer, "00", 2); // 密钥版本号
			memcpy(pPurse->cKeyIndex, "00", 2);// 密钥索引
			memcpy(pPurse->cRandom, "00000000", 8);// 伪随机数
			memcpy(pPurse->cAlgFlag, "00", 2); // 算法标识
			memcpy(pPurse->cCardHolderName, "0000000000000000000000000000000000000000", 40);// 持卡人姓名
			memcpy(pPurse->cCertificateType, "00", 2);// 证件类型
			memcpy(pPurse->cCertificateCode, "000000000000000000000000000000", 30);// 证件号码
			//memset(pPurse->cOriginalCardType,0x20,8);
			memcpy(pPurse->cOriginalCardType, "00000000", 8);// 原来票卡类型
			memcpy(pPurse->cCardLocation, "0000", 4);// 卡属地

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

RETINFO TicketBase::esale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, PETICKETDEALINFO p_eticketdealinfo, uint16_t& len_eticket_sale, TICKET_PARAM& ticket_prm)
{
	RETINFO ret			= {0};
	ret.wErrCode = ERR_CALL_INVALID;
	return ret;
}

RETINFO TicketBase::thirdesale(PSJTSALE p_sjt_sale, uint16_t& len_sjt, PETICKETDEALINFO p_eticketdealinfo, uint16_t& len_eticket_sale, TICKET_PARAM& ticket_prm,char * cNetTranNumber)
{
	RETINFO ret			= {0};
	ret.wErrCode = ERR_CALL_INVALID;
	return ret;
}


RETINFO TicketBase::getETicket(PGETETICKETRESPINFO p_geteticketrespinfo)
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

RETINFO TicketBase::analyse_work_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PPURSETRADE p_purse, PBOMETICANALYZE p_eticanalyse)
{
    RETINFO ret					= {0};
	uint16_t result_prm			= 0;
	uint16_t err				= 0;
	uint16_t err1				= 0;
    long trade_val				= 0;
	long init_val				= 0;	// 原票价
	long penalty_val			= 0;
	long penalty_val1			= 0;
    uint32_t operation_status	= 0;
	char exit_mode				= 0;
	uint16_t tempEntryID		= 0;
	uint8_t	cert_peroidE[7];		// 证件有效期

	char temp = 0;

    do
    {
		// 如果发生列车故障，直接返回正常
		if (p_degrade_falgs->train_trouble)
		{
			exit_mode = (char)(degrade_Trouble + 0x30);
			break;
		}

		g_Record.log_out(0, level_disaster,"analyse_work_zone:[check_logic_expire = %d] [logical_type = %02x%02x] [fare_type = %02x%02x]",p_ticket_prm->check_logic_expire,m_ticket_data.logical_type[0],m_ticket_data.logical_type[1],m_ticket_data.fare_type[0],m_ticket_data.fare_type[1]);
		g_Record.log_out(0, level_disaster,"analyse_work_zone:[logical_peroidS = %02x%02x%02x%02x%02x%02x%02x]",m_ticket_data.logical_peroidS[0],m_ticket_data.logical_peroidS[1],m_ticket_data.logical_peroidS[2],m_ticket_data.logical_peroidS[3],m_ticket_data.logical_peroidS[4],m_ticket_data.logical_peroidS[5],m_ticket_data.logical_peroidS[6]);
		g_Record.log_out(0, level_disaster,"analyse_work_zone:[logical_peroidE = %02x%02x%02x%02x%02x%02x%02x]",m_ticket_data.logical_peroidE[0],m_ticket_data.logical_peroidE[1],m_ticket_data.logical_peroidE[2],m_ticket_data.logical_peroidE[3],m_ticket_data.logical_peroidE[4],m_ticket_data.logical_peroidE[5],m_ticket_data.logical_peroidE[6]);
		g_Record.log_out(0, level_disaster,"analyse_work_zone:[m_time_now = %02x%02x%02x%02x%02x%02x%02x]",CmdSort::m_time_now[0],CmdSort::m_time_now[1],CmdSort::m_time_now[2],CmdSort::m_time_now[3],CmdSort::m_time_now[4],CmdSort::m_time_now[5],CmdSort::m_time_now[6]);
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
					g_Record.log_out(0, level_disaster,"analyse_work_zone:ERR_OVER_PERIOD_L1");
					ret.wErrCode = ERR_OVER_PERIOD_L;
					break;
				}
			}

			if ((m_ticket_data.logical_type[0] == 0x02)&&(m_ticket_data.logical_type[1] == 0x01)&& (m_ticket_data.certificate_type == 0x01))
			{
				if((memcmp(m_ticket_data.certificate_code + 6, "19", 2) == 0) || (memcmp(m_ticket_data.certificate_code + 6, "20", 2) == 0))
				{
					if (memcmp(CmdSort::m_time_now, m_ticket_data.logical_peroidE, 7) >= 0 )
					{
						memcpy(cert_peroidE, "\x00\x00\x00\x00\x00\x00\x00", 7);
						Publics::string_to_bcds(m_ticket_data.certificate_code + 6, 8, cert_peroidE, 4);
						g_Record.log_buffer("cert_peroidE_before:", cert_peroidE, 7);
						TimesEx::bcd_time_calculate(cert_peroidE, T_DATE, 18 * 365 + 4);
						g_Record.log_buffer("cert_peroidE_after:", cert_peroidE, 7);
						memcpy(m_ticket_data.logical_peroidE, cert_peroidE, 7);
						g_Record.log_buffer("m_ticket_data.logical_peroidE:", m_ticket_data.logical_peroidE, 7);
					}
				}
			}

			if (memcmp(CmdSort::m_time_now, m_ticket_data.logical_peroidE, 7) > 0)
			{
				g_Record.log_out(0, level_disaster,"uncheck_datetime = %d",p_degrade_falgs->uncheck_datetime);
				if (p_degrade_falgs->uncheck_datetime)
				{
					exit_mode = (char)(degrade_Uncheck_DateTime + 0x30);
				}
				else
				{
					if (p_ticket_prm->permit_deffer)
						operation_status |= ALLOW_DIFFER;
					g_Record.log_out(0, level_disaster,"analyse_work_zone:ERR_OVER_PERIOD_L2");
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
			//dbg_formatvar("station_id |= %d",m_ticket_data.p_update_info->station_id);

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
				//dbg_formatvar("uncheck_datetime");
                if (g_Parameter.exit_timeout(m_ticket_data.p_exit_info->station_id,
                                             m_ticket_data.p_entry_info->time, CmdSort::m_time_now, &penalty_val))
                {
					//dbg_formatvar("check_exit_timeout");
                    result_prm = g_Parameter.query_fare(CmdSort::m_time_now, m_ticket_data.fare_type,
                                   m_ticket_data.p_entry_info->station_id, m_ticket_data.p_entry_info->time, &trade_val, &init_val);

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
			//dbg_formatvar("check_balance");

			// 不检查余额和票价时取本站最低票价
			//tempEntryID = Api::current_station_id;
			//result_prm = g_Parameter.query_lowest_fare(CmdSort::m_time_now, m_ticket_data.fare_type, tempEntryID, trade_val);

			//g_Record.log_out(0, level_disaster, "1 analyse_work_zone:query_lowest_fare");
			temp = (char)((m_ticket_data.p_entry_info->station_id >> 8) & 0xFF);
			if(temp == 0x60)
				result_prm = g_Parameter.query_cf_lowest_fare(CmdSort::m_time_now, m_ticket_data.fare_type,m_ticket_data.p_entry_info->station_id,trade_val, &init_val);
            else
				result_prm = g_Parameter.query_lowest_fare(CmdSort::m_time_now, m_ticket_data.fare_type,trade_val, &init_val);

			if (p_degrade_falgs->uncheck_fare)
			{
				exit_mode = (char)(degrade_Uncheck_Fare + 0x30);
			}
        }
        else
        {
			//dbg_formatvar("else");

            result_prm = g_Parameter.query_fare(CmdSort::m_time_now, m_ticket_data.fare_type,
				m_ticket_data.p_entry_info->station_id, m_ticket_data.p_entry_info->time, &trade_val, &init_val);
        }
		if (result_prm)
		{
			ret.wErrCode = result_prm;
			break;
		}

		//dbg_formatvar("m_ticket_data.simple_status:%d",m_ticket_data.simple_status);
		//dbg_formatvar("m_ticket_data.init_status:%d",m_ticket_data.init_status);
		//dbg_formatvar("trade_val:%ld",trade_val);

        // 检查超乘
        if (m_ticket_data.simple_status == SStatus_Upd_WZone && m_ticket_data.init_status != MS_Upd_Wzone_Time)
		{
			// 余额足够收取票价，否则将余额减为0
			if (m_ticket_data.wallet_value < trade_val/* - ticket_prm.value_over_draft*/)
			{
				//dbg_formatvar("AAAAA");
				trade_val = m_ticket_data.wallet_value/10*10;//超乘，直接舍掉分
			}
		}
		else
        {
			if (!p_ticket_prm->check_over_travel)
			{
				if (m_ticket_data.wallet_value < trade_val)
				{
					//dbg_formatvar("CCCCC");
					trade_val = m_ticket_data.wallet_value/10*10;
				}
			}
            else if (p_ticket_prm->permit_over_draft)
            {
                if (m_ticket_data.wallet_value < trade_val/* - ticket_prm.value_over_draft*/)
                {
					//dbg_formatvar("DDDDD");
                    //penalty_val1 = ((trade_val - m_ticket_data.wallet_value)/10)*10;/* - ticket_prm.value_over_draft*/;
					penalty_val1 = (trade_val - ((m_ticket_data.wallet_value/10)*10));/* - ticket_prm.value_over_draft*/;

                    ret.wErrCode = err1 = ERR_OVER_MILEAGE;
                    operation_status |= ALLOW_UPDATE;

                    break;
                }
            }
            else if (m_ticket_data.wallet_value < trade_val)
            {
				//dbg_formatvar("EEEEE");
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
		if(m_ticket_data.logical_type[0] == 0x03)
			p_analyse->lPenalty = trade_val;
		else
			p_analyse->lPenalty = penalty_val;
		p_analyse->wError = err;
		p_analyse->lPenalty1 = penalty_val1;
		p_analyse->wError1 = err1;
	}
	if (p_purse != NULL)
	{
		p_purse->lTradeAmount = trade_val;
		p_purse->lTradeWallet = init_val;
		p_purse->cExitMode = exit_mode;
	}

	if (err == ERR_EXIT_TIMEOUT && err1 == ERR_OVER_MILEAGE)
		ret.wErrCode = ERR_OVER_TIME_MILEAGE;

    return ret;
}

RETINFO TicketBase::analyse_free_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PBOMETICANALYZE p_eticanalyse)
{
    RETINFO ret					= {0};
	uint16_t err				= 0;
	uint16_t err1				= 0;
	long penalty_val			= 0;
	long penalty_val1			= 0;
	long lowest_val				= 0;
    uint32_t operation_status	= 0;
	char entry_mode				= 0;
	uint16_t tempEntryID		= 0;
	uint8_t	cert_peroidE[7];		// 证件有效期

	char temp = 0;

	//dbg_formatvar("analyse_free_zone-in");

    do
    {

		//dbg_formatvar("11111");
				g_Record.log_out(0, level_disaster,"analyse_free_zone:[check_logic_expire = %d] [logical_type = %02x%02x] [fare_type = %02x%02x]",p_ticket_prm->check_logic_expire,m_ticket_data.logical_type[0],m_ticket_data.logical_type[1],m_ticket_data.fare_type[0],m_ticket_data.fare_type[1]);
		g_Record.log_out(0, level_disaster,"analyse_free_zone:[logical_peroidS = %02x%02x%02x%02x%02x%02x%02x]",m_ticket_data.logical_peroidS[0],m_ticket_data.logical_peroidS[1],m_ticket_data.logical_peroidS[2],m_ticket_data.logical_peroidS[3],m_ticket_data.logical_peroidS[4],m_ticket_data.logical_peroidS[5],m_ticket_data.logical_peroidS[6]);
		g_Record.log_out(0, level_disaster,"analyse_free_zone:[logical_peroidE = %02x%02x%02x%02x%02x%02x%02x]",m_ticket_data.logical_peroidE[0],m_ticket_data.logical_peroidE[1],m_ticket_data.logical_peroidE[2],m_ticket_data.logical_peroidE[3],m_ticket_data.logical_peroidE[4],m_ticket_data.logical_peroidE[5],m_ticket_data.logical_peroidE[6]);
		g_Record.log_out(0, level_disaster,"analyse_free_zone:[m_time_now = %02x%02x%02x%02x%02x%02x%02x]",CmdSort::m_time_now[0],CmdSort::m_time_now[1],CmdSort::m_time_now[2],CmdSort::m_time_now[3],CmdSort::m_time_now[4],CmdSort::m_time_now[5],CmdSort::m_time_now[6]);
		// 当票价类型和实际类型不符时，说明已经转入普通卡，不再检查逻辑有效期
		if (p_ticket_prm->check_logic_expire && memcmp(m_ticket_data.logical_type, m_ticket_data.fare_type, 2) == 0)
		{
			//dbg_formatvar("22222");

			// 逻辑有效期
			if (memcmp(CmdSort::m_time_now, m_ticket_data.logical_peroidS, 7) < 0)
			{
				//dbg_formatvar("33333");

				if (p_degrade_falgs->uncheck_datetime)
				{
					//dbg_formatvar("44444");

					entry_mode = (char)(0x30 + degrade_Uncheck_DateTime);
				}
				else if (p_degrade_falgs->sell_sjt_by_emergency || p_degrade_falgs->es_sjt_by_emergency || p_degrade_falgs->entry_by_emergency ||
					p_degrade_falgs->update_free_by_emergency || p_degrade_falgs->update_work_by_emergency)
				{
					//dbg_formatvar("55555");

					entry_mode = (char)(0x30 + degrade_Green_Light);
				}
				else if (p_degrade_falgs->es_sjt_by_train_trouble || p_degrade_falgs->exit_train_trouble_effect || p_degrade_falgs->sell_sjt_by_train_trouble)
				{
					//dbg_formatvar("66666");
					entry_mode = (char)(0x30 + degrade_Trouble);
				}
				else
				{
					if ((m_ticket_data.logical_type[0] == 0x03)&&(m_ticket_data.app_lock_flag == 0x00))
					{

					}
					else
					{
						g_Record.log_out(0, level_disaster,"analyse_free_zone:ERR_OVER_PERIOD_L3");
						ret.wErrCode = ERR_OVER_PERIOD_L;
						break;
					}

				}
			}

			if ((m_ticket_data.logical_type[0] == 0x02)&&(m_ticket_data.logical_type[1] == 0x01)&& (m_ticket_data.certificate_type == 0x01))
			{
				if((memcmp(m_ticket_data.certificate_code + 6, "19", 2) == 0) || (memcmp(m_ticket_data.certificate_code + 6, "20", 2) == 0))
				{
					if (memcmp(CmdSort::m_time_now, m_ticket_data.logical_peroidE, 7) >= 0 )
					{
						memcpy(cert_peroidE, "\x00\x00\x00\x00\x00\x00\x00", 7);
						Publics::string_to_bcds(m_ticket_data.certificate_code + 6, 8, cert_peroidE, 4);
						g_Record.log_buffer("cert_peroidE_before:", cert_peroidE, 7);
						TimesEx::bcd_time_calculate(cert_peroidE, T_DATE, 18 * 365 + 4);
						g_Record.log_buffer("cert_peroidE_after:", cert_peroidE, 7);
						memcpy(m_ticket_data.logical_peroidE, cert_peroidE, 7);
						g_Record.log_buffer("m_ticket_data.logical_peroidE:", m_ticket_data.logical_peroidE, 7);
					}
				}
			}

			if (memcmp(CmdSort::m_time_now, m_ticket_data.logical_peroidE, 7) > 0)
			{
				//dbg_formatvar("88888");
				if (p_degrade_falgs->uncheck_datetime)
				{
					//dbg_formatvar("99999");
					entry_mode = (char)(0x30 + degrade_Uncheck_DateTime);
				}
				else if (p_degrade_falgs->sell_sjt_by_emergency || p_degrade_falgs->es_sjt_by_emergency || p_degrade_falgs->entry_by_emergency ||
					p_degrade_falgs->update_free_by_emergency || p_degrade_falgs->update_work_by_emergency)
				{
					//dbg_formatvar("AAAAA");
					entry_mode = (char)(0x30 + degrade_Green_Light);
				}
				else if (p_degrade_falgs->es_sjt_by_train_trouble || p_degrade_falgs->exit_train_trouble_effect || p_degrade_falgs->sell_sjt_by_train_trouble)
				{
					//dbg_formatvar("BBBBB");
					entry_mode = (char)(0x30 + degrade_Trouble);
				}
				else
				{
					//dbg_formatvar("ccccc");
					if (p_ticket_prm->permit_deffer)
						operation_status |= ALLOW_DIFFER;

					if ((m_ticket_data.logical_type[0] == 0x03)&&(m_ticket_data.app_lock_flag == 0x00))
					{
					}
					else
					{
						g_Record.log_out(0, level_disaster,"analyse_free_zone:ERR_OVER_PERIOD_L4");
						ret.wErrCode = ERR_OVER_PERIOD_L;
						break;
					}

					//ret.wErrCode = ERR_OVER_PERIOD_L;
					//break;
				}
			}
		}

		temp = (char)((m_ticket_data.p_entry_info->station_id >> 8) & 0xFF);
		if(temp == 0x60)
			ret.wErrCode = g_Parameter.query_cf_lowest_fare(CmdSort::m_time_now, m_ticket_data.fare_type,m_ticket_data.p_entry_info->station_id,lowest_val);
        else
			ret.wErrCode = g_Parameter.query_lowest_fare(CmdSort::m_time_now, m_ticket_data.fare_type,lowest_val);
		if (ret.wErrCode)	break;

        if (m_ticket_data.simple_status == SStatus_Entry)
        {
			//dbg_formatvar("ddddd");
			//tempEntryID = m_ticket_data.p_entry_info->station_id;
			//ret.wErrCode = g_Parameter.query_lowest_fare(CmdSort::m_time_now, m_ticket_data.fare_type, tempEntryID, lowest_val);
			//if (ret.wErrCode)	break;
			//dbg_formatvar("eeeee");
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
			//dbg_formatvar("fffff");
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
		//dbg_formatvar("ggggg");
        if (m_ticket_data.simple_status == SStatus_Upd_WZone)
        {
			//tempEntryID = m_ticket_data.p_entry_info->station_id;
			//ret.wErrCode = g_Parameter.query_lowest_fare(CmdSort::m_time_now, m_ticket_data.fare_type, tempEntryID, lowest_val);
			//if (ret.wErrCode)	break;

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
		//dbg_formatvar("hhhhh");
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
		//dbg_formatvar("iiiii");
        // 检查最低票价，此时最低票价的进站站点未行程，用本站站点替代
		//tempEntryID = Api::current_station_id;
		//ret.wErrCode = g_Parameter.query_lowest_fare(CmdSort::m_time_now, m_ticket_data.fare_type, tempEntryID, lowest_val);
		//if (ret.wErrCode)	break;
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

void TicketBase::save_last_stack_eticket(ETYTKOPER operType, void * p_trade, size_t size_trade, void * p_written_inf, size_t size_written, bool save_to_file)
{
	//PETICKETDEALINFO p_eticketdealinfo = p_trade;
	cfm_point_e.operType = operType;
	memcpy(&cfm_point_e.eticketdealinfo, p_trade, size_trade);
	memcpy(cfm_point_e.cCenterCode, cfm_point_e.eticketdealinfo.cCenterCode, 32);
	//memcpy(cfm_point_e.cTxnTms, cfm_point_e.eticketdealinfo.cTxnTms, 14);
	memcpy(cfm_point_e.cTxnTms, p_written_inf, 14);
	memcpy(cfm_point_e.cTxnAmount, cfm_point_e.eticketdealinfo.cTxnAmount, 6);
	g_Record.log_out(0, level_disaster,"*******************save last stack eticket*******************");
	//g_Record.log_buffer("cfm_point_e.eticketdealinfo() = ", cfm_point_e.eticketdealinfo, size_trade);
	g_Record.log_out(0, level_disaster,"cfm_point_e.cCenterCode = %s | cTxnTms = %s| cTxnAmount = %s",cfm_point_e.cCenterCode,cfm_point_e.cTxnTms,cfm_point_e.cTxnAmount);
	//g_Record.log_out(0, level_disaster,"cfm_point_e.cTxnTms = %s",cfm_point_e.cTxnTms);
	//g_Record.log_out(0, level_disaster,"cfm_point_e.cTxnAmount = %s",cfm_point_e.cTxnAmount);

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

	//dbg_formatvar("get_confirm_factor(trade_type=%02X)", trade_type);
	//dbg_formatvar("(trade_type=%02X)", ret.tac_type);
	//dbg_formatvar("logic_id=%s", ret.logic_id);
	//dbg_formatvar("trade_amount=%ld", ret.trade_amount);
	//dbg_dumpmemory("trade_time=",ret.trade_time,7);
	//dbg_dumpmemory("device_inf=",ret.device_inf,9);

	//g_Record.log_out(0, level_error, "get_confirm_factor(trade_type=%02X)", trade_type);
	//g_Record.log_out(0, level_error, "(trade_type=%02X)", ret.tac_type);
	//g_Record.log_out(0, level_error, "logic_id=%s", ret.logic_id);
	//g_Record.log_out(0, level_error, "trade_time=%02X%02X%02X%02X%02X%02X%02X", ret.trade_time[0], ret.trade_time[1], ret.trade_time[2], ret.trade_time[3], ret.trade_time[4], ret.trade_time[5], ret.trade_time[6]);
	//g_Record.log_out(0, level_error, "trade_amount=%ld", ret.trade_amount);
	//g_Record.log_out(0, level_error, "device_inf=%02X%02X%02X%02X%02X%02X%02X%02X%02X", ret.device_inf[0], ret.device_inf[1], ret.device_inf[2], ret.device_inf[3], ret.device_inf[4], ret.device_inf[5], ret.device_inf[6], ret.device_inf[7], ret.device_inf[8]);

	return ret;
}

CONFIRM_POINT_E TicketBase::get_confirm_factor_e()
{
	CONFIRM_POINT_E ret;

	memset(&ret, 0, sizeof(ret));

	memcpy(ret.cCenterCode, cfm_point_e.cCenterCode, 32);
	memcpy(ret.cTxnAmount, cfm_point_e.cTxnAmount, 6);
	memcpy(ret.cTxnTms, cfm_point_e.cTxnTms, 14);
	ret.operType = cfm_point_e.operType;
	//memcpy(ret.cCenterCode, cfm_point_e.cCenterCode, 32);

	g_Record.log_out(0, level_disaster, "***************get_confirm_factor_e*************");
	g_Record.log_out(0, level_error, "(cCenterCode=%s) | (cTxnAmount=%s) |(cTxnTms = %s) |(operType=%d)", ret.cCenterCode, ret.cTxnAmount, ret.cTxnTms, ret.operType);
	//g_Record.log_out(0, level_error, "(cTxnAmount=%s)", ret.cTxnAmount);
	//g_Record.log_out(0, level_error, "(cTxnTms = %s)", ret.cTxnTms);
	//g_Record.log_out(0, level_error, "(operType=%d)", ret.operType);
	//g_Record.log_out(0, level_error, "device_inf=%02X%02X%02X%02X%02X%02X%02X%02X%02X", ret.device_inf[0], ret.device_inf[1], ret.device_inf[2], ret.device_inf[3], ret.device_inf[4], ret.device_inf[5], ret.device_inf[6], ret.device_inf[7], ret.device_inf[8]);

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
		//dbg_dumpmemory("cfm_point.cMACorTAC",cfm_point.trade.purse.cMACorTAC,10);
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
		//dbg_formatvar("factor.logic_id:%s  p_logic_id =%s",factor.logic_id,p_logic_id);
		memset(&cfm_point, 0, sizeof(cfm_point));

		if (save_to_file)
		{
			//dbg_formatvar("save_confirm_to_file");
			save_confirm_to_file();
		}
	}
}

void TicketBase::clear_confirm_e(char * cCenterCode, bool check_cCenterCode, bool save_to_file)
{
	CONFIRM_POINT_E factor_e = get_confirm_factor_e();
	if ((check_cCenterCode && memcmp(cCenterCode, factor_e.cCenterCode, 32) == 0) || !check_cCenterCode)
	{
		//dbg_formatvar("factor.logic_id:%s  p_logic_id =%s",factor.logic_id,p_logic_id);
		memset(&cfm_point_e, 0, sizeof(cfm_point_e));

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

//交通部票卡结构
void TicketBase::current_jtbtrade_info(JTBTradeInfo& src_info)
{
	uint8_t temp[2];
    memcpy(src_info.time, CmdSort::m_time_now, 7);
    memcpy(src_info.sam, m_p_current_sam, 16);
    src_info.station_id = Api::current_station_id;
	src_info.line_id = (uint8_t)((Api::current_station_id >> 8) & 0xFF);
	memcpy(src_info.institutionCode, Api::bLocalInstitutionCode, 8);

	temp[0] = (uint8_t)((Api::current_city_id >> 8) & 0xFF);
	temp[1] = (uint8_t)(Api::current_city_id & 0xFF);

	Publics::bcds_to_string(temp, 2, src_info.city_code, 4);

	//dbg_dumpmemory("time",src_info.time,7);
	//dbg_formatvar("sam |= %s",src_info.sam);
	//dbg_formatvar("station_id |= %ld",src_info.station_id);
	//dbg_formatvar("line_id |= %02X",src_info.line_id);
	//dbg_dumpmemory("institutionCode",src_info.institutionCode,8);
	//dbg_formatvar("city_code |= %s",src_info.city_code);
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
