// JsonCPP.cpp : Defines the entry point for the console application.
//

#include "../json/json.h"
#include <string>
#include "../link/myprintf.h"
#include "ble.h"
#include "../api/Publics.h"
#include "../api/Declares.h"
#include "../json/writer.h"
#include "../json/JsonCPP.h"
#include "bleBaseProtocol.h"

using namespace Json;
using namespace std;

SCAN_ALL_BLE_INF ble::scanAllBleInf ={0};

ble::ble(void)
{
}


//进站/出站
string ble::ble_getDealJson(ETYTKOPER operType,uint16_t& len_data)
{
	Json::Value json_temp;      // 临时对象，供如下代码使用
	char StationCode[2] = {0};
	char LineCode[2] = {0};
	char DeviceType[2] = {0};
	char DeviceCode[3] = {0};
	char PsamCode[2] = {0};
	char time[14] = {0};

		//dev_inf[0] = (uint8_t)((current_station_id >> 8) & 0xFF);
		//dev_inf[1] = (uint8_t)(current_station_id & 0xFF);
		//dev_inf[2] = current_device_type;
		//dev_inf[3] = (uint8_t)((current_device_id >> 8) & 0xFF);
		//dev_inf[4] = (uint8_t)(current_device_id & 0xFF);

	sprintf(StationCode, "%02x", ((Api::current_station_id >>8) & 0xFF00));
	sprintf(LineCode, "%02x", (Api::current_station_id & 0xFF));
	sprintf(DeviceType, "%02x", (uint8_t)(Api::current_device_type));
	sprintf(DeviceCode, "%03x", Api::current_device_id);
	sprintf(time, "%02x%02x%02x%02x%02x%02x%02x", CmdSort::m_time_now[0], CmdSort::m_time_now[1], CmdSort::m_time_now[2], CmdSort::m_time_now[3], CmdSort::m_time_now[4], CmdSort::m_time_now[5], CmdSort::m_time_now[6]);

	json_temp["LineCode"] = Json::Value(LineCode);//线路编号
	json_temp["StationCode"] = Json::Value(StationCode);//站点编号
	json_temp["DeviceType"] = Json::Value(DeviceType);//设备类型
	json_temp["DeviceCode"] = Json::Value(DeviceCode);//设备编号
	json_temp["PsamCode"] = Json::Value("112233445566");//终端号
	json_temp["DeviceSequence"] = Json::Value(20180702);//PSAM卡流水号
	json_temp["Status"] = Json::Value("00");//状态码
	json_temp["VerifyCode"] = Json::Value("00000000");//验证码

	if(operType == operEntry)
	{
		json_temp["TxnCode"] = Json::Value("07");//交易码
		json_temp["InTime"] = Json::Value(time);//进站时间
	}
	if(operType == operExit)
	{
		json_temp["TxnCode"] = Json::Value("08");//交易码
		json_temp["OutTime"] = Json::Value(time);//出站时间
		json_temp["DebitAmount"] = Json::Value(200);//实扣金额
	}
	if(operType == operGetETicket)
	{
		json_temp["TxnCode"] = Json::Value("10");//交易码
		json_temp["ExchargeTime"] = Json::Value(time);//出票时间
	}

/*
	Json::Value root; // 表示整个 json 对象
	root["key_string"] = Json::Value("value_string"); // 新建一个 Key(名为：key_string),
	//赋予字符串值:"value_string"
	root["key_number"] = Json::Value(12345); // 新建一个 Key(名为:key_number),
	//赋予数值:12345。
	root["key_boolean"] = Json::Value(false);  // 新建一个 Key(名为:key_boolean),
	//赋予bool值:false。
	root["key_double"] = Json::Value(12.345); // 新建一个 Key(名为:key_double),
	//赋予 double 值:12.345
	root["key_object"] = json_temp; // 新建一个 Key(名为:key_object)，
	//赋予 json::Value 对象值。
	//root["key_array"].append("array_string");// 新建一个 Key(名为:key_array),
	//类型为数组,对第一个元素赋值为字符串:"array_string"
	//root["key_array"].append(1234);  // 为数组 key_array 赋值,对第二个元素赋值为:1234。

	//Json::ValueType type = root.type();// 获得 root 的类型，此处为 objectValue 类型。
	*/

	Json::FastWriter fast_writer;
	string jsonRoot = fast_writer.write(json_temp);
	dbg_formatvar("jsonRoot=%s",jsonRoot.c_str());

	len_data = jsonRoot.length();
	dbg_formatvar("len_data=%d",len_data);
	//Json::StyledWriter styled_writer;
	//std::cout << styled_writer.write(root) << std::endl;

	//root.

	return jsonRoot;
}


//1010—进站验票 2010—出站验票 3010—取票验票
string ble::ble_checkTicket(ETYTKOPER operType)
{
	Json::Value json_temp;      // 临时对象，供如下代码使用
	//json_temp["version"] = Json::Value("01");//报文头-版本号
	//json_temp["encMode"] = Json::Value("00");//报文头-加密方式 0—明文，1—3DES 加密，2—国密
	//json_temp["length"] = Json::Value("0024");

	switch (operType)
	{
		case operEntry:
			json_temp["type"] = Json::Value("1010");
			break;
		case operExit:
			json_temp["type"] = Json::Value("2010");
			break;
		case operGetETicket:
			json_temp["type"] = Json::Value("3010");
			break;
	}


	Json::FastWriter fast_writer;
	string jsonRoot = fast_writer.write(json_temp);
	dbg_formatvar("jsonRoot=%s",jsonRoot.c_str());

	return jsonRoot;
}

int ble::ble_connect(uint8_t * address)
{
	dbg_formatvar("ble_connect");
	uint8_t p_cmd_recved[200] = {0};
	uint8_t p_data_send[256] = {0};
	ETBLEOPER operType = operConn;
	int length = 0;
	long msec = 0;
	length = 15;
	memcpy(p_data_send,"\x05\x06\x30\x0c\x00\x15\x00\x1a\x00\x30\x00\x30\x00\x00\x01",length);
	memcpy(p_data_send+length,address,6);
	length +=6;
	//memcpy(p_data_send+length,"\x00\x00\x06\x00\x06\x00\x00\x00\x20\x03\x00\x00\x18\xf0",14);
	//memcpy(p_data_send+length,"\x00\x00\x06\x00\x06\x00\x00\x00\x80\x0C\x00\x00\x18\xf0",14);
	memcpy(p_data_send+length,"\x00\x00\x06\x00\x06\x00\x00\x00\xd0\x07\x00\x00\x18\xf0",14);//0006 0006 0 2000
	//memcpy(p_data_send+length,"\x00\x00\x06\x00\x12\x00\x00\x00\x58\x02\x00\x00\x18\xf0",14);

	length +=14;
	dbg_formatvar("length1:%d",length);
	dbg_dumpmemory("p_data_send:",p_data_send,length);
	length = cmd_ble.cmd_send_ble(operType,p_cmd_recved,p_data_send,length,&msec);
	dbg_formatvar("length:%d",length);
	return length;

}


void ble::ble_close()
{
	dbg_formatvar("ble_close");
	uint8_t p_cmd_recved[200] = {0};
	uint8_t p_data_send[256] = {0};
	long msec = 0;
	ETBLEOPER operType = operClose;
	int length = 0;
	length = 13;
	memcpy(p_data_send,"\x05\x07\x30\x0c\x00\x15\x00\x04\x00\x00\x00\x13\x00",length);
	dbg_dumpmemory("p_data_send:",p_data_send,length);
	length = cmd_ble.cmd_send_ble(operType,p_cmd_recved,p_data_send,length,&msec);
	dbg_formatvar("length:%d",length);

}


//1010—进站验票 2010—出站验票 3010—取票验票
int ble::ble_getWriteBackData(ETYTKOPER operType,uint8_t *  p_base_add_info,uint16_t lengthData,uint8_t * revData)
{
	int length = 0;
	char type[5] = {0};
	int len_type = 4;

	memcpy(revData, "01", 2);//报文头
	length += 2;

	memcpy(revData + length, "00", 2);//加密方式
	length += 2;

	sprintf((char *)(revData + length),"%04d",lengthData + 6);
	length += 4;
    //g_Record.log_out(0, level_error,"opertype:%1d ,  lengthData =%d",operType,lengthData);
	switch (operType){
	    case operEntry:
            memcpy(type, "1010", len_type);//进站交易
            break;
        case operExit:
            memcpy(type, "2010", len_type);//出站交易
            break;
        case operGetETicket:
            memcpy(type, "3000", len_type);//取票受理
            break;
        case operUnloadTicketInfo:
            memcpy(type, "3010", len_type);//取票结果
            break;
	    case operClearEntry:
            memcpy(type, "4010", len_type);//清进站
            break;
        case operClearExit:
            memcpy(type, "4020", len_type);//清出站
            break;
        case operAddTicketResult:
            memcpy(type, "6020", len_type);//补票
            break;
        case operAddEntry:
            memcpy(type, "7010", len_type);//补进站
            break;
        case operAddExit:
            memcpy(type, "7020", len_type);//补出站
            break;
        case operPenaltyRes:
            memcpy(type, "8020", len_type);//罚款
            break;
        default:
            len_type=0;
            break;
	}

    memcpy(revData + length, type, len_type);//设置交易类型
    length+=len_type;

	memcpy(revData + length, "00", 2);//验票通过
	length += 2;
    //g_Record.log_buffer("blue data ",revData,length,level_error);

	memcpy(revData + length, p_base_add_info, lengthData);
	length += lengthData;

	return length;
}


int ble::ble_writeBack(ETYTKOPER operType, uint8_t *  p_eTicket_entry_info,uint8_t *  p_eTicket_exit_info,uint8_t *  p_eTicket_get_info, uint16_t lengthData)
{
	dbg_formatvar("ble_writeBack");
	int templen = 0;
	int length = 0;
	uint8_t p_cmd_recved[2000] = {0};
	uint8_t p_data_send[2000] = {0};
	uint8_t data[2000] = {0};
	long msec = 0;
	int dataLength = 0;
	int len_data = 0;
	int i=0;
	//ETBLEOPER operType = operWriteBack;
	//templen = dataLength;

	if(operType == operEntry)
	{
		dbg_formatvar("operEntry");
		dataLength = ble_getWriteBackData(operType,p_eTicket_entry_info,lengthData,data);
	}
	if(operType == operExit)
	{
		dbg_formatvar("operExit");
		dataLength = ble_getWriteBackData(operType,p_eTicket_exit_info,lengthData,data);
	}
	if(operType == operGetETicket)
	{
		dbg_formatvar("operGetETicket");
		dataLength = ble_getWriteBackData(operType,p_eTicket_get_info,lengthData,data);
	}



	//dbg_dumpmemory("data:",data,dataLength);
	uint8_t temp_recved[40];
	memset(temp_recved,0x00,sizeof(temp_recved));
	memcpy(temp_recved,"\x05\x05\x2c\x0b\x00\x15\x00\x1f\x00\x00\x00\x2c\x00\x00\x00\x14\x00\x0c\x01",19);
	temp_recved[11] = Api::ble_subscriberHandle;
	//int length = 0;
	int total = 0;
	int count = dataLength /19;
	//dbg_formatvar("count:%d",count);
	int remainder = dataLength %19;
	//dbg_formatvar("remainder:%d",remainder);
	if(remainder !=0)
		count++;

	for(i = 0;i<count;i++)
	{
		if(i == (count-1))
		{
			if(remainder != 0)
			{
				length = remainder + 1;//含0xFF
				temp_recved[7] = (uint8_t)(length + 11);
				temp_recved[15] = (uint8_t)length;
				temp_recved[19] = 0xFF;
				memcpy(&temp_recved[20],data+19*(count-1),remainder);
				temp_recved[20+remainder] = 0x00;
				memcpy(&p_data_send[40*i],temp_recved,(20+length));

				total +=20+length;
			}else{
				length = 20;
				temp_recved[19] = 0xFF;
				memcpy(&temp_recved[20],data+19*i,length);
				//memcpy(&temp_recved[19],data+20*i,length);
				temp_recved[39] = 0x00;
				memcpy(&p_data_send[40*i],temp_recved,40);
				total +=40;
			}

		}
		else
		{

			length = 20;
			if(i == 0)
				temp_recved[19] = 0xF1;
			else
				temp_recved[19] = 0xF2;
			memcpy(&temp_recved[20],data+19*i,length);
			//memcpy(&temp_recved[19],data+20*i,length);
			temp_recved[39] = 0x00;
			memcpy(&p_data_send[40*i],temp_recved,40);
			total +=40;
		}

	}
	len_data = total;

	for(i = 0;i < 1; i++)
	{
		length = cmd_ble.cmd_send_ble(operWriteBack,p_cmd_recved,p_data_send,len_data,&msec);
		//dbg_formatvar("length:%d",length);
		if(length == 0)
			break;
	}

	return length;
	//dbg_formatvar("len_data:%d",len_data);
	//memcpy(revData,p_cmd_recved,len_data);
	//dbg_dumpmemory("revData:",revData,len_data);

}

void  ble::ble_DataExchange(uint8_t * data,int dataLength,uint16_t& len_data,uint8_t * revData)
{
	dbg_dumpmemory("data:",data,dataLength);
	dbg_formatvar("ble_DataExchange");

	uint8_t p_cmd_recved[2000] = {0};
	uint8_t p_data_send[2000] = {0};
	int templen = 0;
	int length = 0;
	long msec = 0;
	ETBLEOPER operType = operDataEx;
	//templen = dataLength;

	//dbg_dumpmemory("p_cmd_recved:",p_cmd_recved,dataLength);
	uint8_t temp_recved[40];
	memset(temp_recved,0x00,sizeof(temp_recved));
	memcpy(temp_recved,"\x05\x05\x2c\x0b\x00\x15\x00\x1f\x00\x00\x00\x2a\x00\x00\x00\x14\x00\x0c\x01",19);
	//int length = 0;
	int total = 0;
	int count = dataLength /20;
	int remainder = dataLength %20;
	if(remainder !=0)
		count++;
	//dbg_formatvar("dataLength:%d | count:%d",dataLength,count);

	for(int i = 0;i<count;i++)
	{
		if(i == (count-1))
		{
			length = remainder;
			temp_recved[7] = (uint8_t)(length + 11);
			temp_recved[15] = (uint8_t)length;
			memcpy(&temp_recved[19],data+20*(count-1),length);
			temp_recved[19+length] = 0x00;
			memcpy(&p_data_send[40*i],temp_recved,(19+length+1));

			total +=19+length+1;
		}
		else
		{
			length = 20;
			memcpy(&temp_recved[19],data+20*i,length);
			temp_recved[39] = 0x00;
			memcpy(&p_data_send[40*i],temp_recved,40);
			total +=40;
		}

	}
	len_data = total;
	//dbg_formatvar("len_data:%d",len_data);
	//dbg_dumpmemory("p_data_send:",p_data_send,len_data);

	//ble_splitPackage(data,templen,p_data_send,length);
	//dbg_formatvar("length:%d",length);
	//dbg_dumpmemory("p_cmd_recved:",p_data_send,length);
	/*length = 19;
	memcpy(p_data_send,"\x05\x05\x2c\x0b\x00\x15\x00\x1a\x00\x00\x00\x2a\x00\x00\x00\x0F\x00\x0c\x01",length);



	memcpy(p_data_send+length,data,dataLength);
	length +=dataLength;
	p_data_send[length] = 0x00;
	length +=1;
	dbg_formatvar("length1:%d",length);
	dbg_dumpmemory("p_data_send:",p_data_send,length);*/
	len_data = cmd_ble.cmd_send_ble(operType,p_cmd_recved,p_data_send,len_data,&msec);
	//dbg_formatvar("len_data:%d",len_data);
	memcpy(revData,p_cmd_recved,len_data);
	//dbg_dumpmemory("revData:",revData,len_data);

}

void ble::ble_scan(/*uint16_t& len_data*/)
{
	int length;
	P_SCAN_ALL_BLE_INF p_sabi;
	SCAN_ALL_BLE_INF sabi;
	long msec = 0;
	//SCAN_SINGLE_BLE_INF ssbi[10];
	dbg_formatvar("ble_scan");
	uint8_t p_cmd_recved[1000] = {0};
	uint8_t p_data_send[20] = {0};
	uint16_t len_data_send = 13;
	ETBLEOPER operType = operScan;
	//dbg_formatvar("11111");
	//dbg_dumpmemory("p_data_send:",p_data_send,13);
	memcpy(p_data_send,"\x05\x0a\x30\x0c\x00\x15\x00\x04\x00\x00\x00\x00\x00",len_data_send);
	//dbg_dumpmemory("p_data_send:",p_data_send,13);
	//dbg_formatvar("cmd_send_ble");
	length = cmd_ble.cmd_send_ble(operType,p_cmd_recved,p_data_send,len_data_send,&msec);
	//dbg_formatvar("length=%d",length);

	//int count = format_bleInfo(p_cmd_recved, sabi.ssbi, length/sizeof(SCAN_SINGLE_BLE_INF));
	//dbg_formatvar("count=%d",count);
	/*for(int i=0;i<count;i++)
	{
		dbg_formatvar("ssbi[%d]:%s",i,sabi.ssbi[i].ble_name);
	}*/

	//len_data = count;

	//return sabi;

}

int ble::ble_scanStop()
{
	int length;
	dbg_formatvar("ble_scanStop");

	uint8_t p_cmd_recved[200] = {0};
	uint8_t p_data_send[11] = {0};
	uint16_t len_data_send = 11;
	ETBLEOPER operType = operScanStop;
	long msec = 0;

	memcpy(p_data_send,"\x05\x0d\x30\x0c\x00\x15\x00\x02\x00\x00\x01",11);
	length = cmd_ble.cmd_send_ble(operType,p_cmd_recved,p_data_send,len_data_send,&msec);
	//dbg_formatvar("length=%d",length);
	return length;

}


int ble::ble_getServiceHandle(/*uint8_t * serviceNo, uint8_t * handleNo*/)
{
	int ret = 0;
	ETBLEOPER operType = operGetHandle;
	int length;
	int i = 0;
	dbg_formatvar("ble_getServiceHandle");
	uint8_t p_cmd_recved[200] = {0};
	uint8_t p_data_send[35] = {0};
	long msec = 0;
	uint16_t len_data_send = 35;
	memcpy(p_data_send,"\x05\x02\x2C\x0B\x00\x15\x00\x1A\x00\x04\x00\x00\x00\x01\x00\xFF\xFF\x02\x00\xF0\x2A\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",len_data_send);
	//memcpy(p_data_send + 18, serviceNo, 2);
	//memcpy(p_data_send,"\x05\x4a\x30\x0c\x00\x15\x00\x04\x00\x0d\x00\x00\x00",13);//230400
	dbg_dumpmemory("ble_getServiceHandle(p_data_send):",p_data_send,len_data_send);

	for(i = 0;i < 1; i++)
	{
		length = cmd_ble.cmd_send_ble(operType,p_cmd_recved,p_data_send,len_data_send,&msec);
		dbg_formatvar("length:%d",length);
		if(length == 0)
			break;
	}

	return length;
}

void ble::ble_setSubscriber()
{
	int ret = 0;
	ETBLEOPER operType = operSetSub;
	int length;
	dbg_formatvar("ble_setSubscriber");
	uint8_t p_cmd_recved[200] = {0};
	uint8_t p_data_send[22] = {0};
	long msec = 0;
	uint16_t len_data_send = 22;
	memcpy(p_data_send,"\x05\x05\x2C\x0B\x00\x15\x00\x0D\x00\x00\x00\x43\x00\x00\x00\x02\x00\x0C\x01\x01\x00\x00",len_data_send);
	p_data_send[11] = Api::ble_subscriberHandle - 0x02;
	dbg_dumpmemory("ble_setSubscriber(p_data_send):",p_data_send,len_data_send);
	length = cmd_ble.cmd_send_ble(operType,p_cmd_recved,p_data_send,len_data_send,&msec);
	dbg_formatvar("length=%d",length);

}


void ble::ble_setbaudrate()
{
	int ret = 0;
    ETBLEOPER operType = operSetBaud;
	int length;
	dbg_formatvar("ble_setbaudrate");
	uint8_t p_cmd_recved[200] = {0};
	uint8_t p_data_send[13] = {0};
	long msec = 0;
	uint16_t len_data_send = 13;
	memcpy(p_data_send,"\x05\x4a\x30\x0c\x00\x15\x00\x04\x00\x0b\x00\x00\x00",13);//115200
	//memcpy(p_data_send,"\x05\x4a\x30\x0c\x00\x15\x00\x04\x00\x0d\x00\x00\x00",13);//230400
	//memcpy(p_data_send,"\x05\x4a\x30\x0c\x00\x15\x00\x04\x00\x0f\x00\x00\x00",13);//460800
	dbg_dumpmemory("p_data_send:",p_data_send,13);
	length = cmd_ble.cmd_send_ble(operType,p_cmd_recved,p_data_send,len_data_send,&msec);
	dbg_formatvar("length=%d",length);
	dbg_dumpmemory("p_cmd_recved:",p_cmd_recved,length);

}

int ble::format_bleInfo(uint8_t * p_cmd_recved/*,P_SCAN_SINGLE_BLE_INF p_ssbi*/,int sabi_max)
{
	int ssbi_count = 0;
	int i = 0;
	int m = 0;

	ssbi_count = sabi_max;

	//dbg_formatvar("sabi_max:%d",sabi_max);

	SCAN_SINGLE_BLE_INF ssbi_all[ssbi_count];
	SCAN_SINGLE_BLE_INF ssbi_temp;

	memset(ssbi_all, 0, sizeof(ssbi_all));

	//for(i=0;i<sabi_max;i++)
	{
		format_bleInfo(p_cmd_recved, ssbi_all[0]);
	}

	//dbg_dumpmemory("ssbi_all = ",&ssbi_all[0],51);

	ssbi_count = 1;

	//如果MAC地址不合法，直接跳过
	if((memcmp(ssbi_all[0].mac_address,"\x00\x00\x00\x00\x00\x00",6) ==0))
		return 0;

	//for (i=0;i<ssbi_count;i++)
	{
		for(m =0; m < 20;m++)
		{

			if((memcmp(ble::scanAllBleInf.ssbi[m].ble_name,ssbi_all[0].ble_name,16) ==0))
			{
				dbg_dumpmemory("ble_name = ",ble::scanAllBleInf.ssbi[m].ble_name,16);
				dbg_dumpmemory("scanAllBleInf.mac_address = ",ble::scanAllBleInf.ssbi[m].mac_address,6);
				dbg_dumpmemory("ssbi_all.mac_address = ",ssbi_all[0].mac_address,6);
				if(memcmp(ble::scanAllBleInf.ssbi[m].mac_address, ssbi_all[0].mac_address, 6) !=0)
				{
					memcpy(ble::scanAllBleInf.ssbi[m].mac_address, ssbi_all[0].mac_address, 6);
					dbg_dumpmemory("scanAllBleInf.mac_address2 = ",ble::scanAllBleInf.ssbi[m].mac_address,6);
					return 1;
				}else
				{
					return 0;
				}
			}

		}

		memcpy(&ble::scanAllBleInf.ssbi[Api::ble_count], ssbi_all, sizeof(SCAN_SINGLE_BLE_INF));
		//dbg_formatvar("insert succ");

		for(m =0; m < 20;m++)
		{
			if(memcmp(ble::scanAllBleInf.ssbi[m].mac_address,"\x00\x00\x00\x00\x00\x00",6) != 0)
			{
				dbg_formatvar("===========================================================");
				dbg_dumpmemory("scanAllBleInf.ssbi = ",&ble::scanAllBleInf.ssbi[m],51);
			}
		}

		Api::ble_count += 1;

	}

	if(Api::ble_count == 20)
        Api::ble_count = 0;

	//memcpy(ble::scanAllBleInf.ssbi + Api::ble_count, ssbi_all, sizeof(SCAN_SINGLE_BLE_INF) * ssbi_count);

	return 1;
}


void ble::format_bleInfo(uint8_t * p_ssbi_buffer, SCAN_SINGLE_BLE_INF& ssbi)
{
	memcpy(ssbi.head,p_ssbi_buffer,12);
	dbg_dumpmemory("head:",ssbi.head,12);

	// mac地址
	memcpy(ssbi.mac_address,p_ssbi_buffer+12,6);
	dbg_dumpmemory("mac_address:",ssbi.mac_address,6);

	// 未知信息1
	memcpy(ssbi.unknowInfo,p_ssbi_buffer+18,6);
	dbg_dumpmemory("unknowInfo:",ssbi.unknowInfo,6);
	// ble_name
	memcpy(ssbi.ble_name,p_ssbi_buffer+24,16);
	dbg_formatvar("ble_name:%s",ssbi.ble_name);
	dbg_dumpmemory("ble_name16:",ssbi.ble_name,16);
	// 结束信息
	memcpy(ssbi.endInfo,p_ssbi_buffer+40,11);
	dbg_dumpmemory("endInfo:",ssbi.endInfo,11);

}


int ble::format_bleInfo37(uint8_t * p_cmd_recved/*,P_SCAN_SINGLE_BLE_INF p_ssbi*/,int sabi_max)
{
	int ssbi_count = 0;
	int i = 0;
	int m = 0;

	ssbi_count = sabi_max;

	//dbg_formatvar("sabi_max:%d",sabi_max);

	SCAN_SINGLE_BLE_INF ssbi_all[ssbi_count];
	SCAN_SINGLE_BLE_INF ssbi_temp;

	memset(ssbi_all, 0, sizeof(ssbi_all));

	//for(i=0;i<sabi_max;i++)
	{
		format_bleInfo37(p_cmd_recved, ssbi_all[0]);
	}

	//dbg_dumpmemory("ssbi_all = ",&ssbi_all[0],51);

	ssbi_count = 1;

	//如果MAC地址不合法，直接跳过
	if((memcmp(ssbi_all[0].mac_address,"\x00\x00\x00\x00\x00\x00",6) ==0))
		return 0;

	//for (i=0;i<ssbi_count;i++)
	{
		for(m =0; m < 20;m++)
		{
			if((memcmp(ble::scanAllBleInf.ssbi[m].ble_name,ssbi_all[0].ble_name,16) ==0))
			{
				dbg_dumpmemory("ble_name = ",ble::scanAllBleInf.ssbi[m].ble_name,16);
				dbg_dumpmemory("scanAllBleInf.mac_address = ",ble::scanAllBleInf.ssbi[m].mac_address,6);
				dbg_dumpmemory("ssbi_all.mac_address = ",ssbi_all[0].mac_address,6);
				if(memcmp(ble::scanAllBleInf.ssbi[m].mac_address, ssbi_all[0].mac_address, 6) !=0)
				{
					memcpy(ble::scanAllBleInf.ssbi[m].mac_address, ssbi_all[0].mac_address, 6);
					dbg_dumpmemory("scanAllBleInf.mac_address2 = ",ble::scanAllBleInf.ssbi[m].mac_address,6);
					return 1;
				}else{
					return 0;
				}

			}
		}

		memcpy(&ble::scanAllBleInf.ssbi[Api::ble_count], ssbi_all, sizeof(SCAN_SINGLE_BLE_INF));
		for(m =0; m < 20;m++)
		{
			if(memcmp(ble::scanAllBleInf.ssbi[m].mac_address,"\x00\x00\x00\x00\x00\x00",6) != 0)
			{
				dbg_formatvar("===========================================================");
				dbg_dumpmemory("scanAllBleInf.ssbi = ",&ble::scanAllBleInf.ssbi[m],51);
			}
		}

		Api::ble_count += 1;

	}

	if(Api::ble_count == 20)
		Api::ble_count = 0;

	return 1;
}


void ble::format_bleInfo37(uint8_t * p_ssbi_buffer, SCAN_SINGLE_BLE_INF& ssbi)
{
	memcpy(ssbi.head,p_ssbi_buffer,12);
	//dbg_dumpmemory("head:",ssbi.head,12);

	// mac地址
	memcpy(ssbi.mac_address,p_ssbi_buffer+14,6);
	//dbg_dumpmemory("mac_address:",ssbi.mac_address,6);

	// 未知信息1
	memcpy(ssbi.unknowInfo,p_ssbi_buffer+20,6);
	//dbg_dumpmemory("unknowInfo:",ssbi.unknowInfo,6);
	// ble_name
	memcpy(ssbi.ble_name,p_ssbi_buffer+26,16);
	//dbg_formatvar("ble_name:%s",ssbi.ble_name);
	//dbg_dumpmemory("ble_name16:",ssbi.ble_name,16);
	// 结束信息
	memcpy(ssbi.endInfo,p_ssbi_buffer+42,11);
	//dbg_dumpmemory("endInfo:",ssbi.endInfo,11);

}


bool ble::getMacAddress(SCAN_ALL_BLE_INF sabi,uint8_t * revData,uint8_t * qr_passcode,int count)
{
	bool flag = false;
	uint8_t mac_address[7] = {0};
	dbg_formatvar("getMacAddress");
	//int count = sizeof(sabi)/sizeof(SCAN_SINGLE_BLE_INF);
	dbg_formatvar("count:%d",count);
	dbg_formatvar("qr_passcode:%s",qr_passcode+16);
	memcpy(qr_passcode,Publics::getUpper(qr_passcode),32);
	dbg_formatvar("qr_passcode|%s",qr_passcode+16);

	{
		for (uint32_t i=0; i<20; i++)
		{
			//dbg_formatvar("ble_name:%s",sabi.ssbi[i].ble_name);
			//dbg_dumpmemory("ble_name|",sabi.ssbi[i].ble_name,16);

			if (memcmp(sabi.ssbi[i].ble_name,qr_passcode+16,16)==0)
			//if (memcmp(sabi.ssbi[i].ble_name,qr_passcode+24,8)==0)//匹配小黄
			{
				dbg_formatvar("getMacAddress-succ");
				dbg_dumpmemory("mac_address = ",sabi.ssbi[i].mac_address,6);
				memcpy(mac_address,sabi.ssbi[i].mac_address,6);

				memcpy(ble::scanAllBleInf.ssbi[i].mac_address, "\x00\x00\x00\x00\x00\x00",6);
				memcpy(ble::scanAllBleInf.ssbi[i].ble_name, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",16);
				//dbg_dumpmemory("mac_address| = ",mac_address,6);
				flag = true;
				break;
			}
		}
	}
	memcpy(revData,mac_address,6);
	return flag;
}



void ble::format_ble_DataExchange(uint8_t * p_cmd_recved,int dataLength,uint16_t& len_data,uint8_t * revData)
{
	//uint8_t revData[2000] = {0};
	int bps_count = 0;
	dbg_formatvar("dataLength=%d",dataLength);
	//dbg_formatvar("BLE_PACKAGE_SINGLE=%d",sizeof(BLE_PACKAGE_SINGLE));
	bps_count = dataLength / (sizeof(BLE_PACKAGE_SINGLE));
	dbg_formatvar("bps_count=%d",sizeof(bps_count));
	int length = 0;
	int total = 0;

	BLE_PACKAGE_SINGLE eps_all[bps_count];
	BLE_PACKAGE_SINGLE eps_temp;
	int i = 0;

	memset(eps_all, 0, sizeof(eps_all));

	for(i = 0;i < bps_count;i++)
	{
		format_ble_DataExchange(p_cmd_recved + 39 * i, eps_all[i]);
	}

	for (i=0;i<bps_count;i++)
	{
		if((eps_all[i].flag == 0xF1)||(eps_all[i].flag == 0xF2))
		{
			memcpy(&revData[i*19],eps_all[i].data,19);
			total +=19;
		}
		else if(eps_all[i].flag == 0xFF)
		{
			length = eps_all[i].data_len[1];
			memcpy(&revData[i*19],eps_all[i].data,19);
			total +=length;
		}

	}

	len_data = total;
	dbg_formatvar("total=%d",total);
	dbg_dumpmemory("revData:",revData,total);

	//return revData;

}


void ble::format_ble_DataExchange(uint8_t * p_eps_buffer, BLE_PACKAGE_SINGLE& eps)
{
	memcpy(eps.head,p_eps_buffer,12);
	//dbg_dumpmemory("head:",eps.head,12);

	//长度
	memcpy(eps.data_len,p_eps_buffer+12,2);
	//dbg_dumpmemory("data_len:",eps.data_len,2);

	// 包标记
	eps.flag = p_eps_buffer[14];
	//dbg_formatvar("flag:%02x",eps.flag);

	// 数据域
	memcpy(eps.data,p_eps_buffer+15,19);
	//dbg_dumpmemory("data:",eps.data,19);

	// 结束信息
	memcpy(eps.endInfo,p_eps_buffer+34,5);
	//dbg_dumpmemory("endInfo:",eps.endInfo,5);

}

bool ble::is_LastPackege(uint8_t * p_buffer,int sumlen)
{
	dbg_formatvar("is_LastPackege");
	bool flag = false;
	uint8_t temp[39] = {0};
	int dataGroupCount = sumlen / 39;
	int remainer = sumlen % 39;
	if(remainer != 0)
		dataGroupCount = dataGroupCount + 1;
	//dbg_formatvar("dataGroupCount:%d",dataGroupCount);
	//dbg_formatvar("remainer:%d",remainer);
	if(remainer == 0)
		memcpy(temp,(p_buffer+(39*(dataGroupCount-1))),39);
	else
		memcpy(temp,(p_buffer+(39*(dataGroupCount-1))),remainer);

	//dbg_dumpmemory("temp:",temp,39);

	int length = temp[13];
	//dbg_formatvar("length:%d",length);

	if((temp[14] == 0xFF) && (temp[13+length] == 0x7D) && (temp[13+length+1] == 0x00))
		//if((temp[13+length-2] == 0x7D) && (temp[13+length-1] == 0x0D)&& (temp[13+length] == 0x0A))//小黄
		flag = true;
	return flag;

}


bool ble::ble_get_version(uint8_t * ble_version)
{
	dbg_formatvar("ble_get_version");
	bool flag = false;
	uint8_t p_cmd_recved[200] = {0};
	uint8_t p_data_send[256] = {0};
	long msec = 0;
	ETBLEOPER operType = operBleGetVersion;
	int length = 0;
	length = 3;
	memcpy(p_data_send, "\xF7\x01\x09", length);
	dbg_dumpmemory("ble_get_version p_data_send:", p_data_send, length);
	//g_Record.log_buffer("ble_get_version p_data_send() = ", p_data_send, length,level_error);
	length = cmd_ble.cmd_send_ble(operType, p_cmd_recved, p_data_send, length, &msec);
	if(length == 0)
		flag = true;
	dbg_formatvar("msec:%ld",msec);
	//g_Record.log_out(0, level_warning,"ble_get_version use time = [%ld]", msec);
	return flag;
}


void ble::setBauTest()
{
	cmd_ble.Serial_ListeningTest(115200);
}

int ble::ble_all_oper(ETYTKOPER operType,uint8_t * passcode, uint8_t * p_eTicket_info, int lengthData)
{
	int templen = 0;
	int length = 0;
	uint8_t p_cmd_recved[2000] = {0};
	uint8_t p_data_send[2000] = {0};
	uint8_t data[2000] = {0};
	uint8_t dataTemp[2000] = {0};
	int dataLength = 0;
	int len_data = 0;
	int i=0;
	int resetCount = 0;

	dataLength = ble_getWriteBackData(operType,p_eTicket_info,lengthData,data);

	uint8_t temp_receved[20];
	memset(temp_receved,0x00,sizeof(temp_receved));
	//int length = 0;
	int total = 0;
	int count = dataLength /19;
	int remainder = dataLength %19;
	long msec = 0;
	if(remainder !=0)
		count++;


	for(i = 0;i<count;i++)
	{
		if(i == (count-1))//最后一包
		{
			if(remainder != 0)
			{
				temp_receved[0] = 0xFF;
				memcpy(&temp_receved[1],data+19*(count-1),remainder);
				memcpy(&dataTemp[20*i],temp_receved,remainder+1);

				total +=remainder+1;
			}else{
				length = 20;
				temp_receved[0] = 0xFF;
				memcpy(&temp_receved[1],data+19*i,length);
				memcpy(&dataTemp[20*i],temp_receved,20);
				total +=20;
			}

		}
		else//非最后一包
		{

			length = 20;
			if(i == 0)
				temp_receved[0] = 0xF1;
			else
				temp_receved[0] = 0xF2;
			memcpy(&temp_receved[1],data+19*i,length);
			memcpy(&dataTemp[20*i],temp_receved,20);
			total +=20;
		}

	}

	dbg_dumpmemory("dataTemp:",dataTemp,total);
    dbg_formatvar("total = %d:",total);
	len_data = bleBaseProtocol::baseProtocol(passcode, dataTemp, total, p_data_send);

	//g_Record.log_buffer_debug("p_data_send() = ", p_data_send,len_data);
	dbg_dumpmemory("p_data_send:",p_data_send,len_data);


	//len_data = total;

	for(i = 0;i < 3; i++)
	{
		length = cmd_ble.cmd_send_ble(operWriteBack, p_cmd_recved, p_data_send, len_data, &msec);
		if((length == 87)&&(msec <= 1200))
		{
			{
				usleep(500 * 1000);
				continue;
			}
		}else if(((length == 88)||(length == 89)||(length == 138)||(length == 139)||(length == 111))&&(msec <= 1200)){
			usleep(500 * 1000);
			continue;
		}else if(((length == 71)/*||(length == 70)*/||(length == 72)||(length == 73))&&(msec <= 1500)){
			usleep(500 * 1000);
			continue;
		}else if((length == 61)){
			resetCount++;
			if(resetCount == 2)
				ble_reset(0);
			usleep(500 * 1000);
			continue;
		}else if ((length == -8)||(length == 70)){
			ble_reset(0);
			usleep(500 * 1000);
			break;
		}else if(length == 74){//碰到74直接退出，否则是否过闸判断不准
			break;
		}else{
			ble_reset(0);
			break;
		}
		/*dbg_formatvar("msec:%ld",msec);
		if(msec <= 10)*/
			ble_reset(0);
	}
	//FIXME: 其他线程打印日志
	//if(length == 0)		g_Record.log_out(0, level_error,"ble succ use counts:%d",(i + 1));
	//g_Record.log_out(0, level_error,"ble back result : %d",length);
	return length;

}


void ble::ble_handshake()
{
	dbg_formatvar("ble_handshake");
	uint8_t p_cmd_recved[200] = {0};
	uint8_t p_data_send[256] = {0};
	long msec = 0;
	ETBLEOPER operType = operClose;
	int length = 0;
	length = 3;
	memcpy(p_data_send,"\xF7\x01\x09",length);
	length = cmd_ble.cmd_send_ble(operType,p_cmd_recved,p_data_send,length,&msec);
	dbg_formatvar("length:%d",length);

}


void ble::ble_reset(int flag)
{
	dbg_formatvar("ble_reset");
	uint8_t p_cmd_recved[200] = {0};
	uint8_t p_data_send[256] = {0};
	ETBLEOPER operType;
	long msec = 0;
	if(flag == 0)
		operType = operBleReset;
	else
		operType = operBleDownloadReset;
	int length = 0;
	length = 6;
	memcpy(p_data_send,"\xF7\x04\x05\x11\x22\x33",length);
	//g_Record.log_buffer("ble_reset p_data_send() = ", p_data_send, 6);
	length = cmd_ble.cmd_send_ble(operType,p_cmd_recved,p_data_send,length,&msec);
	if(flag == 0)
		usleep(100 * 1000);//给100ms
	//g_Record.log_out(0, level_error,"ble_reset use time = [%ld]", msec);
}


void ble::ble_set_baud(int speed)
{
	dbg_formatvar("ble_set_baud");
	int length = 0;

	length = cmd_ble.Serial_ListeningBLE(speed);
	dbg_formatvar("length:%ld",length);
}

void ble::ble_send_test(uint8_t * sendData, int flag, int length)
{
	dbg_formatvar("ble_send_test");
	uint8_t p_cmd_recved[200] = {0};
	uint8_t p_data_send[256] = {0};
	long msec = 0;
	ETBLEOPER operType;
	if(flag == 1)
		operType = operBleTest;
	else
		operType = operBleTestOther;
	memcpy(p_data_send,sendData,length);
	length = cmd_ble.cmd_send_ble(operType,p_cmd_recved,p_data_send,length,&msec);
	dbg_formatvar("length:%d",length);

}


uint16_t ble::ble_err_exchange(int reslut)
{
	uint16_t ret = 0;

	return ret;
}
