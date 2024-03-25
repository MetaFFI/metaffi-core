#include "runtime_plugin.h"
#include <iostream>
#include <utils/scope_guard.hpp>
#include <algorithm>

using namespace metaffi::utils;

//--------------------------------------------------------------------
runtime_plugin::runtime_plugin(const std::string& plugin_filename, bool is_init /*= true*/):_plugin_filename(plugin_filename)
{
	if(is_init){
		init();
	}
}
//--------------------------------------------------------------------
runtime_plugin::~runtime_plugin() 
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
void runtime_plugin::init() 
{
    // load dynamic library
	this->_loaded_plugin = std::make_unique<runtime_plugin_interface_wrapper>(this->_plugin_filename);
}
//--------------------------------------------------------------------
void runtime_plugin::fini()
{
	// make sure plugin is released before unloading the dynamic library
    this->free_runtime();
	this->_loaded_plugin = nullptr;
}
//--------------------------------------------------------------------
void runtime_plugin::load_runtime() 
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
void runtime_plugin::free_runtime() 
{
	// free all functions first
	std::vector<void**> keys(this->_loaded_functions.size());
	for(auto& cur_mod : this->_loaded_functions){
		keys.push_back(cur_mod.first);
	}

	for(const auto& k : keys){
		this->free_function(k);
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
std::shared_ptr<foreign_function> runtime_plugin::get_function(void** pff) const
{
	boost::upgrade_lock<boost::shared_mutex> read_lock(this->_mutex); // read lock

	// check if module already been loaded
	auto it = _loaded_functions.find(pff);
	if(it == _loaded_functions.end()){
		return nullptr;
	}

	return it->second;
}
//--------------------------------------------------------------------
std::shared_ptr<foreign_function> runtime_plugin::load_function(const std::string& module_path, const std::string& function_path, const std::vector<metaffi_type_info>& params_types, const std::vector<metaffi_type_info>& retval_types)
{
	this->load_runtime(); // verify that runtime has been loaded
	
	boost::unique_lock<boost::shared_mutex> exclusive_lock(this->_mutex);

    char* err = nullptr;
	uint32_t err_len = 0;
    void** pxcall_and_context = this->_loaded_plugin->load_function(module_path.c_str(), module_path.length(),
												function_path.c_str(), function_path.length(),
												                    !params_types.empty() ? (metaffi_type_info*)&params_types[0] : nullptr,
												                    !retval_types.empty() ? (metaffi_type_info*)&retval_types[0] : nullptr,
												params_types.size(), retval_types.size(), &err, &err_len);

	if(err != nullptr)
	{
		scope_guard sg([&](){ free(err); });
		throw std::runtime_error(std::string(err, err_len));
	}

	// insert sorted (because of binary search)
	auto fmod = std::make_shared<foreign_function>(this->_loaded_plugin, pxcall_and_context);
	this->_loaded_functions[pxcall_and_context] = fmod;
	return fmod;

}
//--------------------------------------------------------------------
std::shared_ptr<foreign_function> runtime_plugin::make_callable(void* make_callable_context, const std::vector<metaffi_type_info>& params_types, const std::vector<metaffi_type_info>& retval_types)
{
	// no need to load runtime, this can be called only from the target runtime

	boost::unique_lock<boost::shared_mutex> exclusive_lock(this->_mutex);

	char* err = nullptr;
	uint32_t err_len = 0;
	void** pxcall_and_context = this->_loaded_plugin->make_callable(make_callable_context,
	                                                                !params_types.empty() ? (metaffi_type_info*)&params_types[0] : nullptr,
	                                                                !retval_types.empty() ? (metaffi_type_info*)&retval_types[0] : nullptr,
												params_types.size(), retval_types.size(), &err, &err_len);

	if(err != nullptr)
	{
		scope_guard sg([&](){ free(err); });
		throw std::runtime_error(std::string(err, err_len));
	}

	// insert sorted (because of binary search)
	auto fmod = std::make_shared<foreign_function>(this->_loaded_plugin, pxcall_and_context);
	this->_loaded_functions[pxcall_and_context] = fmod;
	return fmod;
}
//--------------------------------------------------------------------
void runtime_plugin::free_function(void** pff)
{
	boost::upgrade_lock<boost::shared_mutex> read_lock(this->_mutex); // read lock

	// check if function is not loaded
	auto it = _loaded_functions.find(pff);
	if(it == _loaded_functions.end()){
		return;
	}

	boost::upgrade_to_unique_lock<boost::shared_mutex> exclusive_lock(read_lock); // upgrade to writer
    char* err = nullptr;
	uint32_t err_len = 0;
    this->_loaded_plugin->free_function(pff, &err, &err_len);

	if(err != nullptr)
	{
		scope_guard sg([&](){ free(err); });
		throw std::runtime_error(std::string(err, err_len));
	}
	
	// remove from vector
	this->_loaded_functions.erase(it);
}
//--------------------------------------------------------------------