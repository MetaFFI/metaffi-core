#pragma once
#include <compiler/idl_plugin_interface.h>
#include <memory>
#include <type_traits>
#include <boost/dll.hpp>

//--------------------------------------------------------------------
class idl_plugin_interface_wrapper : public idl_plugin_interface
{
public:
	static std::shared_ptr<idl_plugin_interface_wrapper> load(const std::string& idl_name);
	
	
private:
	std::shared_ptr<boost::dll::shared_library> mod;
	std::shared_ptr<boost::dll::detail::import_type<void(const char*, uint32_t, const char*, uint32_t, char**, uint32_t*, char**, uint32_t*)>::type> pparse_idl;
	std::shared_ptr<boost::dll::detail::import_type<void(void)>::type> pinit_plugin;
	
protected:
	idl_plugin_interface_wrapper() = default;
	
public:
	explicit idl_plugin_interface_wrapper(const std::string& idl_extension);

	void init() override;

	virtual void parse_idl(const std::string& source_code,
	               const std::string& file_or_dir_path,
	               std::string& out_idl_def_json,
	               std::string& out_err);
	
	/**
	 * Compile IDL to code called from XLLR to the foreign function
	 */
	void parse_idl(const char* source_code, uint32_t source_code_length,
	               const char* file_or_dir_path, uint32_t file_or_dir_path_length,
	               char** out_idl_def_json, uint32_t* out_idl_def_json_length,
	               char** out_err, uint32_t* out_err_len) override;
};
//--------------------------------------------------------------------
class json_idl_plugin : public idl_plugin_interface_wrapper
{
public:
	explicit json_idl_plugin() = default;
	void init() override{}
	
	void parse_idl(const std::string& source_code,
	               const std::string& file_or_dir_path,
	               std::string& out_idl_def_json,
	               std::string& out_err) override
	{
		if(!source_code.empty())
		{
			out_idl_def_json = source_code;
			out_err.clear();
		}
		else
		{
			std::ifstream file(file_or_dir_path);
			out_idl_def_json = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			out_err.clear();
		}
	}
	
	void parse_idl(const char* source_code, uint32_t source_code_length,
	               const char* file_or_dir_path, uint32_t file_or_dir_path_length,
	               char** out_idl_def_json, uint32_t* out_idl_def_json_length,
	               char** out_err, uint32_t* out_err_len) override
	{
		std::string idl_def_json;
		std::string err;

		parse_idl(std::string(source_code, source_code_length),
		          std::string(file_or_dir_path, file_or_dir_path_length),
		          idl_def_json,
		          err);

		if(!err.empty())
		{
			*out_err = (char*)calloc(1, err.length()+1);
			err.copy(*out_err, err.length());
			*out_err_len = err.length();
			*out_idl_def_json_length = 0;
		}
		else
		{
			*out_idl_def_json = (char*)calloc(1, idl_def_json.length()+1);
			idl_def_json.copy(*out_idl_def_json, idl_def_json.length());
			*out_idl_def_json_length = idl_def_json.length();
			*out_err_len = 0;
		}
	}
};
//--------------------------------------------------------------------