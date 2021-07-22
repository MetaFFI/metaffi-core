#include "runtime/xllr_api.h"
#include "runtime_plugin_repository.h"
#include <shared_mutex>
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
int64_t load_function(const char* runtime_plugin_name, uint32_t runtime_plugin_name_len, const char* function_path, uint32_t function_path_len, int64_t function_id, char** err, uint32_t* err_len)
{
	try
    {
		std::string runtime_plugin_name_str(runtime_plugin_name, runtime_plugin_name_len);
		std::shared_ptr<runtime_plugin> p = g_runtime_plugins.get(runtime_plugin_name_str);

		if(!p) // if plugin not loaded - lazy load plugin
		{
			p = g_runtime_plugins.load(runtime_plugin_name_str);
		}
	
	    return p->load_function(std::string(function_path, function_path_len), function_id)->id();
    }
    handle_err(err, err_len,);
	
	return -1;
}
//--------------------------------------------------------------------
void free_function(const char* runtime_plugin_name, uint32_t runtime_plugin_len, int64_t function_id, char** err, uint32_t* err_len)
{
    try
    {
		std::shared_ptr<runtime_plugin> p = g_runtime_plugins.get(std::string(runtime_plugin_name, runtime_plugin_len));
		p->free_function(function_id);
    }
    handle_err(err, err_len,);
}
//--------------------------------------------------------------------
void call(
		const char* runtime_plugin_name, uint32_t runtime_plugin_name_len,
		int64_t function_id,
		cdt* parameters, uint64_t parameters_len,
		cdt* return_values, uint64_t return_values_len,
		char** out_err, uint64_t* out_err_len
)
{
	try
	{
		*out_err_len = 0;
		
		// check if module is loaded, if not - lazy load it.
		std::shared_ptr<runtime_plugin> p = g_runtime_plugins.get(std::string(runtime_plugin_name, runtime_plugin_name_len));
		if(!p)
		{
			throw std::runtime_error("runtime plugin has not been loaded");
		}
		
		std::shared_ptr<foreign_function> f = p->get_function(function_id);
		if(!f)
		{
			throw std::runtime_error("foreign function has not been loaded");
		}
		
		f->call(
				parameters, parameters_len,
				return_values, return_values_len,
				out_err, out_err_len
		);
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