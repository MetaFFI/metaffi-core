#include "xllr_plugin.h"
#include <iostream>

//--------------------------------------------------------------------
xllr_plugin::xllr_plugin(const std::string& plugin_filename):_plugin_filename(plugin_filename)
{
	load();
}
//--------------------------------------------------------------------
xllr_plugin::~xllr_plugin() 
{
	try
	{
		release();
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
void xllr_plugin::load(void) 
{
    // load SO/DLL/DynLib
	this->_loaded_plugin = std::make_unique<xllr_plugin_interface_wrapper>(this->_plugin_filename);
}
//--------------------------------------------------------------------
void xllr_plugin::release(void)
{
    
}
//--------------------------------------------------------------------
void xllr_plugin::load_runtime(void) 
{
    
}
//--------------------------------------------------------------------
void xllr_plugin::release_runtime(void) 
{
    
}
//--------------------------------------------------------------------
void xllr_plugin::load_module(const std::string& module_name) 
{
    
}
//--------------------------------------------------------------------
/*
foreign_module& xllr_plugin::operator[](const std::string& module_name) 
{
    
}*/
//--------------------------------------------------------------------