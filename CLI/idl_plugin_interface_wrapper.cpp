#include "idl_plugin_interface_wrapper.h"
#include <utils/scope_guard.hpp>
#include <utils/function_loader.hpp>
#include <utils/plugin_loader.hpp>
#include <memory>

using namespace metaffi::utils;

std::unordered_map<std::string, std::shared_ptr<idl_plugin_interface_wrapper>> idl_plugins;

//--------------------------------------------------------------------
idl_plugin_interface_wrapper::idl_plugin_interface_wrapper(const std::string& idl_name)
{
	mod = load_plugin(std::string("metaffi.idl.")+idl_name);
	this->pinit_plugin = load_func<void(void)>(*mod, "init_plugin");
	this->pparse_idl = load_func<void(const char*, uint32_t, const char*, uint32_t, char**, uint32_t*, char**, uint32_t*)>(*mod, "parse_idl");
}
//--------------------------------------------------------------------
void idl_plugin_interface_wrapper::init()
{
	(*this->pinit_plugin)();
}
//--------------------------------------------------------------------
void idl_plugin_interface_wrapper::parse_idl(const char* source_code, uint32_t source_code_length,
                                           const char* file_or_dir_path, uint32_t file_or_dir_path_length,
                                           char** out_idl_def_json, uint32_t* out_idl_def_json_length,
                                           char** out_err, uint32_t* out_err_len)
{
	*out_err = nullptr;
	*out_err_len = 0;
	(*this->pparse_idl)(source_code, source_code_length, file_or_dir_path, file_or_dir_path_length, out_idl_def_json, out_idl_def_json_length, out_err, out_err_len);
}
//--------------------------------------------------------------------
std::shared_ptr<idl_plugin_interface_wrapper> idl_plugin_interface_wrapper::load(const std::string& idl_name)
{
	std::shared_ptr<idl_plugin_interface_wrapper> idl;
	auto idl_plugin_it = idl_plugins.find(idl_name);
	if(idl_plugin_it == idl_plugins.end())
	{
		idl = std::make_shared<idl_plugin_interface_wrapper>(idl_name);
		idl->init();
		idl_plugins[idl_name] = idl;
	}
	else
	{
		idl = idl_plugin_it->second;
	}
	
	return idl;
}
//--------------------------------------------------------------------
void idl_plugin_interface_wrapper::parse_idl(const std::string& source_code, const std::string& file_or_dir_path, std::string& out_idl_def_json, std::string& out_err)
{
	{
		char* err = nullptr;
		uint32_t err_len = 0;
		char* idl_def_json = nullptr;
		uint32_t idl_def_json_length = 0;
		
		this->parse_idl(source_code.c_str(), source_code.length(),
                        file_or_dir_path.c_str(), file_or_dir_path.length(),
		                &idl_def_json, &idl_def_json_length,
		                &err, &err_len);
		
		if(idl_def_json_length)
		{
			out_idl_def_json = std::string(idl_def_json, idl_def_json_length);
		}
		
		if(err_len)
		{
			out_err = std::string(err, err_len);
		}
	}
}
//--------------------------------------------------------------------
