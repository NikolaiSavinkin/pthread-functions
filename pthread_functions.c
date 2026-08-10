#include "pthread_functions.h"

int ptf_map(
		void* out_array,
		const void* in_array,
		const size_t array_len,
		const size_t elem_size,
		ptf_transform_t transform,
		const size_t pool_size
) {

	if (
			out_array == NULL
			|| in_array == NULL
			|| array_len == 0
			|| elem_size == 0
			|| transform == NULL
			|| pool_size < 1
			|| array_len < pool_size
	   ) {
		return -1;
	}

	uint8_t* out_bytes = (uint8_t*)out_array;
	const uint8_t* in_bytes = (const uint8_t*)in_array;

	for (size_t i = 0; i < array_len; i++) {
		transform(out_bytes + (i * elem_size), in_bytes + (i * elem_size));
	}
	
	return 0;
}

int ptf_fold(
		void* acc,
		const void* in_array,
		const size_t array_len,
		const size_t elem_size,
		ptf_reducer_t reduce,
		const size_t pool_size
) {

	if (
			acc == NULL
			|| in_array == NULL
			|| array_len == 0
			|| elem_size == 0
			|| reduce == NULL
			|| pool_size < 1
			|| array_len < pool_size
	   ) {
		return -1;
	}

	const uint8_t* in_bytes = (const uint8_t*)in_array;

	for (size_t i = 0; i < array_len; i++){
		reduce(acc, in_bytes + (i * elem_size));
	}

	return 0;
}


int ptf_filter(
		void* out_array,
		size_t* out_len,
		const void* in_array,
		const size_t array_len,
		const size_t elem_size,
		ptf_predicate_t predicate,
		const size_t pool_size
) {

	if (
			out_array == NULL
			|| out_len == NULL
			|| in_array == NULL
			|| array_len == 0
			|| elem_size == 0
			|| predicate == NULL
			|| pool_size < 1
			|| array_len < pool_size
	   ) {
		return -1;
	}

	uint8_t* out_bytes = (uint8_t*)out_array;
	*out_len = 0;
	const uint8_t* in_bytes = (const uint8_t*)in_array;

	for( size_t i = 0; i < array_len; i++) {
		const uint8_t* elem = in_bytes + ( i * elem_size );

		if (predicate(in_bytes + ( i * elem_size ))) {
			memcpy(
					out_bytes + ( *out_len * elem_size ),
					elem,
					elem_size
			);
			*out_len = *out_len + 1;
		}
	}

	return 0;
}
