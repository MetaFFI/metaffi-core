package main
import (
	"fmt"
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
)

import "C"

type compileDirection uint8

const (
	FROM_HOST compileDirection = 0
	TO_GUEST compileDirection = 1
)


//--------------------------------------------------------------------
func compile_idl(idlPath string, outPath string, direction compileDirection) error{

	// read proto file
    proto, err := ioutil.ReadFile(idlPath)
    if err != nil{
        return fmt.Errorf("Failed to read %v. Error: %v", idlPath, err)
    }

	// generate guest code
    _, idlfile := filepath.Split(idlPath)
    compiler, err := NewCompiler(string(proto), idlfile)
    if err != nil{
		return fmt.Errorf("Failed to create compiler for proto file %v. Error: %v", idlPath, err)
    }

	// write to output path
	if _, err = os.Stat(outPath); os.IsNotExist(err) {
		err = os.Mkdir(outPath, os.ModeDir)
	}

	if err != nil{
		return fmt.Errorf("Failed to create output directory %v. Error: %v", outPath, err)
	}

	var outputPostfix string
	var code string
	switch direction {
		case FROM_HOST:
			outputPostfix = "OpenFFIHost.py"
			code, err = compiler.CompileHost()
            if err != nil{
                return fmt.Errorf("Failed to generate code. Error: %v", err)
            }
			break

		case TO_GUEST:
			outputPostfix = "OpenFFIGuest.py"
			code, err = compiler.CompileGuest()
            if err != nil{
                return fmt.Errorf("Failed to generate code. Error: %v", err)
            }
			break

		default:
			return fmt.Errorf("Internal Error - Unexpected compiler direction: %v", direction)
	}

	idlfileWithoutExtension := strings.Replace(idlfile, filepath.Ext(idlfile), "", -1)
	newfilepath := outPath + string(os.PathSeparator) + idlfileWithoutExtension + outputPostfix
	fmt.Printf("Writing code to %v\n", newfilepath)

	err = ioutil.WriteFile(newfilepath, []byte(code), 0660)
	if err != nil{
		return fmt.Errorf("Failed to write %v. Error: %v", newfilepath, err)
	}

    return nil
}
//--------------------------------------------------------------------
//export compile_to_guest
func compile_to_guest(idl_path *C.char, idl_path_length C.uint,
					  output_path *C.char, output_path_length C.uint,
					  out_err **C.char, out_err_len *C.uint){

	idlPath := C.GoStringN(idl_path, C.int(idl_path_length))
	outPath := C.GoStringN(output_path, C.int(output_path_length))

	err := compile_idl(idlPath, outPath, TO_GUEST)

	if err != nil{
		*out_err = C.CString(err.Error())
		*out_err_len = C.uint(len(err.Error()))
		return
	}
}
//--------------------------------------------------------------------
//export compile_from_host
func compile_from_host(idl_path *C.char, idl_path_length C.uint,
	output_path *C.char, output_path_length C.uint,
	out_err **C.char, out_err_len *C.uint){


	idlPath := C.GoStringN(idl_path, C.int(idl_path_length))
	outPath := C.GoStringN(output_path, C.int(output_path_length))

	err := compile_idl(idlPath, outPath, FROM_HOST)

	if err != nil{
		*out_err = C.CString(err.Error())
		*out_err_len = C.uint(len(err.Error()))
		return
	}

}
//--------------------------------------------------------------------
//export compile_serialization
func compile_serialization(idl_path *C.char, idl_path_length C.uint,
	output_path *C.char, output_path_length C.uint,
	out_err **C.char, out_err_len *C.uint){

	idlPath := C.GoStringN(idl_path, C.int(idl_path_length))
	outPath := C.GoStringN(output_path, C.int(output_path_length))

	// compile given IDL to Python
	protoc := exec.Command("protoc", "--python_out="+outPath, idlPath)
	fmt.Printf("%v\n", strings.Join(protoc.Args, " "))
	output, err := protoc.CombinedOutput()

	if err != nil{
		msg := fmt.Sprintf("Failed to compile %v to Protobuf serialization code using \"protoc\". Exit code: %v.\nOutput: %v", idlPath, err, string(output))
		*out_err = C.CString(msg)
		*out_err_len = C.uint(len(msg))
		return
	}

}
//--------------------------------------------------------------------
func main(){
}
//--------------------------------------------------------------------
