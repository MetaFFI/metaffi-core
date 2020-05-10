#include <iostream>
#include <string>
#include "xllr_api.h"

#define reset_err \
	out_err = nullptr; \
	out_err_len = 0;

#define check_and_reset_err_success( desc ) \
	if(out_err != nullptr) \
	{ \
		std::string estr(out_err, out_err_len); \
		std::cout << desc << ": " << estr << std::endl; \
		exit(1); \
	}

#define check_and_reset_err_fail( desc ) \
	if(out_err == nullptr) \
	{ \
		std::cout << desc << ": did not fail, although test expected to fail" << std::endl; \
		exit(1); \
	}

#define run_test_step_expect_success( desc, code ) \
	reset_err; \
	code; \
	check_and_reset_err_success(desc);

#define run_test_step_expect_fail( desc, code ) \
	reset_err; \
	code; \
	check_and_reset_err_fail(desc);

//--------------------------------------------------------------------

std::string plugin_name("xllr.example.so");
std::string module_name("example_module");
char* out_err = nullptr;
uint32_t out_err_len = 0;

//--------------------------------------------------------------------
void test_runtime_success()
{
	// Test 1 - load runtime + free runtime, make sure it releases successfully
	run_test_step_expect_success("Error in load_runtime_plugin()",
				 load_runtime_plugin(plugin_name.c_str(), plugin_name.length(), &out_err, &out_err_len)
	);

	run_test_step_expect_success("Error in free_runtime_plugin()",
				 free_runtime_plugin(plugin_name.c_str(), plugin_name.length(), &out_err, &out_err_len)
	);
}
//--------------------------------------------------------------------
void test_runtime_fail()
{
	// Test 2 - load runtime plugin that doesn't exist - expect failure
	run_test_step_expect_fail("Error in load_runtime_plugin()",
				 load_runtime_plugin("noexist", 7, &out_err, &out_err_len)
	);
}
//--------------------------------------------------------------------
void test_module_success()
{
	// Test 3 - load runtime + load module + free runtime
	run_test_step_expect_success("Error in load_runtime_plugin()",
				 load_runtime_plugin(plugin_name.c_str(), plugin_name.length(), &out_err, &out_err_len)
	);

	run_test_step_expect_success("Error in load_module()",
				 load_module(plugin_name.c_str(), plugin_name.length(), module_name.c_str(), module_name.length(), &out_err, &out_err_len)
	);

	run_test_step_expect_success("Error in free_module()",
				 free_module(plugin_name.c_str(), plugin_name.length(), module_name.c_str(), module_name.length(), &out_err, &out_err_len)
	);

	run_test_step_expect_success("Error in free_runtime_plugin()",
				 free_runtime_plugin(plugin_name.c_str(), plugin_name.length(), &out_err, &out_err_len)
	);
}
//--------------------------------------------------------------------
void test_module_free_module_via_free_runtime_success()
{
	// Test 4 - load runtime + load module + free runtime
	run_test_step_expect_success("Error in load_runtime_plugin()",
				 load_runtime_plugin(plugin_name.c_str(), plugin_name.length(), &out_err, &out_err_len)
	);

	run_test_step_expect_success("Error in load_module()",
				 load_module(plugin_name.c_str(), plugin_name.length(), module_name.c_str(), module_name.length(), &out_err, &out_err_len)
	);

	run_test_step_expect_success("Error in free_runtime_plugin()",
				 free_runtime_plugin(plugin_name.c_str(), plugin_name.length(), &out_err, &out_err_len)
	);
}
//--------------------------------------------------------------------
void test_module_lazy_runtime_success()
{
	// Test 5 - load module + free runtime - lazy loading of runtime
	run_test_step_expect_success("Error in load_module()",
				 load_module(plugin_name.c_str(), plugin_name.length(), module_name.c_str(), module_name.length(), &out_err, &out_err_len)
	);

	run_test_step_expect_success("Error in free_runtime_plugin()",
				 free_runtime_plugin(plugin_name.c_str(), plugin_name.length(), &out_err, &out_err_len)
	);
}
//--------------------------------------------------------------------
void test_module_module_not_exist_fail()
{
	// Test 6 - load module that doesn't exist
	run_test_step_expect_fail("Error in load_runtime_plugin()",
				 load_module(plugin_name.c_str(), plugin_name.length(), "not_exist", 9, &out_err, &out_err_len)
	);
}
//--------------------------------------------------------------------
int main(int argc, char* argv[])
{
	/*
	if(argc <= 1)
	{
		std::cout << "Expect test case number. Quitting..." << std::endl;
		return 1;
	}

	int testcase = 0;

	if(sscanf(argv[1], "%d", &testcase) != 1)
	{
		std::cout << "Given argument is not a test case number. Quitting..." << std::endl;
		return 1;
	}*/
	int testcase = 5;

	switch (testcase)
	{
		case 1:
		{
			test_runtime_success();
		}
		break;

		case 2:
		{
			test_runtime_fail();
		}
		break;

		case 3:
		{
			test_module_success();
		}
		break;

		case 4:
		{
			test_module_free_module_via_free_runtime_success();
		}
		break;

		case 5:
		{
			test_module_lazy_runtime_success();
		}break;

		case 6:
		{
			test_module_module_not_exist_fail();
		}break;

		default:
			std::cout << "Given test case number does not exist: " << testcase << std::endl;
			return 1;
	}
	

	

	// Test 4 - load runtime + load module + free runtime, make sure module released successfully

	// Test 5 - load runtime + load module + release module + free runtime, make sure module released successfully

	// Test 6 - load runtime + load module + call function + free runtime, make sure function called

	// Test 7 - call function + free runtime - make sure everything is loaded and freed at the end

	return 0;
}