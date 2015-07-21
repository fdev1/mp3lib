/*
 * mp3.c
 *
 * Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
 * All rights reserved
 *
 */


#include <stdlib.h>
#include "mp3.h"
#include "mpeg_2.h"
#include "mpeg_2_synth.h"

/*
// machine states
*/
#define MP3_STATE_IDLE				0
#define MP3_STATE_STARTING			1
#define MP3_STATE_WAITING_FOR_DATA	2
#define MP3_STATE_DECODING			3
#define MP3_STATE_OUTPUT_READY		4
#define MP3_STATE_FAILED			5
#define MP3_STATE_FINISHED			6

#if defined(__XC16__) || defined(__C30__)
	static unsigned char state __attribute__((__near__, section(".mp3")));
	static unsigned char mp3_nch __attribute__((__near__, section(".mp3")));
	/*static char decode_header __attribute__((section(".mp3_sm")));*/
	#define WEAK /*__attribute__((__weak__))*/ 
#else
	static unsigned char state;
	static unsigned char mp3_nch;
	/*static char decode_header;*/
	#define WEAK
#endif

/*
// initialize decoder
*/
void mp3_sm_init(unsigned int nch)
{
	/*decode_header = 0;*/
	state = MP3_STATE_IDLE;
	mp3_nch = nch;
}

/*
// start the decoder
*/
void mp3_sm_start(void)
{
	/*
	// get the state machine going
	*/
	state = MP3_STATE_STARTING;
}

#if 0 && (defined(__XC16__) || defined(__C30__))
unsigned int WEAK mp3_sm_input(void)
{
	return MP3_RSP_BREAK;
}
#else
extern unsigned int WEAK mp3_sm_input(void);
#endif

#if 0
unsigned int WEAK mp3_sm_header(mp3_header_t const * header)
{
	return MP3_RSP_CONTINUE;
}
#endif

#if 0 && (defined(__XC16__) || defined(__C30__))
unsigned int WEAK mp3_sm_output(mp3_header_t const * header, mp3_frac pcm_samples[2][1152])
{
	return MP3_RSP_CONTINUE;
}
#else
extern unsigned int WEAK mp3_sm_output(mp3_header_t const * header, mp3_frac pcm_samples[2][1152]);
#endif

#if 0 && (defined(__XC16__) || defined(__C30__))
void WEAK mp3_sm_finished(int result)
{
	return;	
}
#else
void WEAK mp3_sm_finished(int result);
#endif

#if 0 && (defined(__XC16__) || defined(__C30__))
unsigned int WEAK mp3_sm_error(const unsigned int error)
{
	return MP3_RSP_CONTINUE;
}
#else
extern unsigned int WEAK mp3_sm_error(const unsigned int error);
#endif

/*
// decoder state machine
*/
void mp3_sm_tasks(void)
{
	switch (state)
  	{
	  	case MP3_STATE_IDLE:
	  		/*
	  		// nothing to do
	  		*/
	  		return;
	  		
	  	case MP3_STATE_STARTING:
			/*
  			// initialize decoder
  			*/
  			mp3_init();
  			mp3_synth_init();
  			/*
  			// advance state machine
  			*/
  			state = MP3_STATE_WAITING_FOR_DATA;
  			
  			/* no break */

  		case MP3_STATE_WAITING_FOR_DATA:
			/* 
			// call input handler
			*/
			switch (mp3_sm_input()) 
	    	{
    			case MP3_RSP_STOP: 
    				state = MP3_STATE_FINISHED;
    				return;
    			case MP3_RSP_BREAK: 
    				state = MP3_STATE_FAILED;
    				return;
    			case MP3_RSP_NOT_READY:
    			case MP3_RSP_IGNORE: return;
    			case MP3_RSP_CONTINUE: break;
    		}
    		/*
    		// advance state machine
    		*/
    		state = MP3_STATE_DECODING;
    		
    		/* no break */

    	case MP3_STATE_DECODING:
			/*
			// if header decoding is enabled then
			// decode the header
			*/
      		#if 0
	      	{
		      	/*
		      	// decode header
		      	*/
				if (mp3_decode_header() != MP3_SUCCESS) 
				{
					/*
					// if more data is needed request it on
					// next step
					*/
  					if (mp3_get_last_error() == MP3_ERROR_EOF)
  					{
  						state = MP3_STATE_WAITING_FOR_DATA;
  						return;
  					}
					/*
					// invoke error handler
					*/
	  				switch (mp3_sm_error(mp3_get_last_error())) 
		  			{
	  					case MP3_RSP_STOP: 
	  						state = MP3_STATE_FINISHED;
	  						return;
	  					case MP3_RSP_BREAK: 
	  						state = MP3_STATE_FAILED;
	  						return;
	  					case MP3_RSP_IGNORE: 
	  					case MP3_RSP_CONTINUE:
	  					default: return;
	  				}
				}
				/*
				// invoke decoded header handler
				*/
				switch (mp3_sm_header(&mp3_header)) 
				{
					case MP3_RSP_STOP: 
						state = MP3_STATE_FINISHED;
						return;
					case MP3_RSP_BREAK: 
						state = MP3_STATE_FAILED;
						return;
					case MP3_RSP_IGNORE: return;
					case MP3_RSP_CONTINUE: break;
				}
      		}
      		#endif
			/*
			// decode the next frame
			*/
      		if (mp3_decode(mp3_nch) != MP3_SUCCESS) 
	      	{
				/*
				// if more data is needed request it on
				// next step
				*/
  				if (mp3_get_last_error() == MP3_ERROR_EOF)
  				{
  					state = MP3_STATE_WAITING_FOR_DATA;
  					return;
  				}
				/*
				// if there was a decoding error invoke error
				// handler
				*/
				switch (mp3_sm_error(mp3_get_last_error())) 
				{
					case MP3_RSP_STOP: 
						state = MP3_STATE_FINISHED;
						return;
					case MP3_RSP_BREAK: 
						state = MP3_STATE_FAILED;
						return;
					case MP3_RSP_IGNORE: break;
					case MP3_RSP_CONTINUE: 
					default: return;
				}
      		}
			/*
			// synth pcm audio
			*/
      		mp3_synth(mp3_header.no_of_channels, mp3_header.no_of_sb_samples);
      		/*
      		// advance state machine
      		*/
      		state = MP3_STATE_OUTPUT_READY;
      		
      		/* no break */

      	case MP3_STATE_OUTPUT_READY:
			/*
			// invoke output handler
			*/
			switch (mp3_sm_output(&mp3_header, mp3_out->pcm_samples)) 
			{
				case MP3_RSP_STOP: 
					state = MP3_STATE_FINISHED;
					return;
				case MP3_RSP_BREAK: 
					state = MP3_STATE_FAILED;
					return;
				case MP3_RSP_IGNORE:
					return;
				case MP3_RSP_CONTINUE: 
				default:
					state = MP3_STATE_DECODING;
      				return;
			}
			
			/* no break */

  		case MP3_STATE_FAILED:
			/*
			// set the result to an error code
			*/
			mp3_sm_finished(-1);
			/*
			// set state machine to idle
			*/		  	
		  	state = MP3_STATE_IDLE;
			/*
			// return
			*/
			return;

		case MP3_STATE_FINISHED:
			/*
			// finished decoding
			*/
			mp3_sm_finished(0);
			/*
			// set state machine to idle
			*/		  	
		  	state = MP3_STATE_IDLE;
		  	/*
		  	// return
		  	*/
		  	return;
  	}
}
