#pragma once
#include <stdint.h>
#include <string>
#include <memory>
#include "runtime_plugin_interface_wrapper.h"

//--------------------------------------------------------------------
class foreign_function
{
private:
	int64_t _id;
	std::shared_ptr<runtime_plugin_interface_wrapper> _plugin;

public:
	foreign_function(std::shared_ptr<runtime_plugin_interface_wrapper> plugin, int64_t id);
	~foreign_function() = default;
	
	[[nodiscard]] int64_t id() const;

	void call(
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