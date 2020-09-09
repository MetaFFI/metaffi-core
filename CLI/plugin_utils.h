#pragma once
#include <string>
#include <vector>
#include <boost/filesystem.hpp>

class plugin_utils
{
public:
	plugin_utils() = delete;
	~plugin_utils() = default;
	
	[[nodiscard]] static bool is_installed(const std::string& plugin_name) ;
	[[nodiscard]] static std::vector<std::string> list() ;
	
	static void install(const std::string& url_or_path, bool force);
	static void remove(const std::string& name);

private:
	static std::string get_install_path();
	static boost::filesystem::path download(const std::string& url);
	static std::vector<std::string> decompress(const boost::filesystem::path& compressed_file, bool force);
};
