#include "compiler.h"
#include "language_plugin_interface_wrapper.h"
#include "idl_extrator.h"
#include <filesystem>
#include <regex>
#include <sstream>
#include <utils/expand_env.h>

using namespace metaffi::utils;

//--------------------------------------------------------------------
compiler::compiler(const std::string& idl_json, const std::string& output_path)
{
	_output_path = expand_env(output_path);
	_idl_json = idl_json;
	
	if(!std::filesystem::exists(_output_path))
	{
		std::stringstream ss;
		ss << "IDL/Source file \"" << _output_path << "\" was not found";
		throw std::runtime_error(ss.str().c_str());
	}
}
//--------------------------------------------------------------------
void compiler::compile_guest(const std::string& guest_options)
{
	// load compiler
	std::shared_ptr<language_plugin_interface_wrapper> loaded_plugin = language_plugin_interface_wrapper::load(this->get_guest_language() );
	
	char* err = nullptr;
	uint32_t err_len = 0;
	
	// call compile_to_guest with IDL path and output path
	loaded_plugin->compile_to_guest(_idl_json.c_str(), _idl_json.size(),
	                                _output_path.c_str(), _output_path.size(),
	                                guest_options.c_str(), guest_options.size(),
	                                &err, &err_len);
	
	if(err)
	{
		throw std::runtime_error(std::string(err, err_len));
	}
}
//--------------------------------------------------------------------
void compiler::compile_host(const std::vector<std::string>& langs, const std::string& host_options)
{
	for(const std::string& lang : langs){
		this->compile_host(lang, host_options);
	}
}
//--------------------------------------------------------------------
void compiler::compile_host(const std::string& lang, const std::string& host_options)
{
	// load compiler
	std::shared_ptr<language_plugin_interface_wrapper> loaded_plugin = language_plugin_interface_wrapper::load(lang);
	
	// load plugin
	char* err = nullptr;
	uint32_t err_len = 0;
	
	// call compile_to_guest with IDL path and output path
	loaded_plugin->compile_from_host(_idl_json.c_str(), _idl_json.size(),
	                                 _output_path.c_str(), _output_path.size(),
	                                 host_options.c_str(), host_options.length(),
	                                 &err, &err_len);
	
	if (err_len > 0)
	{
		throw std::runtime_error(std::string(err, err_len));
	}
}
//--------------------------------------------------------------------
std::string compiler::get_guest_language() const
{
	// get target language
	std::regex target_lang_regex(R"(target_language"[ ]*:[ ]*"([^\"]+))");
	std::smatch matches;
	if(!std::regex_search(_idl_json, matches, target_lang_regex))
	{
		printf("+++ IDL JSON: %s\n", _idl_json.c_str());
		throw std::runtime_error("MetaFFI IDL does not contains \"target_language\" tag");
	}
	
	if(matches.size() < 2)
	{
		throw std::runtime_error("target_language tag does not contain target language");
	}
	
	return matches[1].str();
}
//--------------------------------------------------------------------