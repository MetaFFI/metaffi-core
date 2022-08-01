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
	int64_t _id;
	std::shared_ptr<runtime_plugin_interface_wrapper> _plugin;

public:
	foreign_function(std::shared_ptr<runtime_plugin_interface_wrapper> plugin, int64_t id);
	~foreign_function() = default;
	
	[[nodiscard]] int64_t id() const;

	void xcall_params_ret(
		// error string
		cdts params_ret[2],
		char** out_err, uint64_t* out_err_len
	);
	
	void xcall_no_params_ret(
			// error string
			cdts return_values[1],
			char** out_err, uint64_t* out_err_len
	);
	
	void xcall_params_no_ret(
			// error string
			cdts parameters[1],
			char** out_err, uint64_t* out_err_len
	);
	
	void xcall_no_params_no_ret(
			// error string
			char** out_err, uint64_t* out_err_len
	);
};
//--------------------------------------------------------------------