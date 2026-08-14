#include <stdio.h>
#include <stdlib.h>

#include "pthread_functions.h"

static void double_float(void* out, const void* element) {
	*(float*)out = *(const float*)element * 2.0f;
}

static void print_float(void* _, const void* element) {
	printf("%f\n", *(float*)element);
}

int main() {
	const float in_array[10] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5};
	const size_t array_len = 10;
	const size_t elem_size = sizeof(float);
	const size_t pool_size = 2;
	float* out_array = calloc(array_len, elem_size);

	ptf_map(
		(void*)out_array,
		(const void*)in_array,
		array_len,
		elem_size,
		double_float,
		pool_size
	);

	float* _new_out_array = calloc(array_len, elem_size);
	const float* result = out_array;

	ptf_map(
		(void*)_new_out_array,
		(const void*)result,
		array_len,
		elem_size,
		print_float,
		1
	);
	
	return 0;
}
