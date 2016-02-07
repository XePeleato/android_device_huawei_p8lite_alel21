#ifndef __MDRV_ACORE_PUBLIC_H__
#define __MDRV_ACORE_PUBLIC_H__
#ifdef __cplusplus
extern "C"
{
#endif


#define MDRV_OK                  (0)
#define MDRV_ERROR               (-1)

typedef int (*FUNCPTR_1)(int);
typedef int  (*pFUNCPTR)(void);
typedef void (*VOIDFUNCPTR)(unsigned int );

#ifdef __cplusplus
}
#endif
#endif
