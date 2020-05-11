#include "example_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

int runtime_loaded = FALSE;
int module_loaded = FALSE;

#define handle_err(err, err_len, desc) \
	*err_len = strlen( desc ); \
	*err = (char*)malloc(*err_len + 1); \
	strcpy(*err, desc ); \
	printf("%.*s\n", *err_len, *err);

//--------------------------------------------------------------------
void load_runtime(char** err, uint32_t* err_len)
{
	if(runtime_loaded == TRUE)
	{
		handle_err(err, err_len, "Runtime has already been loaded!");
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
		handle_err(err, err_len, "Runtime has not been loaded - therefore it cannot be freed!");
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
		handle_err(err, err_len, "Runtime has not been loaded - module cannot be loaded!");
		return;
	}

	if(module_loaded == TRUE)
	{
		handle_err(err, err_len, "Module already been loaded!");
		return;
	}

	if(strcmp(module, "example_module") != 0)
	{
		handle_err(err, err_len, "Expected to load example module!");
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
		handle_err(err, err_len, "Runtime has not been loaded!");
		return;
	}

	if(module_loaded == FALSE)
	{
		handle_err(err, err_len, "Module not been loaded!");
		return;
	}

	printf("Freeing module %.*s in XLLR Example plugin\n", module_len, module);
	module_loaded = TRUE;
}
//--------------------------------------------------------------------
void call(
	const char* module_name, uint32_t module_name_len,
	const char* func_name, uint32_t func_name_len,
	unsigned char* in_params, uint64_t in_params_len,
	unsigned char** out_params, uint64_t* out_params_len,
	unsigned char** out_ret, uint64_t* out_ret_len,
	uint8_t* is_error
)
{
	if(runtime_loaded == FALSE)
	{
		*is_error = TRUE;
		handle_err((char**)out_ret, (uint32_t*)out_ret_len, "Runtime has not been loaded!");
		return;
	}

	if(module_loaded == FALSE)
	{
		*is_error = TRUE;
		handle_err((char**)out_ret, (uint32_t*)out_ret_len, "Module not been loaded!");
		return;
	}

	if(strcmp(func_name, "example_function") != 0)
	{
		*is_error = TRUE;
		handle_err((char**)out_ret, (uint32_t*)out_ret_len, "Expected to call example_function!");
		return;
	}

	printf("Called %.*s.%.*s in XLLR Example plugin\n", func_name_len, func_name, module_name_len, module_name);
	
	// fill out_params with "out params"
	*out_params_len = strlen("out params");
	*out_params = (unsigned char*)malloc(*out_params_len + 1);
	strcpy((char*)*out_params, "out params" );

	// fill ret_params with "ret params"
	*out_ret_len = strlen("ret params");
	*out_ret = (unsigned char*)malloc(*out_ret_len + 1);
	strcpy((char*)*out_ret, "ret params" );
	
	*is_error = FALSE;

}
//--------------------------------------------------------------------