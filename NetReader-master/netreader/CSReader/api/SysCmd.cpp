// SysCmd.cpp

#include "SysCmd.h"
#include "Api.h"
#include "CmdSort.h"

SysCmd::SysCmd(void)
{

}

SysCmd::~SysCmd(void)
{

}

void SysCmd::sys_calling(uint8_t sys_func_addr, uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data, bool& valid_app_addr)
{
	static SYS_FUNC_LIST fun_list[] =
	{
		{0x0001, sys_reset_app},
		{0x0002, sys_reset_os}

	};

	for (uint16_t i=0; i<sizeof(fun_list)/sizeof(fun_list[0]); i++)
	{
		if (sys_func_addr == fun_list[i].addr_func)
		{
			fun_list[i].sys_fun_ptr(param_stack, data_to_send, len_data);
			valid_app_addr = true;
			return;
		}
	}

	valid_app_addr = false;
}

void SysCmd::sys_reset_app(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	CmdSort::m_flag_app_run = false;
	len_data = Api::organize_data_region(data_to_send, &ret);

}

void SysCmd::sys_reset_os(uint8_t * param_stack, uint8_t * data_to_send, uint16_t& len_data)
{
	RETINFO ret = {0};
	CmdSort::m_sys_reboot = true;
	len_data = Api::organize_data_region(data_to_send, &ret);

}
