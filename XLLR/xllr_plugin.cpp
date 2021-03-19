#include "xllr_plugin.h"
#include <iostream>
#include <utils/scope_guard.hpp>
#include <algorithm>

using namespace openffi::utils;

//--------------------------------------------------------------------
xllr_plugin::xllr_plugin(const std::string& plugin_filename, bool is_init /*= true*/):_plugin_filename(plugin_filename)
{
	if(is_init){
		init();
	}
}
//--------------------------------------------------------------------
xllr_plugin::~xllr_plugin() 
{
	try
	{
		fini();
	}
	catch(const std::exception& e)
	{
		std::cout << "Failed to release " << this->_plugin_filename << ". Error: " << e.what() << std::endl;
	}
    catch(...)
	{
		std::cout << "Failed to release " << this->_plugin_filename << std::endl;
	}
}
//--------------------------------------------------------------------
void xllr_plugin::init(void) 
{
    // load dynamic library
	this->_loaded_plugin = std::make_unique<xllr_plugin_interface_wrapper>(this->_plugin_filename);
}
//--------------------------------------------------------------------
void xllr_plugin::fini(void)
{
	// make sure plugin is released before unloading the dynamic library
    this->free_runtime();
	this->_loaded_plugin = nullptr;
}
//--------------------------------------------------------------------
void xllr_plugin::load_runtime(void) 
{
	boost::upgrade_lock<boost::shared_mutex> read_lock(this->_mutex); // read lock
	// check if runtime has been loaded
	if( _is_runtime_loaded ){
		return; // runtime has been loaded
	}
	
	boost::upgrade_to_unique_lock<boost::shared_mutex> exclusive_lock(read_lock); // upgrade to writer

	char* err = nullptr;
	uint32_t err_len = 0;
    this->_loaded_plugin->load_runtime(&err, &err_len);
	
	if(err != nullptr)
	{
		scope_guard sg([&](){ free(err); });
		throw std::runtime_error(std::string(err, err_len));
	}

	_is_runtime_loaded = true;
}
//--------------------------------------------------------------------
void xllr_plugin::free_runtime(void) 
{
	// free all modules first
	std::vector<std::string> keys(this->_loaded_modules.size());
	for(auto& cur_mod : this->_loaded_modules){
		keys.push_back(cur_mod.first);
	}

	for(const std::string& k : keys){
		this->free_module(k);
	}

	boost::upgrade_lock<boost::shared_mutex> read_lock(this->_mutex); // read lock

	// check if runtime has NOT been loaded
	if( !_is_runtime_loaded ){
		return; // runtime is NOT loaded
	}

	boost::upgrade_to_unique_lock<boost::shared_mutex> exclusive_lock(read_lock); // upgrade to writer

	// free runtime
    char* err = nullptr;
	uint32_t err_len = 0;
    this->_loaded_plugin->free_runtime(&err, &err_len);

	if(err != nullptr)
	{
		scope_guard sg([&](){ free(err); });
		throw std::runtime_error(std::string(err, err_len));
	}

	_is_runtime_loaded = false;
}
//--------------------------------------------------------------------
std::shared_ptr<foreign_module> xllr_plugin::get_module(const std::string& module_name) const
{
	boost::upgrade_lock<boost::shared_mutex> read_lock(this->_mutex); // read lock

	// check if module already been loaded
	auto it = _loaded_modules.find(module_name);
	if(it == _loaded_modules.end()){
		return nullptr;
	}

	return it->second;
}
//--------------------------------------------------------------------
std::shared_ptr<foreign_module> xllr_plugin::load_module(const std::string& module_name) 
{
	this->load_runtime(); // verify that runtime has been loaded

	boost::upgrade_lock<boost::shared_mutex> read_lock(this->_mutex); // read lock

	// check if module already been loaded
	auto it = _loaded_modules.find(module_name);
	if(it != _loaded_modules.end()){
		return it->second;
	}

	boost::upgrade_to_unique_lock<boost::shared_mutex> exclusive_lock(read_lock); // upgrade to writer

    char* err = nullptr;
	uint32_t err_len = 0;
    this->_loaded_plugin->load_module(module_name.c_str(), module_name.length(), &err, &err_len);

	if(err != nullptr)
	{
		scope_guard sg([&](){ free(err); });
		throw std::runtime_error(std::string(err, err_len));
	}

	// insert sorted (because of binary search)
	auto fmod = std::make_shared<foreign_module>(this->_loaded_plugin, module_name);
	this->_loaded_modules[module_name] = fmod;
	return fmod;

}
//--------------------------------------------------------------------
void xllr_plugin::free_module(const std::string& module_name) 
{
	boost::upgrade_lock<boost::shared_mutex> read_lock(this->_mutex); // read lock

	// check if module is not loaded
	auto it = _loaded_modules.find(module_name);
	if(it == _loaded_modules.end()){
		return;
	}

	boost::upgrade_to_unique_lock<boost::shared_mutex> exclusive_lock(read_lock); // upgrade to writer
    char* err = nullptr;
	uint32_t err_len = 0;
    this->_loaded_plugin->free_module(module_name.c_str(), module_name.length(), &err, &err_len);

	if(err != nullptr)
	{
		scope_guard sg([&](){ free(err); });
		throw std::runtime_error(std::string(err, err_len));
	}
	
	// remove from vector
	this->_loaded_modules.erase(it);
}
//--------------------------------------------------------------------