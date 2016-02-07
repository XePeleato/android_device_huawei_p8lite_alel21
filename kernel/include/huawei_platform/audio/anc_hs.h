/*
 * anc_hs.h -- anc headset driver
 *
 * Copyright (c) 2014 Huawei Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef ANC_HS_H
#define ANC_HS_H

#define ANC_HS_4POLE 0
#define ANC_HS_3POLE 1
#define ANC_HS_REVERT_4POLE 2

#define ANC_HS_ENABLE_CHARGE 0
#define ANC_HS_DISABLE_CHARGE 1

/*IO controls for user*/
#define IOCTL_ANC_HS_CHARGE_ENABLE_CMD          _IO('A', 0x01)
#define IOCTL_ANC_HS_CHARGE_DISABLE_CMD         _IO('A', 0x02)
#define IOCTL_ANC_HS_GET_TYPE_CMD               _IOR('A', 0xFF, __u32)
#define IOCTL_ANC_HS_GET_HEADSET_CMD            _IOR('A', 0xFE, __u32)

/* codec operate used by anc_hs driver*/
struct anc_codec_ops
{
    bool (*check_headset_in)(void *);
    void (*btn_report)(int, int);
    void (*codec_resume_lock)(void *, bool);
};

struct anc_dev{
    char *name;
    struct anc_codec_ops ops;
};

#ifdef CONFIG_ANC_HS
int anc_hs_dev_register(struct anc_dev *dev, void *codec_data);
void anc_hs_start_charge(void);
void anc_hs_charge_detect(int saradc_value, int headset_type);
void anc_hs_stop_charge(void);
void anc_hs_force_charge(int disable);
bool check_anc_hs_support(void);
#else
static inline int anc_hs_dev_register(struct anc_dev *dev, void *codec_data)
{
    return 0;
}
static inline void anc_hs_start_charge(void)
{
    return;
}
static inline void anc_hs_charge_detect(int saradc_value, int headset_type)
{
    return;
}
static inline void anc_hs_stop_charge(void)
{
    return;
}
static inline void anc_hs_force_charge(int disable)
{
    return;
}
static inline bool check_anc_hs_support(void)
{
    return false;
}
#endif

#endif //ANC_HS_H