sabre_maxTracks:
	.byte 1
sabre_maxSFX:
	.byte 1

trackHeaderTable_lo:
	.byte <_default_New_song_header
trackHeaderTable_hi:
	.byte >_default_New_song_header
trackTable_PRGbank:
	.byte $00

sfxHeaderTable_lo:
	.byte <_sfx_error_header
sfxHeaderTable_hi:
	.byte >_sfx_error_header

;;;; Track index constants
	_default_New_song = 0
;;;; SFX index constants
	_sfx_error = 0

env0:
	.byte 0,ENV_LOOP,0
env1:
	.byte 15,15,15,15,15,15,0,ENV_LOOP,6
env2:
	.byte 10,5,4,3,2,1,0,ENV_LOOP,6
env3:
	.byte 6,4,3,2,1,0,ENV_LOOP,5
env4:
	.byte 6,4,4,4,4,3,3,3,3,2,2,2,2,1,ENV_LOOP,13
env5:
	.byte 5,4,4,4,4,4,4,3,3,3,3,3,3,3,2,2,2,2,2,2,1,1,1,1,1,ENV_LOOP,24
env6:
	.byte 15,15,15,0,ENV_LOOP,3
env7:
	.byte 15,8,4,4,4,4,3,3,3,3,2,2,2,2,1,1,1,1,0,ENV_LOOP,18
env8:
	.byte 6,5,4,3,ENV_LOOP,3
env9:
	.byte 2,ENV_LOOP,0
env10:
	.byte 15,0,ENV_LOOP,1
env11:
	.byte 9,9,9,9,9,8,8,8,8,8,7,7,7,7,7,6,6,6,6,6,5,5,5,5,5,4,4,4,4,4,3,3,3,3,3,2,2,2,2,2,1,1,1,1,1,ENV_LOOP,44
env12:
	.byte 1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,ENV_LOOP,25
env13:
	.byte 6,ENV_LOOP,0
env14:
	.byte 7,4,3,2,1,0,ENV_LOOP,5
env15:
	.byte 9,8,7,6,ENV_LOOP,3
env16:
	.byte 1,6,9,8,7,6,5,ENV_LOOP,6
env17:
	.byte 3,ENV_LOOP,0
env18:
	.byte 15,8,8,8,8,7,7,7,7,7,6,6,6,6,6,5,5,5,5,5,4,4,4,4,4,3,3,3,3,3,2,2,2,2,2,1,ENV_LOOP,35
env19:
	.byte 15,ENV_LOOP,0
env20:
	.byte 1,ENV_LOOP,0
env21:
	.byte 9,15,ENV_LOOP,1
env22:
	.byte 13,14,ENV_LOOP,1
env23:
	.byte 6,10,ENV_LOOP,1
env24:
	.byte 12,7,2,253,248,243,ENV_LOOP,5
env25:
	.byte 15,12,9,ENV_LOOP,2
env26:
	.byte 0,255,254,253,252,251,250,249,248,247,246,245,244,ENV_LOOP,12
env27:
	.byte 0,0,0,0,12,12,12,12,ENV_LOOP,0
env28:
	.byte 12,0,ENV_LOOP,1
env29:
	.byte 6,3,ENV_LOOP,1
env30:
	.byte 5,5,5,5,5,7,7,7,7,7,9,9,9,9,9,11,11,11,11,11,ENV_LOOP,0
env31:
	.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,255,0,255,0,255,0,255,0,1,0,1,0,ENV_LOOP,16
env32:
	.byte 32,ENV_LOOP,0
env33:
	.byte 64,32,0,32,ENV_LOOP,0
env34:
	.byte 64,ENV_LOOP,0
env35:
	.byte 64,64,32,ENV_LOOP,2

instrumentHeaderTable:
	.word inst_silent
	.word inst_Kick_Tri
	.word inst_Kick_Noise
	.word inst_Hat_Closed
	.word inst_Hat_Open
	.word inst_Ride_Rev
	.word inst_Snare_Tri
	.word inst_Snare_Noise
	.word inst_Pluck_I
	.word inst_Pluck_I_Echo
	.word inst_Blank
	.word inst_Tri_Drop
	.word inst_Tri_Pluck
	.word inst_Lead_Arp
	.word inst_Lead_Rev
	.word inst_Lead_Small
	.word inst_Snare_Noise_Small
	.word inst_Bass_Pulse
	.word inst_Lead_Flute
	.word inst_Lead_Flute_Echo
	.word inst_Crash
	.word inst_Pluck_Blank
	.word inst_Noise_Rev
	.word inst_Blank_1

inst_silent:
	.word env0
	.word env0
	.word env0
	.word env0
inst_Kick_Tri:
	.word env1
	.word env24
	.word env0
	.word env0
inst_Kick_Noise:
	.word env2
	.word env21
	.word env0
	.word env0
inst_Hat_Closed:
	.word env3
	.word env22
	.word env0
	.word env0
inst_Hat_Open:
	.word env4
	.word env22
	.word env0
	.word env0
inst_Ride_Rev:
	.word env5
	.word env0
	.word env0
	.word env32
inst_Snare_Tri:
	.word env6
	.word env25
	.word env0
	.word env0
inst_Snare_Noise:
	.word env7
	.word env23
	.word env0
	.word env0
inst_Pluck_I:
	.word env8
	.word env0
	.word env0
	.word env32
inst_Pluck_I_Echo:
	.word env9
	.word env0
	.word env0
	.word env0
inst_Blank:
	.word env19
	.word env0
	.word env0
	.word env0
inst_Tri_Drop:
	.word env0
	.word env26
	.word env0
	.word env0
inst_Tri_Pluck:
	.word env10
	.word env0
	.word env0
	.word env0
inst_Lead_Arp:
	.word env11
	.word env27
	.word env31
	.word env33
inst_Lead_Rev:
	.word env12
	.word env0
	.word env31
	.word env0
inst_Lead_Small:
	.word env13
	.word env0
	.word env0
	.word env34
inst_Snare_Noise_Small:
	.word env14
	.word env29
	.word env0
	.word env0
inst_Bass_Pulse:
	.word env15
	.word env0
	.word env0
	.word env35
inst_Lead_Flute:
	.word env16
	.word env0
	.word env31
	.word env34
inst_Lead_Flute_Echo:
	.word env17
	.word env0
	.word env0
	.word env34
inst_Crash:
	.word env18
	.word env30
	.word env0
	.word env0
inst_Pluck_Blank:
	.word env13
	.word env0
	.word env0
	.word env32
inst_Noise_Rev:
	.word env12
	.word env0
	.word env0
	.word env0
inst_Blank_1:
	.word env20
	.word env0
	.word env0
	.word env34

dpcm_sampleAddressTable:

dpcm_noteToSampleTable:
	.byte $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
	.byte $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
	.byte $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
	.byte $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
dpcm_noteToSampleRateAndFlags:
	.byte $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
	.byte $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
	.byte $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
	.byte $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
dpcm_noteToSampleLength:
	.byte $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
	.byte $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
	.byte $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
	.byte $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff

_sfx_error_header:
	.byte 1
	.byte 150
	.word _sfx_error_pulse1
	.word _sfx_error_pulse2
	.word NULL_triangle
	.word NULL_noise
_sfx_error_pulse1:
	.byte NL4,INST|CONT|9,DS2,NL1,INST|SLNT,NL6,INST|9,NL1,INST|SLNT,NLC
	.byte 50,END_SFX
_sfx_error_pulse2:
	.byte NL4,INST|CONT|9,CS2,NL1,INST|SLNT,NL6,INST|9,NL1,INST|SLNT,NLC
	.byte 50,END_SFX

