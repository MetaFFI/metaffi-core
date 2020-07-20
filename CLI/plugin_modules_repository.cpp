#include "plugin_modules_repository.h"

std::unique_ptr<plugin_modules_repository> plugin_modules_repository::instance;


//--------------------------------------------------------------------
plugin_modules_repository& plugin_modules_repository::get_instance()
{
	// IMPORTANT! This singleton is not thread-safe! If this needs to be thread-safe, use std::once.
	
	if(!plugin_modules_repository::instance)
	{
		plugin_modules_repository::instance = std::make_unique<plugin_modules_repository>();
	}
	
	return *plugin_modules_repository::instance;
}
//--------------------------------------------------------------------
void plugin_modules_repository::free_instance()
{
	plugin_modules_repository::instance = nullptr;
}
//--------------------------------------------------------------------
std::shared_ptr<boost::dll::shared_library> plugin_modules_repository::operator[](const std::string& plugin_name)
{
	auto it = this->modules.find(plugin_name);
	if(it != this->modules.end()){
		return it->second;
	}
	
	// load library and add to "modules"
	std::string plugin_filename = plugin_name+boost::dll::shared_library::suffix().generic_string();
	
	std::shared_ptr<boost::dll::shared_library> plugin_dll = std::make_shared<boost::dll::shared_library>();
	
	// if plugin exists in the same path of the program, load it from there (mainly used for easier development)
	// otherwise, search system folders
	if(boost::filesystem::exists( boost::filesystem::current_path().append(plugin_filename) ))
	{
		//std::cout << "Loading Functions from: " << boost::filesystem::current_path().append(plugin_filename) << std::endl;
		plugin_dll->load( boost::filesystem::current_path().append(plugin_filename) );
	}
	else
	{
		//std::cout << "Loading Functions from: " << plugin_filename << std::endl;
		plugin_dll->load(plugin_filename, boost::dll::load_mode::search_system_folders);
	}
	
	this->modules[plugin_name] = plugin_dll;
	
	return plugin_dll;
}
//--------------------------------------------------------------------