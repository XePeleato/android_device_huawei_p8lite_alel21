###############################################################################
#                     INTERPEAK SOURCE FILE
#
#   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_powerpc_gcc.s,v $ $Revision: 1.5 $
#   $Source: /home/interpeak/CVSRoot/ipcom/arch/powerpc/src/ipcom_powerpc_gcc.s,v $
#   $Author: lob $
#   $State: Exp $ $Locker:  $
#
#       Copyright (C) 1995-98 by ENEA DATA AB. All rights reserved.          
#                 Written by Anders Hagstrom for INET.
#   Copyright Interpeak AB 2000-2004 <www.interpeak.se>. All rights reserved.
###############################################################################
#


#
#############################################################################
#                       DESCRIPTION
#############################################################################
# 

#
#############################################################################
#                       CONFIGURATION
#############################################################################
# 

#
#############################################################################
#                       SECTION
#############################################################################
# 

	.text

#
#############################################################################
#                       DEFINES
#############################################################################
# 

#
#############################################################################
#                       TYPES
#############################################################################
# 

#
#############################################################################
#                       EXTERN PROTOTYPES
#############################################################################
# 

#
#############################################################################
#                       LOCAL PROTOTYPES
#############################################################################
# 

#
#############################################################################
#                       DATA
#############################################################################
# 

#
#############################################################################
#                       STATIC FUNCTIONS
#############################################################################
# 

#
#############################################################################
#                       GLOBAL FUNCTIONS
#############################################################################
# 

#
#############################################################################
#                       PUBLIC FUNCTIONS
#############################################################################
# 

#
#############################################################################
#                    ipcom_in_checksum_update2_asm
#############################################################################
# Prototype:	IP_PUBLIC Ip_u32
#               ipcom_in_checksum_update2_asm(void *pbuf, Ip_size_t bytes);
# Description:  Computes the internet checksum (RFC 1071). NOTE
#               it does not compute the 1 complement checksum.
# Parameters:   16bit aligned pbuf and bytes > 20
# Returns:      A standard internet checksum..
#
        .globl ipcom_in_checksum_update2_asm
ipcom_in_checksum_update2_asm:
	andi.	r6,r3,0x2	#Check if halfword or word alignment.
        li      r5,0            #Init checksum.
	beq+	1f
	lhz	r5,0(r3)	#Get first halword to get word align.
	addi	r3,r3,2		#Move pointer.
	subi	r4,r4,2		#Compensate numBytes.
1:		
        andi.   r6,r4,0xc       #Extract extra word accesses.
        srawi   r7,r4,4         #numBytes / 16.

        cmpi    cr1,r6,0x8
        addic   r7,r7,1         #Compensate loop counter, clear carry.
        subi    r3,r3,4         #Compensate for lwzu.
        mtspr   ctr,r7          #Setup loop counter.
        beq     chk0            #Branch if no extra word.
        bgt     cr1,chk3        #Branch if 3 extra word.
        blt     cr1,chk1        #Branch if 1 extra word.
        b       chk2            #2 extra word required.
chk_loop:
        lwzu    r6,4(r3)
        adde    r5,r5,r6        #Add to checksum.
chk3:
        lwzu    r7,4(r3)
        adde    r5,r5,r7        #Add to checksum.
chk2:
        lwzu    r6,4(r3)
        adde    r5,r5,r6        #Add to checksum.
chk1:
        lwzu    r7,4(r3)
        adde    r5,r5,r7        #Add to checksum.
chk0:
        bc      16,0,chk_loop

	addi	r3,r3,4		#Update pointer.

#       Handle trailing halfword.
        andi.   r7,r4,2
        beq+    no_halfword
        lhz	r6,0(r3)        #Load last halfword.
	addi	r3,r3,2		#Update pointer.
        adde    r5,r5,r6        #Add to checksum.
no_halfword:

#       Handle trailing byte.
        andi.   r7,r4,1
        beq+    no_byte
        lbz	r6,0(r3)	#Load last byte.
        rlwinm  r6,r6,8,16,23   #Shift left 8 times.
        adde    r5,r5,r6        #Add to checksum.
no_byte:

#       Convert checksum from 32 bit to 16 bit.

        rlwinm  r6,r5,16,16,31  #Extract high part of checksum.
        rlwinm  r5,r5,0,16,31   #Truncate high part of checksum.
        adde    r5,r5,r6        #High part + low part + carry.
        rlwinm  r6,r5,16,16,31  #Extract carry from checksum.
        rlwinm  r5,r5,0,16,31   #Truncate high part of checksum.
        add     r3,r5,r6        #Add carry to low part.
        blr


#
#############################################################################
#                    ipcom_memcpy_asm
#############################################################################
# Prototype:	IP_PUBLIC void *
#               ipcom_memcpy_asm(void *s1, const void *s2, Ip_size_t n);
# Description:  Copy data.
# Parameters:   
# Returns:      
#
        .globl ipcom_memcpy_asm
ipcom_memcpy_asm:
        cmpli   cr0,r5,17       #Threshold for byte copy.
        xor     r8,r3,r4	#Compare dest and src.
	mr	r6,r3		#Leave r3 (dest) as return value.
        blt-    byte_copy
        andi.   r7,r8,1         #Check for same byte alignment.
        bne-    byte_copy       #Byte copy if even/odd alignment.

        andi.   r7,r6,1         #Check for leading odd address.
        beq+    check_hword_align
        lbz     r7,0(r4)        #Copy one byte to get halfword align.
        subi    r5,r5,1         #Decrement numBytes.
        stb     r7,0(r6)
        addi    r4,r4,1         #Move pointers.
        addi    r6,r6,1
check_hword_align:
	
        andi.   r7,r8,2         #Check for same halfword alignment.
        beq+    word_align      #Word copy if possible.

#	Do halfword copy.
        subi    r4,r4,2         #Compensate pointers for lhzu/sthu.
        subi    r6,r6,2
        srawi   r8,r5,4         #numBytes / 16.
	rlwinm.	r5,r5,0,28,31	#Calculate trailing bytes.
        mtspr   ctr,r8
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

        beqlr                   #Done if numBytes = 0.
	addi	r4,r4,1		#Move pointers.
	addi	r6,r6,1
        b       byte_copy_cont  #Continue with trailing bytes.

#	Do word copy.

word_align:
        andi.   r7,r6,2         #Check for leading halfword address.
        beq-    word_copy
        lhz     r7,0(r4)        #Copy halfword to get word align.
        subi    r5,r5,2         #Decrement numBytes.
        sth     r7,0(r6)
        subi    r4,r4,2         #Compensate pointers for lwzu/stwu.
        subi    r6,r6,2
        b       word_copy_cont

word_copy:
        subi    r4,r4,4         #Compensate pointers for lwzu/stwu.
        subi    r6,r6,4
word_copy_cont:
	andi.	r7,r5,0xc	#Extract extra word accesses.
        srawi   r8,r5,4         #numBytes / 16.
	cmpi	cr1,r7,0x8
	addi	r8,r8,1		#Compensate loop counter.
        mtspr   ctr,r8
        beq     wcopy0		#Branch if no extra word.
        bgt     cr1,wcopy3	#Branch if 3 extra word.
        blt     cr1,wcopy1	#Branch if 1 extra word.
        b       wcopy2		#2 extra word required.

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

        rlwinm. r5,r5,0,30,31   #Calculate number of trailing bytes.
	addi	r4,r4,3		#Move pointers.
	addi	r6,r6,3
        beqlr                   #Done if numBytes = 0.
        b       byte_copy_cont  #Continue with trailing bytes.
byte_copy:
	subi	r4,r4,1		#Move pointers.
	subi	r6,r6,1
byte_copy_cont:
	andi.	r7,r5,0x3	#Extract extra byte accesses.
        srawi   r8,r5,2         #numBytes / 4.
	cmpi	cr1,r7,2
	addi	r8,r8,1		#Compensate loop counter.
        mtspr   ctr,r8
        beq     bcopy0		#Branch if no extra byte.
        bgt     cr1,bcopy3	#Branch if 3 extra byte.
        blt     cr1,bcopy1	#Branch if 1 extra byte.
        b       bcopy2		#2 extra byte required.
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

	
#
#############################################################################
#                    ipcom_in_checksum_memcpy_asm
#############################################################################
# Prototype:	IP_PUBLIC Ip_u32
#               ipcom_in_checksum_memcpy(void *dst, void *src, Ip_size_t len);
# Description:  Copy data and calculate internet checksum.
# Parameters:   
# Returns:      
#
        .globl ipcom_in_checksum_memcpy_asm
ipcom_in_checksum_memcpy_asm:
	xor     r6,r6,r6        # Interpeak always use 0 as initial checksum
        cmpli   cr0,r5,17       #Threshold for byte copy.
	li	r12,0		#Clear byte swap flag.
        xor     r8,r3,r4	#Compare dest and src.
	addic	r5,r5,0		#Make dummy add to clear carry.
        blt-    scpy_byte_copy
        andi.   r7,r8,1         #Check for same byte alignment.
        bne-    scpy_byte_copy	#Byte copy if even/odd alignment.

        andi.   r7,r3,1         #Check for leading odd address.
        beq+    scpy_check_hword_align
        lbz     r7,0(r4)        #Copy one byte to get halfword align.
        subi    r5,r5,1         #Decrement numBytes.
        li	r12,1		#Set flag.
	rlwinm	r8,r6,8,16,23	#Swap bytes for intitial checksum.
        rlwinm	r6,r6,24,24,31
	or	r6,r6,r8
        stb     r7,0(r3)
        adde    r6,r6,r7        #Add to checksum.
        addi    r4,r4,1         #Move pointers.
        addi    r3,r3,1
scpy_check_hword_align:
	
        andi.   r7,r8,2         #Check for same halfword alignment.
        beq+    scpy_word_align	#Word copy if possible.

#	Do halfword copy.

	andi.	r7,r5,0x6	#Extract extra word accesses.
        subi    r4,r4,2         #Compensate pointers for lhzu/sthu.
        subi    r3,r3,2
        srawi   r8,r5,3         #numBytes / 8.
	cmpi	cr1,r7,0x4
	addi	r8,r8,1		#Compensate loop counter.
        mtspr   ctr,r8
        beq     scpy_hwcopy0	#Branch if no extra hword.
        bgt     cr1,scpy_hwcopy3 #Branch if 3 extra hword.
        blt     cr1,scpy_hwcopy1 #Branch if 1 extra hword.
        b       scpy_hwcopy2	#2 extra hword required.
	
scpy_hword_loop:
        lhzu    r7,2(r4)
        sthu    r7,2(r3)
        adde    r6,r6,r7        #Add to checksum.
scpy_hwcopy3:
        lhzu    r8,2(r4)
        sthu    r8,2(r3)
        adde    r6,r6,r8        #Add to checksum.
scpy_hwcopy2:
        lhzu    r7,2(r4)
        sthu    r7,2(r3)
        adde    r6,r6,r7        #Add to checksum.
scpy_hwcopy1:
        lhzu    r8,2(r4)
        sthu    r8,2(r3)
        adde    r6,r6,r8        #Add to checksum.
scpy_hwcopy0:
        bc      16,0,scpy_hword_loop

	rlwinm.	r5,r5,0,31,31	#Check for trailing byte.
        beq+	scpy_end	#Done if numBytes = 0.
	addi	r4,r4,1		#Move pointers.
	addi	r3,r3,1
        b       scpy_trailing_bytes #Continue with trailing bytes.

#	Do word copy.

scpy_word_align:
        andi.   r7,r3,2         #Check for leading halfword address.
        beq-    scpy_word_copy
        lhz     r7,0(r4)        #Copy halfword to get word align.
        subi    r5,r5,2         #Decrement numBytes.
        sth     r7,0(r3)
        adde    r6,r6,r7        #Add to checksum.
        subi    r4,r4,2         #Compensate pointers for lwzu/stwu.
        subi    r3,r3,2
        b       scpy_word_copy_cont

scpy_word_copy:
        subi    r4,r4,4         #Compensate pointers for lwzu/stwu.
        subi    r3,r3,4
scpy_word_copy_cont:
	andi.	r7,r5,0xc	#Extract extra word accesses.
        srawi   r8,r5,4         #numBytes / 16.
	cmpi	cr1,r7,0x8
	addi	r8,r8,1		#Compensate loop counter.
        mtspr   ctr,r8
        beq     scpy_wcopy0	#Branch if no extra word.
        bgt     cr1,scpy_wcopy3	#Branch if 3 extra word.
        blt     cr1,scpy_wcopy1	#Branch if 1 extra word.
        b       scpy_wcopy2	#2 extra word required.

scpy_word_loop:
        lwzu    r7,4(r4)
        stwu    r7,4(r3)
        adde    r6,r6,r7        #Add to checksum.
scpy_wcopy3:
        lwzu    r8,4(r4)
        stwu    r8,4(r3)
        adde    r6,r6,r8        #Add to checksum.
scpy_wcopy2:
        lwzu    r7,4(r4)
        stwu    r7,4(r3)
        adde    r6,r6,r7        #Add to checksum.
scpy_wcopy1:
        lwzu    r8,4(r4)
        stwu    r8,4(r3)
        adde    r6,r6,r8        #Add to checksum.
scpy_wcopy0:
        bc      16,0,scpy_word_loop

        rlwinm. r5,r5,0,30,31   #Calculate number of trailing bytes.
	addi	r4,r4,3		#Move pointers.
	addi	r3,r3,3
        beq	scpy_end	#Done if numBytes = 0.
        b       scpy_trailing_bytes #Continue with trailing bytes.

#	Byte copy, bytes are not swaped.
#	Four bytes for each loop, trailing bytes are copied later.
scpy_byte_copy:
	subi	r4,r4,1		#Move pointers.
	subi	r3,r3,1
        srawi   r8,r5,2         #numBytes / 4.
	rlwinm	r5,r5,0,30,31	#Calculate number of trailing bytes.
	addi	r8,r8,1		#Compensate loop counter.
        mtspr   ctr,r8
        b       scpy_bcopy0
scpy_byte_loop:
	lbzu	r7,1(r4)
        rlwinm  r8,r7,8,16,23	#Shift left 8 times.
	stbu	r7,1(r3)
        adde    r6,r6,r8        #Add to checksum.
scpy_bcopy3:
	lbzu	r8,1(r4)
	stbu	r8,1(r3)
        adde    r6,r6,r8        #Add to checksum.
scpy_bcopy2:
	lbzu	r7,1(r4)
        rlwinm  r8,r7,8,16,23	#Shift left 8 times.
	stbu	r7,1(r3)
        adde    r6,r6,r8        #Add to checksum.
scpy_bcopy1:
	lbzu	r8,1(r4)
	stbu	r8,1(r3)
        adde    r6,r6,r8        #Add to checksum.
scpy_bcopy0:
        bc      16,0,scpy_byte_loop

#	Copy trailing bytes, byte swap is checked.
scpy_trailing_bytes:
	mr	r9,r12		#Copy swap flag.
trailing_loop:
	cmpli	cr0,r5,0	#Test loop counter.
	subi	r5,r5,1
	beq-	scpy_trailing_end
	cmpli	cr0,r9,0
	lbzu	r7,1(r4)
	stbu	r7,1(r3)
	bne	no_shift
        rlwinm  r7,r7,8,16,23	#Shift left 8 times.
no_shift:
	xori	r9,r9,1		#Toggle flag.
        adde    r6,r6,r7        #Add to checksum.
	b	trailing_loop
scpy_trailing_end:
	
scpy_end:
	cmpli	cr0,r12,0	#Test swap flag.
	
#       Convert checksum from 32 bit to 16 bit.

        rlwinm  r7,r6,16,16,31  #Extract high part of checksum.
        rlwinm  r6,r6,0,16,31   #Truncate high part of checksum.
        adde    r6,r6,r7        #High part + low part + carry.
        rlwinm  r7,r6,16,16,31  #Extract carry from checksum.
        rlwinm  r6,r6,0,16,31   #Truncate high part of checksum.
        add     r3,r6,r7        #Add carry to low part.

#	Check if bytes has been swaped, swap if needed.
	beqlr			#Return if no swap.
	rlwinm	r4,r3,24,24,31	#Swap bytes.
	rlwinm	r3,r3,8,16,23
	or	r3,r3,r4
        blr

        .end


#
#############################################################################
#                       END OF FILE
#############################################################################
# 
