/*
 * mpeg_2__2.c
 *
 * Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
 * All rights reserved
 *
 */

#include <limits.h>
#include "compiler.h"
#include "mpeg_2_fractional.h"
#include "mpeg_2_streamreader.h"
#include "mpeg_2.h"
#include "mpeg_2__2.h"

extern mp3_stream_t mp3_bitstream;		/* bitstream pointer */
extern mp3_frac const mp3_sf_table[64];

/* 
// quantization class structure
*/
typedef struct
{
	unsigned int nlevels;
	unsigned char group;
	unsigned char bits;
	mp3_frac C;
	mp3_frac D;
} 
mp3_qc_t;

/* 
// bit allocation struct
*/
typedef struct 
{
	unsigned char nbal;
	unsigned char offset;
} 
mp3_bitalloc_t;

/*
// prototypes
*/
static void mp3_layer2_samples(mp3_qc_t const qc, mp3_frac samples[3]);
unsigned short mp3_crc_check(mp3_stream_t, unsigned int, unsigned short);

/*
// initialize layer 2 decoder
*/
void mp3_layer2_init(void)
{
	/* nothing to do */
}

/*
// decode the next layer 2 frame
*/
unsigned int mp3_layer2_decode(unsigned int nch_decode)
{
	unsigned int index, sblimit, nbal, nch, bound, gr, ch, s, sb;
	unsigned char const *offsets;
	unsigned char allocation[2][32];
	unsigned char scfsi[2][32];
	unsigned char scalefactor[2][32][3];
	mp3_frac samples[3];
	
	static const mp3_qc_t qc_table[17] = 
	{
		{     3, 2,  5, 0x5555, 0x2000 }, 
		{     5, 3,  7, 0x6666, 0x2000 }, 
		{     7, 0,  3, 0x4925, 0x1000 }, 
		{     9, 4, 10, 0x71c7, 0x2000 }, 
		{    15, 0,  4, 0x4444, 0x0800 }, 
		{    31, 0,  5, 0x4211, 0x0400 }, 
		{    63, 0,  6, 0x4104, 0x0200 }, 
		{   127, 0,  7, 0x4081, 0x0100 }, 
		{   255, 0,  8, 0x4040, 0x0080 }, 
		{   511, 0,  9, 0x4020, 0x0040 }, 
		{  1023, 0, 10, 0x4010, 0x0020 }, 
		{  2047, 0, 11, 0x4008, 0x0010 }, 
		{  4095, 0, 12, 0x4004, 0x0008 }, 
		{  8191, 0, 13, 0x4002, 0x0004 }, 
		{ 16383, 0, 14, 0x4001, 0x0002 }, 
		{ 32767, 0, 15, 0x4001, 0x0001 }, 
		{ 65535, 0, 16, 0x4000, 0x0001 }
	};

	static unsigned char const offset_table[6][15] = 
	{
		{ 0, 1, 16 },
		{ 0, 1, 2, 3, 4, 5, 16 },
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
		{ 0, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 16 },
		{ 0, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 } 
	};
	
	static const mp3_bitalloc_t bitalloc_table[8] = 
	{
		{ 2, 0 },
		{ 2, 3 },
		{ 3, 3 },
		{ 3, 1 },
		{ 4, 2 },
		{ 4, 3 },
		{ 4, 4 },
		{ 4, 5 } 
	};
	
	static const unsigned char sbq_limits[5] = 
	{ 
		27, 30, 8, 12, 30 
	};
	
	static const unsigned char sbq_offsets[5][30] = 
	{
		{ 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0 },
		{ 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0 },
		{ 5, 5, 2, 2, 2, 2, 2, 2 },
		{ 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 },
		{ 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
	};
	
	nch = mp3_header.no_of_channels;
	
	if (mp3_header.lsf_ext)
	{
		index = 4;
	}
	else if (mp3_header.freeformat)
	{
    	index = (mp3_header.samplerate == 48000) ? 0 : 1;
	}
	else 
	{
	  	unsigned long bitrate_per_channel = mp3_header.bitrate;
	  	
	  	if (nch == 2) 
		{
	    	bitrate_per_channel >>= 1;
	  	}
	  	else 
		{  
	    	if (bitrate_per_channel > 192000) 
		    {
				return mp3_last_error = MP3_ERROR_INVALID_BITSTREAM;
	    	}
	  	}
	
	  	if (bitrate_per_channel <= 48000)
	  	{
	    	index = (mp3_header.samplerate == 32000) ? 3 : 2;
	    }
	  	else if (bitrate_per_channel <= 80000)
	  	{
	    	index = 0;
	    }
	  	else 
		{
	    	index = (mp3_header.samplerate == 48000) ? 0 : 1;
	  	}
	}
	
	sblimit = sbq_limits[index];
	offsets = sbq_offsets[index];
	bound = 32;
	
	if (mp3_header.mode == MP3_MODE_JOINT_STEREO) 
	{
		mp3_header.intensity_stereo = 1;
	  	bound = (mp3_header.mode_extension + 1) * 4;
	}
	
	if (bound > sblimit)
	{
	  	bound = sblimit;
	}

	/* 
	// bit allocations 
	*/	
	for (sb = 0; sb < bound; sb++) 
	{
		nbal = bitalloc_table[offsets[sb]].nbal;
	
	  	for (ch = 0; ch < nch; ch++)
		{
			mp3_header.crc_check = mp3_crc_check(mp3_bitstream, nbal, mp3_header.crc_check);
	    	allocation[ch][sb] = mp3_stream_read(&mp3_bitstream, nbal);
		}
	}
	
	for (sb = bound; sb < sblimit; sb++) 
	{
		nbal = bitalloc_table[offsets[sb]].nbal;
		mp3_header.crc_check = mp3_crc_check(mp3_bitstream, nbal, mp3_header.crc_check);
	  	allocation[0][sb] = mp3_stream_read(&mp3_bitstream, nbal);
	  	allocation[1][sb] = allocation[0][sb];
	}
	
	/* 
	// scalefactor selection info 
	*/	
	for (sb = 0; sb < sblimit; sb++) 
	{
		for (ch = 0; ch < nch; ch++) 
		{
	    	if (allocation[ch][sb])
			{
				mp3_header.crc_check = mp3_crc_check(mp3_bitstream, 2, mp3_header.crc_check);
				scfsi[ch][sb] = mp3_stream_read(&mp3_bitstream, 2);
			}
	  	}
	}
	
	/*
	// check CRC
	*/
	if (mp3_header.crc_protected)
	{
	  	if (mp3_header.crc_check != mp3_header.crc_target) 
		{
	    	return mp3_last_error = MP3_ERROR_CRC_FAILED;
	  	}
	}
	
	/* 
	// scalefactors 
	*/	
	for (sb = 0; sb < sblimit; sb++) 
	{
		for (ch = 0; ch < nch; ch++) 
		{
	    	if (allocation[ch][sb]) 
		    {
				scalefactor[ch][sb][0] = mp3_stream_read(&mp3_bitstream, 6);
	
				switch (scfsi[ch][sb]) 
				{
					case 0: scalefactor[ch][sb][1] = mp3_stream_read(&mp3_bitstream, 6);
						/* no break */
					case 1: scalefactor[ch][sb][2] = mp3_stream_read(&mp3_bitstream, 6); break;
					case 2: scalefactor[ch][sb][2] = scalefactor[ch][sb][1] = scalefactor[ch][sb][0]; break;
					case 3: scalefactor[ch][sb][2] = mp3_stream_read(&mp3_bitstream, 6); break;
				}
	
				if (scfsi[ch][sb] & 1)
				{
	 				scalefactor[ch][sb][1] = scalefactor[ch][sb][scfsi[ch][sb] - 1];
				}
	    	}
	  	}
	}
	
	/* 
	// samples 
	*/	
	for (gr = 0; gr < 12; gr++) 
	{
		for (sb = 0; sb < bound; sb++) 
		{
	    	for (ch = 0; ch < nch_decode; ch++) 
		    {
				if ((index = allocation[ch][sb])) 
				{
	 				index = offset_table[bitalloc_table[offsets[sb]].offset][index - 1];
	 				mp3_layer2_samples(qc_table[index], samples);
	
	 				for (s = 0; s < 3; s++) 
		 			{
	   					mp3_out->sb_samples[ch][3 * gr + s][sb] = MP3_FRAC_MUL(samples[s], mp3_sf_table[scalefactor[ch][sb][gr / 4]]);
	 				}
				}
				else 
				{
	 				for (s = 0; s < 3; s++)
	 				{
	   					mp3_out->sb_samples[ch][3 * gr + s][sb] = 0;
	   				}
				}
	    	}
	  	}
	
	  	for (sb = bound; sb < sblimit; sb++) 
		{
	    	if (allocation[0][sb]) 
		    {
			    index = allocation[0][sb];
				index = offset_table[bitalloc_table[offsets[sb]].offset][index - 1];

				mp3_layer2_samples(qc_table[index], samples);
	
				for (ch = 0; ch < nch_decode; ch++) 
				{
	 				for (s = 0; s < 3; s++) 
		 			{
	   					mp3_out->sb_samples[ch][3 * gr + s][sb] = 
	   						MP3_FRAC_MUL(samples[s], mp3_sf_table[scalefactor[ch][sb][gr >> 2]]);
	 				}
				}
	    	}
	    	else 
		    {
				for (ch = 0; ch < nch_decode; ch++) 
				{
	 				for (s = 0; s < 3; s++)
	 				{
	   					mp3_out->sb_samples[ch][3 * gr + s][sb] = 0;
	   				}
				}
	    	}
	  	}
	
	  	for (ch = 0; ch < nch_decode; ch++) 
		{
	    	for (s = 0; s < 3; s++) 
		    {
				for (sb = sblimit; sb < 32; sb++)
				{
	 				mp3_out->sb_samples[ch][3 * gr + s][sb] = 0;
	 			}
	    	}
	  	}
	}
	
	return MP3_SUCCESS;
}

/*
// decode 3 layer 3 samples
*/
static void mp3_layer2_samples(mp3_qc_t const qc, mp3_frac samples[3])
{
	if (qc.group) 
	{
		unsigned int nb, i;
	  	unsigned int s = mp3_stream_read(&mp3_bitstream, qc.bits);
	  	unsigned int nlevels = qc.nlevels;
		nb = qc.group;
		
	  	for (i = 0; i < 3; i++) 
		{
			mp3_frac sample;
	    	sample = s % nlevels;
			sample ^= (1 << (nb - 1));
			sample |= -(sample & (1 << (nb - 1)));
			sample <<= MP3_FRACBITS - (nb - 1);
			samples[i] = MP3_FRAC_MUL(sample + qc.D, qc.C);
	    	s /= nlevels;
	  	}
	}
	else 
	{
		unsigned int i, nb = qc.bits;
	  		
	  	for (i = 0; i < 3; i++)
	  	{
			mp3_frac sample;
	    	sample = mp3_stream_read(&mp3_bitstream, nb);
			sample ^= (1 << (nb - 1));
			sample |= -(sample & (1 << (nb - 1)));
			sample <<= MP3_FRACBITS - (nb - 1);
			samples[i] = MP3_FRAC_MUL(sample + qc.D, qc.C);
	  	}
	}
}
