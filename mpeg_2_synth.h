/*
 * mpeg_2_synth.h
 *
 * Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
 * All rights reserved
 *
 */

#ifndef MP3_SYNTH_H
#define MP3_SYNTH_H

#include "mpeg_2_fractional.h"

/*
// initialize mp3 synthesizer
*/
void mp3_synth_init(void);

/*
// mute output
*/
void mp3_synth_mute(void);

/*
// synthesize frame
*/
void mp3_synth(unsigned int nch, unsigned int ns);

#endif
