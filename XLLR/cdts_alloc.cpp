#include <cstring>
#include "runtime/cdt.h"
#include <utils/logger.hpp>
#include <cstdlib>

thread_local cdt cdt_cache[cdt_cache_size];
thread_local int cdt_current_index = 0;
thread_local cdts cdts_cache[cdts_cache_size];
thread_local int cdts_current_index = 0;
static auto LOG = metaffi::get_logger("xllr");

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
			METAFFI_CRITICAL(LOG, "alloc_cdts_buffer: negative index");
			std::abort();
		}
		
		cdts_cache[cdts_current_index].arr = cdt_cache+cdt_current_index;
		cdts_cache[cdts_current_index].length = params_count;
		cdts_cache[cdts_current_index].allocated_on_cache = 1;
		cdts_cache[cdts_current_index+1].arr = cdt_cache+cdt_current_index+params_count;
		cdts_cache[cdts_current_index+1].length = ret_count;
		cdts_cache[cdts_current_index+1].allocated_on_cache = 1;
		
		auto res = (cdts*)&cdts_cache[cdts_current_index];
		
		cdts_current_index += 2;
		cdt_current_index += static_cast<int>(params_count + ret_count);
		
		return res;
	}
	else
	{
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
		// pcdts[0].arr and pcdts[1].arr point into the same contiguous
		// allocation (pcdts[1].arr = pcdts[0].arr + params_count).
		// cdts::free() calls delete[] on non-cache arrays, so freeing
		// pcdts[0] already destroys ALL cdt elements and releases the
		// memory.  Null pcdts[1].arr to prevent a double-free.
		pcdts[0].free();
		pcdts[1].arr = nullptr;
		pcdts[1].free();  // no-op: arr is null
		delete[] pcdts;
	}
	else
	{
		int cdt_to_free = static_cast<int>(pcdts[0].length + pcdts[1].length);
		pcdts[0].free();
		pcdts[1].free();
		cdts_current_index -= 2;
		cdt_current_index -= cdt_to_free;

		if(cdts_current_index < 0 || cdt_current_index < 0)
		{
			// SIGABRT
			METAFFI_CRITICAL(LOG, "free_cdts_buffer: negative index");
			std::abort();
		}
	}
}
//--------------------------------------------------------------------
