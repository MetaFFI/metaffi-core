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
	std::unordered_map<uint64_t, std::shared_ptr<xcall>> _loaded_entities;
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
	
	std::shared_ptr<xcall> load_entity(const std::string& module_path, const std::string& function_path, const std::vector<metaffi_type_info>& params_types, const std::vector<metaffi_type_info>& retval_types, char** out_err);
	std::shared_ptr<xcall> make_callable(void* make_callable_context, const std::vector<metaffi_type_info>& params_types, const std::vector<metaffi_type_info>& retval_types, char** out_err);
	void free_and_remove_xcall_from_cache(xcall* pxcall, char** out_err);
	void free_and_remove_xcall_from_cache(uint64_t xcall_cache_key, char** out_err);
	
private:
	static uint64_t calc_key(const xcall& pxcall) ;
	
};
//--------------------------------------------------------------------
