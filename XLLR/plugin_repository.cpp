#include "plugin_repository.h"
#include <iostream>

//--------------------------------------------------------------------
plugin_respository::~plugin_respository()
{
	try
	{
		std::vector<std::string> keys(this->_plugins.size());
		for(auto& cur_mod : this->_plugins){
			keys.push_back(cur_mod.first);
		}

		for(const std::string& k : keys){
			this->release(k);
		}
	}
	catch(const std::exception& e)
	{
		std::cout << "Failed to release plugins. Error: " << e.what() << std::endl;
	}
    catch(...)
	{
		std::cout << "Failed to release plugins. Unknown error." << std::endl;
	}
	
}
//--------------------------------------------------------------------
std::shared_ptr<xllr_plugin> plugin_respository::get(const std::string& plugin) const
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
std::shared_ptr<xllr_plugin> plugin_respository::load(const std::string& plugin)
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

	std::shared_ptr<xllr_plugin> res = std::make_shared<xllr_plugin>(plugin);
	this->_plugins[plugin] = res;
	return res;
}
//--------------------------------------------------------------------
void plugin_respository::release(const std::string& plugin)
{
    // readers lock
	boost::upgrade_lock<boost::shared_mutex> read_lock(this->_mutex);

	auto it = this->_plugins.find(plugin);
	if(it == this->_plugins.end()) // if found
	{
		return;
	}

	// not found - load it
	boost::upgrade_to_unique_lock<boost::shared_mutex> exclusive_lock(read_lock);

	this->_plugins.erase(it);
}
//--------------------------------------------------------------------