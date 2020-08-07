package main

import (
	"fmt"
	. "github.com/GreenFuze/OpenFFI/CLI/utils/go"
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

	compilerParams, err := NewTemplateParameters(this.parser.GetProtoFileName(), PROTOBUF_GO_SUFFIX, this.parser.TargetLanguage, ProtoTypeToGoType, UnixNotationToCamelHumps)
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
	return compilerParams.Generate("guest", GuestTemplate)
}
//--------------------------------------------------------------------
// @protobufFileName - The name of the protobuf python generated
func (this *Compiler) CompileHost() (string, error){

	compilerParams, err := NewTemplateParameters(this.parser.GetProtoFileName(), PROTOBUF_GO_SUFFIX, this.parser.TargetLanguage, ProtoTypeToGoType, UnixNotationToCamelHumps)
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

	// generate host code
	return compilerParams.Generate("host", HostTemplate)
}
//--------------------------------------------------------------------