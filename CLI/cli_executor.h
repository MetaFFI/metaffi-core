#include <boost/program_options.hpp>

//--------------------------------------------------------------------
class cli_executor
{
private:
	int _argc;
	char** _argv;
	bool _help_requested = false;

	boost::program_options::options_description _metaffi_options;
	boost::program_options::options_description _compile_options;
	boost::program_options::options_description _plugin_options;
	boost::program_options::options_description _pack_options;

	boost::program_options::variables_map vm;

public:
	cli_executor(int argc, char** argv);
	~cli_executor() = default;
	cli_executor(const cli_executor&) = delete;
	cli_executor(const cli_executor&&) = delete;

	bool parse(void);
	bool help_requested() const { return _help_requested; }

private:
	bool compile(void);
	bool plugin(void);
};
//--------------------------------------------------------------------
