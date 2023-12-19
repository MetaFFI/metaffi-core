#pragma once
#include <cstdint>
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
	std::unordered_map<void**, std::shared_ptr<foreign_function>> _loaded_functions;
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
	
	std::shared_ptr<foreign_function> load_function(const std::string& module_path, const std::string& function_path, const std::vector<metaffi_type_with_alias>& params_types, const std::vector<metaffi_type_with_alias>& retval_types);
	std::shared_ptr<foreign_function> load_callable(void* load_callable_context, const std::vector<metaffi_type_with_alias>& params_types, const std::vector<metaffi_type_with_alias>& retval_types);
	void free_function(void** pff);
	std::shared_ptr<foreign_function> get_function(void** pff) const;
	
};
//--------------------------------------------------------------------
