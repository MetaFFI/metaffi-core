#include "xllr_plugin_interface_wrapper.h"
#include "../utils/scope_guard.hpp"
#include <boost/filesystem.hpp>


//--------------------------------------------------------------------
xllr_plugin_interface_wrapper::xllr_plugin_interface_wrapper(const std::string& plugin_filename_without_extension)
{
	std::string plugin_filename = plugin_filename_without_extension+boost::dll::shared_library::suffix().generic_string();
	
	boost::dll::shared_library plugin_dll;
	
	//std::cout << "Loading Functions from: " << fullpath << std::endl;
	
	// if plugin exists in the same path of the program, load it from there (mainly used for easier development)
	// otherwise, search system folders
	if(boost::filesystem::exists( boost::dll::program_location().append(plugin_filename) ))
	{
		//std::cout << "Loading Functions from: " << p.generic_string() << std::endl;
		plugin_dll.load( boost::dll::program_location().append(plugin_filename) );
	}
	else
	{
		//std::cout << "Loading Functions from: " << p.generic_string() << std::endl;
		plugin_dll.load(plugin_filename, boost::dll::load_mode::search_system_folders);
	}
	
	this->pload_runtime = this->load_func<void(char**,uint32_t*)>(plugin_dll, "load_runtime");
	
	this->pfree_runtime = this->load_func<void(char**,uint32_t*)>(plugin_dll, "free_runtime");
	
	this->pload_module = this->load_func<void(const char*, uint32_t, char**,uint32_t*)>(plugin_dll, "load_module");
	
	this->pfree_module = this->load_func<void(const char*, uint32_t, char**,uint32_t*)>(plugin_dll, "free_module");
	
	this->pcall = this->load_func<void(const char*, uint32_t,
										const char*, uint32_t,
										unsigned char*, uint64_t,
										unsigned char**, uint64_t*,
										unsigned char**, uint64_t*,
										uint8_t*)>(plugin_dll, "call");
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
void xllr_plugin_interface_wrapper::call(
	const char* module_name, uint32_t module_name_len,
	const char* func_name, uint32_t func_name_len,
	unsigned char* in_params, uint64_t in_params_len,
	unsigned char** out_params, uint64_t* out_params_len,
	unsigned char** out_ret, uint64_t* out_ret_len,
	uint8_t* out_is_error)
{
	*out_ret = nullptr;
	*out_ret_len = 0;
	*out_is_error = 0;

	(*this->pcall)(module_name, module_name_len,
					func_name, func_name_len,
					in_params, in_params_len,
					out_params, out_params_len,
					out_ret, out_ret_len,
					out_is_error);
}
//--------------------------------------------------------------------