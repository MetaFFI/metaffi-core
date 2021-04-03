#include "serializer_plugin_interface_wrapper.h"
#include <utils/scope_guard.hpp>
#include <boost/filesystem.hpp>
#include <utils/function_loader.hpp>
#include <utils/plugin_loader.hpp>

using namespace openffi::utils;

//--------------------------------------------------------------------
serializer_plugin_interface_wrapper::serializer_plugin_interface_wrapper(const std::string& serializer_extension)
{
	std::shared_ptr<boost::dll::shared_library> mod = load_plugin(std::string("openffi.serialization")+serializer_extension);
	
	this->pinit_plugin = load_func<void(void)>(*mod, "init_plugin");
	this->pcompile_serialization = load_func<void(const char*, uint32_t, const char*, uint32_t, char**, uint32_t*, char**, uint32_t*)>(*mod, "compile_serialization");
}
//--------------------------------------------------------------------
void serializer_plugin_interface_wrapper::init()
{
	(*this->pinit_plugin)();
}
//--------------------------------------------------------------------
void serializer_plugin_interface_wrapper::compile_serialization(const char* idl, uint32_t idl_length,
                                                                const char* language, uint32_t language_length,
                                                                char** out_serialization_code, uint32_t* out_serialization_code_length,
                                                                char** out_err, uint32_t* out_err_len)
{
	*out_err = nullptr;
	*out_err_len = 0;
	(*this->pcompile_serialization)(idl, idl_length, language, language_length, out_serialization_code, out_serialization_code_length, out_err, out_err_len);
}
//--------------------------------------------------------------------
