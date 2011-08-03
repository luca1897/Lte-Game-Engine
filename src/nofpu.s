	.file	1 "nofpu.c"
	.section .mdebug.eabi32
	.section .gcc_compiled_long32
	.previous
 #APP
	.text
.set push
.set noreorder
.globl _DisableFPUExceptions
.ent _DisableFPUExceptions
_DisableFPUExceptions: 
cfc1 $2, $31 
lui $8, 0x80 
and $8, $2, $8 # Mask off all bits except for 23 of FCR 
ctc1 $8, $31 
jr  $31 
nop 
.end _DisableFPUExceptions
.set pop

	.ident	"GCC: (GNU) 4.0.2 (PSPDEV 20051022)"
