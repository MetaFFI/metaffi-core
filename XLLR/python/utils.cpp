#include "utils.h"
#include "../../utils/scope_guard.hpp"
#include <Python.h>
#include <boost/predef.h>
#include <boost/filesystem.hpp>


//--------------------------------------------------------------------
std::string get_py_error(void)
{
	
	// error has occurred
	PyObject* ptype = nullptr;
	PyObject* pvalue = nullptr;
	PyObject* ptraceback = nullptr;
	PyErr_Fetch(&ptype, &pvalue, &ptraceback);
	
	if(!pvalue)
	{
		return std::string("No Error was found!");
	}
	
	// if pvalue is str, get text, if not, get msg from exception
	if(strcmp(pvalue->ob_type->tp_name, "str") == 0)
	{
		uint64_t size;
		const char* pmsg = PyUnicode_AsUTF8AndSize(pvalue, (Py_ssize_t*)&size);
		PyErr_Clear();
		return std::string(pmsg, size);
	}
	
	PyObject* msg = PyObject_CallMethod(pvalue, "__str__", nullptr);
	if(!msg)
	{
		return std::string("Failed to get error description: call to method \"__str__\" failed");
	}
	
	scope_guard sgmsg([&]()
	{
		if(msg){Py_DecRef(msg);}
	});

	uint64_t size;
	const char* pmsg = PyUnicode_AsUTF8AndSize(msg, (Py_ssize_t*)&size);
	return std::string(pmsg, size);
}
//--------------------------------------------------------------------
void add_sys_module_path(const std::wstring& pathToAdd)
{
	auto sep = []()->std::wstring {
#if BOOST_OS_WINDOWS
		return L";";
#else
		return L":";
#endif
	};
	
	std::wstring path(Py_GetPath());
	path += sep();
	
	std::wstring dist_packages = L"/usr/lib/python3/dist-packages";
	if(boost::filesystem::exists(dist_packages)){
		path += sep() + dist_packages;
	}

	PySys_SetPath(path.c_str());
}
//--------------------------------------------------------------------