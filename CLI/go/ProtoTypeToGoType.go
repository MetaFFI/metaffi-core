package main

//--------------------------------------------------------------------
func ProtoTypeToPythonType(prototype string) string{

	switch prototype {

		case "double": return "float"
		case "float": return "float"

		case "int32": fallthrough
		case "sint32": fallthrough
		case "sfixed32":
			return "int"

		case "int64": fallthrough
		case "uint32": fallthrough
		case "uint64": fallthrough
		case "sint64": fallthrough
		case "fixed32": fallthrough
		case "fixed64": fallthrough
		case "sfixed64":
			return "int"

		case "bool": return "bool"
		case "string": return "str"
		case "bytes": return "bytes"

	}

	return prototype
}
//--------------------------------------------------------------------