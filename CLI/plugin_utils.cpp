#include "plugin_utils.h"
#include <algorithm>
#include <boost/filesystem.hpp>
#include <filesystem>
#include <boost/dll.hpp>
#include <boost/algorithm/string.hpp>
#include <regex>
#include <sstream>
#include "uri.h"
#include <utils/scope_guard.hpp>
#include <filesystem>
#include "zip_file.hpp"

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
	boost::filesystem::directory_iterator end_di;
	
	for(boost::filesystem::directory_iterator di(get_install_path()) ; di != end_di ; di++)
	{
		std::string name;
		plugin_type t;
		if(extract_plugin_name_and_type(di->path(), name, t))
		{
			if(t == plugin_type::runtime_plugin)
			{
				res.push_back( name+" runtime" );
			}
			else if(t == plugin_type::compiler_plugin)
			{
				res.push_back( name+" compiler" );
			}
			else
			{
				res.push_back( name+" IDL" );
			}
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
		is_delete_file = true;
		compressed_plugin_path = download(url_or_path);
	}
	else
	{
		compressed_plugin_path = url_or_path;
		if(!boost::filesystem::exists(compressed_plugin_path)){
			throw std::runtime_error("Plugin path doesn't exist");
		}
	}
	
	auto decompressed_plugin_path = decompress(compressed_plugin_path);
	validate_plugin(decompressed_plugin_path);
	
	boost::filesystem::path install_message_file = decompressed_plugin_path;
	install_message_file.append("notes.txt");
	std::string msg;
	if(boost::filesystem::exists(install_message_file))
	{
		std::ifstream file;
		file.open(install_message_file.generic_string(), std::ios_base::binary);
		const boost::uintmax_t sz = boost::filesystem::file_size(install_message_file);
		msg.resize(static_cast< std::size_t >(sz), '\0');
		if (sz > 0u)
		{
			file.read(&msg[0], static_cast< std::streamsize >(sz));
		}
	}
	
	copy_plugin_package(decompressed_plugin_path);
	
	// print install message, if one exists
	if(!msg.empty()){
		std::cout << "Installation Notes:" << std::endl << msg << std::endl;
	}
	
	std::cout << "Installation completed" << std::endl;
}
//--------------------------------------------------------------------
void plugin_utils::remove(const std::string& name)
{
	if(!is_installed(name)){
		throw std::runtime_error("Plugin not installed");
	}
	
	std::stringstream compiler_path;
	compiler_path << get_install_path() << "/metaffi.compiler." << name << boost::dll::shared_library::suffix().generic_string();
	
	std::stringstream xllr_path;
	xllr_path << get_install_path() << "/xllr." << name << boost::dll::shared_library::suffix().generic_string();
	
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

#ifndef _WIN32
	std::cout << "unlinking..." << std::endl;
	std::string install_path = get_install_path();
	
	std::vector<std::string> to_remove_files
	{
			{ (boost::format("xllr.%1%%2%") % name % boost::dll::shared_library::suffix().generic_string()).str() },
			{ (boost::format("metaffi.compiler.%1%%2%") % name % boost::dll::shared_library::suffix().generic_string()).str() }
	};
	
	#ifdef __linux__
		boost::filesystem::remove((boost::format("/user/lib/%1%") % to_remove_files[0]).str() );
		boost::filesystem::remove((boost::format("/user/lib/%1%") % to_remove_files[1]).str() );
	
	#elif __apple__
		boost::filesystem::remove((boost::format("/user/local/lib/%1%") % to_remove_files[0]).str() );
		boost::filesystem::remove((boost::format("/user/local/lib/%1%") % to_remove_files[1]).str() );
		
	#endif
#endif
	
	std::cout << "Done removing" << name << std::endl;
}
//--------------------------------------------------------------------
std::string plugin_utils::get_install_path()
{
	const char* pmetaffi_home = std::getenv("METAFFI_HOME");
	if(!pmetaffi_home)
	{
		throw std::runtime_error("METAFFI_HOME is not set");
	}
	
	return pmetaffi_home;
}
//--------------------------------------------------------------------
boost::filesystem::path plugin_utils::download(const std::string& url)
{
	uri link(url);
	boost::filesystem::path compressed_plugin_path = link.get_path().substr( link.get_path().rfind('/')+1 );
	
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
boost::filesystem::path plugin_utils::decompress(const boost::filesystem::path& compressed_file)
{
	// unzip/untar to temp directory.
	
	boost::filesystem::path temp_dir = boost::filesystem::temp_directory_path();
	std::string plugin_path(compressed_file.filename().generic_string());
	boost::replace_all(plugin_path, compressed_file.extension().generic_string(), "");
	temp_dir.append(plugin_path);
	boost::filesystem::remove_all(temp_dir); // make sure dir is clear
	boost::filesystem::create_directories(temp_dir);
	
	miniz_cpp::zip_file file(compressed_file.generic_string());
	file.extractall(temp_dir.generic_string());
	
	return temp_dir;
}
//--------------------------------------------------------------------
void plugin_utils::validate_plugin(const boost::filesystem::path& decompressed_plugin_path)
{
	// make sure at least one compiler/runtime/idl plugin exists
	boost::filesystem::recursive_directory_iterator rdi(decompressed_plugin_path);
	boost::filesystem::recursive_directory_iterator end_rdi;
	
	bool is_exist_plugin = false;
	
	for (; rdi != end_rdi; rdi++)
	{
		if(is_regular_file(rdi->path()))
		{
			std::string filename = rdi->path().filename().generic_string();
			transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
			if( filename.find("xllr.") != std::string::npos ||
					filename.find("metaffi.compiler.") != std::string::npos ||
					filename.find("metaffi.idl.") != std::string::npos)
			{
				is_exist_plugin = true;
				break;
			}
		}
	}
	
	if(!is_exist_plugin){
		throw std::runtime_error("Package does not contain at least one plugin (runtime, compiler, IDL)");
	}
	
}
//--------------------------------------------------------------------
void plugin_utils::copy_plugin_package(const boost::filesystem::path& decompressed_plugin_path)
{
	std::string target_path = get_install_path()+"/"+decompressed_plugin_path.filename().generic_string();
	std::filesystem::copy(decompressed_plugin_path.generic_string(), target_path, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
	boost::filesystem::remove_all(decompressed_plugin_path);
	
	// move plugin files to METAFFI_HOME
	boost::filesystem::recursive_directory_iterator rdi(target_path);
	boost::filesystem::recursive_directory_iterator end_rdi;
	for (; rdi != end_rdi; rdi++)
	{
		if(is_regular_file(rdi->path()))
		{
			std::string filename = rdi->path().filename().generic_string();
			transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
			if( filename.find("xllr.") != std::string::npos ||
			    filename.find("metaffi.compiler.") != std::string::npos ||
			    filename.find("metaffi.idl.") != std::string::npos)
			{
				auto src = rdi->path();
				auto dst = rdi->path().parent_path().parent_path().append(rdi->path().filename());
				boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
				boost::filesystem::remove(rdi->path());
			}
		}
	}
	
	// if target_path is empty, remove it
	if(boost::filesystem::is_empty(target_path))
	{
		boost::filesystem::remove(target_path);
	}
}
//--------------------------------------------------------------------
void plugin_utils::pack(const std::vector<std::string>& files_and_dirs, const std::string& root)
{
	// compress directory
	miniz_cpp::zip_file file;
	std::string name;
	plugin_type t;
	for(auto& file_or_dir : files_and_dirs)
	{
		boost::filesystem::path p(root);
		p.append(file_or_dir);
		
		if(!exists(p))
		{
			std::stringstream ss;
			ss << p.generic_string() << " cannot be found";
			throw std::runtime_error(ss.str());
		}
		
		if(p.filename_is_dot() || p.filename_is_dot_dot()){
			continue;
		}
		
		file.write(absolute(p).generic_string(), file_or_dir);
		
		if(name.empty())
		{
			std::string temp;
			if(extract_plugin_name_and_type(p, temp, t)){
				name = temp;
			}
		}
	}
	
	if(name.empty())
	{
		throw std::runtime_error("Did not find any MetaFFI plugin file");
	}
	
	file.save(name+".mffipack");
}
//--------------------------------------------------------------------
bool plugin_utils::extract_plugin_name_and_type(const boost::filesystem::path& path, std::string& out_name, plugin_type& out_type)
{
	if(path.filename_is_dot() || path.filename_is_dot_dot() || is_directory(path)){
		return false;
	}
	
	std::string filename = path.filename().generic_string();
	
	std::stringstream pattern;
	pattern << R"(metaffi\.compiler\.([a-zA-Z0-9_]+)\)" << boost::dll::shared_library::suffix().generic_string();
	pattern << R"(|xllr\.([a-zA-Z0-9_]+)\)" << boost::dll::shared_library::suffix().generic_string();
	pattern << R"(|metaffi\.idl\.([a-zA-Z0-9_]+)\)" << boost::dll::shared_library::suffix().generic_string();
	std::regex plugin_name_pattern(pattern.str(), std::regex::icase);
	
	std::sregex_iterator matches_begin(filename.begin(), filename.end(), plugin_name_pattern); // match and extract plugin name
	if(matches_begin != std::sregex_iterator() && matches_begin->size() > 1) // make sure subgroup got matched
	{
		out_name = (*matches_begin)[1].str();
		
		std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
		if(filename.find("metaffi.compiler.") != std::string::npos)
		{
			out_type = plugin_type::compiler_plugin;
		}
		else if(filename.find("xllr.") != std::string::npos)
		{
			out_type = plugin_type::runtime_plugin;
		}
		else
		{
			out_type = plugin_type::idl_plugin;
		}
		
		return true;
	}
	
	return false;
}
//--------------------------------------------------------------------