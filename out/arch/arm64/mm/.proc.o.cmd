cmd_arch/arm64/mm/proc.o :=  aarch64-linux-android-gcc -Wp,-MD,arch/arm64/mm/.proc.o.d  -nostdinc -isystem /home/edu/AOSP/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.8/bin/../lib/gcc/aarch64-linux-android/4.8/include -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include -Iarch/arm64/include/generated  -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include -Iinclude -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi -Iarch/arm64/include/generated/uapi -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi -Iinclude/generated/uapi -include /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/kconfig.h -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/drivers -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/mm -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/drivers/hisi/ap/config/product/hi6210sft/include -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/drivers/hisi/ap/platform/hi6210sft -D__KERNEL__ -mlittle-endian -D__ASSEMBLY__ -gdwarf-2   -DBSP_CORE_APP -DBSP_COMPILE_ALLY -c -o arch/arm64/mm/proc.o /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/mm/proc.S

source_arch/arm64/mm/proc.o := /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/mm/proc.S

deps_arch/arm64/mm/proc.o := \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/arm64/cpu/suspend.h) \
    $(wildcard include/config/arm64/64k/pages.h) \
    $(wildcard include/config/armv7/compat.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/init.h \
    $(wildcard include/config/broken/rodata.h) \
    $(wildcard include/config/modules.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
    $(wildcard include/config/kprobes.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/types.h \
  arch/arm64/include/generated/asm/types.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/types.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/int-ll64.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/int-ll64.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi/asm/bitsperlong.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/bitsperlong.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/bitsperlong.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/linkage.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/stringify.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/linkage.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/assembler.h \
    $(wildcard include/config/cpu/big/endian.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/ptrace.h \
    $(wildcard include/config/compat.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi/asm/ptrace.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/hwcap.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi/asm/hwcap.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/asm-offsets.h \
  include/generated/asm-offsets.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/pgtable-hwdef.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/pgtable-3level-hwdef.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/pgtable.h \
    $(wildcard include/config/transparent/hugepage.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/proc-fns.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/memory.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/const.h \
  arch/arm64/include/generated/asm/sizes.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/sizes.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/sizes.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/memory_model.h \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
    $(wildcard include/config/sparsemem.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/mm/proc-macros.S \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/thread_info.h \

arch/arm64/mm/proc.o: $(deps_arch/arm64/mm/proc.o)

$(deps_arch/arm64/mm/proc.o):
