#include "runtime_plugin_repository.h"
#include <utils/logger.hpp>

static auto LOG = metaffi::get_logger("xllr");

//--------------------------------------------------------------------
runtime_plugin_repository::~runtime_plugin_repository()
{
	fprintf(stderr, "+++ ~runtime_plugin_repository: entering destructor, %zu plugins\n", this->_plugins.size());
	fflush(stderr);

	try
	{
		std::vector<std::string> keys;
		keys.reserve(this->_plugins.size());
		for(auto& cur_mod : this->_plugins){
			keys.push_back(cur_mod.first);
		}

		for(const std::string& k : keys){
			this->unload(k);
		}
	}
	catch(const std::exception& e)
	{
		METAFFI_ERROR_EX(LOG, "Failed to unload plugins", e);
	}
    catch(...)
	{
		METAFFI_ERROR(LOG, "Failed to unload plugins. Unknown error.");
	}

	fprintf(stderr, "+++ ~runtime_plugin_repository: destructor done\n");
	fflush(stderr);
}
//--------------------------------------------------------------------
std::shared_ptr<runtime_plugin> runtime_plugin_repository::get(const std::string& plugin) const
{
	return this->get(plugin.c_str());
}
//--------------------------------------------------------------------
std::shared_ptr<runtime_plugin> runtime_plugin_repository::get(const char* plugin) const
{
	// readers lock
	boost::shared_lock<boost::shared_mutex> readlock(this->_mutex);
	
	auto it = this->_plugins.find(plugin);
	if(it == this->_plugins.end()) // if NOT found
	{
		return nullptr;
	}

	return it->second;
}
//--------------------------------------------------------------------
std::shared_ptr<runtime_plugin> runtime_plugin_repository::load(const std::string& plugin)
{
	return this->load(plugin.c_str());
}
//--------------------------------------------------------------------
std::shared_ptr<runtime_plugin> runtime_plugin_repository::load(const char* plugin)
{
	// readers lock
	boost::upgrade_lock<boost::shared_mutex> read_lock(this->_mutex);
	
	auto it = this->_plugins.find(plugin);
	if(it != this->_plugins.end()) // if found
	{
		return it->second;
	}

	// not found - load it
	boost::upgrade_to_unique_lock<boost::shared_mutex> exclusive_lock(read_lock);

	std::shared_ptr<runtime_plugin> res = std::make_shared<runtime_plugin>(plugin);
	this->_plugins[plugin] = res;
	return res;
}
//--------------------------------------------------------------------
void runtime_plugin_repository::unload(const std::string& plugin)
{
	this->unload(plugin.c_str());
}
//--------------------------------------------------------------------
void runtime_plugin_repository::unload(const char* plugin)
{
	fprintf(stderr, "+++ runtime_plugin_repository::unload('%s')\n", plugin);
	fflush(stderr);

	// readers lock
	boost::upgrade_lock<boost::shared_mutex> read_lock(this->_mutex);

	auto it = this->_plugins.find(plugin);
	if(it == this->_plugins.end()) // if found
	{
		return;
	}

	char* out_err = nullptr;
	it->second->free_runtime(&out_err);

	// Erase from map before checking error so the shared_ptr ref count drops to zero
	// and the destructor runs in controlled fashion (not later during map destruction).
	boost::upgrade_to_unique_lock<boost::shared_mutex> exclusive_lock(read_lock);
	this->_plugins.erase(it);

	if(out_err != nullptr)
	{
		throw std::runtime_error(out_err);
	}
	
}
//--------------------------------------------------------------------
