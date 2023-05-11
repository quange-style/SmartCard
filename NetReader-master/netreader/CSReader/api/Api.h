#pragma once

#include <stdint.h>
#include <string.h>
#include "Structs.h"
#include "Declares.h"
#include "CmdSort.h"

#define API_VERSION		"\x00\x34"

typedef struct
{
	RETINFO ret;
	uint8_t region1[512];
	uint16_t len_region1;
	uint8_t region2[512];
	uint16_t len_region2;

}TRADELAST, * PTRADELAST;

typedef struct
{
	char dealType[2+1];
	char subType[2+1];
	char cardLogicNo[20+1];
	char centerCode[32+1];
	uint8_t qr_readbuf[512];
}ETICKETTRADELAST, * PETICKETTRADELAST;

class Api
{
public:
    static uint8_t current_device_type;
    static uint16_t current_device_id;
    static uint16_t current_station_id;
	static uint16_t current_city_id;
	static uint8_t bLocalInstitutionCode[8];
	static uint8_t qr_readbuf[512];
	static uint8_t	get_package_time[7];
	static int qr_readbuf_len;
	static uint8_t clear_qr_readbuf[512];
	static uint8_t last_qr_readbuf[512];
	static uint8_t qr_elect_analy_readbuf[512];//电子票扫码分析时保存到本地，在电子票更新时直接从缓存中取
	static uint8_t qr_passcode[32+1];

	static char	cCenterCode[32+1];// 保存进出闸的中心票号
	static char	qr_headversion[2+1];//二维码结构版本信息
	static ETICKETTRADELAST m_tdeticket;

	static uint8_t	testTmpData[14+1];

	static uint16_t ble_count;

	static int search_card_err;

	static uint8_t antFlag;
    static char operator_id[7];
    static uint8_t bom_shift_id;
    static uint8_t work_area;

	static uint8_t door_type;
	static uint8_t locked_entry_id[7];
	static uint8_t locked_exit_id[7];

	static long m_sam_seq_charge;		// SAM流水，充值时使用
	static long m_trade_val;			// 交易金额，充值或充值撤销时使用
	static int sysFlag;//奇数是苹果,偶数是安卓

	static uint8_t	ble_conFlag[4];
	static uint8_t	ble_conFlag2[4];
	static uint8_t	ble_closeFlag[4];
	static uint8_t ble_subscriberHandle;

	//static CmdSort cmd_ble;
	static bool API_IS_RELEASE;


public:
    Api(void);
    ~Api(void);

    typedef void (* P_FUNC_API)(uint8_t *, uint8_t *, uint16_t&);
    typedef struct
    {
        uint16_t	    addr_api;
        P_FUNC_API		api_fun_ptr;
    } API_FUNC_LIST;

    static void api_calling(uint8_t api_addr, uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data, bool& valid_app_addr);

	static void App_Addr_invalid(bool valid_app_addr, uint8_t * data_to_send, uint16_t& len_data);

	static char * sock_sam_id(int sock_id_from1);
	static char * sock_terminal_id(int sock_id_from1);

	static void get_current_device_info(char * p_station, char * p_dev_type, char * p_dev_id);

	static uint16_t organize_data_region(uint8_t * p_data, PRETINFO p_ret,
		uint16_t len_region1 = 0, void * p_region1 = NULL, uint16_t len_region2 = 0, void * p_region2 = NULL);

private:
    static char m_sam[8][17];
	static char m_tml[8][13];

	static TRADELAST m_tdLast;

    static uint16_t api_match_device(ETPDVC dvc_type_support);

    //
    // Common interface
    //
    static void Common_Initialize_Device(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Common_GetVersion(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Common_GetSamInfo(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Common_SetDegradeMode(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Common_SearchCard(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void GetFare(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Common_GetParamInfo(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Get_Reg_Value(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Get_Reg_Inf(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Reset_Reg_Inf(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Common_LastTrade(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Common_GetReaderTime(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Common_TimeSynchronization(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    //扫取三方授权码
	static void Common_GetThirdPayCode(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);
    //
    // 闸机接口
    //
    static void Gate_AisleModel(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Gate_EntryFlow(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Gate_ExitFlow(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Gate_OpenFlow(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	//TVM与BOM公共接口
    //扫电子票获取电子票信息
    static void common_getElectQrMsg(uint8_t *param_stack, uint8_t *data_to_send, uint16_t &len_data);
    //
    // TVM接口
    //
    static void Tvm_Ticket_Sale(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Tvm_SjtClear(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Tvm_TicketAnalyze(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Tvm_PurseDecrease(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Tvm_SvtIncrease1(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Tvm_SvtIncrease2(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Tvm_GetTopPrice(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Tvm_IGetTicket(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Tvm_GetTicketInfo(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Tvm_GetETicket_Sale(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Tvm_ThirdPayGetETicket(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Tvm_ThirdPay_Sale(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Tvm_UploadGetTicketInfo(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Tvm_FunActive1(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Tvm_FunActive2(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Tvm_GetETicketInfo(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    //
    // BOM接口
    //
    static void Bom_Login (uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Bom_TicketAnalyze(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Bom_Ticket_Sale(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Bom_TransactConfirm(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Bom_ExitSale(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Bom_TicketUpdate(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Bom_EticketUpdate(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Bom_PurseDecrease(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Bom_TicketDeffer(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Bom_TicketUnLock(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void Bom_DirectRefund(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Bom_SvtIncrease1(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Bom_SvtIncrease2(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Bom_GetTicketInfo(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Bom_FunActive1(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Bom_FunActive2(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Bom_Charge_Descind1(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Bom_Charge_Descind2(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Bom_Query_Policy_Penalty(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Bom_Query_OverTrip_Val(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);


	static void Bom_ETicketAnalyze(uint8_t* param_stack, uint8_t* data_to_send, uint16_t& len_data);

	static void bom_elect_ticket_analyze(uint8_t *param_stack, uint8_t *data_to_send, uint16_t &len_data);

	//
    // TCM接口
    //
    static void Tcm_GetTicketInfo(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	//
	// ES接口
	//
	static void Es_Ticket_Analyse(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Es_Ticket_Init(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Es_Ticket_Evaluate(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Es_Ticket_Destroy(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Es_Ticket_Recode(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Es_Ticket_Private(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Es_Ticket_Clear(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	//
	// 临时接口
	//
	static void Temp_interface_1(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Get_Configurations(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void Set_Configurations(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

};
