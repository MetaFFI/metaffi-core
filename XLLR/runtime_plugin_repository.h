#pragma once
#include "runtime_plugin.h"
#include <unordered_map>
#include <boost/thread.hpp>

//--------------------------------------------------------------------
class runtime_plugin_respository
{
private:
	mutable boost::shared_mutex _mutex;
	std::unordered_map<std::string, std::shared_ptr<runtime_plugin>> _plugins;

public:
	runtime_plugin_respository() = default;
	~runtime_plugin_respository();

	// returns xllr plugin - throws runtime_error in case not loaded
	std::shared_ptr<runtime_plugin> get(const std::string& plugin) const;

	// loads and returns given xllr plugin
	std::shared_ptr<runtime_plugin> load(const std::string& plugin);
	void release(const std::string& plugin);

};
//--------------------------------------------------------------------
