#pragma once
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include "idl_block.h"
#include "language_plugin_interface_wrapper.h"


class compiler
{
private:
	std::string _input_file_path;
	std::string _output_path;
	
	std::vector<std::string> _host_languages;
	std::string _input_file_code;
	std::vector<idl_block> _idl_blocks;

public:
	compiler(const std::string& idl_path, const std::string& output_path, const std::string& embedded_name_or_pattern, bool is_pattern);
	~compiler() = default;
	compiler(const compiler&) = delete;
	compiler(const compiler&&) = delete;

	void compile_to_guest();
	void compile_from_host(const std::vector<std::string>& langs, const std::string& host_options);
	void print_idl();

private:
	void compile_from_host(const std::string& lang, const std::string& host_options);
	void compile_to_idl_blocks(const std::string& embedded_name_or_pattern, bool is_pattern);
	void extract_idl_blocks(const std::string& embedded_name_or_pattern, bool is_pattern);
	std::shared_ptr<language_plugin_interface_wrapper> load_plugin(const std::string& lang);
};