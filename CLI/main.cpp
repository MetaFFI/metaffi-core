#include "exception_handler.h"
#include <iostream>
#include "cli_executor.h"

namespace po = boost::program_options;

//--------------------------------------------------------------------
int main(int argc, char** argv)
{
	try
	{	
		cli_executor cexec(argc, argv);
		if(!cexec.parse())
		{
			return 2;
		}

	}
	catch(std::exception& err)
	{
		handle_exception(err.what());
		return 1;
	}
	catch(...)
	{
		handle_exception();
		return 1;
	}

	return 0;
}
//--------------------------------------------------------------------