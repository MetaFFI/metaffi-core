#pragma once
#include <cstdarg>
#include <cstdint>
#include <string>
#include <memory>
#include "runtime_plugin_interface_wrapper.h"
#include <runtime/cdt_structs.h>

//--------------------------------------------------------------------
class foreign_function
{
private:
	void* _pforeign_function;
	void* _context;
	std::shared_ptr<runtime_plugin_interface_wrapper> _plugin;

public:
	foreign_function(std::shared_ptr<runtime_plugin_interface_wrapper> plugin, void* xcall_and_context[2]);
	~foreign_function() = default;
	
	[[nodiscard]] void* pforeign_function() const;
	[[nodiscard]] void* pcontext() const;

	void xcall_params_ret(
		cdts params_ret[2],
		char** out_err, uint64_t* out_err_len
	);
	
	void xcall_no_params_ret(
			cdts return_values[1],
			char** out_err, uint64_t* out_err_len
	);
	
	void xcall_params_no_ret(
			cdts parameters[1],
			char** out_err, uint64_t* out_err_len
	);
	
	void xcall_no_params_no_ret(
			char** out_err, uint64_t* out_err_len
	);
};
//--------------------------------------------------------------------