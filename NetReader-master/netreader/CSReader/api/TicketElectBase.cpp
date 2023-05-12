//
// Created by flpkp on 2020/3/17.
//
#include "../ble/ble.h"
#include "TicketElectBase.h"
#include "Errors.h"
#include "Publics.h"
#include "TimesEx.h"
#include "Extra.h"

RETINFO TicketElectBase::bom_elect_update(PETICKET_ABNORML_HANDLE p_update, uint8_t* dataBack, uint16_t& len_data) {
    RETINFO ret				= {0};
    ETYTKOPER operType = operNone;
    uint8_t tac_type=0;
    char * p_trade_tac=NULL;
    long sam_seq=0;

    int type=p_update->trad_type[1];
    type -= 48;//转bcd

    operType = convertType(type);
    //如果是单程票，将罚款类型归类到补票类型中
    if(getCurrentTicketType()==ELECT_TICKET_SJ&&operType==operPenaltyRes){
        operType=operAddTicketResult;
    }
    switch (operType)
    {
    case operAddEntry:
        memcpy(m_ticket_data.qr_ticketdata.startStation, p_update->station_code, 4);
        break;
    case operAddExit:
        memcpy(m_ticket_data.qr_ticketdata.exitStation, p_update->station_code, 4);
        break;
    default:
        break;
    }

    g_Record.log_out(0, level_disaster, "operType=%d", operType);
    //添加更新交易中的更新交易
    m_ticket_data.qr_ticketdata.TxnAmount= p_update->amount_over_take+p_update->amount_timeout;
    //从不同的类型中判断中获取p_eTicket_info 值
    m_ticket_data.qr_ticketdata.lPenalty2 = p_update->amount_over_take;
    getDealData(operType, len_data, dataBack,p_update->amount_timeout);
    if(p_update->net_result[0]!='0'){
        ret = write_card(operType, dataBack, len_data, 0x09, 0, sam_seq, NULL);
        //TODO:两次回写蓝牙，可能超过上位机定的，只在BOM中回写2次
        //FIXME:是否特定异常才需要回写两次
        if(ret.wErrCode !=ERR_SUCCEED){
            ret = write_card(operType, dataBack, len_data, 0x09, 0, sam_seq, NULL);
        }
        g_Record.log_out(0, level_disaster, "ret=%d", ret);
    }else{
        //TODO:丢到线程中去回写？
        Extra::ble_seq_add(operType,m_ticket_data.qr_passcode, dataBack, len_data);
    }
    return ret;
}

RETINFO TicketElectBase::write_card(ETYTKOPER operType, uint8_t * p_eTicket_info,
                                uint16_t length, uint8_t tac_type, long trade_amount,
                                long& sam_seq, char * p_trade_tac)
{
    RETINFO ret			= {0};
    uint8_t u_recv[64]	= {0};
    char sz_tac[11]		= {0};
    int result = -1;

    do
    {
        result = ble::ble_all_oper(operType,m_ticket_data.qr_passcode, p_eTicket_info, length);
        //TODO:两次回写蓝牙，可能超过上位机定的3s，
        //FIXME:是否特定异常才需要回写两次
        //if(result!=0){
          //  result = ble::ble_all_oper(operType,m_ticket_data.qr_passcode, p_eTicket_info, length);
        //}
        if(result != 0)
        {
            if (result == 70){
                g_Record.log_out(0, level_error,"============================if ble_writeBack match ble failed 0x70==================================");
                ret.wErrCode = ERR_BLE_ADDRESS_MATCH;
            }else if(result == 71){
                g_Record.log_out(0, level_error,"============================if ble_writeBack back 0x71==================================");
                ret.wErrCode = ERR_BLE_CONNECT;
            }else if(result == 72){
                g_Record.log_out(0, level_error,"============================if ble_writeBack back 0x72==================================");
                ret.wErrCode = ERR_BLE_GETHANDLE;
            }else if(result == 73){
                g_Record.log_out(0, level_error,"============================if ble send data err 0x73=====================");
                ret.wErrCode = ERR_BLE_SEND_OVERTIME;
            }else if(result == 74){
                g_Record.log_out(0, level_error,"============================if ble_writeBack back 0x74 recive overtime=====================");
                ret.wErrCode = ERR_BLE_WRITEBACK;
            }else if(result == 64){
                ret.wErrCode = ERR_CARD_READ;
            }else if(result == -8){
                g_Record.log_out(0, level_error,"============================ble get data overtime no data resp==================================");
                ret.wErrCode = ERR_BLE_GET_OVERTIME;
            }else if(result == 87){
                g_Record.log_out(0, level_error,"============================ble will be break===========================");
                ret.wErrCode = ERR_BLE_BEBREAK;
            }else{
                g_Record.log_out(0, level_error,"============================ble appear other error==================================");
                ret.wErrCode = ERR_BLE_OTHER;
            }
            break;
        }

    } while (0);
    if (ret.wErrCode == ERR_BLE_WRITEBACK)
        ret.bNoticeCode = support_confirm(operType);

    return ret;
}

uint16_t TicketElectBase::write_card(uint8_t tac_type, uint8_t * p_app_ctrl, int record_id, uint8_t lock_flag, uint8_t * p_file_data, long& sam_seq)
{
    uint16_t ret				= ERR_SUCCEED;
    unsigned char u_recv[512]	= {0};

    do
    {

    } while (0);

    return ret;
}

void TicketElectBase::get_elect_update_info(ETYTKOPER operType, uint8_t * p_eTicket_info,uint16_t len_eTicket_info){

}

ETYTKOPER TicketElectBase::convertType(int trad_type){
    ETYTKOPER operType;
    switch (trad_type){
        case 0:
            operType=operAddTicketResult;
            break;
        case 1:
            operType=operPenaltyRes;
            break;
        case 2:
            operType=operClearEntry;
            break;
        case 3:
            operType=operClearExit;
            break;
        case 4:
            operType=operAddEntry;
            break;
        case 5:
            operType=operAddExit;
            break;
        default:
            operType=operNone;
            break;
    }
    return operType;
}

void TicketElectBase::getQRHeadInfo(uint8_t *p_grInfo){
    uint8_t ttemp = 0x00;

    ttemp = p_grInfo[0];
    int flag = (int)(ttemp);//苹果为1  安卓为2

    Api::sysFlag = flag%2;

    if(Api::sysFlag == 0) {
        g_Record.log_out(0, level_disaster, "android");
    }
    else {
        g_Record.log_out(0, level_disaster, "ios");
    }
    m_ticket_data.qr_ticker_isTest = p_grInfo[3];
    m_ticket_data.qr_Intnet_ststus = p_grInfo[13];//移动网络状态标识
    m_ticket_data.qr_ticket_status = p_grInfo[14];
    g_Record.log_out(0, level_error,"get_qrInfo(qr_ticket_status=%c)", m_ticket_data.qr_ticket_status);
    g_Record.log_out(0, level_disaster,"qr_net = %c", m_ticket_data.qr_Intnet_ststus);

}

void TicketElectBase::getQRDataComInfo(uint8_t *p_grInfo){
    memcpy(m_ticket_data.qr_ticketdata.ticketVersion, p_grInfo + 15, 2);
    memcpy(m_ticket_data.qr_ticketdata.subType, p_grInfo + 17, 2);

    memcpy(m_ticket_data.qr_ticketdata.cardNo, "0000000000", 10);
    memcpy(m_ticket_data.qr_ticketdata.cardNo+10, p_grInfo + 19, 10);
    g_Record.log_out(0, level_error,"get_qrInfo(cardNo=%s)", m_ticket_data.qr_ticketdata.cardNo);

}

RETINFO TicketElectBase::analyse_free_zone(MODE_EFFECT_FLAGS * p_degrade_falgs, TICKET_PARAM * p_ticket_prm, PBOMANALYZE p_analyse, PBOMETICANALYZE p_eticanalyse)
{
    RETINFO ret				    = {0};
    long penalty_val_need		= 0;
    long lowest_val				= 0;
    char entry_mode				= 0;
    uint8_t validOutTime[7]		= {0};
    uint8_t EntryTime[7]        = {0};
    uint16_t StartStation       = 0;
    g_Record.log_out(0, level_error,"analyse_free_zone:[m_time_now = %02x%02x%02x%02x%02x%02x%02x]",CmdSort::m_time_now[0],CmdSort::m_time_now[1],CmdSort::m_time_now[2],CmdSort::m_time_now[3],CmdSort::m_time_now[4],CmdSort::m_time_now[5],CmdSort::m_time_now[6]);
    g_Record.log_out(0, level_disaster,"analyse_free_zone:[status = %01x]",m_ticket_data.simple_status);
    switch (m_ticket_data.simple_status){
        case SStatus_Sale:
            if (getCurrentTicketType()==ELECT_TICKET_SJ && !g_Parameter.same_station(m_ticket_data.p_entry_info->station_id, Api::current_station_id))
            {
                if (p_degrade_falgs->entry_by_emergency)
                {
                    ret.wErrCode = 0;
                    entry_mode = (char)(0x30 + degrade_Green_Light);
                }
                else
                {
                    ret.wErrCode = ERR_CARD_USE;
                    g_Record.log_out(0, level_error, "station [%ld]is not current station[%ld] ", m_ticket_data.p_entry_info->station_id, Api::current_station_id);
                    break;
                }
            }
            break;
        //已经进站：已经有进站码了
        case SStatus_Entry://1，检查紧急模式
            if (p_degrade_falgs->entry_by_emergency)
            {
                ret.wErrCode = 0;
                entry_mode = (char)(0x30 + degrade_Green_Light);
                break;
            }
            //2，查询最低票价
            ret.wErrCode = g_Parameter.query_lowest_fare(CmdSort::m_time_now, m_ticket_data.fare_type,lowest_val);
            if (ret.wErrCode)	break;
            //3，判断进站站点以及进站时间
            Publics::string_to_bcds(m_ticket_data.qr_ticketdata.validOutTime, 14, validOutTime, 7);
            Publics::string_to_bcds(m_ticket_data.qr_ticketdata.entryTime, 14, EntryTime, 7);
            for (int i = 0; i < 4; i++)
            {
                StartStation =( (StartStation << 4) + Publics::char_to_half_hex(m_ticket_data.qr_ticketdata.startStation[i]));
            }
            g_Record.log_out(0, level_disaster, "analyse_free_zone:[EntryTime = %02x%02x%02x%02x%02x%02x%02x StartStation = %x]", EntryTime[0], EntryTime[1], EntryTime[2], EntryTime[3], EntryTime[4], EntryTime[5], EntryTime[6], StartStation);

            if (!station_is_current(StartStation))// 非本站进站
            {
                ret.wErrCode = ERR_ENTRY_STATION;
                if(getCurrentTicketType()!=ELECT_TICKET_SJ){
                    penalty_val_need = lowest_val;
                }
            }else if(TimesEx::timeout(CmdSort::m_time_now, EntryTime, 20)){//进站超时20分钟
                ret.wErrCode = ERR_ENTRY_TIMEOUT;
                if(getCurrentTicketType()!=ELECT_TICKET_SJ){
                    penalty_val_need = lowest_val;
                }
            }else// 有进站码
            {
                ret.wErrCode = ERR_ENTRY_EVER;
            }
            if (m_ticket_data.wallet_value != NULL)
            {
                if (m_ticket_data.wallet_value < lowest_val)
                {
                    ret.wErrCode = ERR_LACK_WALLET;
                    break;
                }
            }
            //4,赋值罚款金额
            if (p_eticanalyse != NULL)
            {
                memcpy(p_eticanalyse->cPenaltyType,"01",2);//todo 非本站进站罚款状态码返回未知
                p_eticanalyse->lPenalty1=penalty_val_need;
            }
            break;
        case SStatus_Exit://没有进站，或者已经出站  正常，不需要更新
            // if (station_is_current(m_ticket_data.p_exit_info->station_id))//已经出站，判断出站站点
            // {
            //     ret.wErrCode = ERR_EXIT_ALREADY;//本站出站
            // }else{
            //     ret.wErrCode = ERR_EXIT_OTHER;//其他站出站
            // }
            //
            if(Api::current_device_type==4||Api::current_device_type==6){
                ret.wErrCode=ERR_ETICKET_QR_INVALID;
            }
            break;
        case SStatus_Invalid:
        default://对于禁止使用以及取票成功的状态，分析时报票卡状态异常
        ret.wErrCode = ERR_ETICKET_UNKNOEWN_STATUS;
            break;
    }
    g_Record.log_out(0, level_disaster, "analyse_free_zone:[validOutTime = %02x%02x%02x%02x%02x%02x%02x lowest_val = %d]", validOutTime[0], validOutTime[1], validOutTime[2], validOutTime[3], validOutTime[4], validOutTime[5], validOutTime[6],lowest_val);

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

void TicketElectBase::getDealData(ETYTKOPER operType,uint16_t& len_data,uint8_t * dataBack,long amount)
{


}

RETINFO TicketElectBase::check_online(ETYTKOPER operType,PETICKETAUTHINFO p_eticketauthinfo){
    RETINFO ret				= {0};
    //双联机方案，进闸接口不执行蓝牙操作
    memcpy(Api::cCenterCode, m_ticket_data.qr_ticketdata.centerCode, 32);//中心票号
    memcpy(Api::m_tdeticket.centerCode, m_ticket_data.qr_ticketdata.centerCode, 32);//中心票号
    memcpy(Api::m_tdeticket.cardLogicNo, m_ticket_data.qr_ticketdata.cardNo, 20);//逻辑卡号
    memcpy(Api::m_tdeticket.subType, m_ticket_data.qr_ticketdata.subType, 2);//子类型
    memcpy(Api::m_tdeticket.qr_readbuf, Api::qr_readbuf, 512);//上次交易的数据

    if(operType == operEntry)
    {
        memcpy(Api::m_tdeticket.dealType, "07", 2);
        operType = operEntryCheck;
    }
    else{
        memcpy(Api::m_tdeticket.dealType, "08", 2);
        operType = operExitCheck;
    }

    transfer_edata_for_out(operType, p_eticketauthinfo);

    ret.bRfu[0] = 0x11;
    ret.bRfu[1] = 0x00;

    return ret;
}

RETINFO TicketElectBase::EPayData(PETICKETPAYDEALINFO p_purse, uint8_t* p_time, char* paytype)
{
    RETINFO ret = { 0 };
    char logical_type[4] = { 0 };

    memcpy(p_purse->cPayType, paytype, 2);                   //交易类型
    Publics::bcds_to_string(p_time, 7, p_purse->cTxnTms, 14);//交易时间
    Publics::bcds_to_string(m_ticket_data.logical_type, 2, logical_type, 4);
    memcpy(p_purse->cTicketMainType, logical_type, 2);//主类型
    memcpy(p_purse->cTicketSubType, logical_type + 2, 2);//子类型
    if (paytype[1] != 0)
    {
        transfer_edata_for_out(operEIncr, p_purse);
    }
    else
    {
        TicketBase::transfer_edata_for_out(operEDecr, p_purse);
    }
}


