#include "foreign_function.h"


//--------------------------------------------------------------------
foreign_function::foreign_function(std::shared_ptr<runtime_plugin_interface_wrapper> plugin, int64_t id):_plugin(plugin), _id(id){}
//--------------------------------------------------------------------
int64_t foreign_function::id() const
{
	return this->_id;
}
//--------------------------------------------------------------------
void foreign_function::call(
		unsigned char* in_params, uint64_t in_params_len,
        unsigned char** out_params, uint64_t* out_params_len,
        unsigned char** out_ret, uint64_t* out_ret_len,
		uint8_t* out_is_error
	)
{
	this->_plugin->call(this->_id,
						in_params, in_params_len,
						out_params, out_params_len,
						out_ret, out_ret_len,
						out_is_error);
}
//--------------------------------------------------------------------
