#include "xllr_plugin_interface_wrapper.h"

//--------------------------------------------------------------------
xllr_plugin_interface_wrapper::xllr_plugin_interface_wrapper(const std::string& plugin_filename)
{
//	std::string fullname("lib");
//	fullname += plugin_filename;
	auto func = boost::dll::import<void(char**,uint32_t*)>(plugin_filename, "load_runtime", boost::dll::load_mode::default_mode);
	printf("Loading: %s\n", plugin_filename.c_str());
}
//--------------------------------------------------------------------
void xllr_plugin_interface_wrapper::load_runtime(char** err, uint32_t* err_len)
{

}
//--------------------------------------------------------------------
void xllr_plugin_interface_wrapper::free_runtime(char** err, uint32_t* err_len)
{

}
//--------------------------------------------------------------------
/*
module_handle xllr_plugin_interface_wrapper::load_module(const char* module, uint32_t module_len, char** err, uint32_t* err_len)
{

}
//--------------------------------------------------------------------
void xllr_plugin_interface_wrapper::free_module(module_handle module, char** err, uint32_t* err_len)
{

}
//--------------------------------------------------------------------
void xllr_plugin_interface_wrapper::call(
		module_handle module,
		const char* func_name, uint32_t func_name_len,
		unsigned char* params, uint64_t params_len,
		unsigned char** out_params, uint64_t* out_params_len,
		char** out_ret, uint64_t* out_ret_len,
		uint8_t* is_error
)
{

}
//--------------------------------------------------------------------
module_handle xllr_plugin_interface_wrapper::call_lazy(
		const char* module_name, uint32_t module_name_len,
		const char* func_name, uint32_t func_name_len,
		unsigned char* params, uint64_t params_len,
		unsigned char** out_params, uint64_t* out_params_len,
		char** out_ret, uint64_t* out_ret_len,
		uint8_t* is_error
)
{

}
*/
//--------------------------------------------------------------------