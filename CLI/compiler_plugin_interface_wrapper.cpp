#include "compiler_plugin_interface_wrapper.h"
#include "../utils/scope_guard.hpp"
#include <boost/filesystem.hpp>


//--------------------------------------------------------------------
compiler_plugin_interface_wrapper::compiler_plugin_interface_wrapper(const std::string& plugin_filename_without_extension)
{
	std::string plugin_filename = plugin_filename_without_extension+boost::dll::shared_library::suffix().generic_string();
	
	boost::dll::shared_library plugin_dll;
	
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
	
	this->pcompile_to_guest = this->load_func<void(const char*, uint32_t, const char*, uint32_t, char**, uint32_t*)>(plugin_dll, "compile_to_guest");
	this->pcompile_from_host = this->load_func<void(const char*, uint32_t, const char*, uint32_t, char**, uint32_t*)>(plugin_dll, "compile_from_host");
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
