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
		_openffi_options("OpenFFI"),
		_compile_options("compile"),
		_plugin_options("plugin")
{
	_openffi_options.add_options()
		("compile,c", "Compile definition file")
		("plugin,p", "Install dependencies and plugins")
		("help", "Display this help page");

	_compile_options.add_options()
		("idl", po::value<std::string>() , "IDL containing functions defitions (i.e. foreign functions)")
		("to-lang,t", "Language the functions are implemented as stated in the IDL (i.e. guest language)")
		("from-langs,f", po::value<std::vector<std::string>>()->multitoken() , "List of languages the functions are called from (i.e. host languages)")
		("skip-compile-serialization", "Skip IDL compilation to serialization code")
		("output,o", po::value<std::string>()->default_value(boost::filesystem::current_path().generic_string()) , "Directory to generate the files (Default: current directory)")
		("redist", "Copies to output directory OpenFFI runtime binaries and required runtime plugins for deployment (TBD!)");

	_plugin_options.add_options()
		("install,i", po::value<std::string>(), "Download & install OpenFFI supported language from given URL or local path")
		("remove,r", po::value<std::string>(), "Remove supported language")
		("list", "List installed OpenFFI languages");

	_openffi_options.add(_compile_options);
	_openffi_options.add(_plugin_options);
}
//--------------------------------------------------------------------
bool cli_executor::parse()
{
	// parse command line
	po::store(po::command_line_parser(this->_argc, this->_argv).options(_openffi_options).run(), vm);
	po::notify(vm);

	// if no args or help - call help
	if(vm.empty() ||
		vm.count("help") ||
		(vm.count("compile") && vm.count("install"))) // make sure only one option is selected
	{
		_openffi_options.print(std::cout);
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
		_openffi_options.print(std::cout);
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
	
	if(!vm.count("from-langs") && !vm.count("to-lang"))
	{
		std::cout << "Expects at least from-langs or to-lang argument" << std::endl;
		_compile_options.print(std::cout);
		return false;
	}
	
	compiler cmp(vm["idl"].as<std::string>(), vm["output"].as<std::string>());
	
	bool compile_serialization_code = (vm.count("skip-compile-serialization") == 0);
	
	if(vm.count("to-lang"))
	{
		cmp.compile_to_guest(compile_serialization_code);
	}
	
	if(vm.count("from-langs"))
	{
		cmp.compile_from_host(vm["from-langs"].as<std::vector<std::string>>(), compile_serialization_code);
	}
	
	if(vm.count("redist"))
	{
		// TODO: implement redist
		std::cout << "Copy redist" << std::endl;
	}

	return true;
}
//--------------------------------------------------------------------
bool cli_executor::plugin()
{
	// compile menu
	po::store(po::command_line_parser(this->_argc, this->_argv).options(_plugin_options).allow_unregistered().run(), vm);
	po::notify(vm);
	
	if(vm.count("install") > 0 && vm.count("remove") > 0)
	{
		std::cout << "Cannot choose both install and remove options" << std::endl;
		return false;
	}
	
	if(vm.count("install"))
	{
		plugin_utils::install(vm["install"].as<std::string>(), true);
	}
	else if(vm.count("remove"))
	{
		plugin_utils::remove(vm["remove"].as<std::string>());
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
			std::cout << "Start installing using \"openffi plugin -install [URL or local path]\" command!" << std::endl;
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