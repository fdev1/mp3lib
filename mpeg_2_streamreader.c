/*
 * mpeg_2_streamreader.c
 *
 * Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
 * All rights reserved
 *
 */

#include <limits.h>
#include "compiler.h"
#include "mpeg_2_streamreader.h"

/*
// initialize streamm structure
*/
void mp3_stream_init(mp3_stream_t *stream, unsigned char const *buffer)
{
	stream->ptr  = buffer;
	if (buffer)
	{
		stream->buffer = ((unsigned int) *stream->ptr++) << 8;
		stream->buffer |= *stream->ptr++;
		stream->bufflen = 16;
	}
	else
	{
		stream->buffer = 0;
		stream->bufflen = 0;
	}
}

/*
// return the number of bits between two points on the stream
*/
unsigned int mp3_stream_len(mp3_stream_t const *begin, mp3_stream_t const *end)
{
	return (8 * (end->ptr - (begin->ptr + 1))) + begin->bufflen + (8 - end->bufflen);
}

/*
// read up to 16 bits from bitstream
*/
#if !defined(__XC16__) && !defined(__C30__)
unsigned int mp3_stream_read(mp3_stream_t *stream, unsigned int len)
{
	unsigned int value;
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
unsigned int mp3_stream_read_bool(mp3_stream_t *stream)
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

unsigned int mp3_stream_peek(mp3_stream_t *stream, unsigned int len)
{
	mp3_stream_t peek = *stream;
	return mp3_stream_read(&peek, len);
}

/*
// get pointer to next byte on the bitstream
*/
unsigned char const *mp3_stream_next(mp3_stream_t const *stream)
{
	if (stream->bufflen == 16)
	{
		return stream->ptr - 2;
	}
	else if (stream->bufflen >= 8)
	{
		return stream->ptr - 1;
	}
	else
	{
		return stream->ptr;
	}
}

/*
// skip an arbitrary number of bits
*/
void mp3_stream_skip(mp3_stream_t *stream, unsigned int len)
{
	if (stream->bufflen >= len)
	{
		stream->bufflen -= len;
	}
	else
	{
		register unsigned int temp = len >> 3;
		stream->ptr += temp;
		temp = len - (temp << 3);

		if (stream->bufflen >= temp)
		{
			stream->bufflen -= temp;
		}
		else
		{
			stream->bufflen = 16 - (temp - stream->bufflen);
			stream->ptr += 2;
		}
		stream->buffer = ((unsigned int) *(stream->ptr - 2)) << 8;
		stream->buffer |= *(stream->ptr - 1);
	}
}

/*
// rewind the stream by an arbitrary number of bits
*/
void mp3_stream_rewind(mp3_stream_t *stream, unsigned int len)
{
	if ((16 - stream->bufflen) >= len)
	{
		stream->bufflen += len;
	}
	else
	{
		register unsigned int temp = len >> 3;
		stream->ptr -= temp;
		temp = len - (temp << 3);

		if ((16 - stream->bufflen) >= temp)
		{
			stream->bufflen += temp;
		}
		else
		{
			stream->bufflen = (temp + stream->bufflen) % 16;
			stream->ptr -= 2;
		}
		stream->buffer = ((unsigned int) *(stream->ptr - 2)) << 8;
		stream->buffer |= *(stream->ptr - 1);
	}
}
