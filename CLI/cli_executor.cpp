#include "cli_executor.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include "compiler.h"

namespace po = boost::program_options;

//--------------------------------------------------------------------
cli_executor::cli_executor(int argc, char** argv) :
	_argc(argc),
	_argv(argv),
	_openffi_options("OpenFFI"),
	_compile_options("compile"),
	_install_options("install")
{
	_openffi_options.add_options()
		("compile,c", "Compile definition file")
		("install,i", "Install dependencies and plugins (TBD!)")
		("help", "Display this help page");

	_compile_options.add_options()
		("idl", po::value<std::string>() , "IDL containing functions defitions (i.e. foreign functions)")
		("to-lang,t", "Language the functions are implemented as stated in the IDL (i.e. guest language)")
		("from-langs,f", po::value<std::vector<std::string>>()->multitoken() , "List of languages the functions are called from (i.e. host languages)")
		("skip-compile-serialization", "Skip IDL compilation to serialization code")
		("output,o", po::value<std::string>()->default_value(boost::filesystem::current_path().generic_string()) , "Directory to generate the files (Default: current directory)")
		("redist", "Copies to output directory OpenFFI redistrabutable binaries for deployment (TBD!)");

	_install_options.add_options()
		("deps,d", po::value<std::string>(), "Download & install OpenFFI & OpenFFI plugin dependencies (TBD!)")
		("lang", po::value<std::string>(), "Download & install OpenFFI supported language from given URL (TBD!)")
		("list", "List installed OpenFFI languages (TBD!)");

	_openffi_options.add(_compile_options);
	_openffi_options.add(_install_options);
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
	else if(vm.count("install"))
	{
		return this->install();
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
bool cli_executor::install()
{
	// compile menu
	po::store(po::command_line_parser(this->_argc, this->_argv).options(_install_options).allow_unregistered().run(), vm);
	po::notify(vm);

	if(vm.count("deps"))
	{
		std::cout << "Requesting to install OpenFFI depedencies" << std::endl;
	}
	else if(vm.count("lang"))
	{
		std::cout << "Requesting to install language: " << vm["lang"].as<std::string>() << std::endl;
	}
	else if(vm.count("list"))
	{
		std::cout << "Requesting to list installed languages" << std::endl;
	}
	else
	{
		_install_options.print(std::cout);
		return false;
	}

	return true;
}
//--------------------------------------------------------------------