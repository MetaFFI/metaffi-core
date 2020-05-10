#include "xllr_plugin_interface_wrapper.h"
#include "scope_guard.hpp"
#include <boost/filesystem.hpp>


//--------------------------------------------------------------------
xllr_plugin_interface_wrapper::xllr_plugin_interface_wrapper(const std::string& plugin_filename)
{
	boost::filesystem::path fullpath(boost::filesystem::current_path());
	fullpath.append(plugin_filename);
	
	std::cout << "Loading Functions from: " << fullpath << std::endl;
	
	this->pload_runtime = this->load_func<void(char**,uint32_t*)>(fullpath.c_str(), "load_runtime");
	this->pfree_runtime = this->load_func<void(char**,uint32_t*)>(fullpath.c_str(), "free_runtime");
	this->pload_module = this->load_func<void(const char*, uint32_t, char**,uint32_t*)>(fullpath.c_str(), "load_module");
	this->pfree_module = this->load_func<void(const char*, uint32_t, char**,uint32_t*)>(fullpath.c_str(), "free_module");

}
//--------------------------------------------------------------------
void xllr_plugin_interface_wrapper::load_runtime(char** err, uint32_t* err_len)
{
	*err = nullptr;
	*err_len = 0;
	(*this->pload_runtime)(err, err_len);
}
//--------------------------------------------------------------------
void xllr_plugin_interface_wrapper::free_runtime(char** err, uint32_t* err_len)
{
	*err = nullptr;
	*err_len = 0;
	(*this->pfree_runtime)(err, err_len);
}
//--------------------------------------------------------------------
void xllr_plugin_interface_wrapper::load_module(const char* module, uint32_t module_len, char** err, uint32_t* err_len)
{
	*err = nullptr;
	*err_len = 0;
	(*this->pload_module)(module, module_len, err, err_len);
}
//--------------------------------------------------------------------
void xllr_plugin_interface_wrapper::free_module(const char* module, uint32_t module_len, char** err, uint32_t* err_len)
{
	*err = nullptr;
	*err_len = 0;
	(*this->pfree_module)(module, module_len, err, err_len);
}
//--------------------------------------------------------------------
/*
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