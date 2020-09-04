# Compiler of the *Python3* Plugin

A compiler plugin, is a dynamic library (`.so`, `.dll`, `.dylib`) which exports *compiler_plugin_interface* interface declared in `CLI/compiler_plugin_interface.h`.

### Prerequisites For Development
The Python3 compiler plugin is written in *Go* and uses Protobuf and `.proto` as the serializer and IDL.

This plugin requires during development:
* [Go](https://golang.org/dl/)
* [Protobuf](https://developers.Python3ogle.com/protocol-buffers/docs/downloads) 

### Prerequisites For Deployment

* Stock Python3 installed

### Using this plugin as a baseline for other plugins

To reuse this plugin for another plugin, the main changes are:
* Change the names from "Python3" to the new target language
* Remove/change the build process of guest code
* Rewrite GuestTemplate and HostTemplate to fit your language (**This is most of the work**)

Proto parsing and creation of TemplateParameters are quite generic, for this reason, their code is in `CLI/utils` and not in `CLI/python3`.

## Design

The plugin uses Go's [text/template](https://golang.org/pkg/text/template/) package to compile the IDL.

The files `GuestTemplate.go` and `HostTemplate.go` contains templates for the generated files.\
By parsing `.proto` file using *ProtoParser* (`CLI/utils/ProtoParser.go`) the plugin creates an instance of *TemplateParameters* (`CLI/utils/TemplateParameters.go`) which is used to generate the guest and host code.

## compiler_plugin_interface

Compiles IDL to **Python3 module** (`.py` file) called from XLLR to the foreign function.

The function assumes Protobuf serialization code is available in the output path. OpenFFI makes sure of that using `compile_serialization` function. Notice OpenFFI will not call `compile_serialization` function if `--skip-compile-serialization` option switch is set.

Steps of the function:
1. Parse `.proto` modules (i.e. Proto `service`s)
2. Creates TemplateParameters instance with the parsed data
3. Generates the foreign function stubs code using **GuestTemplate** and Go's `test/template` package. GuestTemplate generates for each module and for each function within the module a "foreign function" stub with using the format "Foreign[foregin function name]" that does the following:
    1. Deserialize parameters using Protobuf
    2. Call foreign function & receive return values or exception
    3. In the case of returned error or Python exception, catch the exception and return error using `out_err`
    4. Serialize the return values
    5. Return result to XLLR
4. Write the generated code to `[idl filename]OpenFFIGuest.py`
```
compile_to_guest(const char* idl_path, uint32_t idl_path_length, const char* output_path, uint32_t output_path_length, char** out_err, uint32_t* out_err_len)
```

---

Compile IDL to code calling to XLLR from host code.

The parameters of the functions must be in host language data types.

Steps of the function:
1. Parse `.proto` modules (i.e. Proto `service`s)
2. Creates TemplateParameters instance with the parsed data
3. Generates the foreign function stubs code using **HostTemplate** and Go's `test/template` package. HostTemplate generates for each module and for each function within the module a "foreign function" stub. The stub returns the return values of the foreign function & wrap the code in try/except. The function does the following:
    1. Serialize parameters using Protobuf
    2. Call XLLR which passes the **foreign function stub** as the function to call & receive return values or error
    3. In case of error, return raise `RuntimeError`
    4. Deserialize the return values
    5. Return result to the caller
4. Write the generated code to `[idl filename]OpenFFIHost.py` 
```
compile_from_host(const char* idl_path, uint32_t idl_path_length, const char* output_path, uint32_t output_path_length, char** out_err, uint32_t* out_err_len)
```

---
   
Compile IDL to serialization code.
The function compiles the `.proto` using `protoc`
```
compile_serialization(const char* idl_path, uint32_t idl_path_length, const char* output_path, uint32_t output_path_length, char** out_err, uint32_t* out_err_len)
```
