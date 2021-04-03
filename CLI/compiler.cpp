#include "compiler.h"
#include "language_plugin_interface_wrapper.h"
#include "idl_plugin_interface_wrapper.h"
#include "serializer_plugin_interface_wrapper.h"
#include <boost/filesystem.hpp>
#include <utils/scope_guard.hpp>
#include <regex>

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
	std::unique_ptr<idl_plugin_interface_wrapper> idl = std::make_unique<idl_plugin_interface_wrapper>(idl_fs_path.extension().string());
	idl->init();
	
	char* err = nullptr;
	uint32_t err_len = 0;
	char* out_idl_def_json;
	uint32_t out_idl_def_json_length;
	idl->parse_idl(idl_fs_path.filename().c_str(), idl_fs_path.filename().string().length(),
	               _idl_source.c_str(), _idl_source.length(),
	               &out_idl_def_json, &out_idl_def_json_length,
	               &err, &err_len);
	if(err){
		throw std::runtime_error(std::string(err, err_len));
	}
	
	_openffi_idl = std::string(out_idl_def_json, out_idl_def_json_length);
	free(out_idl_def_json);
}
//--------------------------------------------------------------------
std::string compiler::get_target_language()
{
	// get target language
	std::regex target_lang_regex(R"(\"openffi_target_language\":\"([^\"]+)\")");
	std::smatch matches;
	if(!std::regex_search(_openffi_idl, matches, target_lang_regex))
	{
		throw std::runtime_error("OpenFFI IDL does not contains openffi_target_language tag");
	}
	
	if(matches.size() < 2)
	{
		throw std::runtime_error("openffi_target_language tag does not contain target language");
	}
	
	return matches[1].str();
}
//--------------------------------------------------------------------
void compiler::compile_to_guest()
{
	// generate serialization code
	boost::filesystem::path idl_fs_path(_idl_path);
	std::stringstream serializer_plugin_name;
	
	std::string target_language = this->get_target_language();
	
	std::unique_ptr<serializer_plugin_interface_wrapper> serializer = std::make_unique<serializer_plugin_interface_wrapper>(idl_fs_path.extension().string());
	serializer->init();
	
	char* err = nullptr;
	uint32_t err_len = 0;
	scope_guard sg([&](){
		if(err){
			free(err);
		}
	});
	
	char* out_serialization_code = nullptr;
	uint32_t out_serialization_code_length = 0;
	serializer->compile_serialization(_openffi_idl.c_str(), _openffi_idl.length(),
	                                  target_language.c_str(), target_language.length(),
	                                  &out_serialization_code, &out_serialization_code_length,
	                                  &err, &err_len);
	if(err)
	{
		throw std::runtime_error(std::string(err, err_len));
	}
	
	std::string serialization_code(out_serialization_code, out_serialization_code_length);
	free(out_serialization_code);
	
	
	
	// load compiler
	std::stringstream compiler_plugin_name;
	compiler_plugin_name << "openffi.compiler.lang." << target_language;

	// load plugin
	std::unique_ptr<language_plugin_interface_wrapper> loaded_plugin = std::make_unique<language_plugin_interface_wrapper>(compiler_plugin_name.str());
	loaded_plugin->init();
	
	err = nullptr;
	err_len = 0;
	
	
	// call compile_to_guest with IDL path and output path
	loaded_plugin->compile_to_guest(this->_openffi_idl.c_str(), this->_openffi_idl.size(),
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
	std::unique_ptr<serializer_plugin_interface_wrapper> serializer = std::make_unique<serializer_plugin_interface_wrapper>(idl_fs_path.string());
	
	char* err = nullptr;
	uint32_t err_len = 0;
	scope_guard sg([&](){
		if(err){
			free(err);
		}
	});
	
	char* out_serialization_code = nullptr;
	uint32_t out_serialization_code_length = 0;
	serializer->compile_serialization(_openffi_idl.c_str(), _openffi_idl.length(),
	                                  lang.c_str(), lang.length(),
	                                  &out_serialization_code, &out_serialization_code_length,
	                                  &err, &err_len);
	
	if(err)
	{
		throw std::runtime_error(std::string(err, err_len));
	}
	
	std::string serialization_code(out_serialization_code, out_serialization_code_length);
	
	std::string plugin_filename("openffi.compiler.");
	plugin_filename += lang;
	
	// load compiler
	std::stringstream compiler_plugin_name;
	compiler_plugin_name << "openffi.compiler.lang." << this->get_target_language();
	
	// load plugin
	std::unique_ptr<language_plugin_interface_wrapper> loaded_plugin = std::make_unique<language_plugin_interface_wrapper>(compiler_plugin_name.str());
	loaded_plugin->init();
	
	err = nullptr;
	err_len = 0;
	
	// call compile_to_guest with IDL path and output path
	loaded_plugin->compile_from_host(this->_openffi_idl.c_str(), this->_openffi_idl.size(),
	                                 this->_output_path.c_str(), this->_output_path.size(),
	                                 serialization_code.c_str(), serialization_code.length(),
	                                 &err, &err_len);

	if(err)
	{
		throw std::runtime_error(std::string(err, err_len));
	}
}
//--------------------------------------------------------------------