;
; synth.s
;
; Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
; All rights reserved
;

.include "mpeg_2.inc"

.equ TWO_LEVEL_DO_LOOP, 1

.extern _mp3_D
.extern _mp3_filterbank
.extern _mp3_out
.extern _mp3_phase
.extern _mp3_xr

;
; experimental code for scaling the subband samples
; up before DCT. It makes no significant difference
; in the output so it's been taken out
;
.if 0
.data
_mp3_synth_exponent:
.space 2
.text
;
; w0 = ch
;
.global _mp3_synth_normalize
_mp3_synth_normalize:
;{
	return
	mov #_mp3_synth_exponent, w4
	
	mov #(1152 * 2), w1
	mul.uu w0, w1, w0
	mov _mp3_out, w1
	add w1, w0, w1
	mov w1, w3
	;
	; find exponent
	;
	mov #-15, w0
	fbcl [w1++], w2
	do #1150, _normalize_loop1
	cp w2, w0
	bra lt, _normalize_loop1
	mov w2, w0
	_normalize_loop1:
	fbcl [w1++], w2
	cp w2, w0
	bra lt, _fix_exp
	mov w2, w0

_fix_exp:
	add w0, #(MP3_INTBITS), w0
	cp0 w0
	bra lt, _normalize
	clr [w4]
	return
	
	;
	; normalize
	;
_normalize:
	mov w3, w1
	do #1151, _normalize_loop2
	lac [w1], a
	sftac a, w0
	_normalize_loop2:
	sac.r a, [w1++]
	neg w0, [w4]
	return
;}

;
; w0 = ch
; w1 = index
; w2 = lo
; w3 = hi
;
.global _mp3_synth_renormalize
_mp3_synth_renormalize:
;{
	return
	mov _mp3_synth_exponent, w4
	
	add w1, w1, w1
	add w2, w1, w2
	add w3, w1, w3
	mov #(8 * 2), w5
	
	do #15, _renorm_loop
	lac [w2], a
	sftac a, w4
	sac.r a, [w2]
	lac [w3], a
	sftac a, w4
	sac.r a, [w3]
	add w2, w5, w2
	_renorm_loop:
	add w3, w5, w3

	return	
;}
.endif

;
; w0 in[32]
; w1 index
; w2 lo[16][8]
; w3 hi[16][8]
;
; note: this can be optimized
;
.macro dct

	.equ in, w0
	.equ slot, w1
	.equ lo, w2
	.equ hi, w3
	.equ temp, w4
	.equ fraclen, 13
	.equ fracshift, (16 - fraclen - 1)

	.equ cos01, 0x3fec
	.equ cos02, 0x3fb1
	.equ cos03, 0x3f4f
	.equ cos04, 0x3ec5
	.equ cos05, 0x3e15
	.equ cos06, 0x3d3f
	.equ cos07, 0x3c42
	.equ cos08, 0x3b21
	.equ cos09, 0x39db
	.equ cos10, 0x3871
	.equ cos11, 0x36e5
	.equ cos12, 0x3537
	.equ cos13, 0x3368
	.equ cos14, 0x3179
	.equ cos15, 0x2f6c
	.equ cos16, 0x2d41
	.equ cos17, 0x2afb
	.equ cos18, 0x289a
	.equ cos19, 0x2620
	.equ cos20, 0x238e
	.equ cos21, 0x20e7
	.equ cos22, 0x1e2b
	.equ cos23, 0x1b5d
	.equ cos24, 0x187e
	.equ cos25, 0x1590
	.equ cos26, 0x1294
	.equ cos27, 0x0f8d
	.equ cos28, 0x0c7c
	.equ cos29, 0x0964
	.equ cos30, 0x0646
	.equ cos31, 0x0324

	.if __dsPIC33E == 1
		push DSRPAG
		push DSWPAG
		movpag #edspage(_mp3_xr), DSRPAG
		movpag #edspage(_mp3_xr), DSWPAG
		mov #edsoffset(_mp3_xr), temp
	.else
		mov #_mp3_xr, temp
	.endif
	
	add slot, slot, slot
		
	mov [in], w5
	mov [in + (31 * 2)], w6
	add w5, w6, w5
	mov w5, [temp]
		
	mov [in], w5
	mov [in + (31 * 2)], w6
	sub w5, w6, w5
	mov #cos01, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (16 * 2)]
	
	mov [in + (15 * 2)], w5
	mov [in + (16 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (1 * 2)]
	
	mov [in + (15 * 2)], w5
	mov [in + (16 * 2)], w6
	sub w5, w6, w5
	mov #cos31, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (17 * 2)]
	
	mov [temp + (16 * 2)], w5
	mov [temp + (17 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (41 * 2)]	
	
	mov [temp + (16 * 2)], w5
	mov [temp + (17 * 2)], w6
	sub w5, w6, w5
	mov #cos02, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (59 * 2)]
	
	mov [temp], w5
	mov [temp + (1 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (33 * 2)]
	
	mov [temp], w5
	mov [temp + (1 * 2)], w6
	sub w5, w6, w5
	mov #cos02, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (50 * 2)]
		
	mov [in + (7 * 2)], w5
	mov [in + (24 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (2 * 2)]	
	
	mov [in + (7 * 2)], w5
	mov [in + (24 * 2)], w6
	sub w5, w6, w5
	mov #cos15, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (18 * 2)]	
	
	mov [in + (8 * 2)], w5
	mov [in + (23 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (3 * 2)]
	
	mov [in + (8 * 2)], w5
	mov [in + (23 * 2)], w6
	sub w5, w6, w5
	mov #cos17, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (19 * 2)]	
	
	mov [temp + (18 * 2)], w5
	mov [temp + (19 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (42 * 2)]
		
	mov [temp + (18 * 2)], w5
	mov [temp + (19 * 2)], w6
	sub w5, w6, w5
	mov #cos30, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (60 * 2)]	
	
	mov [temp + (2 * 2)], w5
	mov [temp + (3 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (34 * 2)]	
	
	mov [temp + (2 * 2)], w5
	mov [temp + (3 * 2)], w6
	sub w5, w6, w5
	mov #cos30, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (51 * 2)]
	
	mov [in + (3 * 2)], w5
	mov [in + (28 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (4 * 2)]	
	
	mov [in + (3 * 2)], w5
	mov [in + (28 * 2)], w6
	sub w5, w6, w5
	mov #cos07, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (20 * 2)]	
	
	mov [in + (12 * 2)], w5
	mov [in + (19 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (5 * 2)]
	
	mov [in + (12 * 2)], w5
	mov [in + (19 * 2)], w6
	sub w5, w6, w5
	mov #cos25, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (21 * 2)]
		
	mov [temp + (20 * 2)], w5
	mov [temp + (21 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (43 * 2)]	
	
	mov [temp + (20 * 2)], w5
	mov [temp + (21 * 2)], w6
	sub w5, w6, w5
	mov #cos14, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (61 * 2)]	
	
	mov [temp + (4 * 2)], w5
	mov [temp + (5 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (35 * 2)]
		
	mov [temp + (4 * 2)], w5
	mov [temp + (5 * 2)], w6
	sub w5, w6, w5
	mov #cos14, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (52 * 2)]
	
	mov [in + (4 * 2)], w5
	mov [in + (27 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (6 * 2)]
	
	mov [in + (4 * 2)], w5
	mov [in + (27 * 2)], w6
	sub w5, w6, w5
	mov #cos09, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (22 * 2)]

	mov [in + (11 * 2)], w5
	mov [in + (20 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (7 * 2)]

	mov [in + (11 * 2)], w5
	mov [in + (20 * 2)], w6
	sub w5, w6, w5
	mov #cos23, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (23 * 2)]

	mov [temp + (22 * 2)], w5
	mov [temp + (23 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (44 * 2)]

	mov [temp + (22 * 2)], w5
	mov [temp + (23 * 2)], w6
	sub w5, w6, w5
	mov #cos18, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (62 * 2)]

	mov [temp + (6 * 2)], w5
	mov [temp + (7 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (36 * 2)]

	mov [temp + (6 * 2)], w5
	mov [temp + (7 * 2)], w6
	sub w5, w6, w5
	mov #cos18, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (53 * 2)]
	
	mov [in + (1 * 2)], w5
	mov [in + (30 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (8 * 2)]
		  
	mov [in + (1 * 2)], w5
	mov [in + (30 * 2)], w6
	sub w5, w6, w5
	mov #cos03, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (24 * 2)]

	mov [in + (14 * 2)], w5
	mov [in + (17 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (9 * 2)]
	
	mov [in + (14 * 2)], w5
	mov [in + (17 * 2)], w6
	sub w5, w6, w5
	mov #cos29, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (25 * 2)]

	mov [temp + (24 * 2)], w5
	mov [temp + (25 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (45 * 2)]	
	
	mov [temp + (24 * 2)], w5
	mov [temp + (25 * 2)], w6
	sub w5, w6, w5
	mov #cos06, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (63 * 2)]

	mov [temp + (8 * 2)], w5
	mov [temp + (9 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (37 * 2)]

	mov [temp + (8 * 2)], w5
	mov [temp + (9 * 2)], w6
	sub w5, w6, w5
	mov #cos06, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (54 * 2)]
		
	mov [in + (6 * 2)], w5
	mov [in + (25 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (10 * 2)]
		
	mov [in + (6 * 2)], w5
	mov [in + (25 * 2)], w6
	sub w5, w6, w5
	mov #cos13, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (26 * 2)]

	mov [in + (9 * 2)], w5
	mov [in + (22 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (11 * 2)]
	
	mov [in + (9 * 2)], w5
	mov [in + (22 * 2)], w6
	sub w5, w6, w5
	mov #cos19, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (27 * 2)]
	
	mov [temp + (26 * 2)], w5
	mov [temp + (27 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (46 * 2)]
	
	mov [temp + (26 * 2)], w5
	mov [temp + (27 * 2)], w6
	sub w5, w6, w5
	mov #cos26, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (64 * 2)]
	
	mov [temp + (10 * 2)], w5
	mov [temp + (11 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (38 * 2)]

	mov [temp + (10 * 2)], w5
	mov [temp + (11 * 2)], w6
	sub w5, w6, w5
	mov #cos26, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (55 * 2)]
		
	mov [in + (2 * 2)], w5
	mov [in + (29 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (12 * 2)]
	  
	mov [in + (2 * 2)], w5
	mov [in + (29 * 2)], w6
	sub w5, w6, w5
	mov #cos05, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (28 * 2)]

	mov [in + (13 * 2)], w5
	mov [in + (18 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (13 * 2)]
	
	mov [in + (13 * 2)], w5
	mov [in + (18 * 2)], w6
	sub w5, w6, w5
	mov #cos27, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (29 * 2)]

	mov [temp + (28 * 2)], w5
	mov [temp + (29 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (47 * 2)]

	mov [temp + (28 * 2)], w5
	mov [temp + (29 * 2)], w6
	sub w5, w6, w5
	mov #cos10, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (65 * 2)]

	mov [temp + (12 * 2)], w5
	mov [temp + (13 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (39 * 2)]

	mov [temp + (12 * 2)], w5
	mov [temp + (13 * 2)], w6
	sub w5, w6, w5
	mov #cos10, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (56 * 2)]
	
	mov [in + (5 * 2)], w5
	mov [in + (26 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (14 * 2)]
		  
	mov [in + (5 * 2)], w5
	mov [in + (26 * 2)], w6
	sub w5, w6, w5
	mov #cos11, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (30 * 2)]

	mov [in + (10 * 2)], w5
	mov [in + (21 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (15 * 2)]
		  
	mov [in + (10 * 2)], w5
	mov [in + (21 * 2)], w6
	sub w5, w6, w5
	mov #cos21, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (31 * 2)]
	
	mov [temp + (30 * 2)], w5
	mov [temp + (31 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (48 * 2)]

	mov [temp + (30 * 2)], w5
	mov [temp + (31 * 2)], w6
	sub w5, w6, w5
	mov #cos22, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (66 * 2)]

	mov [temp + (14 * 2)], w5
	mov [temp + (15 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (40 * 2)]

	mov [temp + (14 * 2)], w5
	mov [temp + (15 * 2)], w6
	sub w5, w6, w5
	mov #cos22, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (57 * 2)]
	
	mov [temp + (33 * 2)], w5
	mov [temp + (34 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (69 * 2)]

	mov [temp + (33 * 2)], w5
	mov [temp + (34 * 2)], w6
	sub w5, w6, w5
	mov #cos04, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (89 * 2)]

	mov [temp + (35 * 2)], w5
	mov [temp + (36 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (70 * 2)]

	mov [temp + (35 * 2)], w5
	mov [temp + (36 * 2)], w6
	sub w5, w6, w5
	mov #cos28, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (90 * 2)]

	mov [temp + (37 * 2)], w5
	mov [temp + (38 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (71 * 2)]

	mov [temp + (37 * 2)], w5
	mov [temp + (38 * 2)], w6
	sub w5, w6, w5
	mov #cos12, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (91 * 2)]

	mov [temp + (39 * 2)], w5
	mov [temp + (40 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (72 * 2)]

	mov [temp + (39 * 2)], w5
	mov [temp + (40 * 2)], w6
	sub w5, w6, w5
	mov #cos20, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (92 * 2)]

	mov [temp + (41 * 2)], w5
	mov [temp + (42 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (73 * 2)]

	mov [temp + (41 * 2)], w5
	mov [temp + (42 * 2)], w6
	sub w5, w6, w5
	mov #cos04, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (94 * 2)]

	mov [temp + (43 * 2)], w5
	mov [temp + (44 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (74 * 2)]

	mov [temp + (43 * 2)], w5
	mov [temp + (44 * 2)], w6
	sub w5, w6, w5
	mov #cos28, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (95 * 2)]

	mov [temp + (45 * 2)], w5
	mov [temp + (46 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (75 * 2)]

	mov [temp + (45 * 2)], w5
	mov [temp + (46 * 2)], w6
	sub w5, w6, w5
	mov #cos12, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (96 * 2)]

	mov [temp + (47 * 2)], w5
	mov [temp + (48 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (76 * 2)]

	mov [temp + (47 * 2)], w5
	mov [temp + (48 * 2)], w6
	sub w5, w6, w5
	mov #cos20, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (97 * 2)]
	
	mov [temp + (50 * 2)], w5
	mov [temp + (51 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (78 * 2)]

	mov [temp + (50 * 2)], w5
	mov [temp + (51 * 2)], w6
	sub w5, w6, w5
	mov #cos04, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (100 * 2)]

	mov [temp + (52 * 2)], w5
	mov [temp + (53 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (79 * 2)]

	mov [temp + (52 * 2)], w5
	mov [temp + (53 * 2)], w6
	sub w5, w6, w5
	mov #cos28, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (101 * 2)]

	mov [temp + (54 * 2)], w5
	mov [temp + (55 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (80 * 2)]

	mov [temp + (54 * 2)], w5
	mov [temp + (55 * 2)], w6
	sub w5, w6, w5
	mov #cos12, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (102 * 2)]

	mov [temp + (56 * 2)], w5
	mov [temp + (57 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (81 * 2)]

	mov [temp + (56 * 2)], w5
	mov [temp + (57 * 2)], w6
	sub w5, w6, w5
	mov #cos20, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (103 * 2)]
	
	mov [temp + (59 * 2)], w5
	mov [temp + (60 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (83 * 2)]

	mov [temp + (59 * 2)], w5
	mov [temp + (60 * 2)], w6
	sub w5, w6, w5
	mov #cos04, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (106 * 2)]

	mov [temp + (61 * 2)], w5
	mov [temp + (62 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (84 * 2)]

	mov [temp + (61 * 2)], w5
	mov [temp + (62 * 2)], w6
	sub w5, w6, w5
	mov #cos28, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (107 * 2)]

	mov [temp + (63 * 2)], w5
	mov [temp + (64 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (85 * 2)]

	mov [temp + (63 * 2)], w5
	mov [temp + (64 * 2)], w6
	sub w5, w6, w5
	mov #cos12, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (108 * 2)]

	mov [temp + (65 * 2)], w5
	mov [temp + (66 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (86 * 2)]

	mov [temp + (65 * 2)], w5
	mov [temp + (66 * 2)], w6
	sub w5, w6, w5
	mov #cos20, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (109 * 2)]
	
	mov [temp + (69 * 2)], w5
	mov [temp + (70 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (113 * 2)]

	mov [temp + (71 * 2)], w5
	mov [temp + (72 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (114 * 2)]
	
	mov [temp + (113 * 2)], w5
	mov [temp + (114 * 2)], w6
	add w5, w6, w5
	mov #(15 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [hi + w6]	
	
	mov [temp + (113 * 2)], w5
	mov [temp + (114 * 2)], w6
	sub w5, w6, w5
	mov #cos16, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [lo + slot]
	
	mov [temp + (73 * 2)], w5
	mov [temp + (74 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (115 * 2)]

	mov [temp + (75 * 2)], w5
	mov [temp + (76 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (116 * 2)]
	
	mov [temp + (115 * 2)], w5
	mov [temp + (116 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (32 * 2)]
	
	mov #(14 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [hi + w6]
	
	mov [temp + (78 * 2)], w5
	mov [temp + (79 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (118 * 2)]

	mov [temp + (80 * 2)], w5
	mov [temp + (81 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (119 * 2)]
	
	mov [temp + (118 * 2)], w5
	mov [temp + (119 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (58 * 2)]

	mov #(13 * 8 * 2), w6
 	add slot, w6, w6
	mov w5, [hi + w6]
	
	mov [temp + (83 * 2)], w5
	mov [temp + (84 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (121 * 2)]

	mov [temp + (85 * 2)], w5
	mov [temp + (86 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (122 * 2)]
	
	mov [temp + (121 * 2)], w5
	mov [temp + (122 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (67 * 2)]
	
	mov [temp + (67 * 2)], w5
	mov [temp + (32 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (49 * 2)]
	
	mov #(12 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [hi + w6]

	mov [temp + (89 * 2)], w5
	mov [temp + (90 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (125 * 2)]

	mov [temp + (91 * 2)], w5
	mov [temp + (92 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (126 * 2)]
	
	mov [temp + (125 * 2)], w5
	mov [temp + (126 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (93 * 2)]
	
	mov #(11 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [hi + w6]
	
	mov [temp + (94 * 2)], w5
	mov [temp + (95 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (128 * 2)]

	mov [temp + (96 * 2)], w5
	mov [temp + (97 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (129 * 2)]
	
	mov [temp + (128 * 2)], w5
	mov [temp + (129 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (98 * 2)]
	
	mov [temp + (98 * 2)], w5
	mov [temp + (49 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (68 * 2)]
	
	mov #(10 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [hi + w6]
	
	mov [temp + (100 * 2)], w5
	mov [temp + (101 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (132 * 2)]

	mov [temp + (102 * 2)], w5
	mov [temp + (103 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (133 * 2)]

	mov [temp + (132 * 2)], w5
	mov [temp + (133 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (104 * 2)]
	
	mov [temp + (104 * 2)], w5
	mov [temp + (58 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (82 * 2)]
	
	mov #(9 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [hi + w6]
	
	mov [temp + (106 * 2)], w5
	mov [temp + (107 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (136 * 2)]

	mov [temp + (108 * 2)], w5
	mov [temp + (109 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (137 * 2)]
	
	mov [temp + (136 * 2)], w5
	mov [temp + (137 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (110 * 2)]
	
	mov [temp + (110 * 2)], w5
	mov [temp + (67 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (87 * 2)]
	
	mov [temp + (87 * 2)], w5
	mov [temp + (68 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (77 * 2)]
	
	mov #(8 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [hi + w6]
	
	mov [temp + (69 * 2)], w5
	mov [temp + (70 * 2)], w6
	sub w5, w6, w5
	mov #cos08, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (141 * 2)]

	mov [temp + (71 * 2)], w5
	mov [temp + (72 * 2)], w6
	sub w5, w6, w5
	mov #cos24, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (142 * 2)]

	mov [temp + (141 * 2)], w5
	mov [temp + (142 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (143 * 2)]
	
	mov #(7 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [hi + w6]
	
	mov [temp + (141 * 2)], w5
	mov [temp + (142 * 2)], w6
	sub w5, w6, w5
	mov #cos16, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	add w5, w5, w5
	mov [temp + (143 * 2)], w6
	sub w5, w6, w5
	mov #(8 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [lo + w6]
		
	mov [temp + (73 * 2)], w5
	mov [temp + (74 * 2)], w6
	sub w5, w6, w5
	mov #cos08, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (144 * 2)]

	mov [temp + (75 * 2)], w5
	mov [temp + (76 * 2)], w6
	sub w5, w6, w5
	mov #cos24, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (145 * 2)]

	mov [temp + (144 * 2)], w5
	mov [temp + (145 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (146 * 2)]

	mov [temp + (146 * 2)], w5
	mov [temp + (77 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (88 * 2)]
	
	mov #(6 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [hi + w6]	
	
	mov [temp + (78 * 2)], w5
	mov [temp + (79 * 2)], w6
	sub w5, w6, w5
	mov #cos08, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (148 * 2)]

	mov [temp + (80 * 2)], w5
	mov [temp + (81 * 2)], w6
	sub w5, w6, w5
	mov #cos24, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (149 * 2)]

	mov [temp + (148 * 2)], w5
	mov [temp + (149 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (150 * 2)]
	
	mov [temp + (150 * 2)], w5
	mov [temp + (82 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (105 * 2)]
	
	mov #(5 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [hi + w6]
	
	mov [temp + (83 * 2)], w5
	mov [temp + (84 * 2)], w6
	sub w5, w6, w5
	mov #cos08, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (152 * 2)]

	mov [temp + (85 * 2)], w5
	mov [temp + (86 * 2)], w6
	sub w5, w6, w5
	mov #cos24, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (153 * 2)]

	mov [temp + (152 * 2)], w5
	mov [temp + (153 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (154 * 2)]
	
	mov [temp + (154 * 2)], w5
	mov [temp + (87 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (111 * 2)]
	
	mov [temp + (111 * 2)], w5
	mov [temp + (88 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (99 * 2)]
	
	mov #(4 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [hi + w6]	
	
	mov [temp + (89 * 2)], w5
	mov [temp + (90 * 2)], w6
	sub w5, w6, w5
	mov #cos08, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (157 * 2)]

	mov [temp + (91 * 2)], w5
	mov [temp + (92 * 2)], w6
	sub w5, w6, w5
	mov #cos24, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (158 * 2)]

	mov [temp + (157 * 2)], w5
	mov [temp + (158 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (159 * 2)]
	
	mov [temp + (159 * 2)], w5
	mov [temp + (93 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (127 * 2)]
	
	mov #(3 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [hi + w6]	
	
	mov [temp + (125 * 2)], w5
	mov [temp + (126 * 2)], w6
	sub w5, w6, w5
	mov #cos16, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	add w5, w5, w5
	mov [temp + (127 * 2)], w6
	sub w5, w6, w5	
	mov w5, [temp + (160 * 2)]
	
	mov #(4 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [lo + w6]	
	
	mov [temp + (157 * 2)], w5
	mov [temp + (158 * 2)], w6
	sub w5, w6, w5
	mov #cos16, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	add w5, w5, w5
	mov [temp + (159 * 2)], w6
	sub w5, w6, w5
	add w5, w5, w5
	mov [temp + (160 * 2)], w6
	sub w5, w6, w5
	mov #(12 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [lo + w6]
	
	mov [temp + (94 * 2)], w5
	mov [temp + (95 * 2)], w6
	sub w5, w6, w5
	mov #cos08, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (161 * 2)]

	mov [temp + (96 * 2)], w5
	mov [temp + (97 * 2)], w6
	sub w5, w6, w5
	mov #cos24, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (162 * 2)]

	mov [temp + (161 * 2)], w5
	mov [temp + (162 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (163 * 2)]
	
	mov [temp + (163 * 2)], w5
	mov [temp + (98 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (130 * 2)]
	
	mov [temp + (130 * 2)], w5
	mov [temp + (99 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (112 * 2)]
	
	mov #(2 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [hi + w6]
	
	mov [temp + (128 * 2)], w5
	mov [temp + (129 * 2)], w6
	sub w5, w6, w5
	mov #cos16, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	add w5, w5, w5
	mov [temp + (130 * 2)], w6
	sub w5, w6, w5	
	mov w5, [temp + (164 * 2)]
	
	mov [temp + (100 * 2)], w5
	mov [temp + (101 * 2)], w6
	sub w5, w6, w5
	mov #cos08, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (166 * 2)]

	mov [temp + (102 * 2)], w5
	mov [temp + (103 * 2)], w6
	sub w5, w6, w5
	mov #cos24, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (167 * 2)]

	mov [temp + (166 * 2)], w5
	mov [temp + (167 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (168 * 2)]
	
	mov [temp + (168 * 2)], w5
	mov [temp + (104 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (134 * 2)]
	
	mov [temp + (134 * 2)], w5
	mov [temp + (105 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (120 * 2)]
	
	add slot, #(1 * 8 * 2), w6
	mov w5, [hi + w6]
		
	mov [temp + (118 * 2)], w5
	mov [temp + (119 * 2)], w6
	sub w5, w6, w5
	mov #cos16, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	add w5, w5, w5
	mov [temp + (120 * 2)], w6
	sub w5, w6, w5	
	mov w5, [temp + (135 * 2)]

	mov #(2 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [lo + w6]
	
	mov [temp + (132 * 2)], w5
	mov [temp + (133 * 2)], w6
	sub w5, w6, w5
	mov #cos16, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	add w5, w5, w5
	mov [temp + (134 * 2)], w6
	sub w5, w6, w5	
	mov w5, [temp + (169 * 2)]
	
	mov [temp + (169 * 2)], w5
	mov [temp + (135 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (151 * 2)]
	
	mov #(6 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [lo + w6]	
	
	mov [temp + (148 * 2)], w5
	mov [temp + (149 * 2)], w6
	sub w5, w6, w5
	mov #cos16, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	add w5, w5, w5
	mov [temp + (150 * 2)], w6
	sub w5, w6, w5
	add w5, w5, w5
	mov [temp + (151 * 2)], w6
	sub w5, w6, w5
	mov w5, [temp + (170 * 2)]
	
	mov #(10 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [lo + w6]	

	mov [temp + (166 * 2)], w5
	mov [temp + (167 * 2)], w6
	sub w5, w6, w5
	mov #cos16, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	add w5, w5, w5
	mov [temp + (168 * 2)], w6
	sub w5, w6, w5
	add w5, w5, w5
	mov [temp + (169 * 2)], w6
	sub w5, w6, w5
	add w5, w5, w5
	mov [temp + (170 * 2)], w6
	sub w5, w6, w5
	mov #(14 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [lo + w6]

	mov [temp + (106 * 2)], w5
	mov [temp + (107 * 2)], w6
	sub w5, w6, w5
	mov #cos08, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (171 * 2)]

	mov [temp + (108 * 2)], w5
	mov [temp + (109 * 2)], w6
	sub w5, w6, w5
	mov #cos24, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	mov w5, [temp + (172 * 2)]

	mov [temp + (171 * 2)], w5
	mov [temp + (172 * 2)], w6
	add w5, w6, w5
	mov w5, [temp + (173 * 2)]
	
	mov [temp + (173 * 2)], w5
	mov [temp + (110 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (138 * 2)]
	
	mov [temp + (138 * 2)], w5
	mov [temp + (111 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (123 * 2)]
	
	mov [temp + (121 * 2)], w5
	mov [temp + (122 * 2)], w6
	sub w5, w6, w5
	mov #cos16, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	add w5, w5, w5
	mov [temp + (123 * 2)], w6
	sub w5, w6, w5	
	mov w5, [temp + (139 * 2)]

	mov [temp + (123 * 2)], w5
	mov [temp + (112 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (117 * 2)]
	mov w5, [hi + slot]
	
	mov [temp + (115 * 2)], w5
	mov [temp + (116 * 2)], w6
	sub w5, w6, w5
	mov #cos16, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	add w5, w5, w5
	mov [temp + (117 * 2)], w6
	sub w5, w6, w5	
	mov w5, [temp + (124 * 2)]
	add slot, #(1 * 8 * 2), w6
	mov w5, [lo + w6]	
	
	mov [temp + (139 * 2)], w5
	mov [temp + (124 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (131 * 2)]
	mov #(3 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [lo + w6]
	
	mov [temp + (164 * 2)], w5
	mov [temp + (131 * 2)], w6
	add w5, w5, w5
	sub w5, w6, w5
	mov w5, [temp + (140 * 2)]
	mov #(5 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [lo + w6]
	
	mov [temp + (136 * 2)], w5
	mov [temp + (137 * 2)], w6
	sub w5, w6, w5
	mov #cos16, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	add w5, w5, w5
	mov [temp + (138 * 2)], w6
	sub w5, w6, w5	
	mov w5, [temp + (174 * 2)]
	
	mov [temp + (174 * 2)], w5
	mov [temp + (139 * 2)], w6
	add w5, w5, w5			
	sub w5, w6, w5
	mov w5, [temp + (155 * 2)]
	
	mov [temp + (155 * 2)], w5
	mov [temp + (140 * 2)], w6
	add w5, w5, w5			
	sub w5, w6, w5
	mov w5, [temp + (147 * 2)]
	
	mov #(7 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [lo + w6]
	
	mov [temp + (144 * 2)], w5
	mov [temp + (145 * 2)], w6
	sub w5, w6, w5
	mov #cos16, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	add w5, w5, w5
	mov [temp + (146 * 2)], w6
	sub w5, w6, w5
	add w5, w5, w5
	mov [temp + (147 * 2)], w6
	sub w5, w6, w5
	mov w5, [temp + (156 * 2)]
	mov #(9 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [lo + w6]	
	
	mov [temp + (152 * 2)], w5
	mov [temp + (153 * 2)], w6
	sub w5, w6, w5
	mov #cos16, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	add w5, w5, w5
	mov [temp + (154 * 2)], w6
	sub w5, w6, w5
	add w5, w5, w5
	mov [temp + (155 * 2)], w6
	sub w5, w6, w5
	mov w5, [temp + (175 * 2)]
	
	mov [temp + (175 * 2)], w5
	mov [temp + (156 * 2)], w6
	add w5, w5, w5			
	sub w5, w6, w5
	mov w5, [temp + (165 * 2)]
	
	mov #(11 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [lo + w6]
	
	mov [temp + (161 * 2)], w5
	mov [temp + (162 * 2)], w6
	sub w5, w6, w5
	mov #cos16, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	add w5, w5, w5
	mov [temp + (163 * 2)], w6
	sub w5, w6, w5
	add w5, w5, w5
	mov [temp + (164 * 2)], w6
	sub w5, w6, w5
	add w5, w5, w5
	mov [temp + (165 * 2)], w6
	sub w5, w6, w5
	mov w5, [temp + (176 * 2)]
		
	mov #(13 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [lo + w6]
	
	mov [temp + (171 * 2)], w5
	mov [temp + (172 * 2)], w6
	sub w5, w6, w5
	mov #cos16, w7
	mpy w5 * w7, b
	sac.r b, #-(MP3_INTBITS), w5
	add w5, w5, w5
	mov [temp + (173 * 2)], w6
	sub w5, w6, w5
	add w5, w5, w5
	mov [temp + (174 * 2)], w6
	sub w5, w6, w5
	add w5, w5, w5
	mov [temp + (175 * 2)], w6
	sub w5, w6, w5
	add w5, w5, w5
	mov [temp + (176 * 2)], w6
	sub w5, w6, w5
	mov #(15 * 8 * 2), w6
	add slot, w6, w6
	mov w5, [lo + w6]
	
	.if __dsPIC33E == 1
		pop DSWPAG
		pop DSRPAG
	.endif

.endm

.text
.global _mp3_synth_mute
_mp3_synth_mute:
;{
	.if __dsPIC33E == 1
		push DSWPAG
		movpag #edspage(_mp3_filterbank), DSWPAG
		mov #edsoffset(_mp3_filterbank), w0	
	.else
		mov #_mp3_filterbank, w0
	.endif

	repeat #((2 * 2 * 2 * 16 * 8) - 1)
	clr [w0++]
	
	;mov #_mp3_phase, w0
	;clr [w0]

	.if __dsPIC33E == 1
		pop DSWPAG
	.endif

	return
;}

;
; w0 no_of_samples
; w1 no_of_channels
;
.global _mp3_synth
_mp3_synth:
;{
	.equ sc, w6

	exch w0, w1

	push w8
	push w9
	push w10
	push w11
	push w12
	push w13
	push w14
	
	;
	; save and set PSV/EDS paging registers
	;
	.if __dsPIC33E == 1
		push DSRPAG
		push DSWPAG
		movpag #psvpage(_mp3_D), DSRPAG
		movpag #edspage(_mp3_filterbank), DSWPAG
	.else
		push PSVPAG
		mov #psvpage(_mp3_D), w8
		mov w8, PSVPAG
	.endif

	;
	; stack w0 (sc)
	;
	mov w0, [w15++]

	mov _mp3_out, w10
	dec w1, w0
	clr w14
	
	.if __dsPIC33E == 0 || TWO_LEVEL_DO_LOOP == 1
		mov w0, [w15++]
__synth_channels_start:
	.else
		do w0, __synth_channels
	.endif
	;{
		;
		; experimental code for normalizing subband samples
		; before DCT
		;
		.if 0
			.if __dsPIC33E == 0 || TWO_LEVEL_DO_LOOP == 1
				mov #1, w0
				mov [w15 - 2], w6
				sub w0, w6, w0
				rcall _mp3_synth_normalize
			.else
			compile_Error
			.endif
		.endif
		mov #_mp3_phase, w6 
		mov [w6], w13
		.if __dsPIC33E == 0 || TWO_LEVEL_DO_LOOP == 1
			mov [w15 - 4], w0			
		.else
			mov [w15 - 2], w0			
		.endif
		dec w0, w0
		clr w6 ;sample counter
		
		do w0, __synth_samples
		;{
			mov _mp3_out, w0		; &out.sb_samples[ch] ->w0
			mov #(36 * 32 * 2), w1
			mul.uu w1, w14, w4
			add w0, w4, w0

			sl w6, #6, w4				; mul s by (32 * 2)
			add w0, w4, w0				; &out->sb_samples[ch][s] -> w0
			lsr w13, w1					; phase / 2 -> w1	
			and w13, #1, w3				; pahse & 1 -> w3  
			sl w3, #8, w4				; mul by 16 * 8 * 2
			
			.if __dsPIC33E == 1
				mov #edsoffset(_mp3_filterbank), w2	; mp3_filterbank -> w2
			.else
				mov #_mp3_filterbank, w2
			.endif

			sl w14, #10, w5
			add w5, w2, w2			
			add w2, w4, w2				; &mp3_filterbank[ch][0][phase & 1][0][0] -> w2
			mov #(2 * 16 * 8 * 2), w4	; 
			add w2, w4, w3				; &mp3_filterbank[ch][1][phase & 1][0][0] -> w3
			;
			; experimental code for re-normalizig
			; DCT output
			;
			.if 0
				push w6
				push w1
				push w2
				push w3
				dct
				pop w3
				pop w2
				pop w1
				.if __dsPIC33E == 0 || TWO_LEVEL_DO_LOOP == 1
					mov [w15 - 2], w0
					rcall _mp3_synth_renormalize
				.else
					compile_Error
				.endif
				pop w6
			.else
				push w6
				dct
				pop w6
			.endif
			
			mov #(~1), w0
			and w13, w0, w0	;ph in w13
			dec w13, w1
			and w1, #(0xF), w1
			ior w1, #1, w1
			sl w0, #1, w0
			sl w1, #1, w1
			
			.if __dsPIC33E == 1
				mov #edsoffset(_mp3_filterbank), w9
			.else
				mov #_mp3_filterbank, w9
			.endif
			
			and w13, #1, w7
			sl w7, #8, w7			; mul by 16 * 8 * 2

			sl w14, #10, w5
			add w5, w9, w9
			add w9, w7, w2

			com w13, w7
			and w7, #1, w7
			sl w7, #8, w7	; mul by (16 * 8 * 2)
			add w9, w7, w3
			
			mov #(2 * 16 * 8 * 2), w4
			add w3, w4, w4

			mov #psvoffset(_mp3_D), w8	; w8 = &D

			add w8, w1, w9
			mov w3, w11
			clr a, [w9], w7, [w11] += 2, w5
			add w9, #(14 * 2), w9
			repeat #7			
			msc w5*w7, a, [w9] -= 4, w7, [w11] += 2, w5

			add w8, w0, w9
			mov w2, w11
			movsac a, [w9], w7, [w11] += 2, w5
			add w9, #(14 * 2), w9
			repeat #7
			mac w5*w7, a, [w9] -= 4, w7, [w11] += 2, w5
			sac.r a, #-(MP3_INTBITS + MP3_SCALEBITS), w5
			mov w5, [w10++]

			mov #(30 * 2), w5
			add w10, w5, w12

			do #14, __synth_sb
			;{
				mov #(32 * 2), w5
				add w8, w5, w8
				add w2, #(8 * 2), w2

				add w8, w1, w9
				mov w4, w11
				clr a, [w9], w7, [w11] += 2, w5
				add w9, #(14 * 2), w9
				repeat #7				
				msc w5*w7, a, [w9] -= 4, w7, [w11] += 2, w5

				add w8, w0, w9
				add w9, #(2 * 2), w9
				add w2, #(7 * 2), w11
				movsac a, [w9] += 4, w7, [w11] -= 2, w5
				repeat #7
				mac w5*w7, a, [w9] += 4, w7, [w11] -= 2, w5
				sac.r a, #-(MP3_INTBITS + MP3_SCALEBITS), w5
				mov w5, [w10++]

				sub w8, w0, w9
				mov w2, w11
				add w9, #(15 * 2), w9
				clr a, [w9] += 4, w7, [w11] += 2, w5
				repeat #7
				mac w5*w7, a, [w9] += 4, w7, [w11] += 2, w5

				sub w8, w1, w9
				add w4, #(7 *  2), w11
				mov #(29 * 2), w5				
				add w9, w5, w9
				movsac a, [w9] -= 4, w7, [w11] -= 2, w5
				repeat #7
				mac w5*w7, a, [w9] -= 4, w7, [w11] -= 2, w5
				sac.r a, #-(MP3_INTBITS + MP3_SCALEBITS), w5
				mov w5, [w12--]
				
				__synth_sb:
				add w4, #(8 * 2), w4
			;}

			mov #(32 * 2), w5
			add w8, w5, w8		;D++

			add w8, w1, w9
			mov w4, w11
			clr a, [w9], w7, [w11] += 2, w5
			add w9, #(14 * 2), w9
			repeat #7
			msc w5*w7, a, [w9] -= 4, w7, [w11] += 2, w5
			sac.r a, #-(MP3_INTBITS + MP3_SCALEBITS), w5
			mov w5, [w10++]
			
			add w10, #(15 * 2), w10

			inc w13, w13
			and w13, #0xF, w13
			__synth_samples:
			inc sc, sc
		;}
		__synth_channels:
		inc w14, w14
		;
		; decrease channel count and loop back to
		; start if it haven't reached zero (for devices
		; with only two levels of do loops).
		;
		.if __dsPIC33E == 0 || TWO_LEVEL_DO_LOOP == 1
			mov [w15 - 2], w7
			dec w7, w5
			mov w5, [w15 - 2]
			cp0 w7
			bra neq, __synth_channels_start
		.endif
	;}

	mov #_mp3_phase, w6 
	mov w13, [w6]
	mov [--w15], w0

	.if __dsPIC33E == 0 || TWO_LEVEL_DO_LOOP == 1
		dec2 w15, w15
	.endif

	;
	; restore PSV/EDS paging registers
	;
	.if __dsPIC33E == 1
		pop DSWPAG
		pop DSRPAG	
	.else
		pop PSVPAG
	.endif
	
	pop w14
	pop w13
	pop w12
	pop w11
	pop w10
	pop w9
	pop w8
	return
;}

