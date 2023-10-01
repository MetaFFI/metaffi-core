#pragma once
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include "idl_extrator.h"
#include "language_plugin_interface_wrapper.h"


class compiler
{
private:
	std::string _output_path;
	std::string _idl_json;
	std::vector<std::string> _host_languages;

public:
	compiler(const std::string& idl_json, const std::string& output_path);
	~compiler() = default;
	compiler(const compiler&) = delete;
	compiler(const compiler&&) = delete;

	void compile_guest(const std::string& guest_options);
	void compile_host(const std::vector<std::string>& langs, const std::string& host_options);

private:
	void compile_host(const std::string& lang, const std::string& host_options);
	[[nodiscard]] std::string get_guest_language() const;
};