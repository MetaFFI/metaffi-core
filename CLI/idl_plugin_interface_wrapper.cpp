#include "idl_plugin_interface_wrapper.h"
#include <utils/scope_guard.hpp>
#include <boost/filesystem.hpp>
#include <utils/function_loader.hpp>
#include <utils/plugin_loader.hpp>

using namespace openffi::utils;

//--------------------------------------------------------------------
idl_plugin_interface_wrapper::idl_plugin_interface_wrapper(const std::string& plugin_filename_without_extension)
{
	std::shared_ptr<boost::dll::shared_library> mod = load_plugin(plugin_filename_without_extension);
	
	this->pparse_idl = load_func<void(const char*, uint32_t, const char*, uint32_t, char**, uint32_t*, char**, uint32_t*)>(*mod, "parse_idl");
}
//--------------------------------------------------------------------
void idl_plugin_interface_wrapper::parse_idl(const char* idl_name, uint32_t idl_name_length,
                                                               const char* idl, uint32_t idl_length,
                                                               char** out_idl_def_json, uint32_t* out_idl_def_json_length,
                                                               char** out_err, uint32_t* out_err_len)
{
	*out_err = nullptr;
	*out_err_len = 0;
	(*this->pparse_idl)(idl_name, idl_name_length, idl, idl_length, out_idl_def_json, out_idl_def_json_length, out_err, out_err_len);
}
//--------------------------------------------------------------------
