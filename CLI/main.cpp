#include "exception_handler.h"
#include <iostream>
#include "cli_executor.h"
#include <spdlog/spdlog.h>
#include <cstdio>

namespace po = boost::program_options;

//--------------------------------------------------------------------
int main(int argc, char** argv)
{
	fprintf(stderr, "+++ metaffi_main: start\n"); fflush(stderr);
	int exit_code = 0;
	try
	{
		cli_executor cexec(argc, argv);

		if(!cexec.parse())
		{
			exit_code = 2;
			fprintf(stderr, "+++ metaffi_main: cli_executor parse returned false, exiting with 2\n"); fflush(stderr);
			spdlog::shutdown();
			return exit_code;
		}

		fprintf(stderr, "+++ metaffi_main: cli_executor parse done successfully\n"); fflush(stderr);
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

	fprintf(stderr, "+++ metaffi_main: spdlog::shutdown begin\n"); fflush(stderr);
	spdlog::shutdown();
	fprintf(stderr, "+++ metaffi_main: returning exit_code=%d (static destructors follow)\n", exit_code); fflush(stderr);
	return exit_code;
}
//--------------------------------------------------------------------
