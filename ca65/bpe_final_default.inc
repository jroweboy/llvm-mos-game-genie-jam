_default_New_song_header:
	.byte 5
	.byte 150
	.byte 32
	.word _default_New_song_pulse1
	.word _default_New_song_pulse2
	.word _default_New_song_triangle
	.word _default_New_song_noise
	.word NULL_DMC

_default_New_song_pulse1:
	.word _default_New_song_pulse1_pattern0
	.word _default_New_song_pulse1_pattern0
	.word _default_New_song_pulse1_pattern0
	.word _default_New_song_pulse1_pattern0
	.word _default_New_song_pulse1_pattern0
	.word _default_New_song_pulse1_pattern0
	.word _default_New_song_pulse1_pattern0
	.word _default_New_song_pulse1_pattern0
	.word _default_New_song_pulse1_pattern0
	.word _default_New_song_pulse1_pattern0
	.word _default_New_song_pulse1_pattern0
	.word _default_New_song_pulse1_pattern0
	.word _default_New_song_pulse1_pattern1
	.word _default_New_song_pulse1_pattern2
	.word _default_New_song_pulse1_pattern3
	.word _default_New_song_pulse1_pattern4
	.word _default_New_song_pulse1_pattern5
	.word _default_New_song_pulse1_pattern6
	.word _default_New_song_pulse1_pattern7
	.word _default_New_song_pulse1_pattern8
	.word _default_New_song_pulse1_pattern1
	.word _default_New_song_pulse1_pattern2
	.word _default_New_song_pulse1_pattern3
	.word _default_New_song_pulse1_pattern4
	.word _default_New_song_pulse1_pattern5
	.word _default_New_song_pulse1_pattern6
	.word _default_New_song_pulse1_pattern7
	.word _default_New_song_pulse1_pattern13
	.word _default_New_song_pulse1_pattern9
	.word _default_New_song_pulse1_pattern10
	.word _default_New_song_pulse1_pattern11
	.word _default_New_song_pulse1_pattern12
_default_New_song_pulse1_pattern0:
	.byte NL48,INST|SLNT,D00
_default_New_song_pulse1_pattern1:
	.byte NL12,INST|CONT|13,A3,NL6,INST|14,INST|CONT|13,CS4,B3,INST|14,NL4
	.byte INST|CONT|13,DS4,E4,DS4,D00
_default_New_song_pulse1_pattern2:
	.byte NL12,INST|CONT|13,CS4,INST|CONT|14,GS4,NL1,INST|CONT|15,B3,A3,GS3
	.byte E3,GS3,A3,B3,INST|SLNT,INST|CONT|15,E4,INST|SLNT,INST|CONT|15,FS4
	.byte INST|SLNT,INST|CONT|15,E4,INST|SLNT,INST|CONT|15,B4,INST|SLNT,INST|CONT|15,E4,INST|SLNT
	.byte INST|CONT|15,DS4,INST|SLNT,INST|CONT|15,B3,INST|SLNT,INST|CONT|15,A3,INST|SLNT,D00
_default_New_song_pulse1_pattern3:
	.byte NL12,INST|CONT|13,A3,NL6,INST|14,INST|CONT|13,CS4,B3,INST|14,NL4
	.byte INST|CONT|13,DS4,E4,FS4,D00
_default_New_song_pulse1_pattern4:
	.byte NL2,INST|CONT|13,E4,DS4,NL8,E4,NL12,INST|CONT|14,B4,NL1
	.byte INST|CONT|15,FS3,INST|SLNT,INST|CONT|15,B3,INST|SLNT,INST|CONT|15,CS4,INST|SLNT,INST|CONT|15
	.byte B3,INST|SLNT,INST|CONT|15,A4,INST|SLNT,INST|CONT|15,GS4,INST|SLNT,INST|15,A4
	.byte B4,INST|SLNT,INST|CONT|15,CS5,INST|SLNT,INST|CONT|15,B4,INST|SLNT,INST|CONT|15,E4
	.byte INST|SLNT,INST|CONT|15,B3,INST|SLNT,D00
_default_New_song_pulse1_pattern5:
	.byte NL12,INST|CONT|13,C4,NL6,INST|14,INST|CONT|13,E4,D4,INST|14,NL4
	.byte INST|CONT|13,FS4,G4,FS4,D00
_default_New_song_pulse1_pattern6:
	.byte NL12,INST|CONT|13,E4,INST|CONT|14,B4,NL1,INST|CONT|15,G4,INST|SLNT,INST|CONT|15
	.byte FS4,INST|SLNT,INST|CONT|15,G4,INST|SLNT,INST|CONT|15,D5,INST|SLNT,INST|CONT|15,G5
	.byte INST|SLNT,INST|CONT|15,FS5,INST|SLNT,INST|CONT|15,G5,INST|SLNT,INST|CONT|15,B5,A5
	.byte G5,D5,C5,INST|SLNT,INST|CONT|15,B4,INST|SLNT,INST|CONT|15,G4,INST|SLNT
	.byte D00
_default_New_song_pulse1_pattern7:
	.byte NL12,INST|CONT|13,C4,NL6,INST|14,INST|CONT|13,E4,D4,INST|14,NL4
	.byte INST|CONT|13,FS4,G4,A4,D00
_default_New_song_pulse1_pattern8:
	.byte NL12,INST|CONT|13,G4,NL8,INST|14,NL2,INST|CONT|13,A4,G4,NL6
	.byte A4,INST|14,NL1,INST|CONT|15,CS5,D5,CS5,INST|SLNT,INST|CONT|15,A4
	.byte INST|SLNT,INST|CONT|15,E4,INST|SLNT,INST|CONT|15,D4,INST|SLNT,INST|CONT|15,A3,INST|SLNT
	.byte D00
_default_New_song_pulse1_pattern9:
	.byte NL8,INST|CONT|18,E3,NL2,B3,E3,NL6,DS3,INST|19,NL2
	.byte INST|CONT|18,B2,DS3,E3,NL1,A3,B3,NL2,A3,FS3
	.byte NL12,GS3,D00
_default_New_song_pulse1_pattern10:
	.byte NL2,INST|CONT|18,A3,GS3,A3,INST|CONT|19,GS3,INST|18,E3,NL4
	.byte DS3,E3,NL2,FS3,INST|CONT|19,E3,INST|CONT|18,GS3,G3,NL8
	.byte GS3,NL6,INST|19,INST|CONT|14,E4,D00
_default_New_song_pulse1_pattern11:
	.byte NL4,INST|CONT|18,G3,D4,NL2,G3,INST|CONT|19,D4,NL6,INST|CONT|18
	.byte FS3,INST|19,NL2,INST|CONT|18,B3,NL1,FS4,G4,NL2,FS4
	.byte E4,D4,C4,NL6,B3,INST|19,D00
_default_New_song_pulse1_pattern12:
	.byte NL6,INST|CONT|18,E4,NL2,INST|19,INST|CONT|18,FS4,G4,NL4,FS4
	.byte A4,NL2,D5,INST|CONT|19,A4,NL12,INST|CONT|18,E5,NL1,INST|CONT|21
	.byte D3,E3,FS3,A3,D4,E4,FS4,A4,D5,E5
	.byte FS5,A5,BXX,$0C
_default_New_song_pulse1_pattern13:
	.byte NL12,INST|CONT|13,G4,NL8,INST|14,NL2,INST|CONT|13,A4,G4,NL6
	.byte A4,INST|14,NL1,INST|CONT|15,CS5,D5,CS5,INST|SLNT,INST|CONT|15,A4
	.byte INST|SLNT,NL2,INST|CONT|18,B2,B3,DS3,D00

_default_New_song_pulse2:
	.word _default_New_song_pulse2_pattern0
	.word _default_New_song_pulse2_pattern0
	.word _default_New_song_pulse2_pattern0
	.word _default_New_song_pulse2_pattern0
	.word _default_New_song_pulse2_pattern1
	.word _default_New_song_pulse2_pattern1
	.word _default_New_song_pulse2_pattern1
	.word _default_New_song_pulse2_pattern1
	.word _default_New_song_pulse2_pattern2
	.word _default_New_song_pulse2_pattern2
	.word _default_New_song_pulse2_pattern2
	.word _default_New_song_pulse2_pattern3
	.word _default_New_song_pulse2_pattern1
	.word _default_New_song_pulse2_pattern1
	.word _default_New_song_pulse2_pattern1
	.word _default_New_song_pulse2_pattern1
	.word _default_New_song_pulse2_pattern2
	.word _default_New_song_pulse2_pattern2
	.word _default_New_song_pulse2_pattern2
	.word _default_New_song_pulse2_pattern3
	.word _default_New_song_pulse2_pattern1
	.word _default_New_song_pulse2_pattern1
	.word _default_New_song_pulse2_pattern1
	.word _default_New_song_pulse2_pattern1
	.word _default_New_song_pulse2_pattern2
	.word _default_New_song_pulse2_pattern2
	.word _default_New_song_pulse2_pattern2
	.word _default_New_song_pulse2_pattern3
	.word _default_New_song_pulse2_pattern4
	.word _default_New_song_pulse2_pattern5
	.word _default_New_song_pulse2_pattern6
	.word _default_New_song_pulse2_pattern7
_default_New_song_pulse2_pattern0:
	.byte NL48,INST|SLNT,D00
_default_New_song_pulse2_pattern1:
	.byte NL1,INST|CONT|8,E3,INST|9,INST|CONT|8,B2,INST|CONT|9,E3,INST|CONT|8,A2
	.byte INST|CONT|9,B2,INST|CONT|8,E4,INST|CONT|9,A2,INST|CONT|8,B3,INST|CONT|9,E4
	.byte INST|CONT|8,A3,INST|CONT|9,B3,INST|CONT|8,E3,INST|CONT|9,A3,INST|CONT|8,B2
	.byte INST|CONT|9,E3,INST|CONT|8,A2,INST|CONT|9,B2,INST|CONT|8,E4,INST|CONT|9,A2
	.byte INST|CONT|8,B3,INST|CONT|9,E4,INST|CONT|8,A3,INST|CONT|9,B3,INST|CONT|8,FS3
	.byte INST|CONT|9,A3,INST|CONT|8,CS3,INST|CONT|9,FS3,INST|CONT|8,B2,INST|CONT|9,CS3
	.byte INST|CONT|8,FS4,INST|CONT|9,B2,INST|CONT|8,CS4,INST|CONT|9,FS4,INST|CONT|8,B3
	.byte INST|CONT|9,CS4,INST|CONT|8,FS3,INST|CONT|9,B3,INST|CONT|8,CS3,INST|CONT|9,FS3
	.byte INST|CONT|8,B2,INST|CONT|9,CS3,INST|CONT|8,FS4,INST|CONT|9,B2,INST|CONT|8,CS4
	.byte INST|CONT|9,FS4,INST|CONT|8,B3,INST|CONT|9,CS4,D00
_default_New_song_pulse2_pattern2:
	.byte NL1,INST|CONT|8,G3,INST|9,INST|CONT|8,D3,INST|CONT|9,G3,INST|CONT|8,C3
	.byte INST|CONT|9,D3,INST|CONT|8,G4,INST|CONT|9,C3,INST|CONT|8,D4,INST|CONT|9,G4
	.byte INST|CONT|8,C4,INST|CONT|9,D4,INST|CONT|8,G3,INST|CONT|9,C4,INST|CONT|8,D3
	.byte INST|CONT|9,G3,INST|CONT|8,C3,INST|CONT|9,D3,INST|CONT|8,G4,INST|CONT|9,C3
	.byte INST|CONT|8,D4,INST|CONT|9,G4,INST|CONT|8,C4,INST|CONT|9,D4,INST|CONT|8,A3
	.byte INST|CONT|9,C4,INST|CONT|8,E3,INST|CONT|9,A3,INST|CONT|8,D3,INST|CONT|9,E3
	.byte INST|CONT|8,A4,INST|CONT|9,D3,INST|CONT|8,E4,INST|CONT|9,A4,INST|CONT|8,D4
	.byte INST|CONT|9,E4,INST|CONT|8,A3,INST|CONT|9,D4,INST|CONT|8,E3,INST|CONT|9,A3
	.byte INST|CONT|8,D3,INST|CONT|9,E3,INST|CONT|8,A4,INST|CONT|9,D3,INST|CONT|8,E4
	.byte INST|CONT|9,A4,INST|CONT|8,D4,INST|CONT|9,E4,D00
_default_New_song_pulse2_pattern3:
	.byte NL1,INST|CONT|8,AS3,INST|9,INST|CONT|8,F3,INST|CONT|9,AS3,INST|CONT|8,DS3
	.byte INST|CONT|9,F3,INST|CONT|8,AS4,INST|CONT|9,DS3,INST|CONT|8,F4,INST|CONT|9,AS4
	.byte INST|CONT|8,DS4,INST|CONT|9,F4,INST|CONT|8,AS3,INST|CONT|9,DS4,INST|CONT|8,F3
	.byte INST|CONT|9,AS3,INST|CONT|8,DS3,INST|CONT|9,F3,INST|CONT|8,AS4,INST|CONT|9,DS3
	.byte INST|CONT|8,F4,INST|CONT|9,AS4,INST|CONT|8,DS4,INST|CONT|9,F4,INST|CONT|8,A3
	.byte INST|CONT|9,DS4,INST|CONT|8,E3,INST|CONT|9,A3,INST|CONT|8,D3,INST|CONT|9,E3
	.byte INST|CONT|8,A4,INST|CONT|9,D3,INST|CONT|8,E4,INST|CONT|9,A4,INST|CONT|8,D4
	.byte INST|CONT|9,E4,INST|CONT|8,A3,INST|CONT|9,D4,INST|CONT|8,E3,INST|CONT|9,A3
	.byte INST|CONT|8,D3,INST|CONT|9,E3,INST|CONT|8,A4,INST|CONT|9,D3,INST|CONT|8,E4
	.byte INST|CONT|9,A4,INST|CONT|8,D4,INST|CONT|9,E4,D00
_default_New_song_pulse2_pattern4:
	.byte NL2,INST|CONT|8,E3,B2,A2,INST|CONT|17,E2,B1,E1,NL8
	.byte A0,NL2,E1,A1,NL4,GS1,NL2,DS1,GS1,FS1
	.byte GS1,NL6,CS1,NL4,GS1,NL2,F1,D00
_default_New_song_pulse2_pattern5:
	.byte NL6,INST|CONT|17,FS1,NL2,CS2,FS1,CS1,NL6,B0,DS1
	.byte E1,NL2,E2,E1,B1,NL6,E1,D1,D00
_default_New_song_pulse2_pattern6:
	.byte NL4,INST|CONT|17,C1,NL2,G1,C2,B1,C2,NL6,D1
	.byte NL2,A1,B1,D1,NL4,E1,NL2,E1,E2,E1
	.byte FS1,NL6,G1,NL2,D2,G1,B0,D00
_default_New_song_pulse2_pattern7:
	.byte NL4,INST|CONT|17,C1,NL2,C2,C1,G1,C1,NL4,B0
	.byte NL2,B1,B0,FS1,B0,NL4,E1,NL2,E2,E1
	.byte E2,B1,E1,B1,E2,INST|CONT|8,E3,B3,E4,D00

_default_New_song_triangle:
	.word _default_New_song_triangle_pattern0
	.word _default_New_song_triangle_pattern0
	.word _default_New_song_triangle_pattern1
	.word _default_New_song_triangle_pattern1
	.word _default_New_song_triangle_pattern1
	.word _default_New_song_triangle_pattern1
	.word _default_New_song_triangle_pattern1
	.word _default_New_song_triangle_pattern1
	.word _default_New_song_triangle_pattern1
	.word _default_New_song_triangle_pattern1
	.word _default_New_song_triangle_pattern1
	.word _default_New_song_triangle_pattern2
	.word _default_New_song_triangle_pattern3
	.word _default_New_song_triangle_pattern3
	.word _default_New_song_triangle_pattern3
	.word _default_New_song_triangle_pattern3
	.word _default_New_song_triangle_pattern3
	.word _default_New_song_triangle_pattern3
	.word _default_New_song_triangle_pattern3
	.word _default_New_song_triangle_pattern4
	.word _default_New_song_triangle_pattern5
	.word _default_New_song_triangle_pattern5
	.word _default_New_song_triangle_pattern5
	.word _default_New_song_triangle_pattern7
	.word _default_New_song_triangle_pattern5
	.word _default_New_song_triangle_pattern5
	.word _default_New_song_triangle_pattern5
	.word _default_New_song_triangle_pattern8
	.word _default_New_song_triangle_pattern9
	.word _default_New_song_triangle_pattern10
	.word _default_New_song_triangle_pattern11
	.word _default_New_song_triangle_pattern12
_default_New_song_triangle_pattern0:
	.byte NL24,INST|CONT|1,C3,NL18,C3,NL6,INST|6,D00
_default_New_song_triangle_pattern1:
	.byte NL1,INST|CONT|1,C3,INST|CONT|10,A1,NL2,INST|SLNT,NL1,INST|10,INST|SLNT
	.byte NL4,INST|12,INST|CONT|10,A2,NL2,INST|11,INST|CONT|10,A1,E2,B2
	.byte A2,NL1,INST|CONT|1,C3,INST|CONT|10,A1,NL2,INST|SLNT,NL1,INST|10
	.byte INST|SLNT,NL4,INST|12,INST|CONT|10,A2,NL2,INST|11,INST|CONT|10,A1,NL1
	.byte INST|CONT|6,C3,INST|CONT|10,E2,NL2,FS3,E3,D00
_default_New_song_triangle_pattern2:
	.byte NL1,INST|CONT|1,C3,INST|CONT|10,C2,NL2,INST|SLNT,NL1,INST|10,INST|SLNT
	.byte NL4,INST|12,INST|CONT|10,C3,NL2,INST|11,INST|CONT|10,C2,G2,D2
	.byte DS2,NL1,INST|CONT|1,C3,INST|CONT|10,E2,NL2,INST|SLNT,NL1,INST|10
	.byte INST|SLNT,NL4,INST|12,INST|CONT|10,E3,NL2,INST|11,INST|CONT|10,E2,NL1
	.byte INST|CONT|6,C3,INST|CONT|10,E3,NL2,D3,B2,D00
_default_New_song_triangle_pattern3:
	.byte NL1,INST|CONT|1,C3,INST|CONT|10,A1,NL2,INST|SLNT,NL1,INST|10,INST|SLNT
	.byte NL4,INST|12,NL1,INST|CONT|1,C3,NL3,INST|CONT|10,A2,NL2,INST|11
	.byte INST|CONT|10,A1,E2,B2,A2,NL1,INST|CONT|1,C3,INST|CONT|10,A1
	.byte NL2,INST|SLNT,NL1,INST|10,INST|SLNT,NL4,INST|12,NL1,INST|CONT|1,C3
	.byte NL3,INST|CONT|10,A2,NL2,INST|11,INST|CONT|10,A1,NL1,INST|CONT|6,C3
	.byte INST|CONT|10,E2,NL2,FS3,E3,D00
_default_New_song_triangle_pattern4:
	.byte NL1,INST|CONT|1,C3,INST|CONT|10,C2,NL2,INST|SLNT,NL1,INST|10,INST|SLNT
	.byte NL4,INST|12,NL1,INST|CONT|1,C3,NL3,INST|10,NL2,INST|11,INST|CONT|10
	.byte C2,G2,D2,DS2,NL1,INST|CONT|1,C3,INST|CONT|10,E2,NL2
	.byte INST|SLNT,NL1,INST|10,INST|SLNT,NL4,INST|12,NL1,INST|CONT|1,C3,NL3
	.byte INST|CONT|10,E3,NL2,INST|11,INST|CONT|10,E2,NL1,INST|CONT|6,C3,INST|CONT|10
	.byte E3,INST|CONT|6,C3,INST|CONT|10,D3,NL2,B2,D00
_default_New_song_triangle_pattern5:
	.byte NL1,INST|CONT|1,C3,INST|CONT|10,A1,NL2,INST|SLNT,NL1,INST|10,INST|SLNT
	.byte NL2,INST|CONT|6,C3,INST|CONT|12,A1,NL1,INST|CONT|1,C3,NL3,INST|CONT|10
	.byte A2,NL2,INST|11,INST|CONT|10,A1,NL1,INST|CONT|6,C3,INST|CONT|10,E2
	.byte NL2,B2,A2,NL1,INST|CONT|1,C3,INST|CONT|10,A1,NL2,INST|SLNT
	.byte NL1,INST|10,INST|SLNT,NL2,INST|CONT|6,C3,INST|CONT|12,A1,NL1,INST|CONT|1
	.byte C3,NL3,INST|CONT|10,A2,NL2,INST|11,INST|CONT|10,A1,NL1,INST|CONT|6
	.byte C3,INST|CONT|10,E2,NL2,FS3,E3,D00
_default_New_song_triangle_pattern7:
	.byte NL1,INST|CONT|1,C3,INST|CONT|10,A1,NL2,INST|SLNT,NL1,INST|10,INST|SLNT
	.byte NL2,INST|CONT|6,C3,INST|CONT|12,A1,NL1,INST|CONT|1,C3,NL3,INST|CONT|10
	.byte A2,NL2,INST|11,INST|CONT|10,A1,NL1,INST|CONT|6,C3,INST|CONT|10,E2
	.byte NL2,B2,A2,NL1,INST|CONT|1,C3,INST|CONT|10,A1,NL2,INST|SLNT
	.byte NL1,INST|10,INST|SLNT,NL2,INST|CONT|6,C3,INST|CONT|12,A1,NL1,INST|CONT|1
	.byte C3,NL3,INST|CONT|10,A2,NL2,INST|11,INST|CONT|10,A1,NL1,INST|CONT|6
	.byte C3,INST|CONT|10,E2,INST|CONT|6,C3,INST|CONT|10,FS3,NL2,E3,D00
_default_New_song_triangle_pattern8:
	.byte NL1,INST|CONT|1,C3,INST|CONT|10,C2,NL2,INST|SLNT,NL1,INST|10,INST|SLNT
	.byte NL2,INST|CONT|6,C3,INST|CONT|12,C2,NL1,INST|CONT|1,C3,NL3,INST|10
	.byte NL2,INST|11,INST|CONT|10,C2,NL1,INST|CONT|6,C3,INST|CONT|10,G2,NL2
	.byte D2,DS2,NL1,INST|CONT|1,C3,INST|CONT|10,E2,NL2,INST|SLNT,NL1
	.byte INST|10,INST|SLNT,NL2,INST|CONT|6,C3,INST|CONT|12,E2,NL1,INST|CONT|1,C3
	.byte NL3,INST|CONT|10,E3,NL1,INST|CONT|6,C3,INST|11,INST|1,INST|CONT|10,E2
	.byte INST|CONT|6,C3,INST|CONT|10,E3,INST|CONT|6,C3,INST|CONT|10,D3,NL2,B2
	.byte D00
_default_New_song_triangle_pattern9:
	.byte NL1,INST|CONT|1,C3,NL3,INST|CONT|10,A1,NL2,E2,A2,E3
	.byte A3,NL1,A3,NL5,B3,NL6,FS3,GS3,NL1,CS4
	.byte NL5,DS4,NL6,FS4,F4,D00
_default_New_song_triangle_pattern10:
	.byte NL6,INST|CONT|10,E4,CS4,B3,NL2,FS3,GS3,A3,B3
	.byte AS3,NL8,B3,NL2,E2,B2,E3,A3,GS3,E3
	.byte D00
_default_New_song_triangle_pattern11:
	.byte NL1,INST|CONT|1,C3,INST|CONT|10,C2,NL2,C3,G3,E4,FS4
	.byte G4,NL1,FS4,G4,NL2,FS4,NL4,E4,D4,NL6
	.byte B3,E4,F4,G3,D00
_default_New_song_triangle_pattern12:
	.byte NL2,INST|CONT|17,E3,G3,E3,G3,B3,E4,NL4,D4
	.byte B3,A3,NL12,GS3,NL1,INST|CONT|1,C3,INST|CONT|17,A3,INST|CONT|6
	.byte C3,INST|CONT|17,GS3,INST|CONT|1,C3,INST|CONT|17,E3,INST|CONT|6,C3,INST|CONT|17
	.byte B2,INST|CONT|6,C3,INST|CONT|17,E2,NL2,E1,D00

_default_New_song_noise:
	.word _default_New_song_noise_pattern0
	.word _default_New_song_noise_pattern0
	.word _default_New_song_noise_pattern0
	.word _default_New_song_noise_pattern0
	.word _default_New_song_noise_pattern0
	.word _default_New_song_noise_pattern0
	.word _default_New_song_noise_pattern0
	.word _default_New_song_noise_pattern0
	.word _default_New_song_noise_pattern0
	.word _default_New_song_noise_pattern0
	.word _default_New_song_noise_pattern0
	.word _default_New_song_noise_pattern0
	.word _default_New_song_noise_pattern1
	.word _default_New_song_noise_pattern1
	.word _default_New_song_noise_pattern1
	.word _default_New_song_noise_pattern1
	.word _default_New_song_noise_pattern1
	.word _default_New_song_noise_pattern1
	.word _default_New_song_noise_pattern1
	.word _default_New_song_noise_pattern2
	.word _default_New_song_noise_pattern3
	.word _default_New_song_noise_pattern3
	.word _default_New_song_noise_pattern3
	.word _default_New_song_noise_pattern4
	.word _default_New_song_noise_pattern3
	.word _default_New_song_noise_pattern3
	.word _default_New_song_noise_pattern3
	.word _default_New_song_noise_pattern5
	.word _default_New_song_noise_pattern6
	.word _default_New_song_noise_pattern7
	.word _default_New_song_noise_pattern8
	.word _default_New_song_noise_pattern9
_default_New_song_noise_pattern0:
	.byte NL4,INST|CONT|2,$0,NL6,INST|3,INST|4,NL2,INST|3,NL6,INST|CONT|5
	.byte $E,NL4,INST|CONT|2,$0,NL6,INST|3,INST|4,NL2,INST|3,NL6
	.byte INST|7,D00
_default_New_song_noise_pattern1:
	.byte NL4,INST|CONT|2,$0,NL6,INST|3,INST|2,NL2,INST|3,NL6,INST|CONT|5
	.byte $E,NL4,INST|CONT|2,$0,NL6,INST|3,INST|2,NL2,INST|3,NL6
	.byte INST|7,D00
_default_New_song_noise_pattern2:
	.byte NL4,INST|CONT|2,$0,NL6,INST|3,INST|2,NL2,INST|3,NL6,INST|CONT|5
	.byte $E,NL4,INST|CONT|2,$0,NL6,INST|3,INST|2,NL2,INST|3,INST|7
	.byte NL4,$0,D00
_default_New_song_noise_pattern3:
	.byte NL4,INST|CONT|2,$0,NL2,INST|3,INST|7,INST|4,INST|2,INST|3,INST|4
	.byte INST|3,NL3,INST|7,INST|CONT|5,$E,NL4,INST|CONT|2,$0,NL2,INST|3
	.byte INST|7,INST|4,INST|2,INST|3,INST|4,INST|3,INST|7,INST|3,INST|16,D00
_default_New_song_noise_pattern4:
	.byte NL4,INST|CONT|2,$0,NL2,INST|3,INST|7,INST|4,INST|2,INST|3,INST|4
	.byte INST|3,NL3,INST|7,INST|CONT|5,$E,NL4,INST|CONT|2,$0,NL2,INST|3
	.byte INST|7,INST|4,INST|2,INST|3,INST|4,INST|3,INST|7,$0,INST|16,D00
_default_New_song_noise_pattern5:
	.byte NL4,INST|CONT|2,$0,NL2,INST|3,INST|7,INST|4,INST|2,INST|3,INST|4
	.byte INST|3,NL3,INST|7,INST|CONT|5,$E,NL4,INST|CONT|2,$0,NL2,INST|3
	.byte INST|7,INST|4,INST|2,INST|3,INST|7,INST|2,INST|7,$0,INST|16,D00
_default_New_song_noise_pattern6:
	.byte NL12,INST|CONT|20,$0,NL6,INST|4,INST|CONT|5,$E,NL12,INST|CONT|3,$0
	.byte NL6,INST|4,INST|3,D00
_default_New_song_noise_pattern7:
	.byte NL12,INST|CONT|3,$0,NL6,INST|4,INST|CONT|5,$E,NL12,INST|CONT|3,$0
	.byte NL6,INST|4,INST|3,D00
_default_New_song_noise_pattern8:
	.byte NL12,INST|CONT|2,$0,NL6,INST|4,INST|CONT|5,$E,NL12,INST|CONT|3,$0
	.byte NL6,INST|4,INST|3,D00
_default_New_song_noise_pattern9:
	.byte NL12,INST|CONT|3,$0,NL6,INST|4,INST|CONT|5,$E,NL12,INST|CONT|20,$0
	.byte NL2,INST|2,INST|7,INST|2,INST|7,$0,INST|16,D00

