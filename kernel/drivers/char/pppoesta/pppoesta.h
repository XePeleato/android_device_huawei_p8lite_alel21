#ifndef _PPPOESTA_ANDROID_H_
#define _PPPOESTA_ANDROID_H_

#include <linux/cdev.h>
#include <linux/semaphore.h>

#define PPPOESTA_DEVICE_NODE_NAME  "pppoesta"
#define PPPOESTA_DEVICE_FILE_NAME  "pppoesta"
#define PPPOESTA_DEVICE_CLASS_NAME "pppoesta"

#define VAL_LENGTH           10
#define MAX_MSG_LENGTH     20

struct pppoesta_android_dev {
    char val[VAL_LENGTH];
    struct semaphore sem;
    struct cdev dev;
};

#endif
