#pragma once
#include <stdint.h>
#include "xllr_api.h"

/**
 * Interface XLLR Plugin Implements
 */ 
struct xllr_plugin_interface
{
	/**
	 * Load runtime runtime of foreign runtime
	 */ 
	virtual void load_runtime(char** err, uint32_t* err_len) = 0;

	/**
	 * Free runtime runtime of foreign runtime
	 */ 
	virtual void free_runtime(char** err, uint32_t* err_len) = 0;

	/**
	 * Load module of foreign language
	 */ 
	virtual void load_module(const char* module, uint32_t module_len, char** err, uint32_t* err_len) = 0;

	/**
	 * Free module of foreign language
	 */ 
	virtual void free_module(const char* module, uint32_t module_len, char** err, uint32_t* err_len) = 0;

	/***
	 * Call foreign function
	 */
/*	virtual void call(
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
	) = 0;
*/
	/***
	 * Call foreign function and lazy load request module (if ot loaded yet)
	 */
/*	virtual module_handle call_lazy(
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
	) = 0;
*/
};

