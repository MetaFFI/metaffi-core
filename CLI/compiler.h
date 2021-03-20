#pragma once
#include <string>
#include <vector>
#include <memory>

struct idl_definition;

class compiler
{
private:
	std::string _idl_path;
	std::string _output_path;
	
	std::vector<std::string> _host_languages;
	std::unique_ptr<idl_definition> _idl_def = nullptr;
	
	std::string _idl_source;

public:
	compiler(const std::string& idl_path, const std::string& output_path);
	~compiler() = default;
	compiler(const compiler&) = delete;
	compiler(const compiler&&) = delete;

	void compile_to_guest();
	void compile_from_host(const std::vector<std::string>& langs);

private:
	void compile_from_host(const std::string& lang);
};