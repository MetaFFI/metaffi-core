#pragma once
#include <memory>
#include <unordered_map>
#include <boost/dll.hpp>

//--------------------------------------------------------------------
class modules_repository
{
private: // variable
	static std::unique_ptr<modules_repository> instance;

private: // methods
	std::unordered_map<std::string, std::shared_ptr<boost::dll::shared_library>> modules;
	
public: // static functions
	static modules_repository& get_instance();
	static void free_instance();
	
public: // methods
	modules_repository() = default;
	~modules_repository() = default;
	
	std::shared_ptr<boost::dll::shared_library> operator[](const std::string& module_name);
};
//--------------------------------------------------------------------