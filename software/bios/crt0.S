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
	calli	_sdram_init
	xor	r2, r2, r2
	bi	_crt0

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
	calli   isr
	bi      .restore_all_and_eret
	nop
	nop
	nop
	nop

_syscall_handler:
	bi	_syscall_handler
	nop
	nop
	nop
	nop
	nop
	nop
	nop

_itlb_miss_exception:
	bi	_itlb_miss_exception
	nop
	nop
	nop
	nop
	nop
	nop
	nop


_dtlb_miss_exception:
	bi	_fake_dtlb_miss_exception_handler
	nop
	nop
	nop
	nop
	nop
	nop
	nop

_dtlb_fault_exception:
	bi	_dtlb_fault_exception
	nop
	nop
	nop
	nop
	nop
	nop
	nop

_privilege_exception:
	bi	_privilege_exception
	nop
	nop
	nop
	nop
	nop
	nop
	nop

_fake_itlb_miss_exception_handler:
	mvhi    r0, 0x47ff
	ori	r0, r0, 0xfff0
	sw	(r0+0), r1
	sw	(r0+4), r2
	sw	(r0+8), r3
	xor	r0, r0, r0

	mvhi	r2, 0xffff
	ori	r2, r2, 0xf000

	rcsr	r1, TLBPADDR
	and	r1, r1, r2 /* r1 = r1 & r2 to discard page offset */

	/* retrieve current ASID */
//	rcsr	r2, PSW
//	mvhi	r3, 1
//	ori	r3, r3, 0xf000 /* build psw.asid mask */
//	and	r2, r2, r3 /* mask out everything except ASID */
//	sri	r2, r2, 5 /* shift psw.asid to be in place for being or'ed with tlbvaddr.asid */
//	or	r2, r2, r1

	wcsr	TLBVADDR, r1
	wcsr	TLBPADDR, r1

	mvhi    r0, 0x47ff
	ori	r0, r0, 0xfff0
	lw	r1, (r0+0)
	lw	r2, (r0+4)
	lw	r3, (r0+8)
	xor	r0, r0, r0
	eret

_fake_dtlb_miss_exception_handler:
	mvhi    r0, 0x47ff
	ori	r0, r0, 0xfff0
	sw	(r0+0), r1
	sw	(r0+4), r2
	sw	(r0+8), r3
	xor	r0, r0, r0

	mvhi	r2, 0xffff
	ori	r2, r2, 0xf000

	rcsr	r1, TLBPADDR
	and	r1, r1, r2 /* r1 = r1 & r2 to discard page offset */
	ori	r1, r1, 1 /* because we wanna update DTLB */

	/* retrieve current ASID */
	rcsr	r2, PSW
	mvhi	r3, 1
	ori	r3, r3, 0xf000 /* build psw.asid mask */
	and	r2, r2, r3 /* mask out everything except ASID */
	sri	r2, r2, 5 /* shift psw.asid to be in place for being or'ed with tlbvaddr.asid */
	or	r2, r2, r1

	wcsr	TLBVADDR, r2
	wcsr	TLBPADDR, r1

	mvhi    r0, 0x47ff
	ori	r0, r0, 0xfff0
	lw	r1, (r0+0)
	lw	r2, (r0+4)
	lw	r3, (r0+8)
	xor	r0, r0, r0
	eret

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
.clearBSS:
	be      r1, r3, .callMain
	sw      (r1+0), r0
	addi    r1, r1, 4
	bi      .clearBSS

.callMain:
	mv      r1, r2

	/* Activating ITLB and DTLB */
	rcsr	r2, PSW
	ori	r2, r2, 0x40
	wcsr	PSW, r2

	mvi     r2, 0
	mvi     r3, 0
	bi      main

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
