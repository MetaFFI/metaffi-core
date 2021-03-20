#include "compiler_plugin_interface_wrapper.h"
#include <utils/scope_guard.hpp>
#include <boost/filesystem.hpp>
#include <utils/function_loader.hpp>
#include <utils/plugin_loader.hpp>

using namespace openffi::utils;

//--------------------------------------------------------------------
compiler_plugin_interface_wrapper::compiler_plugin_interface_wrapper(const std::string& plugin_filename_without_extension)
{
	std::shared_ptr<boost::dll::shared_library> mod = load_plugin(plugin_filename_without_extension);
	
	this->pcompile_to_guest = load_func<void(idl_definition*, const char*, uint32_t, const char*, uint32_t, char**, uint32_t*)>(*mod, "compile_to_guest");
	this->pcompile_from_host = load_func<void(idl_definition*, const char*, uint32_t, const char*, uint32_t, char**, uint32_t*)>(*mod, "compile_from_host");
}
//--------------------------------------------------------------------
void compiler_plugin_interface_wrapper::compile_to_guest(idl_definition* idl_def,
                                                         const char* output_path, uint32_t output_path_length,
                                                         const char* serialization_code, uint32_t serialization_code_length,
                                                         char** out_err, uint32_t* out_err_len)
{
	*out_err = nullptr;
	*out_err_len = 0;
	(*this->pcompile_to_guest)(idl_path, idl_path_length, output_path, output_path_length, out_err, out_err_len);
}
//--------------------------------------------------------------------
void compiler_plugin_interface_wrapper::compile_from_host(idl_definition* idl_def,
                                                          const char* output_path, uint32_t output_path_length,
                                                          const char* serialization_code, uint32_t serialization_code_length,
                                                          char** out_err, uint32_t* out_err_len)
{
	*out_err = nullptr;
	*out_err_len = 0;
	(*this->pcompile_from_host)(idl_path, idl_path_length, output_path, output_path_length, out_err, out_err_len);
}
//--------------------------------------------------------------------
void compiler_plugin_interface_wrapper::compile_serialization(const char* idl_path, uint32_t idl_path_length,
                           const char* output_path, uint32_t output_path_length,
                           char** out_err, uint32_t* out_err_len)
{
	*out_err = nullptr;
	*out_err_len = 0;
	(*this->pcompile_serialization)(idl_path, idl_path_length, output_path, output_path_length, out_err, out_err_len);
}
//--------------------------------------------------------------------
