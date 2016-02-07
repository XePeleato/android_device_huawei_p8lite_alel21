/*************************************************************************/ /*!
@File           vdecfw_version.h.in
@Title          Version information for VDEC Firmware
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@Description    This file is automatically updated by the build system to contain
                the correct version information.
@License        Strictly Confidential.
*/ /**************************************************************************/
#ifndef VDECFW_VERSION_H
#define VDECFW_VERSION_H

#define VDECFW_VERSION_MAJOR 6
#define VDECFW_VERSION_MINOR 0
#define VDECFW_VERSION_REVISION 0
#define VDECFW_VERSION_BUILD 112

// Setup the firmware version string
#define VERSION_STRING_(maj, min, rev, build) "Version: " #maj "." #min "." #rev "." #build
#define VERSION_STRING(maj, min, rev, build) VERSION_STRING_(maj, min, rev, build)
static const volatile char * pszFirmwareVersion = VERSION_STRING(VDECFW_VERSION_MAJOR, VDECFW_VERSION_MINOR, VDECFW_VERSION_REVISION, VDECFW_VERSION_BUILD);
static const volatile char * pszVersion_;


#endif  // VDECFW_VERSION_H
