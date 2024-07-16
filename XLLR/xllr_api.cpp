#include "runtime/xllr_api.h"
#include "runtime_plugin_repository.h"
#include "utils/foreign_function.h"
#include <shared_mutex>
#include <mutex>
#include <iostream>
#include <cstring>
#include <set>
#include <utils/defines.h>

std::set<std::string> runtime_flags;
std::shared_mutex runtime_flags_lock;

#ifdef _WIN32
#define handle_err(err, before_handle)	\
catch(const std::exception& e)\
{\
	before_handle\
	if(err)\
	{ \
		rsize_t err_len = strlen(e.what());\
		*err = (char*)calloc(1, err_len+1);\
\
		strncpy_s(*err, err_len+1, e.what(), err_len);\
	}\
	else\
	{\
		std::cout << "CRITICAL ERROR: ";\
		std::cout << e.what() << std::endl;\
	}\
}\
catch(...)\
{\
	before_handle\
	const char* e = "unknown error";\
\
	if(err)\
	{\
		rsize_t err_len = strlen(e);\
		*err = (char*)calloc(1, err_len+1);\
\
		strncpy_s(*err, err_len+1, e, err_len);\
	}\
	else\
	{\
		std::cout << "CRITICAL ERROR: " << e << std::endl;\
	}\
}
#else
#define handle_err(err, before_handle)	\
catch(const std::exception& e)\
{\
	before_handle\
	if(err)\
	{ \
		int err_len = strlen(e.what());\
		*err = (char*)calloc(1, err_len+1);\
\
		std::strncpy(*err, e.what(), err_len);\
	}\
	else\
	{\
		std::cout << "CRITICAL ERROR: ";\
		std::cout << e.what() << std::endl;\
	}\
}\
catch(...)\
{\
	before_handle\
	const char* e = "unknown error";\
\
	if(err)\
	{\
		int err_len = strlen(e);\
		*err = (char*)calloc(1, err_len+1);\
\
		std::strncpy(*err, e, err_len);\
	}\
	else\
	{\
		std::cout << "CRITICAL ERROR: " << e << std::endl;\
	}\
}
#endif

runtime_plugin_repository g_runtime_plugins;

//--------------------------------------------------------------------
void load_runtime_plugin(const char* runtime_plugin_name, char** err)
{
    try
    {
		if(err == nullptr)
		{
			std::cerr << "err is null" << std::endl;
			std::abort();
		}
		
		*err = nullptr;
		
		// loads plugin if not loaded
	    std::shared_ptr<runtime_plugin> loaded_plugin = g_runtime_plugins.load(runtime_plugin_name);
		loaded_plugin->load_runtime(err);
    }
    handle_err(err,);
}
//--------------------------------------------------------------------
void free_runtime_plugin(const char* runtime_plugin, char** err)
{
    try
    {
		*err = nullptr;

		// loads plugin if not loaded
		g_runtime_plugins.unload(runtime_plugin);
    }
    handle_err(err,);
}
//--------------------------------------------------------------------
xcall* load_entity(const char* runtime_plugin_name, const char* module_path, const char* function_path, metaffi_type_info* params_types, int8_t params_count, metaffi_type_info* retval_types, int8_t retval_count, char** err)
{
	try
    {
		std::shared_ptr<runtime_plugin> p = g_runtime_plugins.get(runtime_plugin_name);
		
		if(!p) // if plugin not loaded - lazy load plugin
		{
			p = g_runtime_plugins.load(runtime_plugin_name);
		}
		
	    std::vector<metaffi_type_info> params;
		if(params_types != nullptr){
			params = std::vector<metaffi_type_info>(params_types, params_types + params_count);
		}
		
	    std::vector<metaffi_type_info> retvals;
		if(retval_types != nullptr){
			retvals = std::vector<metaffi_type_info>(retval_types, retval_types + retval_count);
		}
		
	    auto res = p->load_entity(module_path, function_path, params, retvals, err);
		if(*err)
		{
			std::stringstream ss;
			ss << "Failed to load function with function path " << function_path;
			throw std::runtime_error(ss.str());
		}
		
		return res;
    }
    handle_err(err,);
	
	return nullptr;
}
//--------------------------------------------------------------------
xcall* make_callable(const char* runtime_plugin_name, void* make_callable_context, metaffi_type_info* params_types, int8_t params_count, metaffi_type_info* retval_types, int8_t retval_count, char** err)
{
	try
	{
		std::shared_ptr<runtime_plugin> p = g_runtime_plugins.get(runtime_plugin_name);

		if(!p) // if plugin not loaded - lazy load plugin
		{
			p = g_runtime_plugins.load(runtime_plugin_name);
		}

		std::vector<metaffi_type_info> params;
		if(params_types != nullptr){
			params = std::vector<metaffi_type_info>(params_types, params_types + params_count);
		}

		std::vector<metaffi_type_info> retvals;
		if(retval_types != nullptr){
			retvals = std::vector<metaffi_type_info>(retval_types, retval_types + retval_count);
		}

		auto res = p->make_callable(make_callable_context, params, retvals, err);
		if(*err)
		{
			std::stringstream ss;
			ss << "Failed to load callable from runtime plugin " << runtime_plugin_name;
			return nullptr;
		}

		return res;
	}
	handle_err(err,);

	return nullptr;
}
//--------------------------------------------------------------------
void free_xcall(const char* runtime_plugin_name, xcall* pxcall, char** err)
{
    try
    {
		std::shared_ptr<runtime_plugin> p = g_runtime_plugins.get(runtime_plugin_name);
		if(!p)
		{
			std::stringstream ss;
			ss << "plugin was not found in the loaded plugin repository: " << runtime_plugin_name;
			throw std::runtime_error(ss.str());
		}
		p->free_xcall(pxcall, err);
    }
    handle_err(err, );
}
//--------------------------------------------------------------------
template<typename char_t>
char_t* alloc_string_t(const char_t* err_message, uint64_t length, char_t null_terminator)
{
	if(err_message == nullptr) {
		return nullptr;
	}
	
	char_t* copy = new char_t[length + 1];
	std::memcpy(copy, err_message, length*sizeof(char_t));
	copy[length] = null_terminator; // Null-terminate the string
	return copy;
}
char* alloc_string(const char* err_message, uint64_t length)
{
	return alloc_string_t(err_message, length, '\0');
}
char8_t* alloc_string8(const char8_t* err_message, uint64_t length)
{
	return alloc_string_t(err_message, length, u8'\0');
}
char16_t* alloc_string16(const char16_t* err_message, uint64_t length)
{
	return alloc_string_t(err_message, length, u'\0');
}
char32_t* alloc_string32(const char32_t* err_message, uint64_t length)
{
	return alloc_string_t(err_message, length, U'\0');
}
//--------------------------------------------------------------------
void free_string(const char* err_to_free)
{
	delete[] err_to_free;
}
//--------------------------------------------------------------------
void* metaffi_alloc(uint64_t size)
{
	return malloc(size);
}
//--------------------------------------------------------------------
void metaffi_free(void* ptr)
{
	free(ptr);
}
//--------------------------------------------------------------------
void xcall_params_ret(
        xcall* pplugin_xcall_and_context,
		cdts params_ret[2],
		char** out_err
)
{
	try
	{
		((pforeign_function_entrypoint_signature_params_ret)pplugin_xcall_and_context->pxcall_and_context[0])(pplugin_xcall_and_context->pxcall_and_context[1], params_ret, out_err);
	}
	handle_err((char**)out_err,);
}
//--------------------------------------------------------------------
void xcall_no_params_ret(
		xcall* pplugin_xcall_and_context,
		cdts return_values[1],
		char** out_err
)
{
	try
	{
		((pforeign_function_entrypoint_signature_no_params_ret)pplugin_xcall_and_context->pxcall_and_context[0])(pplugin_xcall_and_context->pxcall_and_context[1],return_values,out_err);
	}
	handle_err((char**)out_err,);
}
//--------------------------------------------------------------------
void xcall_params_no_ret(
		xcall* pplugin_xcall_and_context,
		cdts parameters[1],
		char** out_err
)
{
	try
	{
		((pforeign_function_entrypoint_signature_params_no_ret)pplugin_xcall_and_context->pxcall_and_context[0])(pplugin_xcall_and_context->pxcall_and_context[1],parameters,out_err);
	}
	handle_err((char**)out_err,);
}
//--------------------------------------------------------------------
void xcall_no_params_no_ret(
		xcall* pplugin_xcall_and_context,
		char** out_err
)
{
	try
	{
		((pforeign_function_entrypoint_signature_no_params_no_ret)pplugin_xcall_and_context->pxcall_and_context[0])(pplugin_xcall_and_context->pxcall_and_context[1], out_err);
	}
	handle_err((char**)out_err,);
}
//--------------------------------------------------------------------
void set_runtime_flag(const char* flag_name)
{
    std::unique_lock<std::shared_mutex> write_lock(runtime_flags_lock); // write lock
    runtime_flags.insert(flag_name);
}
//--------------------------------------------------------------------
int is_runtime_flag_set(const char* flag_name)
{
    std::shared_lock<std::shared_mutex> read_lock(runtime_flags_lock); // read lock
	return runtime_flags.find(flag_name) != runtime_flags.end() ? 1 : 0;
}
//--------------------------------------------------------------------