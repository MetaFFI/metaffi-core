#include "xcall_jit.h"


int main(int argc, char* argv[])
{
	if(init_jit(argc, argv))
	{
		printf("Error\n");
		return 1;
	}
	
	int64_t p1 = 10;
	pforeign_function_entrypoint_signature_params_ret_t f1 = create_xcall_params_ret({sizeof(p1)}, {(int64_t)p1}, "test1",
						(void*)(void(*)(cdts[2], char**, uint64_t*, uint64_t))([](cdts pcdts[2], char** out_err, uint64_t* out_err_len, uint64_t lp1)
						{
							printf("%p %p %p %ld\n", pcdts, out_err, out_err_len, lp1);
							
							if(pcdts != (cdts*)1 || out_err != (char**)2 || out_err_len != (uint64_t*)3 || lp1 != 10)
							{
								throw std::runtime_error("Parameters of lambda are not as expected");
							}
						}));
	
	
	
	int64_t p2 = 100;
	pforeign_function_entrypoint_signature_params_ret_t f2 = create_xcall_params_ret({sizeof(p1), sizeof(p2)}, {p1, p2}, "test2",
                         (void*)(void(*)(cdts[2], char**, uint64_t*, int64_t, int64_t))([](cdts pcdts[2], char** out_err, uint64_t* out_err_len, int64_t lp1, int64_t lp2)
                         {
	                         printf("%p %p %p %ld %ld\n", pcdts, out_err, out_err_len, lp1, lp2);
							 
                             if(pcdts != (cdts*)10 || out_err != (char**)20 || out_err_len != (uint64_t*)30 || lp1 != 10 || lp2 != 100)
                             {
                                 throw std::runtime_error("Parameters of lambda are not as expected");
                             }
                         }));
	
	try
	{
		f1((cdts*)1, (char**)2, (uint64_t*)3);
		f2((cdts*)10, (char**)20, (uint64_t*)30);
	}
	catch(const std::runtime_error& e)
	{
		printf("%s\n", e.what());
		return 1;
	}
	
	return 0;
}