#!/bin/bash
SRCTOP="$(pwd)/../../../../.."
#echo ${SRCTOP}
MSP_CODE="$SRCTOP/modem/oam"

kernel_source="$SRCTOP/ANDROID/android-2.6.35"
#echo ${kernel_source}

export PATH=$PATH:/opt/4.5.1/bin 

arm-linux-gcc -static -o NwInquire -I $kernel_source/include NetworkInquire.c $MSP_CODE/gu/acore/om/AppLinuxDemo.c
arm-linux-strip NwInquire
