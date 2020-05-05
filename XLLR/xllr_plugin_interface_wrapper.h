#pragma once
#include "xllr_plugin_interface.h"
#include <memory>


//--------------------------------------------------------------------
class xllr_plugin_interface_wrapper : public xllr_plugin_interface
{
private:
	xllr_plugin_interface_wrapper();

public:
	static std::unique_ptr<xllr_plugin_interface> load_plugin(const std::string& language);

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
	module_handle load_module(const char* module, uint32_t module_len, char** err, uint32_t* err_len);

	/**
	 * Free module of foreign language
	 */ 
	void free_module(module_handle module, char** err, uint32_t* err_len);

	/***
	 * Call foreign function
	 */
	void call(
			// module to load
			module_handle module,
			
			// function name to call
			const char* func_name, uint32_t func_name_len,
			
			// serialized parameters
			unsigned char* params, uint64_t params_len,

			// serialized out parameters
			unsigned char** out_params, uint64_t* out_params_len,
			
			// out - serialized result or error message
			char** out_ret, uint64_t* out_ret_len,
			
			// out - 0 if not an error, otherwise an error
			uint8_t* is_error
	);

	/***
	 * Call foreign function and lazy load request module (if ot loaded yet)
	 */
	module_handle call_lazy(
			// module to load
			const char* module_name, uint32_t module_name_len,
			
			// function name to call
			const char* func_name, uint32_t func_name_len,
			
			// serialized parameters
			unsigned char* params, uint64_t params_len,

			// serialized out parameters
			unsigned char** out_params, uint64_t* out_params_len,
			
			// out - serialized result or error message
			char** out_ret, uint64_t* out_ret_len,
			
			// out - 0 if not an error, otherwise an error
			uint8_t* is_error
	);
};
//--------------------------------------------------------------------
