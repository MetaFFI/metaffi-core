# XLLR of the *Go* Plugin

Go XLLR plugin calls functions in Go C-Shared dynamic libraries.
It loads the foreign modules, loads the exported foreign function stub (created by the OpenFFI compiler) and calls it.
The XLLR plugin is used when **Calling to** *Go*

## xllr_plugin_interface

Does not do anything. Runtime loading is not explicit.
Loading the C-Shared module makes 
```
void load_runtime(
    char** err, // out error string - null if there is none
    uint32_t* err_len // length of error string
)
```

---

Does not do anything
```
void free_runtime(
    char** err, // out error string - null if there is none
    uint32_t* err_len // length of error string
)
```

---

Loads Go C-Shared module into `modules_repository`
```
void load_module(
    const char* module, // module to load
    uint32_t module_len, // module to load length
    char** err, // out error string - null if there is none
    uint32_t* err_len // length of error string
)
```

---

Does not do anything as freeing Go C-Shared library is [not supported](https://github.com/golang/go/issues/11100) at the time of the plugin implementation
```
void free_module(
    const char* module, // module to load
    uint32_t module_len, // module to load length
    char** err, // out error string - null if there is none
    uint32_t* err_len // length of error string
)
```

---

Lazy load the module, if not loaded.\
Loads the requested function and calls it.\

The code generated in the host language by the OpenFFI compiler (when compiling "from") is calling a stub function of the foreign function generated the OpenFFI compiler (when compiling "to").\
The function signature must be of the expected signature:
```
func f(unsigned char* in_params, uint64_t in_params_len, 
       unsigned char** out_params, uint64_t* out_params_len,
        unsigned char** out_ret, uint64_t* out_ret_len,
        uint8_t* is_error)
```

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

