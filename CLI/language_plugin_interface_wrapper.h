#pragma once
#include <compiler/language_plugin_interface.h>
#include <memory>
#include <type_traits>
#include <boost/dll.hpp>

//--------------------------------------------------------------------
class language_plugin_interface_wrapper : public language_plugin_interface
{
private:
	std::shared_ptr<boost::dll::detail::import_type<void(void)>::type> pinit;
	std::shared_ptr<boost::dll::detail::import_type<void(const char*, uint32_t, const char*, uint32_t, const char*, uint32_t, char**, uint32_t*)>::type> pcompile_to_guest;
	std::shared_ptr<boost::dll::detail::import_type<void(const char*, uint32_t, const char*, uint32_t, const char*, uint32_t, char**, uint32_t*)>::type> pcompile_from_host;

public:
	explicit language_plugin_interface_wrapper(const std::string& plugin_filename_without_extension);

	void init() override;
	
	/**
	 * Compile IDL to code called from XLLR to the foreign function
	 */ 
	void compile_to_guest(const char* idl_def_json, uint32_t idl_def_json_length,
	                      const char* output_path, uint32_t output_path_length,
	                      const char* serialization_code, uint32_t serialization_code_length,
	                      char** out_err, uint32_t* out_err_len) override;

	/**
	 * Compile IDL to code calling to XLLR from host code
	 */ 
	void compile_from_host(const char* idl_def_json, uint32_t idl_def_json_length,
	                       const char* output_path, uint32_t output_path_length,
	                       const char* serialization_code, uint32_t serialization_code_length,
	                       char** out_err, uint32_t* out_err_len) override;
	
};
//--------------------------------------------------------------------
