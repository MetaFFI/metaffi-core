#pragma once
#include <string>
#include <vector>

#define METAFFI_BLOCK_START metaffi-block:
#define METAFFI_BLOCK_END metaffi-end
#define MXSTR(a) MSTR(a)
#define MSTR(a) #a

class idl_block
{
public:
	static idl_block parse_block(std::string block, std::string filename);

private:
	static idl_block parse_header(const std::string& header_string, const std::vector<std::string>& lines, std::string& out_comment_candidate);
	
public:
	std::string _filename;
	std::string _block_name;
	std::string _code_extension;
	std::string _file_code;
	std::string _metaffi_json_idl;
	std::string _single_line_comment;
	
public:
	idl_block(std::string code_extension, std::string file_code, std::string filename);
	std::string get_target_language() const;

private:
	idl_block() = default;
	
public:
	// fills metaffi_json_idl
	void compile_metaffi_idl();
	
	bool is_embedded() const;
};

