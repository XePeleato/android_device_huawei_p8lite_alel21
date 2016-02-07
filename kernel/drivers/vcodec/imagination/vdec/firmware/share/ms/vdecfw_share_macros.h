/*************************************************************************/ /*!
@File           vdecfw_share_macros.h
@Title          Macros for sharing data structures between host and MTX
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@Description    Macros for sharing data structures between host and MTX.
@License        Strictly Confidential.
*/ /**************************************************************************/
#ifndef _VDECFW_SHARE_MACROS_H_
#define _VDECFW_SHARE_MACROS_H_


/*!
******************************************************************************
 This macro sets alignment for a field structure.
 Parameters:
 a - alignment value
 t - field type
 n - field name
******************************************************************************/
#define IMG_ALIGN_FIELD(a, t, n) __declspec(align(a)) t n


#endif /* _VDECFW_SHARE_MACROS_H_ */
