#include "xllr_plugin.h"
#include <iostream>
#include <scope_guard.hpp>

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
	catch(std::exception& e)
	{
		std::cerr << "Failed to release " << this->_plugin_filename << ". Error: " << e.what() << std::endl;
	}
    catch(...)
	{
		std::cerr << "Failed to release " << this->_plugin_filename << std::endl;
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
	char* err = nullptr;
	uint32_t err_len = 0;
    this->_loaded_plugin->load_runtime(&err, &err_len);

	if(err != nullptr)
	{
		scope_guard sg([&](){ free(err); });
		throw std::runtime_error(std::string(err, err_len));
	}
}
//--------------------------------------------------------------------
void xllr_plugin::free_runtime(void) 
{
    char* err = nullptr;
	uint32_t err_len = 0;
    this->_loaded_plugin->free_runtime(&err, &err_len);

	if(err != nullptr)
	{
		scope_guard sg([&](){ free(err); });
		throw std::runtime_error(std::string(err, err_len));
	}
}
//--------------------------------------------------------------------
void xllr_plugin::load_module(const std::string& module_name) 
{
    
}
//--------------------------------------------------------------------
void xllr_plugin::free_module(const std::string& module_name) 
{
    
}
//--------------------------------------------------------------------