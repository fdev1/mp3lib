
#include <stdio.h>
#include "../mp3.h"

char audio_playback;
#if 0
unsigned char mp3_buffer[512];
#endif
FILE* audio_file;
FILE* output_file;
mp3_frac output_buffer[2][1152];

unsigned int mp3_sm_input(void)
{
	#if 0
	size_t bytes_read = fread(mp3_buffer, 1, 512, audio_file);

	if (!bytes_read)
		return MP3_RSP_STOP;

	mp3_bitstream_write(mp3_buffer, bytes_read);
	#else
	unsigned int len;
	void* ptr = mp3_bitstream_get_write_pointer(&len);
	len = fread(ptr, 1, len, audio_file);

	if (!len)
		return MP3_RSP_STOP;

	mp3_bitstream_bytes_written(len);
	#endif
	
	return MP3_RSP_CONTINUE;
}

/*
// process decoder output
*/
unsigned int mp3_sm_output(struct mp3_header_t const * header, mp3_frac samples[2][1152])
{
  	mp3_frac const *left_ch = samples[0];
	mp3_frac const *right_ch = samples[1];
	unsigned int no_of_samples = header->no_of_pcm_samples;
	unsigned int no_of_channels = header->no_of_channels;

	while (no_of_samples--) 
	{
		fwrite(left_ch++, 2, 1, output_file);

    	if (1 && no_of_channels == 2) 
		{
			fwrite(right_ch++, 2, 1, output_file);
		}
  	}

	return MP3_RSP_CONTINUE;
}

unsigned int mp3_sm_error(const unsigned int error)
{
	/*
	static int c=0;
	printf("%i - %x\n", c, error);
	c++;
	return MP3_RSP_CONTINUE;
	*/
	return MP3_RSP_CONTINUE;
}

void mp3_sm_finished(int result)
{
	audio_playback = 0;
}

#if defined (__MSC__)
#include <conio.h>
#endif
#include <math.h>
FILE* debug_output;
int main()
{

#if 0
	FILE *f = fopen("C:\\Users\\Fernando\\Documents\\sf.dat", "wb");

	unsigned int i, c = 0, temp;
	for (i = 0; i < 64; i++)
	{
		if (!i)
		{
			fprintf(f, "\t");
		}
		if (i && !(i % 8))
			fprintf(f, "\r\n\t");

		c = abs(sf[i]);

		if (sf[i] < 0)
		{
			fprintf(f, "-0x%04x, ", c);
		}
		else
		{
			fprintf(f, " 0x%04x, ", c);
		}
	}
	fclose(f);
	printf("done\n");
	getch();
	return;

#elif 0
	FILE *f = fopen("C:\\Users\\Fernando\\Documents\\rq16.dat", "wb");
	FILE *fe = fopen("C:\\Users\\Fernando\\Documents\\rq_exp16.dat", "wb");

	unsigned int i, c = 0, temp;
	for (i = 0; i < 8207; i++)
	{
		if (!i)
		{
			fprintf(f, "\t");
			fprintf(fe, "\t");
		}
		if (i && !(i % 7))
			fprintf(f, "\r\n\t");

		if (i && !(i % 10))
			fprintf(fe, "\r\n\t");

		if (rq_exp[i] <= 3)
		{
			temp = rq_tbl[i] << rq_exp[i];
			if (temp == 0x8000)
				temp = 0x7FFF;
			if (temp > c)
				c = temp;
			fprintf(f, "0x%04x, ", temp);
			fprintf(fe, " %i, ", 0);
		}
		else
		{
			temp = rq_tbl[i] << 3;
			if (temp == 0x8000)
				temp = 0x7FFF;
			if (temp > c)
				c = temp;
			fprintf(f, "0x%04x, ", temp);
			fprintf(fe, "%2i, ", rq_exp[i] - 3);
		}
	}
	fclose(fe);
	fclose(f);
	printf("%04x\n", c);
	printf("done\n");
	getch();
	return;
#endif
	
	/*
	// initialize decoder state machine
	*/
	mp3_sm_init(2);
	mp3_set_output_buffer(output_buffer);
	//C:\Users\Fernando\Music\GUNS N' ROSES\Greatest Hits
	//"C:\\Users\\Fernando\\Music\\GUNS N' ROSES\\Greatest Hits\\NOVEMBER RAIN.mp3"
	audio_playback = 1;
	printf("Decoding MP3 file...");

	/*
	 * we need to flush stdout or else some consoles
	 * won't display the line until the new line.
	 */
	fflush(stdout);

#if defined(__GNUC__)
	debug_output = fopen("/home/fernan/debug_output.pcm", "wb");
	audio_file = fopen("/home/fernan/projects/mp3lib/mp3lib_test/file.mp3", "rb");
	output_file = fopen("/home/fernan/output.pcm", "wb");
#else
	debug_output = fopen("C:\\Users\\Fernando\\Documents\\debug_output.pcm", "wb");
	audio_file = fopen("C:\\Users\\Fernando\\Documents\\projects\\mp3lib\\mp3lib_test_dspic\\file.mp3", "rb");
	output_file = fopen("C:\\Users\\Fernando\\Documents\\output.pcm", "wb");
	//audio_file = fopen("C:\\Users\\Fernando\\Documents\\file.mp2", "rb");
	//audio_file = fopen("C:\\Users\\Fernando\\Music\\GUNS N' ROSES\\Greatest Hits\\november rain.mp3", "rb");
	//audio_file = fopen("C:\\Users\\Fernando\\Music\\GUNS N' ROSES\\Greatest Hits\\sweet child o' mine.mp3", "rb");
	//audio_file = fopen("C:\\Users\\Fernando\\Documents\\hallelujah.mp3", "rb");
#endif

	/*
	// start decoding
	*/
	mp3_sm_start();

	while (1 && audio_playback)
	{
		mp3_sm_tasks();
	}

	fclose(debug_output);
	fclose(audio_file);
	fclose(output_file);
	printf("Done\n");
#if defined(__WIN32__)
	getch();
#endif

	return 0;

}

