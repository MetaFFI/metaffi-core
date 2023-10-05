#include "idl_extrator.h"
#include "idl_plugin_interface_wrapper.h"
#include <regex>
#include <iostream>
#include <string>
#include <utils/expand_env.h>
#include <filesystem>

//--------------------------------------------------------------------
std::string idl_extractor::extract(const std::string& idl_path, const std::string& user_selected_idl_plugin)
{
	std::string expanded_idl_path = metaffi::utils::expand_env(idl_path);
	std::filesystem::path fs_idl_path = std::filesystem::path(expanded_idl_path);
	
	// load IDL plugin
	std::shared_ptr<idl_plugin_interface_wrapper> pidl_plugin;
	if(user_selected_idl_plugin.empty()) // if user didn't choose idl-plugin, try to determine using IDL path extension
	{
		if(std::filesystem::is_directory(fs_idl_path))
		{
			throw std::runtime_error("If given IDL is a directory, you must specify IDL plugin using -idl-plugin switch");
		}
		
		std::string extension = fs_idl_path.extension().string();
		
		std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c){ return std::tolower(c); });
		extension.erase(0, 1); // remove the "." at the beginning of the extension
		
		pidl_plugin = extension == "json" ?
				std::make_shared<json_idl_plugin>() :
		        idl_plugin_interface_wrapper::load(extension); // get IDL by given IDL file extension
	}
	else
	{
		std::string user_selected = user_selected_idl_plugin;
		std::transform(user_selected.begin(), user_selected.end(), user_selected.begin(), [](unsigned char c){ return std::tolower(c); });
		
		pidl_plugin = user_selected == "json" ?
              std::make_shared<json_idl_plugin>() :
              idl_plugin_interface_wrapper::load(user_selected); // get IDL by given IDL file extension
	}
	
	// if IDL is a file, read file contents and send to plugin as "idl",
	// otherwise, place in "idl" full path name.
	std::string idl_json;
	std::string err;

    pidl_plugin->parse_idl("", expanded_idl_path, idl_json, err);
	
	if(!err.empty())
	{
		throw std::runtime_error(err);
	}
	
	return idl_json;
}
//--------------------------------------------------------------------