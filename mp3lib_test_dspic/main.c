/*
// main.c
*/

#define NCHANNELS		2

#include <string.h>
#include <stdio.h>

#if defined(__XC16__) || defined(__C30__)
	#if defined(__dsPIC33E__)
		#include <p33Exxxx.h>
	#else
		#include <p33Fxxxx.h>
	#endif
#elif defined(__XC32__)
	#include <xc.h>
	#include <plib.h>
	
	#define	GetSystemClock()              (80000000ul)
	#define	GetPeripheralClock()          (GetSystemClock()/(1 << OSCCONbits.PBDIV))
	#define	GetInstructionClock()         (GetSystemClock())
	#define BAUD 19200
	
#endif

#include "..\mp3.h"
#include "..\compiler.h"

#if defined(__XC16__) || defined(__C30__)
	_FWDT(FWDTEN_OFF);
#else
	#pragma config FNOSC = FRCPLL   
	#pragma config FPLLODIV = DIV_1, FPLLMUL = MUL_15, FPLLIDIV = DIV_2, FWDTEN = OFF, FCKSM = CSECME, FPBDIV = DIV_1   
	#pragma config OSCIOFNC = OFF, POSCMOD = HS, FSOSCEN = OFF   
	#pragma config CP = OFF, BWP = OFF, PWP = OFF   
	#pragma config ICESEL   = ICS_PGx2   
	#pragma config DEBUG    = OFF   
	#pragma config UPLLEN   = OFF   
	#pragma config UPLLIDIV = DIV_2   
	#pragma config WDTPS    = PS1 
#endif


FILE* audio_file;
FILE* output_file;
char audio_playback;
#if 0
unsigned char mp3_buffer[512];
#endif

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
#if defined(__XC16__) || defined(__C30__)
	#if defined(__dsPIC33E__)
		int reserved[4] __attribute__((section(".reserved"), eds, address(0xCFF8)));
	#else
		int reserved[3] __attribute__((section(".reserved"), address(0x7FFA)));
	#endif
#endif

/*
// this function is called whenever there's no
// whole frames to decode in the bitstream buffer. Call
// mp3_bitstream_write as shown bellow to write data
// to the bitstream and return the CONTINUE response or
// return the STOP response to stop decoding. If the data
// is not ready return the NOT_READY response and the
// state machine will call this function on every iteration
// until data is written to the bitstream.
*/
unsigned int mp3_sm_input(void)
{
	#if 0
	/*
	// read another chunk of data
	*/
	size_t bytes_read = fread(mp3_buffer, 1, 512, audio_file);
	/*
	// if no bytes where read return STOP message
	*/
	if (!bytes_read)
		return MP3_RSP_STOP;
	/*
	// write data to decode bitstream
	*/
	mp3_bitstream_write(mp3_buffer, bytes_read);
	#else
	unsigned int len;
	void* ptr = mp3_bitstream_get_write_pointer(&len);
	//len = fread(ptr, 1, len, audio_file);

	if (!len)
		return MP3_RSP_STOP;

	mp3_bitstream_bytes_written(len);
	#endif	
	/*
	// continue decoding
	*/
  	return MP3_RSP_CONTINUE;
}

/*
// process decoder output
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

unsigned int mp3_sm_error(unsigned int error)
{
	return MP3_RSP_CONTINUE;
}

/*
// this function is called when the decoder
// finishes
*/
void mp3_sm_finished(int result)
{
	audio_playback = 0;
}

#define SystemClock                      (60000000ul)   
#define PeripheralClock            (SystemClock/(1<<OSCCONbits.PBDIV))   

int main()
{
	#if 0
	INTEnableSystemMultiVectoredInt();   
	SYSTEMConfigPerformance(SystemClock);   
	// initialize uart1   
	U1MODE=0;   
	U1STA=0;   
	U1MODEbits.UARTEN = 1;   
	U1MODEbits.BRGH = 0;   
	U1STAbits.UTXEN = 1;   
	U1BRG=31; //115200 at 60Mhz fpb   
	
	//test uart:   
	U1TXREG='A'; // <-my terminal received 'A' char, so uart works OK   
	// now printf something   
	printf("Hello world!\r\n"); // <- generates exception   
	
	while(1);  
	#else
	volatile int x = 1;	
	/*
	// the DSP engine must be configured as follows
	// before calling any mp3lib functions.
	*/
	#if defined(__XC16__) || defined(__C30__)
	CORCONbits.US = 0;			/* signed multiply */
	CORCONbits.SATA = 1;		/* ACCA saturation disabled */
	CORCONbits.SATB = 1;		/* ACCB saturation disabled */
	CORCONbits.SATDW = 1;		/* data write saturation enabled */
	CORCONbits.ACCSAT = 1;		/* super saturation mode */
	CORCONbits.RND = 1;			/* don't care */
	CORCONbits.IF = 1;			/* integer multiply */


	//INTCON1bits.OVATE = 1;
	//INTCON1bits.OVBTE = 1;
	INTCON1bits.COVTE = 1;
	#elif defined(__XC32__)
		INTEnableSystemMultiVectoredInt();   
		SYSTEMConfigPerformance(SystemClock);   
		// initialize uart1   
		U1MODE=0;   
		U1STA=0;   
		U1MODEbits.UARTEN = 1;   
		U1MODEbits.BRGH = 0;   
		U1STAbits.UTXEN = 1;   
		U1BRG=31; //115200 at 60Mhz fpb   
		
		//__XC_UART=1;
		// now printf something   
	    setbuf(stdin,NULL);
	    setbuf(stdout,NULL);
		printf("Hello world!\r\n"); // <- generates exception   
		//while (1);

		//__XC_UART = 1;
	    //SYSTEMConfig(GetSystemClock(), SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

	 
	    //UARTConfigure(UART1, UART_ENABLE_HIGH_SPEED | UART_ENABLE_PINS_TX_RX_ONLY);
	    //UARTSetFifoMode(UART1, UART_INTERRUPT_ON_RX_NOT_EMPTY | UART_INTERRUPT_ON_TX_NOT_FULL);
	    //UARTSetLineControl(UART1, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
	    //UARTSetDataRate(UART1, GetPeripheralClock(), BAUD);
	    //UARTEnable(UART1, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));
	#endif
	
	/*
	// initialize decoder
	*/
	//mp3_sm_init(NCHANNELS);
	//mp3_set_output_buffer(output_buffer);
	
	/*
	// open input and output files
	*/	
	printf("Decoding MP3 file...");
	audio_playback = 1;
	output_file = fopen("output.pcm", "wb");
	audio_file = fopen("file2.mp3", "rb");
	/*
	// start the decoder
	*/
	mp3_sm_start();
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
	#endif
	/*
	// exit
	*/
	return 0;
}

#if 1
void _mon_putc (char c)   
{   
	while (U1STAbits.UTXBF);   
	U1TXREG = c;   
}   

#include <errno.h>
#ifndef _FD_INVALID
#define _FD_INVALID (-1)
#endif

typedef enum
{
	SIM_OPEN,
	SIM_CLOSE,
	SIM_READ,
	SIM_WRITE,
	SIM_LSEEK,
	SIM_UNLINK,
	SIM_RENAME
}
	SIMMETHOD;
typedef struct tagSIMOPEN
{
	const char *	name;
	int		access;
	int		mode;
	int		handle;
}
	SIMOPEN;
typedef struct tagSIMCLOSE
{
	int		handle;
}
	SIMCLOSE;
typedef struct tagSIMREAD
{
	int		handle;
	const void *	buffer;
	unsigned int	len;
}
	SIMREAD;
typedef struct tagSIMWRITE
{
	int		handle;
	const void *	buffer;
	unsigned int	len;
}
	SIMWRITE;
typedef struct tagSIMLSEEK
{
	int		handle;
	long		offset;
	int		origin;
}
	SIMLSEEK;
typedef struct tagSIMUNLINK
{
	const char *	filename;
	int		rc;
}
	SIMUNLINK;
typedef struct tagSIMRENAME
{
	const char *	oldname;
	const char *	newname;
	int		rc;
}
	SIMRENAME;
typedef struct tagSIMIO
{
	SIMMETHOD	method;
	union
	{
	SIMOPEN		open;
	SIMCLOSE	close;
	SIMREAD		read;
	SIMWRITE	write;
	SIMLSEEK	lseek;
	SIMUNLINK	unlink;
	SIMRENAME	rename;
	}
	u;
}
SIMIO, *PSIMIO;

int __C30_UART = 1;

static void __inline__ doopen(PSIMIO psimio) {
  //__asm__("0xDAC00000" :: "a" (psimio) : "memory");
}

static void __inline__
doread(PSIMIO psimio)
{
  //__asm__("0xDAC00000" :: "a" (psimio) : "memory" );
}



open(const char *name, int access, int mode)
{
	SIMIO simio;
	register PSIMIO psimio asm("t0") = &simio;
	
	simio.method = SIM_OPEN;
	simio.u.open.name = name;
	simio.u.open.access = access;
	simio.u.open.mode = mode;
	simio.u.open.handle = _FD_INVALID;
	doopen(psimio);
	
	if (simio.u.open.handle == _FD_INVALID) {
	#ifdef __C30_LEGACY_LIBC__
	  errno = EFOPEN;
	#else
	  errno = EBADF;
	#endif
	}
	return(simio.u.open.handle);
}

read(int handle, void *buffer, unsigned int len)
{
  int i;
  volatile __U1MODEbits_t *umode = &U1MODEbits;
  volatile __U1STAbits_t *ustatus = &U1STAbits;
  volatile unsigned int *rxreg = &U1RXREG;
  volatile unsigned int *brg = &U1BRG;


  switch (handle)
  {
    case 0:
#ifdef __C30_LEGACY_LIBC__
      if (_Files[0]->_Lockno == 0)
#endif
      {
        if ((umode->UARTEN) == 0)
        {
          *brg = 0;
          umode->UARTEN = 1;
        }
        for (i = len; i; --i)
        {
          int nTimeout;

          /*
          ** Timeout is 16 cycles per 10-bit char
          */
          nTimeout = 16*10;
          while (((ustatus->URXDA) == 0) && nTimeout) --nTimeout;
          if ((ustatus->URXDA) == 0) break;
          *(char*)buffer++ = *rxreg;
        }
        len -= i;
        break;
      }

    default: {
      SIMIO simio;
      register PSIMIO psimio asm("t0");

      psimio  = &simio;
      simio.method = SIM_READ;
      simio.u.read.handle = handle;
      simio.u.read.buffer = buffer;
      simio.u.read.len = len;
      doread(psimio);

      len = simio.u.read.len;
      break;
    }
  }
  return(len);
}



#else
void _mon_putc(char c)
{
  while (!UARTTransmitterIsReady(UART1));
  UARTSendDataByte(UART1, c);
}
#endif

#if defined(__XC16__) || defined(__C30__)
void __attribute__((__interrupt__, __shadow__, __auto_psv__)) _MathError(void)
{
	if (INTCON1bits.OVAERR || INTCON1bits.OVBERR)
	{
		HALT();
		INTCON1bits.OVAERR = 0;
		INTCON1bits.OVBERR = 0;
	}
	if (INTCON1bits.COVAERR)
	{
		HALT();
		INTCON1bits.COVAERR = 0;
	}
	
	HALT();
	/*
	// clear interrupt flag
	*/
	INTCON1bits.MATHERR = 0;
}
#endif
