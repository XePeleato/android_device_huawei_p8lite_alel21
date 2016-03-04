cmd_mm/init-mm.o :=  aarch64-linux-android-gcc -Wp,-MD,mm/.init-mm.o.d  -nostdinc -isystem /home/edu/AOSP/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/../lib/gcc/aarch64-linux-android/4.9/include -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include -Iarch/arm64/include/generated  -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include -Iinclude -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi -Iarch/arm64/include/generated/uapi -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi -Iinclude/generated/uapi -include /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/kconfig.h -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/drivers -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/mm -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/drivers/hisi/ap/config/product/hi6210sft/include -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/drivers/hisi/ap/platform/hi6210sft  -I/home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/mm -Imm -D__KERNEL__ -mlittle-endian -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Wimplicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -DFEATURE_ON=1 -DFEATURE_OFF=0 -DFEATURE_UE_MODE_CDMA=FEATURE_OFF -DCHIP_BB_HI6210 -DBSP_CORE_APP -DBSP_COMPILE_ALLY -O2 -g -mgeneral-regs-only -fno-reorder-blocks -fno-ipa-cp-clone -fno-partial-inlining -Wframe-larger-than=2048 -fno-stack-protector -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-var-tracking-assignments -g -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack    -DBSP_CORE_APP -DBSP_COMPILE_ALLY -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(init_mm)"  -D"KBUILD_MODNAME=KBUILD_STR(init_mm)" -c -o mm/init-mm.o /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/mm/init-mm.c

source_mm/init-mm.o := /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/mm/init-mm.c

deps_mm/init-mm.o := \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/mm_types.h \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/have/cmpxchg/double.h) \
    $(wildcard include/config/have/aligned/struct/page.h) \
    $(wildcard include/config/64bit.h) \
    $(wildcard include/config/want/page/debug/flags.h) \
    $(wildcard include/config/kmemcheck.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/aio.h) \
    $(wildcard include/config/mm/owner.h) \
    $(wildcard include/config/mmu/notifier.h) \
    $(wildcard include/config/transparent/hugepage.h) \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/numa/balancing.h) \
    $(wildcard include/config/compaction.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/auxvec.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/auxvec.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/uapi/asm/auxvec.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
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
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/const.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/spinlock.h \
    $(wildcard include/config/smp.h) \
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
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/bug.h \
    $(wildcard include/config/generic/bug.h) \
  arch/arm64/include/generated/asm/bug.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
    $(wildcard include/config/debug/bugverbose.h) \
    $(wildcard include/config/hisi/rdr.h) \
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
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/errno.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/errno.h \
  arch/arm64/include/generated/asm/errno.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/errno.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/asm-generic/errno-base.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/kernel.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/sysinfo.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/thread_info.h \
    $(wildcard include/config/arm64/64k/pages.h) \
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
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/rbtree.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/rwsem.h \
    $(wildcard include/config/ilockdep.h) \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  arch/arm64/include/generated/asm/rwsem.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/rwsem.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/completion.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/wait.h \
  arch/arm64/include/generated/asm/current.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/current.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/uapi/linux/wait.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/cpumask.h \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
    $(wildcard include/config/disable/obsolete/cpumask/functions.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/bitmap.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/page-debug-flags.h \
    $(wildcard include/config/page/poisoning.h) \
    $(wildcard include/config/page/guard.h) \
    $(wildcard include/config/page/debug/something/else.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/uprobes.h \
    $(wildcard include/config/arch/supports/uprobes.h) \
    $(wildcard include/config/uprobes.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/page-flags-layout.h \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  include/generated/bounds.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/sparsemem.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/page.h \
    $(wildcard include/config/have/arch/pfn/valid.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/pgtable-3level-types.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/pgtable-nopud.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/memory.h \
  arch/arm64/include/generated/asm/sizes.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/sizes.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/linux/sizes.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/memory_model.h \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/discontigmem.h) \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/getorder.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/mmu.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/pgtable.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/proc-fns.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/pgtable-hwdef.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/arch/arm64/include/asm/pgtable-3level-hwdef.h \
  /home/edu/AOSP/device/HUAWEI/hi6210sft/kernel/include/asm-generic/pgtable.h \
    $(wildcard include/config/arch/uses/numa/prot/none.h) \

mm/init-mm.o: $(deps_mm/init-mm.o)

$(deps_mm/init-mm.o):
