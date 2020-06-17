#include <iostream>
#include <string>
#include <string.h>
#include "example_api.h"
/*
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
std::string func_name("example_function");
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
void test_module_not_exist_fail()
{
	// Test 6 - load module that doesn't exist
	run_test_step_expect_fail("Error in load_runtime_plugin()",
				 load_module(plugin_name.c_str(), plugin_name.length(), "not_exist", 9, &out_err, &out_err_len)
	);
}
//--------------------------------------------------------------------
void test_call_success()
{
	std::string params("in params");
	unsigned char* out_params = nullptr;
	uint64_t out_params_len = 0;
	unsigned  char* out_ret = nullptr;
	uint64_t out_ret_len = 0;
	uint8_t is_error = 0;

	// Test 6 - load module that doesn't exist
	call(plugin_name.c_str(), plugin_name.length(),
		module_name.c_str(), module_name.length(),
		func_name.c_str(), func_name.length(),
		(unsigned char*)params.c_str(), params.length(),
		&out_params, &out_params_len,
		&out_ret, &out_ret_len,
		&is_error
	);

	if(is_error)
	{
		std::string estr((const char*)out_ret, out_ret_len);
		std::cout << "returned error from call(): " << estr << std::endl;
		exit(1);
	}

	if(out_params == nullptr)
	{
		std::cout << "out_params is empty. expected to have a value" << std::endl;
		exit(1);
	}

	if(out_ret == nullptr)
	{
		std::cout << "out_ret is empty. expected to have a value" << std::endl;
		exit(1);
	}

	if(strlen((const char*)out_params) != out_params_len || strcmp((const char*)out_params, "out params") != 0)
	{
		std::cout << "out_params value OR length is not as expected: " << out_params << ". Expected: out params" << std::endl;
		exit(1);
	}

	if(strlen((const char*)out_ret) != out_ret_len || strcmp((const char*)out_ret, "ret params") != 0)
	{
		std::cout << "out_ret value OR length is not as expected: " << out_ret << ". Expected: ret params" << std::endl;
		exit(1);
	}

	std::cout << "called \"call()\" successfully in XLLR Example plugin" << std::endl;

	free(out_ret);
	free(out_params);

}
//--------------------------------------------------------------------
void test_call_fail()
{
	std::string params("in params");
	unsigned char* out_params = nullptr;
	uint64_t out_params_len = 0;
	unsigned char* out_ret = nullptr;
	uint64_t out_ret_len = 0;
	uint8_t is_error = 0;

	// Test 6 - load module that doesn't exist
	call(plugin_name.c_str(), plugin_name.length(),
		module_name.c_str(), module_name.length(),
		"dummy", 5,
		(unsigned char*)params.c_str(), params.length(),
		&out_params, &out_params_len,
		&out_ret, &out_ret_len,
		&is_error
	);

	if(!is_error)
	{
		std::cout << "\"call()\" did not fail, although it was expected to." << std::endl;
		exit(1);
	}

}
//--------------------------------------------------------------------
int main(int argc, char* argv[])
{
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
	}

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
			test_module_not_exist_fail();
		}break;

		case 7:
		{
			test_call_success();
		}break;

		case 8:
		{
			test_call_fail();
		}break;

		default:
			std::cout << "Given test case number does not exist: " << testcase << std::endl;
			return 1;
	}
	
	return 0;
}
*/