#pragma once
#include <memory>
#include <unordered_map>
#include <boost/dll.hpp>

//--------------------------------------------------------------------
class plugin_modules_repository
{
private: // variable
	static std::unique_ptr<plugin_modules_repository> instance;

private: // methods
	std::unordered_map<std::string, std::shared_ptr<boost::dll::shared_library>> modules;
	
public: // static functions
	static plugin_modules_repository& get_instance();
	static void free_instance();
	
public: // methods
	plugin_modules_repository() = default;
	~plugin_modules_repository() = default;
	
	std::shared_ptr<boost::dll::shared_library> operator[](const std::string& plugin_name);
};
//--------------------------------------------------------------------