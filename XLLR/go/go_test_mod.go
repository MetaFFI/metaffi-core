package main

import "C"
import "fmt"

//export go_test_func
func go_test_func(in_params *C.char, in_params_len C.ulonglong, out_params **C.char, out_params_len *C.ulonglong, out_ret **C.char, out_ret_len *C.ulonglong, is_error *C.char){

	*is_error = 0

	inParams := C.GoStringN(in_params, C.int(in_params_len))
	if inParams != "input"{
		*is_error = 1
		errMsg := fmt.Sprintf("Given in_params are not 'input'. in_params: '%v'", inParams)
		*out_ret = C.CString(errMsg)
		*out_ret_len = C.ulonglong(len(errMsg))
	}

	output := "result"
    *out_ret = C.CString(output)
    *out_ret_len = C.ulonglong(len(output))
}

func main(){}