package main
import "fmt"

import "C"


//--------------------------------------------------------------------
//export compile_to_guest
func compile_to_guest(idl_path *C.char, idl_path_length C.uint,
					  output_path *C.char, output_path_length C.uint,
					  out_err **C.char, out_err_len *C.uint){

	//idlPath := C.GoStringN(idl_path, C.int(idl_path_length))
	//outPath := C.GoStringN(output_path, C.int(output_path_length))

}
//--------------------------------------------------------------------
//export compile_from_host
func compile_from_host(idl_path *C.char, idl_path_length C.uint,
	output_path *C.char, output_path_length C.uint,
	out_err **C.char, out_err_len *C.uint){

	fmt.Printf("Got to compile_from_host\n")

}
//--------------------------------------------------------------------
func main(){
}
//--------------------------------------------------------------------
