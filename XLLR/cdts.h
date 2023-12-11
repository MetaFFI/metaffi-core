#pragma once
#include <cstdlib>
#include <cstdint>
#include <runtime/metaffi_primitives.h>
#include <runtime/cdt_structs.h>

/***
 * Common Data Types (CDTs) is an array of CDT*.
 * Each CDT is a structure containing "type" and "data".
 * THE FIRST ITEM in the structure *MUST* be an "metaffi_type" (i.e. CDT cannot contain v-table!)
 */


#ifndef SKIP_XLLR_API_EXTERN
extern "C"
{
#endif

/************************************************
*   Allocations
*************************************************/

// Declarations
#define alloc_numeric_on_heap_decl(type) \
type* alloc_##type##_on_heap(type val);  \
type* alloc_##type##_array_on_heap(metaffi_size length);

#define alloc_string_on_heap_decl(type)\
type alloc_##type##_on_heap(type str, metaffi_size str_size); \
type* alloc_##type##_array_on_heap(metaffi_size length);

alloc_numeric_on_heap_decl(metaffi_float64);
alloc_numeric_on_heap_decl(metaffi_float32);
alloc_numeric_on_heap_decl(metaffi_int64);
alloc_numeric_on_heap_decl(metaffi_int32);
alloc_numeric_on_heap_decl(metaffi_int16);
alloc_numeric_on_heap_decl(metaffi_int8);
alloc_numeric_on_heap_decl(metaffi_uint64);
alloc_numeric_on_heap_decl(metaffi_uint32);
alloc_numeric_on_heap_decl(metaffi_uint16);
alloc_numeric_on_heap_decl(metaffi_uint8);
alloc_numeric_on_heap_decl(metaffi_size);
alloc_numeric_on_heap_decl(metaffi_bool);
alloc_numeric_on_heap_decl(metaffi_type);
struct cdt_metaffi_handle *alloc_metaffi_handle_on_heap(metaffi_handle val, uint64_t runtime_id);
struct cdt_metaffi_handle *alloc_metaffi_handle_array_on_heap(metaffi_size length);

alloc_string_on_heap_decl(metaffi_string8);
alloc_string_on_heap_decl(metaffi_string16);
alloc_string_on_heap_decl(metaffi_string32);

//====================================================================

/************************************************
*   Getters
*************************************************/


metaffi_type get_type(cdt* data_array, int index);
cdt* get_cdt(cdt* data_array, int index);

//====================================================================

/************************************************
*   Array Elements Getters
*************************************************/

#define get_numeric_element_decl(type) \
type get_##type##_element(type* arr, int index);

#define get_string_element_decl(type) \
type get_##type##_element(type* arr, int index, const metaffi_size *sizes, metaffi_size *out_size);

get_numeric_element_decl(metaffi_float64);
get_numeric_element_decl(metaffi_float32);
get_numeric_element_decl(metaffi_int64);
get_numeric_element_decl(metaffi_int32);
get_numeric_element_decl(metaffi_int16);
get_numeric_element_decl(metaffi_int8);
get_numeric_element_decl(metaffi_uint64);
get_numeric_element_decl(metaffi_uint32);
get_numeric_element_decl(metaffi_uint16);
get_numeric_element_decl(metaffi_uint8);
get_numeric_element_decl(metaffi_size);
get_numeric_element_decl(metaffi_bool);
struct cdt_metaffi_handle get_metaffi_handle_element(struct cdt_metaffi_handle *arr, int index);
get_string_element_decl(metaffi_string8);
get_string_element_decl(metaffi_string16);
get_string_element_decl(metaffi_string32);


/************************************************
*   Array Elements Setters
*************************************************/

#define set_numeric_element_decl(type) \
void set_##type##_element(type* arr, int index, type val);

#define set_string_element_decl(type) \
void set_##type##_element(type* arr, metaffi_size* sizes_array, int index, type str, metaffi_size str_size);

set_numeric_element_decl(metaffi_float64);
set_numeric_element_decl(metaffi_float32);
set_numeric_element_decl(metaffi_int64);
set_numeric_element_decl(metaffi_int32);
set_numeric_element_decl(metaffi_int16);
set_numeric_element_decl(metaffi_int8);
set_numeric_element_decl(metaffi_uint64);
set_numeric_element_decl(metaffi_uint32);
set_numeric_element_decl(metaffi_uint16);
set_numeric_element_decl(metaffi_uint8);
set_numeric_element_decl(metaffi_size);
set_numeric_element_decl(metaffi_bool);
void set_metaffi_handle_element(struct cdt_metaffi_handle *arr, int index, metaffi_handle val, uint64_t runtime_id);
set_string_element_decl(metaffi_string8);
set_string_element_decl(metaffi_string16);
set_string_element_decl(metaffi_string32);

//--------------------------------------------------------------------

#ifndef SKIP_XLLR_API_EXTERN
}
#endif