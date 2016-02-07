IPTARGETS := lib

ifneq ($(IPMAKEFILE),)
include $(IPMAKEFILE)
else
include $(IPCOM_ROOT)/Makefile
endif

ifneq ($(IPTEMPLATE),)
include $(IPTEMPLATE)
else
include ipnet2.mk
endif

include $(IPCOM_ROOT)/gmake/ipcom_make.mk

ifeq ($(IPPORT),las)
lib:	dirs $(IPLIBFILE)
else
lib:	dirs $(IPLIBFILE)
	$(IPMAKE) IPTEMPLATE=ipnet2_krn.mk lib2

lib2:	dirs $(IPLIBFILE)
endif

