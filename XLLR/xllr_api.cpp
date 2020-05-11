#include "xllr_api.h"
#include "plugin_repository.h"
#include <unordered_map>
#include <iostream>
#include <cstring>

#define handle_err(err, err_len, before_handle)	\
catch(const std::exception& e)\
{\
	before_handle\
	if(err)\
	{\
		*err_len = strlen(e.what());\
		*err = (char*)malloc(*err_len+1);\
		(*err)[*err_len-1] = 0;\
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
		*err = (char*)malloc(*err_len+1);\
		(*err)[*err_len-1] = 0;\
\
		std::strncpy(*err, e, *err_len);\
	}\
	else\
	{\
		std::cout << "CRITICAL ERROR: " << e << std::endl;\
	}\
}


plugin_respository g_plugins;

//--------------------------------------------------------------------
void load_runtime_plugin(const char* runtime_plugin, uint32_t runtime_plugin_len, char** err, uint32_t* err_len) 
{
    try
    {
		*err = nullptr;
		*err_len = 0;

		// loads plugin if not loaded
        std::shared_ptr<xllr_plugin> loaded_plugin = g_plugins.load(std::string(runtime_plugin, runtime_plugin_len));
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
        g_plugins.release(std::string(runtime_plugin, runtime_plugin_len));
    }
    handle_err(err, err_len,);
}
//--------------------------------------------------------------------
void load_module(const char* runtime_plugin, uint32_t runtime_plugin_len, const char* module, uint32_t module_len, char** err, uint32_t* err_len) 
{
	try
    {
		std::string plugin_name(runtime_plugin, runtime_plugin_len);
		std::shared_ptr<xllr_plugin> p = g_plugins.get(plugin_name);

		if(!p) // if plugin not loaded - lazy load plugin
		{
			p = g_plugins.load(plugin_name);
		}

		p->load_module(std::string(module, module_len));
    }
    handle_err(err, err_len,);
}
//--------------------------------------------------------------------
void free_module(const char* runtime_plugin, uint32_t runtime_plugin_len, const char* module, uint32_t module_len, char** err, uint32_t* err_len) 
{
    try
    {
		std::shared_ptr<xllr_plugin> p = g_plugins.get(std::string(runtime_plugin, runtime_plugin_len));
		p->free_module(std::string(module, module_len));
    }
    handle_err(err, err_len,);
}
//--------------------------------------------------------------------
void call(
	const char* runtime_plugin, uint32_t runtime_plugin_len,
	const char* module_name, uint32_t module_name_len,
	const char* func_name, uint32_t func_name_len,
	unsigned char* in_params, uint64_t in_params_len,
	unsigned char** out_params, uint64_t* out_params_len,
	unsigned char** out_ret, uint64_t* out_ret_len,
	uint8_t* is_error
)
{
	try
    {
		// check if module is loaded, if not - lazy load it.
		std::string plugin_name(runtime_plugin, runtime_plugin_len);
		std::string module(module_name, module_name_len);
		std::shared_ptr<xllr_plugin> p = g_plugins.get(plugin_name);
		std::shared_ptr<foreign_module> m;

		if(!p) // if plugin not loaded - lazy load plugin and module
		{
			p = g_plugins.load(plugin_name);
			m = p->load_module(module);
		}
		else if(m = p->get_module(module); !m) // load foreign module
		{
			// check if module is load, if not, load it
			m = p->load_module(module);
		}

		m->call(
			func_name, func_name_len,
			in_params, in_params_len,
			out_params, out_params_len,
			out_ret, out_ret_len,
			is_error
		);
    }
    handle_err((char**)out_ret, out_ret_len, *is_error=1;);
}
//--------------------------------------------------------------------