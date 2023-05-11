#pragma once

#include <stdint.h>

class SysCmd
{
public:
	SysCmd(void);
	~SysCmd(void);

	typedef void (* P_FUNC_SYS)(uint8_t *, uint8_t *, uint16_t&);
	typedef struct
	{
		uint16_t			addr_func;
		P_FUNC_SYS			sys_fun_ptr;
	} SYS_FUNC_LIST;

	static void sys_calling(uint8_t debug_func_addr, uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data, bool& valid_app_addr);

	static void sys_reset_app(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);

	static void sys_reset_os(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data);
};
