#pragma once
#include <stdint.h>
#include <string>
#include <memory>
#include <unordered_map>
#include "foreign_module.h"
#include "xllr_plugin_interface_wrapper.h"



//--------------------------------------------------------------------
class xllr_plugin
{
private:
	std::string _plugin_name;
	std::unordered_map<std::string, std::unique_ptr<foreign_module>> _modules;
	std::unique_ptr<xllr_plugin_interface_wrapper> _loaded_plugin;

public:
	xllr_plugin(const std::string& plugin_name);
	~xllr_plugin();

	void load(void);
	void release(void);

	void load_runtime(void);
	void release_runtime(void);
	
	void load_module(const std::string& module_name);
	
	foreign_module& operator[](const std::string& module_name);
	
};
//--------------------------------------------------------------------
