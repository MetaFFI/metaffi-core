#include <cstring>
#include "runtime/cdt.h"
#include <cstdlib>

thread_local cdt cdt_cache[cdts_cache_size];
thread_local cdts cdts_cache[2];

//--------------------------------------------------------------------
extern "C" cdts* alloc_cdts_buffer(metaffi_size params_count, metaffi_size ret_count)
{
	if(params_count == 0 && ret_count == 0){
		return nullptr;
	}
	
	
	if(params_count + ret_count <= 50) // use cache
	{
		cdts_cache[0].arr = cdt_cache;
		cdts_cache[0].length = params_count;
		cdts_cache[0].free_required = 0;
		cdts_cache[1].arr = cdt_cache+params_count;
		cdts_cache[1].length = ret_count;
		cdts_cache[1].free_required = 0;
	}
	else
	{
		cdts_cache[0].arr = new cdt[params_count+ret_count];
		cdts_cache[0].length = params_count;
		cdts_cache[0].free_required = 1;
		cdts_cache[1].arr = cdts_cache[0].arr+params_count;
		cdts_cache[1].length = ret_count;
		cdts_cache[1].free_required = 0;
	}
	
	return (cdts*)cdts_cache;
}
//--------------------------------------------------------------------