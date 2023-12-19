#include "runtime/xllr_api.h"
#include "runtime_plugin_repository.h"
#include "utils/foreign_function.h"
#include <shared_mutex>
#include <mutex>
#include <iostream>
#include <cstring>
#include <set>

std::set<std::string> runtime_flags;
std::shared_mutex runtime_flags_lock;

#define handle_err(err, err_len, before_handle)	\
catch(const std::exception& e)\
{\
	before_handle\
	if(err)\
	{ \
		*err_len = strlen(e.what());\
		*err = (char*)calloc(1, *err_len+1);\
\
		std::strncpy(*err, e.what(), *err_len);\
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
		*err_len = strlen(e);\
		*err = (char*)calloc(1, *err_len+1);\
\
		std::strncpy(*err, e, *err_len);\
	}\
	else\
	{\
		std::cout << "CRITICAL ERROR: " << e << std::endl;\
	}\
}


runtime_plugin_respository g_runtime_plugins;

//--------------------------------------------------------------------
void load_runtime_plugin(const char* runtime_plugin_name, uint32_t runtime_plugin_name_len, char** err, uint32_t* err_len)
{
    try
    {
		*err = nullptr;
		*err_len = 0;
		
		// loads plugin if not loaded
	    std::shared_ptr<runtime_plugin> loaded_plugin = g_runtime_plugins.load(std::string(runtime_plugin_name, runtime_plugin_name_len));
		loaded_plugin->load_runtime();
    }
    handle_err(err, err_len,);
}
//--------------------------------------------------------------------
void free_runtime_plugin(const char* runtime_plugin, uint32_t runtime_plugin_len, char** err, uint32_t* err_len) 
{
    try
    {
		*err = nullptr;
		*err_len = 0;

		// loads plugin if not loaded
        g_runtime_plugins.release(std::string(runtime_plugin, runtime_plugin_len));
    }
    handle_err(err, err_len,);
}
//--------------------------------------------------------------------
void** load_function(const char* runtime_plugin_name, uint32_t runtime_plugin_name_len, const char* module_path, uint32_t module_path_len, const char* function_path, uint32_t function_path_len, metaffi_types_with_alias_ptr params_types, metaffi_types_with_alias_ptr retval_types, uint8_t params_count, uint8_t retval_count, char** err, uint32_t* err_len)
{
	try
    {
		std::string runtime_plugin_name_str(runtime_plugin_name, runtime_plugin_name_len);
		std::shared_ptr<runtime_plugin> p = g_runtime_plugins.get(runtime_plugin_name_str);
		
		if(!p) // if plugin not loaded - lazy load plugin
		{
			p = g_runtime_plugins.load(runtime_plugin_name_str);
		}
		
	    std::vector<metaffi_type_with_alias> params;
		if(params_types != nullptr){
			params = std::vector<metaffi_type_with_alias>(params_types, params_types+params_count);
		}
		
	    std::vector<metaffi_type_with_alias> retvals;
		if(retval_types != nullptr){
			retvals = std::vector<metaffi_type_with_alias>(retval_types, retval_types+retval_count);
		}
		
	    auto res = p->load_function(std::string(module_path, module_path_len), std::string(function_path, function_path_len), params, retvals);
		if(!res)
		{
			std::stringstream ss;
			ss << "Failed to load function with function path" << std::string(function_path, function_path_len);
			throw std::runtime_error(ss.str());
		}
		
		return (void**)res.get();
    }
    handle_err(err, err_len,);
	
	return nullptr;
}
//--------------------------------------------------------------------
void** load_callable(const char* runtime_plugin_name, uint32_t runtime_plugin_name_len, void* load_callable_context, metaffi_types_with_alias_ptr params_types, metaffi_types_with_alias_ptr retval_types, uint8_t params_count, uint8_t retval_count, char** err, uint32_t* err_len)
{
	try
	{
		std::string runtime_plugin_name_str(runtime_plugin_name, runtime_plugin_name_len);
		std::shared_ptr<runtime_plugin> p = g_runtime_plugins.get(runtime_plugin_name_str);

		if(!p) // if plugin not loaded - lazy load plugin
		{
			p = g_runtime_plugins.load(runtime_plugin_name_str);
		}

		std::vector<metaffi_type_with_alias> params;
		if(params_types != nullptr){
			params = std::vector<metaffi_type_with_alias>(params_types, params_types+params_count);
		}

		std::vector<metaffi_type_with_alias> retvals;
		if(retval_types != nullptr){
			retvals = std::vector<metaffi_type_with_alias>(retval_types, retval_types+retval_count);
		}

		auto res = p->load_callable(load_callable_context, params, retvals);
		if(!res)
		{
			std::stringstream ss;
			ss << "Failed to load callable from runtime plugin " << runtime_plugin_name_str;
			throw std::runtime_error(ss.str());
		}

		return (void**)res.get();
	}
	handle_err(err, err_len,);

	return nullptr;
}
//--------------------------------------------------------------------
void free_function(const char* runtime_plugin_name, uint32_t runtime_plugin_len, void** pff, char** err, uint32_t* err_len)
{
    try
    {
		std::shared_ptr<runtime_plugin> p = g_runtime_plugins.get(std::string(runtime_plugin_name, runtime_plugin_len));
		p->free_function(pff);
    }
    handle_err(err, err_len,);
}
//--------------------------------------------------------------------
[[maybe_unused]] void xcall_params_ret(
		void* pplugin_xcall_and_context[2],
		cdts params_ret[2],
		char** out_err, uint64_t* out_err_len
)
{
	try
	{
		*out_err_len = 0;
		((pforeign_function_entrypoint_signature_params_ret)pplugin_xcall_and_context[0])(pplugin_xcall_and_context[1], params_ret,out_err, out_err_len);
	}
	handle_err((char**)out_err, out_err_len,);
}
//--------------------------------------------------------------------
[[maybe_unused]] void xcall_no_params_ret(
		void* pplugin_xcall_and_context[2],
		cdts return_values[1],
		char** out_err, uint64_t* out_err_len
)
{
	try
	{
		*out_err_len = 0;
		
		
		((pforeign_function_entrypoint_signature_no_params_ret)pplugin_xcall_and_context[0])(pplugin_xcall_and_context[1],return_values,out_err, out_err_len);
	}
	handle_err((char**)out_err, out_err_len,);
}
//--------------------------------------------------------------------
[[maybe_unused]] void xcall_params_no_ret(
		void* pplugin_xcall_and_context[2],
		cdts parameters[1],
		char** out_err, uint64_t* out_err_len
)
{
	try
	{
		*out_err_len = 0;
		
		((pforeign_function_entrypoint_signature_params_no_ret)pplugin_xcall_and_context[0])(pplugin_xcall_and_context[1],parameters,out_err, out_err_len);
	}
	handle_err((char**)out_err, out_err_len,);
}
//--------------------------------------------------------------------
[[maybe_unused]] void xcall_no_params_no_ret(
		void* pplugin_xcall_and_context[2],
		char** out_err, uint64_t* out_err_len
)
{
	try
	{
		*out_err_len = 0;
		((pforeign_function_entrypoint_signature_no_params_no_ret)pplugin_xcall_and_context[0])(pplugin_xcall_and_context[1],out_err, out_err_len);
	}
	handle_err((char**)out_err, out_err_len,);
}
//--------------------------------------------------------------------
void set_runtime_flag(const char* flag_name, uint64_t flag_name_length)
{
    std::unique_lock<std::shared_mutex> write_lock(runtime_flags_lock); // write lock
    runtime_flags.insert(std::string(flag_name, flag_name_length));
}
//--------------------------------------------------------------------
int is_runtime_flag_set(const char* flag_name, uint64_t flag_name_length)
{
    std::shared_lock<std::shared_mutex> read_lock(runtime_flags_lock); // read lock
	return runtime_flags.find(std::string(flag_name, flag_name_length)) != runtime_flags.end() ? 1 : 0;
}
//--------------------------------------------------------------------