#include "example_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

int runtime_loaded = FALSE;
int module_loaded = FALSE;

#define handle_err(desc) \
	*err_len = strlen( desc ); \
	*err = (char*)malloc(*err_len + 1); \
	strcpy(*err, desc ); \
	printf("%.*s\n", *err_len, *err);

//--------------------------------------------------------------------
void load_runtime(char** err, uint32_t* err_len)
{
	if(runtime_loaded == TRUE)
	{
		handle_err("Runtime has already been loaded!");
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
		handle_err("Runtime has not been loaded - therefore it cannot be freed!");
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
		handle_err("Runtime has not been loaded - module cannot be loaded!");
		return;
	}

	if(module_loaded == TRUE)
	{
		handle_err("Module already been loaded!");
		return;
	}

	if(strcmp(module, "example_module") != 0)
	{
		handle_err("Expected to load example module!");
		return;
	}

	printf("Loading module %.*s in XLLR Example plugin\n", module_len, module);
	module_loaded = TRUE;
}
//--------------------------------------------------------------------
void free_module(const char* module, uint32_t module_len, char** err, uint32_t* err_len)
{
	if(runtime_loaded == FALSE)
	{
		handle_err("Runtime has not been loaded!");
		return;
	}

	if(module_loaded == FALSE)
	{
		handle_err("Module not been loaded!");
		return;
	}

	printf("Freeing module %.*s in XLLR Example plugin\n", module_len, module);
	module_loaded = TRUE;
}
//--------------------------------------------------------------------