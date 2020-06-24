package main

import (
	"fmt"
)

//--------------------------------------------------------------------
type Compiler struct{
	parser *ProtoParser
}
//--------------------------------------------------------------------
func NewCompiler(proto string, protoFilename string) (*Compiler, error) {

	parser, err := NewProtoParser(proto, protoFilename)
	if err != nil{
		return nil, fmt.Errorf("Failed to create proto pparser. Error: %v", err)
	}

	return &Compiler{parser: parser}, nil
}
//--------------------------------------------------------------------
// @protobufFileName - The name of the protobuf python generated
func (this *Compiler) CompileGuest() (string, error){

	compilerParams, err := NewGuestTemplateParameters(this.parser.GetProtoFileName())
	if err != nil{
		return "", err
	}

	modules, err := this.parser.GetModules()
	if err != nil{
		return "", err
	}


	for _, m := range modules{
		compilerParams.AddModule(m)
	}

	// generate guest code
	return compilerParams.Generate()
}
//--------------------------------------------------------------------
