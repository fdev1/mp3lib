#!/bin/sh

export PATH=/usr/lib/xc16/bin:$PATH

CMD_CC=xc16-gcc
CMD_AS=xc16-gcc
CMD_AR=xc16-ar
CMD_RM="rm -f"
CC_OPT=-O1

CPreProcessor()
{
  #cp $1 $2
  mono CPreProcessor.exe $1 $2
}

# build library for dsPIC33F
#
echo 'Building for dsPIC33F (coff)...'
make -f Makefile \
  CC=$CMD_CC \
  AS=$CMD_AS \
  AR=$CMD_AR \
  RM="$CMD_RM" \
  OPT=$CC_OPT \
  OMF=coff \
  p33f

# build library for dsPIC33F (elf)
#
echo
echo 'Building for dsPIC33F (elf)...'
make -f Makefile \
  CC=$CMD_CC \
  AS=$CMD_AS \
  AR=$CMD_AR \
  RM="$CMD_RM" \
  OPT=$CC_OPT \
  OMF=elf \
  LIBRARY_P33F=mp3lib_p33f_elf.a \
  p33f

# build dsPIC33E library (coff)
#
echo
echo 'Building for dsPIC33E (coff)...'
make -f Makefile \
  CC=$CMD_CC \
  AS=$CMD_AS \
  AR=$CMD_AR \
  RM="$CMD_RM" \
  OPT=$CC_OPT \
  OMF=coff \
  p33e

# build dsPIC33E library (elf)
#
echo
echo 'Building for dsPIC33E (elf)...'
make -f Makefile \
  CC=$CMD_CC \
  AS=$CMD_AS \
  AR=$CMD_AR \
  RM="$CMD_RM" \
  OPT=$CC_OPT \
  OMF=coff \
  LIBRARY_P33E=mp3lib_p33e_elf.a \
  p33e

# create output folders
#
echo
echo Creating package directories...
if [ -x ./package ]; then
	 rm ./package -r
fi

mkdir ./package
mkdir ./package/binary
mkdir ./package/binary/mp3lib
mkdir ./package/binary/mp3lib/mp3lib_test
mkdir ./package/binary/mp3lib/mp3lib_test.X
mkdir ./package/binary/mp3lib/tools

mkdir ./package/source
mkdir ./package/source/mp3lib
mkdir ./package/source/mp3lib/source
mkdir ./package/source/mp3lib/mp3lib_test
mkdir ./package/source/mp3lib/mp3lib_test.X
mkdir ./package/source/mp3lib/tools

# cp files to package directories
#
echo
echo Copying package files...
cp mp3lib_p33f.a ./package/binary/mp3lib/
cp mp3lib_p33f.a ./package/source/mp3lib/
cp mp3lib_p33e.a ./package/binary/mp3lib/
cp mp3lib_p33e.a ./package/source/mp3lib/

cp mp3lib_p33f_elf.a ./package/binary/mp3lib/
cp mp3lib_p33f_elf.a ./package/source/mp3lib/
cp mp3lib_p33e_elf.a ./package/binary/mp3lib/
cp mp3lib_p33e_elf.a ./package/source/mp3lib/

cp mp3.c ./package/binary/mp3lib/
cp mp3.c ./package/source/mp3lib/
cp mp3.h ./package/binary/mp3lib/
cp mp3.h ./package/source/mp3lib/
cp mpeg_2.h ./package/binary/mp3lib/
cp mpeg_2.h ./package/source/mp3lib/
cp mpeg_2_fractional.h ./package/binary/mp3lib/
cp mpeg_2_fractional.h ./package/source/mp3lib/
cp mpeg_2_streamreader.h ./package/binary/mp3lib/
cp mpeg_2_streamreader.h ./package/source/mp3lib/
#cp compiler.h ./package/binary/mp3lib/
#cp compiler.h ./package/source/mp3lib/
cp README.TXT ./package/binary/mp3lib/
cp README.TXT ./package/source/mp3lib/
cp COPYRIGHT.TXT ./package/binary/mp3lib/
cp COPYRIGHT.TXT ./package/source/mp3lib/
cp ./mp3lib_test_package/mp3lib_test.mcw ./package/binary/mp3lib/mp3lib_test/
cp ./mp3lib_test_package/mp3lib_test.mcw ./package/source/mp3lib/mp3lib_test/
cp ./mp3lib_test_package/mp3lib_test.mcp ./package/binary/mp3lib/mp3lib_test/mp3lib_test.mcp
cp ./mp3lib_test_package/mp3lib_test.mcp ./package/source/mp3lib/mp3lib_test/mp3lib_test.mcp
cp ./mp3lib_test_package/main.c ./package/binary/mp3lib/mp3lib_test/main.c
cp ./mp3lib_test_package/main.c ./package/source/mp3lib/mp3lib_test/main.c
cp ./mp3lib_test_package/halleluj.mp3 ./package/binary/mp3lib/mp3lib_test/halleluj.mp3
cp ./mp3lib_test_package/halleluj.mp3 ./package/source/mp3lib/mp3lib_test/halleluj.mp3
cp ./mp3lib_test_package/file.mp2 ./package/binary/mp3lib/mp3lib_test/
cp ./mp3lib_test_package/file.mp2 ./package/source/mp3lib/mp3lib_test/
cp ./mp3lib_test_package/file.mp3 ./package/binary/mp3lib/mp3lib_test/
cp ./mp3lib_test_package/file.mp3 ./package/source/mp3lib/mp3lib_test/

cp ./mp3lib_test_package.X/Makefile ./package/binary/mp3lib/mp3lib_test.X
cp ./mp3lib_test_package.X/main.c ./package/binary/mp3lib/mp3lib_test.X
cp -r ./mp3lib_test_package.X/nbproject ./package/binary/mp3lib/mp3lib_test.X
cp ./mp3lib_test_package.X/halleluj.hex ./package/binary/mp3lib/mp3lib_test.X
cp ./mp3lib_test_package.X/stimulus.sbs ./package/binary/mp3lib/mp3lib_test.X

cp ./mp3lib_test_package.X/Makefile ./package/source/mp3lib/mp3lib_test.X
cp ./mp3lib_test_package.X/main.c ./package/source/mp3lib/mp3lib_test.X
cp -r ./mp3lib_test_package.X/nbproject ./package/source/mp3lib/mp3lib_test.X
cp ./mp3lib_test_package.X/halleluj.hex ./package/source/mp3lib/mp3lib_test.X
cp ./mp3lib_test_package.X/stimulus.sbs ./package/source/mp3lib/mp3lib_test.X

cp ./tools/bin2hex.c ./package/binary/mp3lib/tools/
cp ./tools/hex2bin.c ./package/binary/mp3lib/tools/
cp ./tools/bin2hex.c ./package/source/mp3lib/tools/
cp ./tools/hex2bin.c ./package/source/mp3lib/tools/

# process and cp source files to source
# package directory
#
#rm ./package/source/mp3lib/source/*
#CPreProcessor compiler.h ./package/source/mp3lib/source/compiler.h 
#CPreProcessor compiler.s ./package/source/mp3lib/source/compiler.s
#CPreProcessor D.dat ./package/source/mp3lib/source/D.dat
#CPreProcessor Makefile ./package/source/mp3lib/source/Makefile
#CPreProcessor mp3.c ./package/source/mp3lib/source/mp3.c
#CPreProcessor mp3.h ./package/source/mp3lib/source/mp3.h
#CPreProcessor mpeg_2.c ./package/source/mp3lib/source/mpeg_2.c
#CPreProcessor mpeg_2.h ./package/source/mp3lib/source/mpeg_2.h
#CPreProcessor mpeg_2.inc ./package/source/mp3lib/source/mpeg_2.inc
#CPreProcessor mpeg_2__1.c ./package/source/mp3lib/source/mpeg_2__1.c
#CPreProcessor mpeg_2__1.h ./package/source/mp3lib/source/mpeg_2__1.h
#CPreProcessor mpeg_2__2.c ./package/source/mp3lib/source/mpeg_2__2.c
#CPreProcessor mpeg_2__2.h ./package/source/mp3lib/source/mpeg_2__2.h
#CPreProcessor mpeg_2__3.c ./package/source/mp3lib/source/mpeg_2__3.c
#CPreProcessor mpeg_2__3.h ./package/source/mp3lib/source/mpeg_2__3.h
#CPreProcessor mpeg_2__3_asm.s ./package/source/mp3lib/source/mpeg_2__3_asm.s
#CPreProcessor mpeg_2__3_huff.c ./package/source/mp3lib/source/mpeg_2__3_huff.c
#CPreProcessor mpeg_2__3_huff.h ./package/source/mp3lib/source/mpeg_2__3_huff.h
#CPreProcessor mpeg_2__3_tbl.c ./package/source/mp3lib/source/mpeg_2__3_tbl.c
#CPreProcessor mpeg_2_bitstream.s ./package/source/mp3lib/source/mpeg_2_bitstream.s
#CPreProcessor mpeg_2_fractional.c ./package/source/mp3lib/source/mpeg_2_fractional.c
#CPreProcessor mpeg_2_fractional.h ./package/source/mp3lib/source/mpeg_2_fractional.h
#CPreProcessor mpeg_2_streamreader.c ./package/source/mp3lib/source/mpeg_2_streamreader.c
#CPreProcessor mpeg_2_streamreader.h ./package/source/mp3lib/source/mpeg_2_streamreader.h
#CPreProcessor mpeg_2_synth.c ./package/source/mp3lib/source/mpeg_2_synth.c
#CPreProcessor mpeg_2_synth.h ./package/source/mp3lib/source/mpeg_2_synth.h
#CPreProcessor mpeg_2_synth_asm.s ./package/source/mp3lib/source/mpeg_2_synth_asm.s
#CPreProcessor rq.dat ./package/source/mp3lib/source/rq.dat
#CPreProcessor rq_exp.dat ./package/source/mp3lib/source/rq_exp.dat
cp mp3lib.mcp ./package/source/mp3lib/source/
cp README.TXT ./package/source/mp3lib/source/


# zip packages
#
VERSION=$(cat ./VERSION.TXT)
echo
echo Creating mp3lib_bin.zip...
cd package/binary
zip -rq '../mp3lib_bin_v'$VERSION'.zip' ./*

echo Creating mp3lib_source.zip...
cd ../source
zip -rq '../mp3lib_src_v'$VERSION'.zip' ./*
cd ../..
