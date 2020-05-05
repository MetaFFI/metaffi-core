#pragma once
#include <stdint.h>


//--------------------------------------------------------------------
class foreign_module
{
public:
	foreign_module();
	~foreign_module();

	void call(
		// function name to call
        const char* func_name, uint32_t func_name_len,
        
        // serialized parameters
        unsigned char* params, uint64_t params_len,

		// serialized out parameters
		unsigned char** out_params, uint64_t* out_params_len,
        
        // out - serialized result or error message
        char** out_ret, uint64_t* out_ret_len
	);
};
//--------------------------------------------------------------------