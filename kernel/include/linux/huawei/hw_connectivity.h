#ifndef _HW_CONNECTIVITY_H
#define _HW_CONNECTIVITY_H

#define CHIP_TYPE_HI110X  "hi110x"
#define CHIP_TYPE_BCM     "bcm43xx"
#ifdef CONFIG_HWCONNECTIVITY
bool isMyConnectivityChip(char* name);
#else
/* For no OneTrack image, always return ture */
static inline bool isMyConnectivityChip(char* name)
{
    return true;
}
#endif  //end of CONFIG_HWCONNECTIVITY

#endif
