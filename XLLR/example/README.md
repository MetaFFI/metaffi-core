# XLLR of the *Example* Plugin

An XLLR plugin, is a dynamic library (`.so`, `.dll`, `.dylib`) which exports *xllr_plugin_interface* interface declared in `XLLR/xllr_plugin_interface.h`.

The class `xllr_plugin` takes care that load_runtime, free_runtime, load_module and free_module are called once and not at the same time in a multi-threaded environment.

## xllr_plugin_interface

Load runtime of foreign runtime
```
void load_runtime(
    char** err, // out error string - null if there is none
    uint32_t* err_len // length of error string
)
```
 
Free runtime of foreign runtime
```
void free_runtime(
    char** err, // out error string - null if there is none
    uint32_t* err_len // length of error string
)
```
 
Load module of foreign language
```
void load_module(
    const char* module, // module to load
    uint32_t module_len, // module to load length
    char** err, // out error string - null if there is none
    uint32_t* err_len // length of error string
)
```

Free module of foreign language
```
void free_module(
    const char* module, // module to load
    uint32_t module_len, // module to load length
    char** err, // out error string - null if there is none
    uint32_t* err_len // length of error string
)
```

Call foreign function 
```
void call(
    
    // module foreign function is in. lazy load module if not loaded.
    const char* module_name, uint32_t module_name_len,
    
    // function name to call
    const char* func_name, uint32_t func_name_len,
    
    // serialized parameters
    unsigned char* in_params, uint64_t in_params_len,
    
    // out - serialized returned ref parameters
    unsigned char** out_params, uint64_t* out_params_len,
    
    // out - serialized result or error message
    unsigned char** out_ret, uint64_t* out_ret_len,
    
    // out - 0 if not an error, otherwise an error
    // if is_error is non-zero, out_ret is a string
    uint8_t* is_error
)
```
