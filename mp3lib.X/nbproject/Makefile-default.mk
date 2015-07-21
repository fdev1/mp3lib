#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=a
DEBUGGABLE_SUFFIX=a
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/mp3lib.X.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=a
DEBUGGABLE_SUFFIX=a
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/mp3lib.X.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../mpeg_2__3_tbl.c ../mpeg_2__3_asm.s ../mpeg_2__3.c ../mpeg_2__2.c ../mpeg_2__3_huff.c ../mpeg_2_streamreader.c ../mpeg_2_fractional.c ../mpeg_2__1.c ../mpeg_2.c ../mpeg_2_synth.c ../mpeg_2_synth_asm.s ../mp3.c ../compiler.s ../mpeg_2_bitstream.s

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/mpeg_2__3_tbl.o ${OBJECTDIR}/_ext/1472/mpeg_2__3_asm.o ${OBJECTDIR}/_ext/1472/mpeg_2__3.o ${OBJECTDIR}/_ext/1472/mpeg_2__2.o ${OBJECTDIR}/_ext/1472/mpeg_2__3_huff.o ${OBJECTDIR}/_ext/1472/mpeg_2_streamreader.o ${OBJECTDIR}/_ext/1472/mpeg_2_fractional.o ${OBJECTDIR}/_ext/1472/mpeg_2__1.o ${OBJECTDIR}/_ext/1472/mpeg_2.o ${OBJECTDIR}/_ext/1472/mpeg_2_synth.o ${OBJECTDIR}/_ext/1472/mpeg_2_synth_asm.o ${OBJECTDIR}/_ext/1472/mp3.o ${OBJECTDIR}/_ext/1472/compiler.o ${OBJECTDIR}/_ext/1472/mpeg_2_bitstream.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/mpeg_2__3_tbl.o.d ${OBJECTDIR}/_ext/1472/mpeg_2__3_asm.o.d ${OBJECTDIR}/_ext/1472/mpeg_2__3.o.d ${OBJECTDIR}/_ext/1472/mpeg_2__2.o.d ${OBJECTDIR}/_ext/1472/mpeg_2__3_huff.o.d ${OBJECTDIR}/_ext/1472/mpeg_2_streamreader.o.d ${OBJECTDIR}/_ext/1472/mpeg_2_fractional.o.d ${OBJECTDIR}/_ext/1472/mpeg_2__1.o.d ${OBJECTDIR}/_ext/1472/mpeg_2.o.d ${OBJECTDIR}/_ext/1472/mpeg_2_synth.o.d ${OBJECTDIR}/_ext/1472/mpeg_2_synth_asm.o.d ${OBJECTDIR}/_ext/1472/mp3.o.d ${OBJECTDIR}/_ext/1472/compiler.o.d ${OBJECTDIR}/_ext/1472/mpeg_2_bitstream.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/mpeg_2__3_tbl.o ${OBJECTDIR}/_ext/1472/mpeg_2__3_asm.o ${OBJECTDIR}/_ext/1472/mpeg_2__3.o ${OBJECTDIR}/_ext/1472/mpeg_2__2.o ${OBJECTDIR}/_ext/1472/mpeg_2__3_huff.o ${OBJECTDIR}/_ext/1472/mpeg_2_streamreader.o ${OBJECTDIR}/_ext/1472/mpeg_2_fractional.o ${OBJECTDIR}/_ext/1472/mpeg_2__1.o ${OBJECTDIR}/_ext/1472/mpeg_2.o ${OBJECTDIR}/_ext/1472/mpeg_2_synth.o ${OBJECTDIR}/_ext/1472/mpeg_2_synth_asm.o ${OBJECTDIR}/_ext/1472/mp3.o ${OBJECTDIR}/_ext/1472/compiler.o ${OBJECTDIR}/_ext/1472/mpeg_2_bitstream.o

# Source Files
SOURCEFILES=../mpeg_2__3_tbl.c ../mpeg_2__3_asm.s ../mpeg_2__3.c ../mpeg_2__2.c ../mpeg_2__3_huff.c ../mpeg_2_streamreader.c ../mpeg_2_fractional.c ../mpeg_2__1.c ../mpeg_2.c ../mpeg_2_synth.c ../mpeg_2_synth_asm.s ../mp3.c ../compiler.s ../mpeg_2_bitstream.s


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/mp3lib.X.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ256GP710A
MP_LINKER_FILE_OPTION=,--script=p33FJ256GP710A.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/mpeg_2__3_tbl.o: ../mpeg_2__3_tbl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__3_tbl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__3_tbl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mpeg_2__3_tbl.c  -o ${OBJECTDIR}/_ext/1472/mpeg_2__3_tbl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mpeg_2__3_tbl.o.d"      -g -D__DEBUG     -omf=elf -mlarge-code -mlarge-data -mconst-in-code -menable-large-arrays -O3 -funroll-loops -fomit-frame-pointer -I".." -I"." -msmart-io=1 -Wall -msfr-warn=off -fschedule-insns -fschedule-insns2 -mno-eds-warn
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2__3_tbl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mpeg_2__3.o: ../mpeg_2__3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__3.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__3.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mpeg_2__3.c  -o ${OBJECTDIR}/_ext/1472/mpeg_2__3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mpeg_2__3.o.d"      -g -D__DEBUG     -omf=elf -O0 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2__3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mpeg_2__2.o: ../mpeg_2__2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mpeg_2__2.c  -o ${OBJECTDIR}/_ext/1472/mpeg_2__2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mpeg_2__2.o.d"      -g -D__DEBUG     -omf=elf -O0 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2__2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mpeg_2__3_huff.o: ../mpeg_2__3_huff.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__3_huff.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__3_huff.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mpeg_2__3_huff.c  -o ${OBJECTDIR}/_ext/1472/mpeg_2__3_huff.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mpeg_2__3_huff.o.d"      -g -D__DEBUG     -omf=elf -O0 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2__3_huff.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mpeg_2_streamreader.o: ../mpeg_2_streamreader.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_streamreader.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_streamreader.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mpeg_2_streamreader.c  -o ${OBJECTDIR}/_ext/1472/mpeg_2_streamreader.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mpeg_2_streamreader.o.d"      -g -D__DEBUG     -omf=elf -O0 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2_streamreader.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mpeg_2_fractional.o: ../mpeg_2_fractional.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_fractional.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_fractional.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mpeg_2_fractional.c  -o ${OBJECTDIR}/_ext/1472/mpeg_2_fractional.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mpeg_2_fractional.o.d"      -g -D__DEBUG     -omf=elf -O0 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2_fractional.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mpeg_2__1.o: ../mpeg_2__1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mpeg_2__1.c  -o ${OBJECTDIR}/_ext/1472/mpeg_2__1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mpeg_2__1.o.d"      -g -D__DEBUG     -omf=elf -O0 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2__1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mpeg_2.o: ../mpeg_2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mpeg_2.c  -o ${OBJECTDIR}/_ext/1472/mpeg_2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mpeg_2.o.d"      -g -D__DEBUG     -omf=elf -O0 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mpeg_2_synth.o: ../mpeg_2_synth.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_synth.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_synth.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mpeg_2_synth.c  -o ${OBJECTDIR}/_ext/1472/mpeg_2_synth.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mpeg_2_synth.o.d"      -g -D__DEBUG     -omf=elf -O0 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2_synth.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mp3.o: ../mp3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mp3.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mp3.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mp3.c  -o ${OBJECTDIR}/_ext/1472/mp3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mp3.o.d"      -g -D__DEBUG     -omf=elf -O0 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mp3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1472/mpeg_2__3_tbl.o: ../mpeg_2__3_tbl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__3_tbl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__3_tbl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mpeg_2__3_tbl.c  -o ${OBJECTDIR}/_ext/1472/mpeg_2__3_tbl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mpeg_2__3_tbl.o.d"        -omf=elf -mlarge-code -mlarge-data -mconst-in-code -menable-large-arrays -O3 -funroll-loops -fomit-frame-pointer -I".." -I"." -msmart-io=1 -Wall -msfr-warn=off -fschedule-insns -fschedule-insns2 -mno-eds-warn
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2__3_tbl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mpeg_2__3.o: ../mpeg_2__3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__3.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__3.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mpeg_2__3.c  -o ${OBJECTDIR}/_ext/1472/mpeg_2__3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mpeg_2__3.o.d"        -g -omf=elf -O0 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2__3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mpeg_2__2.o: ../mpeg_2__2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mpeg_2__2.c  -o ${OBJECTDIR}/_ext/1472/mpeg_2__2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mpeg_2__2.o.d"        -g -omf=elf -O0 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2__2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mpeg_2__3_huff.o: ../mpeg_2__3_huff.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__3_huff.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__3_huff.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mpeg_2__3_huff.c  -o ${OBJECTDIR}/_ext/1472/mpeg_2__3_huff.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mpeg_2__3_huff.o.d"        -g -omf=elf -O0 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2__3_huff.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mpeg_2_streamreader.o: ../mpeg_2_streamreader.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_streamreader.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_streamreader.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mpeg_2_streamreader.c  -o ${OBJECTDIR}/_ext/1472/mpeg_2_streamreader.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mpeg_2_streamreader.o.d"        -g -omf=elf -O0 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2_streamreader.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mpeg_2_fractional.o: ../mpeg_2_fractional.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_fractional.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_fractional.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mpeg_2_fractional.c  -o ${OBJECTDIR}/_ext/1472/mpeg_2_fractional.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mpeg_2_fractional.o.d"        -g -omf=elf -O0 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2_fractional.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mpeg_2__1.o: ../mpeg_2__1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mpeg_2__1.c  -o ${OBJECTDIR}/_ext/1472/mpeg_2__1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mpeg_2__1.o.d"        -g -omf=elf -O0 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2__1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mpeg_2.o: ../mpeg_2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mpeg_2.c  -o ${OBJECTDIR}/_ext/1472/mpeg_2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mpeg_2.o.d"        -g -omf=elf -O0 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mpeg_2_synth.o: ../mpeg_2_synth.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_synth.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_synth.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mpeg_2_synth.c  -o ${OBJECTDIR}/_ext/1472/mpeg_2_synth.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mpeg_2_synth.o.d"        -g -omf=elf -O0 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2_synth.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mp3.o: ../mp3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mp3.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mp3.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mp3.c  -o ${OBJECTDIR}/_ext/1472/mp3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mp3.o.d"        -g -omf=elf -O0 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mp3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/mpeg_2__3_asm.o: ../mpeg_2__3_asm.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__3_asm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__3_asm.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../mpeg_2__3_asm.s  -o ${OBJECTDIR}/_ext/1472/mpeg_2__3_asm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG   -omf=elf -I"../" -Wa,-MD,"${OBJECTDIR}/_ext/1472/mpeg_2__3_asm.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2__3_asm.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/mpeg_2_synth_asm.o: ../mpeg_2_synth_asm.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_synth_asm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_synth_asm.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../mpeg_2_synth_asm.s  -o ${OBJECTDIR}/_ext/1472/mpeg_2_synth_asm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG   -omf=elf -I"../" -Wa,-MD,"${OBJECTDIR}/_ext/1472/mpeg_2_synth_asm.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2_synth_asm.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/compiler.o: ../compiler.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/compiler.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/compiler.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../compiler.s  -o ${OBJECTDIR}/_ext/1472/compiler.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG   -omf=elf -I"../" -Wa,-MD,"${OBJECTDIR}/_ext/1472/compiler.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/compiler.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/mpeg_2_bitstream.o: ../mpeg_2_bitstream.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_bitstream.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_bitstream.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../mpeg_2_bitstream.s  -o ${OBJECTDIR}/_ext/1472/mpeg_2_bitstream.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG   -omf=elf -I"../" -Wa,-MD,"${OBJECTDIR}/_ext/1472/mpeg_2_bitstream.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2_bitstream.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/1472/mpeg_2__3_asm.o: ../mpeg_2__3_asm.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__3_asm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2__3_asm.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../mpeg_2__3_asm.s  -o ${OBJECTDIR}/_ext/1472/mpeg_2__3_asm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -I"../" -Wa,-MD,"${OBJECTDIR}/_ext/1472/mpeg_2__3_asm.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2__3_asm.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/mpeg_2_synth_asm.o: ../mpeg_2_synth_asm.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_synth_asm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_synth_asm.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../mpeg_2_synth_asm.s  -o ${OBJECTDIR}/_ext/1472/mpeg_2_synth_asm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -I"../" -Wa,-MD,"${OBJECTDIR}/_ext/1472/mpeg_2_synth_asm.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2_synth_asm.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/compiler.o: ../compiler.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/compiler.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/compiler.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../compiler.s  -o ${OBJECTDIR}/_ext/1472/compiler.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -I"../" -Wa,-MD,"${OBJECTDIR}/_ext/1472/compiler.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/compiler.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/mpeg_2_bitstream.o: ../mpeg_2_bitstream.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_bitstream.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mpeg_2_bitstream.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../mpeg_2_bitstream.s  -o ${OBJECTDIR}/_ext/1472/mpeg_2_bitstream.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -I"../" -Wa,-MD,"${OBJECTDIR}/_ext/1472/mpeg_2_bitstream.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mpeg_2_bitstream.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: archive
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/mp3lib.X.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	@${RM} dist/${CND_CONF}/${IMAGE_TYPE}/mp3lib.X.${OUTPUT_SUFFIX} 
	${MP_AR} $(MP_EXTRA_AR_PRE)  -omf=elf -r dist/${CND_CONF}/${IMAGE_TYPE}/mp3lib.X.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/mp3lib.X.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	@${RM} dist/${CND_CONF}/${IMAGE_TYPE}/mp3lib.X.${OUTPUT_SUFFIX} 
	${MP_AR} $(MP_EXTRA_AR_PRE)  -omf=elf -r dist/${CND_CONF}/${IMAGE_TYPE}/mp3lib.X.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
