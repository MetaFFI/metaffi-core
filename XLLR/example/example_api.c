#include "example_api.h"
#include <stdio.h>
#include <string.h>

int runtime_loaded = 0;

//--------------------------------------------------------------------
void load_runtime(char** err, uint32_t* err_len)
{
    printf("Loading Runtime of XLLR Example plugin\n");
	runtime_loaded = 1;
}
//--------------------------------------------------------------------
void free_runtime(char** err, uint32_t* err_len)
{
	if(runtime_loaded == 1)
	{
		printf("Freeing Runtime of XLLR Example plugin\n");
		runtime_loaded = 0;
	}
	else
	{
		*err = "Runtime has not been loaded - therefore it cannot be freed!\n";
		printf("%s", *err);
		*err_len = strlen(*err);
	}
}
//--------------------------------------------------------------------
