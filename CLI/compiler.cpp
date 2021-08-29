#include "compiler.h"
#include "language_plugin_interface_wrapper.h"
#include "idl_plugin_interface_wrapper.h"
#include "idl_block.h"
#include <boost/filesystem.hpp>
#include <utils/scope_guard.hpp>
#include <regex>
#include <utility>

using namespace metaffi::utils;

//--------------------------------------------------------------------
compiler::compiler(const std::string& idl_path, std::string  output_path, const std::string& embedded_name_or_pattern, bool is_pattern):
		_input_file_path(idl_path),
		_output_path(std::move(output_path))
{
	if(!boost::filesystem::exists(idl_path))
	{
		std::stringstream ss;
		ss << "Error: " << idl_path << " was not found";
		throw std::runtime_error(ss.str().c_str());
	}
	
	// read IDL file
	std::ifstream ifs(_input_file_path);
	_input_file_code = std::string(std::istreambuf_iterator<char>(ifs),
	                               std::istreambuf_iterator<char>());
	ifs.close();
	
	this->compile_to_idl_blocks(embedded_name_or_pattern, is_pattern);
	
}
//--------------------------------------------------------------------
void compiler::compile_to_guest()
{
	for(idl_block& b : this->_idl_blocks)
	{
		// load compiler
		std::stringstream compiler_plugin_name;
		compiler_plugin_name << "metaffi.compiler.lang." << b.get_target_language();
		
		// load plugin
		std::unique_ptr<language_plugin_interface_wrapper> loaded_plugin = std::make_unique<language_plugin_interface_wrapper>(compiler_plugin_name.str());
		loaded_plugin->init();
		
		char* err = nullptr;
		uint32_t err_len = 0;
		scope_guard sg([&](){
			if(err){
				free(err);
			}
		});
		
		// call compile_to_guest with IDL path and output path
		loaded_plugin->compile_to_guest(b._metaffi_json_idl.c_str(), b._metaffi_json_idl.size(),
		                                this->_output_path.c_str(), this->_output_path.size(),
										b._block_name.c_str(), b._block_name.size(),
										b._file_code.c_str(), b._file_code.size(),
		                                &err, &err_len);
		
		if(err)
		{
			throw std::runtime_error(std::string(err, err_len));
		}
	}
}
//--------------------------------------------------------------------
void compiler::compile_from_host(const std::vector<std::string>& langs, const std::string& host_options)
{
	for(const std::string& lang : langs){
		this->compile_from_host(lang, host_options);
	}
}
//--------------------------------------------------------------------
void compiler::compile_from_host(const std::string& lang, const std::string& host_options)
{
	for(idl_block& b : this->_idl_blocks)
	{
		// load compiler
		std::stringstream compiler_plugin_name;
		compiler_plugin_name << "metaffi.compiler.lang." << lang;
		
		// load plugin
		std::unique_ptr<language_plugin_interface_wrapper> loaded_plugin = std::make_unique<language_plugin_interface_wrapper>(compiler_plugin_name.str());
		loaded_plugin->init();
		
		char* err = nullptr;
		uint32_t err_len = 0;
		scope_guard sg([&]()
		               {
			               if (err)
			               {
				               free(err);
			               }
		               });
		
		// call compile_to_guest with IDL path and output path
		loaded_plugin->compile_from_host(b._metaffi_json_idl.c_str(), b._metaffi_json_idl.size(),
		                                 this->_output_path.c_str(), this->_output_path.size(),
		                                 host_options.c_str(), host_options.length(),
		                                 &err, &err_len);
		
		if (err)
		{
			throw std::runtime_error(std::string(err, err_len));
		}
	}
}
//--------------------------------------------------------------------
void compiler::print_idl()
{
	for(idl_block& b : this->_idl_blocks)
	{
		if(!b._block_name.empty())
		{
			std::cout << b._block_name << std::endl;
		}
		
		std::cout << b._metaffi_json_idl << std::endl;
	}
}
//--------------------------------------------------------------------
void compiler::compile_to_idl_blocks(const std::string& embedded_name_or_pattern, bool is_pattern)
{
	this->extract_idl_blocks(embedded_name_or_pattern, is_pattern);
	
	for(idl_block& block : _idl_blocks)
	{
		block.compile_metaffi_idl();
	}
}
//--------------------------------------------------------------------
void compiler::extract_idl_blocks(const std::string& embedded_name_or_pattern, bool is_pattern)
{
	if(embedded_name_or_pattern.empty()) // fill only input file
	{
		boost::filesystem::path idl_fs_path(_input_file_path);
		
		// extract extension
		std::string extension = idl_fs_path.extension().string();
		std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c){ return std::tolower(c); });
		
		idl_block b(extension, this->_input_file_code, this->_input_file_path);
		this->_idl_blocks.push_back(b);
	}
	else // extract source blocks
	{
		std::string re = "[^\\n]*" MXSTR(METAFFI_BLOCK_START) "[\\w\\W]+" MXSTR(METAFFI_BLOCK_END);
		std::regex blocks_regex(re);
		std::smatch found_blocks;
		if(!std::regex_search(_input_file_code, found_blocks, blocks_regex))
		{
			std::cout << "No blocks were found" << std::endl;
			return;
		}
		
		for(auto& found_block : found_blocks)
		{
			std::string block = found_block.str();
			idl_block b = idl_block::parse_block(block, _input_file_path);
			this->_idl_blocks.push_back(b);
		}
	}
}
//--------------------------------------------------------------------