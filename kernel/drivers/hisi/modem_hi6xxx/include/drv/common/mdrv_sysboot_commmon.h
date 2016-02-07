#ifndef __MDRV_COMMON_SYSBOOT_H__
#define __MDRV_COMMON_SYSBOOT_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum{
    MDRV_RESET_CB_BEFORE,   /* callback before doing the reset */
    MDRV_RESET_CB_AFTER,    /* callback after reset done */
    MDRV_RESET_RESETTING,   /* callback during doing the reset */
    MDRV_RESET_CB_INVALID,
}DRV_RESET_CB_MOMENT_E;

#ifdef __cplusplus
}
#endif
#endif
