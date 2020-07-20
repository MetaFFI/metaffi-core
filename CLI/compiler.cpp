#include "compiler.h"
#include "compiler_plugin_interface_wrapper.h"
#include <boost/filesystem.hpp>
#include "../utils/scope_guard.hpp"


//--------------------------------------------------------------------
compiler::compiler(const std::string& idl_path, const std::string& output_path):
	_idl_path(idl_path),
	_output_path(output_path)
{
}
//--------------------------------------------------------------------
void compiler::compile_to_guest(const std::string& lang)
{
	std::string plugin_filename("openffi.compiler.");
	plugin_filename += lang;

	// load plugin
	std::unique_ptr<compiler_plugin_interface_wrapper> loaded_plugin = std::make_unique<compiler_plugin_interface_wrapper>(plugin_filename);
	
	char* err = nullptr;
	uint32_t err_len = 0;

	scope_guard sg([&](){
		if(err){
			free(err);
		}
	});

	// call compile_to_guest with IDL path and output path
	loaded_plugin->compile_to_guest(this->_idl_path.c_str(), this->_idl_path.size(), 
									this->_output_path.c_str(), this->_output_path.size(),
									&err, &err_len);

	if(err)
	{
		std::string errmsg(err, err_len);
		throw std::runtime_error(errmsg.c_str());
	}

}
//--------------------------------------------------------------------
void compiler::compile_from_host(const std::vector<std::string>& langs)
{
	for(const std::string& lang : langs){
		this->compile_from_host(lang);
	}
}
//--------------------------------------------------------------------
void compiler::compile_from_host(const std::string& lang)
{
    std::string plugin_filename("openffi.compiler.");
	plugin_filename += lang;

	// load plugin
	std::unique_ptr<compiler_plugin_interface_wrapper> loaded_plugin = std::make_unique<compiler_plugin_interface_wrapper>(plugin_filename);

	char* err = nullptr;
	uint32_t err_len = 0;

	scope_guard sg([&](){
		if(err){
			free(err);
		}
	});

	// call compile_to_guest with IDL path and output path
	loaded_plugin->compile_from_host(this->_idl_path.c_str(), this->_idl_path.size(), 
									this->_output_path.c_str(), this->_output_path.size(),
									&err,
									&err_len);

	if(err)
	{
		std::string errmsg(err, err_len);
		throw std::runtime_error(errmsg.c_str());
	}
}
//--------------------------------------------------------------------