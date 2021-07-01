#include <cdt.h>
#include <cstring>
#include <runtime/cdt_structs.h>

/************************************************
*   Allocations
*************************************************/


void** alloc_cdts_buffer(int cdt_count)
{
	if(cdt_count <= 0){
		return NULL;
	}
	
	return (void**)malloc(cdt_count*sizeof(void*));
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
alloc_string_on_heap_impl(openffi_string, char);
alloc_string_on_heap_impl(openffi_string8, char);
alloc_string_on_heap_impl(openffi_string16, wchar_t);
alloc_string_on_heap_impl(openffi_string32, wchar_t);

//====================================================================

/************************************************
*   Getters
*************************************************/


openffi_type get_type(void** data_array, int index)
{
	return *(openffi_type*)data_array[index]; // first field in each CDT is openffi_type!
}

// numeric getter
#define get_cdt_numeric_type_impl(type)\
int get_cdt_##type(void** data_array, int index, type* out_res) \
{ \
    *out_res = *(((cdt_##type*)data_array[index]).val); \
	return index+1; \
}

// string getter
#define get_cdt_string_type_impl(type)\
int get_cdt_##type(void** data_array, int index, type* out_res, openffi_size* length) \
{ \
    *length = *((openffi_size*)data_array[index+1]);\
    *out_res = ((type)data_array[index]); \
	return index + type##_type_size; \
}

// numeric array getter
#define get_cdt_numeric_type_array_impl(type)\
int get_cdt_##type##_array(void** data_array, int index, type** out_res, openffi_size** dimensions_lengths, openffi_size* dimensions) \
{ \
    *out_res = ((type*)data_array[index]); \
    *dimensions_lengths = ((openffi_size*)data_array[index+1]); \
    *dimensions = *((openffi_size*)data_array[index+2]); \
	return index + type##_type_size + openffi_array_type_size;\
}

// string array getter
#define get_cdt_string_type_array_impl(type)\
int get_cdt_##type##_array(void** data_array, int index, type** array, openffi_size** sizes_array, openffi_size** dimensions_lengths, openffi_size* dimensions) \
{  \
    *array = ((type*)data_array[index]); \
    *sizes_array = (openffi_size*)data_array[index+1]; \
    *dimensions_lengths = ((openffi_size*)data_array[index+2]); \
    *dimensions = *((openffi_size*)data_array[index+3]); \
	return index + type##_type_size + openffi_array_type_size; \
}

get_cdt_numeric_type_impl(openffi_float64);
get_cdt_numeric_type_impl(openffi_float32);
get_cdt_numeric_type_impl(openffi_int64);
get_cdt_numeric_type_impl(openffi_int32);
get_cdt_numeric_type_impl(openffi_int16);
get_cdt_numeric_type_impl(openffi_int8);
get_cdt_numeric_type_impl(openffi_uint64);
get_cdt_numeric_type_impl(openffi_uint32);
get_cdt_numeric_type_impl(openffi_uint16);
get_cdt_numeric_type_impl(openffi_uint8);
get_cdt_numeric_type_impl(openffi_size);
get_cdt_numeric_type_impl(openffi_bool);
get_cdt_string_type_impl(openffi_string);
get_cdt_string_type_impl(openffi_string8);
get_cdt_string_type_impl(openffi_string16);
get_cdt_string_type_impl(openffi_string32);

get_cdt_numeric_type_array_impl(openffi_float64);
get_cdt_numeric_type_array_impl(openffi_float32);
get_cdt_numeric_type_array_impl(openffi_int64);
get_cdt_numeric_type_array_impl(openffi_int32);
get_cdt_numeric_type_array_impl(openffi_int16);
get_cdt_numeric_type_array_impl(openffi_int8);
get_cdt_numeric_type_array_impl(openffi_uint64);
get_cdt_numeric_type_array_impl(openffi_uint32);
get_cdt_numeric_type_array_impl(openffi_uint16);
get_cdt_numeric_type_array_impl(openffi_uint8);
get_cdt_numeric_type_array_impl(openffi_size);
get_cdt_numeric_type_array_impl(openffi_bool);
get_cdt_string_type_array_impl(openffi_string);
get_cdt_string_type_array_impl(openffi_string8);
get_cdt_string_type_array_impl(openffi_string16);
get_cdt_string_type_array_impl(openffi_string32);


//====================================================================

/************************************************
*   Setters
*************************************************/


// internal function
int set_cdt(void** data_array, int index, void* val)
{
	data_array[index] = val;
	return index + 1;
}

#define set_cdt_string_type_impl(type)\
int set_cdt_##type(void** data_array, int index, openffi_string val, openffi_size* length)\
{\
    openffi_type* ptype = (openffi_type*)malloc( sizeof(openffi_type) ); \
    *ptype = type##_type; \
    set_cdt(data_array, index, (void*)ptype); \
    set_cdt(data_array, index+1, (void*)val); \
    set_cdt(data_array, index+2, (void*)length); \
	return index + openffi_type_type_size + type##_type_size;\
}

#define set_cdt_numeric_type_impl(type) \
int set_cdt_##type(void** data_array, int index, type* val) \
{ \
    openffi_type* ptype = (openffi_type*)malloc( sizeof(openffi_type) );   \
    *ptype = type##_type; \
    set_cdt(data_array, index, (void*)ptype); \
    set_cdt(data_array, index+1, (void*)val); \
    return index + openffi_type_type_size + type##_type_size; \
}

#define set_cdt_string_type_array_impl(type)\
int set_cdt_##type##_array(void** data_array, int index, type* array, openffi_size* string_sizes, openffi_size* dimensions_lengths, openffi_size* dimensions)\
{\
    openffi_type* ptype = (openffi_type*)malloc( sizeof(openffi_type) ); \
    *ptype = type##_type | openffi_array_type; \
	set_cdt(data_array, index, (void*)ptype); \
    set_cdt(data_array, index+1, (void*)array); \
    set_cdt(data_array, index+2, (void*)string_sizes); \
    set_cdt(data_array, index+3, (void*)dimensions_lengths); \
    set_cdt(data_array, index+4, (void*)dimensions); \
    return index + openffi_type_type_size + type##_type_size + openffi_array_type_size; \
}

#define set_cdt_numeric_type_array_impl(type) \
int set_cdt_##type##_array(void** data_array, int index, type* array, openffi_size* dimensions_lengths, openffi_size* dimensions) \
{ \
    data_array[index] = (openffi_type*)malloc( sizeof(openffi_type) ); \
    *((openffi_type*)data_array[index]) = type##_type | openffi_array_type; \
    data_array[index+1] = (void*)array;\
    data_array[index+2] = (void*)dimensions_lengths;\
    data_array[index+3] = dimensions;\
    return index + openffi_type_type_size + openffi_array_type_size + type##_type_size; \
}

set_cdt_numeric_type_impl(openffi_float64);
set_cdt_numeric_type_impl(openffi_float32);
set_cdt_numeric_type_impl(openffi_int64);
set_cdt_numeric_type_impl(openffi_int32);
set_cdt_numeric_type_impl(openffi_int16);
set_cdt_numeric_type_impl(openffi_int8);
set_cdt_numeric_type_impl(openffi_uint64);
set_cdt_numeric_type_impl(openffi_uint32);
set_cdt_numeric_type_impl(openffi_uint16);
set_cdt_numeric_type_impl(openffi_uint8);
set_cdt_numeric_type_impl(openffi_size);
set_cdt_numeric_type_impl(openffi_bool);
set_cdt_string_type_impl(openffi_string);
set_cdt_string_type_impl(openffi_string8);
set_cdt_string_type_impl(openffi_string16);
set_cdt_string_type_impl(openffi_string32);

set_cdt_numeric_type_array_impl(openffi_float64);
set_cdt_numeric_type_array_impl(openffi_float32);
set_cdt_numeric_type_array_impl(openffi_int64);
set_cdt_numeric_type_array_impl(openffi_int32);
set_cdt_numeric_type_array_impl(openffi_int16);
set_cdt_numeric_type_array_impl(openffi_int8);
set_cdt_numeric_type_array_impl(openffi_uint64);
set_cdt_numeric_type_array_impl(openffi_uint32);
set_cdt_numeric_type_array_impl(openffi_uint16);
set_cdt_numeric_type_array_impl(openffi_uint8);
set_cdt_numeric_type_array_impl(openffi_size);
set_cdt_numeric_type_array_impl(openffi_bool);
set_cdt_string_type_array_impl(openffi_string);
set_cdt_string_type_array_impl(openffi_string8);
set_cdt_string_type_array_impl(openffi_string16);
set_cdt_string_type_array_impl(openffi_string32);

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
get_string_element_impl(openffi_string);
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
set_string_element_impl(openffi_string);
set_string_element_impl(openffi_string8);
set_string_element_impl(openffi_string16);
set_string_element_impl(openffi_string32);


//--------------------------------------------------------------------
