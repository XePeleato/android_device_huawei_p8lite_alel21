cmd_mm/migrate.o :=  aarch64-linux-android-gcc -Wp,-MD,mm/.migrate.o.d  -nostdinc -isystem /home/edu/AOSP/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/../lib/gcc/aarch64-linux-android/4.9/include -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include -Iarch/arm64/include/generated  -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include -Iinclude -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi -Iarch/arm64/include/generated/uapi -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi -Iinclude/generated/uapi -include /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/kconfig.h -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/drivers -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/mm -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/drivers/hisi/ap/config/product/hi6210sft/include -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/drivers/hisi/ap/platform/hi6210sft  -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/mm -Imm -D__KERNEL__ -mlittle-endian -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Wimplicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -DFEATURE_ON=1 -DFEATURE_OFF=0 -DFEATURE_UE_MODE_CDMA=FEATURE_OFF -DCHIP_BB_HI6210 -DBSP_CORE_APP -DBSP_COMPILE_ALLY -O2 -g -mgeneral-regs-only -fno-reorder-blocks -fno-ipa-cp-clone -fno-partial-inlining -Wframe-larger-than=2048 -fno-stack-protector -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-var-tracking-assignments -g -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack    -DBSP_CORE_APP -DBSP_COMPILE_ALLY -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(migrate)"  -D"KBUILD_MODNAME=KBUILD_STR(migrate)" -c -o mm/migrate.o /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/mm/migrate.c

source_mm/migrate.o := /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/mm/migrate.c

deps_mm/migrate.o := \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/numa/balancing.h) \
    $(wildcard include/config/transparent/hugepage.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/migrate.h \
    $(wildcard include/config/migration.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/mm.h \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sysctl.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/x86.h) \
    $(wildcard include/config/ppc.h) \
    $(wildcard include/config/parisc.h) \
    $(wildcard include/config/metag.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/ksm.h) \
    $(wildcard include/config/have/memblock/node/map.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/debug/vm/rb.h) \
    $(wildcard include/config/arch/uses/numa/prot/none.h) \
    $(wildcard include/config/proc/fs.h) \
    $(wildcard include/config/debug/pagealloc.h) \
    $(wildcard include/config/hibernation.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/hugetlbfs.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/errno.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/errno.h \
  arch/arm64/include/generated/asm/errno.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/errno.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/errno-base.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/gfp.h \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/hisi/rdr.h) \
    $(wildcard include/config/pm/sleep.h) \
    $(wildcard include/config/cma.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/memory/isolation.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/memcg.h) \
    $(wildcard include/config/compaction.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/no/bootmem.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/have/memoryless/nodes.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/have/arch/pfn/valid.h) \
    $(wildcard include/config/nodes/span/other/nodes.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/typecheck.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/preempt.h \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/context/tracking.h) \
    $(wildcard include/config/preempt/count.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/thread_info.h \
    $(wildcard include/config/compat.h) \
    $(wildcard include/config/debug/stack/usage.h) \
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
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/posix_types.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/stddef.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/stddef.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
    $(wildcard include/config/kprobes.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/compiler-gcc4.h \
    $(wildcard include/config/arch/use/builtin/bswap.h) \
  arch/arm64/include/generated/asm/posix_types.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/posix_types.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/bug.h \
    $(wildcard include/config/generic/bug.h) \
  arch/arm64/include/generated/asm/bug.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/atomic/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  /home/edu/AOSP/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include/stdarg.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/linkage.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/stringify.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/linkage.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/bitops.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/bitops.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/barrier.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/bitops/builtin-__ffs.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/bitops/builtin-ffs.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/bitops/builtin-__fls.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/bitops/builtin-fls.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/bitops/ffz.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/bitops/fls64.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/bitops/find.h \
    $(wildcard include/config/generic/find/first/bit.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/bitops/sched.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/bitops/hweight.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/bitops/arch_hweight.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/bitops/const_hweight.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/bitops/lock.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/bitops/non-atomic.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/bitops/le.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi/asm/byteorder.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/byteorder/little_endian.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/byteorder/little_endian.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/swab.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/swab.h \
  arch/arm64/include/generated/asm/swab.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/swab.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/byteorder/generic.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/printk.h \
    $(wildcard include/config/early/printk.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/init.h \
    $(wildcard include/config/broken/rodata.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/kern_levels.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/dynamic_debug.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/string.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/string.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/kernel.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/sysinfo.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/thread_info.h \
    $(wildcard include/config/arm64/64k/pages.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/const.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/irqflags.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/ptrace.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi/asm/ptrace.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/hwcap.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi/asm/hwcap.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/bottom_half.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/spinlock_types.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/spinlock_types.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
    $(wildcard include/config/prove/rcu.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/rwlock_types.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/spinlock.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/processor.h \
    $(wildcard include/config/hisi/alter/hardware/name.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/fpsimd.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/hw_breakpoint.h \
    $(wildcard include/config/have/hw/breakpoint.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/rwlock.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/spinlock_api_smp.h \
    $(wildcard include/config/inline/spin/lock.h) \
    $(wildcard include/config/inline/spin/lock/bh.h) \
    $(wildcard include/config/inline/spin/lock/irq.h) \
    $(wildcard include/config/inline/spin/lock/irqsave.h) \
    $(wildcard include/config/inline/spin/trylock.h) \
    $(wildcard include/config/inline/spin/trylock/bh.h) \
    $(wildcard include/config/uninline/spin/unlock.h) \
    $(wildcard include/config/inline/spin/unlock/bh.h) \
    $(wildcard include/config/inline/spin/unlock/irq.h) \
    $(wildcard include/config/inline/spin/unlock/irqrestore.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/rwlock_api_smp.h \
    $(wildcard include/config/inline/read/lock.h) \
    $(wildcard include/config/inline/write/lock.h) \
    $(wildcard include/config/inline/read/lock/bh.h) \
    $(wildcard include/config/inline/write/lock/bh.h) \
    $(wildcard include/config/inline/read/lock/irq.h) \
    $(wildcard include/config/inline/write/lock/irq.h) \
    $(wildcard include/config/inline/read/lock/irqsave.h) \
    $(wildcard include/config/inline/write/lock/irqsave.h) \
    $(wildcard include/config/inline/read/trylock.h) \
    $(wildcard include/config/inline/write/trylock.h) \
    $(wildcard include/config/inline/read/unlock.h) \
    $(wildcard include/config/inline/write/unlock.h) \
    $(wildcard include/config/inline/read/unlock/bh.h) \
    $(wildcard include/config/inline/write/unlock/bh.h) \
    $(wildcard include/config/inline/read/unlock/irq.h) \
    $(wildcard include/config/inline/write/unlock/irq.h) \
    $(wildcard include/config/inline/read/unlock/irqrestore.h) \
    $(wildcard include/config/inline/write/unlock/irqrestore.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/atomic.h \
    $(wildcard include/config/arch/has/atomic/or.h) \
    $(wildcard include/config/generic/atomic64.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/atomic.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/cmpxchg.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/atomic-long.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/wait.h \
  arch/arm64/include/generated/asm/current.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/current.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/wait.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/cache.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/seqlock.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/nodemask.h \
    $(wildcard include/config/movable/node.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/bitmap.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/page-flags-layout.h \
  include/generated/bounds.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/sparsemem.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/page.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/pgtable-3level-types.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/pgtable-nopud.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/memory.h \
  arch/arm64/include/generated/asm/sizes.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/sizes.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/sizes.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/memory_model.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/getorder.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/memory_hotplug.h \
    $(wildcard include/config/memory/hotremove.h) \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
    $(wildcard include/config/have/bootmem/info/node.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/notifier.h \
    $(wildcard include/config/srecorder.h) \
    $(wildcard include/config/powercollapse.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/mutex.h \
    $(wildcard include/config/ilockdep.h) \
    $(wildcard include/config/debug/mutexes.h) \
    $(wildcard include/config/mutex/spin/on/owner.h) \
    $(wildcard include/config/have/arch/mutex/cpu/relax.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  arch/arm64/include/generated/asm/rwsem.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/rwsem.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/srcu.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/rcupdate.h \
    $(wildcard include/config/rcu/torture/test.h) \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/tree/preempt/rcu.h) \
    $(wildcard include/config/rcu/trace.h) \
    $(wildcard include/config/preempt/rcu.h) \
    $(wildcard include/config/rcu/user/qs.h) \
    $(wildcard include/config/tiny/rcu.h) \
    $(wildcard include/config/tiny/preempt/rcu.h) \
    $(wildcard include/config/debug/objects/rcu/head.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/rcu/nocb/cpu.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/cpumask.h \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
    $(wildcard include/config/disable/obsolete/cpumask/functions.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/completion.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/debug/objects/free.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/rcutree.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/workqueue.h \
    $(wildcard include/config/debug/objects/work.h) \
    $(wildcard include/config/freezer.h) \
    $(wildcard include/config/sysfs.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/timer.h \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/debug/objects/timers.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/ktime.h \
    $(wildcard include/config/ktime/scalar.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/math64.h \
  arch/arm64/include/generated/asm/div64.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/div64.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/time.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/jiffies.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/timex.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/timex.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/param.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi/asm/param.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/param.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/timex.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/arch_timer.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/clocksource/arm_arch_timer.h \
    $(wildcard include/config/arm/arch/timer.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/clocksource.h \
    $(wildcard include/config/arch/clocksource/data.h) \
    $(wildcard include/config/clocksource/watchdog.h) \
    $(wildcard include/config/clksrc/of.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/io.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/pgtable.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/proc-fns.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/pgtable-hwdef.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/pgtable-3level-hwdef.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/pgtable.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/mm_types.h \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/have/cmpxchg/double.h) \
    $(wildcard include/config/have/aligned/struct/page.h) \
    $(wildcard include/config/want/page/debug/flags.h) \
    $(wildcard include/config/kmemcheck.h) \
    $(wildcard include/config/aio.h) \
    $(wildcard include/config/mm/owner.h) \
    $(wildcard include/config/mmu/notifier.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/auxvec.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/auxvec.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi/asm/auxvec.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/rbtree.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/page-debug-flags.h \
    $(wildcard include/config/page/poisoning.h) \
    $(wildcard include/config/page/guard.h) \
    $(wildcard include/config/page/debug/something/else.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/uprobes.h \
    $(wildcard include/config/arch/supports/uprobes.h) \
    $(wildcard include/config/uprobes.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/mmu.h \
  arch/arm64/include/generated/asm/early_ioremap.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/early_ioremap.h \
    $(wildcard include/config/generic/early/ioremap.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/iomap.h \
    $(wildcard include/config/has/ioport.h) \
    $(wildcard include/config/pci.h) \
    $(wildcard include/config/generic/iomap.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/pci_iomap.h \
    $(wildcard include/config/no/generic/pci/ioport/map.h) \
    $(wildcard include/config/generic/pci/iomap.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/timex.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/topology.h \
    $(wildcard include/config/sched/smt.h) \
    $(wildcard include/config/sched/mc.h) \
    $(wildcard include/config/sched/book.h) \
    $(wildcard include/config/use/percpu/numa/node/id.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/smp.h \
    $(wildcard include/config/use/generic/smp/helpers.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/smp.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/percpu.h \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/pfn.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/percpu.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/percpu.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/percpu-defs.h \
    $(wildcard include/config/debug/force/weak/per/cpu.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/topology.h \
    $(wildcard include/config/disable/cpu/sched/domain/balance.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/topology.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/mmdebug.h \
    $(wildcard include/config/debug/vm.h) \
    $(wildcard include/config/debug/virtual.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/debug_locks.h \
    $(wildcard include/config/debug/locking/api/selftests.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/range.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/bit_spinlock.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/shrinker.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/page-flags.h \
    $(wildcard include/config/hisi/mntn.h) \
    $(wildcard include/config/pageflags/extended.h) \
    $(wildcard include/config/arch/uses/pg/uncached.h) \
    $(wildcard include/config/memory/failure.h) \
    $(wildcard include/config/swap.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/huge_mm.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/vmstat.h \
    $(wildcard include/config/vm/event/counters.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/vm_event_item.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/mempolicy.h \
    $(wildcard include/config/tmpfs.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/slab.h \
    $(wildcard include/config/slab/debug.h) \
    $(wildcard include/config/failslab.h) \
    $(wildcard include/config/slob.h) \
    $(wildcard include/config/slab.h) \
    $(wildcard include/config/slub.h) \
    $(wildcard include/config/debug/slab.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/slub_def.h \
    $(wildcard include/config/slub/stats.h) \
    $(wildcard include/config/memcg/kmem.h) \
    $(wildcard include/config/slub/debug.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/kobject.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/sysfs.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/kobject_ns.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/kref.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/kmemleak.h \
    $(wildcard include/config/debug/kmemleak.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/pagemap.h \
    $(wildcard include/config/arm64.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/fs.h \
    $(wildcard include/config/fs/posix/acl.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/fsnotify.h) \
    $(wildcard include/config/ima.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/debug/writecount.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/fs/xip.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/kdev_t.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/kdev_t.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/dcache.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/rculist.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/rculist_bl.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/list_bl.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/path.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/stat.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/stat.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi/asm/stat.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/stat.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/compat.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/sched.h \
    $(wildcard include/config/sched/debug.h) \
    $(wildcard include/config/no/hz/common.h) \
    $(wildcard include/config/lockup/detector.h) \
    $(wildcard include/config/core/dump/default/elf/headers.h) \
    $(wildcard include/config/sched/autogroup.h) \
    $(wildcard include/config/virt/cpu/accounting/native.h) \
    $(wildcard include/config/bsd/process/acct.h) \
    $(wildcard include/config/taskstats.h) \
    $(wildcard include/config/audit.h) \
    $(wildcard include/config/cgroups.h) \
    $(wildcard include/config/inotify/user.h) \
    $(wildcard include/config/fanotify.h) \
    $(wildcard include/config/posix/mqueue.h) \
    $(wildcard include/config/keys.h) \
    $(wildcard include/config/perf/events.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/task/delay/acct.h) \
    $(wildcard include/config/sched/hmp.h) \
    $(wildcard include/config/fair/group/sched.h) \
    $(wildcard include/config/rt/group/sched.h) \
    $(wildcard include/config/cgroup/sched.h) \
    $(wildcard include/config/blk/dev/io/trace.h) \
    $(wildcard include/config/rcu/boost.h) \
    $(wildcard include/config/compat/brk.h) \
    $(wildcard include/config/cc/stackprotector.h) \
    $(wildcard include/config/virt/cpu/accounting/gen.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/detect/hung/task.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/task/xacct.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/fault/injection.h) \
    $(wildcard include/config/latencytop.h) \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/bcache.h) \
    $(wildcard include/config/have/unstable/sched/clock.h) \
    $(wildcard include/config/irq/time/accounting.h) \
    $(wildcard include/config/no/hz/full.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/sched.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/capability.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/capability.h \
  arch/arm64/include/generated/asm/cputime.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/cputime.h \
    $(wildcard include/config/virt/cpu/accounting.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/cputime_jiffies.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/sem.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/sem.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/ipc.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/uidgid.h \
    $(wildcard include/config/uidgid/strict/type/checks.h) \
    $(wildcard include/config/user/ns.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/highuid.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/ipc.h \
  arch/arm64/include/generated/asm/ipcbuf.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/ipcbuf.h \
  arch/arm64/include/generated/asm/sembuf.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/sembuf.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/signal.h \
    $(wildcard include/config/old/sigaction.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/signal.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi/asm/signal.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/signal.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/signal.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/signal-defs.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi/asm/sigcontext.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi/asm/siginfo.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/siginfo.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/siginfo.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/pid.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/proportions.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/percpu_counter.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/seccomp.h \
    $(wildcard include/config/seccomp.h) \
    $(wildcard include/config/seccomp/filter.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/seccomp.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/rtmutex.h \
    $(wildcard include/config/debug/rt/mutexes.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/plist.h \
    $(wildcard include/config/debug/pi/list.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/resource.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/resource.h \
  arch/arm64/include/generated/asm/resource.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/resource.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/resource.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/hrtimer.h \
    $(wildcard include/config/high/res/timers.h) \
    $(wildcard include/config/timerfd.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/timerqueue.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/task_io_accounting.h \
    $(wildcard include/config/task/io/accounting.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/latencytop.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/cred.h \
    $(wildcard include/config/debug/credentials.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/key.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/sysctl.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/sysctl.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/selinux.h \
    $(wildcard include/config/security/selinux.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/llist.h \
    $(wildcard include/config/arch/have/nmi/safe/cmpxchg.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/ptrace.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/err.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/pid_namespace.h \
    $(wildcard include/config/pid/ns.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/nsproxy.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/ptrace.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/stat.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/radix-tree.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/semaphore.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/fiemap.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/migrate_mode.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/percpu-rwsem.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/blk_types.h \
    $(wildcard include/config/blk/cgroup.h) \
    $(wildcard include/config/blk/dev/integrity.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/fs.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/limits.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/ioctl.h \
  arch/arm64/include/generated/asm/ioctl.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/ioctl.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/ioctl.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/quota.h \
    $(wildcard include/config/quota/netlink/interface.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/dqblk_xfs.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/dqblk_v1.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/dqblk_v2.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/dqblk_qtree.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/projid.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/quota.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/nfs_fs_i.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/fcntl.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/fcntl.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi/asm/fcntl.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/fcntl.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/highmem.h \
    $(wildcard include/config/x86/32.h) \
    $(wildcard include/config/debug/highmem.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/uaccess.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/uaccess.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/compiler.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/hardirq.h \
    $(wildcard include/config/generic/hardirqs.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/ftrace_irq.h \
    $(wildcard include/config/ftrace/nmi/enter.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/vtime.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/hardirq.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/irq.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/irq.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/irq_cpustat.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/cacheflush.h \
  arch/arm64/include/generated/asm/kmap_types.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/kmap_types.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/hugetlb_inline.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/mempolicy.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/swap.h \
    $(wildcard include/config/frontswap.h) \
    $(wildcard include/config/memcg/swap.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/memcontrol.h \
    $(wildcard include/config/inet.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/cgroup.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/cgroupstats.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/taskstats.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/prio_heap.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/idr.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/xattr.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/xattr.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/cgroup_subsys.h \
    $(wildcard include/config/cgroup/debug.h) \
    $(wildcard include/config/cgroup/cpuacct.h) \
    $(wildcard include/config/cgroup/device.h) \
    $(wildcard include/config/cgroup/freezer.h) \
    $(wildcard include/config/net/cls/cgroup.h) \
    $(wildcard include/config/cgroup/perf.h) \
    $(wildcard include/config/netprio/cgroup.h) \
    $(wildcard include/config/cgroup/hugetlb.h) \
    $(wildcard include/config/cgroup/bcache.h) \
    $(wildcard include/config/cgroup/ionice.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/jump_label.h \
    $(wildcard include/config/jump/label.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/node.h \
    $(wildcard include/config/memory/hotplug/sparse.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/device.h \
    $(wildcard include/config/debug/devres.h) \
    $(wildcard include/config/acpi.h) \
    $(wildcard include/config/pinctrl.h) \
    $(wildcard include/config/dma/cma.h) \
    $(wildcard include/config/devtmpfs.h) \
    $(wildcard include/config/sysfs/deprecated.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/ioport.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/klist.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/pinctrl/devinfo.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/pinctrl/consumer.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/seq_file.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/pinctrl/pinctrl-state.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/pm.h \
    $(wildcard include/config/vt/console/sleep.h) \
    $(wildcard include/config/pm.h) \
    $(wildcard include/config/pm/runtime.h) \
    $(wildcard include/config/pm/clk.h) \
    $(wildcard include/config/pm/generic/domains.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/ratelimit.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/device.h \
    $(wildcard include/config/iommu/api.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/pm_wakeup.h \
    $(wildcard include/config/huawei/kernel.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/swapops.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/buffer_head.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/mm_inline.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/pagevec.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/ksm.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/rmap.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/cpu.h \
    $(wildcard include/config/arch/has/cpu/autoprobe.h) \
    $(wildcard include/config/arch/cpu/probe/release.h) \
    $(wildcard include/config/pm/sleep/smp.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/cpuset.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/writeback.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/vmalloc.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/security.h \
    $(wildcard include/config/security/path.h) \
    $(wildcard include/config/security/network.h) \
    $(wildcard include/config/security/network/xfrm.h) \
    $(wildcard include/config/securityfs.h) \
    $(wildcard include/config/security/yama.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/syscalls.h \
    $(wildcard include/config/ftrace/syscalls.h) \
    $(wildcard include/config/old/sigsuspend.h) \
    $(wildcard include/config/old/sigsuspend3.h) \
    $(wildcard include/config/odd/rt/sigaction.h) \
    $(wildcard include/config/clone/backwards.h) \
    $(wildcard include/config/clone/backwards3.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/aio_abi.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/unistd.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/unistd.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi/asm/unistd.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/unistd.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/unistd.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/trace/syscall.h \
    $(wildcard include/config/tracepoints.h) \
    $(wildcard include/config/have/syscall/tracepoints.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/tracepoint.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/static_key.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/ftrace_event.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/ring_buffer.h \
    $(wildcard include/config/ring/buffer/allow/swap.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/kmemcheck.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/poll.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/poll.h \
  arch/arm64/include/generated/asm/poll.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/poll.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/trace_seq.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/perf_event.h \
    $(wildcard include/config/event/tracing.h) \
    $(wildcard include/config/function/tracer.h) \
    $(wildcard include/config/cpu/sup/intel.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/perf_event.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/perf_event.h \
    $(wildcard include/config/hw/perf/events.h) \
  arch/arm64/include/generated/asm/local64.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/local64.h \
  arch/arm64/include/generated/asm/local.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/local.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/ftrace.h \
    $(wildcard include/config/have/function/trace/mcount/test.h) \
    $(wildcard include/config/dynamic/ftrace/with/regs.h) \
    $(wildcard include/config/dynamic/ftrace.h) \
    $(wildcard include/config/stack/tracer.h) \
    $(wildcard include/config/frame/pointer.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/trace_clock.h \
  arch/arm64/include/generated/asm/trace_clock.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/trace_clock.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/kallsyms.h \
    $(wildcard include/config/kallsyms.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/ftrace.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/insn.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/compat.h \
    $(wildcard include/config/compat/old/sigaction.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/socket.h \
  arch/arm64/include/generated/asm/socket.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/socket.h \
  arch/arm64/include/generated/asm/sockios.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/sockios.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/sockios.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/uio.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/uio.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/socket.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/if.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/hdlc/ioctl.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/irq_work.h \
    $(wildcard include/config/irq/work.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/perf_regs.h \
    $(wildcard include/config/have/perf/regs.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi/asm/perf_regs.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/hugetlb.h \
    $(wildcard include/config/arch/want/huge/pmd/share.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/hugetlb_cgroup.h \
    $(wildcard include/config/mem/res/ctlr/hugetlb.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/res_counter.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/balloon_compaction.h \
    $(wildcard include/config/balloon/compaction.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/tlbflush.h \
    $(wildcard include/config/ttbr1/48bits/varange/bug.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/cputype.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/trace/events/migrate.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/trace/define_trace.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/trace/ftrace.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/mm/internal.h \
    $(wildcard include/config/debug/memory/init.h) \

mm/migrate.o: $(deps_mm/migrate.o)

$(deps_mm/migrate.o):
