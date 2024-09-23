#pragma once
#include <string>
#include <vector>
#include <filesystem>

enum plugin_type
{
	compiler_plugin,
	runtime_plugin,
	idl_plugin
};

class plugin_utils
{
public:
	plugin_utils() = delete;
	~plugin_utils() = default;
	
	[[nodiscard]] static bool is_installed(const std::string& plugin_name) ;
	[[nodiscard]] static std::vector<std::string> list() ;
	
	static void install(const std::string& url_or_path);
	static void remove(const std::string& name);

private:
	static std::string get_install_path();
	static std::filesystem::path download(const std::string& url);
	
	static void validate_plugin(const std::filesystem::path& decompressed_plugin_path);
	static void copy_plugin_package(const std::filesystem::path& decompressed_plugin_path);
	static bool extract_plugin_name_and_type(const std::filesystem::path& path, std::string& out_name, plugin_type& out_type);
};
