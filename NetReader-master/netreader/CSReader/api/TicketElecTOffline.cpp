#include "TicketElecTOffline.h"
#include "Errors.h"
#include "Publics.h"
#include "TimesEx.h"
#include "TicketsStructs.h"
#include "EnumTypes.h"

TicketElecTOffline::TicketElecTOffline(void)
{

}

TicketElecTOffline::TicketElecTOffline(char * p_current_sam, char * p_sam_posid, uint8_t * physic_info)
{
	m_p_current_sam = p_current_sam;
	m_p_sam_posid = p_sam_posid;

	memset(&m_ticket_data, 0, sizeof(m_ticket_data));

	sprintf(m_ticket_data.physical_id, "%02X%02X%02X%02X%02X%02X%02X%-6C",
		physic_info[0], physic_info[1], physic_info[2], physic_info[3], physic_info[4], physic_info[5], physic_info[6], 0x20);

	pMemoryPos = ((uint8_t *)this) + sizeof(TicketElecTOffline);
	m_ticket_data.p_entry_info = new(pMemoryPos) TradeInfo;

	m_ticket_data.p_exit_info = &m_ticket_data.read_last_info;
	m_ticket_data.p_update_info = &m_ticket_data.read_last_info;

	memset(&m_ticket_data.read_last_info, 0, sizeof(TradeInfo));

	m_ticket_data.qr_flag = 0x01;
}

TicketElecTOffline::~TicketElecTOffline(void)
{

}


RETINFO TicketElecTOffline::analyse_common(uint8_t wk_area, MODE_EFFECT_FLAGS * p_degrade_falgs,
        TICKET_PARAM * p_ticket_prm, PTICKETLOCK p_lock, uint16_t& lenlock,
        PBOMANALYZE p_analyse, char * p_mode)
{
    RETINFO ret				= {0};
	uint8_t qrTimeTemp[7]		= {0};

	//�������뻪�����飬��ά������ʱ���ÿ3sˢ��һ�Σ���ά��ʱЧ��Ϊ1����
	//Ʊ����������Ӫ����ʹ����Ч����д���޷������ɻ���appʵʱ��������ʱ�䣬���д�����һ����Ϊ��Ч��ά��
	g_Record.log_out(0, level_error,"analyse_common(cur date=%02x%02x%02x%02x%02x%02x%02x)", CmdSort::m_time_now[0],CmdSort::m_time_now[1],CmdSort::m_time_now[2],CmdSort::m_time_now[3],CmdSort::m_time_now[4],CmdSort::m_time_now[5],CmdSort::m_time_now[6]);
	Publics::string_to_bcds(m_ticket_data.qr_ticketdata.applyTime,14,qrTimeTemp,7);
	g_Record.log_out(0, level_error,"analyse_common(qr  date=%02x%02x%02x%02x%02x%02x%02x)", qrTimeTemp[0], qrTimeTemp[1], qrTimeTemp[2], qrTimeTemp[3], qrTimeTemp[4], qrTimeTemp[5], qrTimeTemp[6]);
	TimesEx::bcd_time_calculate(qrTimeTemp, T_TIME, 0, 0, 0, 60);//��ΧƱ��ʱ���60��
	if(memcmp(CmdSort::m_time_now, qrTimeTemp, 7)>0){//��ǰʱ�������60s֮���ʱ�仹Ҫ��ǰ�����ж�Ϊ��Ч
		g_Record.log_out(0, level_error,"analyse_common not today qr");
		ret.wErrCode=ERR_ETICKET_QR_INVALID;
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

RETINFO TicketElecTOffline::analyse_free_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PBOMETICANALYZE p_eticanalyse){
	RETINFO ret = {0};
	return ret;
}

RETINFO TicketElecTOffline::analyse_work_zone(MODE_EFFECT_FLAGS * p_degrade_falgs,
        TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PPURSETRADE p_purse,
        PBOMETICANALYZE p_eticanalyse)
{
	RETINFO ret				    = {0};
	return ret;
}


RETINFO TicketElecTOffline::entry_gate(PENTRYGATE p_entry,PETICKETDEALINFO p_eticketdealinfo,char cOnlineRes)
{
	RETINFO ret				= {0};
	ret.bRfu[0] = 0x11;
	ret.bRfu[1] = 0x00;
	//�ѻ�����Ʊ���ɺ�̨����ODƥ��
	transfer_edata_for_out(operEntry, p_eticketdealinfo);
	return ret;
}

RETINFO TicketElecTOffline::exit_gate(PPURSETRADE p_exit,PETICKETDEALINFO p_eticketdealinfo, MODE_EFFECT_FLAGS * p_degrade_falgs,char cOnlineRes)
{
	RETINFO ret				= {0};
	ret.bRfu[0] = 0x11;
	ret.bRfu[1] = 0x00;
	//�ѻ�����Ʊ���ɺ�̨����ODƥ��
	transfer_edata_for_out(operExit, p_eticketdealinfo);
	return ret;
}

RETINFO TicketElecTOffline::getETicket(PGETETICKETRESPINFO p_geteticketrespinfo)
{
	RETINFO ret				= {0};
	return ret;
}


RETINFO TicketElecTOffline::bom_update(PTICKETUPDATE p_update, uint8_t * p_entry_station)
{
	RETINFO ret				= {0};
	return ret;
}

RETINFO TicketElecTOffline::read_card(ETYTKOPER operType, TICKET_PARAM& ticket_prm, uint8_t * p_read_init/* = NULL*/)
{
    RETINFO ret				= {0};
    uint8_t	eticket_info[512]  = {0};

	memcpy(m_ticket_data.qr_passcode, Api::qr_readbuf + 10 + 7, 32);
	memcpy(Api::qr_passcode, m_ticket_data.qr_passcode, 32);

	memcpy(eticket_info, Api::clear_qr_readbuf/* + 42*/, 512);
	get_qrInfo(eticket_info);
	//ת������
	// �߼�����
	memcpy(m_ticket_data.logical_id, m_ticket_data.qr_ticketdata.cardNo, 20);
	// Ǯ�����
	m_ticket_data.wallet_value = m_ticket_data.qr_ticketdata.balance;
    return ret;
}

void TicketElecTOffline::get_qrInfo(uint8_t * p_grInfo)
{
	uint8_t temp[4]		= {0};
	uint8_t temp_entry[4]		= {0};
	int len_getData=0;  //���ڼ�¼������ά��ĵ�ǰλ��
    getQRHeadInfo(p_grInfo);
    getQRDataComInfo(p_grInfo);
	len_getData+=29;

    //1,Ʊ��
	memcpy(m_ticket_data.qr_ticketdata.centerCode, p_grInfo + len_getData, 32);
	len_getData+=32;
	g_Record.log_out(0, level_error,"get_qrInfo(centerCode=%s)", m_ticket_data.qr_ticketdata.centerCode);
	//բ��ֻ��ǰ7λ��ǰʱ��+���Ͽ���
	memcpy(m_ticket_data.timeAndcenterCode, CmdSort::m_time_now, 7);
	memcpy(m_ticket_data.timeAndcenterCode + 7, m_ticket_data.qr_ticketdata.cardNo, 20);
	g_Record.log_buffer("timeAndcenterCode=", m_ticket_data.timeAndcenterCode,27);
    //2,���
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

	//�ѻ�Ʊ���ɶ�ά��ʱ��
	memcpy(m_ticket_data.qr_ticketdata.applyTime, p_grInfo + len_getData, 14);
	len_getData+=14;
	g_Record.log_out(0, level_error,"get_qrInfo(applyTime=%s)", m_ticket_data.qr_ticketdata.applyTime);
	//��ǰƱ������ʱ��
	memcpy(m_ticket_data.qr_ticketdata.centerCodeProductTime, p_grInfo + len_getData, 14);
	g_Record.log_out(0, level_error,"get_qrInfo(centerCodeProductTime=%s)", m_ticket_data.qr_ticketdata.centerCodeProductTime);
	len_getData+=14;
	//�����ֶ�
	memcpy(m_ticket_data.qr_ticketdata.retainField, p_grInfo + len_getData, 10);
	len_getData+=10;
	//������֤��
	memcpy(m_ticket_data.qr_ticketdata.mac, p_grInfo + len_getData, 8);
	len_getData+=8;

	memcpy(m_ticket_data.logical_type, "\x02\x00", 2);//���ⴢֵƱ
	memcpy(m_ticket_data.fare_type, m_ticket_data.logical_type, 2);
}


ElectTicketType TicketElecTOffline::getCurrentTicketType(){
    return ELECT_TICKET_OFFLINE;//�ѻ�����Ʊ
}

void TicketElecTOffline::getDealData(ETYTKOPER operType,uint16_t& len_data,uint8_t * dataBack,long amount){

}


void TicketElecTOffline::transfer_edata_for_out(ETYTKOPER operType, void * pTradeStruct/*, uint8_t bPaymentMeans*/)
{
	char szTemp[32]		= {0};
	switch (operType)
	{
	case operEntry:
	case operExit:
	{
		PETICKETDEALINFO pGetETicket = (PETICKETDEALINFO)pTradeStruct;
		if(operType==operEntry){
			memcpy(pGetETicket->cTransCode, "07", 2);
		}else{
			memcpy(pGetETicket->cTransCode, "08", 2);
		}
		sprintf(szTemp, "%02x%02x%02x%02x", CmdSort::m_time_now[0], CmdSort::m_time_now[1], CmdSort::m_time_now[2], CmdSort::m_time_now[3]);
		memcpy(pGetETicket->cTxnDate, szTemp, 8);

		memcpy(pGetETicket->cTicketMainType, "11", 2);//������
		memcpy(pGetETicket->cTicketSubType, m_ticket_data.qr_ticketdata.subType, 2);//������

		memcpy(pGetETicket->cChipType, "07", 2);//оƬ����

		memcpy(pGetETicket->cTicketLogicalNo, m_ticket_data.qr_ticketdata.cardNo, 20);//�߼�����

		memcpy(pGetETicket->cCenterCode, m_ticket_data.qr_ticketdata.centerCode, 32);//����Ʊ��

		memcpy(pGetETicket->cCenterMac, m_ticket_data.qr_ticketdata.mac, 8);//mac

		memcpy(pGetETicket->cRealTicketNo, "0000000000000000", 16);//ʵ�忨��

		memcpy(pGetETicket->cMark, "00", 2);//ȡƱ����

		memcpy(pGetETicket->cTicketCSN, "0000000000000000", 16);//��������

		sprintf(pGetETicket->cTicketCount,"%010ld",m_ticket_data.qr_ticketdata.counter);

		memcpy(pGetETicket->cPsamNo, m_p_current_sam, 16);

		memcpy(pGetETicket->cPsamSeq, "0000000000", 10);

		sprintf(szTemp, "%02x%02x%02x%02x%02x%02x%02x", CmdSort::m_time_now[0], CmdSort::m_time_now[1], CmdSort::m_time_now[2], CmdSort::m_time_now[3], CmdSort::m_time_now[4], CmdSort::m_time_now[5], CmdSort::m_time_now[6]);
		memcpy(pGetETicket->cTxnTms, szTemp, 14);
		//������ʱû����Ч��վʱ�䣬���ֶ�ȫ��Ĭ��д0
		memcpy(pGetETicket->cValidOutTms, "00000000000000", 14);

		memcpy(pGetETicket->cPayType, "01", 2);

		sprintf(pGetETicket->cBeforeTxnBalance,"%010ld",m_ticket_data.qr_ticketdata.balance);

		memcpy(pGetETicket->cTxnAmountNo, "000000", 6);

		sprintf(pGetETicket->cTxnAmount,"%06ld",0);

		memcpy(pGetETicket->cDiscountAmt, "000000", 6);

		sprintf(pGetETicket->cTxnStation, "%04x", Api::current_station_id);

		memcpy(pGetETicket->cLastStation, "0000", 4);

		memcpy(pGetETicket->cLastTxnTms, "00000000000000", 14);

		sprintf(pGetETicket->cDevNodeId, "%04x%02x%03x", Api::current_station_id, Api::current_device_type, Api::current_device_id);

		memcpy(pGetETicket->cDevSeqNo, "0000000000", 10);

		memcpy(pGetETicket->cTestMark, "00", 2);

		memcpy(pGetETicket->cQRSerialNo, "00000000", 8);

		memcpy(pGetETicket->cOverAmount, "000000", 6);

		memcpy(pGetETicket->cTac, "00000000", 8);

		memcpy(pGetETicket->cNetTranNumber, "0000000000000000000000000000000000000000000000000000000000000000", 64);
		//������
		memcpy(pGetETicket->cReserve, "01", 2);//������ά��̶�Ϊ01  �������µ�ITP��SLE�ӿ��ĵ�
		memcpy(pGetETicket->cReserve+2, "000000000000000000000000000000", 30);

	}
		break;
	case operEntryCheck:
	case operExitCheck:
	{
		PETICKETAUTHINFO pGetETicket = (PETICKETAUTHINFO)pTradeStruct;
		if(operType==operEntryCheck){
			memcpy(pGetETicket->cTransCode, "07", 2);
		}else{
			memcpy(pGetETicket->cTransCode, "08", 2);
		}

		memcpy(pGetETicket->cTicketMainType, "11", 2);//������

		memcpy(pGetETicket->cTicketSubType, m_ticket_data.qr_ticketdata.subType, 2);//������

		memcpy(pGetETicket->cTicketLogicalNo, m_ticket_data.qr_ticketdata.cardNo, 20);//�߼�����

		memcpy(pGetETicket->cCenterCode, m_ticket_data.qr_ticketdata.centerCode, 32);//����Ʊ��

		memcpy(pGetETicket->cCenterMac, m_ticket_data.qr_ticketdata.mac, 8);//mac

		pGetETicket->cQrNetStatus = m_ticket_data.qr_Intnet_ststus;//����ƱƱ�� ����״̬��־
	}
		break;
	default:
		break;
	}

}
