#pragma once
#include "xllr_plugin.h"
#include <unordered_map>
#include <boost/thread.hpp>

//--------------------------------------------------------------------
class plugin_respository
{
private:
	mutable boost::shared_mutex _mutex;
	std::unordered_map<std::string, std::shared_ptr<xllr_plugin>> _plugins;

public:
	plugin_respository() = default;
	~plugin_respository();

	// returns xllr plugin - throws runtime_error in case not loaded
	std::shared_ptr<xllr_plugin> get(const std::string& plugin) const;

	// loads and returns given xllr plugin
	std::shared_ptr<xllr_plugin> load(const std::string& plugin);
	void release(const std::string& plugin);

};
//--------------------------------------------------------------------
