#include <cdts.h>
#include <cstring>
#include <cstdio>
#include "cdts_alloc.h"

/************************************************
*   Allocations
*************************************************/


#define alloc_numeric_on_heap_impl(type) \
type* alloc_##type##_on_heap(type val)\
{\
    type* res = (type*)malloc(sizeof(type));\
    *res = val;\
    return res;\
}\
type* alloc_##type##_array_on_heap(metaffi_size length)\
{\
	return (type*)calloc(length, sizeof(type));\
}

#define alloc_string_on_heap_impl(type, char_t) \
type alloc_##type##_on_heap(type str, metaffi_size str_size)\
{\
	type res = (type)malloc(sizeof(char_t)*str_size+sizeof(char_t));\
    memcpy(res, str, str_size*sizeof(char_t)); \
    res[str_size] = 0;\
    return res;\
} \
type* alloc_##type##_array_on_heap(metaffi_size length)\
{\
	return (type*)calloc(length, sizeof(type));\
}

alloc_numeric_on_heap_impl(metaffi_float64);
alloc_numeric_on_heap_impl(metaffi_float32);
alloc_numeric_on_heap_impl(metaffi_int64);
alloc_numeric_on_heap_impl(metaffi_int32);
alloc_numeric_on_heap_impl(metaffi_int16);
alloc_numeric_on_heap_impl(metaffi_int8);
alloc_numeric_on_heap_impl(metaffi_uint64);
alloc_numeric_on_heap_impl(metaffi_uint32);
alloc_numeric_on_heap_impl(metaffi_uint16);
alloc_numeric_on_heap_impl(metaffi_uint8);
alloc_numeric_on_heap_impl(metaffi_size);
alloc_numeric_on_heap_impl(metaffi_bool);
alloc_numeric_on_heap_impl(metaffi_type);
struct cdt_metaffi_handle *alloc_metaffi_handle_on_heap(metaffi_handle val, uint64_t runtime_id)
{
	auto res = (struct cdt_metaffi_handle *) malloc(sizeof(struct cdt_metaffi_handle));
	res->val = val;
	res->runtime_id = runtime_id;
	return res;
}
struct cdt_metaffi_handle *alloc_metaffi_handle_array_on_heap(metaffi_size length)
{ return (struct cdt_metaffi_handle *) calloc(length, sizeof(struct cdt_metaffi_handle)); }
alloc_string_on_heap_impl(metaffi_string8, char);
alloc_string_on_heap_impl(metaffi_string16, char16_t);
alloc_string_on_heap_impl(metaffi_string32, char32_t);

//====================================================================

/************************************************
*   Getters
*************************************************/


metaffi_type get_type(cdt* data_array, int index)
{
	return ((cdt*)&data_array[index])->type; // first field in each CDT is metaffi_type!
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
type get_##type##_element(type* arr, int index, const metaffi_size* sizes, metaffi_size* out_size)\
{\
    type res = arr[index];\
	*out_size = sizes[index];\
	return res;\
}

get_numeric_element_impl(metaffi_float64);
get_numeric_element_impl(metaffi_float32);
get_numeric_element_impl(metaffi_int64);
get_numeric_element_impl(metaffi_int32);
get_numeric_element_impl(metaffi_int16);
get_numeric_element_impl(metaffi_int8);
get_numeric_element_impl(metaffi_uint64);
get_numeric_element_impl(metaffi_uint32);
get_numeric_element_impl(metaffi_uint16);
get_numeric_element_impl(metaffi_uint8);
get_numeric_element_impl(metaffi_size);
get_numeric_element_impl(metaffi_bool);
struct cdt_metaffi_handle get_metaffi_handle_element(struct cdt_metaffi_handle *arr, int index)
{ return arr[index]; }
get_string_element_impl(metaffi_string8);
get_string_element_impl(metaffi_string16);
get_string_element_impl(metaffi_string32);


/************************************************
*   Array Elements Setters
*************************************************/

#define set_numeric_element_impl(type) \
void set_##type##_element(type* arr, int index, type val)\
{\
    arr[index] = val;\
}

#define set_string_element_impl(type) \
void set_##type##_element(type* arr, metaffi_size* sizes_array, int index, type val, metaffi_size str_size)\
{\
    arr[index] = val;\
    sizes_array[index] = str_size;\
}

set_numeric_element_impl(metaffi_float64);
set_numeric_element_impl(metaffi_float32);
set_numeric_element_impl(metaffi_int64);
set_numeric_element_impl(metaffi_int32);
set_numeric_element_impl(metaffi_int16);
set_numeric_element_impl(metaffi_int8);
set_numeric_element_impl(metaffi_uint64);
set_numeric_element_impl(metaffi_uint32);
set_numeric_element_impl(metaffi_uint16);
set_numeric_element_impl(metaffi_uint8);
set_numeric_element_impl(metaffi_bool);
set_numeric_element_impl(metaffi_size);
void set_metaffi_handle_element(struct cdt_metaffi_handle *arr, int index, metaffi_handle val, uint64_t runtime_id)
{ arr[index].val = val; arr[index].runtime_id = runtime_id; }
set_string_element_impl(metaffi_string8);
set_string_element_impl(metaffi_string16);
set_string_element_impl(metaffi_string32);


//--------------------------------------------------------------------
