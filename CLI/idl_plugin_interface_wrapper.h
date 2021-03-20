#pragma once
#include <compiler/idl_plugin_interface.h>
#include <memory>
#include <type_traits>
#include <boost/dll.hpp>

//--------------------------------------------------------------------
class idl_plugin_interface_wrapper : public idl_plugin_interface
{
private:
	std::shared_ptr<boost::dll::detail::import_type<idl_definition*(const char*, uint32_t, const char*, uint32_t, char**, uint32_t*)>::type> pparse_idl;

public:
	explicit idl_plugin_interface_wrapper(const std::string& plugin_filename_without_extension);

	/**
	 * Compile IDL to code called from XLLR to the foreign function
	 */
	idl_definition* parse_idl(const char* idl_name, uint32_t idl_name_length,
	               const char* idl, uint32_t idl_length,
	               char** out_err, uint32_t* out_err_len) override;
};
//--------------------------------------------------------------------
