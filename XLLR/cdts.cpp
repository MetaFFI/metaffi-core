#include <cdts.h>
#include <cstring>

/************************************************
*   Allocations
*************************************************/


cdt* alloc_cdts_buffer(openffi_size cdt_count)
{
	if(cdt_count <= 0){
		return nullptr;
	}
	
	return (cdt*)calloc(sizeof(cdt), cdt_count);
	//return (cdt*)alloca(sizeof(cdt)*cdt_count);
}

#define alloc_numeric_on_heap_impl(type) \
type* alloc_##type##_on_heap(type val)\
{\
    type* res = (type*)malloc(sizeof(type));\
    *res = val;\
    return res;\
}\
type* alloc_##type##_array_on_heap(openffi_size length)\
{\
	return (type*)calloc(length, sizeof(type));\
}

#define alloc_string_on_heap_impl(type, char_t) \
type alloc_##type##_on_heap(type str, openffi_size str_size)\
{\
	type res = (type)malloc(sizeof(char_t)*str_size+sizeof(char_t));\
    memcpy(res, str, str_size*sizeof(char_t)); \
    res[str_size] = 0;\
    return res;\
} \
type* alloc_##type##_array_on_heap(openffi_size length)\
{\
	return (type*)calloc(length, sizeof(type));\
}

alloc_numeric_on_heap_impl(openffi_float64);
alloc_numeric_on_heap_impl(openffi_float32);
alloc_numeric_on_heap_impl(openffi_int64);
alloc_numeric_on_heap_impl(openffi_int32);
alloc_numeric_on_heap_impl(openffi_int16);
alloc_numeric_on_heap_impl(openffi_int8);
alloc_numeric_on_heap_impl(openffi_uint64);
alloc_numeric_on_heap_impl(openffi_uint32);
alloc_numeric_on_heap_impl(openffi_uint16);
alloc_numeric_on_heap_impl(openffi_uint8);
alloc_numeric_on_heap_impl(openffi_size);
alloc_numeric_on_heap_impl(openffi_bool);
alloc_numeric_on_heap_impl(openffi_type);
alloc_numeric_on_heap_impl(openffi_handle);
alloc_string_on_heap_impl(openffi_string8, char);
alloc_string_on_heap_impl(openffi_string16, char16_t);
alloc_string_on_heap_impl(openffi_string32, char32_t);

//====================================================================

/************************************************
*   Getters
*************************************************/


openffi_type get_type(cdt* data_array, int index)
{
	return ((cdt*)&data_array[index])->type; // first field in each CDT is openffi_type!
}

cdt* get_cdt(cdt* data_array, int index)
{
	return (cdt*)&data_array[index];
}


//====================================================================

/************************************************
*   Array Elements Getters
*************************************************/

#define get_numeric_element_impl(type) \
type get_##type##_element(type* arr, int index)\
{\
    return arr[index];\
}

#define get_string_element_impl(type) \
type get_##type##_element(type* arr, int index, const openffi_size* sizes, openffi_size* out_size)\
{\
    type res = arr[index];\
	*out_size = sizes[index];\
	return res;\
}

get_numeric_element_impl(openffi_float64);
get_numeric_element_impl(openffi_float32);
get_numeric_element_impl(openffi_int64);
get_numeric_element_impl(openffi_int32);
get_numeric_element_impl(openffi_int16);
get_numeric_element_impl(openffi_int8);
get_numeric_element_impl(openffi_uint64);
get_numeric_element_impl(openffi_uint32);
get_numeric_element_impl(openffi_uint16);
get_numeric_element_impl(openffi_uint8);
get_numeric_element_impl(openffi_size);
get_numeric_element_impl(openffi_handle);
get_string_element_impl(openffi_string8);
get_string_element_impl(openffi_string16);
get_string_element_impl(openffi_string32);


/************************************************
*   Array Elements Setters
*************************************************/

#define set_numeric_element_impl(type) \
void set_##type##_element(type* arr, int index, type val)\
{\
    arr[index] = val;\
}

#define set_string_element_impl(type) \
void set_##type##_element(type* arr, openffi_size* sizes_array, int index, type val, openffi_size str_size)\
{\
    arr[index] = val;\
    sizes_array[index] = str_size;\
}

set_numeric_element_impl(openffi_float64);
set_numeric_element_impl(openffi_float32);
set_numeric_element_impl(openffi_int64);
set_numeric_element_impl(openffi_int32);
set_numeric_element_impl(openffi_int16);
set_numeric_element_impl(openffi_int8);
set_numeric_element_impl(openffi_uint64);
set_numeric_element_impl(openffi_uint32);
set_numeric_element_impl(openffi_uint16);
set_numeric_element_impl(openffi_uint8);
set_numeric_element_impl(openffi_size);
set_numeric_element_impl(openffi_handle);
set_string_element_impl(openffi_string8);
set_string_element_impl(openffi_string16);
set_string_element_impl(openffi_string32);


//--------------------------------------------------------------------
