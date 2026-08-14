/**
 *******************************************************************************
 * @file    pthread_functions.h
 * @author  Nikolai Savinkin (nikolai@savinkin.uk)
 * @date    2026-08-10
 * @brief
 *    A simple library for performing common functions on arrays using pthreads
 *******************************************************************************
 */

#ifndef PTHREAD_FUNCTIONS_H
#define PTHREAD_FUNCTIONS_H

#include <string.h>
#include <stdint.h>
#include <pthread.h>


/*
 * Definitions of the function types transform, reducer, predicate
 * to be passed to map, fold, and filter respectivly.
 */
typedef void (*ptf_transform_t)(void* out, const void* element);
typedef void (*ptf_reducer_t)(void* acc, const void* element);
typedef uint8_t (*ptf_predicate_t)(const void* element);

/*
 * ptf_map() - Apply the transform to each element in the array
 * 	out_array: pointer to pre-allocated space for the result of the map
 * 	in_array: pointer to array to be mapped over (must be same length as
 * 		out_array)
 * 	array_len: number of elements in the arrays
 * 	elem_size: size in bytes of each element in the arrays
 * 	transform: the function to be applied to each element in the in_array
 * 	pool_size: the number of pthreads to use
 * Returns 0 on sucess, otherwise -1
 *
 * Ensure out_array and in_array are the same size.
 *
 * The transform function should take a pointer to the output and a pointer to
 * the input and return nothing.
 *
 * See ptf_transform_t.
 */

int ptf_map(
		void* out_array,
		const void* in_array,
		const size_t array_len,
		const size_t elem_size,
		ptf_transform_t transform,
		const size_t pool_size
);

/*
 * ptf-fold() - Combine elements of the array into one datastructure using the
 * reducer function
 * 	acc: pointer to accumulating datastructure used to combine each
 * 		element into
 * 	in_array: pointer to array to be folded over
 * 	array_len: number of elements in the array
 * 	elem_size: size in bytes of each element in the array
 * 	reducer: the function to be used to combine the elements of the array
 * 	pool_size: the number of pthreads to use
 * Returns 0 on success, otherwise -1
 *
 * It is the consumers job to manage the memory of `acc`, including inside the
 * reducer they provide.
 *
 * The reducer function MUST have the associative property for pool_size > 1.
 * Without this, the result will be incorrect.
 *
 * See ptf_reducer_t
 */

int ptf_fold(
		void* acc,
		const void* in_array,
		const size_t array_len,
		const size_t elem_size,
		ptf_reducer_t reduce,
		const size_t pool_size
);

/*
 * ptf_filter() - Create a new array only containing elements that evaluate to
 * true with the given predicate
 * 	out_array: pointer to pre-allocated space for the result of the filter
 * 	out_len: pointer to number of elements that passed the filter
 * 	in_array: pointer to array to be filtered (must be same length as
 * 		out_array)
 * 	array_len: number of elements in the arrays
 * 	elem_size: size in bytes of each element in the arrays
 * 	predicate: the function to be applied to each element in the in_array
 * 	pool_size: the number of pthreads to use
 * Returns 0 on sucess, otherwise -1
 *
 * Ensure out_array and in_array are the same size initially. Feel free to use
 * out_len to realloc the out_array afterwards.
 *
 * The transform function should take a pointer to the output and a pointer to
 * the input and return nothing.
 *
 * See ptf_transform_t.
 */

int ptf_filter(
		void* out_array,
		size_t* out_len,
		const void* in_array,
		const size_t array_len,
		const size_t elem_size,
		ptf_predicate_t predicate,
		const size_t pool_size
);

#endif /* PTHREAD_FUNCTIONS_H */
