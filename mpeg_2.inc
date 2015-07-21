;
; mpeg_2.inc
;
; Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
; All rights reserved
;



;
; mpeg_2.inc
;
.ifndef MPEG_2_INC
.equ MPEG_2_INC, 1

;
; number of integer bits in fractional type
;
.equ MP3_INTBITS, 2

;
; scale bits
;
.equ MP3_SCALEBITS, 1

;
; fractional constants
;
.equ MP3_FRAC_MIN, -0x8000
.equ MP3_FRAC_ONE, +0x2000	;0x1000 = 3.13 | 0x2000 4.12
.equ MP3_FRAC_MAX, +0x7fff

;
; define to 1 if overlap buffer is in EDS memory
; zero otherwise
;
.equ MP3_OVERLAP_IN_EDS, 1

;
; Define to 1 to have the library manage the
; DSWPAG when writing to EDS memory.
;
; If there's no external memory mapped in the
; EDS space then it is safe to assume that DSWPAG
; will always be set to 1 and there's no need
; to manage this register.
;
.equ MP3_MANAGE_DSWPAG, 0

;
; make sure __dsPIC33E is defined to 1 if this
; is a dsPIC33E part and to zero otherwise
;
.ifndef __dsPIC33E
.equ __dsPIC33E, 0
.endif

.endif
