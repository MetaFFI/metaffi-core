#pragma once
#include <stdint.h>
#include <string>
#include <memory>
#include <unordered_map>
#include "foreign_module.h"
#include "xllr_plugin_interface_wrapper.h"
#include <boost/thread.hpp>
#include <atomic>



//--------------------------------------------------------------------
class xllr_plugin
{
private:
	std::string _plugin_filename;
	std::unordered_map<std::string, std::shared_ptr<foreign_module>> _loaded_modules;
	std::shared_ptr<xllr_plugin_interface_wrapper> _loaded_plugin;
	bool _is_runtime_loaded = false;
	mutable boost::shared_mutex _mutex;

public:
	xllr_plugin(const std::string& plugin_name, bool is_init = true);
	~xllr_plugin();

	void init(void);
	void fini(void);

	void load_runtime(void);
	void free_runtime(void);
	
	std::shared_ptr<foreign_module> load_module(const std::string& module_name);
	void free_module(const std::string& module_name);
	std::shared_ptr<foreign_module> get_module(const std::string& module_name) const;
	
};
//--------------------------------------------------------------------
