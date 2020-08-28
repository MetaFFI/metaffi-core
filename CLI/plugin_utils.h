#pragma once
#include <string>
#include <vector>

class plugin_utils
{
public:
	plugin_utils() = delete;
	~plugin_utils() = default;
	
	[[nodiscard]] static bool is_installed(const std::string& plugin_name) ;
	[[nodiscard]] static std::vector<std::string> list() ;
	
	static void install(const std::string& url_or_path, bool force);
	static void remove(const std::string& name);

};
