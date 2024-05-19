#include <cstring>
#include "runtime/cdt.h"
#include <cstdlib>

thread_local cdt cdt_cache[cdt_cache_size];
thread_local int cdt_current_index = 0;
thread_local cdts cdts_cache[cdts_cache_size];
thread_local int cdts_current_index = 0;

struct initialize_cdts_cache
{
	initialize_cdts_cache()
	{
		for(int i = 0; i < cdts_cache_size; i++)
		{
			cdts_cache[i].allocated_on_cache = 1;
		}
		
		cdt_current_index = 0;
		cdts_current_index = 0;
	}
};
static initialize_cdts_cache init_cdts_cache;

//--------------------------------------------------------------------
extern "C" cdts* alloc_cdts_buffer(metaffi_size params_count, metaffi_size ret_count)
{
	if(params_count == 0 && ret_count == 0){
		return nullptr;
	}
	
	if(cdts_current_index + 2 < cdts_cache_size && cdt_current_index + params_count + ret_count < cdt_cache_size) // use cache
	{
		if(cdts_current_index < 0 || cdt_current_index < 0)
		{
			// SIGABRT
			fprintf(stderr, "FATAL ERROR: alloc_cdts_buffer: negative index\n");
			std::abort();
		}
		
		//printf("++++ allocating from cache: %d , %d (+%ld)\n", cdts_current_index, cdt_current_index, (params_count+ret_count));
		cdts_cache[cdts_current_index].arr = cdt_cache+cdt_current_index;
		cdts_cache[cdts_current_index].length = params_count;
		cdts_cache[cdts_current_index+1].arr = cdt_cache+cdt_current_index+params_count;
		cdts_cache[cdts_current_index+1].length = ret_count;
		
		auto res = (cdts*)&cdts_cache[cdts_current_index];
		
		cdts_current_index += 2;
		cdt_current_index += (params_count + ret_count);
		
		return res;
	}
	else
	{
		//printf("++++ allocating from heap: %d , %d\n", cdts_current_index, cdt_current_index);
		
		cdts* pcdts = new cdts[2];
		
		pcdts[0].arr = new cdt[params_count+ret_count];
		pcdts[0].length = params_count;
		pcdts[0].allocated_on_cache = 0;
		pcdts[1].arr = pcdts[0].arr + params_count;
		pcdts[1].length = ret_count;
		pcdts[1].allocated_on_cache = 0;
		return pcdts;
	}
}
//--------------------------------------------------------------------
extern "C" void free_cdts_buffer(struct cdts* pcdts)
{
	if(pcdts == nullptr)
	{
		return;
	}
	
	if(pcdts->allocated_on_cache == 0)
	{
		//printf("---- freeing from heap: %d , %d\n", cdts_current_index, cdt_current_index);
		
		pcdts[0].free();
		pcdts[1].free();
		delete[] pcdts;
	}
	else
	{
		int cdt_to_free = pcdts[0].length + pcdts[1].length;
		pcdts[0].free();
		pcdts[1].free();
		cdts_current_index -= 2;
		cdt_current_index -= cdt_to_free;
		
		//printf("---- freeing from cache: %d , %d (-%d)\n", cdts_current_index, cdt_current_index, cdt_to_free);
		
		if(cdts_current_index < 0 || cdt_current_index < 0)
		{
			// SIGABRT
			fprintf(stderr, "FATAL ERROR: free_cdts_buffer: negative index\n");
			std::abort();
		}
	}
}
//--------------------------------------------------------------------