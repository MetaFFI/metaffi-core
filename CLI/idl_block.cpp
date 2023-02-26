#include "idl_block.h"
#include "idl_plugin_interface_wrapper.h"
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <numeric>
#include <regex>
#include <iostream>
#include <string>
#include <utility>
#include <unordered_map>

std::unordered_map<std::string, std::shared_ptr<idl_plugin_interface_wrapper>> idl_plugins;

//--------------------------------------------------------------------
idl_block::idl_block(std::string code_extension, std::string file_code, std::string filename):_file_code(std::move(file_code)), _filename(std::move(filename))
{
	boost::to_lower(code_extension);
	this->_code_extension = code_extension;
}
//--------------------------------------------------------------------
void idl_block::compile_metaffi_idl()
{
	// if .JSON file, it is a metaffi IDL
	if (this->_code_extension == ".json") // if not metaffi IDL - load relevant plugin to compile metaffi IDL
	{
		this->_metaffi_json_idl = _file_code;
		return;
	}
	
	std::shared_ptr<idl_plugin_interface_wrapper> idl;
	auto idl_plugin_it = idl_plugins.find(this->_code_extension);
	if(idl_plugin_it == idl_plugins.end())
	{
		idl = std::make_shared<idl_plugin_interface_wrapper>(this->_code_extension);
		idl->init();
		idl_plugins[this->_code_extension] = idl;
	}
	else
	{
		idl = idl_plugin_it->second;
	}
	
	// Use IDL plugin to compile MetaFFI IDL
	
	char* err = nullptr;
	uint32_t err_len = 0;
	char* out_idl_def_json;
	uint32_t out_idl_def_json_length;
	
	std::string filename = _block_name.empty() ? this->_filename : this->_filename + "#" + this->_block_name;
	
	idl->parse_idl(filename.c_str(), filename.length(),
	               this->_file_code.c_str(), this->_file_code.length(),
	               &out_idl_def_json, &out_idl_def_json_length,
	               &err, &err_len);
	if (err)
	{
		throw std::runtime_error(std::string(err, err_len));
	}
	
	this->_metaffi_json_idl = std::string(out_idl_def_json, out_idl_def_json_length);
}
//--------------------------------------------------------------------
idl_block idl_block::parse_block(std::string block, std::string filename)
{
	boost::replace_all(block, "\r", "");
	
	std::vector<std::string> lines;
	boost::split(lines, block, boost::is_any_of("\n"));
	
	// parse block header
	std::string comment_candidate;
	idl_block b = parse_header(lines[0], lines, comment_candidate);
	b._filename = std::move(filename);
	
	// strip comment symbols
	// if comment symbols not defined - try to detect it (if single line comment is used)
	if(b._single_line_comment.empty() && !comment_candidate.empty())
	{
		bool single_line_candidate_is_correct = true;
		for(std::string line : lines)
		{
			boost::trim(line);
			if(!boost::starts_with(line, comment_candidate))
			{
				single_line_candidate_is_correct = false;
				break; // candidate is NOT single line comment
			}
		}
		
		if(single_line_candidate_is_correct){
			b._single_line_comment = comment_candidate;
		}
	}
	
	// remove first and last line
	lines.erase(lines.begin(), lines.begin()+1);
	lines.erase(lines.end()-1, lines.end());
	
	// place source in idl_block
	b._file_code = boost::algorithm::join(lines, "\n");
	
	
	return b;
}
//--------------------------------------------------------------------
idl_block idl_block::parse_header(const std::string& header_string, const std::vector<std::string>& lines, std::string& out_comment_candidate)
{
	idl_block b;
	
	size_t header_sig_it = header_string.find(MXSTR(METAFFI_BLOCK_START));
	out_comment_candidate = header_string.substr(0, header_string.find(MXSTR(METAFFI_BLOCK_START)));
	
	// get comment candidate
	boost::trim(out_comment_candidate);
	
	// get header sig
	std::string header_sig = header_string.substr(header_sig_it, header_string.length()-header_sig_it);
	boost::replace_all(header_sig, MXSTR(METAFFI_BLOCK_START), "");
	boost::trim(header_sig);
	
	std::vector<std::string> sig_pairs;
	boost::split(sig_pairs, header_sig, boost::is_any_of(","));
	
	// iterate pairs
	for(auto& sig_pair : sig_pairs)
	{
		boost::trim(sig_pair);
		
		std::vector<std::string> elements;
		boost::split(elements, sig_pair, boost::is_any_of("="));
		
		if(elements.size() != 2)
		{
			std::stringstream ss;
			ss << "Header signature of block is not valid: " << sig_pair;
			throw std::runtime_error("Header signature of block is not valid");
		}
		
		boost::trim(elements[0]);
		boost::trim(elements[1]);
		boost::to_lower(elements[0]);
		
		if(elements[0] == "name")
		{
			b._block_name = elements[1];
			
			boost::filesystem::path idl_fs_path(elements[1]);
			if(idl_fs_path.extension().empty()){
				throw std::runtime_error("Extension in block name is missing");
			}
			
			b._code_extension = idl_fs_path.extension().string();
		}
		else if(elements[0] == "comment")
		{
			b._single_line_comment = elements[1];
		}
		else
		{
			std::stringstream ss;
			ss << "Unknown MetaFFI block tag: " << elements[0];
			throw std::runtime_error(ss.str());
		}
	}
	
	return b;
}
//--------------------------------------------------------------------
std::string idl_block::get_target_language() const
{
	// get target language
	std::regex target_lang_regex(R"(target_language"[ ]*:[ ]*"([^\"]+))");
	std::smatch matches;
	if(!std::regex_search(this->_metaffi_json_idl, matches, target_lang_regex))
	{
		throw std::runtime_error("MetaFFI IDL does not contains metaffi_target_language tag");
	}
	
	if(matches.size() < 2)
	{
		throw std::runtime_error("metaffi_target_language tag does not contain target language");
	}
	
	return matches[1].str();
}
//--------------------------------------------------------------------
bool idl_block::is_embedded() const
{
	return !this->_block_name.empty();
}
//--------------------------------------------------------------------