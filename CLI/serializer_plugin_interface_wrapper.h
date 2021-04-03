#pragma once
#include <compiler/serializer_plugin_interface.h>
#include <memory>
#include <type_traits>
#include <boost/dll.hpp>

//--------------------------------------------------------------------
class serializer_plugin_interface_wrapper : public serializer_plugin_interface
{
private:
	std::shared_ptr<boost::dll::detail::import_type<void(void)>::type> pinit_plugin;
	std::shared_ptr<boost::dll::detail::import_type<void(const char*, uint32_t, const char*, uint32_t, char**, uint32_t*, char**, uint32_t*)>::type> pcompile_serialization;

public:
	explicit serializer_plugin_interface_wrapper(const std::string& serializer_extension);

	void init() override;
	
	/**
	 * Compile IDL to code called from XLLR to the foreign function
	 */
	void compile_serialization(const char* idl, uint32_t idl_length,
	                           const char* language, uint32_t language_length,
	                           char** out_serialization_code, uint32_t* out_serialization_code_length,
	                           char** out_err, uint32_t* out_err_len) override;
};
//--------------------------------------------------------------------
