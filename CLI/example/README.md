# Compiler of the *Example* Plugin

A compiler plugin, is a dynamic library (`.so`, `.dll`, `.dylib`) which exports *compiler_plugin_interface* interface declared in `CLI/compiler_plugin_interface.h`.

## compiler_plugin_interface

Compiles IDL to **executable code** called from XLLR to the foreign function\
Executable code means it is ready to run.

For compiled languages (e.g. C++, Go, Java), `compile_to_guest` should generate a compiled module to be loaded and called from XLLR.

For interpreted language (e.g. Python), `compile_to_guest` should generate a code-based module to be loaded and called from XLLR.
```
compile_to_guest(const char* idl_path, uint32_t idl_path_length, const char* output_path, uint32_t output_path_length, char** out_err, uint32_t* out_err_len)
```

---

Compile IDL to **code** calling to XLLR from host code.\
The host language uses this code to **easily** call XLLR. The function needs to generate a function with host language data types, serialize the parameters using a serialization mechanism and call XLLR 
```
compile_from_host(const char* idl_path, uint32_t idl_path_length, const char* output_path, uint32_t output_path_length, char** out_err, uint32_t* out_err_len)
```

---
   
Compile IDL to serialization code.\
The function compiles the IDL to serialization code in the plugin language.
 
For example, if plugin for language *L* uses *Protobuf* for serialization, this function executes "protoc" to compile `.proto` IDL to the serialization code in language *L*
```
compile_serialization(const char* idl_path, uint32_t idl_path_length, const char* output_path, uint32_t output_path_length, char** out_err, uint32_t* out_err_len)
```
