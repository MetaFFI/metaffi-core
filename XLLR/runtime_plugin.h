#pragma once
#include "runtime_plugin_interface_wrapper.h"
#include <atomic>
#include <boost/thread.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>


//--------------------------------------------------------------------
class runtime_plugin
{
private:
	std::string _plugin_filename;
	std::unique_ptr<runtime_plugin_interface_wrapper> _loaded_plugin;
	bool _is_runtime_loaded = false;
	mutable boost::shared_mutex _mutex;

public:
	explicit runtime_plugin(std::string plugin_name, bool is_init = true);
	~runtime_plugin();

	void init();
	char* fini();

	void load_runtime(char** out_err);
	void free_runtime(char** out_err);
	
	xcall* load_entity(const std::string& module_path, const std::string& entity_path, const std::vector<metaffi_type_info>& params_types, const std::vector<metaffi_type_info>& retval_types, char** out_err);
	xcall* make_callable(void* make_callable_context, const std::vector<metaffi_type_info>& params_types, const std::vector<metaffi_type_info>& retval_types, char** out_err);
	void free_xcall(xcall* pxcall, char** out_err);
	
};
//--------------------------------------------------------------------
