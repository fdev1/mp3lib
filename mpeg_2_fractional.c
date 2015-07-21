/*
 * mpeg_2_fractional.c
 *
 * Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
 * All rights reserved
 *
 */

#include "mpeg_2_fractional.h"

#if (defined(__XC16__) || defined(__C30__))
	 mp3_frac __result __attribute__((__near__, section(".mp3")));
	 /* long acc; */
#elif defined(__XC32__) || defined(__GNUC__)
	long acc;
#elif defined(_MSC_VER)
	__int64 acc;
#endif

