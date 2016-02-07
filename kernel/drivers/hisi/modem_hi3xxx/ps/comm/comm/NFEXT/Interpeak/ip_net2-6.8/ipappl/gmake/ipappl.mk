#############################################################################
#			      IPAPPL.MK
#
#     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipappl.mk,v $ $Revision: 1.8 $
#     $Source: /home/interpeak/CVSRoot/ipappl/gmake/ipappl.mk,v $
#     $Author: ulf $ $Date: 2007-10-11 17:08:55 $
#     $State: Exp $ $Locker:  $
#
#     INTERPEAK_COPYRIGHT_STRING
#
#############################################################################

ifneq ($(IPPORT),las)
ifneq ($(IPPORT),lkm)

#############################################################################
# PRODUCT
###########################################################################

IPPROD ?= ipappl


#############################################################################
# DEFINE
###########################################################################

# start DHCP client
IPDEFINE	+= -DIPDHCPC

ifneq ($(findstring -basic,$(OPT)),-basic)
# start DNS client
IPDEFINE	+= -DIPDNSC

# start TFTP client
IPDEFINE 	+= -DIPTFTPC

# start TFTP server
IPDEFINE	+= -DIPTFTPS

# start FTP client
IPDEFINE 	+= -DIPFTPC

# start FTP server
IPDEFINE	+= -DIPFTPS
endif   # ifeq ($(findstring -basic,$(OPT)),-basic)

#############################################################################
# INCLUDE
###########################################################################

IPINCLUDE += -I$(IPAPPL_ROOT)/config
IPINCLUDE += -I$(IPAPPL_ROOT)/include

IPLIBINCLUDE += -I$(IPAPPL_ROOT)/src


#############################################################################
# OBJECTS
###########################################################################

# Interpeak TFTP server and client
ifneq ($(findstring -basic,$(OPT)),-basic)
IPLIBOBJECTS_C	+= iptftps_config.o
IPLIBOBJECTS_C	+= iptftpc_config.o
IPLIBOBJECTS	+= iptftp_common.o
IPLIBOBJECTS	+= iptftps.o
IPLIBOBJECTS	+= iptftpc.o
IPLIBOBJECTS	+= iptftpc_cmd_tftp.o

# Interpeak FTP server and client
IPLIBOBJECTS_C	+= ipftps_config.o
IPLIBOBJECTS_C	+= ipftpc_config.o
IPLIBOBJECTS_C	+= ipftps_example.o
IPLIBOBJECTS	+= ipftps.o
IPLIBOBJECTS	+= ipftpc.o
IPLIBOBJECTS	+= ipftpc_cmd_ftp.o

# Interpeak DNS client
IPLIBOBJECTS_C	+= ipdnsc_config.o
IPLIBOBJECTS	+= ipdnsc.o
IPLIBOBJECTS	+= ipdnsc_api.o
IPLIBOBJECTS	+= ipdnsc_cmd_nslookup.o
endif   # ifeq ($(findstring -basic,$(OPT)),-basic)

# Interpeak DHCP client
IPLIBOBJECTS_C	+= ipdhcpc_config.o
IPLIBOBJECTS_C	+= ipdhcpc_example.o
IPLIBOBJECTS	+= ipdhcpc.o

# Compiles the xxx_config.o if the $SKIP_CONFIG macro is either not defined
# or set to anything other than true.
ifneq ($(SKIP_CONFIG),true)
IPLIBOBJECTS    += $(IPLIBOBJECTS_C)
endif


#############################################################################
# VPATH
###########################################################################

IPSRCDIRS += $(IPAPPL_ROOT)/src $(IPAPPL_ROOT)/config


#############################################################################
# LIB
###########################################################################

IPLIBS += $(IPLIBROOT)/libipappl.a

endif
endif

###########################################################################
# END OF IPAPPL.MK
###########################################################################
