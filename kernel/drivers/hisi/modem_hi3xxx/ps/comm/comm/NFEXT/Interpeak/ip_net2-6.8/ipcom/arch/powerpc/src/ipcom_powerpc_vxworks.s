#define _ASMLANGUAGE
#include "vxWorks.h"
#include "asm.h"
#include "regs.h"
        .text

        .globl ipcom_in_checksum_update2_asm
ipcom_in_checksum_update2_asm:
        andi.	r6,r3,0x2
        li      r5,0
        beq+	1f
        lhz	    r5,0(r3)
        addi	r3,r3,2
        subi	r4,r4,2
1:
        andi.   r6,r4,0xc
        srawi   r7,r4,4
        cmpi    cr1,r6,0x8
        addic   r7,r7,1
        subi    r3,r3,4
        mtspr   9,r7
        beq     chk0
        bgt     cr1,chk3
        blt     cr1,chk1
        b       chk2

chk_loop:
        lwzu    r6,4(r3)
        adde    r5,r5,r6

chk3:
        lwzu    r7,4(r3)
        adde    r5,r5,r7

chk2:
        lwzu    r6,4(r3)
        adde    r5,r5,r6

chk1:
        lwzu    r7,4(r3)
        adde    r5,r5,r7

chk0:
        bc      16,0,chk_loop

        addi	r3,r3,4
        andi.   r7,r4,2
        beq+    no_halfword

        lhz	    r6,0(r3)
        addi	r3,r3,2
        adde    r5,r5,r6

no_halfword:
        andi.   r7,r4,1
        beq+    no_byte

        lbz	    r6,0(r3)
        rlwinm  r6,r6,8,16,23
        adde    r5,r5,r6

no_byte:
        rlwinm  r6,r5,16,16,31
        rlwinm  r5,r5,0,16,31
        adde    r5,r5,r6
        rlwinm  r6,r5,16,16,31
        rlwinm  r5,r5,0,16,31
        add     r3,r5,r6
        blr

        .globl ipcom_memcpy_asm
ipcom_memcpy_asm:
        cmpli   cr0,r5,17
        xor     r8,r3,r4
        mr      r6,r3
        blt-    byte_copy

        andi.   r7,r8,1
        bne-    byte_copy

        andi.   r7,r6,1
        beq+    check_hword_align

        lbz     r7,0(r4)
        subi    r5,r5,1
        stb     r7,0(r6)
        addi    r4,r4,1
        addi    r6,r6,1

check_hword_align:
        andi.   r7,r8,2
        beq+    word_align

        subi    r4,r4,2
        subi    r6,r6,2
        srawi   r8,r5,4
        rlwinm.	r5,r5,0,28,31
        mtspr   9,r8

hword_loop:
        lhzu    r7,2(r4)
        sthu    r7,2(r6)
        lhzu    r8,2(r4)
        sthu    r8,2(r6)
        lhzu    r7,2(r4)
        sthu    r7,2(r6)
        lhzu    r8,2(r4)
        sthu    r8,2(r6)
        lhzu    r7,2(r4)
        sthu    r7,2(r6)
        lhzu    r8,2(r4)
        sthu    r8,2(r6)
        lhzu    r7,2(r4)
        sthu    r7,2(r6)
        lhzu    r8,2(r4)
        sthu    r8,2(r6)
        bc      16,0,hword_loop

        beqlr

        addi	r4,r4,1
        addi	r6,r6,1
        b       byte_copy_cont

word_align:
        andi.   r7,r6,2
        beq-    word_copy

        lhz     r7,0(r4)
        subi    r5,r5,2
        sth     r7,0(r6)
        subi    r4,r4,2
        subi    r6,r6,2
        b       word_copy_cont

word_copy:
        subi    r4,r4,4
        subi    r6,r6,4

word_copy_cont:
        andi.	r7,r5,0xc
        srawi   r8,r5,4
        cmpi	cr1,r7,0x8
        addi	r8,r8,1
        mtspr   9,r8
        beq     wcopy0
        bgt     cr1,wcopy3
        blt     cr1,wcopy1
        b       wcopy2

word_loop:
        lwzu    r7,4(r4)
        stwu    r7,4(r6)

wcopy3:
        lwzu    r8,4(r4)
        stwu    r8,4(r6)

wcopy2:
        lwzu    r7,4(r4)
        stwu    r7,4(r6)
wcopy1:
        lwzu    r8,4(r4)
        stwu    r8,4(r6)

wcopy0:
        bc      16,0,word_loop

        rlwinm. r5,r5,0,30,31
        addi	r4,r4,3
        addi	r6,r6,3
        beqlr
        b       byte_copy_cont

byte_copy:
        subi	r4,r4,1
        subi	r6,r6,1
byte_copy_cont:
        andi.	r7,r5,0x3
        srawi   r8,r5,2
        cmpi	cr1,r7,2
        addi	r8,r8,1
        mtspr   9,r8
        beq     bcopy0
        bgt     cr1,bcopy3
        blt     cr1,bcopy1
        b       bcopy2

byte_loop:
        lbzu	r7,1(r4)
        stbu	r7,1(r6)

bcopy3:
        lbzu	r8,1(r4)
        stbu	r8,1(r6)

bcopy2:
        lbzu	r7,1(r4)
        stbu	r7,1(r6)

bcopy1:
        lbzu	r8,1(r4)
        stbu	r8,1(r6)

bcopy0:
        bc      16,0,byte_loop

        blr

        .globl ipcom_in_checksum_memcpy_asm
ipcom_in_checksum_memcpy_asm:
        xor     r6,r6,r6
        cmpli   cr0,r5,17
        li      r12,0
        xor     r8,r3,r4
        addic	r5,r5,0
        blt-    scpy_byte_copy

        andi.   r7,r8,1
        bne-    scpy_byte_copy

        andi.   r7,r3,1
        beq+    scpy_check_hword_align

        lbz     r7,0(r4)
        subi    r5,r5,1
        li      r12,1
        stb     r7,0(r3)
        adde    r6,r6,r7
        addi    r4,r4,1
        addi    r3,r3,1

scpy_check_hword_align:
        andi.   r7,r8,2
        beq+    scpy_word_align

        andi.	r7,r5,0x6
        subi    r4,r4,2
        subi    r3,r3,2
        srawi   r8,r5,3
        cmpi	cr1,r7,0x4
        addi	r8,r8,1
        mtspr   9,r8
        beq     scpy_hwcopy0
        bgt     cr1,scpy_hwcopy3
        blt     cr1,scpy_hwcopy1
        b       scpy_hwcopy2

scpy_hword_loop:
        lhzu    r7,2(r4)
        sthu    r7,2(r3)
        adde    r6,r6,r7

scpy_hwcopy3:
        lhzu    r8,2(r4)
        sthu    r8,2(r3)
        adde    r6,r6,r8

scpy_hwcopy2:
        lhzu    r7,2(r4)
        sthu    r7,2(r3)
        adde    r6,r6,r7

scpy_hwcopy1:
        lhzu    r8,2(r4)
        sthu    r8,2(r3)
        adde    r6,r6,r8

scpy_hwcopy0:
        bc      16,0,scpy_hword_loop

        rlwinm.	r5,r5,0,31,31
        beq+	scpy_end

        addi	r4,r4,1
        addi	r3,r3,1
        b       scpy_trailing_bytes

scpy_word_align:
        andi.   r7,r3,2
        beq-    scpy_word_copy

        lhz     r7,0(r4)
        subi    r5,r5,2
        sth     r7,0(r3)
        adde    r6,r6,r7
        subi    r4,r4,2
        subi    r3,r3,2
        b       scpy_word_copy_cont

scpy_word_copy:
        subi    r4,r4,4
        subi    r3,r3,4

scpy_word_copy_cont:
        andi.	r7,r5,0xc
        srawi   r8,r5,4
        cmpi	cr1,r7,0x8
        addi	r8,r8,1
        mtspr   9,r8
        beq     scpy_wcopy0
        bgt     cr1,scpy_wcopy3
        blt     cr1,scpy_wcopy1
        b       scpy_wcopy2

scpy_word_loop:
        lwzu    r7,4(r4)
        stwu    r7,4(r3)
        adde    r6,r6,r7

scpy_wcopy3:
        lwzu    r8,4(r4)
        stwu    r8,4(r3)
        adde    r6,r6,r8

scpy_wcopy2:
        lwzu    r7,4(r4)
        stwu    r7,4(r3)
        adde    r6,r6,r7

scpy_wcopy1:
        lwzu    r8,4(r4)
        stwu    r8,4(r3)
        adde    r6,r6,r8

scpy_wcopy0:
        bc      16,0,scpy_word_loop

        rlwinm. r5,r5,0,30,31
        addi	r4,r4,3
        addi	r3,r3,3
        beq	    scpy_end
        b       scpy_trailing_bytes

scpy_byte_copy:
        subi	r4,r4,1
        subi	r3,r3,1
        srawi   r8,r5,2
        rlwinm	r5,r5,0,30,31
        addi	r8,r8,1
        mtspr   9,r8
        b       scpy_bcopy0

scpy_byte_loop:
        lbzu	r7,1(r4)
        rlwinm  r8,r7,8,16,23
        stbu	r7,1(r3)
        adde    r6,r6,r8

scpy_bcopy3:
        lbzu	r8,1(r4)
        stbu	r8,1(r3)
        adde    r6,r6,r8

scpy_bcopy2:
        lbzu	r7,1(r4)
        rlwinm  r8,r7,8,16,23
        stbu	r7,1(r3)
        adde    r6,r6,r8

scpy_bcopy1:
        lbzu	r8,1(r4)
        stbu	r8,1(r3)
        adde    r6,r6,r8

scpy_bcopy0:
        bc      16,0,scpy_byte_loop

scpy_trailing_bytes:
        li	    r9,0

trailing_loop:
        cmpli	cr0,r5,0
        subi	r5,r5,1
        beq-	scpy_trailing_end

        cmpli	cr0,r9,0
        lbzu	r7,1(r4)
        stbu	r7,1(r3)
        bne	    no_shift

        rlwinm  r7,r7,8,16,23

no_shift:
        xori	r9,r9,1
        adde    r6,r6,r7
        b	    trailing_loop

scpy_trailing_end:

scpy_end:
	    cmpli	cr0,r12,0
        rlwinm  r7,r6,16,16,31
        rlwinm  r6,r6,0,16,31
        adde    r6,r6,r7
        rlwinm  r7,r6,16,16,31
        rlwinm  r6,r6,0,16,31
        add     r3,r6,r7

        beqlr
        rlwinm	r4,r3,24,24,31
        rlwinm	r3,r3,8,16,23
        or	    r3,r3,r4
        blr

        .end


