package main

import "fmt"

//--------------------------------------------------------------------
func ProtoTypeToPythonType(prototype string, isArray bool) (typeStr string, isComplex bool){

	isComplex = false

	switch prototype {

		case "double": fallthrough
		case "float":
			typeStr = "float"

		case "int32": fallthrough
		case "sint32": fallthrough
		case "sfixed32": fallthrough
		case "int64": fallthrough
		case "uint32": fallthrough
		case "uint64": fallthrough
		case "sint64": fallthrough
		case "fixed32": fallthrough
		case "fixed64": fallthrough
		case "sfixed64":
			typeStr = "int"

		case "bool": typeStr = "bool"
		case "string": typeStr = "str"
		case "bytes": typeStr = "bytes"

		default:
			typeStr = prototype
			isComplex = true
	}

	if isArray{
		typeStr = fmt.Sprintf("List[%v]", typeStr)
	}

	return
}
//--------------------------------------------------------------------