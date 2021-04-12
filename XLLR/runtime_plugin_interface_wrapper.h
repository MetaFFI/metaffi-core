#pragma once
#include "runtime/runtime_plugin_interface.h"
#include <memory>
#include <type_traits>
#include <boost/dll.hpp>

//--------------------------------------------------------------------
class runtime_plugin_interface_wrapper : public runtime_plugin_interface
{
private:
	std::shared_ptr<boost::dll::detail::import_type<void(char**, uint32_t*)>::type> pload_runtime;
	std::shared_ptr<boost::dll::detail::import_type<void(char**, uint32_t*)>::type> pfree_runtime;

	std::shared_ptr<boost::dll::detail::import_type<int64_t(const char*, uint32_t, char**, uint32_t*)>::type> pload_function;
	std::shared_ptr<boost::dll::detail::import_type<void(int64_t, char**, uint32_t*)>::type> pfree_function;

	std::shared_ptr<boost::dll::detail::import_type<void(int64_t,
														 unsigned char*, uint64_t,
														 unsigned char**, uint64_t*,
														 unsigned char**, uint64_t*,
														 uint8_t*)>::type> pcall;

public:
	explicit runtime_plugin_interface_wrapper(const std::string& plugin_filename_without_extension);

	/**
	 * Load runtime runtime of foreign runtime
	 */ 
	void load_runtime(char** err, uint32_t* err_len) override;

	/**
	 * Free runtime runtime of foreign runtime
	 */ 
	void free_runtime(char** err, uint32_t* err_len) override;

	/**
	 * Load module of foreign language
	 */ 
	[[nodiscard]] int64_t load_function(const char* function_path, uint32_t function_path_len, char** err, uint32_t* err_len) override;

	/**
	 * Free module of foreign language
	 */ 
	void free_function(int64_t function_id, char** err, uint32_t* err_len) override;

	/***
	 * Call foreign function
	 */
	void call(
			//function id to call
			int64_t function_id,
			
			// serialized parameters
			unsigned char* in_params, uint64_t in_params_len,

			// serialized returned ref parameters
			unsigned char** out_params, uint64_t* out_params_len,
			
			// out - serialized result or error message
			unsigned char** out_ret, uint64_t* out_ret_len,
			
			// out - 0 if not an error, otherwise an error
			uint8_t* out_is_error
	) override;

};
//--------------------------------------------------------------------
