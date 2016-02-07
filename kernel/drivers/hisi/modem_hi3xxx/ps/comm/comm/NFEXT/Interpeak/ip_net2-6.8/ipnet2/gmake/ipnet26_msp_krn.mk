#############################################################################
#			      IPNET2.MK
#
#     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet26_msp_krn.mk,v $ $Revision: 1.1 $
#     $Source: /home/interpeak/CVSRoot/ipnet2/gmake/ipnet26_msp_krn.mk,v $
#     $Author: rboden $ $Date: 2008-08-11 12:04:27 $
#     $State: Exp $ $Locker:  $
#
#     INTERPEAK_COPYRIGHT_STRING
#
#############################################################################


#############################################################################
# DEFINE
###########################################################################


#############################################################################
# OBJECTS
###########################################################################

# Markets Specific Platform files

# RFC 3971, 3972, Secure NDP, Cryptographically Generated Addresses
IPLIBOBJECTS += ipnet_cga.o
IPLIBOBJECTS += ipnet_send.o


###########################################################################
# END OF IPNET2_MSP_KRN.MK
###########################################################################
