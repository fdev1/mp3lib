/*
// main.c
*/

#define NCHANNELS		2

#include <string.h>
#include <stdio.h>
#if defined(__dsPIC33E__)
	#include <p33Exxxx.h>
#else
	#include <p33Fxxxx.h>
#endif

#include "../mp3.h"

_FWDT(FWDTEN_OFF);

FILE* audio_file;
FILE* output_file;
char audio_playback;

/*
// this buffer must be word aligned hence we define
// it as integer
*/
int output_buffer[NCHANNELS][1152] __attribute__((section(".output")));

/*
// we must make sure that mp3lib DSP buffers don't get allocated
// at the end if the data space (last 3 words). This was done to 
// optimize the DSP algorithms. you may use this memory in your
// application.
*/
#if defined(__dsPIC33E__)
	int reserved[4] __attribute__((section(".reserved"), eds, address(0xCFF8)));
#else
	int reserved[3] __attribute__((section(".reserved"), address(0x7FFA)));
#endif

/*
// this function is called whenever there's no
// whole frames to decode in the bitstream buffer.
//
// Notes:
// ======
// This function uses the most memory-efficient method
// of writing to the audio bitstream. Another way of 
// writing to the bitstream is to use the mp3_bitstream_write
// method defined in mpeg_2.h
//
// For optimal efficient you could this method without the
// state machine by calling mp3_decode() to decode one frame
// then write to the buffer using the method use by this
// function with a file system library that support asynchronous
// reads and finally call mp3_synth() to synthesize the last
// frame. That will allow you to refill the buffer while the
// last frame. For example you could something like this on
// your main loop:
//
// if (decoding)
// {
//		if (mp3_decode(2) == MP3_SUCCESS) / * decode 2 channels * /
//		{
//			if (!io_in_progress)
//			{
//				ptr = mp3_bitstream_get_write_pointer(&len);
//				len = fread_async(ptr, 1, len, audio_file);
//				io_in_progress = 1;
//			}
//
//			mp3_synth(2); / * synthesize 2 channels * /
//			
//		}
//		else
//		{
//			switch(mp3_get_last_error())
//			{
//				case MP3_ERROR_EOF:
//					if (!io_in_progress)
//					{
//						ptr = mp3_bitstream_get_write_pointer(&len);
//						len = fread_async(ptr, 1, len, audio_file);
//						io_in_progress = 1;
//					}
//					break;
//		
//				default:
//					/ * handle other errors if needed * /
//			}
//		}
// }
//
// Then on your async read callback handler:
//
// if (!len)
// {
//		decoding = 0;
// }
// else
// {
// 		mp3_bitstream_bytes_written(len);
// }
// io_in_progress = 0;
//
*/
unsigned int mp3_sm_input(void)
{
	void* ptr;
	unsigned int len;
	/*
	// get the bitstream write pointer and the
	// max number of bytes that can be written to
	// the bitstream without overruning the buffer
	*/	
	ptr = mp3_bitstream_get_write_pointer(&len);
	/*
	// read the audio data from the file directly
	// into the bitstream buffer. If no bytes are read
	// (EOF codition) then return the MP3_RSP_STOP 
	// message to the state machine
	*/
	if (!(len = fread(ptr, 1, len, audio_file)))
		return MP3_RSP_STOP;
	/*
	// notify the decoder of how many bytes have
	// been written to the bitstream
	*/
	mp3_bitstream_bytes_written(len);
	/*
	// continue decoding
	*/
  	return MP3_RSP_CONTINUE;
}

/*
// process decoder output
//
// Note:
// =====
//
// see definition of mp3_header_t on mpeg_2.h for obtaining
// samplerate, bitrate, duration and other frame info. For obtaining
// song name, length, etc. you need to parse ID3 tags. There are
// many examples of doing this available on the web. We will provide
// the routines for doing it in the future.
//
*/
unsigned int mp3_sm_output(mp3_header_t const * header, int samples[2][1152])
{
  	int const *left = samples[0]; 
  	int const *right = samples[1];
	unsigned int length = header->no_of_pcm_samples;	

  	while (length--) 
	{
		fwrite(left++, 2, 1, output_file);

    	if (header->no_of_channels == 2) 
		{	
			fwrite(right++, 2, 1, output_file);
		}
  	}

	return MP3_RSP_CONTINUE;
}

/*
// This function is called by the state machine when
// an error occurs. All errors are recoverable but you
// may want to mute the frame output (mp3_mute_frame())
// to minimize glitches.
*/
unsigned int mp3_sm_error(unsigned int error)
{
	return MP3_RSP_CONTINUE;
}

/*
// this function is called when the decoder
// finishes (ie. when you send the state machine
// an MP3_RSP_STOP or MP3_RSP_BREAK message).
*/
void mp3_sm_finished(int result)
{
	audio_playback = 0;
}


int main()
{
	/*
	// the DSP engine must be configured as follows
	// before calling any mp3lib functions.
	*/
	CORCONbits.US = 0;			/* signed multiply */
	CORCONbits.SATA = 1;		/* ACCA saturation disabled */
	CORCONbits.SATB = 1;		/* ACCB saturation disabled */
	CORCONbits.SATDW = 1;		/* data write saturation enabled */
	CORCONbits.ACCSAT = 1;		/* super saturation mode */
	CORCONbits.RND = 1;			/* don't care */
	CORCONbits.IF = 1;			/* integer multiply */

	/*
	// initialize decoder
	*/
	mp3_sm_init(NCHANNELS);
	mp3_set_output_buffer(output_buffer);
	
	/*
	// open input and output files
	*/	
	audio_playback = 1;
	output_file = fopen("output.pcm", "wb");
	audio_file = fopen("halleluj.mp3", "rb");
	/*
	// start the decoder
	*/
	mp3_sm_start();
	printf("Decoding MP3 file...");
	/*
	// run until finished decoding
	*/
	while (audio_playback)
	{
		mp3_sm_tasks();
	}
	/*
	// close files
	*/
	fclose(audio_file);
	fclose(output_file);
	/*
	// print completed message
	*/
	printf("Done\n");
	/*
	// exit
	*/
	return 0;
}
