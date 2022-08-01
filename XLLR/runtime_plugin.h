#pragma once
#include <stdint.h>
#include <string>
#include <memory>
#include <unordered_map>
#include "foreign_function.h"
#include "runtime_plugin_interface_wrapper.h"
#include <boost/thread.hpp>
#include <atomic>



//--------------------------------------------------------------------
class runtime_plugin
{
private:
	std::string _plugin_filename;
	std::unordered_map<int64_t, std::shared_ptr<foreign_function>> _loaded_functions;
	std::shared_ptr<runtime_plugin_interface_wrapper> _loaded_plugin;
	bool _is_runtime_loaded = false;
	mutable boost::shared_mutex _mutex;

public:
	explicit runtime_plugin(const std::string& plugin_name, bool is_init = true);
	~runtime_plugin();

	void init();
	void fini();

	void load_runtime();
	void free_runtime();
	
	std::shared_ptr<foreign_function> load_function(const std::string& function_path, int64_t function_id, int8_t params_count, int8_t retval_count);
	void free_function(int64_t function_id);
	std::shared_ptr<foreign_function> get_function(int64_t function_id) const;
	
};
//--------------------------------------------------------------------
