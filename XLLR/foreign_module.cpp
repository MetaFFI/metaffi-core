#include "foreign_module.h"


//--------------------------------------------------------------------
foreign_module::foreign_module(std::shared_ptr<xllr_plugin_interface_wrapper> plugin, const std::string& module_name):_plugin(plugin), _module_name(module_name){}
//--------------------------------------------------------------------
void foreign_module::call(
        const char* funcname, uint32_t funcname_len,
		unsigned char* in_params, uint64_t in_params_len,
        unsigned char** out_params, uint64_t* out_params_len,
        unsigned char** out_ret, uint64_t* out_ret_len,
		uint8_t* out_is_error
	)
{
	this->_plugin->call(_module_name.c_str(), _module_name.length(),
						funcname, funcname_len,
						in_params, in_params_len,
						out_params, out_params_len,
						out_ret, out_ret_len,
						out_is_error);
}
//--------------------------------------------------------------------
