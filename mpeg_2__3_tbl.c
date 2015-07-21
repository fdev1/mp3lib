/*
 * mpeg_2__3_tbl.c
 *
 * Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
 * All rights reserved
 *
 */

#include "compiler.h"
#include "mpeg_2__3.h"
#include "mpeg_2_fractional.h"

#if defined(__XC16__) || defined(__C30__)
	#if 1 || !defined(NDEBUG)
		#define MP3_SECTION_CA_CS			".mp3_ca_cs"
		#define MP3_SECTION_IMDCT_S			".mp3_imdct_s"
		#define MP3_SECTION_IMDCT_L			".mp3_imdct_l"
		#define MP3_SECTION_WINDOW_L		".mp3_window_l"
		#define MP3_SECTION_WINDOW_S		".mp3_window_s"
	#else
		#define MP3_SECTION_CA_CS			".mp3_psv0"
		#define MP3_SECTION_IMDCT_S			".mp3_psv0"
		#define MP3_SECTION_IMDCT_L			".mp3_psv0"
		#define MP3_SECTION_WINDOW_L		".mp3_psv0"
		#define MP3_SECTION_WINDOW_S		".mp3_psv0"
	#endif
#endif

mp3_sflen const sflen_table[16] = 
{
	{ 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 },
	{ 3, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 },
	{ 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 1 },
	{ 3, 2 }, { 3, 3 }, { 4, 2 }, { 4, 3 }
};

unsigned char const nsfb_table[6][3][4] = 
{
	{ 
		{  6,  5,  5, 5 },
    	{  9,  9,  9, 9 },
    	{  6,  9,  9, 9 } 
    },
  	{ 
  		{  6,  5,  7, 3 },
    	{  9,  9, 12, 6 },
    	{  6,  9, 12, 6 } 
    },
  	{ 
  		{ 11, 10,  0, 0 },
    	{ 18, 18,  0, 0 },
    	{ 15, 18,  0, 0 } 
    },
  	{ 
  		{  7,  7,  7, 0 },
    	{ 12, 12, 12, 0 },
    	{  6, 15, 12, 0 } 
    },
  	{ 
  		{  6,  6,  6, 3 },
    	{ 12,  9,  9, 6 },
    	{  6, 12,  9, 6 } 
    },
  	{ 
  		{  8,  8,  5, 0 },
    	{ 15, 12,  9, 0 },
    	{  6, 18,  9, 0 } 
    }
};

static unsigned char const sfb_48000_l[] = 
{
	 4,  4,  4,  4,  4,  4,  6,  6,  6,   8,  10,
	12, 16, 18, 22, 28, 34, 40, 46, 54,  54, 192
};

static unsigned char const sfb_44100_l[] = 
{
	 4,  4,  4,  4,  4,  4,  6,  6,  8,   8,  10,
	12, 16, 20, 24, 28, 34, 42, 50, 54,  76, 158
};

static unsigned char const sfb_32000_l[] = 
{
	 4,  4,  4,  4,  4,  4,  6,  6,  8,  10,  12,
	16, 20, 24, 30, 38, 46, 56, 68, 84, 102,  26
};

static unsigned char const sfb_48000_s[] = 
{
	 4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  6,
	 6,  6,  6,  6,  6, 10, 10, 10, 12, 12, 12, 14, 14,
	14, 16, 16, 16, 20, 20, 20, 26, 26, 26, 66, 66, 66
};

static unsigned char const sfb_44100_s[] = 
{
	 4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  6,
	 6,  6,  8,  8,  8, 10, 10, 10, 12, 12, 12, 14, 14,
	14, 18, 18, 18, 22, 22, 22, 30, 30, 30, 56, 56, 56
};

static unsigned char const sfb_32000_s[] = 
{
	 4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  6,
	 6,  6,  8,  8,  8, 12, 12, 12, 16, 16, 16, 20, 20,
	20, 26, 26, 26, 34, 34, 34, 42, 42, 42, 12, 12, 12
};

static unsigned char const sfb_48000_m[] = 
{
	 4,  4,  4,  4,  4,  4,  6,  6,
	 4,  4,  4,  6,  6,  6,  6,  6,  6, 10,
	10, 10, 12, 12, 12, 14, 14, 14, 16, 16,
	16, 20, 20, 20, 26, 26, 26, 66, 66, 66
};

static unsigned char const sfb_44100_m[] = 
{
	 4,  4,  4,  4,  4,  4,  6,  6,
	 4,  4,  4,  6,  6,  6,  8,  8,  8, 10,
	10, 10, 12, 12, 12, 14, 14, 14, 18, 18,
	18, 22, 22, 22, 30, 30, 30, 56, 56, 56
};

static unsigned char const sfb_32000_m[] = 
{
	 4,  4,  4,  4,  4,  4,  6,  6,
	 4,  4,  4,  6,  6,  6,  8,  8,  8, 12,
	12, 12, 16, 16, 16, 20, 20, 20, 26, 26,
	26, 34, 34, 34, 42, 42, 42, 12, 12, 12
};

static unsigned char const sfb_24000_l[] = 
{
	 6,  6,  6,  6,  6,  6,  8, 10, 12,  14,  16,
	18, 22, 26, 32, 38, 46, 54, 62, 70,  76,  36
};

static unsigned char const sfb_22050_l[] = 
{
	 6,  6,  6,  6,  6,  6,  8, 10, 12,  14,  16,
	20, 24, 28, 32, 38, 46, 52, 60, 68,  58,  54
};

static unsigned char const sfb_24000_s[] = 
{
	 4,  4,  4,  4,  4,  4,  4,  4,  4,  6,  6,  6,  8,
	 8,  8, 10, 10, 10, 12, 12, 12, 14, 14, 14, 18, 18,
	18, 24, 24, 24, 32, 32, 32, 44, 44, 44, 12, 12, 12
};

static unsigned char const sfb_22050_s[] = 
{
	 4,  4,  4,  4,  4,  4,  4,  4,  4,  6,  6,  6,  6,
	 6,  6,  8,  8,  8, 10, 10, 10, 14, 14, 14, 18, 18,
	18, 26, 26, 26, 32, 32, 32, 42, 42, 42, 18, 18, 18
};

static unsigned char const sfb_16000_s[] = 
{
	 4,  4,  4,  4,  4,  4,  4,  4,  4,  6,  6,  6,  8,
	 8,  8, 10, 10, 10, 12, 12, 12, 14, 14, 14, 18, 18,
	18, 24, 24, 24, 30, 30, 30, 40, 40, 40, 18, 18, 18
};

static unsigned char const sfb_24000_m[] = 
{
	 6,  6,  6,  6,  6,  6,  6,  6,  6,  8,  8,  8, 
	10, 10, 10, 12, 12, 12, 14, 14, 14, 18, 18, 18, 
	24, 24, 24, 32, 32, 32, 44, 44, 44, 12, 12, 12
};

static unsigned char const sfb_22050_m[] = 
{
	 6,  6,  6,  6,  6,  6, 6,  6,  6,  6,  6,  6,  
	 8,  8,  8, 10, 10, 10, 14, 14, 14, 18, 18, 18, 26, 26,
	26, 32, 32, 32, 42, 42, 42, 18, 18, 18
};

static unsigned char const sfb_16000_m[] = 
{
	 6,  6,  6,  6,  6,  6, 6,  6,  6,  8,  
	 8,  8, 10, 10, 10, 12, 12, 12, 14, 14, 
	14, 18, 18, 18, 24, 24, 24, 30, 30, 30, 
	40, 40, 40, 18, 18, 18
};

static unsigned char const sfb_8000_l[] = 
{
	12, 12, 12, 12, 12, 12, 16, 20, 24,  28,  32,
	40, 48, 56, 64, 76, 90,  2,  2,  2,   2,   2
};

static unsigned char const sfb_8000_s[] = 
{
	 8,  8,  8,  8,  8,  8,  8,  8,  8, 12, 12, 12, 16,
	16, 16, 20, 20, 20, 24, 24, 24, 28, 28, 28, 36, 36,
	36,  2,  2,  2,  2,  2,  2,  2,  2,  2, 26, 26, 26
};

static unsigned char const sfb_8000_m[] = 
{
	12, 12, 12,  4,  4,  4,  8,  8,  8, 12, 12, 12, 16, 16, 16,
	20, 20, 20, 24, 24, 24, 28, 28, 28, 36, 36, 36,  2,  2,  2,  
	 2,  2,  2,  2,  2,  2, 26, 26, 26
};

mp3_sfbwidth_tables const sfbwidth_table[9] = 
{
	{ sfb_48000_l, sfb_48000_s, sfb_48000_m },
	{ sfb_44100_l, sfb_44100_s, sfb_44100_m },
	{ sfb_32000_l, sfb_32000_s, sfb_32000_m },
	{ sfb_24000_l, sfb_24000_s, sfb_24000_m },
	{ sfb_22050_l, sfb_22050_s, sfb_22050_m },
	{ sfb_22050_l, sfb_16000_s, sfb_16000_m },
	{ sfb_22050_l, sfb_16000_s, sfb_16000_m },
	{ sfb_22050_l, sfb_16000_s, sfb_16000_m },
	{  sfb_8000_l,  sfb_8000_s,  sfb_8000_m }
};

unsigned char const mp3_pretab[22] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 3, 2, 0
};


#if defined(__XC16__) || defined(__C30__)
	#if 1 || !defined(NDEBUG)
		#define MP3_SECTION_RQ				".mp3_rq"
		#define MP3_SECTION_RQ_EXP			".mp3_rq_exp"
	#else
		#define MP3_SECTION_RQ				".mp3_psv1"
		#define MP3_SECTION_RQ_EXP			".mp3_psv1"
	#endif
#endif

#if defined(__XC16__) || defined(__C30__)
__psv__ const unsigned int mp3_rq_tbl[8207] __attribute__((section(MP3_SECTION_RQ), space(psv), address(0x10000))) =
#else
const unsigned int mp3_rq_tbl[8207] =
#endif
{
	#include "rq.dat"
};

const unsigned char mp3_rq_exp_tbl[8207] =
{
	#include "rq_exp.dat"
};

/*
// used for requantization
*/
mp3_frac const mp3_rq_powers[7] = 
{
	 0x260e,  0x2d41,  0x35d1,  0x4000,  0x4c1c,  0x5a82,  0x6ba2
};

/*
// coefficients for aliasing reduction
// derived from Table B.9 of ISO/IEC 11172-3
//
//  c[]  = { -0.6, -0.535, -0.33, -0.185, -0.095, -0.041, -0.0142, -0.0037 }
// _cs[i] =    1 / sqrt(1 + c[i]^2)
// ca[i] = c[i] / sqrt(1 + c[i]^2)
*/
#if defined(__XC16__) || defined(__C30__)
__psv__ mp3_frac const mp3__cs[9] __attribute__((section(MP3_SECTION_CA_CS), address(0xD000) , space(psv))) = 
#else
mp3_frac const mp3__cs[9] = 
#endif
{
	0x36e1, 0x386e, 0x3cc7, 0x3eef, 
	0x3fb7, 0x3ff2, 0x3ffe, 0x4000,
	0x0000
};

#if defined(__XC16__) || defined(__C30__)
__psv__ mp3_frac const mp3_ca[9] __attribute__((section(MP3_SECTION_CA_CS), address(0xD012) , space(psv))) = 
#else
mp3_frac const mp3_ca[9] = 
#endif
{
	-0x20ed, -0x1e31, -0x140e, -0x0ba4, 
	-0x060e, -0x029f, -0x00e9, -0x003d,
	 0x0000
};

/*
 * IMDCT coefficients for short blocks
 * derived from section 2.4.3.4.10.2 of ISO/IEC 11172-3
 *
 * imdct_s[i/even][k] = cos((PI / 24) * (2 *       (i / 2) + 7) * (2 * k + 1))
 * imdct_s[i /odd][k] = cos((PI / 24) * (2 * (6 + (i-1)/2) + 7) * (2 * k + 1))
 */
#if defined(__XC16__) || defined(__C30__)
mp3_frac const mp3_imdct_s[36] __attribute__((section(MP3_SECTION_IMDCT_S), address(0xD024) , space(psv))) =
#else
mp3_frac const mp3_imdct_s[36] = 
#endif
{
	 0x26f6, -0x3b21, -0x085b,  0x3f74, -0x187e, -0x32c6,
	-0x32c6,  0x187e,  0x3f74,  0x085b, -0x3b21, -0x26f6,
	 0x187e, -0x3b21,  0x3b21, -0x187e, -0x187e,  0x3b21,
	-0x3b21, -0x187e,  0x187e,  0x3b21,  0x3b21,  0x187e,
	 0x085b, -0x187e,  0x26f6, -0x32c6,  0x3b21, -0x3f74,
	-0x3f74, -0x3b21, -0x32c6, -0x26f6, -0x187e, -0x085b
};

#if defined(__XC16__) || defined(__C30__)
mp3_frac const mp3_imdct_l[188] __attribute__((section(MP3_SECTION_IMDCT_L), address(0xD06C) , space(psv))) =
#else
mp3_frac const mp3_imdct_l[188] =
#endif
{
	 0x3b21,  0x187e, -0x187e, -0x3b21,  0x085b,  0x26f6, 
	-0x32c6, -0x3f74, -0x32c6,  0x3f74,  0x085b, -0x26f6, 
	-0x3b21,  0x187e, -0x26f6,  0x085b, -0x3f74,  0x32c6, 
	 0x0dda, -0x3ff0,  0x133f,  0x35fa, -0x2f30, -0x1d8d, 
	 0x38c5, -0x2b3d, -0x2263,  0x3d0a,  0x02cb, -0x3e7c, 
	-0x3d0a,  0x2263,  0x2b3d, -0x38c5, -0x0dda,  0x3ff0, 
	-0x02cb,  0x3e7c, -0x1d8d, -0x2f30,  0x35fa,  0x133f, 
	-0x2f30, -0x1d8d,  0x3e7c, -0x02cb, -0x3d0a,  0x2263, 
	 0x35fa,  0x133f, -0x3ff0,  0x0dda,  0x38c5, -0x2b3d, 
	 0x187e, -0x3b21, -0x32c6,  0x3f74,  0x085b, -0x26f6, 
	 0x3ff0,  0x0dda, -0x38c5, -0x2b3d,  0x2263,  0x3d0a, 
	 0x133f, -0x35fa, -0x2f30,  0x1d8d,  0x3e7c,  0x02cb, 
	 0x2b3d,  0x38c5, -0x0dda, -0x3ff0, -0x133f,  0x35fa, 
	-0x2263, -0x3d0a,  0x02cb,  0x3e7c,  0x1d8d, -0x2f30, 
	-0x3e7c, -0x02cb,  0x3d0a,  0x2263, -0x2b3d, -0x38c5, 
	-0x1d8d,  0x2f30,  0x35fa, -0x133f, -0x3ff0, -0x0dda, 
	-0x085b, -0x26f6,  0x32c6,  0x3f74,  0x02cb, -0x133f, 
	 0x2263, -0x2f30,  0x38c5, -0x3e7c,  0x0dda, -0x1d8d, 
	 0x2b3d, -0x35fa,  0x3d0a, -0x3ff0, -0x38c5,  0x3e7c, 
	-0x3ff0,  0x3d0a, -0x35fa,  0x2b3d,  0x2f30, -0x2263, 
	 0x133f, -0x02cb, -0x0dda,  0x1d8d, -0x35fa,  0x2b3d, 
	-0x1d8d,  0x0dda,  0x02cb, -0x133f,  0x3d0a, -0x3ff0, 
	 0x3e7c, -0x38c5,  0x2f30, -0x2263, -0x3b21,  0x187e, 
	 0x187e,  0x3b21,  0x26f6, -0x085b,  0x3f74, -0x32c6, 
	-0x3f74, -0x32c6, -0x26f6, -0x085b, -0x3f74, -0x32c6, 
	-0x26f6, -0x085b,  0x2263,  0x2f30,  0x38c5,  0x3e7c, 
	 0x3ff0,  0x3d0a,  0x133f,  0x02cb, -0x0dda, -0x1d8d, 
	-0x2b3d, -0x35fa,  0x1d8d,  0x0dda, -0x02cb, -0x133f, 
	-0x2263, -0x2f30,  0x2b3d,  0x35fa,  0x3d0a,  0x3ff0, 
	 0x3e7c,  0x38c5, -0x3ff0, -0x3d0a, -0x35fa, -0x2b3d, 
	-0x1d8d, -0x0dda, -0x3e7c, -0x38c5, -0x2f30, -0x2263, 
	-0x133f, -0x02cb
};

/*
 * windowing coefficients for long blocks
 * derived from section 2.4.3.4.10.3 of ISO/IEC 11172-3
 *
 * mp3_window_l[i] = sin((PI / 36) * (i + 1/2))
 */
#if defined(__XC16__) || defined(__C30__)
mp3_frac const mp3_window_l[36] __attribute__((section(MP3_SECTION_WINDOW_L), address(0xD1E4) , space(psv))) = 
#else
mp3_frac const mp3_window_l[36] = 
#endif
{
	 0x02cb,  0x085b,  0x0dda,  0x133f, 
	 0x187e,  0x1d8d,  0x2263,  0x26f6, 
	 0x2b3d,  0x2f30,  0x32c6,  0x35fa, 
	 0x38c5,  0x3b21,  0x3d0a,  0x3e7c, 
	 0x3f74,  0x3ff0,  0x3ff0,  0x3f74, 
	 0x3e7c,  0x3d0a,  0x3b21,  0x38c5, 
	 0x35fa,  0x32c6,  0x2f30,  0x2b3d, 
	 0x26f6,  0x2263,  0x1d8d,  0x187e, 
	 0x133f,  0x0dda,  0x085b,  0x02cb
};
 
/*
 * windowing coefficients for short blocks
 * derived from section 2.4.3.4.10.3 of ISO/IEC 11172-3
 *
 * mp3_window_s[i] = sin((PI / 12) * (i + 1/2))
 */
#if defined(__XC16__) || defined(__C30__)
mp3_frac const mp3_window_s[12] __attribute__((section(MP3_SECTION_WINDOW_S), address(0xD22C) , space(psv))) = 
#else
mp3_frac const mp3_window_s[12] = 
#endif
{
	 0x085b,  0x187e,  0x26f6,  0x32c6, 
	 0x3b21,  0x3f74,  0x3f74,  0x3b21, 
	 0x32c6,  0x26f6,  0x187e,  0x085b
};

/*
 * coefficients for intensity stereo processing
 * derived from section 2.4.3.4.9.3 of ISO/IEC 11172-3
 *
 * is_ratio[i] = tan(i * (PI / 12))
 * is_table[i] = is_ratio[i] / (1 + is_ratio[i])
 */
mp3_frac const is_table[7] = 
{
	 0x0000, 
	 0x0d86, 
	 0x176d, 
	 0x2000, 
	 0x2893, 
	 0x327a, 
	 0x4000
};

/*
 * coefficients for LSF intensity stereo processing
 * derived from section 2.4.3.2 of ISO/IEC 13818-3
 *
 * is_lsf_table[0][i] = (1 / sqrt(sqrt(2)))^(i + 1)
 * is_lsf_table[1][i] = (1 /      sqrt(2)) ^(i + 1)
 */
mp3_frac const is_lsf_table[2][15] = 
{
	{
		 0x35d1,  0x2d41,  0x260e,  0x2000,  0x1ae9, 
		 0x16a1,  0x1307,  0x1000,  0x0d74,  0x0b50, 
		 0x0983,  0x0800,  0x06ba,  0x05a8,  0x04c2
	},
	{
		 0x2d41,  0x2000,  0x16a1,  0x1000,  0x0b50, 
		 0x0800,  0x05a8,  0x0400,  0x02d4,  0x0200, 
		 0x016a,  0x0100,  0x00b5,  0x0080,  0x005b
	}
};
