#pragma once
#include <cstring>
#include <runtime/cdt.h>

struct cdts* alloc_cdts_buffer(metaffi_size params_count, metaffi_size ret_count);
void free_cdts_buffer(struct cdts* pcdts);