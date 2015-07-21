/*
 * mpeg_2_streamreader.h
 *
 * Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
 * All rights reserved
 *
 */

#ifndef MP3_STREAMREADER_H
#define MP3_STREAMREADER_H

#if 0 && defined(__GNUC__)
#define MP3_FAST_STREAM
#endif

typedef struct mp3_stream_t 
{
	unsigned char const *ptr;
	unsigned int buffer;
	unsigned int bufflen;
}
mp3_stream_t;

/*
// initialize stream pointer
*/
void mp3_stream_init(mp3_stream_t *stream, unsigned char const *ptr);

/*
// read up to 16 bits from the stream
*/
/* #pragma GCC diagnostic ignored "-Wunused-function" */
unsigned int mp3_stream_read(mp3_stream_t *stream, unsigned int len);

/*
// read a the next bit on the stream and return zero if the
// bit is zero and non-zero if it's one
*/
unsigned int mp3_stream_read_bool(mp3_stream_t *stream);

/*
// peek up to 16 bits from the stream
*/
unsigned int mp3_stream_peek(mp3_stream_t *stream, unsigned int len);

/*
// compare to stream poinnters
*/
unsigned int mp3_stream_len(mp3_stream_t const *begin, mp3_stream_t const *end);

/*
// rewind the stream
*/
void mp3_stream_rewind(mp3_stream_t *stream, unsigned int len);

/*
// get pointer to the next byte
*/
unsigned char const *mp3_stream_next(mp3_stream_t const *stream);

/*
// skip bits
*/
void mp3_stream_skip(mp3_stream_t *stream, unsigned int len);

/*
// this is the same as mp3_stream_read but is
// forced inline
*/
#if defined(MP3_FAST_STREAM)

#if !defined(__XC16__) && !defined(__C30__)
#if defined(__XC16__) || defined(__C30__) || defined(__XC32__) || defined(__GNUC__)
static __attribute((always_inline))
unsigned int mp3_stream_read_fast(mp3_stream_t *stream, unsigned int len)
#else
static _forceinline 
unsigned int mp3_stream_read_fast(mp3_stream_t *stream, unsigned int len)
#endif
{
	register unsigned int value;
	
	if (len < stream->bufflen) 
	{
		value = (stream->buffer & ((1U << stream->bufflen) - 1)) >> (stream->bufflen - len);
		stream->bufflen -= len;
		return value;
	}

	value = stream->buffer & ((1U << stream->bufflen) - 1);
	len -= stream->bufflen;
	stream->bufflen = 0;
	
	while (len >= 8) 
	{
		value = (value << 8) | *stream->ptr++;
		len  -= 8;
	}
	
	if (len > 0) 
	{
		#if defined(MP3_BIG_ENDIAN)
		stream->buffer = (((unsigned int) (*stream->ptr++)) << 8);
		stream->buffer |= *stream->ptr++;
		#else
		((unsigned char*) &stream->buffer)[1] = *stream->ptr++;
		((unsigned char*) &stream->buffer)[0] = *stream->ptr++;
		#endif
		value = (value << len) | (stream->buffer >> (16 - len));
		stream->bufflen = 16 - len;
	}
	return value;
}
#endif

#if !defined(__XC16__) && !defined(__C30__)
#if defined(__XC16__) || defined(__C30__) || defined(__XC32__) || defined(__GNUC__)
static __attribute__((always_inline)) 
unsigned int mp3_stream_read_bool_fast(mp3_stream_t *stream)
#else
static _forceinline 
unsigned int mp3_stream_read_bool_fast(mp3_stream_t *stream)
#endif
{
	if (stream->bufflen) 
	{
		return stream->buffer & (1U << --stream->bufflen);
	}
	else
	{
		#if defined(MP3_BIG_ENDIAN)
		stream->buffer = (((unsigned int) (*stream->ptr++)) << 8);
		stream->buffer |= *stream->ptr++;
		#else
		((unsigned char*) &stream->buffer)[1] = *stream->ptr++;
		((unsigned char*) &stream->buffer)[0] = *stream->ptr++;
		#endif
		stream->bufflen = 16 - 1;
		return (stream->buffer >> (16 - 1));
	}
}
#endif
#endif
#endif
