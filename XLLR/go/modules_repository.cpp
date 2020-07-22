#include "modules_repository.h"

std::unique_ptr<modules_repository> modules_repository::instance;


//--------------------------------------------------------------------
modules_repository& modules_repository::get_instance()
{
	// IMPORTANT! This singleton is not thread-safe! If this needs to be thread-safe, use std::once.
	
	if(!modules_repository::instance)
	{
		modules_repository::instance = std::make_unique<modules_repository>();
	}
	
	return *modules_repository::instance;
}
//--------------------------------------------------------------------
void modules_repository::free_instance()
{
	modules_repository::instance = nullptr;
}
//--------------------------------------------------------------------
std::shared_ptr<boost::dll::shared_library> modules_repository::operator[](const std::string& module_name)
{
	auto it = this->modules.find(module_name);
	if(it != this->modules.end()){
		return it->second;
	}
	
	// load library and add to "modules"
	std::string module_filename = module_name + boost::dll::shared_library::suffix().generic_string();
	
	std::shared_ptr<boost::dll::shared_library> mod = std::make_shared<boost::dll::shared_library>();
	
	// if plugin exists in the same path of the program, load it from there (mainly used for easier development)
	// otherwise, search system folders
	if(boost::filesystem::exists( boost::filesystem::current_path().append(module_filename) ))
	{
		//std::cout << "Loading Go module from: " << boost::filesystem::current_path().append(plugin_filename) << std::endl;
		mod->load(boost::filesystem::current_path().append(module_filename) );
	}
	else if(boost::filesystem::exists( boost::dll::program_location().append(module_filename) ))
	{
		//std::cout << "Loading Go module from: " << boost::dll::program_location().append(module_filename) << std::endl;
		mod->load(boost::dll::program_location().append(module_filename) );
	}
	else
	{
		//std::cout << "Loading Go module from: " << module_filename << std::endl;
		mod->load(module_filename, boost::dll::load_mode::search_system_folders);
	}
	
	this->modules[module_name] = mod;
	
	return mod;
}
//--------------------------------------------------------------------