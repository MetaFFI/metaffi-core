#include "runtime_plugin_interface_wrapper.h"
#include <utils/scope_guard.hpp>
#include <utils/function_loader.hpp>
#include <utils/plugin_loader.hpp>

using namespace metaffi::utils;

//--------------------------------------------------------------------
runtime_plugin_interface_wrapper::runtime_plugin_interface_wrapper(const std::string& plugin_filename_without_extension)
{
	std::shared_ptr<boost::dll::shared_library> plugin_dll = load_plugin(plugin_filename_without_extension);
	
	this->pload_runtime = load_func<void(char**,uint32_t*)>(*plugin_dll, "load_runtime");
	
	this->pfree_runtime = load_func<void(char**,uint32_t*)>(*plugin_dll, "free_runtime");
	
	this->pload_function = load_func<void**(const char*, uint32_t, const char*, uint32_t, metaffi_types_ptr, metaffi_types_ptr, int8_t, int8_t, char**,uint32_t*)>(*plugin_dll, "load_function");
	
	this->pfree_function = load_func<void(void*, char**, uint32_t*)>(*plugin_dll, "free_function");
	
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
void** runtime_plugin_interface_wrapper::load_function(const char* module_path, uint32_t module_path_len, const char* function_path, uint32_t function_path_len, metaffi_types_ptr params_types, metaffi_types_ptr retvals_types, uint8_t params_count, uint8_t retval_count, char** err, uint32_t* err_len)
{
	*err = nullptr;
	*err_len = 0;
	return (*this->pload_function)(module_path, module_path_len, function_path, function_path_len, params_types, retvals_types, params_count, retval_count, err, err_len);
}
//--------------------------------------------------------------------
void runtime_plugin_interface_wrapper::free_function(void* pff, char** err, uint32_t* err_len)
{
	*err = nullptr;
	*err_len = 0;
	(*this->pfree_function)(pff, err, err_len);
}
//--------------------------------------------------------------------