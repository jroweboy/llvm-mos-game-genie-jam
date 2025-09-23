
; Example CA65 assembly source file.
; This file is built using the powerful CA65 macro assembler, and then linked into the final .nes file
; There are a few changes that need to be made when using this that are worth pointing out, hence this example.

; Segment names are much more flexible in llvm-mos, so there is a custom renaming scheme to allow CA65 to reference llvm-mos names.
; For a full list, check out https://llvm-mos.org/wiki/Cc65_integration

; Segments in llvm-mos-sdk linker scripts are typically sorted, allowing you to "build" a complete routine.
; For instance, lets say you wanted to run your audio engine during NMI but do it *after*
; the OAM update routine runs. Many segments are defined in the linker script in llvm-mos with a pattern
; like `.segmentname.number` and changing that number changes where the following code is placed in that segment

; Use the ".nmi.200" segment which should put it near the end of the NMI routine.
; Larger numbers means later, smaller numbers means earlier
; .segment "_pnmi_p200"
; extra_nmi_code:
;     nop ; This nop will appear in the NMI routine right before the end
;     ; We don't want to add something like rts / rti here because this is part of the NMI routine
;     ; At the end of the nmi routine there is a `.nmi_end` segment that will do the rti for us

; ; CA65 can't know which segments use what addressing mode, so you can force it to be "zeropage" with `: zeropage`
; .segment "_pzeropage" : zeropage
; ; Lets make an example variable in ca65 just to show how to use it in llvm-mos (see main.cpp for where we use it)

; ; !! NOTICE !!: If you reserve ZEROPAGE variables in ca65, llvm-mos needs to be told how many bytes you reserved.
; ; By default llvm-mos uses the zeropage space for holding temporary values, so if you reserve ZP variables, then you
; ; need to update the CMakeLists.txt file to include the number of bytes you reserve. Look for the line `-mreserve-zp=`
; ; and update that as well.
; .globalzp var_defined_in_ca65
; var_defined_in_ca65: .res 1

; ; Instead of using the "CODE" segment, you will typically use the ".text" segment
; .segment "_ptext"

; ; In order for LLVM-MOS to reference a symbol, you will need to export it.
; ; The simplest way to export a symbol is to just use .global for the label
; .global example_ca65_data
; example_ca65_data:
; .asciiz "Hello CA65!"


; Some common segment names in the llvm-mos-sdk are
; .zp / .zeropage - Uninitialized zeropage data (see mos-platform\common\lib\zp-noinit-sections.ld)
; .zp.data        - Compiler initialized zeropage data (see mos-platform\common\lib\zp-data-sections.ld)
; .noinit / .bss  - Uninitialized CIRAM data
; .text           - Fixed bank PRG data
; .chr_rom        - CHR ROM data (sized based on the weak symbol you provide for the ines header)
; .nmi            - Code placed into the NMI function
; There isn't a complete full list afaik. For ROM sections, you can find them in the `.map` file generated when building
; For RAM sections ... i dunno i guess just read the linker scripts?
