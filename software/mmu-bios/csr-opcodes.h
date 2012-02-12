/* -*- asm -*-
 *
 * Macros to assemble LM32 CSR read/write instructions for *any* CSR, even the
 * ones not supported by GNU binutils.
 *
 * Author: David Kühling <dvdkhlng TA gmx TOD de>
 * Created: Feb 2012
 *
 * License: (at your opinion) GPLv2 or later or LM32 open-source license.
 */

	.set CSR_IE, 0 
	.set CSR_IM, 1 
	.set CSR_IP, 2 
	.set CSR_ICC, 3 
	.set CSR_DCC, 4 
	.set CSR_CC, 5 
	.set CSR_CFG, 6 
	.set CSR_EBA, 7 
	.set CSR_DC, 8 
	.set CSR_DEBA, 9 
	.set CSR_JTX, 14 
	.set CSR_JRX, 15 
	.set CSR_BP0, 16 
	.set CSR_BP1, 17 
	.set CSR_BP2, 18 
	.set CSR_BP3, 19 
	.set CSR_WP0, 24 
	.set CSR_WP1, 25 
	.set CSR_WP2, 26 
	.set CSR_WP3, 27 
	.set CSR_TLBCTRL, 28 
	.set CSR_TLBVADDR, 29 
	.set CSR_TLBPADDR, 30 
	
	/* usage:  WCSR_EXT  CSR_NR, REG_NR 
	 * 
	 * example: WCSR_EXT  CSR_IM, 7
	 */ 
	.macro WCSR_EXT dst_csr, src_reg
	.word 0xD0000000+(\dst_csr<<21)+(\src_reg<<16)
	.endm

	/* usage:  RCSR_EXT  REG_NR, CSR_NR
	 * 
	 * example: RCSR_EXT  5, CSR_IM
	 */ 
	.macro RCSR_EXT dst_reg, src_csr
	.word 0x90000000+(\src_csr<<21)+(\dst_reg<<11)
	.endm

	    
