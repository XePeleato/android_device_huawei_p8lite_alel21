#include <linux/sched.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/semaphore.h>
#include <linux/device.h>
#include <linux/workqueue.h>
#include <linux/cpu.h>
#include <linux/regulator/consumer.h>
#include <linux/spi/spi.h>
#include <asm/compiler.h>

#include "smc.h"
#include "tee_client_constants.h"
#include "tc_ns_client.h"
#include "teek_ns_client.h"

#define MAPI_TZ_API 0x1     //monitor api map defined in mach-k3v2/k3v2-monitorAPI.h

enum SPI_CLK_MODE{
    SPI_CLK_OFF = 0,
    SPI_CLK_ON,
};

struct regulator_bulk_data regu_burning;

struct smc_work {
        struct kthread_work work;
        phys_addr_t cmd_addr;
	unsigned int cmd_ret;
};

static struct task_struct *smc_thread;
static DEFINE_KTHREAD_WORKER(smc_worker);

#ifdef CONFIG_ARM64
static noinline int smc_send(phys_addr_t cmd_addr)
{
    TCVERBOSE("start to send smc to secure\n");
    register u64 x0 asm("x0") = TSP_REQUEST;
    register u64 x1 asm("x1") = cmd_addr;
    register u64 x2 asm("x2") = TC_NS_CMD_TYPE_NS_TO_SECURE;
    do {
	    asm volatile(
			 __asmeq("%0", "x0")
			 __asmeq("%1", "x0")
			 __asmeq("%2", "x1")
			 __asmeq("%3", "x2")
			 "smc	#0\n"
			 : "+r" (x0)
			 : "r" (x0), "r" (x1), "r" (x2));
    }while(x0 == TSP_REQUEST);
    TCVERBOSE("returned from secure\n");
    return x0;
}
#else
static unsigned int smc_send(phys_addr_t cmd_addr)
{
    register unsigned int r0 asm("r0") = MAPI_TZ_API;
    register unsigned int r1 asm("r1") = cmd_addr;
    register unsigned int r2 asm("r2") = TC_NS_CMD_TYPE_NS_TO_SECURE;
    do {
        asm volatile(
            __asmeq("%0", "r0")
            __asmeq("%1", "r0")
            __asmeq("%2", "r1")
            __asmeq("%3", "r2")
            ".arch_extension sec\n"
            "smc    #0  @ switch to secure world\n"
            : "=r" (r0)
            : "r" (r0), "r" (r1), "r" (r2));
    } while (r0 == 1);

    return r0;
}
#endif

static void smc_work_func(struct kthread_work *work)
{
       struct smc_work *s_work =
                container_of(work, struct smc_work, work);

       s_work->cmd_ret = smc_send(s_work->cmd_addr);
}

static unsigned int smc_send_func(unsigned int cmd_addr)
{
        struct smc_work work = {
                KTHREAD_WORK_INIT(work.work, smc_work_func),
                .cmd_addr = cmd_addr,
        };

        if (!queue_kthread_work(&smc_worker, &work.work))
                return false;
        flush_kthread_work(&work.work);
        return work.cmd_ret;
}

static int sec_s_power_on(void)
{
#ifdef CONFIG_ARCH_HI3XXX
	/*power on ccs*/
	int ret = 0;
	ret = regulator_bulk_enable(1, &regu_burning);
	if (ret)
		pr_err("failed to enable sec_s regulators %d\n", ret);
	return ret;
#else
	return 0;
#endif
}

static int sec_s_power_down(void)
{
#ifdef CONFIG_ARCH_HI3XXX
	/*power down ccs*/
	int ret = 0;
	ret = regulator_bulk_disable(1, &regu_burning);
	if (ret)
		pr_err("failed to disable sec_s regulators %d\n", ret);
	return ret;
#else
	return 0;
#endif
}


static DEFINE_MUTEX(smc_lock);
/*
 * Function:     TC_NS_SMC
 * Description:   This function first power on crypto cell,then send smc cmd to trustedcore.
 *                After finished, power off crypto cell.
 * Parameters:   cmd_addr
 * Return:      0:smc cmd handled succefully
 *              0xFFFFFFFF:power on or power off failed.
 *              0xFFFFxxxx:smc cmd handler failed or cmd pending.
 */
unsigned int TC_NS_SMC(unsigned int cmd_addr)
{
    unsigned int cpu_id;
    unsigned int ret = 0;
    unsigned int power_ret;

    mutex_lock(&smc_lock);

    power_ret = (unsigned int)sec_s_power_on();
    if (power_ret) {
        pr_err("failed to enable sec_s regulators %d\n", power_ret);
        ret = power_ret;
	goto unlock;
    }

#ifdef CONFIG_ARCH_HI3XXX
    spi0_clk_ctrl(SPI_CLK_ON);
#endif

    cpu_id = 0;
    TCDEBUG(KERN_INFO"***TC_NS_SMC call start on cpu %d ***\n", cpu_id);
    /* TODO
       directily call smc_send on cpu0 will call SMP PREEMPT error,
       so just use thread to send smc
    */
    mb();
    ret = smc_send_func(cmd_addr);

#ifdef CONFIG_ARCH_HI3XXX
    spi0_clk_ctrl(SPI_CLK_OFF);
#endif

    power_ret = (unsigned int)sec_s_power_down();
    if (power_ret) {
        pr_err("failed to disable sec_s regulators %d\n", power_ret);
        ret = power_ret;
    }
unlock:
    mutex_unlock(&smc_lock);
    return ret;
}

int smc_init_data(struct device *class_dev)
{
	int ret = 0;
#ifdef CONFIG_ARCH_HI3XXX
	regu_burning.supply = "sec-s-buring";
	ret = devm_regulator_bulk_get(class_dev, 1, &regu_burning);
	if (ret) {
		dev_err(class_dev, "couldn't get sec_burning regulator %d\n\r", ret);
		return ret;
	} else {
		pr_info("get sec_s_buring regulator success!\n");
	}
#endif
	smc_thread = kthread_create(kthread_worker_fn, &smc_worker,
										"smc_work");
	if (IS_ERR(smc_thread)) {
			ret = PTR_ERR(smc_thread);
			smc_thread = NULL;
			return ret;
	}
	/* this thread MUST run on CPU 0 at startup */
	set_cpus_allowed(smc_thread, CPU_MASK_CPU0);
	wake_up_process(smc_thread);

	return 0;
}

void smc_free_data(void)
{
        if (!IS_ERR_OR_NULL(smc_thread)) {
                kthread_stop(smc_thread);
                smc_thread = NULL;
        }
}

