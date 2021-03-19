#include "go_api.h"
#include <utils/scope_guard.hpp>
#include <utils/function_loader.hpp>
#include <boost/filesystem.hpp>
#include "modules_repository.h"

using namespace openffi::utils;

#define handle_err(err, err_len, desc) \
	*err_len = strlen( desc ); \
	*err = (char*)malloc(*err_len + 1); \
	strcpy(*err, desc ); \
	memset((*err+*err_len), 0, 1);

#define handle_err_str(err, err_len, descstr) \
	*err_len = descstr.length(); \
	*err = (char*)malloc(*err_len + 1); \
	descstr.copy(*err, *err_len, 0); \
	memset((*err+*err_len), 0, 1);

#define TRUE 1
#define FALSE 0


//--------------------------------------------------------------------
void load_runtime(char** err, uint32_t* err_len){ /* No runtime to be loaded */ }
//--------------------------------------------------------------------
void free_runtime(char** err, uint32_t* err_len){ /* No runtime to be freed */ }
//--------------------------------------------------------------------
void load_module(const char* mod, uint32_t module_len, char** err, uint32_t* err_len)
{
	/*
	 * Load modules into modules repository - make sure every module is loaded once
	 */
	
	// if fails throws an exception which is handled by xllr_api.cpp.
	modules_repository::get_instance()[std::string(mod, module_len)];
	
}
//--------------------------------------------------------------------
void free_module(const char* /*mod*/, uint32_t /*module_len*/, char** /*err*/, uint32_t* /*err_len*/)
{
	/*
	 * Go doesn't support freeing modules
	 */
}
//--------------------------------------------------------------------
void call(
	const char* mod, uint32_t module_name_len,
	const char* func_name, uint32_t func_name_len,
	unsigned char* in_params, uint64_t in_params_len,
	unsigned char** out_params, uint64_t* out_params_len,
	unsigned char** out_ret, uint64_t* out_ret_len,
	uint8_t* is_error
)
{
	// get module
	std::shared_ptr<boost::dll::shared_library> pmod = modules_repository::get_instance()[std::string(mod, module_name_len)];
	
	// load requested function with the signature:
	// void F(  unsigned char* in_params, uint64_t in_params_len,
	//          char** out_params, uint64_t* out_params_len,
	//          char** out_ret, uint64_t* out_ret_len,
	//          uint8_t* is_error )
	auto func = load_func<void(unsigned char*, uint64_t,
                               unsigned char**, uint64_t*,
                               unsigned char**, uint64_t*,
                               uint8_t*)>(*pmod, std::string(func_name, func_name_len));
		
	// call function
	(*func)(in_params, in_params_len,
			out_params, out_params_len,
			out_ret, out_ret_len,
			is_error);
	
}
//--------------------------------------------------------------------
