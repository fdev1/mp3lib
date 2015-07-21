/*
 * mpeg_2__3.c
 *
 * Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
 * All rights reserved
 *
 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#if defined(__XC16__) || defined(__C30__)
	#define MP3_RENORMALIZE
#endif
#define MP3_FAST_STREAM
#if defined(__XC16__) || defined(__C30__)
#define mp3_stream_read_bool_fast 	mp3_stream_read_bool
#define mp3_stream_read_fast 		mp3_stream_read
#endif

#include "compiler.h"
#include "mpeg_2_fractional.h"
#include "mpeg_2_streamreader.h"
#include "mpeg_2.h"
#include "mpeg_2__3_huff.h"
#include "mpeg_2__3.h"

extern mp3_stream_t mp3_bitstream;		/* bitstream pointer */

#if defined (__XC16__) || defined(__C30__)

#if defined(__dsPIC33E__)
	/*
	// when overlap is allocated on EDS memory MP3_OVERLAP_IN_EDS must be
	// defined to 1 on mpeg_2.inc
	*/
	__eds__ mp3_frac __attribute__((section(".mp3_overlap"), eds)) mp3_overlap[2][32][18];
	__eds__ mp3_frac __attribute__((section(".mp3_xr"), space(ymemory), eds)) mp3_xr[2][576];
	__eds__ mp3_frac __attribute__((section(".mp3_y"), space(ymemory), eds)) mp3_y[36];
	#define MP3_XR_PTR __eds__
	#define MP3_Y_PTR __eds__
#else
	mp3_frac __attribute__((section(".mp3_overlap"))) mp3_overlap[2][32][18];
	mp3_frac __attribute__((section(".mp3_xr"), space(ymemory))) mp3_xr[2][576];
	mp3_frac __attribute__((section(".mp3_y"), space(ymemory))) mp3_y[36];
	#define MP3_XR_PTR
	#define MP3_Y_PTR
#endif
mp3_frac __attribute__((section(".mp3_z"))) mp3_z[36];
static unsigned char __attribute__((section(".mp3_bit_reservoir"))) mp3_bit_reservoir[MP3_BIT_RESERVOIR_LEN];
static unsigned int mp3_bit_reservoir_len;

#else
static mp3_frac mp3_overlap[2][32][18];
static mp3_frac mp3_xr[2][576];
static mp3_frac mp3_y[36];
static mp3_frac mp3_z[36];
static unsigned char mp3_bit_reservoir[MP3_BIT_RESERVOIR_LEN];
static unsigned int mp3_bit_reservoir_len;
#define MP3_XR_PTR
#define MP3_Y_PTR
#endif

/*
// lookup tables
*/
extern mp3_sflen const sflen_table[16];
extern mp3_sfbwidth_tables const sfbwidth_table[9];
extern unsigned char const nsfb_table[6][3][4];
extern unsigned char const mp3_pretab[22];
extern mp3_frac const mp3_rq_powers[7];
extern mp3_frac const mp3__cs[9];
extern mp3_frac const mp3_ca[9];
extern mp3_frac const mp3_imdct_s[36];
extern mp3_frac const mp3_imdct_l[188];
extern mp3_frac const mp3_window_l[36];
extern mp3_frac const mp3_window_s[12];
extern mp3_frac const is_table[7];
extern mp3_frac const is_lsf_table[2][15];

/*
// tables for requantization
//
// x^(4/3) * 2 = mp3_rq_tbl[x] * 2 ^ (mp3_rq_exp_tbl[x])
*/
#if defined(__XC16__) || defined(__C30__)
extern __psv__ const unsigned int mp3_rq_tbl[8207];
#else
extern const unsigned int mp3_rq_tbl[8207];
#endif
extern const unsigned char mp3_rq_exp_tbl[8207];

/*
// prototypes
*/
static unsigned int mp3_layer3_main_data(unsigned int nch, mp3_sideinfo_t *si, unsigned int nch_decode);
static unsigned int mp3_layer3_sideinfo_decode(unsigned int nch, unsigned int *data_bitlen, mp3_sideinfo_t *si);
static unsigned int mp3_layer3_scalefactors(unsigned int gr, unsigned int ch, unsigned char scalefac[2][2][39], mp3_sideinfo_t *si);
static unsigned int mp3_layer3_scalefactors_lsf(unsigned int gr, unsigned int ch, unsigned char scalefac[2][2][39], mp3_channel_t *channel);
static unsigned int mp3_layer3_huffmandecode(unsigned int gr, unsigned int ch, unsigned char const *sfbwidth, unsigned char scalefac[2][2][39], int bits_to_decode, mp3_sideinfo_t *si);
static unsigned int mp3_layer3_stereo(unsigned int gr, unsigned char const *sfbwidth, unsigned char scalefac[2][2][39], mp3_sideinfo_t *si);
unsigned short mp3_crc_check(mp3_stream_t, unsigned int, unsigned short);

#if defined(__XC16__) || defined(__C30__)

#if defined(MP3_RENORMALIZE)
int mp3_layer3_normalize(unsigned int ch);
void mp3_layer3_renormalize(int exp);
#endif
mp3_frac mp3_layer3_rq_sample(unsigned int value, int exp);
void mp3_layer3_reorder(unsigned int ch, unsigned char const sfbwidth[39], unsigned int mixed_block);
void mp3_layer3_aliasreduce(unsigned int ch, unsigned int lines);
void mp3_layer3_imdct_l(unsigned int ch, unsigned int sb, unsigned int block_type);
void mp3_layer3_imdct_s(unsigned int ch, unsigned int sb);
void mp3_layer3_overlap(unsigned int gr, unsigned int ch, unsigned int sb);
void mp3_layer3_overlap_z(unsigned int gr, unsigned int ch, unsigned int sb);
void mp3_layer3_invfreq(unsigned int gr, unsigned int ch, unsigned int sb);

#else
static mp3_frac mp3_layer3_rq_sample(unsigned int value, int exp);
static void mp3_layer3_reorder(unsigned int ch, unsigned char const sfbwidth[39], unsigned int mixed_block);
static void mp3_layer3_aliasreduce(unsigned int ch, unsigned int lines);
static void mp3_layer3_imdct_l(unsigned int ch, unsigned int sb, unsigned int block_type);
static void mp3_layer3_imdct_s(unsigned int ch, unsigned int sb);
static void mp3_layer3_overlap(unsigned int gr, unsigned int ch, unsigned int sb);
static void mp3_layer3_overlap_z(unsigned int gr, unsigned int ch, unsigned int sb);
static void mp3_layer3_invfreq(unsigned int gr, unsigned int ch, unsigned int sb);
#endif

/*
// initialize layer 3 decoder
*/
void mp3_layer3_init(void)
{
	mp3_layer3_overlap_init();
	mp3_bit_reservoir_len = 0;
}

/*
// zero out overlap buffer
*/
#if !(defined(__XC16__) || defined(__C30__))
void mp3_layer3_overlap_init(void)
{
	memset(mp3_overlap, 0, 2 * 32 * 18 * sizeof(mp3_frac));
}
#endif

/*
// decode the next frame
*/
unsigned int mp3_layer3_decode(unsigned int nch_decode)
{
	unsigned int nch, next_md_begin = 0;
	unsigned int si_len, data_bitlen, md_len;
	unsigned int frame_len, frame_used, frame_unused;
	unsigned int error;
	unsigned int result = 0;
	unsigned char * p_md = 0;
	mp3_sideinfo_t si;

	extern unsigned char const *mp3_next_frame;
	extern unsigned int mp3_anc_bitstream_len;
	extern unsigned int mp3_bitstream_len;
	extern unsigned char mp3_bitstream_buffer[];

	nch = mp3_header.no_of_channels;
	if (nch_decode > nch)
	{
		nch_decode = nch;
	}
	/*
	//
	// 320000 * .024 bits = 7680 bits = 960 bytes 1 ch
	// granule = frame / 2 = 480 bytes
	// main data 1 frame = 960 = 4 (header) = 956 bytes
	// 
	//
	*/
	if (mp3_header.lsf_ext)
	{
		if (nch == 1)
		{
			si_len = 9;
		}
		else
		{
			si_len = 17;
		}
	}
	else
	{
		if (nch == 1)
		{
			si_len = 17;
		}
		else
		{
			si_len = 32;
		}
	}

	/* 
	// frame sanity check
	*/	
	if (mp3_next_frame - mp3_stream_next(&mp3_bitstream) < (int) si_len) 
	{
		mp3_bit_reservoir_len = 0;
		return mp3_last_error = MP3_ERROR_INVALID_BITSTREAM;
	}

	/*
	// check CRC
	*/
	if (mp3_header.crc_protected)
	{
		mp3_header.crc_check = mp3_crc_check(mp3_bitstream, si_len * CHAR_BIT, mp3_header.crc_check);
	
	  	if (mp3_header.crc_check != mp3_header.crc_target) 
		{
	    	result = mp3_last_error = MP3_ERROR_CRC_FAILED;
	  	}
	}
	
	/* 
	// decode frame sideinfo
	*/	
	error = mp3_layer3_sideinfo_decode(nch, &data_bitlen, &si);
	if (error && result == 0) 
	{
		result = mp3_last_error = error;
	}
	
	mp3_header.private_bits |= si.private_bits;
	
	/* 
	// find main_data of next frame 
	*/
	if (mp3_next_frame[0] == 0xFF && (mp3_next_frame[1] & 0xE6) == 0xE2)
	{
		if (mp3_next_frame[1] & 1)
		{
			if (mp3_next_frame[1] & 0x08)
			{
				next_md_begin = mp3_next_frame[4];
				next_md_begin = (next_md_begin << 1) | (mp3_next_frame[5] >> 7);
			}
			else
			{
				next_md_begin = mp3_next_frame[4];
			}
		}
		else
		{
			if (mp3_next_frame[1] & 0x08)
			{
				next_md_begin = mp3_next_frame[6];
				next_md_begin = (next_md_begin << 1) | (mp3_next_frame[7] >> 7);
			}
			else
			{
				next_md_begin = mp3_next_frame[6];
			}
		}
	}
	
	/* 
	// find main_data of this frame 
	*/	
	frame_len = mp3_next_frame - mp3_stream_next(&mp3_bitstream);
	
	if (next_md_begin > si.main_data_begin + frame_len)
	{
		next_md_begin = 0;
	}
	
	md_len = si.main_data_begin + frame_len - next_md_begin;
	frame_used = 0;
	
	/*
	// if the main data begins before the current frame load
	// the data from the bit reservoir
	*/
	if (si.main_data_begin) 
	{
		if (si.main_data_begin > mp3_bit_reservoir_len) 
		{
	    	if (result == 0) 
		    {
				result = mp3_last_error = MP3_ERROR_INVALID_BITSTREAM;
	    	}
	  	}
	  	else 
		{
			p_md = (unsigned char*) mp3_stream_next(&mp3_bitstream);
			memmove(p_md + si.main_data_begin, p_md, mp3_bitstream_len - ((unsigned int) (p_md - mp3_bitstream_buffer)) + MP3_BUFFER_GUARD);
			memcpy(p_md, mp3_bit_reservoir + mp3_bit_reservoir_len - si.main_data_begin, si.main_data_begin);
			mp3_next_frame += si.main_data_begin;
			mp3_bitstream_len += si.main_data_begin;
			
	    	if (md_len > si.main_data_begin) 
			{
				mp3_bit_reservoir_len += frame_used;
	    	}
	  	}
	}
	else
	{
		frame_used = md_len;
		mp3_bit_reservoir_len = 0;
	}
	
	frame_unused = frame_len - frame_used;
	
	/* 
	// decode frame main_data 
	*/	
	if (result == 0) 
	{
		error = mp3_layer3_main_data(nch, &si, nch_decode);
		if (error) 
		{
			result = mp3_last_error = error;
		}
		/* 
		// designate ancillary bits 
		*/
		mp3_anc_bitstream = mp3_bitstream;
		mp3_anc_bitstream_len = md_len * CHAR_BIT - data_bitlen;
	}
	/*
	// update no_of_channels in header
	*/
	mp3_header.no_of_channels = nch_decode;
	/*
	// save any unused data on the bit reservoir
	*/
	if (frame_unused >= next_md_begin) 
	{
		memcpy(mp3_bit_reservoir, mp3_next_frame - next_md_begin, next_md_begin);
		mp3_bit_reservoir_len = next_md_begin;
		_ASSERT(mp3_bit_reservoir_len <= 511);
	}
	else 
	{
		if (md_len < si.main_data_begin) 
		{
	    	unsigned int extra = si.main_data_begin - md_len;
	    	if (extra + frame_unused > next_md_begin)
			{
				extra = next_md_begin - frame_unused;
			}
	
	    	if (extra < mp3_bit_reservoir_len) 
		    {
				_ASSERT(p_md != 0);
				memmove(mp3_bit_reservoir, p_md + mp3_bit_reservoir_len - extra, extra);
				mp3_bit_reservoir_len = extra;
				_ASSERT(mp3_bit_reservoir_len <= 511);
	    	}
	  	}
	  	else
	  	{
	    	mp3_bit_reservoir_len = 0;
	    }
	
		memcpy(mp3_bit_reservoir + mp3_bit_reservoir_len, mp3_next_frame - frame_unused, frame_unused);
		mp3_bit_reservoir_len += frame_unused;
		_ASSERT(mp3_bit_reservoir_len <= 511);
	}
	
	return result;
}

/*
// decode main data
*/
static unsigned int mp3_layer3_main_data(unsigned int nch, mp3_sideinfo_t *si, unsigned int nch_decode)
{
	unsigned int sfreqi, ngr, gr;

	if (mp3_header.mpeg_2_5_ext)
	{
		sfreqi = mp3_header.samplerate * 2;
		sfreqi = ((sfreqi >> 7) & 0xF) + ((sfreqi >> 15) & 1) - 5;
	}
	else
	{
		sfreqi = ((mp3_header.samplerate >> 7) & 0xF) + ((mp3_header.samplerate >> 15) & 1) - 8;
	}
	
	if (mp3_header.lsf_ext)
	{
		ngr = 1;
	}
	else
	{
		ngr = 2;	
	}

	for (gr = 0; gr < ngr; gr++) 
	{
		unsigned int error;
		unsigned int ch;
		unsigned char const *sfbwidth[2];
		mp3_granule_t *granule = &si->granule[gr];
		#if defined(MP3_RENORMALIZE)
		int exponents[2];
		#endif

		{
			unsigned char scalefac[2][2][39];

			for (ch = 0; ch < nch; ch++) 
			{
				mp3_channel_t *channel = &granule->channel[ch];
				int bits_to_decode;

				sfbwidth[ch] = sfbwidth_table[sfreqi].l;

				if (channel->block_type == 2) 
				{
					if (channel->mixed_block)
					{
						sfbwidth[ch] = sfbwidth_table[sfreqi].m;
					}
					else
					{
						sfbwidth[ch] = sfbwidth_table[sfreqi].s;
					}
				}
				/*
				// decode scalefactors
				*/
				if (mp3_header.lsf_ext)
				{
					bits_to_decode = mp3_layer3_scalefactors_lsf(gr, ch, scalefac, &granule->channel[ch]);
					bits_to_decode = (int) channel->part2_3_length - (int) bits_to_decode;
				}
				else 
				{
					bits_to_decode = mp3_layer3_scalefactors(gr, ch, scalefac, si);
					bits_to_decode = (int) channel->part2_3_length - (int) bits_to_decode;
				}

				if (bits_to_decode < 0)
  					return MP3_ERROR_INVALID_BITSTREAM;
				/*
				// decode huffman codes
				*/
				error = mp3_layer3_huffmandecode(gr, ch, sfbwidth[ch], scalefac, bits_to_decode, si);
				if (error)
					return error;
				/*
				// normalize frequency lines
				*/
				#if defined(MP3_RENORMALIZE)
				exponents[ch] = mp3_layer3_normalize(ch);
				#endif
			}
			/*
			// stereo processing
			*/
	  		if (mp3_header.mode == MP3_MODE_JOINT_STEREO && mp3_header.mode_extension) 
			{
	    		error = mp3_layer3_stereo(gr, sfbwidth[0], scalefac, si);
	    		if (error)
					return error;
	  		}
		}

		for (ch = 0; ch < nch_decode; ch++) 
		{
			mp3_channel_t const *channel = &granule->channel[ch];
			unsigned int sb, i, sblimit;
			#if defined(MP3_RENORMALIZE)
			int exp = exponents[ch];
			#endif

			/*
			// find out where the nonzero subbands end
			*/
			for (i = 576; i > 36 && mp3_xr[ch][i - 1] == 0; i--);
    		sblimit = 32 - (576 - i) / 18;
	    	/* 
			// (nonzero) subbands 
			*/
	    	if (channel->block_type != 2) 
		    {
				/*
				// alias reduction
				*/
				mp3_layer3_aliasreduce(ch, 576);
				/*
				// IMDCT, renormalization, overlap, and 
				// frequency inversion of long blocks
				*/
				for (sb = 0; sb < sblimit; sb++) 
				{
					mp3_layer3_imdct_l(ch, sb, channel->block_type);
					#if defined(MP3_RENORMALIZE)
					mp3_layer3_renormalize(exp);
					#endif
					mp3_layer3_overlap(gr, ch, sb);
					
					if (sb & 1)
					{
						mp3_layer3_invfreq(gr, ch, sb);
					}
				}
				_ASSERT(channel->mixed_block == 0);
			}
		    else 
			{

				/*
				// re-order freq lines
				*/
				mp3_layer3_reorder(ch, sfbwidth[ch], channel->mixed_block);
				
				if (channel->mixed_block)
				{
					mp3_layer3_aliasreduce(ch, 36); /* ???? */
					mp3_layer3_imdct_l(ch, 0, 0);
					#if defined(MP3_RENORMALIZE)
					mp3_layer3_renormalize(exp);
					#endif
					mp3_layer3_overlap(gr, ch, 0);
					mp3_layer3_imdct_l(ch, 1, 0);
					#if defined(MP3_RENORMALIZE)
					mp3_layer3_renormalize(exp);
					#endif
					mp3_layer3_overlap(gr, ch, 1);
			    	mp3_layer3_invfreq(gr, ch, 1);
				}
				else
				{
					mp3_layer3_imdct_s(ch, 0);
					#if defined(MP3_RENORMALIZE)
					mp3_layer3_renormalize(exp);
					#endif
					mp3_layer3_overlap(gr, ch, 0);
					mp3_layer3_imdct_s(ch, 1);
					#if defined(MP3_RENORMALIZE)
					mp3_layer3_renormalize(exp);
					#endif
					mp3_layer3_overlap(gr, ch, 1);
			    	mp3_layer3_invfreq(gr, ch, 1);
				}
				/*
				// imdct and overlap short blocks
				*/
				for (sb = 2; sb < sblimit; sb++) 
				{
					mp3_layer3_imdct_s(ch, sb);
					#if defined(MP3_RENORMALIZE)
					mp3_layer3_renormalize(exp);
					#endif
					mp3_layer3_overlap(gr, ch, sb);
					
					if (sb & 1)
					{
						mp3_layer3_invfreq(gr, ch, sb);
					}
				}
			}
		
		    /* 
			// zero subbands 
			*/		
		    for (sb = sblimit; sb < 32; sb++) 
			{
				mp3_layer3_overlap_z(gr, ch, sb);
				if (sb & 1)
				{
					mp3_layer3_invfreq(gr, ch, sb);
				}
		    }
		}
	}
	
	return MP3_SUCCESS;
}

/*
// decode side info
*/
static unsigned int mp3_layer3_sideinfo_decode(unsigned int nch, unsigned int *data_bitlen, mp3_sideinfo_t *si)
{
	unsigned int ngr, gr, ch, i;
	unsigned int result = 0;
	
	*data_bitlen = 0;
	
	if (!mp3_header.lsf_ext) 
	{
		si->main_data_begin = mp3_stream_read_fast(&mp3_bitstream, 9);
		si->private_bits = mp3_stream_read_fast(&mp3_bitstream, (nch == 1) ? 5 : 3);
		ngr = 2;
	
	  	for (ch = 0; ch < nch; ++ch)
		{
	    	si->scfsi[ch] = mp3_stream_read_fast(&mp3_bitstream, 4);
		}
	}
	else
	{
		si->main_data_begin = mp3_stream_read_fast(&mp3_bitstream, 8);
		si->private_bits = mp3_stream_read_fast(&mp3_bitstream, (nch == 1) ? 1 : 2);
		ngr = 1;
	}
	
	for (gr = 0; gr < ngr; gr++) 
	{
	  	mp3_granule_t *granule = &si->granule[gr];
	
	  	for (ch = 0; ch < nch; ch++) 
		{
	    	mp3_channel_t *channel = &granule->channel[ch];

			channel->mixed_block = 0;
		    channel->part2_3_length = mp3_stream_read_fast(&mp3_bitstream, 12);
		    channel->big_values = mp3_stream_read_fast(&mp3_bitstream, 9);
		    channel->global_gain = mp3_stream_read_fast(&mp3_bitstream, 8);
		    channel->scalefac_compress = mp3_stream_read_fast(&mp3_bitstream, mp3_header.lsf_ext ? 9 : 4);
		
		    *data_bitlen += channel->part2_3_length;
		
		    if (channel->big_values > 288 && result == 0)
			{
				result = MP3_ERROR_INVALID_BITSTREAM;
			}
	
			if (mp3_stream_read_fast(&mp3_bitstream, 1)) 
			{
				channel->block_type = mp3_stream_read_fast(&mp3_bitstream, 2);
	
				if (!result && channel->block_type == 0)
				{
	 				result = MP3_ERROR_INVALID_BITSTREAM;
				}
	
				if (!result && !mp3_header.lsf_ext && channel->block_type == 2 && si->scfsi[ch])
				{
	 				result = MP3_ERROR_INVALID_BITSTREAM;
				}
	
				channel->region0_count = 7;
				channel->region1_count = 36;
			
				if (mp3_stream_read_fast(&mp3_bitstream, 1))
				{
					channel->mixed_block = 1;
				}
				else if (channel->block_type == 2)
				{
			 		channel->region0_count = 8;
				}
			
				for (i = 0; i < 2; i++)
				{
		 			channel->table_select[i] = mp3_stream_read_fast(&mp3_bitstream, 5);
		 		}
	
				for (i = 0; i < 3; i++)
				{
					channel->subblock_gain[i] = mp3_stream_read_fast(&mp3_bitstream, 3);
				}
	    	}
	    	else 
			{
				channel->block_type = 0;
	
				for (i = 0; i < 3; i++)
				{
		 			channel->table_select[i] = mp3_stream_read_fast(&mp3_bitstream, 5);
		 		}
	
				channel->region0_count = mp3_stream_read_fast(&mp3_bitstream, 4);
				channel->region1_count = mp3_stream_read_fast(&mp3_bitstream, 3);
	    	}
	
		    if (mp3_header.lsf_ext)
		    {
				channel->preflag = 0;
				channel->scalefac_scale = mp3_stream_read_fast(&mp3_bitstream, 1);
				channel->count1table_select = mp3_stream_read_fast(&mp3_bitstream, 1);
		    }
		    else
		    {
				channel->preflag = mp3_stream_read_fast(&mp3_bitstream, 1);
				channel->scalefac_scale = mp3_stream_read_fast(&mp3_bitstream, 1);
				channel->count1table_select = mp3_stream_read_fast(&mp3_bitstream, 1);
			}
	  	}
	}
	
	return result;
}

/*
// decode channel scalefactors of one granule from a bitstream
*/
static unsigned int mp3_layer3_scalefactors(unsigned int gr, unsigned int ch, unsigned char scalefac[2][2][39], mp3_sideinfo_t *si)
{
	mp3_stream_t start;
	mp3_channel_t *channel = &si->granule[gr].channel[ch];
	unsigned int slen1, slen2, sfb, scfsi = gr == 0 ? 0 : si->scfsi[ch];

	start = mp3_bitstream;
	slen1 = sflen_table[channel->scalefac_compress].slen1;
	slen2 = sflen_table[channel->scalefac_compress].slen2;
	
	if (channel->block_type == 2) 
	{
		unsigned int nsfb;
	
	  	sfb = 0;
	
	  	nsfb = (channel->mixed_block) ? 8 + 3 * 3 : 6 * 3;
	  	
	  	while (nsfb--)
	  	{
			scalefac[gr][ch][sfb++]  = mp3_stream_read_fast(&mp3_bitstream, slen1);
		}
		
	  	nsfb = 6 * 3;
	  	
	  	while (nsfb--)
	  	{
			scalefac[gr][ch][sfb++] = mp3_stream_read_fast(&mp3_bitstream, slen2);
		}
		
	  	nsfb = 1 * 3;
	  	
	  	while (nsfb--)
	  	{
			scalefac[gr][ch][sfb++] = 0;
	    }
	}
	else 
	{
		if (scfsi & 0x8) 
		{
	    	for (sfb = 0; sfb < 6; sfb++)
			{
				scalefac[gr][ch][sfb] = scalefac[0][ch][sfb];
			}
	  	}
	  	else 
		{
	    	for (sfb = 0; sfb < 6; sfb++)
	    	{
				scalefac[gr][ch][sfb] = mp3_stream_read_fast(&mp3_bitstream, slen1);
			}
	  	}
	
	  	if (scfsi & 0x4) 
		{
	    	for (sfb = 6; sfb < 11; sfb++)
			{
				scalefac[gr][ch][sfb] = scalefac[0][ch][sfb];
			}
	  	}
	  	else 
		{
	    	for (sfb = 6; sfb < 11; sfb++)
	    	{
				scalefac[gr][ch][sfb] = mp3_stream_read_fast(&mp3_bitstream, slen1);
			}
	  	}
	
	  	if (scfsi & 0x2) 
		{
	    	for (sfb = 11; sfb < 16; sfb++)
			{
				scalefac[gr][ch][sfb] = scalefac[0][ch][sfb];
			}
	  	}
	  	else 
		{
	    	for (sfb = 11; sfb < 16; sfb++)
	    	{
				scalefac[gr][ch][sfb] = mp3_stream_read_fast(&mp3_bitstream, slen2);
			}
	  	}
	
	  	if (scfsi & 0x1) 
		{
	    	for (sfb = 16; sfb < 21; sfb++)
			{
				scalefac[gr][ch][sfb] = scalefac[0][ch][sfb];
			}
	  	}
	  	else 
		{
	    	for (sfb = 16; sfb < 21; sfb++)
	    	{
				scalefac[gr][ch][sfb] = mp3_stream_read_fast(&mp3_bitstream, slen2);
			}
	  	}
	
		scalefac[gr][ch][21] = 0;
	}
	
	return mp3_stream_len(&start, &mp3_bitstream);
}

/*
// decode lsf scalefactors
*/
static unsigned int mp3_layer3_scalefactors_lsf(unsigned int gr, unsigned int ch, unsigned char scalefac[2][2][39], mp3_channel_t *channel)
{
	mp3_stream_t start;
	unsigned int scalefac_compress, index, slen[4], part, n, i;
	unsigned char const *nsfb;
	start = mp3_bitstream;
	
	scalefac_compress = channel->scalefac_compress;
	index = (channel->block_type == 2) ? ((channel->mixed_block) ? 2 : 1) : 0;
	
	if (!((mp3_header.mode_extension & MP3_MODEEXT_I_STEREO) && ch)) 
	{
	  	if (scalefac_compress < 400) 
		{
		    slen[0] = (scalefac_compress >> 4) / 5;
		    slen[1] = (scalefac_compress >> 4) % 5;
		    slen[2] = (scalefac_compress % 16) >> 2;
		    slen[3] =  scalefac_compress %  4;
		
		    nsfb = nsfb_table[0][index];
	  	}
	  	else if (scalefac_compress < 500) 
		{
		    scalefac_compress -= 400;
		
		    slen[0] = (scalefac_compress >> 2) / 5;
		    slen[1] = (scalefac_compress >> 2) % 5;
		    slen[2] =  scalefac_compress %  4;
		    slen[3] = 0;
		
		    nsfb = nsfb_table[1][index];
	  	}
	  	else 
		{
		    scalefac_compress -= 500;
		
		    slen[0] = scalefac_compress / 3;
		    slen[1] = scalefac_compress % 3;
		    slen[2] = 0;
		    slen[3] = 0;
		
			channel->preflag = 1;
		
		    nsfb = nsfb_table[2][index];
	  	}
	
	  	n = 0;
	  	for (part = 0; part < 4; part++) 
		{
	    	for (i = 0; i < nsfb[part]; ++i)
	    	{
				scalefac[gr][ch][n++] = mp3_stream_read_fast(&mp3_bitstream, slen[part]);
			}
	  	}
	
	  	while (n < 39)
		{
			scalefac[gr][ch][n++] = 0;
		}
	}
	else 
	{  
	  	scalefac_compress >>= 1;
	
	  	if (scalefac_compress < 180) 
		{
		    slen[0] =  scalefac_compress / 36;
		    slen[1] = (scalefac_compress % 36) / 6;
		    slen[2] = (scalefac_compress % 36) % 6;
		    slen[3] = 0;
		
		    nsfb = nsfb_table[3][index];
	  	}
	  	else if (scalefac_compress < 244) 
		{
		    scalefac_compress -= 180;
		
		    slen[0] = (scalefac_compress % 64) >> 4;
		    slen[1] = (scalefac_compress % 16) >> 2;
		    slen[2] =  scalefac_compress %  4;
		    slen[3] = 0;
		
		    nsfb = nsfb_table[4][index];
	  	}
	  	else 
		{
	    	scalefac_compress -= 244;
	
		    slen[0] = scalefac_compress / 3;
		    slen[1] = scalefac_compress % 3;
		    slen[2] = 0;
		    slen[3] = 0;
		
		    nsfb = nsfb_table[5][index];
	  	}
	
	  	n = 0;
	  	for (part = 0; part < 4; part++) 
		{
	    	unsigned int max, is_pos;
	
	    	max = (1 << slen[part]) - 1;
	
	    	for (i = 0; i < nsfb[part]; i++) 
		    {
				is_pos = mp3_stream_read_fast(&mp3_bitstream, slen[part]);
				scalefac[gr][ch][n] = is_pos;
				scalefac[1][1][n++] = (is_pos == max);
	    	}
	  	}
	
	  	while (n < 39) 
		{
			scalefac[gr][ch][n] = 0;
			scalefac[ 1][ 1][n++] = 0;
	  	}
	}
	
	return mp3_stream_len(&start, &mp3_bitstream);
}

/*
// requantize value
*/
#if 0 || (!defined(__XC16__) && !defined(__C30__))
static mp3_frac mp3_layer3_rq_sample(unsigned int value, int exp)
{
	int frac;
	long sample;
#if 1
	frac = exp % 4;
	exp = exp / 4;
#else
{
	register int sign = 0;
	if (exp < 0)
	{
		exp = -exp;	
	}
	exp = abs(exp);
	sample = exp >> 2;
	frac = exp - (sample * 4);
	exp = sample;
	if (sign)
	{
		exp = -exp;
		frac = -frac;	
	}
}
#endif

	sample = mp3_rq_tbl[value];
	exp += mp3_rq_exp_tbl[value];

	_ASSERT(exp <= 2);

	if (exp >= 0)
	{
		if (exp > 2)
		{
			sample = MP3_FRAC_MAX;
		}
		else
		{
			sample <<= exp;
		}
	}
	else
	{
		exp = -exp;

		if (exp >= sizeof(mp3_frac) * CHAR_BIT) 
		{
    		sample = 0;
  		}
  		else 
		{
			sample += 1 << (exp - 1);
			sample >>= exp;
  		}
	}

	if (sample > MP3_FRAC_MAX)
	{
		sample = MP3_FRAC_MAX;
	}
	else if (sample < MP3_FRAC_MIN)
	{
		sample = MP3_FRAC_MIN;
	}

	_ASSERT(sample >= 0);

	if (frac)
	{
		sample = MP3_FRAC_MUL(sample, mp3_rq_powers[3 + frac]);
	}
	
	return (mp3_frac) sample;
}
#endif

/*
// decode huffman codes
*/
static unsigned int mp3_layer3_huffmandecode
(
	unsigned int gr, 
	unsigned int ch, 
	unsigned char const *sfbwidth, 
	unsigned char mp3_scalefac[2][2][39], 
	int bits_to_decode,
	mp3_sideinfo_t *si
)
{

	#define MP3_CACHE_BIG_VALUES
	int sfb_count;
	int exp;
	int exponents[39];
	int const *p_exp;
	/*unsigned int sample_count;*/
	mp3_channel_t *channel = &si->granule[gr].channel[ch];
	mp3_frac *xr;
	mp3_frac *p_xr;
   	mp3_frac sample;

	/*
	// since we're only writting to this buffer we
	// can assume that DSWPAG is already set to 1 and
	// and use a standard pointer
	*/
	#if 1 && defined(__dsPIC33E__)
	xr = (mp3_frac*) __builtin_edsoffset(mp3_xr);
	xr += (ch * 576);
	p_xr = &xr[0];
	#else
	xr = &mp3_xr[ch][0];
	p_xr = &xr[0];
	#endif

	/*
	// calculate exponents
	*/
	{
		int gain;
		unsigned int scalefac_multiplier, sfb;
		gain = (int) channel->global_gain - 210;
		scalefac_multiplier = (channel->scalefac_scale) ? 2 : 1;
		
		if (si->granule[gr].channel[ch].block_type == 2) 
		{
			unsigned int l;
	  		register int gain0, gain1, gain2;
		
	  		sfb = l = 0;
		
	  		if (channel->mixed_block) 
			{
				if (channel->preflag)
				{
					while (l < 36) 
					{
						exponents[sfb] = gain - (int)((mp3_scalefac[gr][ch][sfb] + mp3_pretab[sfb]) << scalefac_multiplier);
						l += sfbwidth[sfb++];
					}
				}
				else
				{
					while (l < 36) 
					{
						exponents[sfb] = gain - (int)(mp3_scalefac[gr][ch][sfb] << scalefac_multiplier);
						l += sfbwidth[sfb++];
					}

				}
	  		}
		
	  		gain0 = gain - 8 * (int) channel->subblock_gain[0];
	  		gain1 = gain - 8 * (int) channel->subblock_gain[1];
	  		gain2 = gain - 8 * (int) channel->subblock_gain[2];
		
	  		while (l < 576) 
			{
				exponents[sfb + 0] = gain0 - (int) (mp3_scalefac[gr][ch][sfb + 0] << scalefac_multiplier);
				exponents[sfb + 1] = gain1 - (int) (mp3_scalefac[gr][ch][sfb + 1] << scalefac_multiplier);
				exponents[sfb + 2] = gain2 - (int) (mp3_scalefac[gr][ch][sfb + 2] << scalefac_multiplier);
		
	    		l += 3 * sfbwidth[sfb];
	    		sfb += 3;
	  		}
		}
		else 
		{  
			if (channel->preflag) 
			{
	    		for (sfb = 0; sfb < 22; sfb++) 
				{
					exponents[sfb] = gain - (int) ((mp3_scalefac[gr][ch][sfb] + mp3_pretab[sfb]) << scalefac_multiplier);
	    		}
	  		}
	  		else 
			{
	    		for (sfb = 0; sfb < 22; sfb++) 
				{
					exponents[sfb] = gain - (int) (mp3_scalefac[gr][ch][sfb] << scalefac_multiplier);
	    		}
	  		}
		}
	}

	/*
	// decode big_values
	*/
	{
		unsigned int region;
		unsigned int rcount;
	  	unsigned int linbits; 
	  	unsigned int startbits;
	  	unsigned int big_values;
	  	mp3_huff_table_t const *entry;
	  	mp3_huff_pair_t const *table;

		#if defined(MP3_CACHE_BIG_VALUES)
	  	mp3_frac big_values_cache[16];
	  	unsigned int big_values_hits;
		#endif
		sfb_count = (int) *sfbwidth++;
	  	rcount  = channel->region0_count + 1;
	  	entry = &mp3_huff_pair_table[channel->table_select[region = 0]];
	  	table = entry->table;
	  	linbits = entry->linbits;
	  	startbits = entry->startbits;
	
	  	if (table == 0)
	    	return MP3_ERROR_INVALID_BITSTREAM;
	
		p_exp = &exponents[0];
		exp = *p_exp++;
		#if defined(MP3_CACHE_BIG_VALUES)
		big_values_hits = 0;
		#endif		
		big_values = channel->big_values;
		
	  	while (big_values-- && bits_to_decode > 0) 
		{
	    	register mp3_huff_pair_t pair;
	    	unsigned int numbits, value;
	
			/*
			// check if we've reached a scalefactor band boundary
			*/
	    	if (sfb_count <= 0) 
		    {
				sfb_count = (int) *sfbwidth++;
				/*
				// check if we've reacched a region boundry and
				// change the huffman atble
				*/
				if (--rcount == 0) 
				{
	 				if (region == 0)
	 				{
	   					rcount = channel->region1_count + 1;
	   				}
	 				else
	 				{	
	   					rcount = 0;
	   				}
	
					entry = &mp3_huff_pair_table[channel->table_select[++region]];
					table = entry->table;
					linbits = entry->linbits;
					startbits = entry->startbits;
					
					if (table == 0)
					  	return MP3_ERROR_INVALID_BITSTREAM;
				}
	
				if (exp != *p_exp) 
				{
					exp = *p_exp;
					#if defined(MP3_CACHE_BIG_VALUES)
					big_values_hits = 0;
					#endif
				}
	
				p_exp++;
	    	}
	
			pair = table[mp3_stream_peek(&mp3_bitstream, numbits = startbits)];
	
	    	while (!pair.final) 
		    {
				bits_to_decode -= numbits;
				mp3_stream_skip(&mp3_bitstream, numbits);
				numbits = pair.hlen;
				pair = table[pair.data.offset + mp3_stream_peek(&mp3_bitstream, numbits)];
	    	}
	
			mp3_stream_skip(&mp3_bitstream, pair.hlen);
	    	bits_to_decode -= pair.hlen;
	
	    	if (linbits) 
		    {
				value = pair.data.values.x;
	
				if (!value)
				{
			 		*p_xr++ = 0;
				}
				else if (value == 15)
				{
					value += mp3_stream_read_fast(&mp3_bitstream, linbits);
					sample = mp3_layer3_rq_sample(value, exp);
					bits_to_decode -= linbits + 1;

					if (mp3_stream_read_bool_fast(&mp3_bitstream))	/* xsign */
					{
						*p_xr++ = -sample;
					}
					else
					{
						*p_xr++ = sample;
					}
				}
				else
				{
					#if defined(MP3_CACHE_BIG_VALUES)
					if (big_values_hits & (1 << value))
					{
						sample = big_values_cache[value];
					}
					else 
					{
						big_values_hits |= (1 << value);
						sample = mp3_layer3_rq_sample(value, exp);
						big_values_cache[value] = sample;
					}
					#else
					sample = mp3_layer3_rq_sample(value, exp);
					#endif

					if (mp3_stream_read_bool_fast(&mp3_bitstream))	/* xsign */
					{
						*p_xr++ = -sample;
					}
					else
					{
						*p_xr++ = sample;
					}
					bits_to_decode--;
				}
	
				value = pair.data.values.y;
				
				if (!value)
				{
			 		*p_xr++ = 0;
				}
				else if (value == 15)
				{
			 		value += mp3_stream_read_fast(&mp3_bitstream, linbits);
			 		sample = mp3_layer3_rq_sample(value, exp);
			 		bits_to_decode -= linbits + 1;

					if (mp3_stream_read_bool_fast(&mp3_bitstream))	/* ysign */
					{
						*p_xr++ = -sample;
					}
					else
					{
						*p_xr++ = sample;
					}
				}
				else
				{
					#if defined(MP3_CACHE_BIG_VALUES)
			 		if (big_values_hits & (1 << value))
			 		{
			   			sample = big_values_cache[value];
			   		}
			 		else 
				 	{
			   			big_values_hits |= (1 << value);
			   			sample = mp3_layer3_rq_sample(value, exp);
			   			big_values_cache[value] = sample;
			 		}
					#else
		   			sample = mp3_layer3_rq_sample(value, exp);
					#endif
			
					if (mp3_stream_read_bool_fast(&mp3_bitstream))	/* ysign */
					{
						*p_xr++ = -sample;
					}
					else
					{
						*p_xr++ = sample;
					}
					
					bits_to_decode--;
				}
	    	}
	    	else 
		    {
				value = pair.data.values.x;
	
				if (value == 0)
				{
	 				*p_xr++ = 0;
	 			}
				else 
				{
					#if defined(MP3_CACHE_BIG_VALUES)
	 				if (big_values_hits & (1 << value))
	 				{
	   					sample = big_values_cache[value];
	   				}
	 				else 
		 			{
	   					big_values_hits |= (1 << value);
	   					sample = mp3_layer3_rq_sample(value, exp);
	   					big_values_cache[value] = sample;
	 				}
					#else
   					sample = mp3_layer3_rq_sample(value, exp);
					#endif
	
					if (mp3_stream_read_bool_fast(&mp3_bitstream))	/* xsign */
					{
						*p_xr++ = -sample;
					}
					else
					{
						*p_xr++ = sample;
					}
					bits_to_decode--;
				}
	
				value = pair.data.values.y;
	
				if (value == 0)
				{
	 				*p_xr++ = 0;
	 			}
				else 
				{
					#if defined(MP3_CACHE_BIG_VALUES)
	 				if (big_values_hits & (1 << value))
	 				{
	   					sample = big_values_cache[value];
	   				}
	 				else 
		 			{
	   					big_values_hits |= (1 << value);
	   					sample = mp3_layer3_rq_sample(value, exp);
	   					big_values_cache[value] = sample;
	 				}
					#else
   					sample = mp3_layer3_rq_sample(value, exp);
					#endif
	
					if (mp3_stream_read_bool_fast(&mp3_bitstream))	/* ysign */
					{
						*p_xr++ = -sample;
					}
					else
					{
						*p_xr++ = sample;
					}
					bits_to_decode--;
				}
	    	}
			sfb_count -= 2;
	  	}
	}
	
	if (bits_to_decode < 0)
	{
		return MP3_ERROR_INVALID_BITSTREAM;
	}
	
	/*
	// decode count1
	*/
	{
		unsigned int numbits = channel->count1table_select ? 4 : 6;
	  	mp3_huff_quad_t const *table = mp3_huff_quad_table[channel->count1table_select];
	  	sample = mp3_layer3_rq_sample(1, exp);
	
	  	while (bits_to_decode > 0 && p_xr <= &xr[572]) 
		{
	    	register const mp3_huff_quad_t quad = table[mp3_stream_peek(&mp3_bitstream, numbits)];
			mp3_stream_skip(&mp3_bitstream, quad.hlen);
	    	bits_to_decode -= quad.hlen;
	
	    	if (sfb_count <= 0) 
		    {
				sfb_count = (int) *sfbwidth++;
	
				if (exp != *p_exp) 
				{
	 				exp = *p_exp;
	 				sample = mp3_layer3_rq_sample(1, exp);
				}
	
				++p_exp;
	    	}
	
			if (quad.v)
			{
				if (mp3_stream_read_bool_fast(&mp3_bitstream))
				{
					*p_xr++ = -sample;
				}
				else
				{
					*p_xr++ = sample;
				}
				bits_to_decode--;
			}
			else
			{
				*p_xr++ = 0;
			}
	
			if (quad.w)
			{
				if (mp3_stream_read_bool_fast(&mp3_bitstream))
				{
					*p_xr++ = -sample;
				}
				else
				{
					*p_xr++ = sample;
				}
				bits_to_decode--;
			}
			else
			{
				*p_xr++ = 0;
			}

			sfb_count -= 2;

	    	if (sfb_count <= 0) 
		    {
				sfb_count = (int) *sfbwidth++;
	
				if (exp != *p_exp) 
				{
	 				exp = *p_exp;
	 				sample = mp3_layer3_rq_sample(1, exp);
				}
	
				p_exp++;
	    	}
	
			if (quad.x)
			{
				if (mp3_stream_read_bool_fast(&mp3_bitstream))
				{
					*p_xr++ = -sample;
				}
				else
				{
					*p_xr++ = sample;
				}
				bits_to_decode--;
			}
			else
			{
				*p_xr++ = 0;
			}

			if (quad.y)
			{
				if (mp3_stream_read_bool_fast(&mp3_bitstream))
				{
					*p_xr++ = -sample;
				}
				else
				{
					*p_xr++ = sample;
				}
				bits_to_decode--;
			}
			else
			{
				*p_xr++ = 0;
			}
			sfb_count -= 2;
	  	}
	
	  	if (bits_to_decode < 0) 
		{
			_ASSERT(-bits_to_decode <= MP3_BUFFER_GUARD * 8);
			mp3_stream_rewind(&mp3_bitstream, -bits_to_decode);	
	    	p_xr -= 4;
	  	}
	}
	
	/*
	// "decode" rzero
	*/
	while (p_xr < &xr[576]) 
	{
		*p_xr++ = 0;
	  	*p_xr++ = 0;
	}
	
	return MP3_SUCCESS;
}

#if 0 || !(defined(__XC16__) || defined(__C30__))
/*
// reorder frequency lines
*/
static void mp3_layer3_reorder(unsigned int ch, unsigned char const sfbwidth[39], unsigned int mixed_block)
{
	mp3_frac (*tmp)[32][3][6] = (void*) &mp3_out->xr_temp[1][1][0];
	unsigned int sb = 0, l, f, w, sbw[3], sw[3];

	if (mixed_block) 
	{
		sb = 2;
		l = 0;

		while (l < 36)
		{
	    	l += *sfbwidth++;
	    }
	}
	
  	sw[0] = 0;
  	sw[1] = 0;
  	sw[2] = 0;
	sbw[0] = sb;
	sbw[1] = sb;
	sbw[2] = sb;
	
	f = *sfbwidth++;
	w = 0;
	
	for (l = sb * 18; l < 576; l++) 
	{
		if (f-- == 0) 
		{
	    	f = *sfbwidth++ - 1;
			if (++w == 3)
				w = 0;
	  	}
	
	  	(*tmp)[ sbw[w] ][ w ][ sw[w]++ ] = mp3_xr[ch][l];
	
	  	if (sw[w] == 6) 
		{
	    	sw[w] = 0;
	    	sbw[w]++;
	  	}
	}
	
	memcpy(&mp3_xr[ch][18 * sb], &(*tmp)[sb], (576 - 18 * sb) * sizeof(mp3_frac));
}
#endif

static inline void mp3_layer3_ms_stereo(unsigned char const *sfbwidth, int *modes)
{
	unsigned int sfb, i, l;

	if (mp3_header.mode_extension & MP3_MS_STEREO) 
	{
		mp3_header.ms_stereo = 1;
	
	  	for (sfb = l = 0; l < 576; sfb++) 
		{
	    	unsigned int n = sfbwidth[sfb];
	
	    	if (modes[sfb] == MP3_MS_STEREO)
			{	
	    		for (i = 0; i < n; i++) 
				{
					mp3_frac m = mp3_xr[0][l + i];
					mp3_frac s = mp3_xr[1][l + i];
					mp3_xr[0][l + i] = MP3_FRAC_MUL(m + s, 0x2d41);
					mp3_xr[1][l + i] = MP3_FRAC_MUL(m - s, 0x2d41);
	    		}
			}
			l += n;
	  	}
	}
}

/*
// joint stereo processing
*/
static unsigned int mp3_layer3_stereo
(
	unsigned int gr, 
	unsigned char const *sfbwidth, 
	unsigned char mp3_scalefac[2][2][39],
	mp3_sideinfo_t *si
)
{
	int modes[39];
	unsigned int sfb, i, l, n;
	mp3_granule_t const *granule = &si->granule[gr];

	if (granule->channel[0].block_type != granule->channel[1].block_type ||
	    (granule->channel[0].mixed_block) != (granule->channel[1].mixed_block))
	{
		return MP3_ERROR_INVALID_BITSTREAM;
	}
	
	for (i = 0; i < 39; i++)
	{
		modes[i] = mp3_header.mode_extension;
	}
	
	if (mp3_header.mode_extension & MP3_MODEEXT_I_STEREO) 
	{
		mp3_channel_t const *right_ch = &granule->channel[1];
		unsigned int right_xr_index = 0;
	  	unsigned int is_pos;
	
		mp3_header.intensity_stereo = 1;
	
	  	if (right_ch->block_type == 2) 
		{		  	
	    	unsigned int lower = 0, start = 0, max = 0, w = 0;
			unsigned int bound[3] = { 0, 0, 0 };
	
	    	sfb = 0;
			l = 0;
	
	    	if (right_ch->mixed_block) 
		    {
				while (l < 36) 
				{
	 				n = sfbwidth[sfb++];
	
	 				for (i = 0; i < n; i++) 
		 			{
	   					if (mp3_xr[1][i + right_xr_index]) 
		   				{
	     					lower = sfb;
	     					break;
	   					}
	 				}
	
	 				right_xr_index += n;
	 				l += n;
				}
	
				start = sfb;
	    	}
	
	    	while (l < 576) 
		    {
				n = sfbwidth[sfb++];
	
				for (i = 0; i < n; i++) 
				{
	 				if (mp3_xr[1][i + right_xr_index]) 
		 			{
	   					max = bound[w] = sfb;
	   					break;
	 				}
				}
	
				right_xr_index += n;
				l += n;
				if (++w == 3)
					w = 0;
	    	}
	
	    	if (max)
				lower = start;
	
	    	for (i = 0; i < lower; ++i)
	    	{
				modes[i] = mp3_header.mode_extension & ~MP3_MODEEXT_I_STEREO;
			}
	
	    	for (w = 0, i = start; i < max; ++i) 
		    {
				if (i < bound[w])
				{
	 				modes[i] = mp3_header.mode_extension & ~MP3_MODEEXT_I_STEREO;
				}
	
				if (++w == 3)
					w = 0;
	    	}
	  	}
	  	else 
		{  
	    	unsigned int bound;
	
	    	bound = 0;
	    	for (sfb = l = 0; l < 576; l += n) 
		    {
				n = sfbwidth[sfb++];
	
				for (i = 0; i < n; ++i) 
				{
	 				if (mp3_xr[1][i + right_xr_index]) 
		 			{
	   					bound = sfb;
	   					break;
	 				}
				}
	
				right_xr_index += n;
	    	}
	
	    	for (i = 0; i < bound; i++)
			{
				modes[i] = mp3_header.mode_extension & ~MP3_MODEEXT_I_STEREO;
			}
	  	}
	
		if (mp3_header.lsf_ext)
		{
			unsigned char const *illegal_pos = &mp3_scalefac[1][1][0];
	    	mp3_frac const *lsf_scale = is_lsf_table[right_ch->scalefac_compress & 0x1];
	
	    	for (sfb = l = 0; l < 576; sfb++) 
		    {
				n = sfbwidth[sfb];
	
				if (!(modes[sfb] & MP3_MODEEXT_I_STEREO))
	 				continue;
	
				if (illegal_pos[sfb]) 
				{
	 				modes[sfb] &= ~MP3_MODEEXT_I_STEREO;
	 				continue;
				}
	
				is_pos = mp3_scalefac[gr][1][sfb];
	
				for (i = 0; i < n; ++i) 
				{
	 				register mp3_frac left;
	
	 				left = mp3_xr[0][l + i];
	
	 				if (is_pos == 0)
	 				{
	   					mp3_xr[1][l + i] = left;
	 				}
	 				else 
		 			{
	   					mp3_frac opposite = 
							MP3_FRAC_MUL(left, lsf_scale[(is_pos - 1) >> 1]);
	
	   					if (is_pos & 1) 
		   				{
	     					mp3_xr[0][l + i] = opposite;
	     					mp3_xr[1][l + i] = left;
	   					}
	   					else
	   					{
	     					mp3_xr[1][l + i] = opposite;
	     				}
	 				}
				}
				l += n;
	    	}
	  	}
	  	else 
		{  	
	    	for (sfb = l = 0; l < 576; sfb++, l += n) 
		    {
				n = sfbwidth[sfb];
	
				if (!(modes[sfb] & MP3_MODEEXT_I_STEREO))
	 				continue;
	
				is_pos = mp3_scalefac[gr][1][sfb];
	
				if (is_pos >= 7) 
				{  
	 				modes[sfb] &= ~MP3_MODEEXT_I_STEREO;
	 				continue;
				}
	
				for (i = 0; i < n; ++i) 
				{
	 				mp3_frac left = mp3_xr[0][l + i];
	 				mp3_xr[0][l + i] = MP3_FRAC_MUL(left, is_table[    is_pos]);
	 				mp3_xr[1][l + i] = MP3_FRAC_MUL(left, is_table[6 - is_pos]);
				}
	    	}
	  	}
	}
	
	mp3_layer3_ms_stereo(sfbwidth, modes);
	
	return MP3_SUCCESS;
}

/*
// alias reduction of frequency lines
*/
#if 0 || !(defined(__XC16__) || defined(__C30__))
static void mp3_layer3_aliasreduce(unsigned int ch, unsigned int lines)
{
	int i, c;
	for (c = 18; c < (int) lines; c += 18) 
	{
		/*
		// butterflies
		*/
		for (i = 0; i < 8; ++i) 
		{
	    	mp3_frac a = mp3_xr[ch][c - 1 - i];
	    	mp3_frac b = mp3_xr[ch][c + i];
	
			MP3_FRAC_MPY(a, mp3__cs[i]);
			MP3_FRAC_MAC(-b, mp3_ca[i]);
			mp3_xr[ch][c - 1 - i] = MP3_FRAC_SAC();

			MP3_FRAC_MPY(b, mp3__cs[i]);
			MP3_FRAC_MAC(a, mp3_ca[i]);
			mp3_xr[ch][c + i] = MP3_FRAC_SAC();
	  	}
	}
}
#endif

/*
// Long blocks IMDCT and windowing
*/
#if 0 || !(defined(__XC16__) || defined(__C30__))
static void mp3_layer3_imdct_l(unsigned int ch, unsigned int sb, unsigned int block_type)
{
	unsigned int i;
	mp3_frac const *imdct = mp3_imdct_l;
	MP3_XR_PTR mp3_frac *X = &mp3_xr[ch][sb * 18];
	
	MP3_FRAC_MPY(X[4],  *imdct++);
	MP3_FRAC_MAC(X[13], *imdct++);
	
	mp3_z[ 6] = MP3_FRAC_SAC();
	mp3_z[14] = X[1] - X[10];
	mp3_z[15] = X[7] + X[16];
	
	MP3_FRAC_MAC(mp3_z[14], *imdct++);
	MP3_FRAC_MAC(mp3_z[15], *imdct++);
	
	mp3_z[0] = MP3_FRAC_SAC();
	
	mp3_z[ 8] = X[0] - X[11] - X[12];
	mp3_z[ 9] = X[2] - X[ 9] - X[14];
	mp3_z[10] = X[3] - X[ 8] - X[15];
	mp3_z[11] = X[5] - X[ 6] - X[17];
	
	MP3_FRAC_MAC(mp3_z[8],  *imdct++);
	MP3_FRAC_MAC(mp3_z[9],  *imdct++);
	MP3_FRAC_MAC(mp3_z[10], *imdct++);
	MP3_FRAC_MAC(mp3_z[11], *imdct++);
	
	mp3_y[7]  = MP3_FRAC_SAC();
	mp3_y[10] = -mp3_y[7];
	
	MP3_FRAC_MPY(mp3_z[8],  *imdct++);
	MP3_FRAC_MAC(mp3_z[9],  *imdct++);
	MP3_FRAC_MAC(mp3_z[10], *imdct++);
	MP3_FRAC_MAC(mp3_z[11], *imdct++);
	
	mp3_y[19] = mp3_y[34] = MP3_FRAC_SAC() - mp3_z[0];
	
	mp3_z[12] = X[0] - X[3] + X[8] - X[11] - X[12] + X[15];
	mp3_z[13] = X[2] + X[5] - X[6] - X[ 9] - X[14] - X[17];
	
	MP3_FRAC_MPY(mp3_z[12], *imdct++);
	MP3_FRAC_MAC(mp3_z[13], *imdct++);
	
	mp3_y[22] = mp3_y[31] = MP3_FRAC_SAC() + mp3_z[0];
	
	MP3_FRAC_MPY(X[1],  *imdct++);
	MP3_FRAC_MAC(X[7],  *imdct++);
	MP3_FRAC_MAC(X[10], *imdct++);
	MP3_FRAC_MAC(X[16], *imdct++);
	
	mp3_z[1] = MP3_FRAC_SAC() + mp3_z[6];
	
	MP3_FRAC_MPY(X[0],  *imdct++);
	MP3_FRAC_MAC(X[3],  *imdct++);
	MP3_FRAC_MAC(X[6],  *imdct++);
	MP3_FRAC_MAC(X[9],  *imdct++);
	MP3_FRAC_MAC(X[12], *imdct++);
	MP3_FRAC_MAC(X[15], *imdct++);

	MP3_FRAC_MAC(X[2],  *imdct++);
	MP3_FRAC_MAC(X[5],  *imdct++);
	MP3_FRAC_MAC(X[8],  *imdct++);
	MP3_FRAC_MAC(X[11], *imdct++);
	MP3_FRAC_MAC(X[14], *imdct++);
	MP3_FRAC_MAC(X[17], *imdct++);
	
	mp3_y[6]  = MP3_FRAC_SAC() + mp3_z[1];
	mp3_y[11] = -mp3_y[6];
	
	MP3_FRAC_MPY(X[0],  *imdct++);
	MP3_FRAC_MAC(X[3],  *imdct++);
	MP3_FRAC_MAC(X[6],  *imdct++);
	MP3_FRAC_MAC(X[9],  *imdct++);
	MP3_FRAC_MAC(X[12], *imdct++);
	MP3_FRAC_MAC(X[15], *imdct++);

	MP3_FRAC_MAC(X[2],  *imdct++);
	MP3_FRAC_MAC(X[5],  *imdct++);
	MP3_FRAC_MAC(X[8],  *imdct++);
	MP3_FRAC_MAC(X[11], *imdct++);
	MP3_FRAC_MAC(X[14], *imdct++);
	MP3_FRAC_MAC(X[17], *imdct++);
	
	mp3_y[23] = mp3_y[30] = MP3_FRAC_SAC() + mp3_z[1];
	
	MP3_FRAC_MPY(X[0],  *imdct++);
	MP3_FRAC_MAC(X[3],  *imdct++);
	MP3_FRAC_MAC(X[6],  *imdct++);
	MP3_FRAC_MAC(X[9],  *imdct++);
	MP3_FRAC_MAC(X[12], *imdct++);
	MP3_FRAC_MAC(X[15], *imdct++);

	MP3_FRAC_MAC(X[2],  *imdct++);
	MP3_FRAC_MAC(X[5],  *imdct++);
	MP3_FRAC_MAC(X[8],  *imdct++);
	MP3_FRAC_MAC(X[11], *imdct++);
	MP3_FRAC_MAC(X[14], *imdct++);
	MP3_FRAC_MAC(X[17], *imdct++);
	
	mp3_y[18] = mp3_y[35] = MP3_FRAC_SAC() - mp3_z[1];
	
	MP3_FRAC_MPY(X[4],  *imdct++);
	MP3_FRAC_MAC(X[13], *imdct++);
	
	mp3_z[7] = MP3_FRAC_SAC();
	
	MP3_FRAC_MAC(X[1],  *imdct++);
	MP3_FRAC_MAC(X[7],  *imdct++);
	MP3_FRAC_MAC(X[10], *imdct++);
	MP3_FRAC_MAC(X[16], *imdct++);
	
	mp3_z[2] = MP3_FRAC_SAC();
	
	MP3_FRAC_MAC(X[2],  *imdct++);
	MP3_FRAC_MAC(X[5],  *imdct++);
	MP3_FRAC_MAC(X[8],  *imdct++);
	MP3_FRAC_MAC(X[11], *imdct++);
	MP3_FRAC_MAC(X[14], *imdct++);
	MP3_FRAC_MAC(X[17], *imdct++);

	MP3_FRAC_MAC(X[0],  *imdct++);
	MP3_FRAC_MAC(X[3],  *imdct++);
	MP3_FRAC_MAC(X[6],  *imdct++);
	MP3_FRAC_MAC(X[9],  *imdct++);
	MP3_FRAC_MAC(X[12], *imdct++);
	MP3_FRAC_MAC(X[15], *imdct++);
	
	mp3_y[5]  = MP3_FRAC_SAC();
	mp3_y[12] = -mp3_y[5];
	
	MP3_FRAC_MPY(X[0],  *imdct++);
	MP3_FRAC_MAC(X[3],  *imdct++);
	MP3_FRAC_MAC(X[6],  *imdct++);
	MP3_FRAC_MAC(X[9],  *imdct++);
	MP3_FRAC_MAC(X[12], *imdct++);
	MP3_FRAC_MAC(X[15], *imdct++);

	MP3_FRAC_MAC(X[2],  *imdct++);
	MP3_FRAC_MAC(X[5],  *imdct++);
	MP3_FRAC_MAC(X[8],  *imdct++);
	MP3_FRAC_MAC(X[11], *imdct++);
	MP3_FRAC_MAC(X[14], *imdct++);
	MP3_FRAC_MAC(X[17], *imdct++);
	
	mp3_y[0]  = MP3_FRAC_SAC() + mp3_z[2];
	mp3_y[17] = -mp3_y[0];
	
	MP3_FRAC_MPY(X[0],  *imdct++);
	MP3_FRAC_MAC(X[3],  *imdct++);
	MP3_FRAC_MAC(X[6],  *imdct++);
	MP3_FRAC_MAC(X[9],  *imdct++);
	MP3_FRAC_MAC(X[12], *imdct++);
	MP3_FRAC_MAC(X[15], *imdct++);

	MP3_FRAC_MAC(X[2],  *imdct++);
	MP3_FRAC_MAC(X[5],  *imdct++);
	MP3_FRAC_MAC(X[8],  *imdct++);
	MP3_FRAC_MAC(X[11], *imdct++);
	MP3_FRAC_MAC(X[14], *imdct++);
	MP3_FRAC_MAC(X[17], *imdct++);
	
	mp3_y[24] = mp3_y[29] = MP3_FRAC_SAC() + mp3_z[2];
	
	MP3_FRAC_MPY(X[1],  *imdct++);
	MP3_FRAC_MAC(X[7],  *imdct++);
	MP3_FRAC_MAC(X[10], *imdct++);
	MP3_FRAC_MAC(X[16], *imdct++);
	
	mp3_z[3] = MP3_FRAC_SAC() + mp3_z[7];
	
	MP3_FRAC_MPY(X[0],  *imdct++);
	MP3_FRAC_MAC(X[3],  *imdct++);
	MP3_FRAC_MAC(X[6],  *imdct++);
	MP3_FRAC_MAC(X[9],  *imdct++);
	MP3_FRAC_MAC(X[12], *imdct++);
	MP3_FRAC_MAC(X[15], *imdct++);

	MP3_FRAC_MAC(X[2],  *imdct++);
	MP3_FRAC_MAC(X[5],  *imdct++);
	MP3_FRAC_MAC(X[8],  *imdct++);
	MP3_FRAC_MAC(X[11], *imdct++);
	MP3_FRAC_MAC(X[14], *imdct++);
	MP3_FRAC_MAC(X[17], *imdct++);
	
	mp3_y[8] = MP3_FRAC_SAC() + mp3_z[3];
	
	mp3_y[9] = -mp3_y[8];
	
	MP3_FRAC_MPY(X[0],  *imdct++);
	MP3_FRAC_MAC(X[3],  *imdct++);
	MP3_FRAC_MAC(X[6],  *imdct++);
	MP3_FRAC_MAC(X[9],  *imdct++);
	MP3_FRAC_MAC(X[12], *imdct++);
	MP3_FRAC_MAC(X[15], *imdct++);
	
	MP3_FRAC_MAC(X[2],  *imdct++);
	MP3_FRAC_MAC(X[5],  *imdct++);
	MP3_FRAC_MAC(X[8],  *imdct++);
	MP3_FRAC_MAC(X[11], *imdct++);
	MP3_FRAC_MAC(X[14], *imdct++);
	MP3_FRAC_MAC(X[17], *imdct++);

	mp3_y[21] = mp3_y[32] = MP3_FRAC_SAC() + mp3_z[3];
	
	MP3_FRAC_MPY(X[0],  *imdct++);
	MP3_FRAC_MAC(X[3],  *imdct++);
	MP3_FRAC_MAC(X[6],  *imdct++);
	MP3_FRAC_MAC(X[9],  *imdct++);
	MP3_FRAC_MAC(X[12], *imdct++);
	MP3_FRAC_MAC(X[15], *imdct++);
	
	MP3_FRAC_MAC(X[2],  *imdct++);
	MP3_FRAC_MAC(X[5],  *imdct++);
	MP3_FRAC_MAC(X[8],  *imdct++);
	MP3_FRAC_MAC(X[11], *imdct++);
	MP3_FRAC_MAC(X[14], *imdct++);
	MP3_FRAC_MAC(X[17], *imdct++);

	mp3_y[20] = mp3_y[33] = MP3_FRAC_SAC() - mp3_z[3];
	
	MP3_FRAC_MPY(mp3_z[14], *imdct++);
	MP3_FRAC_MAC(mp3_z[15], *imdct++);
	
	mp3_z[4] = MP3_FRAC_SAC() - mp3_z[7];
	
	MP3_FRAC_MPY(mp3_z[12], *imdct++);
	MP3_FRAC_MAC(mp3_z[13], *imdct++);
	
	mp3_y[4]  = MP3_FRAC_SAC() + mp3_z[4];
	mp3_y[13] = -mp3_y[4];
	
	MP3_FRAC_MPY(mp3_z[8],  *imdct++);
	MP3_FRAC_MAC(mp3_z[9],  *imdct++);
	MP3_FRAC_MAC(mp3_z[10], *imdct++);
	MP3_FRAC_MAC(mp3_z[11], *imdct++);
	
	mp3_y[1]  = MP3_FRAC_SAC() + mp3_z[4];
	mp3_y[16] = -mp3_y[1];
	
	MP3_FRAC_MPY(mp3_z[8],  *imdct++);
	MP3_FRAC_MAC(mp3_z[9],  *imdct++);
	MP3_FRAC_MAC(mp3_z[10], *imdct++);
	MP3_FRAC_MAC(mp3_z[11], *imdct++);
	
	mp3_y[25] = mp3_y[28] = MP3_FRAC_SAC() + mp3_z[4];
	
	MP3_FRAC_MPY(X[1],  *imdct++);
	MP3_FRAC_MAC(X[7],  *imdct++);
	MP3_FRAC_MAC(X[10], *imdct++);
	MP3_FRAC_MAC(X[16], *imdct++);
	
	mp3_z[5] = MP3_FRAC_SAC() - mp3_z[6];
	
	MP3_FRAC_MPY(X[0],  *imdct++);
	MP3_FRAC_MAC(X[3],  *imdct++);
	MP3_FRAC_MAC(X[6],  *imdct++);
	MP3_FRAC_MAC(X[9],  *imdct++);
	MP3_FRAC_MAC(X[12], *imdct++);
	MP3_FRAC_MAC(X[15], *imdct++);
	
	MP3_FRAC_MAC(X[2],  *imdct++);
	MP3_FRAC_MAC(X[5],  *imdct++);
	MP3_FRAC_MAC(X[8],  *imdct++);
	MP3_FRAC_MAC(X[11], *imdct++);
	MP3_FRAC_MAC(X[14], *imdct++);
	MP3_FRAC_MAC(X[17], *imdct++);

	mp3_y[2]  = MP3_FRAC_SAC() + mp3_z[5];
	mp3_y[15] = -mp3_y[2];
	
	MP3_FRAC_MPY(X[0],  *imdct++);
	MP3_FRAC_MAC(X[3],  *imdct++);
	MP3_FRAC_MAC(X[6],  *imdct++);
	MP3_FRAC_MAC(X[9],  *imdct++);
	MP3_FRAC_MAC(X[12], *imdct++);
	MP3_FRAC_MAC(X[15], *imdct++);
	
	MP3_FRAC_MAC(X[2],  *imdct++);
	MP3_FRAC_MAC(X[5],  *imdct++);
	MP3_FRAC_MAC(X[8],  *imdct++);
	MP3_FRAC_MAC(X[11], *imdct++);
	MP3_FRAC_MAC(X[14], *imdct++);
	MP3_FRAC_MAC(X[17], *imdct++);

	mp3_y[3]  = MP3_FRAC_SAC() + mp3_z[5];
	mp3_y[14] = -mp3_y[3];
	
	MP3_FRAC_MPY(X[0],  *imdct++);
	MP3_FRAC_MAC(X[3],  *imdct++);
	MP3_FRAC_MAC(X[6],  *imdct++);
	MP3_FRAC_MAC(X[9],  *imdct++);
	MP3_FRAC_MAC(X[12], *imdct++);
	MP3_FRAC_MAC(X[15], *imdct++);
	
	MP3_FRAC_MAC(X[2],  *imdct++);
	MP3_FRAC_MAC(X[5],  *imdct++);
	MP3_FRAC_MAC(X[8],  *imdct++);
	MP3_FRAC_MAC(X[11], *imdct++);
	MP3_FRAC_MAC(X[14], *imdct++);
	MP3_FRAC_MAC(X[17], *imdct++);

	mp3_y[26] = mp3_y[27] = MP3_FRAC_SAC() + mp3_z[5];
	
	if (block_type == 0)
	{
		for (i = 0; i < 36; i++) mp3_z[i] = MP3_FRAC_MUL(mp3_y[i], mp3_window_l[i]);
	}
	else if (block_type == 1)	/* start window */
	{
  		for (i =  0; i < 18; i++) mp3_z[i] = MP3_FRAC_MUL(mp3_y[i], mp3_window_l[i]);
		for (i = 18; i < 24; i++) mp3_z[i] = mp3_y[i];
		for (i = 24; i < 30; i++) mp3_z[i] = MP3_FRAC_MUL(mp3_y[i], mp3_window_s[i - 18]);
		for (i = 30; i < 36; i++) mp3_z[i] = 0;
	}
	else if (block_type == 3)	/* stop window */
	{
		for (i =  0; i <  6; i++) mp3_z[i] = 0;
		for (i =  6; i < 12; i++) mp3_z[i] = MP3_FRAC_MUL(mp3_y[i], mp3_window_s[i - 6]);
		for (i = 12; i < 18; i++) mp3_z[i] = mp3_y[i];
		for (i = 18; i < 36; i++) mp3_z[i] = MP3_FRAC_MUL(mp3_y[i], mp3_window_l[i]);
	}
}
#endif

/*
// short blocks IMDCT and windowing
//
// note: the dsPIC ASM version of this function requires
// mp3_y[] to be placed in ymemory on dsPIC devices
// and will generate an address error otherwise.
//
// The C version, if used in dsPIC33E devices requires
// mp3_xr[], mp3_y[], and mp3_z[] to be allocated on the
// base address space.
*/
#if 0 || !(defined(__XC16__) || defined(__C30__))
static void mp3_layer3_imdct_s(unsigned int ch, unsigned int sb)
{
	MP3_XR_PTR mp3_frac* xr;
	MP3_Y_PTR mp3_frac* y;
	mp3_frac const *w;
	int l, i;
	
	xr = &mp3_xr[ch][sb * 18];
	y = &mp3_y[0];
	
	/*
	// imdct
	*/
	for (l = 0; l < 3; l++) 
	{
		mp3_frac const *imdct = mp3_imdct_s;
		
		for (i = 0; i < 3; i++) 
		{
			MP3_FRAC_MPY(xr[0], *imdct++);
			MP3_FRAC_MAC(xr[1], *imdct++);
			MP3_FRAC_MAC(xr[2], *imdct++);
			MP3_FRAC_MAC(xr[3], *imdct++);
			MP3_FRAC_MAC(xr[4], *imdct++);
			MP3_FRAC_MAC(xr[5], *imdct++);
			y[i] = MP3_FRAC_SAC();
			y[5 - i] = -y[i];
			
			MP3_FRAC_MPY(xr[0], *imdct++);
			MP3_FRAC_MAC(xr[1], *imdct++);
			MP3_FRAC_MAC(xr[2], *imdct++);
			MP3_FRAC_MAC(xr[3], *imdct++);
			MP3_FRAC_MAC(xr[4], *imdct++);
			MP3_FRAC_MAC(xr[5], *imdct++);
			y[i + 6] = MP3_FRAC_SAC();
			y[11 - i] = y[i + 6];
		}
		
		y += 12;
		xr += 6;
	}
	
	y = &mp3_y[0];
	w = &mp3_window_s[0];
	
	/*
	// windowing
	*/
	for (i = 0; i < 6; i++) 
	{
		mp3_z[i] = 0;
		mp3_z[i +  6] = MP3_FRAC_MUL(y[0], w[0]);
		
		MP3_FRAC_MPY(y[ 6], w[6]);
		MP3_FRAC_MAC(y[12], w[0]);
		mp3_z[i + 12] = MP3_FRAC_SAC();
		
		MP3_FRAC_MPY(y[18], w[6]);
		MP3_FRAC_MAC(y[24], w[0]);
		mp3_z[i + 18] = MP3_FRAC_SAC();
		
		mp3_z[i + 24] = MP3_FRAC_MUL(y[30], w[6]);
		mp3_z[i + 30] = 0;
		
		y++;
		w++;
	}
}
#endif

/*
// overlap add of windowed IMDCT data
*/
#if 0 || !(defined(__XC16__) || defined(__C30__))
static void mp3_layer3_overlap(unsigned int gr, unsigned int ch, unsigned int sb)
{
	unsigned int i;
	for (i = 0; i < 18; ++i) 
	{
		mp3_out->sb_samples[ch][(18 * gr) + i][sb] = mp3_z[i] + mp3_overlap[ch][sb][i];
		mp3_overlap[ch][sb][i] = mp3_z[i + 18];
	}
}
#endif

/*
// overlap add of zero IMDCT data
*/
#if 0 || !(defined(__XC16__) || defined(__C30__))
static void mp3_layer3_overlap_z(unsigned int gr, unsigned int ch, unsigned int sb)
{
	unsigned int i;
	for (i = 0; i < 18; ++i) 
	{
		mp3_out->sb_samples[ch][(18 * gr) + i][sb] = mp3_overlap[ch][sb][i];
		mp3_overlap[ch][sb][i] = 0;
	}
}
#endif

/*
// subband freq invert
*/
#if 0 || !(defined(__XC16__) || defined(__C30__))
static void mp3_layer3_invfreq(unsigned int gr, unsigned int ch, unsigned int sb)
{
	unsigned int i;
	for (i = 1; i < 18; i += 2)
	{
		mp3_out->sb_samples[ch][(18 * gr) + i][sb] = -mp3_out->sb_samples[ch][(18 * gr) + i][sb];
	}
}
#endif

