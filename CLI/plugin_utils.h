#pragma once
#include <string>
#include <vector>
#include <boost/filesystem.hpp>

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
	
	static void pack(const std::vector<std::string>& files_and_dirs, const std::string& root);

private:
	static std::string get_install_path();
	static boost::filesystem::path download(const std::string& url);
	static boost::filesystem::path decompress(const boost::filesystem::path& compressed_file);
	
	static void validate_plugin(const boost::filesystem::path& decompressed_plugin_path);
	static void copy_plugin_package(const boost::filesystem::path& decompressed_plugin_path);
	static bool extract_plugin_name_and_type(const boost::filesystem::path& path, std::string& out_name, plugin_type& out_type);
};
