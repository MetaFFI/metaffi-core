#include "runtime_plugin.h"
#include <iostream>
#include <utility>
#include <utils/scope_guard.hpp>
#include <algorithm>

using namespace metaffi::utils;

//--------------------------------------------------------------------
runtime_plugin::runtime_plugin(std::string plugin_filename, bool is_init /*= true*/):_plugin_filename(std::move(plugin_filename))
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
    this->_loaded_plugin->load_runtime(&err);
	
	if(err != nullptr)
	{
		scope_guard sg([&](){ free(err); });
		throw std::runtime_error(err);
	}

	_is_runtime_loaded = true;
}
//--------------------------------------------------------------------
void runtime_plugin::free_runtime() 
{
	// free all functions first
	std::vector<uint64_t> keys;
	for(auto& cur_mod : this->_loaded_entities){
		keys.push_back(cur_mod.first);
	}

	for(const auto& k : keys){
		this->free_and_remove_xcall_from_cache(k);
	}

	boost::upgrade_lock<boost::shared_mutex> read_lock(this->_mutex); // read lock

	// check if runtime has NOT been loaded
	if( !_is_runtime_loaded ){
		return; // runtime is NOT loaded
	}

	boost::upgrade_to_unique_lock<boost::shared_mutex> exclusive_lock(read_lock); // upgrade to writer

	// free runtime
    char* err = nullptr;
    this->_loaded_plugin->free_runtime(&err);

	if(err != nullptr)
	{
		scope_guard sg([&](){ free(err); });
		throw std::runtime_error(err);
	}

	_is_runtime_loaded = false;
}
//--------------------------------------------------------------------
std::shared_ptr<xcall> runtime_plugin::load_entity(const std::string& module_path, const std::string& function_path, const std::vector<metaffi_type_info>& params_types, const std::vector<metaffi_type_info>& retval_types)
{
	this->load_runtime(); // verify that runtime has been loaded
	
	boost::unique_lock<boost::shared_mutex> exclusive_lock(this->_mutex);

    char* err = nullptr;
    xcall* xcall_and_context = this->_loaded_plugin->load_entity(module_path.c_str(),
																	function_path.c_str(),
												                    !params_types.empty() ? (metaffi_type_info*)&params_types[0] : nullptr, params_types.size(),
												                    !retval_types.empty() ? (metaffi_type_info*)&retval_types[0] : nullptr, retval_types.size(),
																	&err);

	if(err != nullptr)
	{
		scope_guard sg([&](){ free(err); });
		throw std::runtime_error(err);
	}

	
	auto fmod = std::make_shared<xcall>(xcall_and_context);
	this->_loaded_entities[calc_key(*fmod)] = fmod;
	return fmod;

}
//--------------------------------------------------------------------
std::shared_ptr<xcall> runtime_plugin::make_callable(void* make_callable_context, const std::vector<metaffi_type_info>& params_types, const std::vector<metaffi_type_info>& retval_types)
{
	// no need to load runtime, this can be called only from the target runtime

	boost::unique_lock<boost::shared_mutex> exclusive_lock(this->_mutex);

	char* err = nullptr;
	xcall* xcall_and_context = this->_loaded_plugin->make_callable(make_callable_context,
	                                                                !params_types.empty() ? (metaffi_type_info*)&params_types[0] : nullptr, params_types.size(),
	                                                                !retval_types.empty() ? (metaffi_type_info*)&retval_types[0] : nullptr, retval_types.size(),
																	&err);

	if(err != nullptr)
	{
		scope_guard sg([&](){ free(err); });
		throw std::runtime_error(err);
	}

	auto fmod = std::make_shared<xcall>(xcall_and_context);
	this->_loaded_entities[calc_key(*fmod)] = fmod;
	return fmod;
}
//--------------------------------------------------------------------
void runtime_plugin::free_and_remove_xcall_from_cache(xcall* pxcall)
{
	if(pxcall == nullptr){
		throw std::runtime_error("pxcall is nullptr in runtime_plugin::free_and_remove_xcall_from_cache");
	}
	
	uint64_t key = calc_key(*pxcall);
	this->free_and_remove_xcall_from_cache(key);
}
//--------------------------------------------------------------------
void runtime_plugin::free_and_remove_xcall_from_cache(uint64_t xcall_cache_key)
{
	boost::upgrade_lock<boost::shared_mutex> read_lock(this->_mutex); // read lock

	// check if function is not loaded
	auto it = _loaded_entities.find(xcall_cache_key);
	if(it == _loaded_entities.end()){
		return;
	}

	boost::upgrade_to_unique_lock<boost::shared_mutex> exclusive_lock(read_lock); // upgrade to writer
    char* err = nullptr;
    this->_loaded_plugin->free_xcall(it->second.get(), &err);

	if(err != nullptr)
	{
		scope_guard sg([&](){ free(err); });
		throw std::runtime_error(err);
	}
	
	// remove from vector
	this->_loaded_entities.erase(it);
}
//--------------------------------------------------------------------
uint64_t runtime_plugin::calc_key(const xcall& pxcall)
{
	return (uint64_t)pxcall.pxcall_and_context[0] ^ (uint64_t)pxcall.pxcall_and_context[1];
}
//--------------------------------------------------------------------