/*
 * mpeg_2__2.h
 *
 * Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
 * All rights reserved
 *
 */

#ifndef MP3_MPEG_2__2_H
#define MP3_MPEG_2__2_H

/*
// initialize layer 2 decoder
*/
void mp3_layer2_init(void);

/*
// decode the next layer 2 frame
*/
unsigned int mp3_layer2_decode(unsigned int nch);

#endif
