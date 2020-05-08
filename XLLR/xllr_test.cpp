#include <iostream>
#include "xllr_api.h"
#include <string>

int main()
{
	std::string plugin("xllr.example.so");
	char* out_err = nullptr;
	uint32_t out_err_len = 0;

	load_runtime_plugin(plugin.c_str(), plugin.length(), &out_err, &out_err_len);
	if(out_err != nullptr)
	{
		std::string estr(out_err, out_err_len);
		std::cout << "Error in load_runtime_plugin(): " << estr << std::endl;
		return 1;
	}

	free_runtime_plugin(plugin.c_str(), plugin.length(), &out_err, &out_err_len);
	if(out_err != nullptr)
	{
		std::string estr(out_err, out_err_len);
		std::cout << "Error in free_runtime_plugin(): " << estr << std::endl;
		return 1;
	}

	return 0;
}