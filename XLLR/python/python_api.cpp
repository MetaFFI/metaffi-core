#include "python_api.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../utils/scope_guard.hpp"
#include "utils.h"
#include <boost/filesystem.hpp>
#include <python3.7/Python.h>

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

#define handle_py_err(err, err_len) \
	std::string pyerr(get_py_error()); \
	handle_err_str(err, err_len, pyerr);

#define TRUE 1
#define FALSE 0

#define pyscope()\
	PyGILState_STATE gstate = PyGILState_Ensure(); \
	scope_guard sggstate([&]() \
	{ \
		PyGILState_Release(gstate); \
	});
	

//--------------------------------------------------------------------
void load_runtime(char** err, uint32_t* err_len)
{
	// load python runtime
	if(!Py_IsInitialized())
	{
		Py_InitializeEx(1); // 1 means register signal handlers
	}
	
	pyscope();
	
	add_sys_module_path(boost::filesystem::current_path().wstring());
}
//--------------------------------------------------------------------
void free_runtime(char** err, uint32_t* err_len)
{
	if(!Py_IsInitialized())
	{
		return;
	}
		
	int res = Py_FinalizeEx();
	if(res == -1)
	{
		handle_err(err, err_len, "Python finalization has failed!");
		return;
	}
}
//--------------------------------------------------------------------
void load_module(const char* mod, uint32_t module_len, char** err, uint32_t* err_len)
{
	if(!Py_IsInitialized())
	{
		handle_err(err, err_len, "Runtime has not been loaded - module cannot be loaded!");
		return;
	}
	
	pyscope();

	// copying to std::string to make sure the string is null terminated
	std::string module_name(mod, module_len);
	PyObject* pymod = PyImport_ImportModuleEx(module_name.c_str(), Py_None, Py_None, Py_None);

	if(!pymod)
	{
		// error has occurred
		handle_py_err(err, err_len);
		return;
	}

	Py_DecRef(pymod);
}
//--------------------------------------------------------------------
void free_module(const char* mod, uint32_t module_len, char** err, uint32_t* err_len)
{
	if(!Py_IsInitialized())
	{
		return;
	}
	
	pyscope();
	
	// copying to std::string to make sure the string is null terminated
	std::string module_name(mod, module_len);

	PyObject* modules = PyImport_GetModuleDict();
	if(!modules)
	{
		handle_err(err, err_len, "Failed to get modules dictionary!");
		return;
	}

	scope_guard sgmods([&]()
	{
		Py_DecRef(modules);
	});

	// free module
	PyObject* module_obj = PyDict_GetItemString(modules, module_name.c_str());
	if(!module_obj)
	{
		handle_err(err, err_len, "Module not been loaded!");
		return;
	}

	scope_guard sgmod([&]()
	{
		Py_DecRef(module_obj);
	});

	if(PyDict_DelItemString(modules, module_name.c_str()) == -1)
	{
		handle_err(err, err_len, "Failed to remove module from modules dictionary!");
		return;
	}

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
	// check if python is initialized
	if(!Py_IsInitialized())
	{
		handle_err((char**)out_ret, out_ret_len, "Cannot call, runtime has not been loaded!");
		*is_error = TRUE;
		return;
	}
	
	pyscope();
	
	// check if module is loaded initialized
	PyObject* modules = PyImport_GetModuleDict();
	scope_guard sgmodules([&]()
	{
		Py_DecRef(modules);
	});

	// copying to std::string to make sure the string is null terminated
	std::string module_name(mod, module_name_len);

	PyObject* module_obj = PyDict_GetItemString(modules, module_name.c_str());
	if(!module_obj || strcmp(module_obj->ob_type->tp_name, "module") != 0)
	{
		handle_err((char**)out_ret, out_ret_len, "Cannot call, Module cannot be loaded!");
		*is_error = TRUE;
		return;
	}

	scope_guard sgmodule([&]()
	{
		Py_DecRef(module_obj);
	});

	// load function
	std::string funcname(func_name, func_name_len);
	PyObject* pyfunc = PyObject_GetAttrString(module_obj, funcname.c_str());

	if(!pyfunc)
	{
		handle_err((char**)out_ret, out_ret_len, "Cannot find function in module!");
		*is_error = TRUE;
		return;
	}

	if(!PyCallable_Check(pyfunc))
	{
		handle_err((char**)out_ret, out_ret_len, "Requested function found in module, but it is not a function!");
		*is_error = TRUE;
		return;
	}

	// set parameters
	PyObject* pyParams = PyByteArray_FromStringAndSize((const char*)in_params, in_params_len);
	PyObject* paramsArray = PyTuple_New(1);
	scope_guard sgParams([&]()
	{
		Py_DecRef(paramsArray);
		Py_DecRef(pyParams);
	});

	PyTuple_SetItem(paramsArray, 0, pyParams);
	PyObject* res = PyObject_CallObject(pyfunc, paramsArray);
	if(!res)
	{
		handle_err((char**)out_ret, out_ret_len, "OpenFFI Python guest code returned with an exception! The generated code must return error as str. (something is wrong...");
		*is_error = TRUE;
		return;
	}

	PyObject* ret = nullptr;
	PyObject* out = nullptr;
	PyObject* errmsg = nullptr;

	scope_guard sgrets([&]()
	{
		Py_DecRef(res);
		if(ret && res != ret){ Py_DecRef(ret); }
		if(out && out != Py_None){ Py_DecRef(out); }
		if(errmsg && errmsg != Py_None){ Py_DecRef(errmsg); }
	});

	// if the return values is a turple:
	// first parameter is the result - serialized
	// second parameter is the out parameters - serialized OR empty
	if(!PyTuple_Check(res))
	{
		handle_err((char**)out_ret, out_ret_len, "OpenFFI Python guest code did not return a tuple. Expects a tuple! (something is wrong...");
		*is_error = TRUE;
		return;
	}
	
	if(PyTuple_Size(res) == 2) // (serialized return value, error string)
	{
		ret = PyTuple_GetItem(res, 0);
		errmsg = PyTuple_GetItem(res, 1);
	}
	else if(PyTuple_Size(res) == 3) // (serialized return value, serialized parameters value (out params support), error string)
	{
		ret = PyTuple_GetItem(res, 0);
		errmsg = PyTuple_GetItem(res, 1);
		out = PyTuple_GetItem(res, 2);
	}
	else
	{
		handle_err((char**)out_ret, out_ret_len, "OpenFFI Python guest code expected a tuple of size 2 or 3 (something is wrong...");
		*is_error = TRUE;
		return;
	}
	
	if(errmsg != nullptr && errmsg != Py_None) // error has occurred
	{
		const char* perrmsg = PyUnicode_AsUTF8(errmsg);
		handle_err((char**)out_ret, out_ret_len, perrmsg);
		*is_error = TRUE;
		return;
	}
	
	// get out parameters
	if(out && out != Py_None)
	{
		*out_params_len = PyByteArray_Size(out);
		*out_params = (unsigned char*)malloc(*out_params_len);
		memcpy(*out_params, (unsigned char*)PyByteArray_AsString(out), *out_params_len);
	}

	// get return values
	// serialized proto results are in "str"
	*out_ret_len = PyByteArray_Size(ret);
	*out_ret = (unsigned char*)malloc(*out_ret_len);
	memcpy(*out_ret, (unsigned char*)PyByteArray_AsString(ret), *out_ret_len);	

	*is_error = FALSE;
}
//--------------------------------------------------------------------
