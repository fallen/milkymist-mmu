/*
 * LatticeMico32 C startup code.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* Exception handlers - Must be 32 bytes long. */
.section    .text, "ax", @progbits
.global     _start
_start:
_reset_handler:
	xor	r0, r0, r0
	wcsr	IE, r0
	mvhi	r1, hi(_reset_handler)
	ori	r1, r1, lo(_reset_handler)
	wcsr	EBA, r1
	bi	_crt0
	nop
	nop

_breakpoint_handler:
	bi _breakpoint_handler
	nop
	nop
	nop
	nop
	nop
	nop
	nop

_instruction_bus_error_handler:
	bi _instruction_bus_error_handler
	nop
	nop
	nop
	nop
	nop
	nop
	nop

_watchpoint_hander:
	bi _watchpoint_hander
	nop
	nop
	nop
	nop
	nop
	nop
	nop

_data_bus_error_handler:
	bi _data_bus_error_handler
	nop
	nop
	nop
	nop
	nop
	nop
	nop

_divide_by_zero_handler:
	bi _divide_by_zero_handler
	nop
	nop
	nop
	nop
	nop
	nop
	nop

_interrupt_handler:
	sw      (sp+0), ra
	calli   .save_all
//	calli   isr
	nop
	bi      .restore_all_and_eret
	nop
	nop
	nop
	nop

macaddress:
	.byte 0x10
	.byte 0xe2
	.byte 0xd5
	.byte 0x00
	.byte 0x00
	.byte 0x00

	/* padding to align to a 32-bit boundary */
	.byte 0x00
	.byte 0x00

_crt0:
/* useless load bytes */
	lb r2, (r0+0x164) 
	lb r2, (r0+0xe1)
	lb r2, (r0+0x14C)
	lb r2, (r0+0xe3)
	lb r2, (r0+0xe4)
	lb r2, (r0+0x238)
	nop
	nop
	mvi r3, 1
	mvi r2, 0x1ff
/* give time for DTLB to flush itself */
loop:
	nop
	sub r2, r2, r3
	be r2, r0, loop
	nop
	nop
	nop
/* TLBVADDR = 0x80001000 */
	mvhi r1, 0x8000
	ori r1, r1, 0x1000
	wcsr TLBVADDR, r1
/* TLBPADDR = 0x80000000 */
	xor r1, r1, r1
	mvhi r1, 0x8000
	wcsr TLBPADDR, r1
/* TLBCTRL = 0x80000010 which is cmd DTLB_UPDATE */
	xor r1, r1, r1
	mvhi r1, 0x8000
	ori r1, r1, 2
	wcsr TLBCTRL, r1
/* TLBCTRL = 0xA0000000 which is cmd DTLB_SWITCH_TO_USER_MODE */
	xor r1, r1, r1
	mvhi r1, 0xA000
	wcsr TLBCTRL, r1
/* Now DTLB should be active 
 * All accesses to page 1 should be remapped to page 0
 */
	nop
	nop
	nop
	nop
	nop
/* This should read at physical address 0x00000000 */
	mvi r1, 0x1040
	lw r2, (r1+0)
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	bi _crt0
	/* Setup stack and global pointer */
	mvhi    sp, hi(_fstack)
	ori     sp, sp, lo(_fstack)
	mvhi    gp, hi(_gp)
	ori     gp, gp, lo(_gp)

	/* Clear BSS */
	mvhi    r1, hi(_fbss)
	ori     r1, r1, lo(_fbss)
	mvhi    r3, hi(_ebss)
	ori     r3, r3, lo(_ebss)

.pipeau:
	.word 0x42424242
	.word 0x55555555

.clearBSS:
	be      r1, r3, .callMain
	sw      (r1+0), r0
	addi    r1, r1, 4
	bi      .clearBSS

.callMain:
	mv      r1, r2
	mvi     r2, 0
	mvi     r3, 0
//	bi      main
	nop

.save_all:
	addi    sp, sp, -56
	sw      (sp+4), r1
	sw      (sp+8), r2
	sw      (sp+12), r3
	sw      (sp+16), r4
	sw      (sp+20), r5
	sw      (sp+24), r6
	sw      (sp+28), r7
	sw      (sp+32), r8
	sw      (sp+36), r9
	sw      (sp+40), r10
	sw      (sp+48), ea
	sw      (sp+52), ba
	/* ra needs to be moved from initial stack location */
	lw      r1, (sp+56)
	sw      (sp+44), r1
	ret

.restore_all_and_eret:
	lw      r1, (sp+4)
	lw      r2, (sp+8)
	lw      r3, (sp+12)
	lw      r4, (sp+16)
	lw      r5, (sp+20)
	lw      r6, (sp+24)
	lw      r7, (sp+28)
	lw      r8, (sp+32)
	lw      r9, (sp+36)
	lw      r10, (sp+40)
	lw      ra, (sp+44)
	lw      ea, (sp+48)
	lw      ba, (sp+52)
	addi    sp, sp, 56
	eret


.section .lol:
.org 0x304
	.byte 0x42
	.byte 0x43
	.byte 0x55
	.byte 0x44
