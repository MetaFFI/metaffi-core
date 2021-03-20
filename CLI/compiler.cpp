#include "compiler.h"
#include "compiler_plugin_interface_wrapper.h"
#include "idl_plugin_interface_wrapper.h"
#include "serializer_plugin_interface_wrapper.h"
#include <boost/filesystem.hpp>
#include <utils/scope_guard.hpp>
#include <sstream>

using namespace openffi::utils;

//--------------------------------------------------------------------
compiler::compiler(const std::string& idl_path, const std::string& output_path):
	_idl_path(idl_path),
	_output_path(output_path)
{
	if(!boost::filesystem::exists(idl_path))
	{
		std::stringstream ss;
		ss << "Error: " << idl_path << " was not found";
		throw std::runtime_error(ss.str().c_str());
	}
	
	std::ifstream ifs(_idl_path);
	_idl_source = std::string(std::istreambuf_iterator<char>(ifs),
	                 std::istreambuf_iterator<char>());
	ifs.close();
	
	// get idl data
	boost::filesystem::path idl_fs_path(_idl_path);
	std::unique_ptr<idl_plugin_interface_wrapper> idl = std::make_unique<idl_plugin_interface_wrapper>(
				std::string("openffi.idl.")+idl_fs_path.extension().string());
	
	char* err = nullptr;
	uint32_t err_len = 0;
	_idl_def = std::unique_ptr<idl_definition>(idl->parse_idl(idl_fs_path.filename().c_str(), idl_fs_path.filename().string().length(),
	               _idl_source.c_str(), _idl_source.length(),
	               &err, &err_len));
	
	if(err){
		throw std::runtime_error(std::string(err, err_len));
	}
}
//--------------------------------------------------------------------
void compiler::compile_to_guest()
{
	// generate serialization code
	boost::filesystem::path idl_fs_path(_idl_path);
	std::stringstream serializer_plugin_name;
	serializer_plugin_name << "openffi.serializer." << idl_fs_path.extension().string() << "." << std::string(_idl_def->target_language, _idl_def->target_language_length);
	
	std::unique_ptr<serializer_plugin_interface_wrapper> serializer = std::make_unique<serializer_plugin_interface_wrapper>(serializer_plugin_name.str());
	
	char* err = nullptr;
	uint32_t err_len = 0;
	scope_guard sg([&](){
		if(err){
			free(err);
		}
	});
	
	char* out_serialization_code = nullptr;
	uint32_t out_serialization_code_length = 0;
	serializer->compile_serialization(idl_fs_path.filename().c_str(), idl_fs_path.filename().size(),
	                                  _idl_source.c_str(), _idl_source.length(),
	                                  &out_serialization_code, &out_serialization_code_length,
	                                  &err, &err_len);
	
	if(err)
	{
		throw std::runtime_error(std::string(err, err_len));
	}
	
	std::string serialization_code(out_serialization_code, out_serialization_code_length);
	
	// load compiler
	std::stringstream compiler_plugin_name;
	compiler_plugin_name << "openffi.compiler." << std::string(_idl_def->target_language, _idl_def->target_language_length);

	// load plugin
	std::unique_ptr<compiler_plugin_interface_wrapper> loaded_plugin = std::make_unique<compiler_plugin_interface_wrapper>(compiler_plugin_name.str());
	
	err = nullptr;
	err_len = 0;
	
	
	// call compile_to_guest with IDL path and output path
	loaded_plugin->compile_to_guest(this->_idl_def.get(),
									this->_output_path.c_str(), this->_output_path.size(),
									serialization_code.c_str(), serialization_code.length(),
									&err, &err_len);

	if(err)
	{
		throw std::runtime_error(std::string(err, err_len));
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
	// generate serialization code
	boost::filesystem::path idl_fs_path(_idl_path);
	std::stringstream serializer_plugin_name;
	serializer_plugin_name << "openffi.serializer." << idl_fs_path.extension().string() << "." << lang;
	
	std::unique_ptr<serializer_plugin_interface_wrapper> serializer = std::make_unique<serializer_plugin_interface_wrapper>(serializer_plugin_name.str());
	
	char* err = nullptr;
	uint32_t err_len = 0;
	scope_guard sg([&](){
		if(err){
			free(err);
		}
	});
	
	char* out_serialization_code = nullptr;
	uint32_t out_serialization_code_length = 0;
	serializer->compile_serialization(idl_fs_path.filename().c_str(), idl_fs_path.filename().size(),
	                                  _idl_source.c_str(), _idl_source.length(),
	                                  &out_serialization_code, &out_serialization_code_length,
	                                  &err, &err_len);
	
	if(err)
	{
		throw std::runtime_error(std::string(err, err_len));
	}
	
	std::string serialization_code(out_serialization_code, out_serialization_code_length);
	
	std::string plugin_filename("openffi.compiler.");
	plugin_filename += lang;

	// load plugin
	std::stringstream compiler_plugin_name;
	compiler_plugin_name << "openffi.compiler." << std::string(_idl_def->target_language, _idl_def->target_language_length);
	
	// load plugin
	std::unique_ptr<compiler_plugin_interface_wrapper> loaded_plugin = std::make_unique<compiler_plugin_interface_wrapper>(compiler_plugin_name.str());
	
	err = nullptr;
	err_len = 0;
	
	// call compile_to_guest with IDL path and output path
	loaded_plugin->compile_from_host(this->_idl_def.get(),
									this->_output_path.c_str(), this->_output_path.size(),
									serialization_code.c_str(), serialization_code.length(),
									&err, &err_len);

	if(err)
	{
		throw std::runtime_error(std::string(err, err_len));
	}
}
//--------------------------------------------------------------------