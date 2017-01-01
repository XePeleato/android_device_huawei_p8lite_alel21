#include <stdlib.h>
#include <cutils/log.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include <hardware/lights.h>
#include <stdbool.h>

struct led_data {
    const char *max_brightness;
    const char *brightness;
    const char *delay_on;
    const char *delay_off;
    bool blink;
};
