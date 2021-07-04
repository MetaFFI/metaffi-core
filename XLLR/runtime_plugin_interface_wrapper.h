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
	                                                     cdt*, uint64_t,
	                                                     cdt*, uint64_t,
	                                                     char**, uint64_t*)>::type> pcall;

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
			int64_t function_id,
			cdt* parameters, uint64_t parameters_len,
			cdt* return_values, uint64_t return_values_len,
			char** out_err, uint64_t* out_err_len
	) override;

};
//--------------------------------------------------------------------
