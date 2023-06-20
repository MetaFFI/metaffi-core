#include <cstring>
#include "runtime/cdt_structs.h"
#include <cstdlib>

thread_local cdt cdt_cache[50];
thread_local cdts cdts_cache[2];

//--------------------------------------------------------------------
extern "C" cdts* alloc_cdts_buffer(metaffi_size params_count, metaffi_types ret_count)
{
	if(params_count == 0 && ret_count == 0){
		return nullptr;
	}
	
	
	if(params_count + ret_count <= 50) // use cache
	{
		cdts_cache[0].pcdt = cdt_cache;
		cdts_cache[0].len = params_count;
		cdts_cache[0].is_free = 0;
		cdts_cache[1].pcdt = cdt_cache+params_count;
		cdts_cache[1].len = ret_count;
		cdts_cache[1].is_free = 0;
	}
	else
	{
		cdts_cache[0].pcdt = (cdt*)calloc(sizeof(cdt), params_count+ret_count);
		cdts_cache[0].len = params_count;
		cdts_cache[0].is_free = 1;
		cdts_cache[1].pcdt = cdts_cache[0].pcdt+params_count;
		cdts_cache[1].len = ret_count;
		cdts_cache[1].is_free = 0;
	}
	
	return (cdts*)cdts_cache;
}
//--------------------------------------------------------------------