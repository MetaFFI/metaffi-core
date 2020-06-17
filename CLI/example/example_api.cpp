#include "example_api.h"
#include <string>
#include <iostream>
#include <stdlib.h>

#define return_error(desc) \
	std::string err(desc);\
	*out_err = (char*)malloc(err.length()+1);\
	*out_err_len = err.length();\
	err.copy(*out_err, *out_err_len);\
	return;

//--------------------------------------------------------------------
void compile_to_guest(const char* idl_path, uint32_t idl_path_length,
                      const char* output_path, uint32_t output_path_length,
                      char** out_err, uint32_t* out_err_len)
{
	if(idl_path == nullptr)
	{
		return_error("IDL path is null");
	}
	
	if(output_path == nullptr)
	{
		return_error("output path is null");
	}
	
	std::string idl(idl_path, idl_path_length);
	std::string output(output_path, output_path_length);
	
	if(idl.empty())
	{
		return_error("IDL path is empty");
	}
	
	if(output.empty())
	{
		return_error("output path is empty");
	}
	
	std::cout << "Compiling \"to language\" using IDL \"" << idl << "\" to path: " << (output.empty() ? "Current directory" : output) << std::endl;
}
//--------------------------------------------------------------------
void compile_from_host(const char* idl_path, uint32_t idl_path_length,
                       const char* output_path, uint32_t output_path_length,
                       char** out_err, uint32_t* out_err_len)
{
	if(idl_path == nullptr)
	{
		return_error("IDL path is null");
	}
	
	if(output_path == nullptr)
	{
		return_error("output path is null");
	}
	
	std::string idl(idl_path, idl_path_length);
	std::string output(output_path, output_path_length);
	
	if(idl.empty())
	{
		return_error("IDL path is empty");
	}
	
	if(output.empty())
	{
		return_error("output path is empty");
	}
	
	std::cout << "Compiling \"from language\" using IDL \"" << idl << "\" to path: " << (output.empty() ? "Current directory" : output) << std::endl;
}
//--------------------------------------------------------------------