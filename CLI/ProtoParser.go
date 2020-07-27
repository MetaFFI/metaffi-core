package main

import (
	"encoding/json"
	"fmt"
	"github.com/antchfx/jsonquery"
	"github.com/yoheimuta/go-protoparser"
	"github.com/yoheimuta/go-protoparser/parser"
	"strings"
)

//--------------------------------------------------------------------
type ProtoParser struct{
	pparser            *parser.Proto
	proto              string
	jsonProto          string
	jsonProtoQueryRoot *jsonquery.Node
	protoMap           map[string]interface{}
}
//--------------------------------------------------------------------
type Module struct {
	Name string
	Functions []*FunctionData
}
//--------------------------------------------------------------------
type FunctionData struct {
	Name string
	RequestName string
	ResponseName string
	Parameters []*ParameterData
	Return []*ParameterData
}
//--------------------------------------------------------------------
type ParameterData struct{
	Name string
	Type string
	IsArray bool
}
//--------------------------------------------------------------------
func NewProtoParser(proto string, protoFilename string) (*ProtoParser, error){

    proto += "\n"
	pparser, err := protoparser.Parse(strings.NewReader(proto),
									protoparser.WithFilename(protoFilename),
									protoparser.WithBodyIncludingComments(true))

	if err != nil{
		return nil, fmt.Errorf("Failed to create proto pparser. Error: %v", err)
	}

	jsonProto, err := json.MarshalIndent(pparser, "", "\t")
	if err != nil {
		return nil, fmt.Errorf("Failed to marshal proto to JSON: %v", err)
	}

	var protoMap map[string]interface{}
	err = json.Unmarshal(jsonProto, &protoMap)
	if err != nil {
		return nil, fmt.Errorf("Failed to unmarshal proto file: %v", err)
	}

	root, err := jsonquery.Parse(strings.NewReader(string(jsonProto)))
	if err != nil {
		return nil, fmt.Errorf("Failed to parse JSON Proto to JsonQuery: %v", err)
	}

	return &ProtoParser{pparser: pparser,
		proto:              proto,
		jsonProto:          string(jsonProto),
		jsonProtoQueryRoot: root,
		protoMap:           protoMap}, nil

}
//--------------------------------------------------------------------
func (this *ProtoParser) GetProtoFileName() string{
	return this.pparser.Meta.Filename
}
//--------------------------------------------------------------------
func (this *ProtoParser) GetModules() ([]*Module, error){

	servicesNodes := jsonquery.Find(this.jsonProtoQueryRoot, "//ServiceName")

	if len(servicesNodes) == 0{
		return nil, fmt.Errorf("No modules defined!")
	}

	services := make([]*Module, 0)

	for _, serviceNode := range servicesNodes {

		module := &Module{ Name: serviceNode.InnerText() }
		functions, err := this.getFunctions(serviceNode.Parent)
		if err != nil{
			return nil, err
		}

		module.Functions = functions

		services = append(services, module)
	}

	return services, nil
}
//--------------------------------------------------------------------
func (this *ProtoParser) getFunctions(serviceNode *jsonquery.Node) ([]*FunctionData, error){

	rpcNameNodes := jsonquery.Find(serviceNode, "//RPCName")

	if rpcNameNodes == nil || len(rpcNameNodes) == 0{
		return nil, fmt.Errorf("No functions defined")
	}

	funcsData := make([]*FunctionData, 0, len(rpcNameNodes))

	var err error
	for _, rpcNameNode := range rpcNameNodes{

		fData := &FunctionData{}
		fData.Name = rpcNameNode.InnerText()

		fData.RequestName, fData.ResponseName, fData.Parameters, fData.Return, err = this.getParameters(rpcNameNode.Parent)
		if err != nil{
			return nil, err
		}

		funcsData = append(funcsData, fData)
	}

	return funcsData, nil
}
//--------------------------------------------------------------------
func (this *ProtoParser) getParameters(functionNode *jsonquery.Node) (requestName string, responseName string, parameters []*ParameterData, returns []*ParameterData, err error){

	requestMessageNode := jsonquery.Find(functionNode, "//RPCRequest/MessageType")
	responseMessageNode := jsonquery.Find(functionNode, "//RPCResponse/MessageType")

	if requestMessageNode == nil || responseMessageNode == nil || len(requestMessageNode) != 1 || len(responseMessageNode) != 1 {
		return "", "", nil, nil, fmt.Errorf("Cannot find Request message or Response message is proto file")
	}

	requestName = requestMessageNode[0].InnerText()
	parameters, err = this.getMessageAsParameters(requestName)
	if err != nil{
		return "", "", nil, nil, err
	}

	responseName = responseMessageNode[0].InnerText()
	returns, err = this.getMessageAsParameters(responseName)
	if err != nil{
		return "", "", nil, nil, err
	}

	return requestName, responseName, parameters, returns, nil
}
//--------------------------------------------------------------------
func (this *ProtoParser) getMessageAsParameters(messageName string) (parameters []*ParameterData, err error){

	// find message in proto file
	messagesNodes := jsonquery.Find(this.jsonProtoQueryRoot, "//MessageName")
	var messageNode *jsonquery.Node
	for _, curNode := range messagesNodes{
		if curNode.InnerText() == messageName{
			messageNode = curNode.Parent
			break
		}
	}
	
	if messageNode == nil{
		return nil, fmt.Errorf("Message %v was not found", messageName)
	}

	// get parameters
	parameters = make([]*ParameterData, 0)
	parametersNodes := jsonquery.Find(messageNode, "//FieldName")
	for _, paramNode := range parametersNodes{

		// get parameter data
		curParam := &ParameterData{}
		curParam.Name = paramNode.InnerText()
		curParam.Type = paramNode.Parent.SelectElement("Type").InnerText()
		curParam.IsArray = strings.ToLower(paramNode.Parent.SelectElement("IsRepeated").InnerText()) == "true"

		parameters = append(parameters, curParam)
	}

	return parameters, nil
}
//--------------------------------------------------------------------