#pragma once
#include <string>
#include <vector>
#include <memory>

class compiler
{
private:
	std::string _idl_path;
	std::string _output_path;
	
	std::vector<std::string> _host_languages;
	std::string _idl_source;
	std::string _openffi_idl;

public:
	compiler(const std::string& idl_path, const std::string& output_path);
	~compiler() = default;
	compiler(const compiler&) = delete;
	compiler(const compiler&&) = delete;

	void compile_to_guest();
	void compile_from_host(const std::vector<std::string>& langs);

private:
	void compile_from_host(const std::string& lang);
	std::string get_target_language();
};