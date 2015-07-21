/*
 * compiler.h
 *
 * Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
 * All rights reserved
 *
 */

#ifndef MP3_COMPILER_H
#define MP3_COMPILER_H

#include <string.h>
 
#if defined(__C30__) || defined(__XC16__)
	/*
	// fast memcpy and memmove implementations
	*/
	void memcpy_fast(void *dst, void const*src, size_t size);
	void memmove_fast(void *dst, void const *src, size_t size);
	#define memcpy	memcpy_fast
	#define memmove memmove_fast
	/*
	// debug symbols
	*/
	#if !defined(__DEBUG)
	#define NDEBUG
	#endif
	/* #define inline __attribute__((always_inline)) */
 	#define PTR_M
	#define ROM
	#define HALT()	__asm__ volatile (".pword 0xDA4000\nnop\nnop\nnop")
	#if !defined(_ASSERT)
		#if defined(NDEBUG)
			#define _ASSERT(c)
		#else
			#define _ASSERT(c)	if (!(c)) HALT()
		#endif
	#endif	
	typedef unsigned int uintptr_t;
#else
 	#define inline __inline
 	#define PTR_M
	#define ROM

	#if defined(_MSC_VER)
		//#define _W64
		//#define int	short
		//#define long __int32
		typedef unsigned int uintptr_t ;
	#else
		/* todo: add define for 32 bits */
		typedef unsigned long uintptr_t ;
	#endif


	#if !defined(_ASSERT)
		#if defined(__XC32__)
			#define _ASSERT(c) if (!(c)) while (1);
		#else
			#include <assert.h>
			#define _ASSERT(c)	assert((c))
		#endif
	#endif	
	
#endif

#endif
