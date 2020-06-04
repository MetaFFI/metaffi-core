#include "utils.h"
#include "../scope_guard.hpp"
#include <python3.7/Python.h>

std::string get_py_error(void)
{
	// error has occurred
	PyObject* ptype = nullptr;
	PyObject* pvalue = nullptr;
	PyObject* ptraceback = nullptr;
	PyErr_Fetch(&ptype, &pvalue, &ptraceback);

	scope_guard sg([&]()
	{
		if(ptype){Py_DecRef(ptype);}
		if(pvalue){Py_DecRef(pvalue);}
		if(ptraceback){Py_DecRef(ptraceback);}
	});

	const char* pStrErrorMessage = PyUnicode_AsUTF8(pvalue);
	return std::string(pStrErrorMessage);
}