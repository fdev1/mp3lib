;
; compiler.s
;
; Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
; All rights reserved
;

.global _memcpy_fast
_memcpy_fast:

	cp0 w2
	bra z, _memcpy_return

_memcpy_fast2:
	btst w0, #0
	bra nz, _memcpy_dst_unaligned
	btst w1, #0
	bra nz, _memcpy_unaligned
	goto _memcpy_aligned
	
_memcpy_dst_unaligned:

	btst w1, #0
	bra z, _memcpy_unaligned
	mov.b [w1++], w3
	mov.b w3, [w0++]
	dec w2, w2
	cp0 w2
	bra z, _memcpy_return
			
_memcpy_aligned:

	cp w2, #1
	bra gtu, _memcpy_aligned2
	mov.b [w1++], w3
	mov.b w3, [w0++]
	return

_memcpy_aligned2:

	mov #~1, w3
	and w2, w3, w3
	lsr w3, #1, w3
	dec w3, w3
	
	do w3, _memcpy_fast_loop
	;nop  ;<-- this is needed if the source is in auto_psv
	mov [w1++], w3
	_memcpy_fast_loop:
	mov w3, [w0++]

	btss w2, #0
	return
	
	mov.b [w1], w3
	mov.b w3, [w0]
	
_memcpy_return:
	return

_memcpy_unaligned:
	dec w2, w3
	do w3, _memcpy_fast_loop_u
	;nop  ;<-- this is needed if the source is in auto_psv
	mov.b [w1++], w3
	_memcpy_fast_loop_u:
	mov.b w3, [w0++]
	return

.global _memmove_fast
_memmove_fast:

	cp0 w2
	bra z, _memmove_return

	cp w0, w1
	bra ltu, _memcpy_fast2
	
	add w0, w2, w0
	add w1, w2, w1
	
	btst w0, #0
	bra nz, _memmove_dst_unaligned
	btst w1, #0
	bra nz, _memmove_unaligned
	goto _memmove_aligned
	
_memmove_dst_unaligned:

	btst w1, #0
	bra z, _memmove_unaligned
	mov.b [--w1], w3
	mov.b w3, [--w0]
	dec w2, w2
	cp0 w2
	bra z, _memmove_return

_memmove_aligned:

	cp w2, #1
	bra gtu, _memmove_aligned2
	mov.b [--w1], w3
	mov.b w3, [--w0]
	return
	
_memmove_aligned2:
	
	mov #~1, w3
	and w2, w3, w3
	lsr w3, #1, w3
	dec w3, w3
	
	do w3, _memmove_fast_loop
	;nop  ;<-- this is needed if the source is in auto_psv
	mov [--w1], w3
	_memmove_fast_loop:
	mov w3, [--w0]

	btss w2, #0
	return
	
	mov.b [w1 + 1], w3
	mov.b w3, [w0 + 1]
_memmove_return:
	return

_memmove_unaligned:
	dec w2, w3
	do w3, _memmove_fast_loop_u
	;nop  ;<-- this is needed if the source is in auto_psv
	mov.b [--w1], w3
	_memmove_fast_loop_u:
	mov.b w3, [--w0]
	return
