#include "foreign_function.h"

#include <utility>
#include <utils/foreign_function.h> // In MetaFFI SDK

//--------------------------------------------------------------------
foreign_function::foreign_function(std::shared_ptr<runtime_plugin_interface_wrapper> plugin, void* xcall_and_context[2]):
													_plugin(std::move(plugin)),
													_pforeign_function(xcall_and_context[0]),
													_context(xcall_and_context[1]){}
//--------------------------------------------------------------------
void* foreign_function::pforeign_function() const
{
	return this->_pforeign_function;
}
//--------------------------------------------------------------------
void* foreign_function::pcontext() const
{
	return this->_context;
}
//--------------------------------------------------------------------
void foreign_function::xcall_params_ret(
		cdts params_ret[2],
		char** out_err, uint64_t* out_err_len
	)
{
	((pforeign_function_entrypoint_signature_params_ret)this->_pforeign_function)(this->_context, params_ret, out_err, out_err_len);
}
//--------------------------------------------------------------------
void foreign_function::xcall_no_params_ret(
		cdts return_values[1],
		char** out_err, uint64_t* out_err_len
)
{
	((pforeign_function_entrypoint_signature_no_params_ret)this->_pforeign_function)(this->_context, return_values, out_err, out_err_len);
}
//--------------------------------------------------------------------
void foreign_function::xcall_params_no_ret(
		cdts parameters[1],
		char** out_err, uint64_t* out_err_len
)
{
	((pforeign_function_entrypoint_signature_params_no_ret)this->_pforeign_function)(this->_context, parameters, out_err, out_err_len);
}
//--------------------------------------------------------------------
void foreign_function::xcall_no_params_no_ret(
		char** out_err, uint64_t* out_err_len
)
{
	((pforeign_function_entrypoint_signature_no_params_no_ret)this->_pforeign_function)(this->_context, out_err, out_err_len);
}
//--------------------------------------------------------------------