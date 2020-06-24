package main

import (
	"encoding/json"
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

	bodyItems, err := this.getProtoBodyItems()
	if err != nil{
		return "", err
	}

	for _, item := range bodyItems{
		var bodymap map[string]json.RawMessage
		err = json.Unmarshal(item, &bodymap)
		if err != nil {
			return "", fmt.Errorf("Failed to unmarshal to proto body item: %v", err)
		}

		_, found := bodymap["MessageName"]
		if !found{ // not a message
			continue
		}

		f, err := this.parseFunction(bodymap)
		if err != nil {
			return "", fmt.Errorf("Failed to extract function information from proto file: %v", err)
		}

		compilerParams.AddFunction(f)
	}

	// generate guest code
	return compilerParams.Generate()
}
//--------------------------------------------------------------------
func (this *Compiler) parseFunction(bodymap map[string]json.RawMessage) (*GuestTemplateFunctionParameters, error){

	return nil, nil
}
//--------------------------------------------------------------------
func (this *Compiler) getProtoBodyItems() ([]json.RawMessage, error){

	protojson, err := json.MarshalIndent(this.parser, "", "\t")
	if err != nil {
		return nil, fmt.Errorf("Failed to marshal proto to JSON: %v", err)
	}

	var protomap map[string]json.RawMessage
	err = json.Unmarshal(protojson, &protomap)
	if err != nil {
		return nil, fmt.Errorf("Failed to unmarshal proto file: %v", err)
	}

	body, found := protomap["ProtoBody"]
	if !found{
		return nil, fmt.Errorf("Failed to find ProtoBody")
	}

	var bodyItems []json.RawMessage
	err = json.Unmarshal(body, &bodyItems)
	if err != nil {
		return nil, fmt.Errorf("Failed to unmarshal to proto body: %v", err)
	}

	return bodyItems, nil
}
//--------------------------------------------------------------------