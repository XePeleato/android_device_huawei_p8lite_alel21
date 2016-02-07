#############################################################################
#			INTERPEAK MAKEFILE
#
#     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_make.mk,v $ $Revision: 1.65 $
#     $Source: /home/interpeak/CVSRoot/ipcom/gmake/ipcom_make.mk,v $
#     $Author: ulf $
#     $State: Exp $ $Locker:  $
#
#     INTERPEAK_COPYRIGHT_STRING
#
#############################################################################

#############################################################################
# MACROS
#############################################################################

# library root.
ifeq ($(WIND_IPNET),true)
IPLIBROOT = $(IPOBJDIR)/..
else
IPLIBROOT = $(IPOBJDIR)/lib
endif


#############################################################################
# COMPILATION DEFAULTS
#############################################################################

IPLD	    = $(IPCC)
ifeq ("$(IPCOMPILER)","shc")
IPCCOUT 	= -object=$@
IPAROUT 	= -output=$@
else
ifeq ("$(IPCOMPILER)","MSVC")
IPCCOUT 	= -Fo$@
IPAROUT 	= /out:$@
else
IPCCOUT     = -o $@
IPAROUT     = $@
endif
endif
IPARTAIL    =
IPLIBPREFIX = lib
IPLIBSUFFIX = .a
IPBINSUFFIX = .elf


#############################################################################
# HOST COMMANDS
#############################################################################

ifndef IPHOST
UNAME := $(shell uname)
ifeq ($(UNAME),Linux)
IPHOST = unix
else
ifeq ($(UNAME),SunOS)
IPHOST = unix
else
ifeq ($(UNAME),LynxOS)
IPHOST = LynxOS
else
IPHOST = win32
endif
endif
endif
endif


ifeq ($(IPVERBOSE),yes)
  IPVERB =
else
  IPVERB = @
endif
ifeq ($(IPHOST),LynxOS)
IPMKDIR	 = mkdir -pf
else
IPMKDIR	 = mkdir -p
endif

IPCD	 = cd
IPCP	 = cp
IPECHO   = echo
IPRM     = rm -f
IPRMDIR  = rm -rf
IPTEST   = test -f


#############################################################################
# MACROS
#############################################################################

IPDEFINE ?=
IPINCLUDE ?=

ifneq ($(FEATURE_SET),)
ifneq ($(FEATURE_SET),gpp)
IPDEFINE += -DIPNET_MSP_DISTRIBUTION
endif
endif

ifeq ($(findstring lib,$(IPTARGETS)),lib)
ifeq ($(WIND_IPNET),true)
IPLIBFILE = $(IPLIBROOT)/$(IPLIBPREFIX)$(IPPROD)$(OPT)$(IPLIBSUFFIX)
else
IPLIBFILE := $(IPLIBROOT)/$(IPLIBPREFIX)$(IPPROD)$(IPLIBSUFFIX)
endif
endif
IPLIBOBJS := $(addprefix $(IPOBJDIR)/$(IPPROD)/,$(IPLIBOBJECTS))
IPARLIBOBJS := $(IPLIBOBJECTS)
IPARLIBDIR := $(IPOBJDIR)/$(IPPROD)
IPLINTS   := $(addprefix $(IPOBJDIR)/,$(IPLIBOBJECTS))
IPBINARY   = $(IPOBJDIR)/ipout$(IPBINSUFFIX)
IPPRODOBJ := $(addprefix $(IPOBJDIR)/,$(IPOBJECTS))
IPOBJECTS  =
IPCOMMA    = ,
ifeq ("$(IPCOMPILER)","shc")
ifneq ($(IPLIBDEFINE),)
IPLIBDEF  := $(patsubst -D%,$(IPCOMMA)%,$(IPLIBDEFINE))
endif
ifneq ($(IPLIBINCLUDE),)
IPLIBINC  := $(patsubst -I%,$(IPCOMMA)%,$(IPLIBINCLUDE))
endif
else
IPLIBFLAG := $(IPLIBCFLAGS)
IPLIBDEF  := $(IPLIBDEFINE)
IPLIBINC  := $(IPLIBINCLUDE)
endif
IPVPATH	  := $(IPSRCDIRS)
IPASVPATH := $(IPASDIRS)

ifeq ($(IPPORT),lkm)
IPMAKE 	    = $(MAKE) IPCOMPILER="$(IPCOMPILER)" IPCC="$(IPCOMPILER)"
else
IPMAKE 	    = $(MAKE) IPCOMPILER="$(IPCOMPILER)"
endif


# Export these variables to submakes
export IPCOM_ROOT
export BASE_ROOT
export IPPORT
export IPBUILD
export IPVERBOSE
ifdef IPMAKEFILE
export IPMAKEFILE
endif

ifeq ($(IPTESTENGINE),yes)
IPDEFINE += -DIPTESTENGINE
endif

# Used by build test to exclude IPv4
ifeq ($(IPCOM_USE_INET),no)
IPDEFINE += -DIPCOM_EXCLUDE_INET
endif

# SMP configuration,
ifeq ($(IPSMP),ready)
IPDEFINE += -DIPCOM_SMP_READY
endif
ifeq ($(IPSMP),optimized)
IPDEFINE += -DIPCOM_SMP_OPTIMIZED
endif


#############################################################################
# USAGE
#############################################################################

.PHONY: usage usage1 pusage usage2 usage3

usage: usage1 pusage usage2 usage3

usage1:
	@$(IPECHO) ""
	@$(IPECHO) "This is an Interpeak demo GNU makefile."
	@$(IPECHO) "Please check ipcom_config.mk for configuration options!"
	@$(IPECHO) ""
	@$(IPECHO) "Configuration:"
#	@$(IPECHO) "  IPHOST     = $(IPHOST)"
	@$(IPECHO) "  IPPORT     = $(IPPORT)"
	@$(IPECHO) "  IPBUILD    = $(IPBUILD)"
	@$(IPECHO) "  IPTARGET   = $(IPTARGET)"
ifneq ($(IPARCH),)
	@$(IPECHO) "  IPARCH     = $(IPARCH)"
endif
	@$(IPECHO) "  IPVER      = $(IPVER)"
	@$(IPECHO) "  IPCOMPILER = $(IPCOMPILER)"
	@$(IPECHO) "  IPOBJDIR   = $(IPOBJDIR)"
	@$(IPECHO) "  IPPRODOBJ  = $(IPPRODOBJ)"

usage2:
	@$(IPECHO) "Products:"
ifeq ($(IPWIN32_MAKE_FOR_FIX),yes)
	@$(IPVERB)for %p in ($(IPPRODS)) do \
	if [ -d $$prod ]; then  @$(IPECHO) "  %p"; fi
else
	@$(IPVERB)for prod in $(IPPRODS); do \
	if [ -d $$prod ]; then $(IPECHO) "  $$prod"; fi; \
        done
endif

usage3:
	@$(IPECHO) "Targets:"
	@$(IPECHO) "  all        build all"
	@$(IPECHO) "  cleanall   remove all"
	@$(IPECHO) "  cleanlib   cleanobj + remove library"
	@$(IPECHO) "  cleanobj   remove library objects"
	@$(IPECHO) "  lib        build library"
	@$(IPECHO) "  liball     build all libraries"
	@$(IPECHO) "  new        cleanall + all"


#############################################################################
# IPBUILD
#############################################################################
# IPBUILD = debug | speed | debugspeed | normal | size

ifeq ($(IPBUILD),debug)
IPDEFINE	+= -DIP_DEBUG
IPDEFINE	+= -DIP_ASSERT
endif

ifeq ($(IPBUILD),debugspeed)
IPDEFINE	+= -DIP_DEBUG
IPDEFINE	+= -DIP_ASSERT
endif

ifeq ($(IPBUILD),size)
IPDEFINE	+= -DIP_SIZE
endif


#############################################################################
# lib
#############################################################################

.PHONY:	lib


#############################################################################
# nop
#############################################################################

.PHONY:	nop

nop:


#############################################################################
# PRODUCTS
#############################################################################

-include $(foreach prod,$(IPPRODS),$(prod)/gmake/$(firstword $(subst -,$(empty) $(empty),$(notdir $(prod)))).mk)


#############################################################################
# PORT
#############################################################################

include $(IPCOM_ROOT)/port/$(IPPORT)/gmake/ipcom_$(IPPORT)_make.mk


#############################################################################
# DEFINE/INCLUDE
#############################################################################

# itron compiler only
ifeq ("$(IPCOMPILER)","shc")
ifneq ($(IPDEFINE),)
IPDEF     := -define=IP_COMPILER_SHC $(patsubst -D%,$(IPCOMMA)%,$(IPDEFINE))
endif
ifneq ($(IPINCLUDE),)
IPINC      = $(patsubst -I%,$(IPCOMMA)%,$(IPINCLUDE))
endif
IPCFLAGS  += -include=$(dir $<)$(IPLIBINC)$(IPINC)

# all other compilers
else
IPDEF     := $(IPDEFINE)
IPINC     := $(IPINCLUDE)
ifeq ($(IPPORT),vxworks)
# Avoid including OpenSSL files from the VxWorks distribution
IPCFLAGS  := $(IPINC) $(IPLIBINC) $(IPEXTRACFLAGS) $(IPCFLAGS)
else
IPCFLAGS  += $(IPEXTRACFLAGS) $(IPLIBINC) $(IPINC)
endif
IPASFLAGS += $(IPDEF) $(IPLIBDEF)
endif

# generic
IPCFLAGS  += $(IPDEF) $(IPLIBDEF)
IPCFLAGS  += $(IPLIBFLAG)


#############################################################################
# obj
#############################################################################

IPOBJS     = $(addprefix $(IPOBJDIR)/,$(IPOBJECTS))
IPOBJS	  += $(IPABSOBJECTS)

.PHONY:	obj

obj: dirs $(IPPRODOBJ)


#############################################################################
# DIRS
#############################################################################

.PHONY: dirs pdirs

dirs: $(IPOBJDIR)/$(IPPROD) $(IPLIBROOT) pdirs

$(IPOBJDIR)/$(IPPROD):
	$(IPVERB)$(IPMKDIR) $(IPOBJDIR)
	$(IPVERB)$(IPMKDIR) $(IPOBJDIR)/$(IPPROD)

$(IPLIBROOT):
	$(IPVERB)$(IPMKDIR) $(IPLIBROOT)


#############################################################################
# CLEAN
#############################################################################

.PHONY: cleanobj cleanlib cleanall pcleanobj pcleanall

cleanobj:	pcleanobj
	@$(IPECHO) "+++ Removing $(IPOBJDIR)/$(IPPROD)"
	$(IPVERB)$(IPRMDIR) $(IPOBJDIR)/$(IPPROD)

cleanlib:	cleanobj
	@$(IPECHO) "+++ Removing $(IPLIBFILE)"
	$(IPVERB)$(IPRM) $(IPLIBFILE)

cleanall:	pcleanall
ifeq ($(WIND_IPNET),true)
	@$(IPECHO) "+++ Removing IPNET archives"
	$(IPVERB)$(IPRM) $(IPLIBS)
endif
	@$(IPECHO) "+++ Removing $(IPOBJDIR)"
	$(IPVERB)$(IPRMDIR) $(IPOBJDIR)


#############################################################################
# NEW
#############################################################################

.PHONY:	new

new:	cleanall all


#############################################################################
# BIN
#############################################################################

.PHONY:	bin


#############################################################################
# LIBALL
#############################################################################

.PHONY: liball

liball:
ifeq ($(IPWIN32_MAKE_FOR_FIX),yes)
	$(IPVERB)for %p in ($(IPPRODS)) do \
	$(IPMAKE) -C %p/gmake lib
else
	$(IPVERB)for prod in $(IPPRODS); do \
        if [ -d $$prod ]; then \
		$(IPECHO) "+++ Making library: $$prod"; \
		$(IPMAKE) -C $$prod/gmake lib || exit 1; \
		$(IPECHO) ""; \
	fi; \
	done
endif


#############################################################################
# BINALL
#############################################################################

.PHONY: binall

binall:
ifeq ($(IPWIN32_MAKE_FOR_FIX),yes)
	$(IPVERB)for %p in ($(IPPRODS)) do \
	$(IPMAKE) -C %p/gmake bin
else
	$(IPVERB)for prod in $(IPPRODS); do \
        if [ -d $$prod ]; then \
		$(IPECHO) "+++ Making binaries in: $$prod"; \
		$(IPMAKE) -C $$prod/gmake bin || exit 1; \
		$(IPECHO) ""; \
	fi; \
	done
endif


#############################################################################
# OBJALL
#############################################################################

.PHONY: objall

objall:
ifeq ($(IPWIN32_MAKE_FOR_FIX),yes)
	$(IPVERB)for %p in ($(IPPRODS)) do \
	$(IPMAKE) -C %p/gmake obj
else
	$(IPVERB)for prod in $(IPPRODS); do \
        if [ -d $$prod ]; then \
		$(IPECHO) "+++ Making object: $$prod"; \
		$(IPMAKE) -C $$prod/gmake obj || exit 1;\
		$(IPECHO) ""; \
	fi; \
	done
endif


#############################################################################
# LINT
#############################################################################
# linux or win32 only.

.PHONY:	lint lintall

ifeq ($(IPHOST),unix)
IPLINT	= -$(IPCOM_ROOT)/util/flexelint/bin/flint-linux
IPLINT	+= -I/usr/include
IPLINT	+= -I/usr/include/sys
IPLINT	+= -I/usr/include/linux
#IPLINT  += -"dsize_t=unsigned int"
#test 16bit int: IPLINT  += -si2
else
IPLINT	= -$(IPCOM_ROOT)/util/flexelint/bin/lint-nt.exe
endif
IPLINT	+= $(IPCOM_ROOT)/util/flexelint/lint
IPLINT	+= -I$(IPCOM_ROOT)/util/flexelint

_IPLINTS = $(patsubst %.o,%.l,$(IPLINTS))


lint:	$(_IPLINTS)

lintall:
	$(IPVERB)for prod in $(IPPRODS); do \
        if [ -d $$prod ]; then \
		$(IPECHO) "+++ Linting: $$prod"; \
		$(IPMAKE) -C $$prod/gmake lint || exit 1; \
		$(IPECHO) ""; \
	fi; \
	done


#############################################################################
# DEPENDENCIES
#############################################################################

vpath %.c $(IPVPATH)
vpath %.s $(IPASVPATH)
vpath %.ppc $(IPASVPATH)

LIBS_TOCLEAN =

ifeq ("$(IPCOMPILER)","shc")
IPLIBS     := $(patsubst %,-library=%,$(IPLIBS))
$(IPBINARY):	liball objall
	@$(IPECHO) "+++ Linking $@"
	$(IPLD) -output=$@ $(IPLDFLAGS) $(IPOBJS) $(IPLIBS) $(IPLASTLIBS) $(IPMAPFILE)
else
ifeq ($(IPPORT),mqx)
$(IPBINARY):	liball objall
	@$(IPECHO) "+++ Built $(IPPRODS)"
else
ifeq ($(IPPORT),winnt)
$(IPBINARY):	liball objall
	@$(IPECHO) "+++ Linking $@"
	$(IPLD) -out:$@ $(IPLDFLAGS) $(IPOBJS) $(IPLIBS) $(IPLIBS) $(IPLIBS) $(IPLIBS) $(IPLASTLIBS) $(IPMAPFILE)
else
ifneq ($(OPT),)
	LIBS_TOCLEAN += $(foreach ELIB,$(IPLIBS),$(subst .a,,$(ELIB))$(OPT).a)
	IPLIBS := $(LIBS_TOCLEAN)
else
$(IPBINARY):	liball objall
	@$(IPECHO) "+++ Linking $@"
	$(IPLD) -o $@ $(IPLDFLAGS) $(IPOBJS) $(IPLIBS) $(IPLIBS) $(IPLIBS) $(IPLIBS) $(IPLASTLIBS) $(IPMAPFILE)
endif
endif
endif
endif

$(IPOBJDIR)/%.l: %.c
	@$(IPECHO) "+++ Linting $<"
	$(IPVERB)$(IPLINT) $(IPDEF) $(IPLIBDEF) $(IPINC) $(IPLIBINC) $<
	@$(IPECHO) ""

$(IPOBJDIR)/%.o: %.c
	@$(IPECHO) "+++ Compiling $<"
	$(IPVERB)$(IPCC) $(IPCFLAGS) $(IPCCOUT) $<

$(IPOBJDIR)/$(IPPROD)/%.o: %.c
	@$(IPECHO) "+++ Compiling $<"
	$(IPVERB)$(IPCC) $(IPCFLAGS) $(IPCCOUT) $<

$(IPOBJDIR)/%.o: %.s
	@$(IPECHO) "+++ Assembling $<"
	$(IPVERB)$(IPAS) $(IPASFLAGS) $(IPCCOUT) $<

$(IPOBJDIR)/$(IPPROD)/%.o: %.s
	@$(IPECHO) "+++ Assembling $<"
	$(IPVERB)$(IPAS) $(IPASFLAGS) $(IPCCOUT) $<

$(IPOBJDIR)/$(IPPROD)/%.o: %.ppc
	@$(IPECHO) "+++ Assembling $<"
	$(IPVERB)$(IPAS) $(IPASFLAGS) $(IPCCOUT) $<

$(IPLIBFILE): $(IPLIBOBJS)
	@$(IPECHO) "+++ Archiving $@"
	$(IPVERB)($(IPCD) "$(IPARLIBDIR)" && $(IPAR) $(IPARFLAGS) "$(IPAROUT)" $(IPARLIBOBJS))

ifeq ($(IPPORT),las)
# Cannot use IPCFLAGS in the two build rules below. For some obscure reason IPLIBINCLUDE
# from ipcrypto is included (in case ipcrypto is included in the build). ipcrypto adds
# ipxinc/include to IPLIBINCLUDE, and this breaks the las build of the two files below.
$(IPOBJDIR)/ipcom_shell_cmd_stub.o:	$(IPCOM_ROOT)/port/las/src/ipcom_shell_cmd_stub.c
	@$(IPECHO) "+++ Compiling $<"
	$(IPVERB)$(IPCC) $(IPEXTRACFLAGS) $(IPINC) $(IPDEF) -c -o $@ $<

$(IPOBJDIR)/ipcom_daemon_stub.o:	$(IPCOM_ROOT)/port/las/src/ipcom_daemon_stub.c
	@$(IPECHO) "+++ Compiling $<"
	$(IPVERB)$(IPCC) $(IPEXTRACFLAGS) $(IPINC) $(IPDEF) -c -o $@ $<
endif


#############################################################################
# UTILITIES
#############################################################################

.PHONY : util_ipdef util_ipinc

util_ipdef:
	@$(IPECHO) "$(IPDEF) $(IPLIBDEF)" $(IP_UTIL_REDIR)

util_ipinc:
	@$(IPECHO) "$(IPLIBINC) $(IPINC)" $(IP_UTIL_REDIR)

#############################################################################
# END OF IPCOM_MAKE.MK
#############################################################################

