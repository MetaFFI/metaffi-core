#pragma once
#include "xllr_plugin_interface.h"
#include <memory>
#include <type_traits>
#include <boost/dll.hpp>


//--------------------------------------------------------------------
class xllr_plugin_interface_wrapper : public xllr_plugin_interface
{
private:
	std::shared_ptr<boost::dll::detail::import_type<void(char**, uint32_t*)>::type> pload_runtime;
	std::shared_ptr<boost::dll::detail::import_type<void(char**, uint32_t*)>::type> pfree_runtime;

	std::shared_ptr<boost::dll::detail::import_type<void(const char*, uint32_t, char**, uint32_t*)>::type> pload_module;
	std::shared_ptr<boost::dll::detail::import_type<void(const char*, uint32_t, char**, uint32_t*)>::type> pfree_module;

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

	/**
	 * Load module of foreign language
	 */ 
	void load_module(const char* module, uint32_t module_len, char** err, uint32_t* err_len);

	/**
	 * Free module of foreign language
	 */ 
	void free_module(const char* module, uint32_t module_len, char** err, uint32_t* err_len);

private:
	template<typename T>
	std::shared_ptr<typename boost::dll::detail::import_type<T>::type> load_func(const std::string& fullpath, const std::string& funcname)
	{
		return std::make_shared<typename boost::dll::detail::import_type<T>::type>(
							boost::dll::import<T>(fullpath, funcname, boost::dll::load_mode::default_mode)
						);
	}
};
//--------------------------------------------------------------------
