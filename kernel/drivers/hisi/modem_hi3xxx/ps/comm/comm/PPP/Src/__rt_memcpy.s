/*
* __rt_memcpy.s is a memory copy assemble function.
*/

.text
.global __rt_memcpy
.hidden __rt_memcpy
.type __rt_memcpy,%function
.align 4
  
__rt_memcpy:
    CMP      r2,#3
    BLS      ._memcpy_lastbytes
    ANDS     r12,r0,#3
    BEQ      ._memcpy_dest_aligned
    LDRB     r3,[r1],#1
    CMP      r12,#2
    ADD      r2,r2,r12
    LDRLSB   r12,[r1],#1
    STRB     r3,[r0],#1
    LDRCCB   r3,[r1],#1
    SUB      r2,r2,#4
    STRLSB   r12,[r0],#1
    STRCCB   r3,[r0],#1

._memcpy_dest_aligned:
    ANDS     r3,r1,#3
    BEQ      .__rt_memcpy_w
    SUBS     r2,r2,#4
    BCC      ._memcpy_lastbytes
    LDR      r12,[r1,-r3]!
    CMP      r3,#2
    BEQ      ._memcpy_src2_loop
    BHI      ._memcpy_src3_loop

._memcpy_src1_loop:
    MOV      r3,r12,LSR #8
    LDR      r12,[r1,#4]!
    SUBS     r2,r2,#4
    ORR      r3,r3,r12,LSL #24
    STR      r3,[r0],#4
    BCS      ._memcpy_src1_loop
    ADD      r1,r1,#1
    B        ._memcpy_lastbytes

._memcpy_src2_loop:
    MOV      r3,r12,LSR #16
    LDR      r12,[r1,#4]!
    SUBS     r2,r2,#4
    ORR      r3,r3,r12,LSL #16
    STR      r3,[r0],#4
    BCS      ._memcpy_src2_loop
    ADD      r1,r1,#2
    B        ._memcpy_lastbytes

._memcpy_src3_loop:
    MOV      r3,r12,LSR #24
    LDR      r12,[r1,#4]!
    SUBS     r2,r2,#4
    ORR      r3,r3,r12,LSL #8
    STR      r3,[r0],#4
    BCS      ._memcpy_src3_loop
    ADD      r1,r1,#3
    B        ._memcpy_lastbytes

.__rt_memcpy_w:
    STMFD    r13!,{r4,r12,r14}
    SUBS     r2,r2,#0x20
    BCC      ._memcpy_small

._memcpy_aligned_loop:
    LDMCSIA  r1!,{r3,r4,r12,r14}
    STMCSIA  r0!,{r3,r4,r12,r14}
    LDMCSIA  r1!,{r3,r4,r12,r14}
    STMCSIA  r0!,{r3,r4,r12,r14}
    SUBCSS   r2,r2,#0x20
    BCS      ._memcpy_aligned_loop

._memcpy_small:
    MOVS     r12,r2,LSL #28
    LDMCSIA  r1!,{r3,r4,r12,r14}
    STMCSIA  r0!,{r3,r4,r12,r14}
    LDMMIIA  r1!,{r3,r4}
    STMMIIA  r0!,{r3,r4}
    LDMFD    r13!,{r4,r12,r14}
    MOVS     r12,r2,LSL #30
    LDRCS    r3,[r1],#4
    STRCS    r3,[r0],#4
    MOVEQ    pc,r14

._memcpy_lastbytes:
    MOVS     r2,r2,LSL #31
    LDRMIB   r2,[r1],#1
    LDRCSB   r3,[r1],#1
    LDRCSB   r12,[r1],#1
    STRMIB   r2,[r0],#1
    STRCSB   r3,[r0],#1
    STRCSB   r12,[r0],#1
    MOV      pc,r14
