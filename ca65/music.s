
.importzp NTSC_MODE

.include "sabre_includes.inc"

.segment "_pzeropage" : zeropage
.include "sabre_ZP_RAM.inc"

.segment "_pbss_psabre"
.include "sabre_Misc_RAM.inc"

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

    ; lda #0
    ; sta currentTrack
	; jsr sabre_playTrack

.segment "_pnmi_p200"
run_sabre_driver:
    jsr sabre_soundUpdate

.segment "_ptext_psabre__music"
.include "bpe_final_static.inc"
.include "bpe_final_default.inc"
.segment "_ptext_psabre__driver"
.include "sabre.inc"
