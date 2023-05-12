//
// Created by flpkp on 2020/3/22.
//

#include "TicketElectDepositBase.h"

void TicketElectDepositBase::getDealData(ETYTKOPER operType,uint16_t& len_data,uint8_t * dataBack,long amount){
    int length = 0;
    char StationCode[2+1] = {0};
    char LineCode[2+1] = {0};
    char DeviceType[2+1] = {0};
    char DeviceCode[3+1] = {0};
    char time[14+1] = {0};
    char exittime[14+1] = {0};
    long penalty_val	= 0;
    uint8_t tempTime[7] = {0};
    uint8_t validexitTime[7] = {0};

    char dealData[256] = {0};
    sprintf(LineCode, "%02x", ((Api::current_station_id >>8) & 0x00FF));
    sprintf(StationCode, "%02x", (Api::current_station_id & 0xFF));
    sprintf(DeviceType, "%02x", (uint8_t)(Api::current_device_type));
    sprintf(DeviceCode, "%03x", Api::current_device_id);
    
    memcpy(tempTime, CmdSort::m_time_now, 7);
    sprintf(time, "%02x%02x%02x%02x%02x%02x%02x", tempTime[0], tempTime[1], tempTime[2], tempTime[3], tempTime[4], tempTime[5], tempTime[6]);
    g_Parameter.get_validexittime(Api::current_station_id,tempTime, validexitTime, &penalty_val);
    sprintf(exittime, "%02x%02x%02x%02x%02x%02x%02x", validexitTime[0], validexitTime[1], validexitTime[2], validexitTime[3], validexitTime[4], validexitTime[5], validexitTime[6]);
    memcpy(m_ticket_data.qr_ticketdata.validOutTime, exittime, 14);
    switch (operType)
    {
        case operEntry:
        {
            memcpy(dealData,"01",2);
            length += 2;
            memcpy(dealData + length, time, 14);
            length += 14;
            memcpy(dealData + length, exittime, 14);
            length += 14;
            memcpy(dealData + length, LineCode, 2);//线路编码
            length += 2;
            memcpy(dealData + length, StationCode, 2);//站点编码
            length += 2;
            memcpy(dealData + length, DeviceType, 2);//设备类型
            length += 2;
            memcpy(dealData + length, DeviceCode, 3);//设备编号
            length += 3;
            memcpy(dealData + length, "112233445566", 12);//暂时不写
            length += 12;
            memcpy(dealData + length, "00", 2);
            length += 2;
            memcpy(dealData + length, "00000000", 8);
            length += 8;
            len_data = length;

        }
            break;
        case operExit:
        {
            memcpy(dealData,"02",2);
            length += 2;
            memcpy(dealData + length, time, 14);
            length += 14;
            memcpy(dealData + length, LineCode, 2);//线路编码
            length += 2;
            memcpy(dealData + length, StationCode, 2);//站点编码
            length += 2;
            memcpy(dealData + length, DeviceType, 2);//设备类型
            length += 2;
            memcpy(dealData + length, DeviceCode, 3);//设备编号
            length += 3;
            memcpy(dealData + length, "112233445566", 12);
            length += 12;
            //应扣金额
            sprintf(dealData + length,"%08X",amount);
            length += 8;

            memcpy(dealData + length, "00", 2);
            length += 2;
            memcpy(dealData + length, "00000000", 8);
            length += 8;
            len_data = length;

        }
            break;

        case operPenaltyRes:										//罚款通知
        {
            memcpy(dealData, "07", 2);								//交易码 07罚款
            length += 2;
            memcpy(dealData + length, time, 14);					//时间
            length += 14;
            memcpy(dealData + length, "0000", 4);
            length += 4;
            memcpy(dealData + length, Api::operator_id, 6);			//操作员ID operator_id定义是10位，BOM操作员ID为6位前补4位0
            length += 6;
            memcpy(dealData + length, LineCode, 2);				    //线路编码
            length += 2;
            memcpy(dealData + length, StationCode, 2);				//站点编码
            length += 2;
            memcpy(dealData + length, "01", 2);						//罚款类型 01-超时罚款
            length += 2;
            sprintf(dealData + length, "%08X", amount);				//金额
            length += 8;
            memcpy(dealData + length, exittime, 14);				//有效出站时间
            length += 14;
            // 罚款描述没有传 经与华腾沟通，不传可以
            memcpy(dealData + length, "00000000", 8);				//验证码
            length += 8;
            len_data = length;
        }
            break;
        case operClearEntry:										//清进站
        {
            memcpy(dealData, "21", 2);								//交易码 21清进站
            length += 2;
            memcpy(dealData + length, m_ticket_data.qr_ticketdata.entryTime, 14);//进站时间 TODO 我觉得这个应该是已记录的进站时间
            length += 14;
            memcpy(dealData + length, exittime, 14);				//有效出站时间
            length += 14;
            memcpy(dealData + length, m_ticket_data.qr_ticketdata.startStation, 4);//进站的车站id
            length += 4;
            memcpy(dealData + length, m_ticket_data.qr_ticketdata.entryDeviceType, 2);	//设备类型
            length += 2;
            memcpy(dealData + length, m_ticket_data.qr_ticketdata.entryDeviceCode, 3);//设备编号
            length += 3;
            memcpy(dealData + length, "0000", 4);
            length += 4;
            memcpy(dealData + length, Api::operator_id, 6);				//操作员ID
            length += 6;
            memcpy(dealData + length, LineCode, 2);				        //清进站线路编码
            length += 2;
            memcpy(dealData + length, StationCode, 2);					//清进站站点编码
            length += 2;
            memcpy(dealData + length, DeviceType, 2);				    //清进站设备类型
            length += 2;
            memcpy(dealData + length, DeviceCode, 3);				    //清进站设备编号
            length += 3;
            memcpy(dealData + length, time, 14);					    //清进站的时间
            length += 14;
            memcpy(dealData + length, m_p_current_sam + 4, 12);		    //终端号
            length += 12;
            memcpy(dealData + length, "00", 2);						    //状态码0—正常；1—失败
            length += 2;
            memcpy(dealData + length, "00000000", 8);				    //验证码
            length += 8;
            len_data = length;
        }
            break;
        case operClearExit:												      //清出站
        {
            memcpy(dealData, "22", 2);								            //交易码 22清出站
            length += 2;
            memcpy(dealData + length, m_ticket_data.qr_ticketdata.exitTime, 14);//出站时间 TODO 我觉得这个应该是已记录的出站时间
            length += 14;
            memcpy(dealData + length, m_ticket_data.qr_ticketdata.exitStation, 4);//出站的车站id
            length += 4;
            memcpy(dealData + length, m_ticket_data.qr_ticketdata.exitDeviceType, 2);//设备类型
            length += 2;
            memcpy(dealData + length, m_ticket_data.qr_ticketdata.exitDeviceCode, 3);//设备编号
            length += 3;
            memcpy(dealData + length, m_p_current_sam + 4, 12);		//终端号
            length += 12;
            //应扣金额
            sprintf(dealData + length, "%08X", amount);
            length += 8;
            memcpy(dealData + length, "0000", 4);
            length += 4;
            memcpy(dealData + length, Api::operator_id, 6);			//操作员ID
            length += 6;
            memcpy(dealData + length, LineCode, 2);				    //清进站线路编码
            length += 2;
            memcpy(dealData + length, StationCode, 2);				//清进站站点编码
            length += 2;
            memcpy(dealData + length, DeviceType, 2);				//清进站设备类型
            length += 2;
            memcpy(dealData + length, DeviceCode, 3);				//清进站设备编号
            length += 3;
            memcpy(dealData + length, time, 14);					//清出站的时间
            length += 14;
            memcpy(dealData + length, "00", 2);						//状态码0—正常；1—失败
            length += 2;
            memcpy(dealData + length, "00000000", 8);				//验证码
            length += 8;
            len_data = length;
        }
            break;
        case operAddEntry:											//补进站
        {
            memcpy(dealData, "23", 2);								//交易码 23补进站
            length += 2;
            memcpy(dealData + length,time, 14);						//补进站时间 TODO 用的现在时间
            length += 14;
            memcpy(dealData + length, exittime, 14);				//有效出站时间
            length += 14;
            memcpy(dealData + length, m_ticket_data.qr_ticketdata.startStation, 4);//进站的车站id
            length += 4;
            //进站设备类型  为固定值00
            memcpy(dealData + length, "00", 2);
            length += 2;
            //进站设备编号   固定值为000
            memcpy(dealData + length, "000", 3);
            length += 3;
            //操作员ID  一共10位
            memcpy(dealData + length, "0000", 4);
            length += 4;
            memcpy(dealData + length, Api::operator_id, 6);
            //补进站设备编码
            length += 6;
            memcpy(dealData + length, LineCode, 2);			    	//补进站线路编码
            length += 2;
            memcpy(dealData + length, StationCode, 2);				//补进站站点编码
            length += 2;
            memcpy(dealData + length, DeviceType, 2);				//补进站设备类型
            length += 2;
            memcpy(dealData + length, DeviceCode, 3);				//补进站设备编号
            length += 3;
            memcpy(dealData + length, m_p_current_sam + 4, 12);		//终端号
            length += 12;
            memcpy(dealData + length, "00", 2);						//状态码0—正常；1—失败
            length += 2;
            memcpy(dealData + length, "00000000", 8);				//验证码
            length += 8;
            len_data = length;
        }
            break;
        case operAddExit:											//补出站
        {
            memcpy(dealData, "24", 2);								//交易码 24补出站
            length += 2;
            memcpy(dealData + length, time, 14);					//补出站时间 TODO 用的现在时间
            length += 14;
            memcpy(dealData + length, LineCode, 2);				    //补出站线路编码
            length += 2;
            memcpy(dealData + length, StationCode, 2);				//补出站线路编码
            length += 2;
            memcpy(dealData + length, "00", 2);						//设备类型 固定为00
            length += 2;
            memcpy(dealData + length, "000", 3);					//设备编号 固定为000
            length += 3;
            sprintf(dealData + length, "%08X", amount);				//应扣金额
            length += 8;
            //10位操作员ID
            memcpy(dealData + length, "0000", 4);
            length += 4;
            memcpy(dealData + length, Api::operator_id, 6);			//操作员ID
            length += 6;
            memcpy(dealData + length, LineCode, 2);				    //补出站线路编码
            length += 2;
            memcpy(dealData + length, StationCode, 2);				//补出站线路编码
            length += 2;
            memcpy(dealData + length, DeviceType, 2);				//补出站线路编码
            length += 2;
            memcpy(dealData + length, DeviceCode, 3);				//补出站线路编码
            length += 3;
            memcpy(dealData + length, m_p_current_sam + 4, 12);		    //终端号
            length += 12;
            memcpy(dealData + length, "00", 2);						    //状态码0—正常；1—失败
            length += 2;    
            memcpy(dealData + length, "00000000", 8);			    	//验证码
            length += 8;
            len_data = length;
        }
            break;
        default:
            break;
    }
    memcpy(dataBack, (uint8_t * )dealData, length);
    //g_Record.log_buffer("blue write ", dataBack,len_data);


}
