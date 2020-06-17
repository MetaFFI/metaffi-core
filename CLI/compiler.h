#pragma once
#include <string>
#include <vector>

class compiler
{
private:
	std::string _idl_path;
	std::string _output_path;
	
	std::string _guest_language;
	std::vector<std::string> _host_languages;


public:
	compiler(const std::string& idl_path, const std::string& output_path);
	~compiler() = default;
	compiler(const compiler&) = delete;
	compiler(const compiler&&) = delete;

	void compile_to_guest(const std::string& lang);
	void compile_from_host(const std::vector<std::string>& langs);

private:
	void compile_from_host(const std::string& lang);
};