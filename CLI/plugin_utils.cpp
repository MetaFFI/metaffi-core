#include "plugin_utils.h"
#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/dll.hpp>
#include <regex>
#include <sstream>
#include "httplib.h"
#include "uri.h"
#include "../utils/scope_guard.hpp"

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
	boost::filesystem::directory_iterator end_di;
	
	std::stringstream pattern;
	pattern << R"(openffi\.compiler\.([a-zA-Z0-9_]+)\)" << boost::dll::shared_library::suffix().generic_string();
	std::regex plugin_name_pattern(pattern.str());
	
	for(boost::filesystem::directory_iterator di(get_install_path()) ; di != end_di ; di++)
	{
		if( !boost::filesystem::is_regular_file(di->status())){
			continue;
		}
		
		std::string path = di->path().generic_string();
		std::sregex_iterator matches(path.begin(), path.end(), plugin_name_pattern); // match and extract plugin name
		if(matches->begin() != matches->end() && matches->size() > 1) // make sure subgroup got matched
		{
			res.push_back( (*matches)[1].str() );
		}
	}
	
	return res;
}
//--------------------------------------------------------------------
void plugin_utils::install(const std::string& url_or_path, bool force)
{
	// download (if url)
	std::string lowered_url_or_path = url_or_path;
	std::transform(lowered_url_or_path.begin(), lowered_url_or_path.end(), lowered_url_or_path.begin(), [](unsigned char c){ return std::tolower(c); });
	
	boost::filesystem::path compressed_plugin_path;
	bool is_delete_file = false;
	scope_guard sg([&]()
	{
		if(is_delete_file && boost::filesystem::exists(compressed_plugin_path)){
			boost::filesystem::remove(compressed_plugin_path);
		}
	});
	
	if(lowered_url_or_path.rfind("http", 0) == 0) // if starts with "http" - download
	{
		uri link(url_or_path);
		compressed_plugin_path = link.get_path().substr( link.get_path().rfind('/')+1 );
		
		is_delete_file = true;
		
		std::stringstream ss_download_cmds;

#ifdef _WIN32
		ss_download_cmds << R"(powershell -ExecutionPolicy ByPass -command "Expand-Archive -Path )" << compressed_plugin_path << R"( -DestinationPath ")" << get_install_path() << "\"";
		if(force)
		{
			ss_download_cmds << " -Force";
		}
#else
		ss_download_cmds << "wget -q " << url_or_path;
#endif
		std::cout << "Downloading " << url_or_path << std::endl;
		int exit_code = system(ss_download_cmds.str().c_str());
		if(exit_code != 0) // failure
		{
			throw std::runtime_error("Failed to download plugin package");
		}
		
		/* // TODO: Replace "wget" cmd. The code below crashes!
		
		std::stringstream ss;
		ss << link.get_scheme() << "://" << link.get_host();
		if(link.get_port() != 0){ ss << ":" << link.get_port(); }
		
		std::cout << "connecting to " << ss.str() << std::endl;
		httplib::Client cli(ss.str().c_str());
		
		ss = std::stringstream();
		ss << "/" << link.get_path() << link.get_query();
		std::cout << "downloading path " << ss.str() << std::endl;
		httplib::Result res = cli.Get(ss.str().c_str(),
								
								{ { "Accept-Encoding", "text/plain" } },
								
								      [](uint64_t len, uint64_t total) { // print progress
									      printf("Downloading %lu bytes: %d%% complete\n",
									             total,
									             (int)(len*100/total));
									      return true; // return 'false' if you want to cancel the request.
								      }
								);
		
		
		
		
		std::cout << "writing file to " << compressed_plugin_path << std::endl;
		
		std::ofstream out(compressed_plugin_path);
		out << res->body;
		out.close();
		*/
	}
	else
	{
		compressed_plugin_path = url_or_path;
		if(!boost::filesystem::exists(compressed_plugin_path)){
			throw std::runtime_error("Plugin path doesn't exist");
		}
	}
	
	// unzip/untar - TODO: Must be replaced with actual code. I'm just getting lazy here.
	std::stringstream decompress_cmd;
#ifdef _WIN32
	decompress_cmd << R"(powershell -ExecutionPolicy ByPass -command "Expand-Archive -Path )" << compressed_plugin_path << R"( -DestinationPath ")" << get_install_path() << "\"";
	if(force)
	{
		decompress_cmd << " -Force";
	}
#else
	decompress_cmd << "tar -C " << get_install_path() << " -zx" << (force? "f " : " ") << compressed_plugin_path;
#endif
	
	std::cout << "Decompressing: " << compressed_plugin_path << std::endl;
	
	int exit_code = system(decompress_cmd.str().c_str());
	if(exit_code != 0) // failure
	{
		throw std::runtime_error("Failed to decompress plugin package");
	}
	
	std::cout << "Done installing package" << compressed_plugin_path << std::endl;
}
//--------------------------------------------------------------------
void plugin_utils::remove(const std::string& name)
{
	if(!is_installed(name)){
		throw std::runtime_error("Plugin not installed");
	}
	
	std::stringstream compiler_path;
	compiler_path << get_install_path() << boost::filesystem::path::preferred_separator << "openffi.compiler." << name << boost::dll::shared_library::suffix().generic_string();
	
	std::stringstream xllr_path;
	xllr_path << get_install_path() << boost::filesystem::path::preferred_separator << "xllr." << name << boost::dll::shared_library::suffix().generic_string();
	
	if(boost::filesystem::exists(compiler_path.str()))
	{
		std::cout << "Deleting: " << compiler_path.str() << std::endl;
		boost::filesystem::remove(compiler_path.str());
	}
	
	if(boost::filesystem::exists(xllr_path.str()))
	{
		std::cout << "Deleting: " << xllr_path.str() << std::endl;
		boost::filesystem::remove(xllr_path.str());
	}
	
	std::cout << "Done removing " << name << std::endl;
}
//--------------------------------------------------------------------
std::string plugin_utils::get_install_path()
{
	boost::filesystem::path location = boost::dll::program_location();
	if(boost::filesystem::is_symlink(location))
	{
		location = boost::filesystem::read_symlink(location);
	}
	
	return location.parent_path().generic_string();
}
//--------------------------------------------------------------------