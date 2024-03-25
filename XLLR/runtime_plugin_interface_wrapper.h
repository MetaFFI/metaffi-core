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

	std::shared_ptr<boost::dll::detail::import_type<void**(const char*, uint32_t, const char*, uint32_t, struct metaffi_type_info*, struct metaffi_type_info*, int8_t, int8_t, char**, uint32_t*)>::type> pload_function;
	std::shared_ptr<boost::dll::detail::import_type<void**(void*, struct metaffi_type_info*, struct metaffi_type_info*, int8_t, int8_t, char**, uint32_t*)>::type> pmake_callable;
	std::shared_ptr<boost::dll::detail::import_type<void(void*, char**, uint32_t*)>::type> pfree_function;

public:
	explicit runtime_plugin_interface_wrapper(const std::string& plugin_filename_without_extension);

	/**
	 * Load runtime runtime of foreign runtime
	 */ 
	void load_runtime(char** err, uint32_t* err_len) override;

	/**
	 * Free runtime runtime of foreign rnbuntime
	 */ 
	void free_runtime(char** err, uint32_t* err_len) override;

	/**
	 * Load module of foreign language
	 */ 
	[[nodiscard]] void** load_function(const char* module_path, uint32_t module_path_len, const char* function_path, uint32_t function_path_len, struct metaffi_type_info* params_types, struct metaffi_type_info* retval_types, uint8_t params_count, uint8_t retval_count, char** err, uint32_t* err_len) override;

	/**
	* Load module of foreign language
	*/
	[[nodiscard]] void** make_callable(void* make_callable_context, struct metaffi_type_info* params_types, struct metaffi_type_info* retval_types, uint8_t params_count, uint8_t retval_count, char** err, uint32_t* err_len) override;


	/**
	 * Free module of foreign language
	 */ 
	void free_function(void* pff, char** err, uint32_t* err_len) override;


};
//--------------------------------------------------------------------
