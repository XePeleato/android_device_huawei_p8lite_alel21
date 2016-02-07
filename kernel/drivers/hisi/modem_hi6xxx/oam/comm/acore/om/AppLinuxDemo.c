/*/opt/4.5.1/bin/arm-linux-gcc -o test --static *.c*/

#include <linux/fs.h>
#include <stdarg.h>
#include <stddef.h>
#include "string.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define OK              (0)
#define ERR             (-1)
#define ERROR           (1)

#define MAX_PRINT_LEN   1024

#define NV_APP_HEAD     unsigned short  usNvId;\
                        unsigned short  usLength;\
                        unsigned long   ulResult;

#define NV_APP_HEAD_LEN 8

typedef struct
{
    NV_APP_HEAD
    unsigned char   aucData[MAX_PRINT_LEN];
}NV_APP_DATA_STRU;

typedef struct
{
    unsigned long   ulModuleId;
    unsigned long   ulPrintLev;
    unsigned long   ulStrLen;
    unsigned char   aucStr[MAX_PRINT_LEN];
}OM_PRINTF_MODULE_APP_STRU;

#define KERNEL_CALL

#ifndef KERNEL_CALL

FILE *g_pfOmprintf;

FILE *g_pfOmprintfModule;

FILE *g_pfNv;

#else

int g_pfOmprintf = 0;

int g_pfOmprintfModule = 0;

int g_pfNv = 0;

#endif

unsigned long OM_PrintfWithModule(unsigned long ulModuleId, unsigned long ulLevel, char * pcformat, ...)
{
    int                         lRetLen;
    va_list                     argument;
    OM_PRINTF_MODULE_APP_STRU   stWriteData;
#ifndef KERNEL_CALL
    FILE                        *fd;
#else
    int                         fd;
#endif

    fd = open("/proc/OmPrintfModule", O_RDWR);

    if(0 == fd)
    {
        printf("\r\nOM_PrintfWithModule: Open OmPrintfModule File Failed\r\n");
        return ERROR;
    }

    va_start( argument, pcformat );
    lRetLen = vsnprintf(stWriteData.aucStr, MAX_PRINT_LEN-1, pcformat, argument);
    va_end(argument);

    if(ERR == lRetLen)
    {
        printf("\r\n OM_PrintfWithModule: nsprintf Return Error\r\n");
        return ERROR;
    }

    stWriteData.ulModuleId      = ulModuleId;
    stWriteData.ulPrintLev      = ulLevel;
    stWriteData.ulStrLen        = lRetLen;

#ifndef KERNEL_CALL
    lRetLen = fwrite(&stWriteData, sizeof(OM_PRINTF_MODULE_APP_STRU), 1, fd);
#else
    lRetLen = write(fd, &stWriteData, sizeof(OM_PRINTF_MODULE_APP_STRU));
#endif

#ifndef KERNEL_CALL
    fclose(fd);
#else
    close(fd);
#endif

    return lRetLen;
}

unsigned long OM_Printf(char * pcformat, ...)
{
    int     lRetLen;
    va_list     argument;
    unsigned char   acOutput[MAX_PRINT_LEN];
#ifndef KERNEL_CALL
        FILE                        *fd;
#else
        int                         fd;
#endif

    fd = open("/proc/OmPrintf", O_RDWR);

    if(0 == fd)
    {
        printf("\r\nOM_Printf: Open OmPrintf File Failed\r\n");
        return ERROR;
    }

    va_start(argument, pcformat);
    lRetLen = vsnprintf(acOutput, MAX_PRINT_LEN-1, pcformat, argument);
    va_end(argument);

    printf("\r\nOM_Printf: lRetLen is %d, Data:", lRetLen);

    if(ERR == lRetLen)
    {
        printf("\r\n OM_Printf: nsprintf Return Error");
        return ERROR;
    }

#ifndef KERNEL_CALL
    lRetLen = fwrite(acOutput, lRetLen+1, 1, fd);
#else
    lRetLen = write(fd, acOutput, lRetLen);
#endif

#ifndef KERNEL_CALL
        fclose(fd);
#else
        close(fd);
#endif

    return lRetLen;
}

unsigned long OM_VFInit(void)
{
    printf("\n---in OM_VFInit()\n");
#if 0
#ifndef KERNEL_CALL
    g_pfOmprintf = fopen("/proc/OmPrintf", "w");
#else
    g_pfOmprintf = open("/proc/OmPrintf", O_RDWR);
#endif

    if(0 == g_pfOmprintf)
    {
        printf("\r\nOM_VFInit: Open OmPrintf File Failed\r\n");
    }
    else
    {
        printf("\r\nOM_VFInit: Open OmPrintf File Successed\r\n");
    }

#ifndef KERNEL_CALL
    g_pfOmprintfModule = fopen("/proc/OmPrintfModule", "w");
#else
    g_pfOmprintfModule = open("/proc/OmPrintfModule", O_RDWR);
#endif

    if(0 == g_pfOmprintfModule)
    {
        printf("\r\nOM_VFInit: Open OmPrintfModule File Failed\r\n");
        return ERROR;
    }
    else
    {
        printf("\r\nOM_VFInit: Open OmPrintfModule File Successed\r\n");
    }
#endif

    printf("\n---out OM_VFInit()\n");

    return OK;
}

unsigned long NV_Read(unsigned short usID, void * pItem, unsigned long ulLength)
{
    NV_APP_DATA_STRU    stData;
    unsigned long       ulDataLen;

    VOS_MemSet(&stData, 0, sizeof(stData));

    if((0 == ulLength)||(MAX_PRINT_LEN < ulLength))
    {
        printf("\r\nNV_Read: Para is Error");
        return ERROR;
    }

    ulDataLen       = NV_APP_HEAD_LEN+ulLength;
    stData.usNvId   = usID;
    stData.usLength = (unsigned short)ulLength;
    stData.ulResult = ERROR;


#ifndef KERNEL_CALL
    fread((char *)&stData, ulDataLen, 1, g_pfNv);
#else
    read(g_pfNv,(char *)&stData, ulDataLen);
#endif

    if(OK == stData.ulResult)
    {
        memcpy(pItem, &stData.aucData, ulLength);
    }

    return stData.ulResult;
}

unsigned long NV_Write(unsigned short usID, void *pItem, unsigned long ulLength)
{
    NV_APP_DATA_STRU    stData;
    unsigned long       ulDataLen;

    if((0 == ulLength)||(0 == pItem))
    {
        printf("\r\nNV_Write: Para Error\r\n");
        return ERROR;
    }

    ulDataLen = NV_APP_HEAD_LEN+ulLength;

    stData.usNvId = usID;
    stData.usLength = (unsigned short)ulLength;
    stData.ulResult = ERROR;

    memcpy(stData.aucData, pItem, ulLength);
#ifndef KERNEL_CALL
    fwrite(&stData, ulDataLen, 1, g_pfNv);
#else
    write(g_pfNv,&stData,ulDataLen);
#endif

    return stData.ulResult;
}

unsigned long NV_VFInit(void)
{
#ifndef KERNEL_CALL
    g_pfNv = fopen("/proc/OmNv", "rw");
#else
    g_pfNv = open("/proc/OmNv", O_RDWR);
#endif

    if(0 == g_pfNv)
    {
        return ERROR;
    }


    return OK;
}

