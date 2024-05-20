#include "runtime_plugin_interface_wrapper.h"
#include <utils/scope_guard.hpp>
#include <utils/function_loader.hpp>
#include <utils/plugin_loader.hpp>

using namespace metaffi::utils;

//--------------------------------------------------------------------
runtime_plugin_interface_wrapper::runtime_plugin_interface_wrapper(const std::string& plugin_filename_without_extension)
    : runtime_plugin_interface_wrapper(plugin_filename_without_extension.c_str()){}
//--------------------------------------------------------------------
runtime_plugin_interface_wrapper::runtime_plugin_interface_wrapper(const char* plugin_filename_without_extension)
{
	this->plugin_dll = load_plugin(plugin_filename_without_extension);
	
	this->pload_runtime = load_func<void(char**)>(*plugin_dll, "load_runtime");
	this->pfree_runtime = load_func<void(char**)>(*plugin_dll, "free_runtime");
	
	this->pload_entity = load_func<xcall*(const char*, const char*, struct metaffi_type_info*, int8_t, struct metaffi_type_info*, int8_t, char**)>(*plugin_dll, "load_entity");
	this->pmake_callable = load_func<xcall*(void*, struct metaffi_type_info*, int8_t, struct metaffi_type_info*, int8_t, char**)>(*plugin_dll, "make_callable");
	
	this->pfree_xcall = load_func<void(xcall*, char**)>(*plugin_dll, "free_xcall");
}
//--------------------------------------------------------------------
void runtime_plugin_interface_wrapper::load_runtime(char** err)
{
	*err = nullptr;
	(*this->pload_runtime)(err);
}
//--------------------------------------------------------------------
void runtime_plugin_interface_wrapper::free_runtime(char** err)
{
	*err = nullptr;
	(*this->pfree_runtime)(err);
}
//--------------------------------------------------------------------
xcall* runtime_plugin_interface_wrapper::load_entity(const char* module_path, const char* function_path, struct metaffi_type_info* params_types, uint8_t params_count, struct metaffi_type_info* retvals_types, uint8_t retval_count, char** err)
{
	*err = nullptr;
	return (*this->pload_entity)(module_path, function_path, params_types, params_count, retvals_types, retval_count, err);
}
//--------------------------------------------------------------------
xcall* runtime_plugin_interface_wrapper::make_callable(void* make_callable_context, struct metaffi_type_info* params_types, uint8_t params_count, struct metaffi_type_info* retval_types, uint8_t retval_count, char** err)
{
	*err = nullptr;
	return (*this->pmake_callable)(make_callable_context, params_types, params_count, retval_types, retval_count, err);
}
//--------------------------------------------------------------------
void runtime_plugin_interface_wrapper::free_xcall(xcall* pxcall, char** err)
{
	*err = nullptr;
	(*this->pfree_xcall)(pxcall, err);
}
//--------------------------------------------------------------------
void runtime_plugin_interface_wrapper::fini()
{
	// unload dynamic library
	this->pload_runtime = nullptr;
	this->pfree_runtime = nullptr;
	this->pload_entity = nullptr;
	this->pmake_callable = nullptr;
	this->pfree_xcall = nullptr;
	this->plugin_dll->unload();
	this->plugin_dll = nullptr;
}
//--------------------------------------------------------------------