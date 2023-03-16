#include "cli_executor.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include "plugin_utils.h"
#include "compiler.h"

namespace po = boost::program_options;

//--------------------------------------------------------------------
cli_executor::cli_executor(int argc, char** argv) :
		_argc(argc),
		_argv(argv),
		_metaffi_options("MetaFFI"),
		_compile_options("compile"),
		_plugin_options("plugin"),
		_pack_options("pack")
{
	_metaffi_options.add_options()
		("compile,c", "Compile definition file")
		("plugin", "Install dependencies and plugins")
		("help", "Display this help page");

	_compile_options.add_options()
		("idl", po::value<std::string>() , "Functions definitions (IDL or source code)")
		("block-name,n", po::value<std::string>(), "Name of embedded code block")
		("block-pattern", po::value<std::string>(), "Regular expression pattern of embedded code blocks names")
		("print-idl", "Prints MetaFFI IDL")
		("guest-lang,g", "Language the functions are implemented as stated in the IDL (i.e. guest language)")
		("host-langs,h", po::value<std::vector<std::string>>()->multitoken() , "List of languages the functions are called from (i.e. host languages)")
		("output,o", po::value<std::string>()->default_value(boost::filesystem::current_path().generic_string()) , "Directory to generate the files (Default: current directory)")
		("host-options", po::value<std::string>()->default_value(std::string()) , "Options to the host language plugin (format: key1=val1,key2=val2...)");

	_plugin_options.add_options()
		("install,i", po::value<std::string>(), "Download & install MetaFFI plugin from given URL or local path")
		("remove,r", po::value<std::string>(), "Remove supported plugin")
		("exist,e", po::value<std::string>(), "Is plugin installed")
		("confirm", po::value<std::string>(), "Confirms plugin installed. Sets exit error to 2 if not plugin is not installed")
		("pack", "Package files into a MetaFFI package")
		("list", "List installed MetaFFI plugins");
	
	_pack_options.add_options()
		("files", po::value<std::vector<std::string>>(), "Files to pack relative to the root directory")
		("root", po::value<std::string>(), "Root directory of files to pack");
	
	_plugin_options.add(_pack_options);
	_metaffi_options.add(_compile_options);
	_metaffi_options.add(_plugin_options);
}
//--------------------------------------------------------------------
bool cli_executor::parse()
{
	// parse command line
	po::store(po::command_line_parser(this->_argc, this->_argv).options(_metaffi_options).run(), vm);
	po::notify(vm);

	// if no args or help - call help
	if(vm.empty() ||
		vm.count("help") ||
		(vm.count("compile") && vm.count("install"))) // make sure only one option is selected
	{
		_metaffi_options.print(std::cout);
		return false;
	}

	if(vm.count("compile"))
	{
		return this->compile();
	}
	else if(vm.count("plugin"))
	{
		return this->plugin();
	}
	else
	{
		_metaffi_options.print(std::cout);
		return false;
	}
	
}
//--------------------------------------------------------------------
bool cli_executor::compile()
{
	// compile menu
	po::store(po::command_line_parser(this->_argc, this->_argv).options(_compile_options).allow_unregistered().run(), vm);
	po::notify(vm);
	
	if(!vm.count("idl"))
	{
		std::cout << "Missing IDL" << std::endl;
		_compile_options.print(std::cout);
		return false;
	}
	
	if(vm.count("host-langs") == 0 && vm.count("guest-lang") == 0)
	{
		std::cout << "Expects at least host-langs or guest-lang argument" << std::endl;
		_compile_options.print(std::cout);
		return false;
	}
	
	std::string embedded_name;
	bool is_pattern = false;
	
	if(vm.count("block-name") || vm.count("block-pattern"))
	{
		if(vm.count("block-name") && vm.count("block-pattern"))
		{
			std::cout << "block-name and block-pattern cannot be used together. Choose one of them" << std::endl;
			_compile_options.print(std::cout);
			return false;
		}
		
		if(vm.count("block-name"))
		{
			embedded_name = vm["block-name"].as<std::string>();
		}
		else
		{
			embedded_name = vm["block-pattern"].as<std::string>();
			is_pattern = true;
		}
	}
	
	compiler cmp(vm["idl"].as<std::string>(), vm["output"].as<std::string>(), embedded_name, is_pattern);
	
	if(vm.count("print-idl"))
	{
		cmp.print_idl();
	}
	
	if(vm.count("guest-lang"))
	{
		cmp.compile_to_guest();
	}
	
	if(vm.count("host-langs"))
	{
		cmp.compile_from_host(vm["host-langs"].as<std::vector<std::string>>(), vm["host-options"].as<std::string>());
	}
	
	return true;
}
//--------------------------------------------------------------------
bool cli_executor::plugin()
{
	// compile menu
	po::store(po::command_line_parser(this->_argc, this->_argv).options(_plugin_options).options(_pack_options).allow_unregistered().run(), vm);
	po::notify(vm);
	
	if(vm.count("install") > 0 && vm.count("remove") > 0)
	{
		std::cout << "Cannot choose both install and remove options" << std::endl;
		return false;
	}
	
	if(vm.count("install"))
	{
		plugin_utils::install(vm["install"].as<std::string>());
	}
	else if(vm.count("remove"))
	{
		plugin_utils::remove(vm["remove"].as<std::string>());
	}
	else if(vm.count("exist"))
	{
		std::string plugin = vm["exist"].as<std::string>();
		if(plugin_utils::is_installed(plugin)){
			std::cout << "installed" << std::endl;
		}
		else{
			std::cout << "not installed" << std::endl;
		}
	}
	else if(vm.count("confirm"))
	{
		std::string plugin = vm["confirm"].as<std::string>();
		if(plugin_utils::is_installed(plugin)){
			std::cout << "installed" << std::endl;
		}
		else
		{
			std::cout << "not installed" << std::endl;
			return false;
		}
	}
	else if(vm.count("pack"))
	{
		if(vm.count("root") == 0){
			throw std::runtime_error("Please specify package name");
		}
		
		if(vm.count("root") == 0){
			throw std::runtime_error("Please specify files to pack");
		}
		
		plugin_utils::pack(vm["files"].as<std::vector<std::string>>(), vm["root"].as<std::string>());
	}
	else if(vm.count("list"))
	{
		std::vector<std::string> plugins = plugin_utils::list();
		
		if(!plugins.empty())
		{
			for(const std::string& plugin : plugins)
			{
				std::cout << plugin << std::endl;
			}
		}
		else
		{
			std::cout << "No plugins installed :-(" << std::endl;
			std::cout << "Start installing using \"metaffi plugin -install [URL or local path]\" command!" << std::endl;
		}
	}
	else
	{
		_plugin_options.print(std::cout);
		return false;
	}

	return true;
}
//--------------------------------------------------------------------