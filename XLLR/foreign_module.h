#pragma once
#include <stdint.h>
#include <string>
#include <memory>
#include "xllr_plugin_interface_wrapper.h"

//--------------------------------------------------------------------
class foreign_module
{
private:
	std::string _module_name;
	std::shared_ptr<xllr_plugin_interface_wrapper> _plugin;

public:
	foreign_module(std::shared_ptr<xllr_plugin_interface_wrapper> plugin, const std::string& module_name);
	~foreign_module() = default;

	void call(
		// function name to call
        const char* funcname, uint32_t funcname_len,
        
		// serialized parameters
        unsigned char* in_params, uint64_t in_params_len,

        // serialized parameters
        unsigned char** in_out_params, uint64_t* in_out_params_len,
        
        // out - serialized result or error message
        unsigned char** out_ret, uint64_t* out_ret_len,

		uint8_t* out_is_error
	);
};
//--------------------------------------------------------------------