#
# Makefile
#
# Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
# All rights reserved
#

#
# Note: This makefile has only been tested with
# GNU Make v3.81 on Windows. You may need to modify
# the "toolchain" section bellow (even if you're running
# it on Windows as your tools path may be different)
#


#
# toolchain
	#
#CC="C:\Program Files (x86)\Microchip\xc16\v1.21\bin\xc16-gcc.exe" 
#AS="C:\Program Files (x86)\Microchip\xc16\v1.21\bin\xc16-gcc.exe"
#AR="C:\Program Files (x86)\Microchip\xc16\v1.21\bin\xc16-ar.exe"
#RM=del /Q

CC=xc16-gcc
AS=xc16-gcc
AR=xc16-ar
RM=rm -f

OPT=-O3
OMF=elf

#
# target libraries
#
LIBRARY_P33E=mp3lib_p33e.a
LIBRARY_P33F=mp3lib_p33f.a



#
# target flags
#
p33f: CFLAGS=-omf=$(OMF) -x c -c -Wall $(OPT) -mlarge-code -mlarge-data -mconst-in-code -funroll-loops -fomit-frame-pointer -fschedule-insns -fschedule-insns2 -mno-eds-warn -mcpu=33FJ256GP710A
p33f: ASFLAGS=-c -mcpu=33FJ256GP710A  -omf=$(OMF) -g -Wa
p33e: CFLAGS=-omf=$(OMF) -x c -c -Wall $(OPT) -mlarge-code -mlarge-data -mconst-in-code -funroll-loops -fomit-frame-pointer -fschedule-insns -fschedule-insns2 -mno-eds-warn -mcpu=33EP512GP502
p33e: ASFLAGS=-c -mcpu=33EP512GP502  -omf=$(OMF) -g -Wa
ARFLAGS=-omf=$(OMF) r

#
# sources
#
SOURCES=mp3.c mpeg_2.c mpeg_2__1.c mpeg_2__2.c mpeg_2__3.c mpeg_2__3_huff.c mpeg_2__3_tbl.c mpeg_2_fractional.c mpeg_2_streamreader.c mpeg_2_synth.c
SSOURCES=compiler.s mpeg_2__3_asm.s mpeg_2_bitstream.s mpeg_2_synth_asm.s
OBJECTS=$(SOURCES:.c=.o)
SOBJECTS=$(SSOURCES:.s=.o)

#
# make
#
all: p33f
p33f: clean $(LIBRARY_P33F)
p33e: clean $(LIBRARY_P33E)

$(LIBRARY_P33E): $(OBJECTS) $(SOBJECTS)
	$(RM) $(LIBRARY_P33E)
	$(AR) $(ARFLAGS) $@ $(OBJECTS) $(SOBJECTS)
	
$(LIBRARY_P33F): $(OBJECTS) $(SOBJECTS)
	$(RM) $(LIBRARY_P33F)
	$(AR) $(ARFLAGS) $@ $(OBJECTS) $(SOBJECTS)
	
.c.o:
	$(CC) $(CFLAGS) $< -o $@
.s.o:
	$(AS) $(ASFLAGS) $< -o $@

clean:
	$(RM) $(OBJECTS) $(SOBJECTS)
