#############################################################################
#			INTERPEAK MAKEFILE
#
#     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_vxworks_config.mk,v $ $Revision: 1.25 $
#     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/gmake/ipcom_vxworks_config.mk,v $
#     $Author: kenneth $
#     $State: Exp $ $Locker:  $
#
#     INTERPEAK_COPYRIGHT_STRING
#
#############################################################################

#############################################################################
# GENERAL
#############################################################################

# Define target
ifneq ($(WR_IPTARGET),)
IPTARGET = $(WR_IPTARGET)
else
IPTARGET = simlinux
endif

# Define VxWorks version
ifneq ($(WR_IPCOM_VERSION),)
IPVER = $(WR_IPCOM_VERSION)
else
IPVER = 6_7
endif

IPINCL_SYM_TBL = yes
IP_MAP_FILE = yes

# Define board
ifneq ($(WIND_IPNET),true)

ifeq ($(IPTARGET),mips)
# Examples: malta4kc_mips32sf, malta4kec_mips32sf, malta4kec_mips32sfle
IPBOARD = malta4kc_mips32sf
endif

ifeq ($(IPTARGET),simlinux)
IPBOARD = linux
endif

ifeq ($(IPTARGET),simpc)
IPBOARD = simpc
#IPBOARD = simpc-ipnet
endif

ifeq ($(IPTARGET),pentium)
IPBOARD = pcPentium3-IP
#IPBOARD = pcPentium3
endif

ifeq ($(IPTARGET),cf)
IPBOARD = m54x5evb
endif

ifeq ($(IPTARGET),ppc)
IPBOARD = cds85xx
#IPBOARD = ep8260_bsp
#IPBOARD = wrPpmc74xx
#IPBOARD = wrSbc8560-IP
#IPBOARD = wrSbc8260-IP
#IPBOARD = wrSbc8560-v4-v6-min
IPARCH = powerpc
endif

endif

ifeq ($(IPTARGET),ppc)
IPARCH = powerpc
endif

ifeq ($(IPTARGET),arm)
IPARCH = arm
endif

ifneq ($(IPBOARD),)
IPDEFINE += -DIPBSPBUILD
endif


#############################################################################
# END OF IPCOM_VXWORKS_CONFIG.MK
#############################################################################

