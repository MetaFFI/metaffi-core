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
		char* err = fini();
		if(err != nullptr)
		{
			std::cerr << "Failed to release " << this->_plugin_filename << ". Error: " << err << std::endl;
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << "Failed to release " << this->_plugin_filename << ". Error: " << e.what() << std::endl;
	}
    catch(...)
	{
		std::cerr << "Failed to release " << this->_plugin_filename << std::endl;
	}
}
//--------------------------------------------------------------------
void runtime_plugin::init() 
{
    // load dynamic library
	this->_loaded_plugin = std::make_unique<runtime_plugin_interface_wrapper>(this->_plugin_filename);
}
//--------------------------------------------------------------------
char* runtime_plugin::fini()
{
	// make sure plugin is released before unloading the dynamic library
	char* err = nullptr;
    this->free_runtime(&err);
	if(err != nullptr)
	{
		std::cerr << "Failed to free runtime. Error: " << err << std::endl;
		return err;
	}
	
	this->_loaded_plugin = nullptr;
	return nullptr;
}
//--------------------------------------------------------------------
void runtime_plugin::load_runtime(char** out_err)
{
	*out_err = nullptr;
	boost::upgrade_lock<boost::shared_mutex> read_lock(this->_mutex); // read lock
	// check if runtime has been loaded
	if( _is_runtime_loaded ){
		return; // runtime has been loaded
	}
	
	boost::upgrade_to_unique_lock<boost::shared_mutex> exclusive_lock(read_lock); // upgrade to writer

    this->_loaded_plugin->load_runtime(out_err);
	
	if(*out_err != nullptr)
	{
		return;
	}

	_is_runtime_loaded = true;
}
//--------------------------------------------------------------------
void runtime_plugin::free_runtime(char** out_err)
{
	boost::upgrade_lock<boost::shared_mutex> read_lock(this->_mutex); // read lock

	// check if runtime has NOT been loaded
	if( !_is_runtime_loaded ){
		return; // runtime is NOT loaded
	}

	boost::upgrade_to_unique_lock<boost::shared_mutex> exclusive_lock(read_lock); // upgrade to writer

	// free runtime
    this->_loaded_plugin->free_runtime(out_err);

	if(*out_err != nullptr)
	{
		return;
	}

	_is_runtime_loaded = false;
}
//--------------------------------------------------------------------
xcall* runtime_plugin::load_entity(const std::string& module_path, const std::string& function_path, const std::vector<metaffi_type_info>& params_types, const std::vector<metaffi_type_info>& retval_types, char** out_err)
{
	*out_err = nullptr;
	this->load_runtime(out_err); // verify that runtime has been loaded
	
	boost::unique_lock<boost::shared_mutex> exclusive_lock(this->_mutex);

    xcall* xcall_and_context = this->_loaded_plugin->load_entity(module_path.c_str(),
																	function_path.c_str(),
												                    !params_types.empty() ? (metaffi_type_info*)&params_types[0] : nullptr, params_types.size(),
												                    !retval_types.empty() ? (metaffi_type_info*)&retval_types[0] : nullptr, retval_types.size(),
																	out_err);

	if(*out_err != nullptr)
	{
		return nullptr;
	}

	
	return xcall_and_context;
}
//--------------------------------------------------------------------
xcall* runtime_plugin::make_callable(void* make_callable_context, const std::vector<metaffi_type_info>& params_types, const std::vector<metaffi_type_info>& retval_types, char** out_err)
{
	// no need to load runtime, this can be called only from the target runtime
	*out_err = nullptr;
	boost::unique_lock<boost::shared_mutex> exclusive_lock(this->_mutex);

	xcall* xcall_and_context = this->_loaded_plugin->make_callable(make_callable_context,
	                                                                !params_types.empty() ? (metaffi_type_info*)&params_types[0] : nullptr, params_types.size(),
	                                                                !retval_types.empty() ? (metaffi_type_info*)&retval_types[0] : nullptr, retval_types.size(),
																	out_err);

	if(*out_err != nullptr)
	{
		return nullptr;
	}

	return xcall_and_context;
}
//--------------------------------------------------------------------
void runtime_plugin::free_xcall(xcall* xcall_key, char** out_err)
{
	this->_loaded_plugin->free_xcall(xcall_key, out_err);
}
//--------------------------------------------------------------------