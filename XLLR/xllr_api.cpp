#include "xllr_api.h"
#include "plugin_repository.h"
#include <unordered_map>
#include <iostream>
#include <cstring>

#define handle_err(err, err_len)	\
catch(const std::exception& e)\
{\
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
		std::cerr << e.what() << std::endl;\
	}\
}\
catch(...)\
{\
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
		std::cerr << e << std::endl;\
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
        g_plugins.load(std::string(runtime_plugin, runtime_plugin_len));
    }
    handle_err(err, err_len);
}
//--------------------------------------------------------------------
void release_runtime_plugin(const char* runtime_plugin, uint32_t runtime_plugin_len, char** err, uint32_t* err_len) 
{
    try
    {
		*err = nullptr;
		*err_len = 0;

		// loads plugin if not loaded
        g_plugins.release(std::string(runtime_plugin, runtime_plugin_len));
    }
    handle_err(err, err_len);
}
//--------------------------------------------------------------------
/*
void load_module(const char* plugin, uint32_t plugin_len, const char* module, uint32_t module_len, char** err, uint32_t* err_len) 
{
	try
    {
		std::shared_ptr<xllr_plugin> p = g_plugins.get(std::string(plugin, plugin_len));
		p->load(std::string(module, module_len));
    }
    handle_err(err, err_len);
}
//--------------------------------------------------------------------
void release_module(const char* plugin, uint32_t plugin_len, const char* module, uint32_t module_len, char** err, uint32_t* err_len) 
{
    try
    {
		std::shared_ptr<xllr_plugin> p = g_plugins.get(std::string(plugin, plugin_len));
		p->release(std::string(module, module_len));
    }
    handle_err(err, err_len);
}
//--------------------------------------------------------------------
void call(
	uint8_t is_initialize,
	const char* plugin, uint32_t plugin_len,
	const char* module_name, uint32_t module_name_len,
	const char* func_name, uint32_t func_name_len,
	unsigned char* params, uint64_t params_len,
	unsigned char** out_params, uint64_t* out_params_len,
	char** out_ret, uint64_t* out_ret_len,
	uint8_t* is_error
)
{
	try
    {
		if(!is_initialize)
		{
			std::shared_ptr<xllr_plugin> p = g_plugins.get(std::string(plugin, plugin_len));
			auto m = p->get(std::string(module, module_len));
			m->call(
				func_name, func_name_len,
				params, params_len,
				out_params, out_params_len,
				out_ret, out_ret_len,
				is_error
			);
		}
		else
		{
			std::shared_ptr<xllr_plugin> p = g_plugins.load(std::string(plugin, plugin_len));
			auto m = p->load(std::string(module, module_len));
			m->call(
				func_name, func_name_len,
				params, params_len,
				out_params, out_params_len,
				out_ret, out_ret_len,
				is_error
			);
		}
    }
    handle_err(out_ret, out_ret_len);
}
//--------------------------------------------------------------------
*/