#include "cli_executor.h"
#include <iostream>

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
		("from-langs,l", po::value<std::vector<std::string>>()->multitoken() , "List of languages the functions are called from (i.e. host languages)")
		("redist", "Copyies to output directory OpenFFI redistrabutable binaries for deployment (TBD!)");

	_install_options.add_options()
		("deps,d", po::value<std::string>(), "Download & install OpenFFI dependencies")
		("lang", po::value<std::string>(), "Download & install OpenFFI supported language from given URL")
		("list", "List installed OpenFFI languages");

	_openffi_options.add(_compile_options);
	_openffi_options.add(_install_options);
}
//--------------------------------------------------------------------
bool cli_executor::parse(void)
{
	// parse command line
	po::store(po::command_line_parser(this->_argc, this->_argv).options(_openffi_options).run(), vm);
	po::notify(vm);

	// if no args or help - call help
	if(vm.size() == 0 || 
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
bool cli_executor::compile(void)
{
	// compile menu
	po::store(po::command_line_parser(this->_argc, this->_argv).options(_compile_options).allow_unregistered().run(), vm);
	po::notify(vm);

	if(vm.count("idl"))
	{
		std::cout << "IDL: " << vm["idl"].as<std::string>() << std::endl;
	}
	else if(vm.count("from-langs"))
	{
		std::cout << "from-langs: " << vm["from-langs"].as<std::vector<std::string>>().size() << std::endl;
	}
	else if(vm.count("redist"))
	{
		std::cout << "Copy redist" << std::endl;
	}
	else
	{
		_compile_options.print(std::cout);
		return false;
	}

	return true;
}
//--------------------------------------------------------------------
bool cli_executor::install(void)
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