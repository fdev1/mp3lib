/*
 * mpeg_2__3_huff.h
 *
 * Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
 * All rights reserved
 *
 */

#ifndef MP3_HUFF_H
#define MP3_HUFF_H

/*
// huffman quad table structure
*/
typedef struct
{
	unsigned int hlen:8;
	unsigned int padding:4;
	unsigned int v:1;
	unsigned int w:1;
	unsigned int x:1;
	unsigned int y:1;
}
mp3_huff_quad_t;

/*
// huffman pair table
*/
typedef struct
{
	unsigned int final:1;
	unsigned int hlen:3;
	union
	{
		unsigned int offset:12;
		struct
		{
			unsigned int padding:4;
			unsigned int x:4;
			unsigned int y:4;
		}
		values;		
	}
	data;
}
mp3_huff_pair_t;


/*
// huffman table structure
*/
typedef struct
{
	mp3_huff_pair_t const *table;
	unsigned short linbits;
	unsigned short startbits;
}
mp3_huff_table_t;

/*
// declare huffman tables
*/
extern mp3_huff_quad_t const *const mp3_huff_quad_table[2];
extern mp3_huff_table_t const mp3_huff_pair_table[32];

#endif
