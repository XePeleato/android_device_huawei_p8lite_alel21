IPTARGETS := lib
ifneq ($(IPMAKEFILE),)
include $(IPMAKEFILE)
else
include $(IPCOM_ROOT)/Makefile
endif
include ipcom.mk
include ipcom_make.mk

lib: dirs $(IPLIBFILE)
