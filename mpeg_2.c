/*
 * mpeg_2.c
 *
 * Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
 * All rights reserved
 *
 */

#include <stdlib.h>
#include "compiler.h"
#include "mpeg_2_fractional.h"
#include "mpeg_2_streamreader.h"
#include "mpeg_2.h"
#include "mpeg_2__1.h"
#include "mpeg_2__2.h"
#include "mpeg_2__3.h"


#if defined(__XC16__) || defined(__C30__)
	mp3_stream_t mp3_bitstream __attribute__((__near__, section(".mp3")));
	mp3_stream_t mp3_anc_bitstream __attribute__((__near__, section(".mp3")));
	mp3_buffers_t *mp3_out __attribute__((__near__, section(".mp3")));
	mp3_header_t mp3_header __attribute__((__near__, section(".mp3")));
	unsigned int mp3_last_error __attribute__((__near__, section(".mp3")));
	unsigned int mp3_anc_bitstream_len __attribute__((__near__, section(".mp3")));
	unsigned char const *mp3_next_frame __attribute__((__near__, section(".mp3")));
	static unsigned char const *mp3_current_frame __attribute__((__near__, section(".mp3")));
	unsigned char mp3_bitstream_buffer[MP3_BITSTREAM_BUFFER_LEN] __attribute__((section(".mp3_bitstream")));
	unsigned int mp3_bitstream_len __attribute__((__near__, section(".mp3")));
	static unsigned long mp3_freerate __attribute__((__near__, section(".mp3")));
	static unsigned char mp3_sync __attribute__((__near__, section(".mp3")));
	static unsigned char mp3_first_sync __attribute__((__near__, section(".mp3")));
#else
	mp3_stream_t mp3_bitstream;									/* bitstream pointer */
	mp3_stream_t mp3_anc_bitstream;								/* ancillary bits pointer */
	mp3_buffers_t *mp3_out;										/* output buffer */
	mp3_header_t mp3_header;									/* header */
	unsigned int mp3_last_error;								/* last error */
	unsigned int mp3_anc_bitstream_len;							/* number of ancillary bits */
	unsigned char const *mp3_next_frame;						/* start of next frame */
	static unsigned char const *mp3_current_frame;				/* start of current frame */
	unsigned char mp3_bitstream_buffer[MP3_BITSTREAM_BUFFER_LEN];		/* bitstream buffer */
	unsigned int mp3_bitstream_len;								/* bytes in bitstream buffer */
	static unsigned long mp3_freerate;							/* free bitrate (fixed) */
	static unsigned char mp3_sync;								/* stream sync found */
	static unsigned char mp3_first_sync;
#endif

/*
// bitrate table
*/
static unsigned long const bitrate_table[5][15] = 
{
	/*
	// layer 1
	*/ 
	{ 
		0,  32000,  64000,  96000, 128000, 160000, 192000, 224000,
		   256000, 288000, 320000, 352000, 384000, 416000, 448000 
	},
	/*
	// layer 2
	*/
	{ 
		0,  32000,  48000,  56000,  64000,  80000,  96000, 112000,
		   128000, 160000, 192000, 224000, 256000, 320000, 384000 
	},
	/*
	// layer 3
	*/
	{ 
		0,  32000,  40000,  48000,  56000,  64000,  80000,  96000,
		   112000, 128000, 160000, 192000, 224000, 256000, 320000 
	},
	/*
	// layer 1 LSF 
	*/
	{ 
		0,  32000,  48000,  56000,  64000,  80000,  96000, 112000, 
		   128000, 144000, 160000, 176000, 192000, 224000, 256000 
	},
	/*
	// layers 2 & 3 LSF
	*/
	{
		0,   8000,  16000,  24000,  32000,  40000,  48000,  56000,
			64000,  80000,  96000, 112000, 128000, 144000, 160000 
	}
};

/*
// samplerate table
*/
static unsigned int const samplerate_table[3] = 
{ 
	44100, 48000, 32000 
};

/*
// scalefactor table for layers 1 and 2
*/
mp3_frac const mp3_sf_table[64] = 
{
	0x7fff, 0x6598, 0x50a3, 0x4000, 0x32cc, 0x2851, 0x2000, 0x1966, 
	0x1429, 0x1000, 0x0cb3, 0x0a14, 0x0800, 0x0659, 0x050a, 0x0400, 
	0x032d, 0x0285, 0x0200, 0x0196, 0x0143, 0x0100, 0x00cb, 0x00a1, 
	0x0080, 0x0066, 0x0051, 0x0040, 0x0033, 0x0028, 0x0020, 0x0019, 
	0x0014, 0x0010, 0x000d, 0x000a, 0x0008, 0x0006, 0x0005, 0x0004, 
	0x0003, 0x0003, 0x0002, 0x0002, 0x0001, 0x0001, 0x0001, 0x0001, 
	0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

/*
// CRC lookup table
*/
static unsigned short const mp3_crc_table[256] = 
{
	0x0000, 0x8005, 0x800f, 0x000a, 0x801b, 0x001e, 0x0014, 0x8011,
	0x8033, 0x0036, 0x003c, 0x8039, 0x0028, 0x802d, 0x8027, 0x0022,
	0x8063, 0x0066, 0x006c, 0x8069, 0x0078, 0x807d, 0x8077, 0x0072,
	0x0050, 0x8055, 0x805f, 0x005a, 0x804b, 0x004e, 0x0044, 0x8041,
	0x80c3, 0x00c6, 0x00cc, 0x80c9, 0x00d8, 0x80dd, 0x80d7, 0x00d2,
	0x00f0, 0x80f5, 0x80ff, 0x00fa, 0x80eb, 0x00ee, 0x00e4, 0x80e1,
	0x00a0, 0x80a5, 0x80af, 0x00aa, 0x80bb, 0x00be, 0x00b4, 0x80b1,
	0x8093, 0x0096, 0x009c, 0x8099, 0x0088, 0x808d, 0x8087, 0x0082,
	0x8183, 0x0186, 0x018c, 0x8189, 0x0198, 0x819d, 0x8197, 0x0192,
	0x01b0, 0x81b5, 0x81bf, 0x01ba, 0x81ab, 0x01ae, 0x01a4, 0x81a1,
	0x01e0, 0x81e5, 0x81ef, 0x01ea, 0x81fb, 0x01fe, 0x01f4, 0x81f1,
	0x81d3, 0x01d6, 0x01dc, 0x81d9, 0x01c8, 0x81cd, 0x81c7, 0x01c2,
	0x0140, 0x8145, 0x814f, 0x014a, 0x815b, 0x015e, 0x0154, 0x8151,
	0x8173, 0x0176, 0x017c, 0x8179, 0x0168, 0x816d, 0x8167, 0x0162,
	0x8123, 0x0126, 0x012c, 0x8129, 0x0138, 0x813d, 0x8137, 0x0132,
	0x0110, 0x8115, 0x811f, 0x011a, 0x810b, 0x010e, 0x0104, 0x8101,
	0x8303, 0x0306, 0x030c, 0x8309, 0x0318, 0x831d, 0x8317, 0x0312,
	0x0330, 0x8335, 0x833f, 0x033a, 0x832b, 0x032e, 0x0324, 0x8321,
	0x0360, 0x8365, 0x836f, 0x036a, 0x837b, 0x037e, 0x0374, 0x8371,
	0x8353, 0x0356, 0x035c, 0x8359, 0x0348, 0x834d, 0x8347, 0x0342,
	0x03c0, 0x83c5, 0x83cf, 0x03ca, 0x83db, 0x03de, 0x03d4, 0x83d1,
	0x83f3, 0x03f6, 0x03fc, 0x83f9, 0x03e8, 0x83ed, 0x83e7, 0x03e2,
	0x83a3, 0x03a6, 0x03ac, 0x83a9, 0x03b8, 0x83bd, 0x83b7, 0x03b2,
	0x0390, 0x8395, 0x839f, 0x039a, 0x838b, 0x038e, 0x0384, 0x8381,
	0x0280, 0x8285, 0x828f, 0x028a, 0x829b, 0x029e, 0x0294, 0x8291,
	0x82b3, 0x02b6, 0x02bc, 0x82b9, 0x02a8, 0x82ad, 0x82a7, 0x02a2,
	0x82e3, 0x02e6, 0x02ec, 0x82e9, 0x02f8, 0x82fd, 0x82f7, 0x02f2,
	0x02d0, 0x82d5, 0x82df, 0x02da, 0x82cb, 0x02ce, 0x02c4, 0x82c1,
	0x8243, 0x0246, 0x024c, 0x8249, 0x0258, 0x825d, 0x8257, 0x0252,
	0x0270, 0x8275, 0x827f, 0x027a, 0x826b, 0x026e, 0x0264, 0x8261,
	0x0220, 0x8225, 0x822f, 0x022a, 0x823b, 0x023e, 0x0234, 0x8231,
	0x8213, 0x0216, 0x021c, 0x8219, 0x0208, 0x820d, 0x8207, 0x0202
};

/*
// prototypes
*/
static void mp3_header_init(void);
static unsigned int mp3_get_free_bitrate(void);
static unsigned int mp3_decode_header_internal(mp3_stream_t *ptr, mp3_header_t *header);
static unsigned int mp3_bitstream_sync(void);
unsigned short mp3_crc_check(mp3_stream_t, unsigned int, unsigned short);

/*
// initialize decoder
*/
void mp3_init(void)
{
	mp3_header_init();
	mp3_layer1_init();
	mp3_layer2_init();
	mp3_layer3_init();

	mp3_sync = 0;
	mp3_first_sync = 1;
	mp3_freerate = 0;
	mp3_bitstream_len = 0;
	mp3_current_frame = 0;
	mp3_next_frame = 0;
	mp3_anc_bitstream_len = 0;
	mp3_last_error = MP3_SUCCESS;

	mp3_stream_init(&mp3_bitstream, 0);
	mp3_stream_init(&mp3_anc_bitstream, 0);
}

/*
// initialize header
*/
static void mp3_header_init(void)
{
	mp3_header.layer = 0;
	mp3_header.mode = 0;
	mp3_header.mode_extension = 0;
	mp3_header.emphasis = 0;
	mp3_header.bitrate = 0;
	mp3_header.samplerate = 0;
	mp3_header.crc_check = 0;
	mp3_header.crc_target = 0;
	mp3_header.no_of_channels = 0;
	mp3_header.no_of_sb_samples = 0;
	mp3_header.no_of_pcm_samples = 0;
	mp3_header.copyright = 0;
	mp3_header.crc_protected = 0;
	mp3_header.original = 0;
	mp3_header.intensity_stereo = 0;
	mp3_header.ms_stereo = 0;
	mp3_header.freeformat = 0;
	mp3_header.lsf_ext = 0;
	mp3_header.mpeg_2_5_ext = 0;
	mp3_header.padding = 0;
	mp3_header.frame_pending = 0;
	mp3_header.private_bits = 0;
	mp3_header.duration_us = 0;
}

/*
// zero out all subband samples in current frame
*/
void mp3_mute_frame(void)
{
	mp3_layer3_overlap_init();
	memset(mp3_out, 0, 2 * 36 * 32 * sizeof(mp3_frac));
}

/*
// writes data to the stream
*/
unsigned int mp3_bitstream_write(void* bytes, unsigned int length)
{
	if (mp3_next_frame)
	{
		/* #pragma GCC diagnostic ignored "-Wpointer-to-int-cast" */
		mp3_bitstream_len = (unsigned int) ((uintptr_t) &mp3_bitstream_buffer[mp3_bitstream_len] - (uintptr_t) mp3_next_frame);
		/* #pragma GCC diagnostic warning "-Wpointer-to-int-cast" */
		memmove(mp3_bitstream_buffer, mp3_next_frame, mp3_bitstream_len);
	}
	/*
	// if the length would overflow the buffer substract the excess from it
	*/
	if (length + mp3_bitstream_len > (MP3_BITSTREAM_BUFFER_LEN - MP3_BIT_RESERVOIR_LEN))
	{
		length -= (length + mp3_bitstream_len) - MP3_BITSTREAM_BUFFER_LEN;
	}

	memcpy(&mp3_bitstream_buffer[mp3_bitstream_len], bytes, length);

	mp3_bitstream_len += length;
	mp3_current_frame = mp3_bitstream_buffer;
	mp3_next_frame = mp3_bitstream_buffer;
	mp3_stream_init(&mp3_bitstream, mp3_bitstream_buffer);
	mp3_sync = 1;
	/*
	// return # of bytes actually written
	*/
	return length;
}

/*
// get write pointer
*/
void* mp3_bitstream_get_write_pointer(unsigned int *capacity)
{
	if (mp3_next_frame)
	{
		mp3_bitstream_len = (unsigned int) ((uintptr_t) &mp3_bitstream_buffer[mp3_bitstream_len] - (uintptr_t) mp3_next_frame);
		memmove(mp3_bitstream_buffer, mp3_next_frame, mp3_bitstream_len);
		*capacity = MP3_BITSTREAM_BUFFER_LEN - MP3_BIT_RESERVOIR_LEN - mp3_bitstream_len;
	}
	else
	{
		*capacity = MP3_BITSTREAM_BUFFER_LEN - MP3_BIT_RESERVOIR_LEN - mp3_bitstream_len;
	}

	return &mp3_bitstream_buffer[mp3_bitstream_len];
}

/*
// notify decoder of bytes written
*/
void mp3_bitstream_bytes_written(unsigned int length)
{
	mp3_bitstream_len += length;
	mp3_current_frame = mp3_bitstream_buffer;
	mp3_next_frame = mp3_bitstream_buffer;
	mp3_stream_init(&mp3_bitstream, mp3_bitstream_buffer);
	mp3_sync = 1;
}

/*
// set output buffer
*/
void mp3_set_output_buffer(void* buffer)
{
	mp3_out = buffer;
}

/*
// locate the next sync word in the stream
*/
unsigned int mp3_bitstream_sync(void)
{
	unsigned char const *p_cur = mp3_stream_next(&mp3_bitstream);
	unsigned char const *p_eof = mp3_bitstream_buffer + mp3_bitstream_len;
	while (p_cur < p_eof - 1 && (p_cur[0] != 0xff || (p_cur[1] & 0xe0) != 0xe0)) p_cur++;
	if (p_eof - p_cur < MP3_BUFFER_GUARD) return 1;
	mp3_stream_init(&mp3_bitstream, p_cur);
	return MP3_SUCCESS;
}

/*
// decode a header
*/
static unsigned int mp3_decode_header_internal(mp3_stream_t *ptr, mp3_header_t *header)
{
	unsigned int bitrate_index, freq_index;

	/*
	// read header from bitstream
	*/
	mp3_stream_skip(ptr, 11);
	header->mpeg_2_5_ext = (mp3_stream_read(ptr, 1) == 0);
	header->lsf_ext = (mp3_stream_read(ptr, 1) == 0);
	header->layer = 4 - mp3_stream_read(ptr, 2);
	header->crc_protected = (mp3_stream_read(ptr, 1) == 0);
	if (header->crc_protected)
	{
	  	header->crc_check = mp3_crc_check(*ptr, 16, 0xffff);
	}
	bitrate_index = mp3_stream_read(ptr, 4);
	freq_index = mp3_stream_read(ptr, 2);
	header->padding = mp3_stream_read(ptr, 1);
	header->private_bits = mp3_stream_read(ptr, 1) ? 0x100 : 0;
	header->mode = mp3_stream_read(ptr, 2);
	header->mode_extension = mp3_stream_read(ptr, 2);
	header->copyright = mp3_stream_read(ptr, 1);
	header->original = mp3_stream_read(ptr, 1);
	header->emphasis = mp3_stream_read(ptr, 2);
	header->no_of_channels = (header->mode == MP3_MODE_SINGLE_CHANNEL) ? 1 : 2;
	header->no_of_sb_samples = (header->layer == 1) ? 12 : (header->layer == 3 && header->lsf_ext) ? 18 : 36;
	header->no_of_pcm_samples = header->no_of_sb_samples * 32;
	header->ms_stereo = 0;
	header->intensity_stereo = 0;
	header->freeformat = 0;
	header->frame_pending = 0;

	if (!header->lsf_ext && header->mpeg_2_5_ext)
	{
		return MP3_ERROR_LOST_SYNC;
	}

	if (header->layer == 4) 
	{
		return MP3_ERROR_INVALID_BITSTREAM;
	}
	
	if (bitrate_index == 15) 
	{
	  	return MP3_ERROR_INVALID_BITRATE;
	}
	
	if (header->lsf_ext)
	{
		header->bitrate = bitrate_table[3 + (header->layer >> 1)][bitrate_index];
	}
	else
	{
	  	header->bitrate = bitrate_table[header->layer - 1][bitrate_index];
	}

	if (freq_index == 3) 
	{
		return MP3_ERROR_INVALID_SAMPLERATE;
	}
	
	header->samplerate = samplerate_table[freq_index];
	
	if (header->lsf_ext)
	{
		header->samplerate >>= 1;
		if (header->mpeg_2_5_ext)
	  	{
	    	header->samplerate >>= 1;
	    }
	}
	
	/*
	// CRC check
	*/
	if (header->crc_protected)
	{
	  	header->crc_target = mp3_stream_read(ptr, 16);
	}
	
	return MP3_SUCCESS;
}

/*
// discover freeformat stream bitrate
*/
static unsigned int mp3_get_free_bitrate(void)
{
	unsigned int rate = 0;
	unsigned int slots_per_frame;
	mp3_stream_t saved_stream = mp3_bitstream;

	if (mp3_header.layer == 3 && (mp3_header.lsf_ext))
	{
		slots_per_frame = 72;
	}
	else
	{
		slots_per_frame = 144;
	}

	while (mp3_bitstream_sync() == MP3_SUCCESS) 
	{
	  	mp3_header_t peek_header = mp3_header;
		mp3_stream_t peek = mp3_bitstream;

	  	if (mp3_decode_header_internal(&peek, &peek_header) == MP3_SUCCESS &&
			peek_header.layer == mp3_header.layer && peek_header.samplerate == mp3_header.samplerate) 
		{
	    	if (mp3_header.layer == 1) 
			{
				rate = (unsigned long) mp3_header.samplerate * 
					((mp3_stream_next(&mp3_bitstream) - mp3_current_frame) - 4 * 
					mp3_header.padding + 4) / 48 / 1000;
	    	}
	    	else 
		    {
				rate = (unsigned long) mp3_header.samplerate * 
					((mp3_stream_next(&mp3_bitstream) - mp3_current_frame) - 
					mp3_header.padding + 1) / slots_per_frame / 1000;
	    	}
	
	    	if (rate >= 8)
				break;
	  	}
	
	  	mp3_stream_skip(&mp3_bitstream, 8);
	}
	/*
	// restore bitstream object
	*/
	mp3_bitstream = saved_stream;
	/*
	// error check
	*/
	if (rate < 8 || (mp3_header.layer == 3 && rate > 640)) 
	{
		mp3_last_error = MP3_ERROR_LOST_SYNC;
		return 1;
	}
	/*
	// save free bitrate value
	*/
	mp3_freerate = ((unsigned long) rate) * 1000;
	
	return MP3_SUCCESS;
}

/*
// decode the next header on the stream
*/
unsigned int mp3_decode_header(void)
{
	unsigned char const *p_cur, *p_eof;
	unsigned int N, result;
	
	p_cur = mp3_next_frame;
	p_eof = mp3_bitstream_buffer + mp3_bitstream_len;

	do
	{
		do
		{
			if (mp3_sync) 
			{
				/*
				// check that the whole frame is in the 
				// bitstream buffer and that
				*/
				if (p_eof - p_cur < MP3_BUFFER_GUARD) 
				{
	    			mp3_next_frame = p_cur;
					mp3_sync = 0;
	    			return mp3_last_error = MP3_ERROR_EOF;
	  			}
	  			else if (p_cur[0] != 0xff || (p_cur[1] & 0xe0) != 0xe0)
				{
	    			mp3_current_frame = p_cur++;
	    			mp3_next_frame = p_cur;
					if (!mp3_first_sync)
						continue;

					mp3_sync = 0;
	    			return mp3_last_error = MP3_ERROR_LOST_SYNC;
	  			}
				mp3_first_sync = 1;
			}
			else 
			{
				mp3_stream_init(&mp3_bitstream, p_cur);
			
	  			if (mp3_bitstream_sync() != MP3_SUCCESS) 
				{
	    			if (p_eof - mp3_next_frame >= MP3_BUFFER_GUARD)
					{
						mp3_next_frame = p_eof - MP3_BUFFER_GUARD;
					}

					mp3_sync = 0;

	    			return mp3_last_error = MP3_ERROR_EOF;
	  			}
			
	  			p_cur = mp3_stream_next(&mp3_bitstream);
			}
		}
		while (!mp3_first_sync);
		
		mp3_current_frame = p_cur;
		mp3_next_frame = p_cur + 1;  
		mp3_stream_init(&mp3_bitstream, mp3_current_frame);

		/*
		// decode header
		*/
		if ((result = mp3_decode_header_internal(&mp3_bitstream, &mp3_header)) != MP3_SUCCESS)
		{
			mp3_sync = 0;
			return mp3_last_error = result;
		}

		/*
		// calculate the duration of the frame
		*/ 
		mp3_header.duration_us = (mp3_header.no_of_pcm_samples * 1000000L) / mp3_header.samplerate; 
		
		/* 
		// calculate free bit rate 
		*/
		if (!mp3_header.bitrate) 
		{
	  		if ((mp3_freerate == 0 || !mp3_sync ||
				(mp3_freerate > 640000 && mp3_header.layer == 3)) &&
				mp3_get_free_bitrate() != MP3_SUCCESS)
			{
				mp3_sync = 0;
				return MP3_ERROR_INVALID_BITRATE;
			}

	  		mp3_header.bitrate = mp3_freerate;
			mp3_header.freeformat = 1;
		}
		
		/* 
		// calculate start of next frame 
		*/
		switch (mp3_header.layer)
		{
			case 1:	N = ((12 * mp3_header.bitrate / mp3_header.samplerate) + mp3_header.padding) * 4; break;
			case 2: N = (144 * mp3_header.bitrate / mp3_header.samplerate) + mp3_header.padding; break;
			default:
				if (mp3_header.lsf_ext)
				{
					N = (72 * mp3_header.bitrate / mp3_header.samplerate) + mp3_header.padding;
				}
				else
				{
					N = (144 * mp3_header.bitrate / mp3_header.samplerate) + mp3_header.padding;			
				}
		}
		/*
		// make sure the whole frame is in the bitstream buffer
		*/
		if (N + MP3_BUFFER_GUARD > (unsigned int) (p_eof - mp3_current_frame)) 
		{
			mp3_next_frame = mp3_current_frame;
			mp3_sync = 0;
	  		return mp3_last_error = MP3_ERROR_EOF;
		}
		
		mp3_next_frame = mp3_current_frame + N;
		
		if (!mp3_sync) 
		{
	  		/* 
			// check that a valid frame header follows this frame 
			*/
			p_cur = mp3_next_frame;
	  		if (p_cur[0] == 0xff && (p_cur[1] & 0xe0) == 0xe0) 
			{
	  			mp3_sync = 1;
	  		}
			else
			{
	    		p_cur = mp3_next_frame = mp3_current_frame + 1;
			}
		}
	}
	while (!mp3_sync);
	
	mp3_header.frame_pending = 1;
	
	return MP3_SUCCESS;
}

/*
// decode the next frame on the bitstream
*/
unsigned int mp3_decode(unsigned int nch)
{
	/*
	// decode the header if not already done
	*/ 
	if (!mp3_header.frame_pending && mp3_decode_header() != MP3_SUCCESS)
	{
		mp3_anc_bitstream_len = 0;
		return mp3_last_error;
	}
	/*
	// clear last error
	*/
	mp3_last_error = 0;
	/*
	// invoke layer decoder to decode audio data
	*/
	switch (mp3_header.layer)
	{
		case 1: mp3_layer1_decode(nch); break;
		case 2: mp3_layer2_decode(nch); break;
		case 3: mp3_layer3_decode(nch); break;
		default:  mp3_last_error = MP3_ERROR_INVALID_BITSTREAM;
	}

	/*
	// clear frame_pending flag
	*/
	mp3_header.frame_pending = 0;

	if (mp3_last_error != MP3_SUCCESS)
	{
		if (mp3_last_error == MP3_ERROR_DECODER_NOT_INITIALIZED)
		{
			mp3_next_frame = mp3_current_frame;
		}
		mp3_anc_bitstream_len = 0;
	}
	else
	{
		/* 
		// set ancillary data stream
		*/	
		if (mp3_header.layer != 3) 
		{
			mp3_stream_t next_frame;
	  		mp3_stream_init(&next_frame, mp3_next_frame);
	  		mp3_anc_bitstream = mp3_bitstream;
	  		mp3_anc_bitstream_len = mp3_stream_len(&mp3_bitstream, &next_frame);
		}
	}
	
	return mp3_last_error;
}

/*
// compute CRC
*/
unsigned short mp3_crc_check(mp3_stream_t stream, unsigned int len, unsigned short crc)
{
  	while (len >= 8) 
	{
	    crc = (crc << 8) ^ mp3_crc_table[((crc >> 8) ^ mp3_stream_read(&stream, 8)) & 0xff];
  		len -= 8;
  	}

  	while (len--) 
	{
    	unsigned int msb = mp3_stream_read(&stream, 1) ^ (crc >> 15);
		crc <<= 1;
		if (msb & 1)
			crc ^= 0x8005;
  	}

  	return crc;
}
