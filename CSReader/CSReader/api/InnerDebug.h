#pragma once

#include <stdint.h>

class InnerDebug
{
public:
    InnerDebug(void);
    ~InnerDebug(void);


    typedef void (* P_FUNC_DEBUG)(uint8_t *, uint8_t *, uint16_t&);
    typedef struct
    {
        uint16_t			addr_func;
        P_FUNC_DEBUG		debug_fun_ptr;
    } DEBUG_FUNC_LIST;


    static void debug_calling(uint8_t debug_func_addr, uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data, bool& valid_app_addr);

	static void debug_rf_version(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void debug_test(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

    static void debug_test_kk(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void debug_test_once(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);
	//
	//iso14443A
	//
	static void debug_14443a_init(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void debug_14443a_request(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void debug_14443a_anticoll(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void debug_14443a_select(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void debug_14443a_read(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void debug_14443a_write(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void debug_14443a_auth(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);
	//
	// ul
	//
	static void debug_ul_request(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void debug_ul_anticoll(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void debug_ul_select(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void debug_ul_read(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void debug_ul_write(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void debug_ul_cWrite(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);
	//
	// cpu
	//
	static void debug_MifareProRst(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void debug_MifareProCom(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);
	//
	// sam
	//
	//static void debug_sam_init(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void debug_sam_setbaud(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void debug_sam_rst(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void debug_sam_cmd(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	//
	//others
	//
	static void debug_put_reader_data(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);
};

