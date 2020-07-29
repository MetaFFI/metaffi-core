//+build !windows

package main

/*
#include <dlfcn.h>

void* xllr_handle = NULL;
const char* load_library(const char* name) // return error string. null if no error.
{
	xllr_handle = (name, RTLD_NOW);
	if(!xllr_handle)
	{
		return dlerror();
	}

	return NULL;
}
 */
import "C"
import "runtime"
import "fmt"

//--------------------------------------------------------------------
func LoadXLLR() error{

	var name string
	if runtime.GOOS == "darwin" {
		name = C.CString("xllr.dylib")
	}else if runtime.GOOS == "windows"{
		name = C.CString("xllr.dll")
	} else {
		name = C.CString("xllr.so")
	}

	defer C.free(name)
	errstr := C.load_library(name)
	if errstr != nil{ // error has occurred
		return fmt.Errorf(C.GoString(errstr))
	}

	return nil
}
//--------------------------------------------------------------------