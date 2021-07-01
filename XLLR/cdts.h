#pragma once
#include <cstdlib>
#include <cstdint>
#include <runtime/openffi_primitives.h>

/***
 * Common Data Types (CDTs) is an array of CDT*.
 * Each CDT is a structure containing "type" and "data".
 * THE FIRST ITEM in the structure *MUST* be an "openffi_type" (i.e. CDT cannot contain v-table!)
 */


#ifndef SKIP_XLLR_API_EXTERN
extern "C"
{
#endif

/************************************************
*   Allocations
*************************************************/

void** alloc_cdts_buffer(int cdt_count);

// Declarations
#define alloc_numeric_on_heap_decl(type) \
type* alloc_##type##_on_heap(type val);  \
type* alloc_##type##_array_on_heap(openffi_size length);

#define alloc_string_on_heap_decl(type)\
type alloc_##type##_on_heap(type str, openffi_size str_size); \
type* alloc_##type##_array_on_heap(openffi_size length);

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

alloc_string_on_heap_decl(openffi_string);
alloc_string_on_heap_decl(openffi_string8);
alloc_string_on_heap_decl(openffi_string16);
alloc_string_on_heap_decl(openffi_string32);

//====================================================================

/************************************************
*   Getters
*************************************************/


openffi_type get_type(void** data_array, int index);

#define get_cdt_numeric_type_decl(type) \
int get_cdt_##type(void** data_array, int index, type* out_res); \
int get_cdt_##type##_array(void** data_array, int index, type** out_res, openffi_size** dimensions_lengths, openffi_size* dimensions);

#define get_cdt_string_type_decl(type)\
int get_cdt_##type(void** data_array, int index, type* out_res, openffi_size* length); \
int get_cdt_##type##_array(void** data_array, int index, type** array, openffi_size** sizes_array, openffi_size** dimensions_lengths, openffi_size* dimensions);

get_cdt_numeric_type_decl(openffi_float64);
get_cdt_numeric_type_decl(openffi_float32);
get_cdt_numeric_type_decl(openffi_int64);
get_cdt_numeric_type_decl(openffi_int32);
get_cdt_numeric_type_decl(openffi_int16);
get_cdt_numeric_type_decl(openffi_int8);
get_cdt_numeric_type_decl(openffi_uint64);
get_cdt_numeric_type_decl(openffi_uint32);
get_cdt_numeric_type_decl(openffi_uint16);
get_cdt_numeric_type_decl(openffi_uint8);
get_cdt_numeric_type_decl(openffi_size);
get_cdt_numeric_type_decl(openffi_bool);
get_cdt_string_type_decl(openffi_string);
get_cdt_string_type_decl(openffi_string8);
get_cdt_string_type_decl(openffi_string16);
get_cdt_string_type_decl(openffi_string32);


//====================================================================

/************************************************
*   Setters
*************************************************/

#define set_cdt_string_type_decl(type)\
int set_cdt_##type(void** data_array, int index, openffi_string val, openffi_size* length); \
int set_cdt_##type##_array(void** data_array, int index, type* array, openffi_size* string_sizes, openffi_size* dimensions_lengths, openffi_size* dimensions);

#define set_cdt_numeric_type_decl(type) \
int set_cdt_##type(void** data_array, int index, type* val); \
int set_cdt_##type##_array(void** data_array, int index, type* array, openffi_size* dimensions_lengths, openffi_size* dimensions);

set_cdt_numeric_type_decl(openffi_float64);
set_cdt_numeric_type_decl(openffi_float32);
set_cdt_numeric_type_decl(openffi_int64);
set_cdt_numeric_type_decl(openffi_int32);
set_cdt_numeric_type_decl(openffi_int16);
set_cdt_numeric_type_decl(openffi_int8);
set_cdt_numeric_type_decl(openffi_uint64);
set_cdt_numeric_type_decl(openffi_uint32);
set_cdt_numeric_type_decl(openffi_uint16);
set_cdt_numeric_type_decl(openffi_uint8);
set_cdt_numeric_type_decl(openffi_size);
set_cdt_numeric_type_decl(openffi_bool);
set_cdt_string_type_decl(openffi_string);
set_cdt_string_type_decl(openffi_string8);
set_cdt_string_type_decl(openffi_string16);
set_cdt_string_type_decl(openffi_string32);

//====================================================================

/************************************************
*   Array Elements Getters
*************************************************/

#define get_numeric_element_decl(type) \
type get_##type##_element(type* arr, int index);

#define get_string_element_decl(type) \
type get_##type##_element(type* arr, int index, const openffi_size *sizes, openffi_size *out_size);

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
get_string_element_decl(openffi_string);
get_string_element_decl(openffi_string8);
get_string_element_decl(openffi_string16);
get_string_element_decl(openffi_string32);


/************************************************
*   Array Elements Setters
*************************************************/

#define set_numeric_element_decl(type) \
void set_##type##_element(type* arr, int index, type val);

#define set_string_element_decl(type) \
void set_##type##_element(type* arr, openffi_size* sizes_array, int index, type str, openffi_size str_size);

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
set_string_element_decl(openffi_string);
set_string_element_decl(openffi_string8);
set_string_element_decl(openffi_string16);
set_string_element_decl(openffi_string32);

//--------------------------------------------------------------------

#ifndef SKIP_XLLR_API_EXTERN
}
#endif