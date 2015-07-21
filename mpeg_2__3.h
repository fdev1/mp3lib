/*
 * mpeg_2__3.h
 *
 * Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
 * All rights reserved
 *
 */


#ifndef MP3_MPEG_2__3_H
#define MP3_MPEG_2__3_H

typedef struct
{
    unsigned short part2_3_length;
    unsigned short big_values;
    unsigned short global_gain;
    unsigned short scalefac_compress;
	unsigned int count1table_select:1;
	unsigned int scalefac_scale:1;
	unsigned int preflag:1;
	unsigned int mixed_block:1;
    unsigned char block_type;
    unsigned char region0_count;
    unsigned char region1_count;
    unsigned char table_select[3];
    unsigned char subblock_gain[3];
}
mp3_channel_t;


typedef struct
{
  	mp3_channel_t channel[2];
} 
mp3_granule_t;

typedef struct
{
	unsigned int main_data_begin;
	unsigned int private_bits;		/* 5 bits */
	unsigned char scfsi[2];
	mp3_granule_t granule[2];
}
mp3_sideinfo_t;

typedef struct mp3_sfbwidth_tables
{
	unsigned char const *l;
	unsigned char const *s;
	unsigned char const *m;
} 
mp3_sfbwidth_tables;

typedef struct mp3_sflen
{
	unsigned char slen1;
	unsigned char slen2;
} 
mp3_sflen;

/*
// initialize layer 3 decoder
*/
void mp3_layer3_init(void);

/*
// initialize overlap buffer
*/
void mp3_layer3_overlap_init(void);

/*
// decode the next layer 3 frame on the bitstream
*/
unsigned int mp3_layer3_decode(unsigned int nch);

#endif
