#pragma once
/***
 * OpenFFI XLLR API 
 */

#include <stdint.h> 

/**
 * Load language runtime of foreign language
 */ 
void load_runtime_plugin(const char* runtime_plugin, uint32_t runtime_plugin_len, char** err, uint32_t* err_len);

/**
 * Free language runtime of foreign language
 */ 
void free_runtime_plugin(const char* runtime_plugin, uint32_t runtime_plugin_len, char** err, uint32_t* err_len);

/**
 * Load module of foreign language
 */ 
//void load_module(plugin_handle, const char* module, uint32_t module_len, char** err, uint32_t* err_len);

/**
 * Free module of foreign language
 */ 
//void free_module(module_handle module, char** err, uint32_t* err_len);


/***
 * Call foreign function
 */
/*
void call(

    // if runtime or module are not loaded, is initialize
    uint8_t is_initialize,

    // language to call
    const char* language, uint32_t language_len,

    // module to load
    const char* module_name, uint32_t module_name_len,
    
    // function name to call
    const char* func_name, uint32_t func_name_len,
    
    // serialized parameters
    unsigned char* params, uint64_t params_len,

    // serialized out parameters
    unsigned char** out_params, uint64_t* out_params_len,
    
    // out - serialized result or error message
    char** out_ret, uint64_t* out_ret_len,
    
    // out - 0 if not an error, otherwise an error
    uint8_t* is_error
);
*/