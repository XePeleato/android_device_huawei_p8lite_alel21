

#ifndef _SMC_H_
#define _SMC_H_

#include <linux/of_device.h>

enum TC_NS_CMD_type {
    TC_NS_CMD_TYPE_INVALID = 0,
    TC_NS_CMD_TYPE_NS_TO_SECURE,
    TC_NS_CMD_TYPE_SECURE_TO_NS,
    TC_NS_CMD_TYPE_SECURE_TO_SECURE,
    TC_NS_CMD_TYPE_MAX
};

int smc_init_data(struct device *class_dev);
void smc_free_data(void);

unsigned int TC_NS_SMC(unsigned int cmd_addr);
#endif
