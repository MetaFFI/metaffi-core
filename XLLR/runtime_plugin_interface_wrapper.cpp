#include "runtime_plugin_interface_wrapper.h"
#include <utils/scope_guard.hpp>
#include <boost/filesystem.hpp>
#include <utils/function_loader.hpp>
#include <utils/plugin_loader.hpp>

using namespace metaffi::utils;

//--------------------------------------------------------------------
runtime_plugin_interface_wrapper::runtime_plugin_interface_wrapper(const std::string& plugin_filename_without_extension)
{
	std::shared_ptr<boost::dll::shared_library> plugin_dll = load_plugin(plugin_filename_without_extension);
	
	this->pload_runtime = load_func<void(char**,uint32_t*)>(*plugin_dll, "load_runtime");
	
	this->pfree_runtime = load_func<void(char**,uint32_t*)>(*plugin_dll, "free_runtime");
	
	this->pload_function = load_func<int64_t(const char*, uint32_t, char**,uint32_t*)>(*plugin_dll, "load_function");
	
	this->pfree_function = load_func<void(int64_t, char**, uint32_t*)>(*plugin_dll, "free_function");
	
	this->pxcall_params_ret = load_func<void(int64_t,
				                             cdts[2],
				                             char**, uint64_t*)>(*plugin_dll, "xcall_params_ret");
	
	this->pxcall_no_params_ret = load_func<void(int64_t,
                                                cdts[1],
	                                            char**, uint64_t*)>(*plugin_dll, "xcall_no_params_ret");
	
	this->pxcall_params_no_ret = load_func<void(int64_t,
	                                            cdts[1],
	                                            char**, uint64_t*)>(*plugin_dll, "xcall_params_no_ret");
	
	this->pxcall_no_params_no_ret = load_func<void(int64_t,
	                                            char**, uint64_t*)>(*plugin_dll, "xcall_no_params_no_ret");
}
//--------------------------------------------------------------------
void runtime_plugin_interface_wrapper::load_runtime(char** err, uint32_t* err_len)
{
	*err = nullptr;
	*err_len = 0;
	(*this->pload_runtime)(err, err_len);
}
//--------------------------------------------------------------------
void runtime_plugin_interface_wrapper::free_runtime(char** err, uint32_t* err_len)
{
	*err = nullptr;
	*err_len = 0;
	(*this->pfree_runtime)(err, err_len);
}
//--------------------------------------------------------------------
int64_t runtime_plugin_interface_wrapper::load_function(const char* function_path, uint32_t function_path_len, int8_t params_count, int8_t retval_count, char** err, uint32_t* err_len)
{
	*err = nullptr;
	*err_len = 0;
	return (*this->pload_function)(function_path, function_path_len, err, err_len);
}
//--------------------------------------------------------------------
void runtime_plugin_interface_wrapper::free_function(int64_t function_id, char** err, uint32_t* err_len)
{
	*err = nullptr;
	*err_len = 0;
	(*this->pfree_function)(function_id, err, err_len);
}
//--------------------------------------------------------------------
void runtime_plugin_interface_wrapper::xcall_params_ret(
		int64_t function_id,
		cdts params_ret[2],
		char** out_err, uint64_t* out_err_len)
{
	*out_err_len = 0;
	*out_err = nullptr;

	(*this->pxcall_params_ret)(function_id,
	                           params_ret,
				               out_err, out_err_len);
}
//--------------------------------------------------------------------
void runtime_plugin_interface_wrapper::xcall_no_params_ret(
		int64_t function_id,
		cdts ret[1],
		char** out_err, uint64_t* out_err_len)
{
	*out_err_len = 0;
	*out_err = nullptr;
	
	(*this->pxcall_no_params_ret)(function_id,
	                              ret,
	                           out_err, out_err_len);
}
//--------------------------------------------------------------------
void runtime_plugin_interface_wrapper::xcall_params_no_ret(
		int64_t function_id,
		cdts params[1],
		char** out_err, uint64_t* out_err_len)
{
	*out_err_len = 0;
	*out_err = nullptr;
	
	(*this->pxcall_params_no_ret)(function_id,
	                              params,
	                              out_err, out_err_len);
}
//--------------------------------------------------------------------
void runtime_plugin_interface_wrapper::xcall_no_params_no_ret(
		int64_t function_id,
		char** out_err, uint64_t* out_err_len)
{
	*out_err_len = 0;
	*out_err = nullptr;
	
	(*this->pxcall_no_params_no_ret)(function_id,
	                              out_err, out_err_len);
}
//--------------------------------------------------------------------