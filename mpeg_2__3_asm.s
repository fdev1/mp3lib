;
; mpeg_2_3_asm.s
;
; Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
; All rights reserved
;

.include "mpeg_2.inc"

.extern _mp3_out					; xmemory
.extern _mp3_overlap				; EDS or DS (compile time option)
.extern _mp3_imdct_s				; PSV
.extern _mp3_imdct_l				; PSV	
.extern _mp3_window_l				; PSV
.extern _mp3_window_s				; PSV
.extern _mp3_xr						; ymemory (EDS on dspic33e)
.extern _mp3_y						; ymemory (EDS on dspic33e)
.extern _mp3_z						; xmemory
.extern _mp3__cs					; PSV
.extern _mp3_ca						; PSV
.extern _mp3_rq_tbl
.extern _mp3_rq_exp_tbl
.extern _mp3_rq_powers
.text

;
; w0 = ch;
;
.if 1
.global _mp3_layer3_normalize
_mp3_layer3_normalize:
;{
	.if __dsPIC33E == 1
		push DSWPAG
		push DSRPAG
		movpag #edspage(_mp3_xr), DSRPAG
		movpag #edspage(_mp3_xr), DSWPAG
	.endif


	mov #(576 * 2), w1
	mul.uu w0, w1, w0
	.if __dsPIC33E == 1
		mov #edsoffset(_mp3_xr), w1
	.else
		mov #_mp3_xr, w1
	.endif
	add w1, w0, w1
	mov w1, w3
	;
	; find exponent
	;
	mov #-15, w0
	fbcl [w1++], w2
	do #574, _normalize_loop1
	cp w2, w0
	bra lt, _normalize_loop1
	mov w2, w0
_normalize_loop1:
	fbcl [w1++], w2
	cp w2, w0
	bra lt, _fix_exp
	mov w2, w0

_fix_exp:
	add w0, #(MP3_INTBITS + 1), w0
	cp0 w0
	bra lt, _normalize
	clr w0
	
	.if __dsPIC33E == 1
		pop DSRPAG
		pop DSWPAG
	.endif
	
	return
	
	;
	; normalize
	;
_normalize:
	mov w3, w1
	do #575, _normalize_loop2
	lac [w1], a
	sftac a, w0
	_normalize_loop2:
	sac.r a, [w1++]
	neg w0, w0

	.if __dsPIC33E == 1
		pop DSRPAG
		pop DSWPAG
	.endif
	
	return
;}

;
; w0 = exp
;
.global _mp3_layer3_renormalize
_mp3_layer3_renormalize:
;{
	mov #_mp3_z, w1
	do #35, _renormalize_loop
	lac [w1], a
	sftac a, w0
_renormalize_loop:
	sac.r a, [w1++]
	return
;}
.endif

;
; w0 = value
; w1 = exponent
;
.global _mp3_layer3_rq_sample
_mp3_layer3_rq_sample:
;{
	;
	; w2 = sign(w1) * (abs(w1) / 4)
	; w3 = sign(w1) * (abs(w1) % 4)
	;
	mov w1, w5
	btsc w1, #15
	neg w1, w5
	lsr w5, #2, w2
	sl w2, #2, w3
	sub w5, w3, w3
	btst w1, #15
	bra z, _positive_exp
	neg w2, w2
	neg w3, w3
_positive_exp:

	mov #psvoffset(_mp3_rq_exp_tbl), w1
	add w1, w0, w1
	ze [w1], w1
	add w2, w1, w2	
	neg w2, w2
	
	.if __dsPIC33E == 1
		mov DSRPAG, w7
		movpag #psvpage(_mp3_rq_tbl), DSRPAG
	.else
		mov PSVPAG, w7
		mov #psvpage(_mp3_rq_tbl), w6
		mov w6, PSVPAG	
	.endif
	mov #psvoffset(_mp3_rq_tbl), w1
	sl w0, #1, w0
	lac [w1 + w0], a
	
	.if __dsPIC33E == 1
		mov w7, DSRPAG
	.else
		mov w7, PSVPAG
	.endif

	clr w0
	cp w2, #15
	bra gt, _rq_return	
	sftac a, w2
	sac.r a, w0

	cp0 w3
	bra z, _rq_return
	mov #(_mp3_rq_powers + (3 * 2)), w1	;psvoffset needed?
	sl w3, #1, w3
	
	;mov [w1 + w3], w6
	add w1, w3, w1
	mov [w1], w6
	.if __dsPIC33E == 1
		mul.ss w0, w6, a
	.else
		mov w0, w5
		mpy w5 * w6, a
	.endif
	sac.r a, #-(MP3_INTBITS), w0

_rq_return:
	return	
	
;}

;
; w0 = ch
; w1 = sfbwidth (byte ptr)
; w2 = mixed_block
; w3 = gr
;
; note: this routine can be improved
;
.global _mp3_layer3_reorder
_mp3_layer3_reorder:
;{
	.equ temp, w3
	.equ sfbwidth, w1
	.equ sb, w9
	.equ sbw, w8
	.equ sw, w11
	.equ f, w12
	.equ w, w2
	.equ tmp_ptr, w7
	.equ xr_ptr, w10

	push w8
	push w9
	push w10
	push w11
	push w12
	;
	; save paging register
	;	
	.if __dsPIC33E == 1
		push DSWPAG	
	.endif
	;
	; mp3_out.xr_temp[1][1][0] -> temp
	;
	mov _mp3_out, temp
	mov #(576 * 2 * 3), w4
	add temp, w4, temp
	;
	; allocate memory from stack
	;
	; sbw[3]
	; sw[3]
	;
	mov w15, sbw
	add w15, #(3 * 2), sw
	add w15, #(6 * 2), w15
	clr sb
	;
	; set paging register for writing
	; to mp3_xr
	;
	.if __dsPIC33E == 1
		movpag #edspage(_mp3_xr), DSWPAG
	.endif
	
	cp0 w2
	bra z, __normal_block
	mov #2, sb
	clr w4
	.if __dsPIC33E == 0
		mov #36, w6
	.endif
__mixed_block:
	ze [sfbwidth++], w5
	add w4, w5, w4    
	.if __dsPIC33E == 0
		cp w4, w6
	.else
    	cp w4, #36
    .endif
    bra lt, __mixed_block    
__normal_block:
	clr w4
	mov sb, [sbw]
	mov sb, [sbw + (1 * 2)]
	mov sb, [sbw + (2 * 2)]
	mov w4, [sw]
	mov w4, [sw + (1 * 2)]
	mov w4, [sw + (2 * 2)]
	;
	;f = *sfbwidth++;
	;w = 0;
	;
	ze [sfbwidth++], f
	clr w
	;
	; &tmp[ sbw[0] ][0][0] -> w7
	;
	mov temp, tmp_ptr
	mov #(3 * 6 * 2), w4
	mul.uu w4, [sbw], w4
	add tmp_ptr, w4, tmp_ptr
	;
	; (576 - (18 * sb) - 1) -> w6
	;
	mul.uu sb, #18, w4
	mov #575, w5
	sub w5, w4, w6
	;
	; &mp3_xr[ch][l] -> w10
	;
	.if __dsPIC33E == 1
		mov #edsoffset(_mp3_xr), xr_ptr
	.else
		mov #_mp3_xr, xr_ptr
	.endif
	sl w4, w4
	add xr_ptr, w4, xr_ptr
	mov #(576 * 2), w4
	mul.uu w4, w0, w4
	add xr_ptr, w4, xr_ptr
	
	do w6, __reorder_loop
	;{
		cp0 f
		bra nz, __continue_reorder
		ze [sfbwidth++], f
		inc w, w
		inc2 sw, sw
		inc2 sbw, sbw
		cp w, #3
		bra neq, __recalc_pointer
		clr w
		sub sw, #(3 * 2), sw
		sub sbw, #(3 * 2), sbw

	__recalc_pointer:
		mov temp, tmp_ptr
		add tmp_ptr, [sw], tmp_ptr
		mul.uu w, #(6 * 2), w4
		add tmp_ptr, w4, tmp_ptr
		mov #(3 * 6 * 2), w5
		mul.uu w5, [sbw], w4
		add tmp_ptr, w4, tmp_ptr
		
	__continue_reorder:
		movsac a, [xr_ptr] += 2, w4
		mov w4, [tmp_ptr++]		
		inc2 [sw], [sw]

		mov #12, w4
		cp w4, [sw]
		bra neq, __reorder_loop
		clr [sw]
		inc [sbw], [sbw]
		add tmp_ptr, #(2 * 6 * 2), tmp_ptr
		__reorder_loop:
		dec f, f
	;}

	;
	; ((576 - (18 * sb)) - 1) -> w
	;
	mul.uu sb, #18, w4
	mov #575, w5
	sub w5, w4, w
	;
	; &mp3_xr[ch][18 * sb] -> w10
	;
	.if __dsPIC33E == 1
		mov #edsoffset(_mp3_xr), xr_ptr
	.else
		mov #_mp3_xr, xr_ptr
	.endif
	mov #(576 * 2), w4
	mul.uu w0, w4, w4
	add xr_ptr, w4, xr_ptr
	mov #(18 * 2), w4
	mul.uu w4, sb, w4
	add xr_ptr, w4, xr_ptr
	;
	; &tmp[sb][0][0] -> w7
	;
	mov temp, tmp_ptr
	mov #(3 * 6 * 2), w4
	mul.uu w4, sb, w4
	add tmp_ptr, w4, tmp_ptr
	;
	; copy reordered samples
	;	
	do w, __copy_reordered_loop
	;{	
		mov [tmp_ptr++], w4
		__copy_reordered_loop:
		mov w4, [xr_ptr++]
	;}
	
	;
	; free stack
	;
	sub w15, #(6 * 2), w15
	;
	; restore paging register
	;
	.if __dsPIC33E == 1
		pop DSWPAG
	.endif
	;
	; restore registers
	;
	pop w12
	pop w11
	pop w10
	pop w9
	pop w8
	return
;}

;
; w0 = ch
; w1 = lines
;
; Assumptions:
; 	#psvpage(_mp3_ca) == #psvpage(_mp3__cs)
;
.global _mp3_layer3_aliasreduce
_mp3_layer3_aliasreduce:
;{
	;
	; save working registers
	;
	push w8
	push w9
	push w10
	push w11
	;
	; save paging registers
	;
	.if __dsPIC33E == 1
		push DSRPAG
		push DSWPAG
	.else
		push PSVPAG
	.endif
	;
	; &mp3_xr[ch][18] -> w10
	; &mp3_xr[ch][17] -> w11
	;
	.if __dsPIC33E == 1
		mov #edsoffset(_mp3_xr), w10
	.else
		mov #_mp3_xr, w10
	.endif
	mov #(576 * 2), w2
	mul.uu w2, w0, w4
	add w10, w4, w10
	mov #(18 * 2), w4
	add w10, w4, w10	
	sub w10, #2, w11

	.if __dsPIC33E == 1	
		movpag #edspage(_mp3_xr), DSWPAG
		movpag #psvpage(_mp3__cs), DSRPAG
	.else
		mov #psvpage(_mp3__cs), w4
		mov w4, PSVPAG
	.endif
	
	movsac a, [w10] += 2, w4
	movsac b, [w11] -= 2, w6

	mov #0, w7
	mov #576, w3
	cpsne w1, w3
	mov #30, w7

	do w7, __alias_reduce_outter
	;{
		mov #psvoffset(_mp3_ca), w8
		mov #psvoffset(_mp3__cs), w9
		movsac a, [w8] += 2, w5
		movsac b, [w9] += 2, w7

		do #7, __alias_reduce_inner
		;{
			mpy w4 * w7, b
			mac w6 * w5, b
			mpy w6 * w7, a, [w9] += 2, w7, [w11] -= 2, w6
			msc w4 * w5, a, [w8] += 2, w5, [w10] += 2, w4
			sac.r a, #-(MP3_INTBITS), w0
			sac.r b, #-(MP3_INTBITS), w1
			mov w0, [w11 + 4]
__alias_reduce_inner:
			mov w1, [w10 - 4]
		;}

		add w10, #(9 * 2), w10
		mov #(27 * 2), w0
		add w11, w0, w11
		movsac a, [w10] += 2, w4
__alias_reduce_outter:
		movsac b, [w11] -= 2, w6
	
	;}
	
	;
	; restore paging registers
	;
	.if __dsPIC33E == 1
		pop DSWPAG
		pop DSRPAG
	.else
		pop PSVPAG
	.endif
	;
	; restore working registers
	;
	pop w11
	pop w10
	pop w9
	pop w8
	return
;}

;
; IMDCT and windowing of long blocks
;
; note: these can be improved to remove NOPs from
; loops
;
; w0 = ch
; w1 = sb
; w2 = block_type
;
.global _mp3_layer3_imdct_l
_mp3_layer3_imdct_l:
;{
	;
	; mul subband by 18 to get offset
	;
	mul.uu w1, #18, w4
	mov w4, w1

	mov #0, w3
	mov #1, w4
	mov #3, w5

	cpsne w2, w3
	goto _normal_window
	cpsne w2, w4
	goto _start_window
	cpsne w2, w5
	goto _stop_window
	goto _mp3_layer3_imdct
	
	;sl w2, #1, w2
	;bra w2
	;goto _normal_window
	;goto _start_window
	;goto _mp3_layer3_imdct36
	;goto _stop_window
	;goto _mp3_layer3_imdct_l
;}

;
; normal block window
;
_normal_window:
;{
	push w8
	push w11
	
	.if __dsPIC33E == 1
		push DSRPAG
	.else
		push PSVPAG
	.endif

	rcall _mp3_layer3_imdct

	.if __dsPIC33E == 1
		movpag #psvpage(_mp3_window_l), DSRPAG
	.else
		mov #psvpage(_mp3_window_l), w0
		mov w0, PSVPAG
	.endif
	
	mov #_mp3_z, w0
	mov #psvoffset(_mp3_window_l), w8
	
	.if __dsPIC33E == 1
		mov #edsoffset(_mp3_y), w11
	.else
		mov #_mp3_y, w11
	.endif
		
	movsac a, [w8] += 2, w6, [w11] += 2, w7
	
	do #34, __normal_window_loop
	;{
		nop
		mpy w6 * w7, a, [w8] += 2, w6, [w11] += 2, w7
		sac.r a, #-(MP3_INTBITS), w5
		__normal_window_loop:
		mov w5, [w0++]
	;}

	mpy w6 * w7, a
	sac.r a, #-(MP3_INTBITS), w5
	mov w5, [w0]

	.if __dsPIC33E == 1
		pop DSRPAG
	.else
		pop PSVPAG
	.endif
	
	pop w11
	pop w8
	return
;}

;
; start block window
;
_start_window:
;{
	rcall _mp3_layer3_imdct

	push w8
	push w11
	
	.if __dsPIC33E == 1
		push DSRPAG
		movpag #psvpage(_mp3_window_l), DSRPAG
	.else
		push PSVPAG
		mov #psvpage(_mp3_window_l), w0
		mov w0, PSVPAG
	.endif
	
	mov #_mp3_z, w0
	mov #psvoffset(_mp3_window_l), w8
	
	.if __dsPIC33E == 1
		mov #edsoffset(_mp3_y), w11
	.else
		mov #_mp3_y, w11
	.endif
	
	movsac a, [w8] += 2, w6, [w11] += 2, w7

	do #17, __start_window_loop1
	;{
		nop
		mpy w6 * w7, a, [w8] += 2, w6, [w11] += 2, w7
		sac.r a, #-(MP3_INTBITS), w5
		__start_window_loop1:
		mov w5, [w0++]
	;}	

	do #5, __start_window_loop2
	;{
		mov w7, [w0++]
		__start_window_loop2:
		movsac a, [w11] += 2, w7
	;}
	
	.if __dsPIC33E == 1	
		movpag #psvpage(_mp3_window_s), DSRPAG
	.else
		mov #psvpage(_mp3_window_s), w8
		mov w8, PSVPAG
	.endif

	mov #psvoffset(_mp3_window_s), w8
	add w8, #(6 * 2), w8
	movsac a, [w8] += 2, w6	
	
	do #4, __start_window_loop3
	;{
		nop
		mpy w6 * w7, a, [w8] += 2, w6, [w11] += 2, w7
		sac.r a, #-(MP3_INTBITS), w5
		__start_window_loop3:
		mov w5, [w0++]
	;}

	mpy w6 * w7, a
	sac.r a, #-(MP3_INTBITS), w5
	mov w5, [w0++]

	repeat #5
	clr [w0++]

	.if __dsPIC33E == 1
		pop DSRPAG
	.else
		pop PSVPAG
	.endif
	
	pop w11
	pop w8
	return
;}

;
; stopp block window
;
_stop_window:

	rcall _mp3_layer3_imdct

	push w8
	push w11
	
	.if __dsPIC33E == 1
		push DSRPAG
		movpag #psvpage(_mp3_window_s), DSRPAG
	.else
		push PSVPAG
		mov #psvpage(_mp3_window_s), w0
		mov w0, PSVPAG
	.endif
	
	mov #_mp3_z, w0
	mov #psvoffset(_mp3_window_s), w8
	
	.if __dsPIC33E == 1
		mov #edsoffset(_mp3_y), w11
	.else
		mov #_mp3_y, w11
	.endif

	repeat #5
	clr [w0++]
	
	add w11, #(6 * 2), w11
	movsac a, [w8] += 2, w6, [w11] += 2, w7
	
	do #4, __stop_window_loop1
	;{
		nop
		mpy w6 * w7, a, [w8] += 2, w6, [w11] += 2, w7
		sac.r a, #-(MP3_INTBITS), w5
		__stop_window_loop1:
		mov w5, [w0++]
	;}

	mpy w6 * w7, a, [w11] += 2, w7
	sac.r a, #-(MP3_INTBITS), w5
	mov w5, [w0++]
	
	do #5, __stop_window_loop2
	;{
		mov w7, [w0++]
		__stop_window_loop2:
		movsac a, [w11] += 2, w7
	;}

	.if __dsPIC33E == 1
		movpag #psvpage(_mp3_window_l), DSRPAG
	.else
		mov #psvpage(_mp3_window_l), w8
		mov w8, PSVPAG
	.endif
	
	mov #psvoffset(_mp3_window_l), w8
	add w8, #18, w8
	add w8, #18, w8
	
	movsac a, [w8] += 2, w6

	do #16, __stop_window_loop3
	;{
		nop
		mpy w6 * w7, a, [w8] += 2, w6, [w11] += 2, w7
		sac.r a, #-(MP3_INTBITS), w5
		__stop_window_loop3:
		mov w5, [w0++]
	;}
	
	mpy w6 * w7, a
	sac.r a, #-(MP3_INTBITS), w5
	mov w5, [w0++]

	.if __dsPIC33E == 1
		pop DSRPAG
	.else
		pop PSVPAG
	.endif
	
	pop w11
	pop w8
	return

;}

;
; IMDCT for long blocks
;
; w0 = ch
; w1 = index
;
_mp3_layer3_imdct:
;{
	push w9
	push w11
	;
	; save and set paging registers
	;
	.if __dsPIC33E == 1
		push DSRPAG
		push DSWPAG
		movpag #psvpage(_mp3_imdct_l), DSRPAG
		movpag #edspage(_mp3_y), DSWPAG
	.else
		push PSVPAG
		mov #psvpage(_mp3_imdct_l), w2
		mov w2, PSVPAG
		nop
	.endif
	;
	; &mp3_y -> w0
	; &mp3_z -> w1
	; &mp3_xr[ch][index] -> w2
	;
	.if __dsPIC33E == 1
		mov #edsoffset(_mp3_xr), w2
	.else
		mov #_mp3_xr, w2
	.endif
	
	mov #(576 * 2), w3
	mul.uu, w3, w0, w4
	add w2, w4, w2
	add w1, w1, w1
	add w2, w1, w2
	mov #_mp3_z, w1 

	.if __dsPIC33E == 1
		mov #edsoffset(_mp3_y), w0
	.else
		mov #_mp3_y, w0
	.endif
	
	.equ xr, w2
	.equ y, w0
	.equ z, w1
		
	mov #psvoffset(_mp3_imdct_l), w9

	add xr, #(4 * 2), w11
	movsac a, [w9] += 2, w5, [w11], w7
	add w11, #(9 * 2), w11
	mpy w5 * w7, a, [w9] += 2, w5, [w11], w7
	mac w5 * w7, a, [w9] += 2, w5
	sac.r a, #-(MP3_INTBITS), w7
	mov w7, [z + (6 * 2)]	
	
	add xr, #(1 * 2), w11
	movsac a, [w11], w6
	add w11, #(9 * 2), w11
	movsac a, [w11], w7
	sub w6, w7, w6
	mov w6, [z + (14 * 2)]

	add xr, #(7 * 2), w11
	movsac a, [w11], w6
	add w11, #(9 * 2), w11
	movsac a, [w11], w7
	add w6, w7, w6
	mov w6, [z + (15 * 2)]

	mov [z + (14 * 2)], w7
	mac w5 * w7, a, [w9] += 2, w5
	mov [z + (15 * 2)], w7
	mac w5 * w7, a, [w9] += 2, w5
	
	sac.r a, #-(MP3_INTBITS), w7
	mov w7, [z]

	mov xr, w11
	movsac a, [w11], w6
	add w11, #(11 * 2), w11
	movsac a, [w11] += 2, w7
	sub w6, w7, w6
	movsac a, [w11], w7
	sub w6, w7, w6
	mov w6, [z + (8 * 2)]
			
	add xr, #(2 * 2), w11
	movsac a, [w11], w6
	add w11, #(7 * 2), w11
	movsac a, [w11], w7
	sub w6, w7, w6
	add w11, #(5 * 2), w11
	movsac a, [w11], w7
	sub w6, w7, w6
	mov w6, [z + (9 * 2)]
	
	add xr, #(3 * 2), w11
	movsac a, [w11], w6
	add w11, #(5 * 2), w11
	movsac a, [w11], w7
	sub w6, w7, w6
	add w11, #(7 * 2), w11
	movsac a, [w11], w7
	sub w6, w7, w6
	mov w6, [z + (10 * 2)]
	
	add xr, #(5 * 2), w11
	movsac a, [w11] += 2, w6
	movsac a, [w11], w7
	sub w6, w7, w6
	add w11, #(11 * 2), w11
	movsac a, [w11], w7
	sub w6, w7, w6
	mov w6, [z + (11 * 2)]

	mov [z + (8 * 2)], w7
	mac w5 * w7, a, [w9] += 2, w5
	mov [z + (9 * 2)], w7
	mac w5 * w7, a, [w9] += 2, w5
	mov [z + (10 * 2)], w7
	mac w5 * w7, a, [w9] += 2, w5
	mov [z + (11 * 2)], w7
	mac w5 * w7, a, [w9] += 2, w5
	
	sac.r a, #-(MP3_INTBITS), w7
	mov w7, [y + (7 * 2)]
	neg w7, w7
	mov w7, [y + (10 * 2)]
		
	mov [z + (8 * 2)], w7
	mpy w5 * w7, a, [w9] += 2, w5
	mov [z + (9 * 2)], w7
	mac w5 * w7, a, [w9] += 2, w5
	mov [z + (10 * 2)], w7
	mac w5 * w7, a, [w9] += 2, w5
	mov [z + (11 * 2)], w7
	mac w5 * w7, a, [w9] += 2, w5
	sac.r a, #-(MP3_INTBITS), w7
	mov [z], w4
	sub w7, w4, w7
	mov w7, [y + (19 * 2)]
	mov w7, [y + (34 * 2)]	
	
	mov xr, w11
	movsac a, [w11] += 6, w6
	movsac a, [w11], w7
	sub w6, w7, w6
	add w11, #(5 * 2), w11
	movsac a, [w11] += 6, w7
	add w6, w7, w6
	movsac a, [w11] += 2, w7
	sub w6, w7, w6
	movsac a, [w11] += 6, w7
	sub w6, w7, w6
	movsac a, [w11], w7
	add w6, w7, w6
	mov w6, [z + (12 * 2)]

	add xr, #(2 * 2), w11
	movsac a, [w11] += 6, w6
	movsac a, [w11] += 2, w7
	add w6, w7, w6
	movsac a, [w11] += 6, w7
	sub w6, w7, w6
	movsac a, [w11], w7
	sub w6, w7, w6
	add w11, #(5 * 2), w11
	movsac a, [w11] += 6, w7
	sub w6, w7, w6
	movsac a, [w11], w7
	sub w6, w7, w6
	mov w6, [z + (13 * 2)]

	mov [z + (12 * 2)], w7
	mpy w5 * w7, a, [w9] += 2, w5
	mov [z + (13 * 2)], w7
	mac w5 * w7, a, [w9] += 2, w5	
	sac.r a, #-(MP3_INTBITS), w7
	mov [z], w3
	add w7, w3, w7
	mov w7, [y + (22 * 2)]
	mov w7, [y + (31 * 2)]	

	add xr, #(1 * 2), w11
	movsac a, [w11], w7
	add w11, #(6 * 2), w11
	mpy w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	mac w5 * w7, a, [w9] += 2, w5, [w11], w7
	add w11, #(6 * 2), w11
	mac w5 * w7, a, [w9] += 2, w5, [w11], w7
	mac w5 * w7, a, [w9] += 2, w5
	sac.r a, #-(MP3_INTBITS), w7
	mov [z + (6 * 2)], w3
	add w7, w3, w7
	mov w7, [z + (1 * 2)]
	
	mov xr, w11
	clr a, [w11] += 6, w7
	repeat #5	
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	add xr, #(2 * 2), w11
	movsac a, [w11] += 6, w7
	repeat #5	
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	sac.r a, #-(MP3_INTBITS), w7
	mov [z + (1 * 2)], w3
	add w7, w3, w7
	mov w7, [y + (6 * 2)]
	neg w7, w7
	mov w7, [y + (11 * 2)]
	
	mov xr, w11
	clr a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	add xr, #(2 * 2), w11
	movsac a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	sac.r a, #-(MP3_INTBITS), w7
	mov [z + (1 * 2)], w3
	add w7, w3, w7
	mov w7, [y + (23 * 2)]
	mov w7, [y + (30 * 2)]	
	
	mov xr, w11
	clr a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	add xr, #(2 * 2), w11
	movsac a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	sac.r a, #-(MP3_INTBITS), w7
	mov [z + (1 * 2)], w3
	sub w7, w3, w7
	mov w7, [y + (18 * 2)]
	mov w7, [y + (35 * 2)]
	
	add xr, #(4 * 2), w11
	movsac a, [w11], w7
	add w11, #(9 * 2), w11
	mpy w5 * w7, a, [w9] += 2, w5, [w11], w7
	mac w5 * w7, a, [w9] += 2, w5	
	sac.r a, #-(MP3_INTBITS), w7
	mov w7, [z + (7 * 2)]
		
	add xr, #(1 * 2), w11
	movsac a, [w11], w7
	add w11, #(6 * 2), w11
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	mac w5 * w7, a, [w9] += 2, w5, [w11], w7
	add w11, #(6 * 2), w11
	mac w5 * w7, a, [w9] += 2, w5, [w11], w7
	mac w5 * w7, a, [w9] += 2, w5
	sac.r a, #-(MP3_INTBITS), w7
	mov w7, [z + (2 * 2)]	

	add xr, #(2 * 2), w11
	movsac a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	mov xr, w11
	movsac a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	sac.r a, #-(MP3_INTBITS), w7
	mov w7, [y + (5 * 2)]
	neg w7, w7
	mov w7, [y + (12 * 2)]

	mov xr, w11
	clr a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	add xr, #(2 * 2), w11
	movsac a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	sac.r a, #-(MP3_INTBITS), w7
	mov [z + (2 * 2)], w3
	add w7, w3, w7
	mov w7, [y]
	neg w7, w7
	mov w7, [y + (17 * 2)]
	
	mov xr, w11
	clr a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	add xr, #(2 * 2), w11
	movsac a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	sac.r a, #-(MP3_INTBITS), w7
	mov [z + (2 * 2)], w3
	add w7, w3, w7  
	mov w7, [y + (24 * 2)]
	mov w7, [y + (29 * 2)]
		
	add xr, #(1 * 2), w11
	movsac a, [w11], w7
	add w11, #(6 * 2), w11
	mpy w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	mac w5 * w7, a, [w9] += 2, w5, [w11], w7
	add w11, #(6 * 2), w11
	mac w5 * w7, a, [w9] += 2, w5, [w11], w7
	mac w5 * w7, a, [w9] += 2, w5
	sac.r a, #-(MP3_INTBITS), w7
	mov [z + (7 * 2)], w3
	add w7, w3, w7
	mov w7, [z + (3 * 2)]
	
	mov xr, w11
	clr a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	add xr, #(2 * 2), w11
	movsac a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	sac.r a, #-(MP3_INTBITS), w7
	mov [z + (3 * 2)], w3
	add w7, w3, w7
	mov w7, [y + (8 * 2)]
	neg w7, w7
	mov w7, [y + (9 * 2)]

	mov xr, w11
	clr a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	add xr, #(2 * 2), w11
	movsac a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	sac.r a, #-(MP3_INTBITS), w7
	mov [z + (3 * 2)], w3
	add w7, w3, w7
	mov w7, [y + (21 * 2)]
	mov w7, [y + (32 * 2)]
	
	mov xr, w11
	clr a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	add xr, #(2 * 2), w11
	movsac a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	sac.r a, #-(MP3_INTBITS), w7
	mov [z + (3 * 2)], w3
	sub w7, w3, w7
	mov w7, [y + (20 * 2)]
	mov w7, [y + (33 * 2)]
	
	mov [z + (14 * 2)], w7
	mpy w5 * w7, a, [w9] += 2, w5
	mov [z + (15 * 2)], w7
	mac w5 * w7, a, [w9] += 2, w5
	sac.r a, #-(MP3_INTBITS), w7
	mov [z + (7 * 2)], w3
	sub w7, w3, w7
	mov w7, [z + (4 * 2)]	

	mov [z + (12 * 2)], w7
	mpy w5 * w7, a, [w9] += 2, w5
	mov [z + (13 * 2)], w7
	mac w5 * w7, a, [w9] += 2, w5
	sac.r a, #-(MP3_INTBITS), w7
	mov [z + (4 * 2)], w3
	add w7, w3, w7
	mov w7, [y + (4 * 2)]
	neg w7, w7
	mov w7, [y + (13 * 2)]

	mov [z + (8 * 2)], w7
	mpy w5 * w7, a, [w9] += 2, w5
	mov [z + (9 * 2)], w7
	mac w5 * w7, a, [w9] += 2, w5
	mov [z + (10 * 2)], w7
	mac w5 * w7, a, [w9] += 2, w5
	mov [z + (11 * 2)], w7
	mac w5 * w7, a, [w9] += 2, w5
	sac.r a, #-(MP3_INTBITS), w7
	mov [z + (4 * 2)], w3
	add w7, w3, w7
	mov w7, [y + (1 * 2)]
	neg w7, w7
	mov w7, [y + (16 * 2)]

	mov [z + (8 * 2)], w7
	mpy w5 * w7, a, [w9] += 2, w5
	mov [z + (9 * 2)], w7
	mac w5 * w7, a, [w9] += 2, w5
	mov [z + (10 * 2)], w7
	mac w5 * w7, a, [w9] += 2, w5
	mov [z + (11 * 2)], w7
	mac w5 * w7, a, [w9] += 2, w5
	sac.r a, #-(MP3_INTBITS), w7
	mov [z + (4 * 2)], w3
	add w7, w3, w7
	mov w7, [y + (25 * 2)]
	mov w7, [y + (28 * 2)]

	add xr, #(1 * 2), w11
	clr a, [w11], w7
	add w11, #(6 * 2), w11
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	mac w5 * w7, a, [w9] += 2, w5, [w11], w7
	add w11, #(6 * 2), w11
	mac w5 * w7, a, [w9] += 2, w5, [w11], w7
	mac w5 * w7, a, [w9] += 2, w5
	sac.r a, #-(MP3_INTBITS), w7
	mov [z + (6 * 2)], w3
	sub w7, w3, w7
	mov w7, [z + (5 * 2)]

	mov xr, w11
	clr a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	add xr, #(2 * 2), w11
	movsac a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	sac.r a, #-(MP3_INTBITS), W7
	mov [z + (5 * 2)], w3
	add w7, w3, w7
	mov w7, [y + (2 * 2)]
	neg w7, w7
	mov w7, [y + (15 * 2)]

	mov xr, w11
	clr a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	add xr, #(2 * 2), w11
	movsac a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	sac.r a, #-(MP3_INTBITS), w7
	mov [z + (5 * 2)], w3
	add w7, w3, w7
	mov w7, [y + (3 * 2)]
	neg w7, w7
	mov w7, [y + (14 * 2)]

	mov xr, w11
	clr a, [w11] += 6, w7
	repeat #5
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	add xr, #(2 * 2), w11
	movsac a, [w11] += 6, w7
	repeat #4
	mac w5 * w7, a, [w9] += 2, w5, [w11] += 6, w7
	mac w5 * w7, a
	sac.r a, #-(MP3_INTBITS), w7
	mov [z + (5 * 2)], w3
	add w7, w3, w7
	mov w7, [y + (26 * 2)]
	mov w7, [y + (27 * 2)]
	;
	; restore paging registers
	;
	.if __dsPIC33E == 1
		pop DSWPAG
		pop DSRPAG
	.else
		pop PSVPAG
	.endif
	;
	; restore saved registers
	;
	pop w11
	pop w9	
	return
;}

;
; IMDCT and windowing of short blocks
;
; w0 = ch
; w1 = sb
;
.global _mp3_layer3_imdct_s
_mp3_layer3_imdct_s:
;{
	push w8
	push w9
	push w10
	push w11
	push w12
	
	.if __dsPIC33E == 1
		push DSWPAG
		push DSRPAG
		movpag #edspage(_mp3_y), DSWPAG
		movpag #psvpage(_mp3_imdct_s), DSRPAG
	.else
		push PSVPAG
		mov #psvpage(_mp3_imdct_s), w12
		mov w12, PSVPAG
	.endif

	;xr = &mp3_xr[ch][sb * 18];
	mov #edsoffset(_mp3_xr), w12
	mov #(576 * 2), w4
	mul.uu w0, w4, w4
	add w12, w4, w12
	sl w1, #1, w1
	mul.uu w1, #18, w4
	add w12, w4, w12	
	
	;y = &mp3_y[0];
	.if __dsPIC33E == 1
		mov #edsoffset(_mp3_y), w6
	.else
		mov #_mp3_y, w6
	.endif
	
	do #2, __imdct_s_outer1
	;{
		mov #psvoffset(_mp3_imdct_s), w9
		mov w6, w3
		add w6, #(5 * 2), w8
		add w6, #(11 * 2), w10
		
		do #2, __imdct_s_inner1
		;{
			mov w12, w11
			clr a, [w9] += 2, w5, [w11] += 2, w7
			repeat #5
			mac w5 * w7, a, [w9] += 2, w5, [w11] += 2, w7
			sac.r a, #-(MP3_INTBITS), w7
			mov w7, [w3++]
			neg w7, w7
			mov w7, [w8--]
			
			mov w12, w11
			clr a, [w11] += 2, w7
			repeat #4
			mac w5 * w7, a, [w9] += 2, w5, [w11] += 2, w7
			mac w5 * w7, a, [w11] += 2, w7
			sac.r a, #-(MP3_INTBITS), w5
			mov w5, [w3 + (5 * 2)]
			__imdct_s_inner1:
			mov w5, [w10--]	
		;}
	
		add w6, #(12 * 2), w6
		__imdct_s_outer1:
		add w12, #(6 * 2), w12
	;}

	.if __dsPIC33E == 1
		movpag #psvpage(_mp3_window_s), DSRPAG
		mov #edsoffset(_mp3_y), w0
	.else
		mov #psvpage(_mp3_window_s), w3
		mov w3, PSVPAG
		mov #_mp3_y, w0
	.endif
	
	mov #_mp3_z, w1
	mov #psvoffset(_mp3_window_s), w3
	
	do #5, __imdct_s
	;{
		mov w3, w8
		mov w0, w11
				
		clr [w1]
		movsac a, [w8], w4, [w11], w7
		add w8, #(6 * 2), w8
		add w11, #(6 * 2), w11

		mpy w4 * w7, a, [w8], w5, [w11], w7
		sac.r a, #-(MP3_INTBITS), w2
		mov w2, [w1 + (6 * 2)]
		
		add w11, #(6 * 2), w11
		mpy w5 * w7, a, [w11], w7
		add w11, #(6 * 2), w11
		mac w4 * w7, a, [w11], w7
		sac.r a, #-(MP3_INTBITS), w2
		mov w2, [w1 + (12 * 2)]
		
		add w11, #(6 * 2), w11
		mpy w5 * w7, a, [w11], w7
		add w11, #(6 * 2), w11
		mac w4 * w7, a, [w11], w7
		sac.r a, #-(MP3_INTBITS), w2
		mov w2, [w1 + (18 * 2)]

		mpy w5 * w7, a
		sac.r a, #-(MP3_INTBITS), w2
		mov w2, [w1 + (24 * 2)]
		mov #0, w2
		mov w2, [w1 + (30 * 2)]

		inc2 w0, w0
		inc2 w3, w3
	__imdct_s:
		inc2 w1, w1
	;}

	.if __dsPIC33E == 1
		pop DSRPAG
		pop DSWPAG
	.else
		pop PSVPAG
	.endif
	
	pop w12
	pop w11
	pop w10
	pop w9
	pop w8
	return
;}

;
; frequency inversion
;
; w0 = gr
; w1 = ch
; w2 = sb
;
.global _mp3_layer3_invfreq
_mp3_layer3_invfreq:
;{
	;
	; mp3_out.sb_samples[0][sb] -> w0
	;
	mov _mp3_out, w6
	sl w2, #1, w2
	add w6, w2, w6
	mov #(32 * 2 * 18), w3
	mul.uu w3, w0, w4
	add w6, w4, w6
	mov #(36 * 32 * 2), w3
	mul.uu w3, w1, w4
	add w6, w4, w0
	
	mov #(32 * 2), w1
	add w0, w1, w0
	mov #(32 * 2 * 2), w1
	
	do #8, __invfreq
	;{
		mov [w0], w2
		neg w2, [w0]
		__invfreq:
		add w0, w1, w0
	;}

	return
;}

;
; initialize overlap buffer
;
.global _mp3_layer3_overlap_init
_mp3_layer3_overlap_init:
;{
	;
	; save and set paging registers
	;
	.if __dsPIC33E == 1 && MP3_OVERLAP_IN_EDS == 1
		push DSWPAG
		movpag #edspage(_mp3_overlap), DSWPAG
		mov #edsoffset(_mp3_overlap), w0
	.else
		mov #_mp3_overlap, w0
	.endif
	;
	; initialize overlap buffer to zero
	;
	repeat #((2 * 32 * 18) - 1)
	clr [w0++]		
	;
	; restore paging registers
	;
	.if __dsPIC33E == 1 && MP3_OVERLAP_IN_EDS == 1
		pop DSWPAG
	.endif
	
	return
;}

;
; overlap of non-zero subbands
;
; w0 = gr	
; w1 = ch
; w2 = sb
;
.global _mp3_layer3_overlap
_mp3_layer3_overlap:
;{
	;
	; save and set paging registers
	;
	.if __dsPIC33E == 1 && MP3_OVERLAP_IN_EDS == 1
		push DSRPAG
		push DSWPAG
		movpag #edspage(_mp3_overlap), DSRPAG
		movpag #edspage(_mp3_overlap), DSWPAG
	.endif
	;
	; mp3_overlap[ch][0][0] -> w7
	;
	.if __dsPIC33E == 1 && MP3_OVERLAP_IN_EDS == 1
		mov #edsoffset(_mp3_overlap), w7
	.else
		mov #_mp3_overlap, w7
	.endif
	mov #(32 * 18 * 2), w5
	mul.uu w5, w1, w4
	add w7, w4, w7
	;
	; mp3_overlap[ch][sb][0] -> w7
	;
	mov #(18 * 2), w4
	mul.uu w4, w2, w4
	add w7, w4, w7
	;
	; &mp3_out.sb_samples[0][sb] -> w1
	;
	mov _mp3_out, w6
	sl w2, #1, w2
	add w6, w2, w6
	mov #(32 * 2 * 18), w3
	mul.uu w3, w0, w4
	add w6, w4, w6
	mov #(36 * 32 * 2), w3
	mul.uu w3, w1, w4
	add w6, w4, w1 
	;
	; &mp3_z[0] ->w0
	;
	mov #_mp3_z, w0	
	;
	; for incrementing w1
	;	
	mov #(32 * 2), w3

	do #17, __overlap
	;{
		mov [w7], w4
		add w4, [w0++], w4
		mov w4, [w1]
		add w1, w3, w1
		mov [w0 + (17 * 2)], w4
		__overlap:
		mov w4, [w7++]
	;}

	;
	; restore paging registers
	;
	.if __dsPIC33E == 1 && MP3_OVERLAP_IN_EDS == 1
		pop DSWPAG
		pop DSRPAG
	.endif

	return
;}
	
;
; overlap add of zero subbands
;
; w0 = sample[18][32]
; w1 = ch
; w2 = sb
;
.global _mp3_layer3_overlap_z
_mp3_layer3_overlap_z:
;{
	;
	; save and set paging registers
	;
	.if __dsPIC33E == 1 && MP3_OVERLAP_IN_EDS == 1
		push DSRPAG
		push DSWPAG
		movpag #edspage(_mp3_overlap), DSRPAG
		movpag #edspage(_mp3_overlap), DSWPAG
	.endif
	;
	; mp3_overlap[ch][0][0] -> w7
	;
	.if __dsPIC33E == 1 && MP3_OVERLAP_IN_EDS == 1
		mov #edsoffset(_mp3_overlap), w7
	.else
		mov #_mp3_overlap, w7
	.endif
	mov #(32 * 18 * 2), w3
	mul.uu w3, w1, w4
	add w7, w4, w7
	;
	; mp3_overlap[ch][sb][0] -> w7
	;
	mov #(18 * 2), w4
	mul.uu w4, w2, w4
	add w7, w4, w7
	;
	; mp3_out.sb_samples[0][sb] -> w0
	;
	mov _mp3_out, w6
	sl w2, #1, w2
	add w6, w2, w6
	mov #(32 * 2 * 18), w3
	mul.uu w3, w0, w4
	add w6, w4, w6
	mov #(36 * 32 * 2), w3
	mul.uu w3, w1, w4
	add w6, w4, w0
	;
	; for incrementing w0
	;	
	mov #(32 * 2), w3
	
	do #17, __overlap_z
	;{
		mov [w7], w4
		mov w4, [w0]
		add w0, w3, w0
		__overlap_z:
		clr [w7++]		
	;}
	
	;
	; restore paging registers
	;
	.if __dsPIC33E == 1 && MP3_OVERLAP_IN_EDS
		pop DSWPAG
		pop DSRPAG
	.endif

	return
;}
