#pragma once
#include <stdint.h>

/**
 * Interface Compiler Plugin Implements
 */ 
struct compiler_plugin_interface
{
	/**
	 * Compile IDL to code called from XLLR to the foreign function
	 */ 
	virtual void compile_to_guest(const char* idl_path, uint32_t idl_path_length, const char* output_path, uint32_t output_path_length, char** out_err, uint32_t* out_err_len) = 0;

	/**
	 * Compile IDL to code calling to XLLR from host code
	 */ 
	virtual void compile_from_host(const char* idl_path, uint32_t idl_path_length, const char* output_path, uint32_t output_path_length, char** out_err, uint32_t* out_err_len) = 0;
};
