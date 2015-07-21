/*
 * mpeg_2__1.h
 *
 * Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
 * All rights reserved
 *
 */

#ifndef MP3_MPEG_2__1_H
#define MP3_MPEG_2__1_H

/*
// initialize layer 1 decoder
*/
void mp3_layer1_init(void);

/*
// decode the next layer 1 frame
*/
unsigned int mp3_layer1_decode(unsigned int nch);

#endif
