
.importzp NTSC_MODE

.include "sabre_includes.inc"

.segment "_pzeropage" : zeropage
.include "sabre_ZP_RAM.inc"

.segment "_pbss_psabre"
.include "sabre_Misc_RAM.inc"

.global currentSFX

.global sabre_playSFX

.segment "_pinit_psabre"
init_sabre:
    clc
    lda NTSC_MODE
    rol
    bcs :+
        lda #1
    :
    sta soundRegion
	jsr sabre_initAPU

    lda #0
    sta currentTrack
	jsr sabre_playTrack

.segment "_pnmi_p200"
run_sabre_driver:
    jsr sabre_soundUpdate

.segment "_ptext_psabre__music"
.include "bpe_final_static.inc"
.include "bpe_final_default.inc"
.segment "_ptext_psabre__driver"
.include "sabre.inc"

.segment "_ptext_pdelaycode"

.global delay_256a_x_33_clocks, delay_a_27_clocks

;;;;;;;;;;;;;;;;;;;;;;;;
; Delays A:X clocks+overhead
; Time: 256*A+X+33 clocks (including JSR)
; Clobbers A. Preserves X,Y. Has relocations.
;;;;;;;;;;;;;;;;;;;;;;;;
:	; 5 cycles done, do 256-5 more.
	sbc #1			; 2 cycles - Carry was set from cmp
	pha                     ; 3
	    ldy #49  ; 2
@l:     dey      ; 49*2
        bne @l   ; 49*3
        ldy $A4  ; 3-1
	pla                     ; 4
delay_256a_x_33_clocks:
	cmp #1			; +2
	bcs :-			; +3 
	; 0-255 cycles remain, overhead = 4
	txa 			; -1+2; 6; +27 = 33
	;passthru
delay_a_27_clocks:
        sec     
@L:     sbc #5  
        bcs @L  ;  6 6 6 6 6  FB FC FD FE FF
        adc #3  ;  2 2 2 2 2  FE FF 00 01 02
        bcc @4  ;  3 3 2 2 2  FE FF 00 01 02
        lsr     ;  - - 2 2 2  -- -- 00 00 01
        beq @5  ;  - - 3 3 2  -- -- 00 00 01
@4:     lsr     ;  2 2 - - 2  7F 7F -- -- 00
@5:     bcs @6  ;  2 3 2 3 2  7F 7F 00 00 00
@6:     rts     ;