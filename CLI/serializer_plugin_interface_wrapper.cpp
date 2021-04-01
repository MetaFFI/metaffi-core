#include "serializer_plugin_interface_wrapper.h"
#include <utils/scope_guard.hpp>
#include <boost/filesystem.hpp>
#include <utils/function_loader.hpp>
#include <utils/plugin_loader.hpp>

using namespace openffi::utils;

//--------------------------------------------------------------------
serializer_plugin_interface_wrapper::serializer_plugin_interface_wrapper(const std::string& plugin_filename_without_extension)
{
	std::shared_ptr<boost::dll::shared_library> mod = load_plugin(plugin_filename_without_extension);
	
	this->pcompile_serialization = load_func<void(const char*, uint32_t, char**, uint32_t*, char**, uint32_t*)>(*mod, "parse_idl");
}
//--------------------------------------------------------------------
void serializer_plugin_interface_wrapper::compile_serialization(const char* idl, uint32_t idl_length,
                                                                char** out_serialization_code, uint32_t* out_serialization_code_length,
                                                                char** out_err, uint32_t* out_err_len)
{
	*out_err = nullptr;
	*out_err_len = 0;
	(*this->pcompile_serialization)(idl, idl_length, out_serialization_code, out_serialization_code_length, out_err, out_err_len);
}
//--------------------------------------------------------------------
