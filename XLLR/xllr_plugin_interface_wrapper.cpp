#include "xllr_plugin_interface_wrapper.h"
#include "scope_guard.hpp"
#include <boost/filesystem.hpp>

//--------------------------------------------------------------------
xllr_plugin_interface_wrapper::xllr_plugin_interface_wrapper(const std::string& plugin_filename)
{
	boost::filesystem::path fullpath(boost::filesystem::current_path());
	fullpath.append(plugin_filename);
	
	std::cout << "Loading Functions from: " << fullpath << std::endl;
	
	this->pload_runtime = std::make_shared<boost::dll::detail::import_type<void(char**,uint32_t*)>::type>(
							boost::dll::import<void(char**,uint32_t*)>(fullpath, "load_runtime", boost::dll::load_mode::default_mode)
						);

	this->pfree_runtime = std::make_shared<boost::dll::detail::import_type<void(char**,uint32_t*)>::type>(
							boost::dll::import<void(char**,uint32_t*)>(fullpath, "free_runtime", boost::dll::load_mode::default_mode)
						);

}
//--------------------------------------------------------------------
void xllr_plugin_interface_wrapper::load_runtime(char** err, uint32_t* err_len)
{
	err = nullptr;
	err_len = 0;
	(*this->pload_runtime)(err, err_len);
}
//--------------------------------------------------------------------
void xllr_plugin_interface_wrapper::free_runtime(char** err, uint32_t* err_len)
{
	err = nullptr;
	err_len = 0;
	(*this->pfree_runtime)(err, err_len);
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