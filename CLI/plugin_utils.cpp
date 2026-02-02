#include "plugin_utils.h"
#include <algorithm>
#include <boost/dll.hpp>
#include <boost/algorithm/string.hpp>
#include <regex>
#include <sstream>
#include "uri.h"
#include <utils/scope_guard.hpp>
#include <utils/env_utils.h>
#include <filesystem>

using namespace metaffi::utils;

//--------------------------------------------------------------------
bool plugin_utils::is_installed(const std::string& plugin_name)
{
	std::vector<std::string> plugins = list();
	return std::find(plugins.begin(), plugins.end(), plugin_name) != plugins.end();
}
//--------------------------------------------------------------------
std::vector<std::string> plugin_utils::list()
{
	std::vector<std::string> res;
	
	// get all plugin files and extract the names
	std::filesystem::directory_iterator end_di;
	
	for(std::filesystem::directory_iterator di(get_install_path()) ; di != end_di ; di++)
	{
		if(di->is_directory())
		{
			if(di->path().filename().string() == "include"){
				continue;
			}

			res.push_back(di->path().filename().string());
		}
	}
	
	return res;
}
//--------------------------------------------------------------------
void plugin_utils::install(const std::string& url_or_path)
{
	// download (if url)
	std::string lowered_url_or_path = url_or_path;
	std::transform(lowered_url_or_path.begin(), lowered_url_or_path.end(), lowered_url_or_path.begin(), [](unsigned char c){ return std::tolower(c); });
	
	std::filesystem::path plugin_installer_python_script;
	bool is_delete_file = false;
	scope_guard sg([&]()
	{
		if(is_delete_file && std::filesystem::exists(plugin_installer_python_script)){
			std::filesystem::remove(plugin_installer_python_script);
		}
	});
	
	if(lowered_url_or_path.rfind("http", 0) == 0) // if starts with "http" - download
	{
		is_delete_file = true;
		plugin_installer_python_script = download(url_or_path);
	}
	else
	{
		plugin_installer_python_script = url_or_path;
		if(!std::filesystem::exists(plugin_installer_python_script)){
			throw std::runtime_error("Plugin path doesn't exist");
		}
	}
	
	// run installer using python
	std::stringstream ss_install_cmds;
#ifdef _WIN32
	ss_install_cmds << "python " << plugin_installer_python_script.generic_string() << " install";
#else
	ss_install_cmds << "python3 " << plugin_installer_python_script.generic_string() << " install";
#endif

	std::cout << "Installing plugin" << std::endl;
	int exit_code = system(ss_install_cmds.str().c_str());

	if(exit_code != 0) // failure
	{
		throw std::runtime_error("Failed to install plugin");
	}
	
	std::cout << "Installation completed" << std::endl;
}
//--------------------------------------------------------------------
void plugin_utils::remove(const std::string& name)
{
	if(!is_installed(name)){
		throw std::runtime_error("Plugin not installed");
	}
	
	// get uninstaller python script in the plugin directory
	std::stringstream ss_plugin_path;
	ss_plugin_path << get_install_path() << "/" << name << "/uninstall.py";
	std::filesystem::path uninstaller_python_script = ss_plugin_path.str();
	if(!std::filesystem::exists(uninstaller_python_script)){
		throw std::runtime_error("Uninstaller script not found");
	}

	// run uninstaller using python
	std::stringstream ss_uninstall_cmds;
#ifdef _WIN32
	ss_uninstall_cmds << "python " << uninstaller_python_script.generic_string();
#else
	ss_uninstall_cmds << "python3 " << uninstaller_python_script.generic_string();
#endif

	std::cout << "Uninstalling plugin" << std::endl;
	int exit_code = system(ss_uninstall_cmds.str().c_str());

	if(exit_code != 0) // failure
	{
		throw std::runtime_error("Failed to uninstall plugin");
	}
	
	std::cout << "Uninstallation completed" << std::endl;
}
//--------------------------------------------------------------------
std::string plugin_utils::get_install_path()
{
	std::string metaffi_home = get_env_var("METAFFI_HOME");
	if(metaffi_home.empty())
	{
		throw std::runtime_error("METAFFI_HOME is not set");
	}
	
	return metaffi_home;
}
//--------------------------------------------------------------------
std::filesystem::path plugin_utils::download(const std::string& url)
{
	uri link(url);
	std::filesystem::path compressed_plugin_path = link.get_path().substr( link.get_path().rfind('/')+1 );
	
	std::stringstream ss_download_cmds;
	
#ifdef _WIN32
	ss_download_cmds << R"(powershell -ExecutionPolicy ByPass -command "wget )" << url << R"( -OutFile ')" << compressed_plugin_path.generic_string() << "'";
#else
	ss_download_cmds << "wget -q " << url;
#endif
	std::cout << "Downloading " << url << std::endl;
	int exit_code = system(ss_download_cmds.str().c_str());
	if(exit_code != 0) // failure
	{
		throw std::runtime_error("Failed to download plugin package");
	}
	
	return compressed_plugin_path;
}
//--------------------------------------------------------------------
