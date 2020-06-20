package main

//--------------------------------------------------------------------
type Compiler struct{
	proto string
}
//--------------------------------------------------------------------
func NewPythonCompiler(proto string) *Compiler {
	return &Compiler{proto: proto}
}
//--------------------------------------------------------------------
func (this *Compiler) CompileGuest() string{

	// parse proto file

	// generate guest code
	return ""
}
//--------------------------------------------------------------------