#pragma once

#include <utils/foreign_function.h>

int init_jit(int argc, char* argv[]);

extern "C"
{
	pforeign_function_entrypoint_signature_params_ret_t create_xcall_params_ret(const std::vector<uint32_t>& constant_sizes, const std::vector<int64_t>& constant_values, const char* function_path, void* func);
}
