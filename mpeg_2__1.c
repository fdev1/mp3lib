/*
 * mpeg_2__1.c
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
#include "mpeg_2__1.h"

extern mp3_stream_t mp3_bitstream;		/* bitstream pointer */
extern mp3_frac const mp3_sf_table[64];

static mp3_frac mp3_layer1_sample(unsigned int nb);
unsigned short mp3_crc_check(mp3_stream_t, unsigned int, unsigned short);

/*
// initialize layer 1 decoder
*/
void mp3_layer1_init(void)
{
	/* nothing to do */
}

/*
// decode a layer1 sample
*/
unsigned int mp3_layer1_decode(unsigned int nch_decode)
{
	unsigned int nch, bound, ch, s, sb, nb;
	unsigned char allocation[2][32];
	unsigned char scalefactor[2][32];
	
	nch = mp3_header.no_of_channels;
	
	bound = 32;
	if (mp3_header.mode == MP3_MODE_JOINT_STEREO) 
	{
		mp3_header.intensity_stereo = 1;
	  	bound = (mp3_header.mode_extension + 1) * 4;
	}
	/* 
	// CRC check 
	*/	
	if (mp3_header.crc_protected)
	{
		mp3_header.crc_check =
	    	mp3_crc_check(mp3_bitstream, 4 * (bound * nch + (32 - bound)),
	  		mp3_header.crc_check);
	
	  	if (mp3_header.crc_check != mp3_header.crc_target) 
		{
	    	return mp3_last_error = MP3_ERROR_CRC_FAILED;
	  	}
	}
	
	/* 
	// bit allocations 
	*/	
	for (sb = 0; sb < bound; sb++) 
	{
		for (ch = 0; ch < nch; ch++) 
		{
	    	nb = mp3_stream_read(&mp3_bitstream, 4);
	
	    	if (nb == 15) 
		    {
				return mp3_last_error = MP3_ERROR_INVALID_BITSTREAM;
	    	}
	
	    	allocation[ch][sb] = nb ? nb + 1 : 0;
	  	}
	}
	
	for (sb = bound; sb < 32; sb++) 
	{
		nb = mp3_stream_read(&mp3_bitstream, 4);
	
	  	if (nb == 15) 
		{
	    	return mp3_last_error = MP3_ERROR_INVALID_BITSTREAM;
	  	}
	
	  	allocation[1][sb] = nb ? nb + 1 : 0;
	  	allocation[0][sb] = allocation[1][sb];
	}
	
	/* 
	// scalefactors 
	*/	
	for (sb = 0; sb < 32; sb++) 
	{
		for (ch = 0; ch < nch; ch++) 
		{
	    	if (allocation[ch][sb]) 
		    {
				scalefactor[ch][sb] = mp3_stream_read(&mp3_bitstream, 6);
	    	}
	  	}
	}
	
	/* 
	// decode samples 
	*/	
	for (s = 0; s < 12; s++) 
	{
		for (sb = 0; sb < bound; sb++) 
		{
	    	for (ch = 0; ch < nch_decode; ch++) 
		    {
				nb = allocation[ch][sb];
				if (nb) 
				{
					mp3_out->sb_samples[ch][s][sb] = MP3_FRAC_MUL(mp3_layer1_sample(nb), mp3_sf_table[scalefactor[ch][sb]]);
				}
				else
				{
					mp3_out->sb_samples[ch][s][sb] = 0;
				}
	    	}
	  	}
	
	  	for (sb = bound; sb < 32; sb++) 
		{
			nb = allocation[0][sb];

	    	if (nb) 
		    {
				mp3_frac sample = mp3_layer1_sample(nb);
	
				for (ch = 0; ch < nch_decode; ch++) 
				{
	 				mp3_out->sb_samples[ch][s][sb] = MP3_FRAC_MUL(sample, mp3_sf_table[scalefactor[ch][sb]]);
				}
	    	}
	    	else 
		    {
				for (ch = 0; ch < nch_decode; ++ch)
				{
	 				mp3_out->sb_samples[ch][s][sb] = 0;
				}
	    	}
	  	}
	}
	
	return MP3_SUCCESS;
}

/*
// decode 1 layer 1 sample
*/
static mp3_frac mp3_layer1_sample(unsigned int nb)
{
	mp3_frac sample;
	static mp3_frac const linear_table[14] = 
	{
		0x2aab, 0x2492, 0x2222, 0x2108, 0x2082, 0x2041, 0x2020, 
		0x2010, 0x2008, 0x2004, 0x2002, 0x2001, 0x2001, 0x2000
	};

	sample = mp3_stream_read(&mp3_bitstream, nb);
	sample ^= 1 << (nb - 1);
	sample |= -(sample & (1 << (nb - 1)));
	sample <<= MP3_FRACBITS - (nb - 1);
	sample += MP3_FRAC_ONE >> (nb - 1);
	return MP3_FRAC_MUL(sample, linear_table[nb - 2]);
}

