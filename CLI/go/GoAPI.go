package main
import (
	"fmt"
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
)

import "C"

type compileDirection uint8

const (
	FROM_HOST compileDirection = 0
	TO_GUEST compileDirection = 1
)

func getDynamicLibSuffix() string{
	switch runtime.GOOS{
		case "windows": return ".dll"
		case "darwin": return ".dylib"
		default: // We might need to make this more specific in the future
			return ".so"
	}
}
//--------------------------------------------------------------------
func getFilesDirs(outPath string, idlfile string, direction compileDirection) (codeFullFilePath string, libFullFilePath string, codeFullPath string){

	idlfileWithoutExtension := strings.Replace(idlfile, filepath.Ext(idlfile), "", -1)

	var outputPostfix string
	switch direction {
	case FROM_HOST:
		outputPostfix = "OpenFFIHost"

	case TO_GUEST:
		outputPostfix = "OpenFFIGuest"
	}

	curDir, err := os.Getwd()
	if err != nil{
		panic(err)
	}

	// Go code to compile
	codeFullFilePath = curDir + string(os.PathSeparator) + idlfileWithoutExtension + outputPostfix + ".go"
	codeFullPath = curDir + string(os.PathSeparator) // directory with go code

	// output dynamic library path
	libFullFilePath = outPath + string(os.PathSeparator) + idlfileWithoutExtension + outputPostfix + getDynamicLibSuffix()

	return
}
//--------------------------------------------------------------------
func getDirectionString(direction compileDirection) string{
	if direction == FROM_HOST {
		return "host"
	} else {
		return "guest"
	}
}
//--------------------------------------------------------------------
func compileIDL(idlPath string, outPath string, direction compileDirection) error{

	// NOTICE - this error check if very important, as the rest of the function
	// assumes "direction" is one of these two options!
	if direction != FROM_HOST && direction != TO_GUEST{
		return fmt.Errorf("Unknown direction: %v", direction)
	}

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

    var compileFunc func()(string, error)
	switch direction {
		case FROM_HOST: compileFunc = compiler.CompileHost
		case TO_GUEST: compileFunc = compiler.CompileGuest
		default:
			return fmt.Errorf("Unknown requested call direction")
	}

	code, err := compileFunc()
	if err != nil{
		return fmt.Errorf("Failed to generate %v code. Error: %v", getDirectionString(direction), err)
	}

	// write to output path
	if _, err = os.Stat(outPath); os.IsNotExist(err) {
		err = os.Mkdir(outPath, os.ModeDir)
	}

	if err != nil{
		return fmt.Errorf("Failed to create output directory %v. Error: %v", outPath, err)
	}

	codeFullFilePath, libFullFilePath, _ := getFilesDirs(outPath, idlfile, direction)

	fmt.Printf("Writing Go %v code to %v\n", getDirectionString(direction), codeFullFilePath)

	err = ioutil.WriteFile(codeFullFilePath, []byte(code), 0660)
	if err != nil{
		return fmt.Errorf("Failed to write %v. Error: %v", codeFullFilePath, err)
	}

	if direction == TO_GUEST{ // if guest - build code to shared object

		fmt.Printf("Building %v Go runtime linker to %v\n", getDirectionString(direction), codeFullFilePath)

		if _, err := os.Stat("go.mod"); os.IsNotExist(err) { // if go.mod doesn't exist. create it.
			modInitCmd := exec.Command("go", "mod", "init", "guest")
			fmt.Printf("%v\n", strings.Join(modInitCmd.Args, " "))
			output, err := modInitCmd.CombinedOutput()
			if err != nil {
				return fmt.Errorf("Failed building %v Go runtime linker to %v. Failed creating module 'guest' with error: %v.\nOutput:\n%v", getDirectionString(direction), codeFullFilePath, err, string(output))
			}
		}

		buildCmd := exec.Command("go", "build", "-tags=guest" , "-buildmode=c-shared", "-gcflags=-shared", "-o", libFullFilePath)
		fmt.Printf("%v\n", strings.Join(buildCmd.Args, " "))
		output, err := buildCmd.CombinedOutput()
		if err != nil{
			return fmt.Errorf("Failed building %v Go runtime linker to %v. Exit with error: %v.\nOutput:\n%v", getDirectionString(direction), codeFullFilePath, err, string(output))
		}

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

	err := compileIDL(idlPath, outPath, TO_GUEST)

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

	err := compileIDL(idlPath, outPath, FROM_HOST)

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
	protoc := exec.Command("protoc", "--go_out="+outPath, idlPath)
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
