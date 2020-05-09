#include "example_api.h"
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

int runtime_loaded = FALSE;
int module_loaded = FALSE;

//--------------------------------------------------------------------
void load_runtime(char** err, uint32_t* err_len)
{
	if(runtime_loaded == TRUE)
	{
		*err = "Runtime has already been loaded!\n";
		printf("%s\n", *err);
		*err_len = strlen(*err);
		return;
	}

    printf("Loading Runtime of XLLR Example plugin\n");
	runtime_loaded = TRUE;
}
//--------------------------------------------------------------------
void free_runtime(char** err, uint32_t* err_len)
{
	if(runtime_loaded == FALSE)
	{
		*err = "Runtime has not been loaded - therefore it cannot be freed!\n";
		printf("%s\n", *err);
		*err_len = strlen(*err);
		return;
	}
		
	printf("Freeing Runtime of XLLR Example plugin\n");
	runtime_loaded = FALSE;
}
//--------------------------------------------------------------------
void load_module(const char* module, uint32_t module_len, char** err, uint32_t* err_len)
{
	if(runtime_loaded == FALSE)
	{
		*err = "Runtime has not been loaded - module cannot be loaded!\n";
		printf("%s\n", *err);
		*err_len = strlen(*err);
		return;
	}

	if(module_loaded == TRUE)
	{
		*err = "Module already been loaded!\n";
		printf("%s\n", *err);
		*err_len = strlen(*err);
		return;
	}

	if(strcmp(module, "example_module") != 0)
	{
		*err = "Expected to load example module!\n";
		printf("%s\n", *err);
		*err_len = strlen(*err);
		return;
	}

	printf("Loading module %s in XLLR Example plugin\n", module);
	module_loaded = TRUE;
}
//--------------------------------------------------------------------
void free_module(const char* module, uint32_t module_len, char** err, uint32_t* err_len)
{
	if(runtime_loaded == FALSE)
	{
		*err = "Runtime has not been loaded!\n";
		printf("%s\n", *err);
		*err_len = strlen(*err);
		return;
	}

	if(module_loaded == FALSE)
	{
		*err = "Module not been loaded!\n";
		printf("%s\n", *err);
		*err_len = strlen(*err);
		return;
	}

	printf("Freeing module %s in XLLR Example plugin\n", module);
	module_loaded = TRUE;
}
//--------------------------------------------------------------------