#include "exception_handler.h"
#include <boost/exception/all.hpp>
#include <iostream>

//--------------------------------------------------------------------
template<typename T>
void handle_exception(const std::string& custom_msg)
{
	try
	{
		if(custom_msg.empty())
		{
			std::cout << "Unknown error during compilation" << std::endl;
		}
		else
		{
			std::cout << custom_msg << std::endl;
		}
		
		throw;
	}
	catch (std::exception& err)
	{
		std::cout << err.what() << std::endl;
		if( const std::string* extra  = boost::get_error_info<T>(err) )
		{
			std::cout << *extra << std::endl;
		}
	
	}
	catch (...)
	{
		std::cout << "Unknown error" << std::endl;
	}
}
//--------------------------------------------------------------------
void handle_exception(const std::string& custom_msg)
{
	try
	{
		if(!custom_msg.empty()){
			std::cout << custom_msg << std::endl;
		}
		
		throw;
	}
	catch (std::exception& err)
	{
		std::cout << err.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Unknown error" << std::endl;
	}
}
//--------------------------------------------------------------------