#include <args_helpers.h>
#include <cstring>
#include <cstdio>

//--------------------------------------------------------------------
int8_t is_arg_overflow(uint64_t* size_left, int size)
{
	if((*size_left - size) < 0){
		return 1;
	}
	else
	{
		*size_left -= size;
		return 0;
	}
}
//--------------------------------------------------------------------
int get_type(void** data_array, int index, openffi_type* out_type)
{
	*out_type = *(uint64_t*)data_array[index];
	return index + 1;
}
//--------------------------------------------------------------------
void** alloc_args_buffer(int size)
{
	if(size <= 0){
		return NULL;
	}
	
	return (void**)malloc(size*sizeof(void*));
}
//--------------------------------------------------------------------
get_arg_type_str(openffi_string);
get_arg_type_str(openffi_string8);
get_arg_type_str(openffi_string16);
get_arg_type_str(openffi_string32);
//--------------------------------------------------------------------
int get_arg_pointer_type(void** data_array, int index, void** out_res)
{
	*out_res = data_array[index];
	return index + 1;
}
//--------------------------------------------------------------------
get_arg_type(openffi_float64);
get_arg_type(openffi_float32);
get_arg_type(openffi_int64);
get_arg_type(openffi_int32);
get_arg_type(openffi_int16);
get_arg_type(openffi_int8);
get_arg_type(openffi_uint64);
get_arg_type(openffi_uint32);
get_arg_type(openffi_uint16);
get_arg_type(openffi_uint8);
get_arg_type(openffi_size);
get_arg_type(openffi_bool);
//--------------------------------------------------------------------
get_arg_type_str_array(openffi_string);
get_arg_type_str_array(openffi_string8);
get_arg_type_str_array(openffi_string16);
get_arg_type_str_array(openffi_string32);
//--------------------------------------------------------------------
get_arg_type_array(openffi_float64);
get_arg_type_array(openffi_float32);
get_arg_type_array(openffi_int64);
get_arg_type_array(openffi_int32);
get_arg_type_array(openffi_int16);
get_arg_type_array(openffi_int8);
get_arg_type_array(openffi_uint64);
get_arg_type_array(openffi_uint32);
get_arg_type_array(openffi_uint16);
get_arg_type_array(openffi_uint8);
get_arg_type_array(openffi_size);
get_arg_type_array(openffi_bool);
//--------------------------------------------------------------------
int set_arg(void** data_array, int index, void* val)
{
	data_array[index] = val;
	return index + 1;
}
//--------------------------------------------------------------------
set_arg_openffi_str(openffi_string);
set_arg_openffi_str(openffi_string8);
set_arg_openffi_str(openffi_string16);
set_arg_openffi_str(openffi_string32);
//--------------------------------------------------------------------
set_arg_type(openffi_float64);
set_arg_type(openffi_float32);
set_arg_type(openffi_int64);
set_arg_type(openffi_int32);
set_arg_type(openffi_int16);
set_arg_type(openffi_int8);
set_arg_type(openffi_uint64);
set_arg_type(openffi_uint32);
set_arg_type(openffi_uint16);
set_arg_type(openffi_uint8);
set_arg_type(openffi_size);
set_arg_type(openffi_bool);
//--------------------------------------------------------------------
set_arg_openffi_str_array(openffi_string);
set_arg_openffi_str_array(openffi_string8);
set_arg_openffi_str_array(openffi_string16);
set_arg_openffi_str_array(openffi_string32);
//--------------------------------------------------------------------
set_arg_type_array(openffi_float64);
set_arg_type_array(openffi_float32);
set_arg_type_array(openffi_int64);
set_arg_type_array(openffi_int32);
set_arg_type_array(openffi_int16);
set_arg_type_array(openffi_int8);
set_arg_type_array(openffi_uint64);
set_arg_type_array(openffi_uint32);
set_arg_type_array(openffi_uint16);
set_arg_type_array(openffi_uint8);
set_arg_type_array(openffi_size);
set_arg_type_array(openffi_bool);
//--------------------------------------------------------------------
const char* get_openffi_string_element(int index, openffi_string* str, const openffi_size* sizes, openffi_size* out_size)
{
	const char* res = str[index];
	*out_size = sizes[index];
	return res;
}
//--------------------------------------------------------------------
get_numeric_element(openffi_float64);
get_numeric_element(openffi_float32);
get_numeric_element(openffi_int64);
get_numeric_element(openffi_int32);
get_numeric_element(openffi_int16);
get_numeric_element(openffi_int8);
get_numeric_element(openffi_uint64);
get_numeric_element(openffi_uint32);
get_numeric_element(openffi_uint16);
get_numeric_element(openffi_uint8);
get_numeric_element(openffi_size);
//--------------------------------------------------------------------
void set_openffi_string_element(int index, openffi_string* str_array, openffi_size* sizes_array, const openffi_string str, openffi_size str_size)
{
	str_array[index] = str;
	sizes_array[index] = str_size;
}
//--------------------------------------------------------------------
set_numeric_element(openffi_float64);
set_numeric_element(openffi_float32);
set_numeric_element(openffi_int64);
set_numeric_element(openffi_int32);
set_numeric_element(openffi_int16);
set_numeric_element(openffi_int8);
set_numeric_element(openffi_uint64);
set_numeric_element(openffi_uint32);
set_numeric_element(openffi_uint16);
set_numeric_element(openffi_uint8);
set_numeric_element(openffi_size);

//--------------------------------------------------------------------

alloc_str_on_heap(openffi_string, char);
alloc_str_on_heap(openffi_string8, char);
alloc_str_on_heap(openffi_string16, wchar_t);
alloc_str_on_heap(openffi_string32, wchar_t);

alloc_numeric_on_heap(openffi_float64);
alloc_numeric_on_heap(openffi_float32);
alloc_numeric_on_heap(openffi_int64);
alloc_numeric_on_heap(openffi_int32);
alloc_numeric_on_heap(openffi_int16);
alloc_numeric_on_heap(openffi_int8);
alloc_numeric_on_heap(openffi_uint64);
alloc_numeric_on_heap(openffi_uint32);
alloc_numeric_on_heap(openffi_uint16);
alloc_numeric_on_heap(openffi_uint8);
alloc_numeric_on_heap(openffi_size);
alloc_numeric_on_heap(openffi_bool);
alloc_numeric_on_heap(openffi_type);

//--------------------------------------------------------------------
