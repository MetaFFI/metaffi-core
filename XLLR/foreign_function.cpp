#include "foreign_function.h"


//--------------------------------------------------------------------
foreign_function::foreign_function(std::shared_ptr<runtime_plugin_interface_wrapper> plugin, int64_t id):_plugin(plugin), _id(id){}
//--------------------------------------------------------------------
int64_t foreign_function::id() const
{
	return this->_id;
}
//--------------------------------------------------------------------
void foreign_function::xcall_params_ret(
		cdts params_ret[2],
		char** out_err, uint64_t* out_err_len
	)
{
	this->_plugin->xcall_params_ret(this->_id,
			                        params_ret,
				                    out_err, out_err_len);
}
//--------------------------------------------------------------------
void foreign_function::xcall_no_params_ret(
		cdts return_values[1],
		char** out_err, uint64_t* out_err_len
)
{
	this->_plugin->xcall_no_params_ret(this->_id,
	                     return_values,
	                     out_err, out_err_len);
}
//--------------------------------------------------------------------
void foreign_function::xcall_params_no_ret(
		cdts parameters[1],
		char** out_err, uint64_t* out_err_len
)
{
	this->_plugin->xcall_params_no_ret(this->_id,
	                     parameters,
	                     out_err, out_err_len);
}
//--------------------------------------------------------------------
void foreign_function::xcall_no_params_no_ret(
		char** out_err, uint64_t* out_err_len
)
{
	this->_plugin->xcall_no_params_no_ret(this->_id,
	                     out_err, out_err_len);
}
//--------------------------------------------------------------------