#include "foreign_function.h"

#include <utility>
#include <utils/foreign_function.h> // In MetaFFI SDK

//--------------------------------------------------------------------
foreign_function::foreign_function(std::shared_ptr<runtime_plugin_interface_wrapper> plugin, void* pff):_plugin(std::move(plugin)), _pforeign_function(pff){}
//--------------------------------------------------------------------
void* foreign_function::pforeign_function() const
{
	return this->_pforeign_function;
}
//--------------------------------------------------------------------
void foreign_function::xcall_params_ret(
		cdts params_ret[2],
		char** out_err, uint64_t* out_err_len
	)
{
	((pforeign_function_entrypoint_signature_params_ret)this->_pforeign_function)(params_ret, out_err, out_err_len);
}
//--------------------------------------------------------------------
void foreign_function::xcall_no_params_ret(
		cdts return_values[1],
		char** out_err, uint64_t* out_err_len
)
{
	((pforeign_function_entrypoint_signature_no_params_ret)this->_pforeign_function)(return_values, out_err, out_err_len);
}
//--------------------------------------------------------------------
void foreign_function::xcall_params_no_ret(
		cdts parameters[1],
		char** out_err, uint64_t* out_err_len
)
{
	((pforeign_function_entrypoint_signature_params_no_ret)this->_pforeign_function)(parameters, out_err, out_err_len);
}
//--------------------------------------------------------------------
void foreign_function::xcall_no_params_no_ret(
		char** out_err, uint64_t* out_err_len
)
{
	((pforeign_function_entrypoint_signature_no_params_no_ret)this->_pforeign_function)(out_err, out_err_len);
}
//--------------------------------------------------------------------