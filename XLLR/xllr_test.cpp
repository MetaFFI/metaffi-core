#include <stdio.h>
#include "xllr_api.h"
#include <string>

int main()
{
	std::string plugin("xllr.example.so");
	char* out_err;
	uint32_t out_err_len;
	load_runtime_plugin(plugin.c_str(), plugin.length(), &out_err, &out_err_len);

	if(out_err != nullptr)
	{
		printf("Error: %s", out_err);
		return 1;
	}

	return 0;
}