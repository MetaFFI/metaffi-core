package main
import (
	"fmt"
	"io/ioutil"
	"os"
	"path/filepath"
)

import "C"


//--------------------------------------------------------------------
//export compile_to_guest
func compile_to_guest(idl_path *C.char, idl_path_length C.uint,
					  output_path *C.char, output_path_length C.uint,
					  out_err **C.char, out_err_len *C.uint){

	idlPath := C.GoStringN(idl_path, C.int(idl_path_length))
	outPath := C.GoStringN(output_path, C.int(output_path_length))

	// read proto file
	proto, err := ioutil.ReadFile(idlPath)
	if err != nil{
		msg := fmt.Sprintf("Failed to read %v. Error: %v", idlPath, err)
		*out_err = C.CString(msg)
		*out_err_len = C.uint(len(msg))
		return
	}

	// generate guest code
	_, idlfile := filepath.Split(idlPath)
	compiler, err := NewCompiler(string(proto), idlfile)
	if err != nil{
		msg := fmt.Sprintf("Failed to create compiler for proto file %v. Error: %v", idlPath, err)
		*out_err = C.CString(msg)
		*out_err_len = C.uint(len(msg))
		return
	}

	pyGuestCode, err := compiler.CompileGuest()
	if err != nil{
		msg := fmt.Sprintf("Failed to generate python guest code. Error: %v", err)
		*out_err = C.CString(msg)
		*out_err_len = C.uint(len(msg))
		return
	}

	// write to output path
	if _, err = os.Stat(outPath); os.IsNotExist(err) {
		err = os.Mkdir(outPath, os.ModeDir)
	}

	if err != nil{
		msg := fmt.Sprintf("Failed to create output directory %v. Error: %v", outPath, err)
		*out_err = C.CString(msg)
		*out_err_len = C.uint(len(msg))
		return
	}

	newfilepath := outPath + string(os.PathSeparator) + idlfile + "_fuse.py"
	err = ioutil.WriteFile(newfilepath, []byte(pyGuestCode), 0660)
	if err != nil{
		msg := fmt.Sprintf("Failed to write %v. Error: %v", newfilepath, err)
		*out_err = C.CString(msg)
		*out_err_len = C.uint(len(msg))
		return
	}

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
