#pragma once
#include "xllr_plugin_interface.h"
#include <memory>
#include <type_traits>
#include <boost/dll.hpp>


//--------------------------------------------------------------------
class xllr_plugin_interface_wrapper : public xllr_plugin_interface
{
private:
	std::shared_ptr<boost::dll::detail::import_type<void(char**,uint32_t*)>::type> pload_runtime;
	std::shared_ptr<boost::dll::detail::import_type<void(char**,uint32_t*)>::type> pfree_runtime;

public:
	explicit xllr_plugin_interface_wrapper(const std::string& plugin_filename);

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
