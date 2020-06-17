#pragma once
#include "compiler_plugin_interface.h"
#include <memory>
#include <type_traits>
#include <boost/dll.hpp>

//--------------------------------------------------------------------
class compiler_plugin_interface_wrapper : public compiler_plugin_interface
{
private:
	std::shared_ptr<boost::dll::detail::import_type<void(const char*, uint32_t, const char*, uint32_t, char**, uint32_t*)>::type> pcompile_to_guest;
	std::shared_ptr<boost::dll::detail::import_type<void(const char*, uint32_t, const char*, uint32_t, char**, uint32_t*)>::type> pcompile_from_host;

public:
	explicit compiler_plugin_interface_wrapper(const std::string& plugin_filename);

	/**
	 * Compile IDL to code called from XLLR to the foreign function
	 */ 
	void compile_to_guest(const char* idl_path, uint32_t idl_path_length, const char* output_path, uint32_t output_path_length, char** out_err, uint32_t* out_err_len) override;

	/**
	 * Compile IDL to code calling to XLLR from host code
	 */ 
	void compile_from_host(const char* idl_path, uint32_t idl_path_length, const char* output_path, uint32_t output_path_length, char** out_err, uint32_t* out_err_len) override;

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
