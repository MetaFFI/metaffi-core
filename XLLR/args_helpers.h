#pragma once
#include <cstdlib>
#include <cstdint>
#include <runtime/openffi_primitives.h>


#ifndef SKIP_XLLR_API_EXTERN
extern "C"
{
#endif

int8_t is_arg_overflow(uint64_t* size_left, int size);

int get_type(void** data_array, int index, uint64_t* out_type);

void** alloc_args_buffer(int size);

#define get_arg_type_decl(type) \
int get_arg_##type(void** data_array, int index, type* out_res);

#define get_arg_type(type)\
int get_arg_##type(void** data_array, int index, type* out_res) \
{ \
    *out_res = *((type*)data_array[index]); \
	return index+1; \
}

#define get_arg_type_str_decl(type)\
int get_arg_##type(void** data_array, int index, type* out_res, openffi_size* length);

#define get_arg_type_str(type)\
int get_arg_##type(void** data_array, int index, type* out_res, openffi_size* length) \
{ \
    *length = *((openffi_size*)data_array[index+1]);\
    *out_res = ((type)data_array[index]); \
	return index + 2; \
}

get_arg_type_str_decl(openffi_string);
get_arg_type_str_decl(openffi_string8);
get_arg_type_str_decl(openffi_string16);
get_arg_type_str_decl(openffi_string32);

int get_arg_pointer_type(void** data_array, int index, void** out_res);

get_arg_type_decl(openffi_float64);
get_arg_type_decl(openffi_float32);
get_arg_type_decl(openffi_int64);
get_arg_type_decl(openffi_int32);
get_arg_type_decl(openffi_int16);
get_arg_type_decl(openffi_int8);
get_arg_type_decl(openffi_uint64);
get_arg_type_decl(openffi_uint32);
get_arg_type_decl(openffi_uint16);
get_arg_type_decl(openffi_uint8);
get_arg_type_decl(openffi_size);
get_arg_type_decl(openffi_bool);

#define get_arg_type_array_decl(type) \
int get_arg_##type##_array(void** data_array, int index, type** out_res, openffi_size** dimensions, openffi_size* dimensions_length);

#define get_arg_type_array(type)\
int get_arg_##type##_array(void** data_array, int index, type** out_res, openffi_size** dimensions, openffi_size* dimensions_length) \
{ \
    *out_res = ((type*)data_array[index]); \
    *dimensions = ((openffi_size*)data_array[index+1]); \
    *dimensions_length = *((openffi_size*)data_array[index+2]); \
	return index + 3;\
}

#define get_arg_type_str_array_decl(type)\
int get_arg_##type##_array(void** data_array, int index, type** array, openffi_size** sizes_array, openffi_size** dimensions, openffi_size* dimensions_length);

#define get_arg_type_str_array(type)\
int get_arg_##type##_array(void** data_array, int index, type** array, openffi_size** sizes_array, openffi_size** dimensions, openffi_size* dimensions_length) \
{  \
    *array = ((type*)data_array[index]); \
    *sizes_array = (openffi_size*)data_array[index+1]; \
    *dimensions = ((openffi_size*)data_array[index+2]); \
    *dimensions_length = *((openffi_size*)data_array[index+3]); \
	return index + 4; \
}

get_arg_type_str_array_decl(openffi_string);
get_arg_type_str_array_decl(openffi_string8);
get_arg_type_str_array_decl(openffi_string16);
get_arg_type_str_array_decl(openffi_string32);

get_arg_type_array_decl(openffi_float64);
get_arg_type_array_decl(openffi_float32);
get_arg_type_array_decl(openffi_int64);
get_arg_type_array_decl(openffi_int32);
get_arg_type_array_decl(openffi_int16);
get_arg_type_array_decl(openffi_int8);
get_arg_type_array_decl(openffi_uint64);
get_arg_type_array_decl(openffi_uint32);
get_arg_type_array_decl(openffi_uint16);
get_arg_type_array_decl(openffi_uint8);
get_arg_type_array_decl(openffi_size);
get_arg_type_array_decl(openffi_bool);

//--------------------------------------------------------------------

int set_arg(void** data_array, int index, void* val);

#define set_arg_openffi_str_decl(type)\
int set_arg_##type(void** data_array, int index, openffi_string val, openffi_size* length);

#define set_arg_openffi_str(type)\
int set_arg_##type(void** data_array, int index, openffi_string val, openffi_size* length)\
{\
    openffi_type* ptype = (openffi_type*)malloc( sizeof(openffi_type) ); \
    *ptype = type##_type; \
    set_arg(data_array, index, (void*)ptype); \
    set_arg(data_array, index+1, (void*)val); \
    set_arg(data_array, index+2, (void*)length); \
	return index + 3;\
}
set_arg_openffi_str_decl(openffi_string);
set_arg_openffi_str_decl(openffi_string8);
set_arg_openffi_str_decl(openffi_string16);
set_arg_openffi_str_decl(openffi_string32);

#define set_arg_type_decl(type) \
int set_arg_##type(void** data_array, int index, type* val);

#define set_arg_type(type) \
int set_arg_##type(void** data_array, int index, type* val) \
{ \
    openffi_type* ptype = (openffi_type*)malloc( sizeof(openffi_type) );   \
    *ptype = type##_type; \
    set_arg(data_array, index, (void*)ptype); \
    set_arg(data_array, index+1, (void*)val); \
    return index + 2; \
}

set_arg_type_decl(openffi_float64);
set_arg_type_decl(openffi_float32);
set_arg_type_decl(openffi_int64);
set_arg_type_decl(openffi_int32);
set_arg_type_decl(openffi_int16);
set_arg_type_decl(openffi_int8);
set_arg_type_decl(openffi_uint64);
set_arg_type_decl(openffi_uint32);
set_arg_type_decl(openffi_uint16);
set_arg_type_decl(openffi_uint8);
set_arg_type_decl(openffi_size);
set_arg_type_decl(openffi_bool);

#define set_arg_openffi_str_array_decl(type)\
int set_arg_##type##_array(void** data_array, int index, type* array, openffi_size* string_sizes, openffi_size* dimensions, openffi_size* dimensions_length);

#define set_arg_openffi_str_array(type)\
int set_arg_##type##_array(void** data_array, int index, type* array, openffi_size* string_sizes, openffi_size* dimensions, openffi_size* dimensions_length)\
{\
    openffi_type* ptype = (openffi_type*)malloc( sizeof(openffi_type) ); \
    *ptype = type##_type | openffi_array_type; \
	set_arg(data_array, index, (void*)ptype); \
    set_arg(data_array, index+1, (void*)array); \
    set_arg(data_array, index+2, (void*)string_sizes); \
    set_arg(data_array, index+3, (void*)dimensions); \
    set_arg(data_array, index+4, (void*)dimensions_length); \
    return index + 5; \
}

set_arg_openffi_str_array_decl(openffi_string);
set_arg_openffi_str_array_decl(openffi_string8);
set_arg_openffi_str_array_decl(openffi_string16);
set_arg_openffi_str_array_decl(openffi_string32);

#define set_arg_type_array_decl(type) \
int set_arg_##type##_array(void** data_array, int index, type* array, openffi_size* dimensions, openffi_size* dimensions_length);

#define set_arg_type_array(type) \
int set_arg_##type##_array(void** data_array, int index, type* array, openffi_size* dimensions, openffi_size* dimensions_length) \
{ \
    data_array[index] = (openffi_type*)malloc( sizeof(openffi_type) ); \
    *((openffi_type*)data_array[index]) = type##_type | openffi_array_type; \
    data_array[index+1] = (void*)array;\
    data_array[index+2] = (void*)dimensions;\
    data_array[index+3] = dimensions_length;\
    return index + 4; \
}

set_arg_type_array_decl(openffi_float64);
set_arg_type_array_decl(openffi_float32);
set_arg_type_array_decl(openffi_int64);
set_arg_type_array_decl(openffi_int32);
set_arg_type_array_decl(openffi_int16);
set_arg_type_array_decl(openffi_int8);
set_arg_type_array_decl(openffi_uint64);
set_arg_type_array_decl(openffi_uint32);
set_arg_type_array_decl(openffi_uint16);
set_arg_type_array_decl(openffi_uint8);
set_arg_type_array_decl(openffi_size);
set_arg_type_array_decl(openffi_bool);

//--------------------------------------------------------------------

#define get_numeric_element_decl(type) \
type get_##type##_element(type* arr, int index); \

#define get_numeric_element(type) \
type get_##type##_element(type* arr, int index)\
{\
    return arr[index];\
}

#define set_numeric_element_decl(type) \
void set_##type##_element(type* arr, int index, type val);\

#define set_numeric_element(type) \
void set_##type##_element(type* arr, int index, type val)\
{\
    arr[index] = val;\
}

const char* get_openffi_string_element(int index, openffi_string *str, const openffi_size *sizes, openffi_size *out_size);

get_numeric_element_decl(openffi_float64);
get_numeric_element_decl(openffi_float32);
get_numeric_element_decl(openffi_int64);
get_numeric_element_decl(openffi_int32);
get_numeric_element_decl(openffi_int16);
get_numeric_element_decl(openffi_int8);
get_numeric_element_decl(openffi_uint64);
get_numeric_element_decl(openffi_uint32);
get_numeric_element_decl(openffi_uint16);
get_numeric_element_decl(openffi_uint8);
get_numeric_element_decl(openffi_size);

void set_openffi_string_element(int index, openffi_string* str_array, openffi_size* sizes_array, openffi_string str, openffi_size str_size);

set_numeric_element_decl(openffi_float64);
set_numeric_element_decl(openffi_float32);
set_numeric_element_decl(openffi_int64);
set_numeric_element_decl(openffi_int32);
set_numeric_element_decl(openffi_int16);
set_numeric_element_decl(openffi_int8);
set_numeric_element_decl(openffi_uint64);
set_numeric_element_decl(openffi_uint32);
set_numeric_element_decl(openffi_uint16);
set_numeric_element_decl(openffi_uint8);
set_numeric_element_decl(openffi_size);

//--------------------------------------------------------------------

#define alloc_str_on_heap_decl(type)\
type alloc_##type##_on_heap(type str, openffi_size str_size);

#define alloc_str_on_heap(type, char_t) \
type alloc_##type##_on_heap(type str, openffi_size str_size)\
{\
	type res = (type)malloc(sizeof(char_t)*str_size+sizeof(char_t));\
    memcpy(res, str, str_size*sizeof(char_t)); \
    res[str_size] = 0;\
    return res;\
}

alloc_str_on_heap_decl(openffi_string);
alloc_str_on_heap_decl(openffi_string8);
alloc_str_on_heap_decl(openffi_string16);
alloc_str_on_heap_decl(openffi_string32);

#define alloc_numeric_on_heap_decl(type) \
type* alloc_##type##_on_heap(type val);

#define alloc_numeric_on_heap(type) \
type* alloc_##type##_on_heap(type val)\
{\
    type* res = (type*)malloc(sizeof(type));\
    *res = val;\
    return res;\
}

alloc_numeric_on_heap_decl(openffi_float64);
alloc_numeric_on_heap_decl(openffi_float32);
alloc_numeric_on_heap_decl(openffi_int64);
alloc_numeric_on_heap_decl(openffi_int32);
alloc_numeric_on_heap_decl(openffi_int16);
alloc_numeric_on_heap_decl(openffi_int8);
alloc_numeric_on_heap_decl(openffi_uint64);
alloc_numeric_on_heap_decl(openffi_uint32);
alloc_numeric_on_heap_decl(openffi_uint16);
alloc_numeric_on_heap_decl(openffi_uint8);
alloc_numeric_on_heap_decl(openffi_size);
alloc_numeric_on_heap_decl(openffi_bool);
alloc_numeric_on_heap_decl(openffi_type);


//--------------------------------------------------------------------


#ifndef SKIP_XLLR_API_EXTERN
}
#endif