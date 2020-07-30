package main

//--------------------------------------------------------------------
func ProtoTypeToGoType(prototype string) string{

	switch prototype {

		case "double": return "float64"
		case "float": return "float32"

		case "fixed32": fallthrough
		case "int32": fallthrough
		case "sint32": fallthrough
		case "sfixed32":
			return "int32"

		case "sfixed64": fallthrough
		case "fixed64": fallthrough
		case "sint64": fallthrough
		case "int64": return "int64"

		case "uint32": return "uint32"

		case "uint64": return "uint64"

		case "bool": return "bool"
		case "string": return "string"
		case "bytes": return "[]byte"

	}

	return prototype
}
//--------------------------------------------------------------------