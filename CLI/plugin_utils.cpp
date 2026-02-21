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
#include <iostream>
#include <utility>
#include <fstream>

using namespace metaffi::utils;

namespace
{
struct command_result
{
	std::string command;
	int exit_code;
};

struct uninstall_candidate
{
	std::filesystem::path artifact;
	std::string command;
	bool is_legacy_python;
};

std::string quote_arg(const std::string& value)
{
	std::string escaped = value;
	boost::replace_all(escaped, "\"", "\\\"");
	return "\"" + escaped + "\"";
}

std::string path_arg(const std::filesystem::path& p)
{
	return quote_arg(p.generic_string());
}

std::string to_lower_copy(std::string value)
{
	std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
	return value;
}

bool has_extension(const std::filesystem::path& p, const std::string& extension_lower)
{
	return to_lower_copy(p.extension().generic_string()) == extension_lower;
}

bool python_installer_supports_contract(const std::filesystem::path& installer_artifact)
{
	if(!has_extension(installer_artifact, ".py"))
	{
		return false;
	}

	std::ifstream in(installer_artifact, std::ios::in | std::ios::binary);
	if(!in.good())
	{
		return false;
	}

	std::stringstream ss;
	ss << in.rdbuf();
	const std::string content = ss.str();

	return content.find("--check-prerequisites") != std::string::npos &&
	       content.find("--install") != std::string::npos;
}

int run_command(const std::string& command)
{
	std::cout << command << std::endl;
	return system(command.c_str());
}

std::string format_attempts(const std::vector<command_result>& failures)
{
	std::stringstream ss;
	for(const auto& failed : failures)
	{
		ss << "\n  command: " << failed.command << "\n  exit_code: " << failed.exit_code << "\n";
	}
	return ss.str();
}

std::vector<std::string> build_install_commands(const std::filesystem::path& installer_artifact)
{
	std::vector<std::string> commands;
	const std::string artifact = path_arg(installer_artifact);

#ifdef _WIN32
	if(has_extension(installer_artifact, ".py"))
	{
		commands.emplace_back("python " + artifact + " --install");
		commands.emplace_back("python " + artifact + " install");
	}
	else if(has_extension(installer_artifact, ".ps1"))
	{
		commands.emplace_back("powershell -ExecutionPolicy ByPass -File " + artifact + " --install");
		commands.emplace_back("powershell -ExecutionPolicy ByPass -File " + artifact + " install");
	}
	else
	{
		commands.emplace_back(artifact + " --install");
		commands.emplace_back(artifact + " install");
	}
#else
	if(has_extension(installer_artifact, ".py"))
	{
		commands.emplace_back("python3 " + artifact + " --install");
		commands.emplace_back("python3 " + artifact + " install");
	}
	else if(has_extension(installer_artifact, ".sh"))
	{
		commands.emplace_back("bash " + artifact + " --install");
		commands.emplace_back("bash " + artifact + " install");
	}
	else
	{
		commands.emplace_back(artifact + " --install");
		commands.emplace_back(artifact + " install");
	}
#endif

	return commands;
}

std::vector<uninstall_candidate> build_uninstall_candidates(const std::filesystem::path& plugin_dir)
{
	std::vector<uninstall_candidate> candidates;

#ifdef _WIN32
	std::filesystem::path uninstall_exe = plugin_dir / "uninstall_plugin.exe";
	candidates.push_back({uninstall_exe, path_arg(uninstall_exe), false});

	std::filesystem::path uninstall_script = plugin_dir / "uninstall.bat";
	candidates.push_back({uninstall_script, path_arg(uninstall_script), false});

	std::filesystem::path uninstall_plugin_py = plugin_dir / "uninstall_plugin.py";
	candidates.push_back({uninstall_plugin_py, "python " + path_arg(uninstall_plugin_py), true});

	std::filesystem::path uninstall_py = plugin_dir / "uninstall.py";
	candidates.push_back({uninstall_py, "python " + path_arg(uninstall_py), true});
#else
	std::filesystem::path uninstall_exe = plugin_dir / "uninstall_plugin";
	candidates.push_back({uninstall_exe, path_arg(uninstall_exe), false});

	std::filesystem::path uninstall_script = plugin_dir / "uninstall.sh";
	candidates.push_back({uninstall_script, "bash " + path_arg(uninstall_script), false});

	std::filesystem::path uninstall_plugin_py = plugin_dir / "uninstall_plugin.py";
	candidates.push_back({uninstall_plugin_py, "python3 " + path_arg(uninstall_plugin_py), true});

	std::filesystem::path uninstall_py = plugin_dir / "uninstall.py";
	candidates.push_back({uninstall_py, "python3 " + path_arg(uninstall_py), true});
#endif

	return candidates;
}
}

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
	std::string lowered_url_or_path = to_lower_copy(url_or_path);
	std::filesystem::path installer_artifact;
	bool is_delete_file = false;
	scope_guard sg([&]()
	{
		if(is_delete_file && std::filesystem::exists(installer_artifact)){
			std::filesystem::remove(installer_artifact);
		}
	});
	
	if(lowered_url_or_path.rfind("http", 0) == 0) // if starts with "http" - download
	{
		is_delete_file = true;
		installer_artifact = download(url_or_path);
	}
	else
	{
		installer_artifact = url_or_path;
		if(!std::filesystem::exists(installer_artifact)){
			throw std::runtime_error("Plugin path doesn't exist");
		}
	}

	std::vector<std::string> install_commands = build_install_commands(installer_artifact);
	std::vector<command_result> failures;

	std::cout << "Installing plugin" << std::endl;

	// Fail-fast prerequisite gate for contract-aware Python installers.
	if(python_installer_supports_contract(installer_artifact))
	{
#ifdef _WIN32
		const std::string check_command = "python " + path_arg(installer_artifact) + " --check-prerequisites";
#else
		const std::string check_command = "python3 " + path_arg(installer_artifact) + " --check-prerequisites";
#endif
		int check_exit_code = run_command(check_command);
		if(check_exit_code != 0)
		{
			std::stringstream err;
			err << "Plugin prerequisites check failed. command: " << check_command << ", exit_code: " << check_exit_code;
			throw std::runtime_error(err.str());
		}
	}

	for(const auto& command : install_commands)
	{
		int exit_code = run_command(command);
		if(exit_code == 0)
		{
			std::cout << "Installation completed" << std::endl;
			return;
		}

		failures.push_back({command, exit_code});
	}

	std::stringstream err;
	err << "Failed to install plugin. Attempted commands:" << format_attempts(failures);
	throw std::runtime_error(err.str());
}
//--------------------------------------------------------------------
void plugin_utils::remove(const std::string& name)
{
	if(!is_installed(name)){
		throw std::runtime_error("Plugin not installed");
	}

	std::filesystem::path plugin_dir = std::filesystem::path(get_install_path()) / name;
	if(!std::filesystem::exists(plugin_dir) || !std::filesystem::is_directory(plugin_dir))
	{
		throw std::runtime_error("Plugin directory not found");
	}

	std::vector<uninstall_candidate> candidates = build_uninstall_candidates(plugin_dir);
	std::vector<command_result> failures;
	bool found_candidate = false;

	std::cout << "Uninstalling plugin" << std::endl;
	for(const auto& candidate : candidates)
	{
		if(!std::filesystem::exists(candidate.artifact))
		{
			continue;
		}

		found_candidate = true;
		if(candidate.is_legacy_python)
		{
			std::cout << "WARNING: Falling back to legacy Python plugin uninstaller: " << candidate.artifact.generic_string() << std::endl;
		}

		int exit_code = run_command(candidate.command);
		if(exit_code == 0)
		{
			std::cout << "Uninstallation completed" << std::endl;
			return;
		}

		failures.push_back({candidate.command, exit_code});
	}

	if(!found_candidate)
	{
		throw std::runtime_error("Uninstaller not found. Expected uninstall_plugin(.exe), uninstall.(bat|sh), uninstall_plugin.py, or uninstall.py");
	}

	std::stringstream err;
	err << "Failed to uninstall plugin. Attempted commands:" << format_attempts(failures);
	throw std::runtime_error(err.str());
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
