#include "language_plugin_interface_wrapper.h"
#include <utils/scope_guard.hpp>
#include <boost/filesystem.hpp>
#include <utils/function_loader.hpp>
#include <utils/plugin_loader.hpp>

using namespace metaffi::utils;

//--------------------------------------------------------------------
language_plugin_interface_wrapper::language_plugin_interface_wrapper(const std::string& plugin_filename_without_extension)
{
	std::shared_ptr<boost::dll::shared_library> mod = load_plugin(plugin_filename_without_extension);
	
	this->pinit = load_func<void(void)>(*mod, "init_plugin");
	this->pcompile_to_guest = load_func<void(const char*, uint32_t, const char*, uint32_t, const char*, uint32_t, const char*, uint32_t, char**, uint32_t*)>(*mod, "compile_to_guest");
	this->pcompile_from_host = load_func<void(const char*, uint32_t, const char*, uint32_t, const char*, uint32_t, char**, uint32_t*)>(*mod, "compile_from_host");
}
//--------------------------------------------------------------------
void language_plugin_interface_wrapper::init()
{
	(*this->pinit)();
}
//--------------------------------------------------------------------
void language_plugin_interface_wrapper::compile_to_guest(const char* idl_def_json, uint32_t idl_def_json_length,
                                                         const char* output_path, uint32_t output_path_length,
                                                         const char* block_name, uint32_t block_name_length,
                                                         const char* block_code, uint32_t block_code_length,
                                                         char** out_err, uint32_t* out_err_len)
{
	*out_err = nullptr;
	*out_err_len = 0;
	(*this->pcompile_to_guest)(idl_def_json, idl_def_json_length, output_path, output_path_length, block_name, block_name_length, block_code, block_code_length, out_err, out_err_len);
}
//--------------------------------------------------------------------
void language_plugin_interface_wrapper::compile_from_host(const char* idl_def_json, uint32_t idl_def_json_length,
                                                          const char* output_path, uint32_t output_path_length,
                                                          const char* host_options, uint32_t host_options_length,
                                                          char** out_err, uint32_t* out_err_len)
{
	*out_err = nullptr;
	*out_err_len = 0;
	(*this->pcompile_from_host)(idl_def_json, idl_def_json_length,
								output_path, output_path_length,
								host_options, host_options_length,
								out_err, out_err_len);
}
//--------------------------------------------------------------------
