/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_arm.s,v $ $Revision: 1.2.34.1 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/arch/arm/src/ipcom_arm.s,v $
 *   $Author: hchowdhu $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kjonsson@windriver.com>
 ******************************************************************************
 */

/*
modification history
--------------------
01a,25Aug10,hfc  fix for defect WIND00220454
*/

/*
 ****************************************************************************
 *                      DESCRIPTION
 ****************************************************************************
 */
/*
   Assembler version of Internet checksum routines used in IPNET
 */


/*
 ****************************************************************************
 *                      CONFIGURATION
 ****************************************************************************
 */
/* Include to defined endian macros */
#define _ASMLANGUAGE
#include "vxWorks.h"
#include "asm.h"
#include "regs.h"
#if _BYTE_ORDER == _BIG_ENDIAN
#define BIG_ENDIAN_ARM
#endif

        .file   "ipcom_arm.s"
        .text

/*
 *===========================================================================
 *                    ipcom_in_checksum_update2_asm
 *===========================================================================
 * Description: Calculates the Internet checksum for a buffer.
 * Parameters:  r0 - Pointer to the buffer to calculate checksum for. Must
 *                   be word or halfword aligned.
 *              r1 - Length of the buffer
 * Returns:     r0 = 32-bit partial checksum
 *
 */
        .align  2
        .global ipcom_in_checksum_update2_asm

ipcom_in_checksum_update2_asm:
        stmfd   sp!, {r4,r5,r6,r7,lr}
        mov     r2, r0
        movs    r4, r0, lsl #30
        mov     r0, #0
        beq     .chksum_aligned
        /* r0 is not a multiple of 4 */
        cmp     r1, #2
        bls     .chksum_bytes
        ldrh    r0, [r2], #2
        sub     r1, r1, #2

.chksum_aligned:
        movs    r7, r1, lsr #5
        beq     .chksum_1_word
        sub     r1, r1, r7, lsl #5

.chksum_8_words:
        ldmia   r2!, {r3,r4,r5,r6}
        adds    r0, r0, r3
        adcs    r0, r0, r4
        adcs    r0, r0, r5
        adcs    r0, r0, r6
        ldmia   r2!, {r3,r4,r5,r6}
        adcs    r0, r0, r3
        adcs    r0, r0, r4
        adcs    r0, r0, r5
        adcs    r0, r0, r6
        adccs   r0, r0, #0
        subs    r7, r7, #1
        bne     .chksum_8_words

.chksum_1_word:
        cmp     r1, #4
        blo     .chksum_bytes
        ldr     r3, [r2], #4
        adds    r0, r0, r3
        adccs   r0, r0, #0
        subs    r1, r1, # 4
        b       .chksum_1_word

.chksum_bytes:
        /* Take care of the last 0-3 bytes */
        cmp     r1, #0
        beq     .chk_done
        ldrb    r4, [r2], #1
#ifdef BIG_ENDIAN_ARM
        adds    r0, r0, r4, lsl #8
#else
        adds    r0, r0, r4
#endif
        adccs   r0, r0, #0
        cmp     r1, #1
        beq     .chk_done
        ldrb    r4, [r2], #1
#ifdef BIG_ENDIAN_ARM
        adds    r0, r0, r4
#else
        adds    r0, r0, r4, lsl #8
#endif
        adccs   r0, r0, #0
        cmp     r1, #2
        beq     .chk_done
        ldrb    r4, [r2], #1
#ifdef BIG_ENDIAN_ARM
        adds    r0, r0, r4, lsl #8
#else
        adds    r0, r0, r4
#endif
        adccs   r0, r0, #0

.chk_done:
        ldmfd   sp!, {r4,r5,r6,r7,pc}

        .size   ipcom_in_checksum_update2_asm, .-ipcom_in_checksum_update2_asm


/*
 *===========================================================================
 *                    ipcom_in_checksum_memcpy_asm
 *===========================================================================
 * Description: Calculates the Internet checksum while copying a buffer.
 * Parameters:  r0 - Pointer to the destination buffer. Can have any
 *                  alignment.
 *              r1 - Pointer to the source buffer. Can have any alignment.
 *              r2 - Number of bytes to copy
 * Returns:     r0 = 32-bit partial checksum
 *
 */
dst     .req    r3
src     .req    r1
sum     .req    r0

        .align  2
        .global ipcom_in_checksum_memcpy_asm

ipcom_in_checksum_memcpy_asm:
        stmfd   sp!, {r4,r5,r6,r7,r8,r9,r10,lr}

        mov     dst, r0
        eor     sum, sum, sum
      	eor		r10,r10,r10 
        /* r9 will be 1 if the checksum must be swapped */
        and     r9, src, #1
        /* Less than 4 bytes is copied byte for byte */
        cmp     r2, #4
        blt     .cpy_epilog      
		b		.calculate_no_of_extra_swap				

.align_dst:
        ands    r8, dst, #3
        beq     .dst_aligned        
        /* dst not word aligned, keep copy one byte until dst is word
           aligned */
        sub     r7, r2, #1
        mov     r2, #1
        bl      .cpy_byte
        mov     r2, r7
        b       .align_dst

.dst_aligned:
        /* dst is always word aligned at this point, src can have any
           alignment at this point */
        tst     src, #3
        bne     .cpy_unaligned
        
        movs    r8, r2, lsr #5
        sub     r2, r2, r8, lsl #5
        blne    .cpy_32_bytes_aligned

        movs    r8, r2, lsr #2
        sub     r2, r2, r8, lsl #2
        blne    .cpy_4_bytes_aligned

.cpy_epilog:
        bl      .cpy_bytes
        /* Check if the bytes must be swapped in the checksum, this is
           the case if the src pointer passed to
           ipcom_in_checksum_memcpy_asm was odd */
#ifdef BIG_ENDIAN_ARM
		/* In big endian the last swap is not required if src is odd unaligned 
		   and vice versa  */
		cmp     r9, #0
#else
	    cmp     r9, #1
#endif                
        moveq   r0, r0, ror #8
       
       
        mov    r4, r0, lsr #16
        mov    r0, r0, lsl #16
        add   r0, r4, r0, lsr #16
         
        ldmfd   sp!, {r4,r5,r6,r7,r8,r9,r10,pc}

        /* Subroutine for copy unaligned buffers */
.cpy_unaligned:
        movs    r8, r2, lsr #3
       
        beq     .cpy_epilog
        sub     r2, r2, r8, lsl #3
        
        cmp 	r10,#2 
        moveq	r0,r0, ror #8
        
        cmp 	r10,#3 
        moveq   r0, r0, ror #8 
        
        and     r7, src, #3
        sub     src, src, r7
        ldr     r4, [src], #4
        tst     r7, #1
        beq     .cpy_2_bytes_unaligned
        tst     r7, #2
        bne     .cpy_3_bytes_unaligned

.cpy_1_byte_unaligned:
        ldmia   src!, {r5,r6}
#ifdef BIG_ENDIAN_ARM
        mov     r4, r4, lsl #8
        orr     r4, r4, r5, lsr #24
        mov     r5, r5, lsl #8
        orr     r5, r5, r6, lsr #24
#else
        mov     r4, r4, lsr #8
        orr     r4, r4, r5, lsl #24
        mov     r5, r5, lsr #8
        orr     r5, r5, r6, lsl #24
#endif
        adds    sum, sum, r4
        adcs    sum, sum, r5
        adccs   sum, sum, #0
        stmia   dst!, {r4,r5}
        mov     r4, r6
        subs    r8, r8, #1
        bne     .cpy_1_byte_unaligned
        b       .cpy_unaligned_epilog

.cpy_3_bytes_unaligned:
        ldmia   src!, {r5,r6}
#ifdef BIG_ENDIAN_ARM
        mov     r4, r4, lsl #24
        orr     r4, r4, r5, lsr #8
        mov     r5, r5, lsl #24
        orr     r5, r5, r6, lsr #8
#else
        mov     r4, r4, lsr #24
        orr     r4, r4, r5, lsl #8
        mov     r5, r5, lsr #24
        orr     r5, r5, r6, lsl #8
#endif
        adds    sum, sum, r4
        adcs    sum, sum, r5
        adccs   sum, sum, #0
        stmia   dst!, {r4,r5}
        mov     r4, r6
        subs    r8, r8, #1
        bne     .cpy_3_bytes_unaligned
        b       .cpy_unaligned_epilog

.cpy_2_bytes_unaligned:
        ldmia   src!, {r5,r6}
#ifdef BIG_ENDIAN_ARM
        mov     r4, r4, lsl #16
        orr     r4, r4, r5, lsr #16
        mov     r5, r5, lsl #16
        orr     r5, r5, r6, lsr #16
#else
        mov     r4, r4, lsr #16
        orr     r4, r4, r5, lsl #16
        mov     r5, r5, lsr #16
        orr     r5, r5, r6, lsl #16
#endif
        adds    sum, sum, r4
        adcs    sum, sum, r5
        adccs   sum, sum, #0
        stmia   dst!, {r4,r5}
        mov     r4, r6
        subs    r8, r8, #1
        bne     .cpy_2_bytes_unaligned

.cpy_unaligned_epilog:
        sub     src, src, #4
        add     src, src, r7
        /* the 1 & 3 bytes unaligned cases must swap the bytes of the
           checksum since the byte copy logic assumes that it is
           swapped and it will be swapped back after the byte copy */
        cmp     r9, #1
        moveq   r0, r0, ror #8
       
        cmp 	r10,#1
        moveq   r0, r0, ror #8
    	 
    	cmp 	r10,#3 
        moveq   r0, r0, ror #8 
        
        b       .cpy_epilog

        /* Subroutine for copy aligned buffers */
.cpy_32_bytes_aligned:
        /* Copy 32 byte per round */
        ldmia   src!, {r4,r5,r6,r7}
        adds    sum, sum, r4
        adcs    sum, sum, r5
        adcs    sum, sum, r6
        adcs    sum, sum, r7
        stmia   dst!, {r4,r5,r6,r7}
        ldmia   src!, {r4,r5,r6,r7}
        adcs    sum, sum, r4
        adcs    sum, sum, r5
        adcs    sum, sum, r6
        adcs    sum, sum, r7
        stmia   dst!, {r4,r5,r6,r7}
        adccs   sum, sum, #0
        subs    r8, r8, #1
        bne     .cpy_32_bytes_aligned
        mov     pc, lr

.cpy_4_bytes_aligned:
        /* Copy 4 byte per round */
        ldr     r4, [src], #4
        adds    sum, sum, r4
        adccs   sum, sum, #0
        str     r4, [dst], #4
        subs    r8, r8, #1
        bne     .cpy_4_bytes_aligned
        mov     pc, lr

        /* Subroutine for copy byte for byte */
.cpy_bytes:
        /* Fold the 32-bit csum so carry does not have to be checked
           when summing the left-over bytes */
        cmp     r2, #0
        moveq   pc, lr
        mov     r4, r0, lsr #16
        mov     r0, r0, lsl #16
        add     r0, r4, r0, lsr #16
        
.cpy_byte:
        ldrb    r4, [src], #1
        tst     src, #1
#ifdef BIG_ENDIAN_ARM
        addeq   sum, sum, r4
        addne   sum, sum, r4, lsl #8
#else
        addeq   sum, sum, r4, lsl #8
        addne   sum, sum, r4
#endif
        strb    r4, [dst], #1

        subs    r2, r2, #1
        bne     .cpy_byte
        mov     pc, lr

.calculate_no_of_extra_swap:
		ands   r8,dst, #3			
		beq     .dst_aligned	/* if dst  is aligned no need for extra byte swap */
		
		/* at this stage dst is unaligned odd(1 or 3 bytes) or unaligned by halfword */		
		ands 	r7, src, #3
		beq		.src_is_even
		cmp		r7,#2
		beq		.src_is_even
		
        cmp 	r8,r7 
        bne		.set_flags_to_r10
		b 		.align_dst


.src_is_even:
		cmp		r8,#2
		beq     .align_dst 	
		 /*  at this src is even, aligned(0,4, ..) or unaligned (2,6,..)
			but dst is odd unaligned. This require 2 extra swap  
		 */
		 mov r10,#3  
		 /* r10 = 3 indicates src is even, align or half word align and dst is odd unaligned. */
		 b 		.align_dst
		 
.set_flags_to_r10:

		cmp		r7,#3
		addeq 	r10,r10,#1
		cmp 	r8,	#3
		addeq 	r10,r10,#1
		
		cmp 	r7,#2
		addeq	r10,r10,#1
		cmp 	r8,#2
		addeq	r10,r10,#1			
		
		cmp		r7,#3
		beq 		.align_dst
		cmp		r8,#3
		beq 		.align_dst
		
		cmp 	r8,#1
		addeq	r10,r10,#1
		
		cmp 	r7,#1
		addeq	r10,r10,#1
		
		/* 
		r10 = 2, indicates src is 3 byte unaligned and dst is 2 byte unaligned or vice versa. 
		r10 = 1, indicates src is 1 byte unaligned and dst is 2 byte unaligned or vice versa	
		*/
		
		b 		.align_dst 
		

        .size   ipcom_in_checksum_memcpy_asm, .-ipcom_in_checksum_memcpy_asm
