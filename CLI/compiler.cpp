#include "compiler.h"
#include "compiler_plugin_interface_wrapper.h"
#include <boost/filesystem.hpp>
#include <utils/scope_guard.hpp>
#include <regex>
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
}
//--------------------------------------------------------------------
std::string compiler::get_target_lang() const
{
	// read idl file
	boost::filesystem::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	file.open(this->_idl_path, std::ios_base::binary);
	size_t sz = static_cast<std::size_t>(boost::filesystem::file_size(this->_idl_path));
	std::string proto;
	proto.resize(sz);
	file.read(&proto[0], sz);
	file.close();
	
	// get target language
	// // openffi_target_lang: "go"
	std::regex target_lang_regex("openffi_target_lang:[ \t]*\"([a-zA-Z0-9_]+)\"");
	std::sregex_iterator matches(proto.begin(), proto.end(), target_lang_regex);
	if(matches->begin() != matches->end()){
		if(matches->size() > 1) // make sure subgroup got matched
		{
			return (*matches)[1].str();
		}
	}
	
	// target lang not found!
	throw std::runtime_error("openffi_target_lang was not found in IDL");
}
//--------------------------------------------------------------------
void compiler::compile_to_guest(bool compile_serialization_code)
{
	std::string lang(this->get_target_lang());
	
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

	if(compile_serialization_code)
	{
		loaded_plugin->compile_serialization(this->_idl_path.c_str(), this->_idl_path.size(),
		                                     this->_output_path.c_str(), this->_output_path.size(),
		                                     &err, &err_len);
		
		if(err)
		{
			std::string errmsg(err, err_len);
			throw std::runtime_error(errmsg.c_str());
		}
	}
	
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
void compiler::compile_from_host(const std::vector<std::string>& langs, bool compile_serialization_code)
{
	for(const std::string& lang : langs){
		this->compile_from_host(lang, compile_serialization_code);
	}
}
//--------------------------------------------------------------------
void compiler::compile_from_host(const std::string& lang, bool compile_serialization_code)
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
	
	if(compile_serialization_code)
	{
		loaded_plugin->compile_serialization(this->_idl_path.c_str(), this->_idl_path.size(),
		                                     this->_output_path.c_str(), this->_output_path.size(),
		                                     &err, &err_len);
		
		if(err)
		{
			std::string errmsg(err, err_len);
			throw std::runtime_error(errmsg.c_str());
		}
	}
	
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