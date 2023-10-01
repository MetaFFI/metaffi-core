#include "language_plugin_interface_wrapper.h"
#include <utils/scope_guard.hpp>
#include <utils/function_loader.hpp>
#include <utils/plugin_loader.hpp>

using namespace metaffi::utils;
std::unordered_map<std::string,std::shared_ptr<language_plugin_interface_wrapper>> compiler_plugins;

//--------------------------------------------------------------------
language_plugin_interface_wrapper::language_plugin_interface_wrapper(const std::string& plugin_filename_without_extension)
{
	std::shared_ptr<boost::dll::shared_library> mod = load_plugin(plugin_filename_without_extension);
	
	this->pinit = load_func<void(void)>(*mod, "init_plugin");
	this->pcompile_to_guest = load_func<void(const char*, uint32_t, const char*, uint32_t, const char*, uint32_t, char**, uint32_t*)>(*mod, "compile_to_guest");
	this->pcompile_from_host = load_func<void(const char*, uint32_t, const char*, uint32_t, const char*, uint32_t, char**, uint32_t*)>(*mod, "compile_from_host");
	
}
//--------------------------------------------------------------------
void language_plugin_interface_wrapper::init()
{
	(*this->pinit)();
}
//--------------------------------------------------------------------
void language_plugin_interface_wrapper::compile_to_guest(const char* idl_def_json, uint32_t idl_def_json_length,
                                                         const char* output_path, uint32_t output_path_length,
                                                         const char* guest_options, uint32_t guest_options_length,
                                                         char** out_err, uint32_t* out_err_len)
{
	*out_err = nullptr;
	*out_err_len = 0;
	(*this->pcompile_to_guest)(idl_def_json, idl_def_json_length, output_path, output_path_length, guest_options, guest_options_length, out_err, out_err_len);
}
//--------------------------------------------------------------------
void language_plugin_interface_wrapper::compile_from_host(const char* idl_def_json, uint32_t idl_def_json_length,
                                                          const char* output_path, uint32_t output_path_length,
                                                          const char* host_options, uint32_t host_options_length,
                                                          char** out_err, uint32_t* out_err_len)
{
	*out_err = nullptr;
	*out_err_len = 0;
	(*this->pcompile_from_host)(idl_def_json, idl_def_json_length,
								output_path, output_path_length,
								host_options, host_options_length,
								out_err, out_err_len);
}
//--------------------------------------------------------------------
std::shared_ptr<language_plugin_interface_wrapper> language_plugin_interface_wrapper::load(const std::string& lang)
{
	std::stringstream compiler_plugin_name;
	compiler_plugin_name << "metaffi.compiler." << lang;
	
	std::shared_ptr<language_plugin_interface_wrapper> loaded_plugin;
	auto compiler_plugin_iter = compiler_plugins.find(compiler_plugin_name.str());
	if(compiler_plugin_iter == compiler_plugins.end())
	{
		loaded_plugin = std::make_shared<language_plugin_interface_wrapper>(compiler_plugin_name.str());
		loaded_plugin->init();
		
		compiler_plugins[compiler_plugin_name.str()] = loaded_plugin;
	}
	else
	{
		loaded_plugin = compiler_plugin_iter->second;
	}
	
	return loaded_plugin;
}
//--------------------------------------------------------------------
