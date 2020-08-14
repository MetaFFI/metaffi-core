package main

import (
	. "github.com/GreenFuze/OpenFFI/CLI/utils/go"
	"testing"
)

//--------------------------------------------------------------------
func TestProtoParserToJSON(t *testing.T){

	proto := `
syntax = "proto3";

// openffi_target_lang:    "go"

//--------------------------------------------------------------------
service Service1 { // openffi_module: ""
    rpc F1 (Params1) returns (Return1); 
	rpc F2 (Params2) returns (Return2);
}
//--------------------------------------------------------------------
// param1 comment
message Params1 {
    repeated string p1 = 1; // openffi_param: "byval"
}
//--------------------------------------------------------------------
message Return1 {
    string r1 = 1;
}
//--------------------------------------------------------------------
message Params2 {
    string p21 = 1;
	Params1 p22 = 2; 
}
//--------------------------------------------------------------------
message Return2 {
    string r21 = 1;
	Return1 r22 = 2;
}
//--------------------------------------------------------------------
`

	parser, err := NewProtoParser(proto, "test.proto")
	if err != nil{
		t.Fatalf("Failed to create proto pparser. Error: %v", err)
	}

	mods, err := parser.GetModules()
	if err != nil{
		t.Fatalf("Failed to get modules: %v", err)
	}

	if len(mods) != 1{
		t.Fatalf("Expected 1 module. Found: %v", len(mods))
	}

	// Test Service1
	if mods[0].Name != ""{
		t.Fatalf("Expected service name to be empty. Found: %v", mods[0].Name)
	}

	if len(mods[0].Functions) != 2{
		t.Fatalf("Expected Service1 to have 2 functions Found: %v", mods[0].Functions)
	}

	// Test Function F1
	if mods[0].Functions[0].Name != "F1"{
		t.Fatalf("Expected F1 as function name. Got: %v", mods[0].Functions[0].Name)
	}

	if mods[0].Functions[0].RequestName != "Params1"{
		t.Fatalf("Expected Params1 as function name. Got: %v", mods[0].Functions[0].RequestName)
	}

	if mods[0].Functions[0].ResponseName != "Return1"{
		t.Fatalf("Expected Return1 as function name. Got: %v", mods[0].Functions[0].ResponseName)
	}

	if len(mods[0].Functions[0].Parameters) != 1{
		t.Fatalf("F1 should have 1 parameter. Got: %v", len(mods[0].Functions[0].Parameters))
	}

	if len(mods[0].Functions[0].Return) != 1{
		t.Fatalf("F1 should have 1 return value. Got: %v", len(mods[0].Functions[0].Return))
	}

	expectedParameterF1 := ParameterData{
		Name:          "p1",
		Type:          "string",
		IsArray:       true,
		PassParam:	   mods[0].Functions[0].Parameters[0].PassParam, // to ignore this param in the comparison
	}
	if *mods[0].Functions[0].Parameters[0] != expectedParameterF1{
		t.Fatalf("F1 parameters parsed incorrectly. Got: %v", *mods[0].Functions[0].Parameters[0])
	}

	expectedReturnF1 := ParameterData{
		Name:          "r1",
		Type:          "string",
		IsArray:       false,
		PassParam:	   mods[0].Functions[0].Return[0].PassParam, // to ignore this param in the comparison
	}
	if *mods[0].Functions[0].Return[0] != expectedReturnF1{
		t.Fatalf("R1 Return parsed incorrectly. Got: %v", *mods[0].Functions[0].Return[0])
	}


	// Test function F2
	if mods[0].Functions[1].Name != "F2"{
		t.Fatalf("Expected F2 as function name. Got: %v", mods[0].Functions[1].Name)
	}

	if mods[0].Functions[1].RequestName != "Params2"{
		t.Fatalf("Expected Params2 as function name. Got: %v", mods[0].Functions[0].RequestName)
	}

	if mods[0].Functions[1].ResponseName != "Return2"{
		t.Fatalf("Expected Return2 as function name. Got: %v", mods[0].Functions[0].ResponseName)
	}

	if len(mods[0].Functions[1].Parameters) != 2{
		t.Fatalf("F2 should have 2 parameter. Got: %v", len(mods[0].Functions[1].Parameters))
	}

	if len(mods[0].Functions[1].Return) != 2{
		t.Fatalf("F2 should have 2 return value. Got: %v", len(mods[0].Functions[1].Return))
	}

	expectedParameterFP21 := ParameterData{
		Name:          "p21",
		Type:          "string",
		IsArray:       false,
		PassParam:	   mods[0].Functions[1].Parameters[0].PassParam, // to ignore this param in the comparison
	}
	if *mods[0].Functions[1].Parameters[0] != expectedParameterFP21{
		t.Fatalf("F2 parameter p21 parsed incorrectly. Got: %v", *mods[0].Functions[1].Parameters[0])
	}

	expectedParameterFP22 := ParameterData{
		Name:          "p22",
		Type:          "Params1",
		IsArray:       false,
		PassParam:	   mods[0].Functions[1].Parameters[1].PassParam, // to ignore this param in the comparison
	}
	if *mods[0].Functions[1].Parameters[1] != expectedParameterFP22{
		t.Fatalf("F2 parameters p22 parsed incorrectly. Got: %v", *mods[0].Functions[1].Parameters[1])
	}

	expectedReturnFR21 := ParameterData{
		Name:          "r21",
		Type:          "string",
		IsArray:       false,
		PassParam:	   mods[0].Functions[1].Return[0].PassParam, // to ignore this param in the comparison
	}
	if *mods[0].Functions[1].Return[0] != expectedReturnFR21{
		t.Fatalf("F2 Return r21 parsed incorrectly. Got: %v", *mods[0].Functions[1].Return[0])
	}

	expectedReturnFR22 := ParameterData{
		Name:          "r22",
		Type:          "Return1",
		IsArray:       false,
		PassParam:	   mods[0].Functions[1].Return[1].PassParam, // to ignore this param in the comparison
	}
	if *mods[0].Functions[1].Return[1] != expectedReturnFR22{
		t.Fatalf("F2 Return r22 parsed incorrectly. Got: %v", *mods[0].Functions[1].Return[1])
	}

	testGuestTemplate(mods, parser.TargetLanguage, t)
	testHostTemplate(mods, parser.TargetLanguage, t)
}
//--------------------------------------------------------------------
func testGuestTemplate(mods []*Module, targetLang string, t *testing.T){

	htp, err := NewTemplateParameters("test.proto", PROTOBUF_GO_SUFFIX, targetLang, ProtoTypeToGoType, UnixNotationToCamelHumps)
	if err != nil{
		t.Fatal(err)
	}

	for _, m := range mods{
		htp.AddModule(m)
	}

	_, err = htp.Generate("guest", GuestTemplate)
	if err != nil{
		t.Fatalf("Failed to generate guest code: %v", err)
	}

	//fmt.Println(guestCode)
}
//--------------------------------------------------------------------
func testHostTemplate(mods []*Module, targetLang string, t *testing.T){

	htp, err := NewTemplateParameters("test.proto", PROTOBUF_GO_SUFFIX, targetLang, ProtoTypeToGoType, UnixNotationToCamelHumps)
	if err != nil{
		t.Fatal(err)
	}

	for _, m := range mods{
		htp.AddModule(m)
	}

	_, err = htp.Generate("host", HostTemplate)
	if err != nil{
		t.Fatalf("Failed to generate guest code: %v", err)
	}

	//fmt.Println(hostCode)

}
//--------------------------------------------------------------------