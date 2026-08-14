#include "pthread_functions.h"

/*
 * Argument structure for passing to pthreads
 */
typedef struct {
	uint8_t* out_bytes;
	const uint8_t* in_bytes;
	size_t array_len;
	const size_t elem_size;
	ptf_transform_t transform;
} _ptf_map_t;


static void* _ptf_map1( void* arg ) {
	_ptf_map_t* args = (_ptf_map_t*)arg;

	for (size_t i = 0; i < args->array_len; i++) {
		args->transform(
			args->out_bytes + (i * args->elem_size),
			args->in_bytes + (i * args->elem_size)
		);
	}
	return NULL;
}

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

	_ptf_map_t arg = {
		.out_bytes = out_bytes,
		.in_bytes = in_bytes,
		.array_len = array_len,
		.elem_size = elem_size,
		.transform = transform
	};

	if (pool_size == 1) {
		_ptf_map1((void*)&arg);
		return 0;
	}

	/* 
	 * Fist batch is a special case to handle the remainder
	 */
	size_t batch_size = array_len % pool_size;
	if ( batch_size == 0 ) {
		batch_size = array_len / pool_size;
	}

	arg.array_len = batch_size;
	pthread_t pool[pool_size];

	if (0 != pthread_create(&pool[0], NULL, _ptf_map1, (void*)&arg)) {
		return -1;
	}
	
	/*
	 * The rest of the batches are all equal in size
	 */
	batch_size = array_len / pool_size;
	arg.array_len = batch_size;

	for (size_t t = 1 /* skip first */; t < pool_size; t++) {
		arg.out_bytes = arg.out_bytes + (t * batch_size * elem_size);
		arg.in_bytes = arg.in_bytes + (t * batch_size * elem_size);

		if (0 != pthread_create(&pool[t], NULL, _ptf_map1, (void*)&arg)) {
			// TODO: cancell all running threads
			return -1;
		}
	}

	for (size_t t = 0; t < pool_size; t++) {
		if (0 != pthread_join(pool[t], NULL)) {
			// TODO: cancell all running threads
			return -1;
		}
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
