#include "Api.h"
#include "Publics.h"
#include "ParamMgr.h"
#include "../link/linker.h"
#include "Errors.h"
#include "TicketUl.h"
#include "TicketSvt.h"
#include "Records.h"
#include <string.h>
#include "../link/myprintf.h"
#include "../link/SerialFile.h"
#include "../ble/ble.h"
#include "TimeCalcUtils.h"

uint8_t Api::current_device_type	= 0;
uint16_t Api::current_device_id		= 0;
uint16_t Api::current_station_id	= 0;
uint16_t Api::current_city_id	    = 0;
uint8_t Api::bLocalInstitutionCode[8] = {0};			    // 受理机构代码
uint8_t Api::door_type				= 3;
uint8_t Api::locked_entry_id[7]		= {0};
uint8_t Api::locked_exit_id[7]		= {0};
uint8_t	Api::qr_readbuf[512]  = {0};
int Api::qr_readbuf_len				= 0;
uint8_t	Api::clear_qr_readbuf[512]  = {0};
uint8_t	Api::get_package_time[7]  = {0};
//uint8_t	Api::last_qr_readbuf[512]  = {0};
uint8_t	Api::qr_passcode[32+1]  = {0};
char	Api::cCenterCode[32+1] = {0};// 保存进出闸的中心票号
char	Api::qr_headversion[2+1] = {0};// 二维码结构版本信息,全部变量需要维护

ETICKETTRADELAST Api::m_tdeticket				= {0, 0, 0};
uint8_t Api::antFlag = 0;
uint16_t Api::ble_count = 0;
int Api::search_card_err = 0;
TRADELAST Api::m_tdLast				= {{0}, {0}, 0, {0}, 0};
long Api::m_sam_seq_charge			= 0;			// SAM流水，充值时使用
long Api::m_trade_val				= 0;			// 充值金额，充值时使用
int Api::sysFlag = 0;//奇数是苹果,偶数是安卓

uint8_t	Api::testTmpData[14+1]  = {0};

uint8_t	Api::ble_conFlag[4]  = {0xD0,0x07,0x00,0x00};
uint8_t	Api::ble_conFlag2[4]  = {0x00,0x00,0x00,0x00};
uint8_t	Api::ble_closeFlag[4]  = {0x3E,0x00,0x00,0x00};
uint8_t Api::ble_subscriberHandle = 0x00;

char Api::m_sam[8][17] = {{0}};
char Api::m_tml[8][13] = {{0}};

uint8_t	Api::qr_elect_analy_readbuf[512]  = {0};

#define API_FILE_VERSION        "2021111701"
bool Api::API_IS_RELEASE=       true;//true isRelease,  false isDebug

CLoopList	g_LoopList;

Api::Api(void)
{

	for (uint8_t i=0;i<sizeof(m_sam)/sizeof(m_sam[0]);i++)
	{
		memset(m_sam[i], '0', sizeof(m_sam[i] - 1));
		memset(m_tml[i], '0', sizeof(m_tml[i] - 1));
	}
}

Api::~Api(void)
{
}

uint16_t Api::organize_data_region(uint8_t * p_data, PRETINFO p_ret, uint16_t len_region1, void * p_region1, uint16_t len_region2, void * p_region2)
{
    uint16_t pos = 0;

    memcpy(p_data + pos, p_ret, sizeof(RETINFO));
    pos += sizeof(RETINFO);

    memcpy(p_data + pos, &len_region1, sizeof(len_region1));
    pos += sizeof(len_region1);

    memcpy(p_data + pos, p_region1, len_region1);
    pos += len_region1;

    memcpy(p_data + pos, &len_region2, sizeof(len_region2));
    pos += sizeof(len_region1);

    memcpy(p_data + pos, p_region2, len_region2);
    pos += len_region2;

	memcpy(&m_tdLast.ret, p_ret, sizeof(RETINFO));
	m_tdLast.len_region1 = len_region1;
	memcpy(m_tdLast.region1, p_region1, len_region1);
	m_tdLast.len_region2 = len_region2;
	memcpy(m_tdLast.region2, p_region2, len_region2);

    return pos;
}

uint16_t Api::api_match_device(ETPDVC dvc_type_support)
{
    if (dvc_type_support != current_device_type)
        return ERR_DEVICE_SUPPROT;
    return 0;
}

void Api::get_current_device_info(char * p_station, char * p_dev_type, char * p_dev_id)
{
	char dev_info[16] = {0};
	sprintf(dev_info, "%04x%02x%03x", Api::current_station_id, Api::current_device_type, Api::current_device_id);
	memcpy(p_station, dev_info, 4);
	memcpy(p_dev_type, dev_info + 4, 2);
	memcpy(p_dev_id, dev_info + 6, 3);
}

void Api::api_calling(uint8_t api_addr, uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data, bool& valid_app_addr)
{
    static API_FUNC_LIST fun_list[] =
    {
        {0,		Common_Initialize_Device},
		{1,		Common_GetVersion},
        {2,		Common_GetSamInfo},
        {3,		Common_SetDegradeMode},
        {4,		GetFare},
		{5,		Common_GetParamInfo},
		{6,		Get_Reg_Value},
		{7,		Get_Reg_Inf},
		{8,		Reset_Reg_Inf},
		{9,		Common_LastTrade},
		{10,	Common_TimeSynchronization},
		{11,	Common_GetReaderTime},

        {73,	Common_GetThirdPayCode},

        {13,	Gate_AisleModel},
        {14,	Gate_EntryFlow},
        {15,	Gate_ExitFlow},
		{16,	Gate_OpenFlow},

        {20,	Tvm_Ticket_Sale},
        {21,	Tvm_SjtClear},
        {22,	Tvm_TicketAnalyze},
        {23,	Tvm_PurseDecrease},
		{24,	Tvm_SvtIncrease1},
		{25,	Tvm_SvtIncrease2},
		{26,    Tvm_GetTopPrice},
		{27,    Tvm_IGetTicket},
		{28,    Tvm_GetTicketInfo},
		//{29,    Tvm_GetEticketRecord},
		{29,	Tvm_GetETicket_Sale},
		{67,	Tvm_ThirdPay_Sale},
		{68,	Tvm_ThirdPayGetETicket},
		{69,	Tvm_UploadGetTicketInfo},
		{70,	Tvm_FunActive1},
        {71,	Tvm_FunActive2},
        {72,	Tvm_GetETicketInfo},

        {30,	Bom_Login},
        {31,	Bom_TicketAnalyze},
        {32,	Bom_GetTicketInfo},
        {33,	Bom_Ticket_Sale},
		{35,	Bom_TransactConfirm},
		{36,	Bom_ExitSale},
		{37,	Bom_TicketUpdate},
        {38,	Bom_PurseDecrease},
		{39,	Bom_DirectRefund},
        {40,	Bom_TicketDeffer},
		{41,	Bom_TicketUnLock},
		{42,	Bom_SvtIncrease1},
		{43,	Bom_SvtIncrease2},
		{44,	Bom_FunActive1},
		{45,	Bom_FunActive2},
		{46,	Bom_Charge_Descind1},
		{47,	Bom_Charge_Descind2},
		{48,	Bom_Query_Policy_Penalty},
		{49,	Bom_Query_OverTrip_Val},

        {50,	Tcm_GetTicketInfo},

		{51,	Bom_EticketUpdate},
		{61,	Bom_ETicketAnalyze},//获取二维码信息并进行分析  计算超时超乘情况

		{253,	Get_Configurations},
		{254,	Set_Configurations},
		{255,	Temp_interface_1}
    };

    for (uint16_t i=0; i<sizeof(fun_list)/sizeof(fun_list[0]); i++)
    {
        if (api_addr == fun_list[i].addr_api)
        {
            fun_list[i].api_fun_ptr(param_stack, data_to_send, len_data);
			valid_app_addr = true;
            return;
        }
    }

	// 不存在的
	valid_app_addr = false;
}

char * Api::sock_sam_id(int sock_id_from1)
{
    return (char *)m_sam[sock_id_from1];
}

char * Api::sock_terminal_id(int sock_id_from1)
{
	return (char *)m_tml[sock_id_from1];
}

void Api::Common_Initialize_Device(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret				= {0};
	uint8_t sam_status[8]	= {0xFF};

	do
    {
		////////////////////////
		///////默认城市代码/////
		////////////////////////
		param_stack[5] = 0x55;
		param_stack[6] = 0x10;

		////////////////////////
		///////默认机构代码/////
		////////////////////////
		//memcpy(bLocalInstitutionCode,"\x02\x19\x55\x10\x00\x00\x00\x02",8);
		memcpy(bLocalInstitutionCode,"\x02\x19\x55\x10\xFF\xFF\xFF\xFF",8);

		memset(locked_entry_id, 0, sizeof(locked_entry_id));
		memset(locked_exit_id, 0, sizeof(locked_exit_id));

        current_station_id = (uint16_t)((param_stack[0] << 8) + param_stack[1]);
        current_device_type = param_stack[2];
        current_device_id = (uint16_t)((param_stack[3] << 8) + param_stack[4]);
		current_city_id = (uint16_t)((param_stack[5] << 8) + param_stack[6]);
		//memcpy(bLocalInstitutionCode,&param_stack[7],8);

		//首先清掉二维码数据//
		memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码

		//dbg_formatvar("current_station_id=%04X",current_station_id);
		//dbg_formatvar("current_device_type:%02X",current_device_type);
		//dbg_formatvar("current_device_id:%03X",current_device_id);
		//dbg_formatvar("current_city_id:%04X",current_city_id);
		//dbg_dumpmemory("bLocalInstitutionCode:",bLocalInstitutionCode,8);

		memcpy(operator_id, "000000", 6);
		bom_shift_id = 0;
		work_area = 1;
		door_type = 3;


		if (current_device_type != 0x09)
		{
			ret.wErrCode = g_Parameter.load_all_prms();
			if (ret.wErrCode != 0)	break;

			ret.wErrCode = g_Parameter.device_registed(current_station_id, current_device_type, current_device_id);
			if (ret.wErrCode != 0)	break;

			TicketBase::load_confirm_from_file();
		}

		memset(sam_status, 0xFF, sizeof(sam_status));
        if (sam_init(SAM_SOCK_1, m_sam[SAM_SOCK_1], m_tml[SAM_SOCK_1]) == 0)
		{
			if (g_Parameter.sam_counterpart(SAM_SOCK_1, m_sam[SAM_SOCK_1], param_stack, param_stack[2], param_stack + 3, true))
				sam_status[SAM_SOCK_1] = 0x00;
			else
				sam_status[SAM_SOCK_1] = 0x01;
		}

		if (sam_init(SAM_SOCK_2, m_sam[SAM_SOCK_2], m_tml[SAM_SOCK_2]) == 0)
		{
			if (g_Parameter.sam_counterpart(SAM_SOCK_2, m_sam[SAM_SOCK_2], param_stack, param_stack[2], param_stack + 3, true))
				sam_status[SAM_SOCK_2] = 0x00;
			else
				sam_status[SAM_SOCK_2] = 0x01;
		}

		if (sam_init(SAM_SOCK_3, m_sam[SAM_SOCK_3], m_tml[SAM_SOCK_3]) == 0)
		{
			if (g_Parameter.sam_counterpart(SAM_SOCK_3, m_sam[SAM_SOCK_3], param_stack, param_stack[2], param_stack + 3, true))
				sam_status[SAM_SOCK_3] = 0x00;
			else
				sam_status[SAM_SOCK_3] = 0x01;
		}

		if (sam_init(SAM_SOCK_4, m_sam[SAM_SOCK_4], m_tml[SAM_SOCK_4]) == 0)
		{
			if (g_Parameter.sam_counterpart(SAM_SOCK_4, m_sam[SAM_SOCK_4], param_stack, param_stack[2], param_stack + 3, true))
				sam_status[SAM_SOCK_4] = 0x00;
			else
				sam_status[SAM_SOCK_4] = 0x01;
		}

		g_Parameter.device_initialize();
		g_Record.delete_file_overdue();
		g_Parameter.delete_prm_outof_config();

		ubeep(50);
		//FIXME:获取蓝牙版本信息接口有问题，总是报超时
		ble::ble_reset(0);
		uint8_t sendData[256] = {0};
		ble::ble_get_version(sendData);
    } while (0);

    len_data = organize_data_region(data_to_send, &ret, (uint16_t)sizeof(sam_status), sam_status);

	g_Record.log_out(ret.wErrCode, level_disaster,
		"Common_Initialize_Device(station=%04X,type=%02X,device=%03X)=(%04X,%02X) version = (%s)",
		current_station_id, current_device_type, current_device_id, ret.wErrCode, ret.bNoticeCode,API_FILE_VERSION);
}

void Api::Common_GetSamInfo(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret			= {0};
	bool flags[8]		= {true, true, true, false, false, false, false, false};
	uint8_t dev_inf[5]	= {0};

	SAMSTATUS sst[8];

    memset(sst, 0, sizeof(sst));

	memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码

	ret.wErrCode = g_Parameter.device_initialized();
	if (ret.wErrCode == 0)
	{
		dev_inf[0] = (uint8_t)((current_station_id >> 8) & 0xFF);
		dev_inf[1] = (uint8_t)(current_station_id & 0xFF);
		dev_inf[2] = current_device_type;
		dev_inf[3] = (uint8_t)((current_device_id >> 8) & 0xFF);
		dev_inf[4] = (uint8_t)(current_device_id & 0xFF);

		for (int i=0;i<8;i++)
		{
			sst[i].bSAMStatus = 0xFF;
			if (i < 4 && sam_init(i, m_sam[i], m_tml[i]) == 0)
			{
				if (g_Parameter.sam_counterpart(i, m_sam[i], dev_inf, dev_inf[2], dev_inf + 3, flags[i]))
					sst[i].bSAMStatus = 0x00;
				else
					sst[i].bSAMStatus = 0x01;

				memcpy(sst[i].cSAMID, m_sam[i], sizeof(sst[i].cSAMID));
			}
			else
			{
				memset(sst[i].cSAMID, '0', sizeof(sst[i].cSAMID));
			}
		}
	}

    len_data = organize_data_region(data_to_send, &ret, sizeof(sst), sst, 0, NULL);
}

void Api::Common_GetVersion(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret			= {0};
    uint8_t		verApiFileTemp[5]={0};
    char  verApiStr[10]={0};
	READERVERSION ver;

	memset(&ver, 0, sizeof(ver));
	memcpy(verApiStr, API_FILE_VERSION, 10);
    Publics::string_to_bcds(verApiStr,10,verApiFileTemp,5);
	memcpy(ver.verApi, API_VERSION, 2);
	memcpy(ver.verApiFile, verApiFileTemp, 5);

	if (rf_version((unsigned char *)(&ver.verRfDev)) == 0)
		memcpy(ver.verRfFile, "\x20\x18\x06\x63\x22", 5);

	if (sam_version((unsigned char *)(&ver.verSamDev)) == 0)
		memcpy(ver.verSamFile, "\x20\x13\x07\x06\x01", 5);

	len_data = organize_data_region(data_to_send, &ret, sizeof(ver), &ver);
}

void Api::Common_SetDegradeMode(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret = {0};
	ret.wErrCode = g_Parameter.device_initialized();
	if (ret.wErrCode == 0)
	{
		g_Parameter.modify_degrade(param_stack[0], param_stack + 1, param_stack + 8, param_stack[10]);
	}

	len_data = organize_data_region(data_to_send, &ret);

	g_Record.log_out(ret.wErrCode, level_disaster,
		"Degrade mode info:type=%d,time=%02x%02x%02x%02x%02x%02x%02x,station=%02x%02x,flag=%d",
		param_stack[0], param_stack[1], param_stack[2], param_stack[3], param_stack[4], param_stack[5],
		param_stack[6], param_stack[7], param_stack[8],param_stack[9],param_stack[10]);
}

void Api::Common_SearchCard(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret = {0};
    len_data = organize_data_region(data_to_send, &ret);
}

void Api::GetFare(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret = {0};
    long fare_trade = 0;
    uint8_t ticket_type[2];
    uint16_t entry_station;

	ret.wErrCode = g_Parameter.device_initialized();
	if (ret.wErrCode == 0)
	{
		memcpy(&ticket_type, param_stack, 2);
		entry_station = (uint16_t)((param_stack[2] << 8) + param_stack[3]);

		ret.wErrCode = g_Parameter.query_fare(CmdSort::m_time_now, ticket_type, entry_station, CmdSort::m_time_now, &fare_trade);
	}

	len_data = organize_data_region(data_to_send, &ret, sizeof(fare_trade), &fare_trade);
}

void Api::Common_GetParamInfo(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret							= {0};
	char sz_prm_name[_POSIX_PATH_MAX]	= {0};

	//ret.wErrCode = g_Parameter.device_initialized();
	//if (ret.wErrCode == 0)
	{
		uint16_t prm_type = (uint16_t)((param_stack[0] << 8) + param_stack[1]);

		if((prm_type == 0x0001) || (prm_type == 0x0002)|| (prm_type == 0x0003)|| (prm_type == 0x0004)|| (prm_type == 0x0010)|| (prm_type == 0x0012)|| (prm_type == 0x1002))
			ret.wErrCode = g_Parameter.get_prm_info_itp(prm_type, sz_prm_name);
		else
			ret.wErrCode = g_Parameter.get_prm_info(prm_type, sz_prm_name);
	}

	len_data = organize_data_region(data_to_send, &ret, strlen(sz_prm_name), sz_prm_name);
}

void Api::Bom_FunActive1(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret         = {0};
    char sz_temp[32]    = {0};
    uint8_t free_num[8] = {0};
	SAM_ACTIVE sact;
	uint16_t len_act	= 0;

    memset(&sact, 0, sizeof(sact));

	ret.wErrCode = g_Parameter.device_initialized();
	if (ret.wErrCode == 0)
	{
		if (sam_metro_function_active(SAM_SOCK_1, 0, free_num) < 0)
		{
			ret.wErrCode = ERR_SAM_RESPOND;
		}
		else
		{
			len_act	= sizeof(SAM_ACTIVE);

			memcpy(sact.cMsgType, "55", 2);
			memcpy(sact.dtTime, CmdSort::m_time_now, 7);

			get_current_device_info(sact.cStation, sact.cDevType, sact.cDevId);
			memcpy(sact.cSamId, m_sam[0], 16);

			sprintf(sz_temp, "%02X%02X%02X%02X%02X%02X%02X%02X",
				free_num[0], free_num[1], free_num[2], free_num[3], free_num[4], free_num[5], free_num[6], free_num[7]);
			memcpy(sact.cFreedom, sz_temp, 16);
		}
	}

	len_data = organize_data_region(data_to_send, &ret, len_act, &sact, 0, NULL);

	g_Record.log_out(ret.wErrCode, level_error, "Bom_FunActive1()=(%04X,%02X)", ret.wErrCode, ret.bNoticeCode);

}

void Api::Bom_FunActive2(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret         = {0};
	uint8_t mac[8]		= {0};

	ret.wErrCode = g_Parameter.device_initialized();
	if (ret.wErrCode == 0)
	{
		Publics::string_to_hexs((char *)(param_stack + 38), 16, mac, 8);

		if (sam_metro_function_active(SAM_SOCK_1, 1, mac) < 0)
		{
			ret.wErrCode = ERR_SAM_RESPOND;
		}
	}

	len_data = organize_data_region(data_to_send, &ret, 0, NULL, 0, NULL);

	g_Record.log_out(ret.wErrCode, level_error, "Bom_FunActive2()=(%04X,%02X)", ret.wErrCode, ret.bNoticeCode);
}

void Api::Tvm_FunActive1(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{

    RETINFO ret         = {0};
    char sz_temp[32]    = {0};
    uint8_t free_num[8] = {0};
    SAM_ACTIVE sact;
    uint16_t len_act	= 0;

    memset(&sact, 0, sizeof(sact));

    ret.wErrCode = g_Parameter.device_initialized();
    if (ret.wErrCode == 0)
    {
        if (sam_metro_function_active(SAM_SOCK_1, 0, free_num) < 0)
        {
            ret.wErrCode = ERR_SAM_RESPOND;
        }
        else
        {
            len_act	= sizeof(SAM_ACTIVE);

            memcpy(sact.cMsgType, "55", 2);
            memcpy(sact.dtTime, CmdSort::m_time_now, 7);

            get_current_device_info(sact.cStation, sact.cDevType, sact.cDevId);
            memcpy(sact.cSamId, m_sam[0], 16);

            sprintf(sz_temp, "%02X%02X%02X%02X%02X%02X%02X%02X",
                    free_num[0], free_num[1], free_num[2], free_num[3], free_num[4], free_num[5], free_num[6], free_num[7]);
            memcpy(sact.cFreedom, sz_temp, 16);
        }
    }

    len_data = organize_data_region(data_to_send, &ret, len_act, &sact, 0, NULL);

    g_Record.log_out(ret.wErrCode, level_error, "Tvm_FunActive1()=(%04X,%02X)", ret.wErrCode, ret.bNoticeCode);

}

void Api::Tvm_FunActive2(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
    RETINFO ret         = {0};
    uint8_t mac[8]		= {0};

    ret.wErrCode = g_Parameter.device_initialized();
    if (ret.wErrCode == 0)
    {
        Publics::string_to_hexs((char *)(param_stack + 38), 16, mac, 8);

        if (sam_metro_function_active(SAM_SOCK_1, 1, mac) < 0)
        {
            ret.wErrCode = ERR_SAM_RESPOND;
        }
    }

    len_data = organize_data_region(data_to_send, &ret, 0, NULL, 0, NULL);

    g_Record.log_out(ret.wErrCode, level_error, "Tvm_FunActive2()=(%04X,%02X)", ret.wErrCode, ret.bNoticeCode);
}

void Api::Get_Reg_Value(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret         = {0};
	uint32_t reg_val	= 0;

	len_data = organize_data_region(data_to_send, &ret, 4, &reg_val, 0, NULL);
}

void Api::Get_Reg_Inf(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret				= {0};
	uint32_t reg_inf[256]	= {0};

	len_data = organize_data_region(data_to_send, &ret, 1024, reg_inf, 0, NULL);
}

void Api::Reset_Reg_Inf(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret         = {0};

	len_data = organize_data_region(data_to_send, &ret, 0, NULL, 0, NULL);
}

void Api::Common_LastTrade(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	len_data = organize_data_region(data_to_send, &m_tdLast.ret, m_tdLast.len_region1, m_tdLast.region1, m_tdLast.len_region2, m_tdLast.region2);
}

void Api::Temp_interface_1(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret					= {0};
	TicketBase * p_ticket		= NULL;
	TICKET_PARAM ticket_prm;

	do
	{
		ret.wErrCode = TicketBase::search_card(&p_ticket);
		if (ret.wErrCode != 0)	break;

		ret = p_ticket->read_card(operSale, ticket_prm);
		if (ret.wErrCode != 0)	break;

		ret = p_ticket->data_version_upgrade();

	}
	while (0);

	len_data = organize_data_region(data_to_send, &ret);
}

void Api::Get_Configurations(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret			= {0};
	LOCCONFIG loc_conf	= {0};

	g_Record.get_record_param(loc_conf.trade_save_days, loc_conf.log_save_days, loc_conf.log_level);

	len_data = organize_data_region(data_to_send, &ret, (uint16_t)(sizeof(loc_conf)), &loc_conf);
}

void Api::Set_Configurations(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret			= {0};
	LOCCONFIG loc_conf	= {0};

	memcpy(&loc_conf.trade_save_days, param_stack, 2);
	memcpy(&loc_conf.log_save_days, param_stack + 2, 2);
	memcpy(&loc_conf.log_level, param_stack + 4, 2);

	//todo:可写本地始终保存状态日志状态，末尾添加crc校验防止串改
	ret.wErrCode = g_Record.set_record_param(loc_conf.trade_save_days, loc_conf.log_save_days, loc_conf.log_level);

	len_data = organize_data_region(data_to_send, &ret);

}
void Api::App_Addr_invalid(bool valid_app_addr, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret			= {0};

	if (!valid_app_addr)
	{
		ret.wErrCode = ERR_APP_NOT_EXSIT;

		len_data = organize_data_region(data_to_send, &ret);
	}
}

void Api::Common_TimeSynchronization(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	uint8_t time_now[7] = {0};
	//uint8_t ticket_type[2];
	//uint16_t entry_station;

	memcpy(time_now, param_stack, 7);
	//TODO:同步读写器系统时间 系统时间设置指令为：date -s "2021-02-02 14:33:01"
	len_data = organize_data_region(data_to_send, &ret, 7, time_now);

	g_Record.log_out(ret.wErrCode, level_error,
		"Common_TimeSynchronization=(%02X%02X%02X%02X%02X%02X%02X)",time_now);
}


void Api::Common_GetReaderTime(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	uint8_t time_now[7] = {0};
	//uint8_t ticket_type[2];
	//uint16_t entry_station;

	memcpy(time_now, CmdSort::m_time_now, 7);

	len_data = organize_data_region(data_to_send, &ret, 7, time_now);

	g_Record.log_out(ret.wErrCode, level_error,
		"Common_GetReaderTime=(%02X%02X%02X%02X%02X%02X%02X)",time_now);
}
void Api::Common_GetThirdPayCode(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{

    RETINFO ret = {0};
    TicketBase * p_ticket		= NULL;

    TICKET_PARAM ticket_prm		= {0};
    TICKET_DATA ticket_data;

    do
    {
        ret.wErrCode = g_Parameter.device_initialized();
        if (ret.wErrCode != 0) break;

        ret.wErrCode = TicketBase::search_card(&p_ticket, NULL, NULL, NULL, operThirdGetETicket);
        g_Record.log_out(0, level_disaster,"ret.wErrCode=%d",ret.wErrCode);
        if (ret.wErrCode != 0) break;

        ret = p_ticket->read_card(operThirdGetETicket, ticket_prm);
        if (ret.wErrCode != 0)	break;

        p_ticket->get_ticket_infos(ticket_data);
        g_Record.log_out(0, level_disaster,"third_qr_info:",ticket_data.third_qr_info,18);

        //检查是否重复上送授权码  //todo：没有校验码体规则
        if(ticket_data.qr_flag == 0x01)
        {
            dbg_formatvar("third_qr_info:%s",ticket_data.third_qr_info);
            g_Record.log_out(0, level_disaster,"third_qr_info:",ticket_data.third_qr_info,18);
            if(g_LoopList.QueryList((char *)ticket_data.third_qr_info,operThirdGetETicket))
            {
                dbg_formatvar("this is the same qr code");
                //g_Record.log_out(0, level_error,"QueryList:clear the same qr");
                ret.wErrCode = ERR_CARD_NONE;
                memset(Api::qr_readbuf,0x00,sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
                break;
            }
        }

        dbg_dumpmemory("third_qr_info:",ticket_data.third_qr_info,18);
        g_LoopList.UpdateList((char *)ticket_data.third_qr_info,operThirdGetETicket);

    }while (0);

    if (ret.wErrCode)
    {
        if (ret.wErrCode == ERR_CARD_READ)
        {
            ret.wErrCode = ERR_CARD_NONE;
            ret.bNoticeCode = 0;
        }
    }

    //组包给上位机
    if(ret.wErrCode != ERR_SUCCEED){
        len_data = organize_data_region(data_to_send, &ret, 0, NULL);
    }
    else{
        len_data = organize_data_region(data_to_send, &ret, 64, ticket_data.third_qr_info);
    }

	//不管是否成功都置空前一次的扫码
	memset(Api::qr_readbuf, 0x00, sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
    memset(Api::clear_qr_readbuf, 0x00, sizeof(Api::clear_qr_readbuf));//交易结束，不管是否成功，再次清掉二维码

    //日志记录
    if(ret.wErrCode != ERR_CARD_NONE){
        g_Record.log_out(0, level_disaster,"Tvm_ThirdPayGetETicket = %s",ticket_data.third_qr_info);
    }
}



void Api::common_getElectQrMsg(uint8_t *param_stack, uint8_t *data_to_send, uint16_t &len_data)
{
    RETINFO ret = { 0 };
    TicketBase* p_ticket = NULL;
    TICKET_PARAM ticket_prm = { 0 };
    TICKET_DATA ticket_data;
    BOMESJTRETURN ReturnSJT = { 0 };
    BOMESVTRETURN ReturnSVT = { 0 };
    uint16_t len_eticket = 0;

    struct timeval start;
    struct timeval end;
    long msec = 0;
    gettimeofday(&start, NULL);

	//组分析数据包
    ret.bRfu[0] = 0x11;
    ret.bRfu[1] = 0x00;

    do
    {
        ret.wErrCode = g_Parameter.device_initialized();
        if (ret.wErrCode != 0)
            break;

        if (TicketBase::check_confirm_his(1440))
        {
            ret.bNoticeCode = NTC_MUST_CONFIRM;
            break;
        }

        memset(&ticket_data, 0, sizeof(ticket_data));

        //1，寻卡
        ret.wErrCode = TicketBase::search_card(&p_ticket, NULL, NULL, NULL, operGetETicket);
        if (ret.wErrCode != 0)	break;
        //2，读卡
        ret = p_ticket->read_card(operAnalyse, ticket_prm);
        if (ret.wErrCode != 0)	break;
        p_ticket->get_ticket_infos(ticket_data);
    	if (memcmp(ticket_data.qr_ticketdata.subType, "00", 2) == 0)
    	{//电子单程票
        	len_eticket = sizeof(ReturnSJT);
        	p_ticket->transfer_edata_for_out(operESJT, &ReturnSJT);
        	len_data = organize_data_region(data_to_send, &ret, len_eticket, &ReturnSJT, 0, NULL);
    	}
    	else
    	{//电子储值票
        	len_eticket = sizeof(ReturnSVT);
       		p_ticket->transfer_edata_for_out(operESVT, &ReturnSVT);
       		len_data = organize_data_region(data_to_send, &ret, len_eticket, &ReturnSVT, 0, NULL);
    	}

    } while (0);


    if (ret.wErrCode != ERR_SUCCEED)
    {//只要异常，数据域长度返回为0
		len_data = organize_data_region(data_to_send, &ret, len_eticket, &ReturnSJT, 0, NULL);
    }
	//不管是否成功都置空前一次的扫码
	memset(Api::qr_readbuf, 0x00, sizeof(Api::qr_readbuf));//交易结束，不管是否成功，再次清掉二维码
    memset(Api::clear_qr_readbuf, 0x00, sizeof(Api::clear_qr_readbuf));//交易结束，不管是否成功，再次清掉二维码

    gettimeofday(&end, NULL);
    msec = TimeCalcUtils::calcMsec(start, end);
    g_Record.log_out(ret.wErrCode, level_error,
                     "common_getElectQrMsg(lid=%s,work_area=%d)=(%04X,%02X) use time=[%ld]",
                     ticket_data.logical_id, work_area, ret.wErrCode, ret.bNoticeCode, msec);
}



