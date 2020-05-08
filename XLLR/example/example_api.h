#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

	/**
	 * Load runtime runtime of foreign runtime
	 */ 
	void load_runtime(char** err, uint32_t* err_len);

	/**
	 * Free runtime runtime of foreign runtime
	 */ 
	void free_runtime(char** err, uint32_t* err_len);

#ifdef __cplusplus
extern "C"
}
#endif