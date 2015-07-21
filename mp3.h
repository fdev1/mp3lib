/*
 * mp3.h
 *
 * Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
 * All rights reserved
 *
 */

#ifndef MP3_STATE_MACHINE_H
#define MP3_STATE_MACHINE_H

#include "mpeg_2.h"

#define MP3_RSP_CONTINUE	0x01
#define MP3_RSP_STOP		0x02
#define MP3_RSP_BREAK		0x03
#define MP3_RSP_IGNORE		0x04
#define MP3_RSP_NOT_READY	0x05

/*
// initialize decoder state machine
*/
void mp3_sm_init(unsigned int nch);

/*
// start decoding mp3 frames
*/
void mp3_sm_start(void);

/*
// perform state machine tasks
*/
void mp3_sm_tasks(void);

#endif
