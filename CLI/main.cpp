#include "exception_handler.h"
#include <iostream>
#include "cli_executor.h"
#include <spdlog/spdlog.h>

namespace po = boost::program_options;

//--------------------------------------------------------------------
int main(int argc, char** argv)
{
	int exit_code = 0;
	try
	{
		cli_executor cexec(argc, argv);
		
		if(!cexec.parse())
		{
			exit_code = 2;
			spdlog::shutdown();
			return exit_code;
		}
		
	}
	catch(std::exception& err)
	{
		handle_exception(err.what());
		exit_code = 1;
		spdlog::shutdown();
		return exit_code;
	}
	catch(...)
	{
		handle_exception();
		exit_code = 1;
		spdlog::shutdown();
		return exit_code;
	}

	spdlog::shutdown();
	return exit_code;
}
//--------------------------------------------------------------------
