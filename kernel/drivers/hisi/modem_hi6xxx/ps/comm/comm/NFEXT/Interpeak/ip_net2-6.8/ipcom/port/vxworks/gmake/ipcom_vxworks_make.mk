#############################################################################
#			INTERPEAK MAKEFILE
#
#     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_vxworks_make.mk,v $ $Revision: 1.71.24.2 $
#     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/gmake/ipcom_vxworks_make.mk,v $
#     $Author: zan $
#     $State: Exp $ $Locker:  $
#
#     INTERPEAK_COPYRIGHT_STRING
#
#############################################################################

#############################################################################
# DEFINE
#############################################################################

# Assign IP_PORT_VXWORKS a numerical value to be used in #if (55, 60, etc.)
ifneq ($(IPGMAKE),yes)
IPDEFINE += -DIP_PORT_VXWORKS=$(subst _,,$(IPVER))
endif

#############################################################################
# ENDIAN
#############################################################################

# Includes the definition of TOOL
ifneq ($(IPBOARD),)
-include $(WIND_BASE)/target/config/$(IPBOARD)/Makefile

MACH_EXTRA = $(filter-out ipcom_vxworks.o,$(MACH_EXTRA))
endif

ifeq ($(TGT_DIR),)
TGT_DIR = $(WIND_BASE)/target
endif

CC_WARNINGS     = $(CC_WARNINGS_LOW)
IPCOMPILER 	= $(TOOL)

# looking for a better solution
ifneq ($(IPGMAKE),yes)
include $(TGT_DIR)/h/make/defs.$(WIND_HOST_TYPE)
include $(TGT_DIR)/h/make/defs.default
include $(TGT_DIR)/h/make/defs.option
include $(TGT_DIR)/h/make/defs.options
include $(TGT_DIR)/h/tool/$(TOOL_FAMILY)/make.$(CPU)$(TOOL)
include $(VSB_MAKE_CONFIG_FILE)
endif

# Set endianness
ifeq ($(IPTARGET),ppc)
IPDEFINE	+= -DIP_BIG_ENDIAN
endif
ifeq ($(IPTARGET),cf)
IPDEFINE	+= -DIP_BIG_ENDIAN
endif
ifeq ($(IPTARGET),i386)
IPDEFINE	+= -DIP_LITTLE_ENDIAN
endif
ifeq ($(IPTARGET),mips)
ifeq  ($(_WRS_CONFIG_ENDIAN),little)
IPDEFINE        += -DIP_LITTLE_ENDIAN
else
IPDEFINE        += -DIP_BIG_ENDIAN
endif
endif
ifeq ($(IPTARGET),arm)
ifeq  ($(_WRS_CONFIG_ENDIAN),big)
IPDEFINE        += -DIP_BIG_ENDIAN
else
ifeq  ($(_WRS_CONFIG_ENDIAN),be8)
IPDEFINE        += -DIP_BIG_ENDIAN
else
IPDEFINE        += -DIP_LITTLE_ENDIAN
endif
endif
endif
ifeq ($(IPTARGET),sh)
ifeq  ($(_WRS_CONFIG_ENDIAN),little)
IPDEFINE        += -DIP_LITTLE_ENDIAN
else
IPDEFINE        += -DIP_BIG_ENDIAN
endif
endif
ifeq ($(IPTARGET),sparc)
IPDEFINE	+= -DIP_BIG_ENDIAN
endif
ifeq ($(IPTARGET),68k)
IPDEFINE	+= -DIP_BIG_ENDIAN
endif
ifeq ($(IPTARGET),simso)
IPDEFINE	+= -DIP_BIG_ENDIAN
endif
ifeq ($(IPTARGET),simpc)
IPDEFINE	+= -DIP_LITTLE_ENDIAN
endif
ifeq ($(IPTARGET),pentium)
IPDEFINE	+= -DIP_LITTLE_ENDIAN
endif
ifeq ($(IPTARGET),simlinux)
IPDEFINE	+= -DIP_LITTLE_ENDIAN
endif
ifeq ($(IPTARGET),cf)
IPDEFINE	+= -DIP_BIG_ENDIAN
endif


#############################################################################
# INCLUDE
#############################################################################

IPINCLUDE 	+= -I$(IPCOM_ROOT)/port/$(IPPORT)/config
IPINCLUDE 	+= -I$(IPCOM_ROOT)/port/$(IPPORT)/include


#############################################################################
# SYMBOL TABLE
#############################################################################

ifeq ($(IPINCL_SYM_TBL),yes)
IPEXTRA_DEFINE	+= -DSTANDALONE_NET
IPVXBINARY	= vxWorks.st
else
IPVXBINARY	= vxWorks
endif


#############################################################################
# IPCOM INITIALIZATION
#############################################################################

ifeq ($(findstring IPCOM_CCIPERFORMANCE_TEST,$(IPEXTRACFLAGS)),IPCOM_CCIPERFORMANCE_TEST)
IPEXTRA_DEFINE	+= -DINCLUDE_USER_APPL -DUSER_APPL_INIT='do {extern int ipcom_create(void); ipcom_create(); usrCciInit(); } while(0)'
else
ifeq ($(findstring IPCOM_CCIVALIDATE_TEST,$(IPEXTRACFLAGS)),IPCOM_CCIVALIDATE_TEST)
IPEXTRA_DEFINE	+= -DINCLUDE_USER_APPL -DUSER_APPL_INIT='do {extern int ipcom_create(void); ipcom_create(); usrCciInit(); } while(0)'
else
IPEXTRA_DEFINE	+= -DINCLUDE_USER_APPL -DUSER_APPL_INIT='do {extern int ipcom_create(void); ipcom_create(); } while(0)'
endif
endif

ifeq ($(findstring IPNET,$(filter-out -DIPNETPERF -DIPNET_MSP_DISTRIBUTION,$(IPDEFINE))),IPNET)
IPEXTRA_DEFINE	+= -DIPNET
endif

ifeq ($(findstring IPLITE,$(IPDEFINE)),IPLITE)
IPEXTRA_DEFINE	+= -DIPLITE
endif

ifeq ($(findstring IPTCP,$(IPDEFINE)),IPTCP)
IPEXTRA_DEFINE	+= -DIPTCP
endif

# Add Windview facilities (requires RAMDISK)
ifeq ($(findstring IPCOM_WV_INSTRUMENTATION_TEST,$(IPEXTRACFLAGS)),IPCOM_WV_INSTRUMENTATION_TEST)
IPEXTRA_DEFINE += -DINCLUDE_XBD
IPEXTRA_DEFINE += -DINCLUDE_XBD_PART_LIB
IPEXTRA_DEFINE += -DINCLUDE_XBD_BLK_DEV
IPEXTRA_DEFINE += -DINCLUDE_XBD_RAMDRV
IPEXTRA_DEFINE += -DINCLUDE_DOSFS_MAIN
IPEXTRA_DEFINE += -DINCLUDE_DOSFS_FAT
IPEXTRA_DEFINE += -DINCLUDE_DOSFS_DIR_VFAT
IPEXTRA_DEFINE += -DINCLUDE_DISK_UTIL
IPEXTRA_DEFINE += -DINCLUDE_WVNETD
IPEXTRA_DEFINE += -DINCLUDE_WINDVIEW
IPEXTRA_DEFINE += -DINCLUDE_WINDVIEW_CLASS
IPEXTRA_DEFINE += -DINCLUDE_WVUPLOAD_ALL
IPEXTRA_DEFINE += -DINCLUDE_TRIGGERING
IPEXTRA_DEFINE += -DINCLUDE_RBUFF
IPEXTRA_DEFINE += -DINCLUDE_TIMESTAMP
IPEXTRA_DEFINE += -DINCLUDE_SYS_TIMESTAMP
endif



ifeq ($(findstring IPCOM_CCIPERFORMANCE_TEST,$(IPEXTRACFLAGS)),IPCOM_CCIPERFORMANCE_TEST)
IPEXTRA_DEFINE += -DINCLUDE_CCI_COREAPI
IPEXTRA_DEFINE += -DINCLUDE_CCI_DEFAULT_PROVIDER
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_AES
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_AESKW
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_AES_CMAC
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_BLOWFISH
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_CAST
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_DES
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_CRC32
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_MD2
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_MD4
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_MD5
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_RIP128
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_RIP160
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_SHA1
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_SHA224
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_SHA256
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_SHA384
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_SHA512
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_AESXCBC
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_MD4
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_MD5
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_RIP128
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_RIP160
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_SHA1
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_SHA224
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_SHA256
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_SHA384
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_SHA512
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_INTEGER
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_IPSEC_SINGLEPASS
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_NULL
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_PRNG
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_PUBLICKEY_RSA
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_RC4
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_RC4TKIP
IPEXTRA_DEFINE += -DINCLUDE_CCI_PERFORMANCE
IPEXTRA_DEFINE += -DINCLUDE_OBJ_INFO
IPEXTRA_DEFINE += -DINCLUDE_SEM_EVENTS
IPEXTRA_DEFINE += -DINCLUDE_SEM_EXCHANGE
IPEXTRA_DEFINE += -DINCLUDE_SEM_INFO
IPEXTRA_DEFINE += -DINCLUDE_SEM_SHOW
EXTRA_INCLUDE += -I$(WIND_BASE)/target/config/comps/src/net
EXTRA_INCLUDE += -I$(WIND_BASE)/target/h
endif

ifeq ($(findstring IPCOM_CCIVALIDATE_TEST,$(IPEXTRACFLAGS)),IPCOM_CCIVALIDATE_TEST)
IPEXTRA_DEFINE += -DINCLUDE_CCI_COREAPI
IPEXTRA_DEFINE += -DINCLUDE_CCI_DEFAULT_PROVIDER
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_AES
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_AESKW
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_AES_CMAC
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_BLOWFISH
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_CAST
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_DES
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_CRC32
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_MD2
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_MD4
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_MD5
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_RIP128
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_RIP160
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_SHA1
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_SHA224
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_SHA256
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_SHA384
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HASH_SHA512
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_AESXCBC
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_MD4
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_MD5
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_RIP128
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_RIP160
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_SHA1
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_SHA224
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_SHA256
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_SHA384
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_HMAC_SHA512
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_INTEGER
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_IPSEC_SINGLEPASS
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_NULL
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_PRNG
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_PUBLICKEY_RSA
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_RC4
IPEXTRA_DEFINE += -DINCLUDE_CCI_IMPORT_RC4TKIP
IPEXTRA_DEFINE += -DINCLUDE_CCI_VALIDATE
IPEXTRA_DEFINE += -DINCLUDE_OBJ_INFO
IPEXTRA_DEFINE += -DINCLUDE_SEM_EVENTS
IPEXTRA_DEFINE += -DINCLUDE_SEM_EXCHANGE
IPEXTRA_DEFINE += -DINCLUDE_SEM_INFO
IPEXTRA_DEFINE += -DINCLUDE_SEM_SHOW
EXTRA_INCLUDE += -I$(WIND_BASE)/target/config/comps/src/net
EXTRA_INCLUDE += -I$(WIND_BASE)/target/h
endif


#############################################################################
# COMPILE
#############################################################################

ifeq ($(TOOL_FAMILY),)
ifeq  ($(findstring gnu,$(TOOL)),gnu)
TOOL_FAMILY	= gnu
else
ifeq  ($(findstring diab,$(TOOL)),diab)
TOOL_FAMILY	= diab
endif
endif
endif

ifeq ($(WIND_IPNET),true)
CC_DEFINES += $(OPTION_DEFINE_MACRO)_WRS_LIB_BUILD
endif

IPCC 		= $(CC)
IPCPP		= $(IPCC) -E
IPAS		= $(CC)
IPAR 		= $(AR)
IPRANLIB	= $(RANLIB)
IPLD		= $(LD)

#ifeq ($(IPNGC),yes)
#IPCFLAGS 	+= -nostdinc
#endif
IPCFLAGS 	+= -c $(CFLAGS) $(CC_SDA_FLAGS)
IPASFLAGS	+= -c $(CFLAGS_AS)
IPARFLAGS 	= r
IPINCLDFLAGS    = -r -nostartfiles -nostdlib

ifeq ($(IP_MAP_FILE),yes)
ifeq ($(IPVER),5_4)
IP_MAP_FILE_FLAGS	= \"-Wl,-Map $(IPOBJDIR)/mapfile.txt\"
else
# 5.5 and 6.0 compilers cannot convey linker flags that are longer than one character, output to stdout instead
IP_MAP_FILE_FLAGS = -Wl,-X,--print-map,--cref,-Map,$(IPOBJDIR)/mapfile.txt
endif  # version 5_4
endif

IP_LD_PARTIAL	= $(LD_PARTIAL) $(IP_MAP_FILE_FLAGS)

EXTRA_INCLUDE	+= $(IPLIBINC)
EXTRA_INCLUDE	+= -I$(IPCOM_ROOT)/port/vxworks/$(IPVER)/update
#To get cci.h into ipcom include path for ipcom_cmd_ccivalidate
EXTRA_INCLUDE   += -I $(WIND_BASE)/target/h/wrn/cci

IPINCLUDE 	+= -I$(IPCOM_ROOT)/port/vxworks/$(IPVER)/update
ADDED_MODULES	+= $(IPOBJS) $(IPLIBS) $(IPLIBS) $(IPLIBS) $(IPLIBS)

#ifeq ($(IPNGC),yes)
#ADDED_MODULES	+= -lm
#endif

# Use the "make" command that is put in the path by "wrenv.sh"
IPVXMAKE	= make
ifneq ($(IPBOARD),)
IPBSPDIR	= $(subst \,/,$(WIND_BASE)/target/config/$(IPBOARD))
endif


#############################################################################
# IPBUILD
#############################################################################
# IPBUILD = debug | speed | debugspeed | size
# The normal build includes -O2 optimization. This optimization flag is
# removed for some build types below.

# General flags
ifeq ($(findstring diab,$(TOOL)),diab)
IPCFLAGS        := $(subst -Xlint,,$(IPCFLAGS))
IPCFLAGS        += -ei1546 -ei4167 -ei4513 -Xdialect-c89
else
IPCFLAGS        := $(subst -fvolatile,,$(IPCFLAGS))
ifneq ($(IPVER),5_4)
IPCFLAGS        += -fno-strict-aliasing
endif
endif

ifeq ($(IPBUILD),debug)
IPCFLAGS	+= -g
ifeq ($(findstring diab,$(TOOL)),diab)
IPCFLAGS	:= $(subst -XO,,$(IPCFLAGS))
else
IPCFLAGS	:= $(subst -O2,,$(IPCFLAGS))
endif
endif

ifeq ($(IPBUILD),debugspeed)
IPCFLAGS	+= -g
endif

ifeq ($(IPBUILD),speed)
ifneq ($(findstring diab,$(TOOL)),diab)
IPCFLAGS := $(subst -fvolatile,,$(IPCFLAGS))
endif
endif

# The vxWorks 5.4 compiler does not support -Os
ifeq ($(IPBUILD),size)
ifneq ($(IPVER),5_4)
IPCFLAGS	+= -Os
IPCFLAGS	= $(subst -O2,,$(IPCFLAGS))
endif
endif

ifeq ($(IPGENDEPEND),yes)
IPCFLAGS := $(IPCFLAGS) $(OPTION_DEPEND_GEN_D)

IPOBJS_NODEP	= $(foreach file, $(IPNODEPENDOBJS), $(IPOBJDIR)/$(file))

IPDEP_OBJS = $(filter-out $(IPOBJS_NODEP), $(IPLIBOBJS))

IPEXISTING_OBJS = $(foreach obj, $(IPDEP_OBJS), $(wildcard $(obj)))

IPDEP_FILES     = $(filter %.d , $(IPEXISTING_OBJS:.o=.d))

IPEXISTING_DEP_FILES	= $(foreach obj, $(IPDEP_FILES), $(wildcard $(obj)))

IPMISSING_DEP = $(filter-out $(IPEXISTING_DEP_FILES), $(IPDEP_FILES))

ifneq ($(strip $(IPMISSING_DEP)),)
$(warning warning: missing dependency files $(IPMISSING_DEP))
$(warning warning: run make depend to regenerate)
endif

ifneq ($(strip $(IPEXISTING_DEP_FILES)),)
include $(IPEXISTING_DEP_FILES)
endif
endif


#############################################################################
# PORT USAGE
#############################################################################

pusage:
	@$(IPECHO) "Port configuration:"
	@$(IPECHO) "  WIND_BASE      = $(WIND_BASE)"
	@$(IPECHO) "  IPBOARD        = $(IPBOARD)"
	@$(IPECHO) "  IPINCL_SYM_TBL = $(IPINCL_SYM_TBL)"
	@$(IPECHO) "  EXECUTABLE FILE = $(WIND_BASE)/target/config/$(IPBOARD)/$(IPVXBINARY)"


#############################################################################
# PORT CLEAN
#############################################################################

pcleanobj:

ifneq ($(IPBOARD),)
pcleanall:
	$(IPVXMAKE) -C $(IPBSPDIR) -o depend.$(BSP_NAME) clean
endif


#############################################################################
# ALL
#############################################################################

ifneq ($(IPSMP),)
# Configure SMP make switches for BSP builds
IPSMP_SWITCHES	= VXBUILD=SMP
ifeq ($(IPGMAKE),yes)
IPCFLAGS	+= -D_WRS_VX_SMP -D_WRS_CONFIG_SMP
endif
else
IPSMP_SWITCHES 	=
endif

.PHONY:	vxworks all

ifneq ($(IPBOARD),)
vxworks:
ifneq ($(IP_SKIP_BSP_CLEAN),yes)
	$(IPVXMAKE) -C $(IPBSPDIR) -o depend.$(BSP_NAME) clean
endif
	$(IPVXMAKE) -C $(IPBSPDIR) $(IPSMP_SWITCHES) \
	EXTRA_INCLUDE="$(EXTRA_INCLUDE)" EXTRA_DEFINE="$(EXTRA_DEFINE) $(IPEXTRA_DEFINE)" \
	ADDED_MODULES="$(ADDED_MODULES)" LD_PARTIAL="$(IP_LD_PARTIAL)" IPOBJDIR="$(IPOBJDIR)" \
	-o depend.$(BSP_NAME) $(IPVXBINARY)
endif

ifneq ($(IPBOARD),)
all:	liball objall vxworks
else
all:	liball objall
endif

.PHONY: depend.$(BSP_NAME)

depend.$(BSP_NAME):
	@$(ECHO) ""


#############################################################################
# END OF IPCOM_VXWORKS_MAKE.MK
#############################################################################
