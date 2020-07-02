/*
 * This file contains tests requiring all OpenFFI modules
 */
#include <iostream>
#include <string>
#include <string.h>
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
void test_python()
{
	// write test.py foreign functions in python to file
	std::string test_py(R"(
import math

def add(x, y):
	z = x+y
	print("{}+{}={}".format(x, y, z))

	return z
	
def upper(input): # input must be str (otherwise - error)
	output = input.upper()
	print(input+" --> "+output)

	return output

def sqrt(input): # input must be a positive number (otherwise - error)
	output = math.sqrt( input )
	print("root of "+str(input)+" = "+str(output))

	return output

def hello_world():
	print('Hello World')
)");
	
	// write test.proto
	
	// compile test.proto to test_openffi_guest.py
	
	// compile test.proto to test_pb2.py
	
	// call using XLLR to to foreign functions in test.py
	
}
//--------------------------------------------------------------------
int main(int argc, char* argv[])
{
	if(argc <= 1)
	{
		std::cout << "Expect test name. Quitting..." << std::endl;
		return 1;
	}
	
	std::string chosen_test(argv[1]);
	
	if (chosen_test == "test_python")
	{
		test_python();
	}
	else
	{
		std::cout << "Given test name does not exist: " << chosen_test << std::endl;
		return 1;
	}
	
	return 0;
}