;
; bitstream.s
;
; Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
; All rights reserved
;

;typedef struct mp3_stream_t 
;{;
;	unsigned char const *ptr;
;	unsigned int buffer;
;	unsigned int bufflen;
;}
;mp3_stream_t;

;
; w0 = stream pointer
; w1 = bits to read
;
.global _mp3_stream_read
_mp3_stream_read:
;{
	;
	; if there's enough bits in the buffer 
	; return them
	;
	mov [w0 + (1 * 2)], w2 	; stream->buffer => w2
	mov [w0 + (2 * 2)], w3	; stream->bufflen => w3
	cp w1, w3
	bra gtu, _mp3_stream_read_reload
	mov #1, w4
	sl w4, w3, w4
	dec w4, w4
	sub w3, w1, w3
	mov w3, [w0 + (2 * 2)]
	and w2, w4, w0
	lsr w0, w3, w0
	return

	
_mp3_stream_read_reload:
	;
	; read whatever's left on the buffer
	;
	mov [w0], w5
	mov #1, w4
	sl w4, w3, w4
	dec w4, w4
	and w2, w4, w4
	sub w1, w3, w1
	clr w3
	;
	; if we still need more than 8 bits read a
	; byte directly from the stream
	;
	cp w1, #8
	bra ltu, _mp3_stream_read_no_bytes
	sl w4, #8, w4
	ior.b w4, [w5++], w4
	sub w1, #8, w1 
	
_mp3_stream_read_no_bytes:
	;
	; if we still need any bits reload the buffer
	; with two bytes and read the bits
	;
	cp0 w1
	bra z, _mp3_stream_read_no_bits
	ze [w5++], w6
	mov.b w6, [w0 + (1 * 2) + 1]
	ze [w5++], w6
	mov.b w6, [w0 + (1 * 2)]
	mov [w0 + (1 * 2)], w2
	sl w4, w1, w4
	mov #16, w6
	sub w6, w1, w3
	lsr w2, w3, w6
	ior w4, w6, w4
	
_mp3_stream_read_no_bits:
	;
	; update the stream object and return
	; the bits read
	;
	mov w5, [w0]
	mov w3, [w0 + (2 * 2)]
	mov w4, w0
	return
;}

.global _mp3_stream_read_bool
_mp3_stream_read_bool:
;{
	;
	; if there's at least one bit on the stram 
	; return it
	;
	mov [w0 + (1 * 2)], w1
	mov [w0 + (2 * 2)], w2
	cp0 w2
	bra z, _mp3_stream_read_bool_reload
	dec w2, w2
	mov w2, [w0 + (2 * 2)]
	mov #1, w0
	sl w0, w2, w0
	and w1, w0, w0
	return
		
_mp3_stream_read_bool_reload:

	mov [w0], w3
	ze [w3++], w4
	mov.b w4, [w0 + (1 * 2) + 1]
	ze [w3++], w4
	mov.b w4, [w0 + (1 * 2)]
	mov w3, [w0]
	mov [w0 + (1 * 2)], w1
	mov #(16 - 1), w3
	mov w3, [w0 + (2 * 2)]
	lsr w1, #15, w0	
	return	
;}

