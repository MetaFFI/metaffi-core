package main

import (
	"fmt"
	"strings"
)

//--------------------------------------------------------------------
func ProtoTypeToGoType(prototype string, isArray bool) (typeStr string, isComplex bool){

	isComplex = false

	switch prototype {

		case "double": typeStr = "float64"
		case "float": typeStr = "float32"

		case "fixed32": fallthrough
		case "int32": fallthrough
		case "sint32": fallthrough
		case "sfixed32":
			typeStr = "int32"

		case "sfixed64": fallthrough
		case "fixed64": fallthrough
		case "sint64": fallthrough
		case "int64":
			typeStr = "int64"

		case "uint32": typeStr = "uint32"

		case "uint64": typeStr = "uint64"

		case "bool": typeStr = "bool"
		case "string": typeStr = "string"
		case "bytes": typeStr = "[]byte"

		default:
			typeStr = "*"+UnixNotationToCamelHumps(prototype)
			isComplex = true
	}

	if isArray{
		typeStr = fmt.Sprintf("[]%v", typeStr)
	}

	return
}
//--------------------------------------------------------------------
func UnixNotationToCamelHumps(txt string)string{
	txt = strings.ReplaceAll(txt, "_", " ")
	txt = strings.Title(txt)
	txt = strings.ReplaceAll(txt, " ", "")

	return txt
}
//--------------------------------------------------------------------