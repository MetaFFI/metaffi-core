#pragma once
#include <cstdarg>
#include <cstdint>
#include <string>
#include <memory>
#include "runtime_plugin_interface_wrapper.h"

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

	void call(
		// error string
		void** parameters, uint64_t parameters_len,
		void** return_values, uint64_t return_values_len,
		char** out_err, uint64_t* out_err_len
	);
};
//--------------------------------------------------------------------