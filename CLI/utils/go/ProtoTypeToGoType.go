package openffi_cli

//--------------------------------------------------------------------
func ProtoTypeToGoType(prototype string) (res string, isComplexType bool){

	switch prototype {

		case "double": return "float64", false
		case "float": return "float32", false

		case "fixed32": fallthrough
		case "int32": fallthrough
		case "sint32": fallthrough
		case "sfixed32":
			return "int32", false

		case "sfixed64": fallthrough
		case "fixed64": fallthrough
		case "sint64": fallthrough
		case "int64": return "int64", false

		case "uint32": return "uint32", false

		case "uint64": return "uint64", false

		case "bool": return "bool", false
		case "string": return "string", false
		case "bytes": return "[]byte", false

	}

	return prototype, true
}
//--------------------------------------------------------------------