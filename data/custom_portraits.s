.section .custom_portraits, "a"
.align 4

.macro kao_siro ptr:req
.string "SIRO"
.4byte \ptr
.word 0, 0
.endm

@ See "include/structs_str_mon_portrait.h"
.macro kao_portrait pal:req, gfx:req
.4byte \pal
.4byte \gfx
.endm

.include "data/kao/grookey.inc"