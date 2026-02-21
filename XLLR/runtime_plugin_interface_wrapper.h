#pragma once
#include "runtime/runtime_plugin_interface.h"
#include <memory>
#include <type_traits>
#include <utils/boost_dll_compat.hpp>

//--------------------------------------------------------------------
class runtime_plugin_interface_wrapper : public runtime_plugin_interface
{
private:
	std::shared_ptr<boost::dll::shared_library> plugin_dll;
	std::shared_ptr<metaffi::utils::boost_dll_import_t<void(char**)>> pload_runtime;
	std::shared_ptr<metaffi::utils::boost_dll_import_t<void(char**)>> pfree_runtime;

	std::shared_ptr<metaffi::utils::boost_dll_import_t<xcall*(const char*, const char*, struct metaffi_type_info*, int8_t, struct metaffi_type_info*, int8_t, char**)>> pload_entity;
	std::shared_ptr<metaffi::utils::boost_dll_import_t<xcall*(void*, struct metaffi_type_info*, int8_t, struct metaffi_type_info*, int8_t, char**)>> pmake_callable;
	std::shared_ptr<metaffi::utils::boost_dll_import_t<void(xcall*, char**)>> pfree_xcall;
	
public:
	explicit runtime_plugin_interface_wrapper(const std::string& plugin_filename_without_extension);
	explicit runtime_plugin_interface_wrapper(const char* plugin_filename_without_extension);

	/**
	 * Load runtime runtime of foreign runtime
	 */ 
	void load_runtime(char** err) override;

	/**
	 * Free runtime runtime of foreign runtime
	 */ 
	void free_runtime(char** err) override;

	/**
	 * Load module of foreign language
	 */ 
	[[nodiscard]] xcall* load_entity(const char* module_path, const char* entity_path, struct metaffi_type_info* params_types, uint8_t params_count, struct metaffi_type_info* retval_types, uint8_t retval_count, char** err) override;

	/**
	* Load module of foreign language
	*/
	[[nodiscard]] xcall* make_callable(void* make_callable_context, struct metaffi_type_info* params_types, uint8_t params_count, struct metaffi_type_info* retval_types, uint8_t retval_count, char** err) override;


	/**
	 * Free module of foreign language
	 */ 
	void free_xcall(xcall* pxcall, char** err) override;
	
	void fini();
};
//--------------------------------------------------------------------
