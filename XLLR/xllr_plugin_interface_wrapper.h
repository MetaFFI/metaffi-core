#pragma once
#include "../../plugin-sdk/xllr_plugin_interface.h"
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

	std::shared_ptr<boost::dll::detail::import_type<void(const char*, uint32_t,
														 const char*, uint32_t,
														 unsigned char*, uint64_t,
														 unsigned char**, uint64_t*,
														 unsigned char**, uint64_t*,
														 uint8_t*)>::type> pcall;

public:
	explicit xllr_plugin_interface_wrapper(const std::string& plugin_filename_without_extension);

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

	/***
	 * Call foreign function
	 */
	void call(
			// module to load
			const char* module_name, uint32_t module_name_len,
			
			// function name to call
			const char* func_name, uint32_t func_name_len,
			
			// serialized parameters
			unsigned char* in_params, uint64_t in_params_len,

			// serialized returned ref parameters
			unsigned char** out_params, uint64_t* out_params_len,
			
			// out - serialized result or error message
			unsigned char** out_ret, uint64_t* out_ret_len,
			
			// out - 0 if not an error, otherwise an error
			uint8_t* out_is_error
	);

};
//--------------------------------------------------------------------
