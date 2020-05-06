#pragma once
#include "xllr_plugin_interface.h"
#include <memory>
#include <boost/dll.hpp>


//--------------------------------------------------------------------
class xllr_plugin_interface_wrapper : public xllr_plugin_interface
{
public:
	xllr_plugin_interface_wrapper(const std::string& plugin_filename);

	/**
	 * Load runtime runtime of foreign runtime
	 */ 
	void load_runtime(char** err, uint32_t* err_len);

	/**
	 * Free runtime runtime of foreign runtime
	 */ 
	void free_runtime(char** err, uint32_t* err_len);
	
};
//--------------------------------------------------------------------
