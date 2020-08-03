package openffi_cli

import "testing"

func TestProtoParser(t *testing.T) {

	proto := `
syntax = "proto3";

// openffi_target_lang: "go"

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

	_, err = parser.GetModules()
}
