/*
 * mpeg_2.h
 *
 * Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
 * All rights reserved
 *
 */

#ifndef MP3_MPEG_2_H
#define MP3_MPEG_2_H

#include "mpeg_2_fractional.h"
#include "mpeg_2_streamreader.h"

/*
// channel mode
*/
#define MP3_MODE_STEREO							0			/* stereo */
#define MP3_MODE_JOINT_STEREO					1			/* joint stereo */
#define MP3_MODE_DUAL_CHANNEL					2			/* dual channel */
#define MP3_MODE_SINGLE_CHANNEL					3			/* single channel */

/*
// mode extension
*/
#define MP3_MODEEXT_I_STEREO					1
#define	MP3_MS_STEREO							2


/*
// emphasis
*/
#define MP3_EMPHASIS_NONE						0			/* no emphasis */
#define MP3_EMPHASIS_50_15_US					1			/* 50/15 us emphasis */
#define MP3_EMPHASIS_RESERVED					2			/* reserved */
#define MP3_EMPHASIS_CCITT_J_17					3			/* CCITT J.17 emphasis */

/*
// return codes
*/
#define MP3_SUCCESS								0x00
#define MP3_ERROR_DECODER_NOT_INITIALIZED		0x01
#define MP3_ERROR_EOF							0x02
#define MP3_ERROR_LOST_SYNC						0x03
#define MP3_ERROR_CRC_FAILED					0x04
#define MP3_ERROR_INVALID_BITSTREAM				0x05
#define MP3_ERROR_INVALID_BITRATE				0x06
#define MP3_ERROR_INVALID_SAMPLERATE			0x07

#define MP3_BUFFER_GUARD						8		/* used for synchronization */
#define MP3_BIT_RESERVOIR_LEN					511		/* bit reservoir */

/*
// The bitstram bufffer length can be calculated as follows:
//
// 144 * BITRATE / SAMPLERATE + 1 + RESERVOIR_LEN + BUFFER_GUARD + WRITE_GUARD
//
// Where:
// BITRATE is the highest bitrate supported
// SAMPLERATE is the lowest samplerate supported
// RESERVOIR_LEN is the constant defined above
// BUFFER_GUARD is the constant defined above
// WRITE_GUARD is an optional guard that allows you to write fixed size blocks
// whenever an EOF error is thrown by the decoder without worrying about overrunning 
// the buffer.
//
// Note: This are compile time options, you should not change anything in this
// file unless you're rebuilding the library
//
*/
#define MP3_BITSTREAM_BUFFER_LEN				(1441 + MP3_BIT_RESERVOIR_LEN + 1)

/*
// mp3 header structure
*/
typedef struct mp3_header_t 
{
	unsigned int layer:3;				/* mp3 audio layer */
	unsigned int no_of_pcm_samples:11;	/* number of PCM samples */
	unsigned int no_of_channels:2;		/* number of channels */
	unsigned int no_of_sb_samples:6;	/* number of subband samples */
	unsigned int copyright:1;			/* copyright bit */
	unsigned int crc_protected:1;		/* frame is CRC protected */
	unsigned int original:1;			/* frame is original */
	unsigned int mode:2;				/* channel mode */
	unsigned int mode_extension:2;		/* additional mode info */
	unsigned int intensity_stereo:1;	/* frame uses intensity stereo */
	unsigned int ms_stereo:1;			/* frame uses middle/side stereo */
	unsigned int freeformat:1;			/* frame is free format */
	unsigned int lsf_ext:1;				/* lsf extension */
	unsigned int mpeg_2_5_ext:1;		/* MPEG-2.5 extension */
	unsigned int padding:1;				/* frame has padding bits */
	unsigned int frame_pending:1;		/* frame data has not been decoded */
	unsigned int private_bits:5;		/* private bits */
	unsigned int emphasis;				/* emphasis */
	unsigned long bitrate;				/* stream bitrate */
	unsigned int samplerate;			/* sampling frequency */
	unsigned short crc_check;			/* frame CRC accumulator */
	unsigned short crc_target;			/* final target CRC checksum */
	unsigned long duration_us;			/* frame duration in us */
}
mp3_header_t;

/*
// output buffers union
*/
typedef union
{
	mp3_frac sb_samples[2][36][32];							/* subband samples */
	mp3_frac pcm_samples[2][1152];							/* pcm samples */
	mp3_frac xr_temp[2][2][576];
}
mp3_buffers_t;

/*
// external symbols
*/
extern mp3_header_t mp3_header;				/* header */
extern mp3_buffers_t *mp3_out;				/* output buffers */
extern unsigned int mp3_last_error;			/* last error */
extern mp3_stream_t mp3_anc_bitstream;		/* ancillary bitstream */

/*
// initialize deccoder
*/
void mp3_init(void);

/*
// write to the bitstream
*/
unsigned int mp3_bitstream_write(void* bytes, unsigned int length);

/*
// returns a pointer for writing to the bitstream buffer
// and the max number of bytes that can be written to it without
// overruning it.
*/
void *mp3_bitstream_get_write_pointer(unsigned int *maxlen);

/*
// this is used to notify the decoder that data has been
// written to the bitstream buffer. It must be called after writing
// to the bitstream buffer with a pointer returned by mp3_bitstream_get_write_pointer
*/
void mp3_bitstream_bytes_written(unsigned int length);

/*
// set output buffer
*/
void mp3_set_output_buffer(void* buffer);

/*
// decode header for next frame
*/
unsigned int mp3_decode_header(void);

/*
// decode next frame
*/
unsigned int mp3_decode(unsigned int nch);

/*
// mute current frame
*/
void mp3_mute_frame(void);

/*
// get the last eror
*/
#define mp3_get_last_error()		(mp3_last_error)

#endif
