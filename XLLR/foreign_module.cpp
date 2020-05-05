#include "foreign_module.h"


//--------------------------------------------------------------------
foreign_module::foreign_module() {
    
}
//--------------------------------------------------------------------
foreign_module::~foreign_module() 
{
	
}
//--------------------------------------------------------------------
void foreign_module::call(
    	const char* func_name, uint32_t func_name_len,
    	unsigned char* params, uint64_t params_len,
		unsigned char** out_params, uint64_t* out_params_len,
    	char** out_ret, uint64_t* out_ret_len
	)
{
	
}
//--------------------------------------------------------------------
