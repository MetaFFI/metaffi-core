#include "compiler_plugin_interface_wrapper.h"
#include "../utils/scope_guard.hpp"
#include <boost/filesystem.hpp>


//--------------------------------------------------------------------
compiler_plugin_interface_wrapper::compiler_plugin_interface_wrapper(const std::string& plugin_filename)
{
	boost::filesystem::path fullpath(boost::filesystem::current_path());
	fullpath.append(plugin_filename);
	
	std::cout << "Loading Functions from: " << fullpath << std::endl;
	
	this->pcompile_to_guest = this->load_func<void(const char*, uint32_t, const char*, uint32_t, char**, uint32_t*)>(fullpath.c_str(), "compile_to_guest");
	this->pcompile_from_host = this->load_func<void(const char*, uint32_t, const char*, uint32_t, char**, uint32_t*)>(fullpath.c_str(), "compile_from_host");
}
//--------------------------------------------------------------------
void compiler_plugin_interface_wrapper::compile_to_guest(const char* idl_path, uint32_t idl_path_length,
														const char* output_path, uint32_t output_path_length,
														char** out_err, uint32_t* out_err_len)
{
	*out_err = nullptr;
	*out_err_len = 0;
	(*this->pcompile_to_guest)(idl_path, idl_path_length, output_path, output_path_length, out_err, out_err_len);
}
//--------------------------------------------------------------------
void compiler_plugin_interface_wrapper::compile_from_host(const char* idl_path, uint32_t idl_path_length,
															const char* output_path, uint32_t output_path_length,
															char** out_err, uint32_t* out_err_len)
{
	*out_err = nullptr;
	*out_err_len = 0;
	(*this->pcompile_from_host)(idl_path, idl_path_length, output_path, output_path_length, out_err, out_err_len);
}
//--------------------------------------------------------------------
