#pragma once
#include <string>
#include <vector>


class idl_extractor
{
public:
	static std::string extract(const std::string& idl_path, const std::string& user_selected_idl_plugin);
};
