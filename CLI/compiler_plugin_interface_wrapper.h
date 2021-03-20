#pragma once
#include <compiler/compiler_plugin_interface.h>
#include <memory>
#include <type_traits>
#include <boost/dll.hpp>

//--------------------------------------------------------------------
class compiler_plugin_interface_wrapper : public compiler_plugin_interface
{
private:
	std::shared_ptr<boost::dll::detail::import_type<void(idl_definition*, const char*, uint32_t, const char*, uint32_t, char**, uint32_t*)>::type> pcompile_to_guest;
	std::shared_ptr<boost::dll::detail::import_type<void(idl_definition*, const char*, uint32_t, const char*, uint32_t, char**, uint32_t*)>::type> pcompile_from_host;

public:
	explicit compiler_plugin_interface_wrapper(const std::string& plugin_filename_without_extension);

	/**
	 * Compile IDL to code called from XLLR to the foreign function
	 */ 
	void compile_to_guest(idl_definition* idl_def,
	                      const char* output_path, uint32_t output_path_length,
	                      const char* serialization_code, uint32_t serialization_code_length,
	                      char** out_err, uint32_t* out_err_len) override;

	/**
	 * Compile IDL to code calling to XLLR from host code
	 */ 
	void compile_from_host(idl_definition* idl_def,
	                       const char* output_path, uint32_t output_path_length,
	                       const char* serialization_code, uint32_t serialization_code_length,
	                       char** out_err, uint32_t* out_err_len) override;
	
};
//--------------------------------------------------------------------
