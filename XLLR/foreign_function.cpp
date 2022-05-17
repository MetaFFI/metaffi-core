#include "foreign_function.h"


//--------------------------------------------------------------------
foreign_function::foreign_function(std::shared_ptr<runtime_plugin_interface_wrapper> plugin, int64_t id):_plugin(plugin), _id(id){}
//--------------------------------------------------------------------
int64_t foreign_function::id() const
{
	return this->_id;
}
//--------------------------------------------------------------------
void foreign_function::xcall(
		cdt* parameters, uint64_t parameters_len,
		cdt* return_values, uint64_t return_values_len,
		char** out_err, uint64_t* out_err_len
	)
{
	this->_plugin->xcall(this->_id,
	                    parameters, parameters_len,
	                    return_values, return_values_len,
	                    out_err, out_err_len);
}
//--------------------------------------------------------------------
