/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_bitarray.c,v $ $Revision: 1.4 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_bitarray.c,v $
 *     $Author: braghava $ $Date: 2008-06-05 09:24:00 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *        Design and implementation by Balaji Raghavan(balaji.raghavan@windriver.com)
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"


#ifdef IPCOM_USE_BITARRAY

#ifndef IPCOM_DEBUG_BITARRAY
#undef IPCOM_USE_SYSLOG
#endif

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_bitarray.h"
#include "ipcom_err.h"
#include "ipcom_clib.h"


/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM


/*
 ****************************************************************************
 * 6                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

IP_STATIC Ip_err ipcom_bitarray_mod(Ipcom_bitarray *bitarray, Ip_u32 bitpos, Ip_u32 range, Ip_bool set);
IP_STATIC Ip_err ipcom_bitarray_mod_private(Ipcom_bitarray *bitarray, Ip_u32 start, Ip_u32 end, Ip_bool set);

#ifdef IP_DEBUG
IP_PUBLIC void ipcom_bitarray_test(void);
IP_PUBLIC int ipcom_cmd_bitarraytest(int argc, char **argv);
#endif


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_bitarray_new
 *===========================================================================
 * Description: Allocates and returns a pointer to a bit array
 *              of 8*size bits
 * Parameters:size in bytes
 * Returns:
 *
 */
Ipcom_bitarray*
ipcom_bitarray_new(Ip_s32 size)
{
    Ipcom_bitarray *bitarray;

    bitarray = ipcom_malloc(sizeof(Ipcom_bitarray));
    if (!bitarray)
    {
        IPCOM_LOG0(ERR, "ipcom_bitarray_new() :: out of memory");
        return IP_NULL;
    }

    bitarray->buf = ipcom_malloc(size);
    if (bitarray->buf == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipcom_bitarray_new() :: out of memory");
        ipcom_free(bitarray);
        return IP_NULL;
    }
    ipcom_memset(bitarray->buf,0,size);

    bitarray->alloc = size*8;
    bitarray->offset = 0;

    return bitarray;
}


/*
 *===========================================================================
 *                    ipcom_bitarray_free
 *===========================================================================
 * Description: Frees any memory used for the bitarray.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_bitarray_free(Ipcom_bitarray *bitarray)
{
    ip_assert( bitarray->buf && bitarray );
    ipcom_free(bitarray->buf);
    ipcom_free(bitarray);
}


/*
 *===========================================================================
 *                    ipcom_bitarray_reset
 *===========================================================================
 * Description: Clears any data from the bitarray, making it empty and ready
 *              to reuse.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_bitarray_reset(Ipcom_bitarray *bitarray)
{
    bitarray->offset = 0;
    ipcom_memset(bitarray->buf,0,(bitarray->alloc/8));
}



/*
 *===========================================================================
 *                    ipcom_bitarray_set
 *===========================================================================
 * Description: Sets a bit range or bit (range = 0) in the bitarray.
 *              for example bitpos=0 range=2 sets bits 0,1,2
 *              in bit array
 * Parameters: bitpos := 0 <= bitpos<sizeofbitarray
 *             range  := 0 <= sizeofbitarray - 1
 * Returns:  IPCOM_SUCCESS or IPCOM_ERR_FAILED
 *
 */
IP_PUBLIC Ip_err ipcom_bitarray_set(Ipcom_bitarray *bitarray, Ip_u32 bitpos, Ip_u32 range)
{

   if (!bitarray
       || bitpos > (bitarray->alloc)
       || range > (bitarray->alloc))
    {
        IPCOM_LOG0(ERR, "ipcom_bitarray_set() :: invalid parameters");
        return IPCOM_ERR_FAILED;
    }

   return ipcom_bitarray_mod(bitarray,bitpos,range,IP_TRUE);

}


/*
 *===========================================================================
 *                    ipcom_bitarray_isset
 *===========================================================================
 * Description: Returns IP_TRUE if a bit is set in the bitarray
 * Parameters: bitpos :=  position of bit in bitarray
 *                        0 <= bitpos< alloc
 * Returns:  IP_TRUE or IP_FALSE
 *
 */
IP_PUBLIC Ip_bool ipcom_bitarray_isset(Ipcom_bitarray *bitarray, Ip_u32 bitpos)
{
    int byte_num = 0 , bit_num = 0;

    Ip_u8 *buffer;

    if (!bitarray
       || bitpos > (bitarray->alloc))
    {
        IPCOM_LOG0(ERR, "ipcom_bitarray_isset() :: invalid parameters");
        return IP_FALSE;
    }


    bitpos = (bitpos + bitarray->offset) % bitarray->alloc;

    byte_num = bitpos / 8;
    bit_num = bitpos % 8;

    buffer = (Ip_u8*)(bitarray->buf) + byte_num;

    return  (Ip_bool)((*buffer) & (1<<(7 - bit_num)));

}


/*
 *===========================================================================
 *                    ipcom_bitarray_shiftL
 *===========================================================================
 * Description: left shift bitarray
 * Parameters: bitcount := number of bit shifts
 * Returns: IPCOM_SUCCESS or IPCOM_ERR_FAILED
 *
 */
IP_PUBLIC Ip_err ipcom_bitarray_shiftL(Ipcom_bitarray *bitarray, Ip_u32 bit_count)
{

    if (!bitarray)
    {
        IPCOM_LOG0(ERR, "ipcom_bitarray_shiftL() :: invalid parameters");
        return IPCOM_ERR_FAILED;
    }

    if (bit_count >= bitarray->alloc)
    {
        ipcom_bitarray_reset(bitarray);
        return IPCOM_SUCCESS;
    }

    if (ipcom_bitarray_clear(bitarray,0, bit_count - 1) == IPCOM_SUCCESS)
    {
        IPCOM_LOG2(DEBUG2,
                   "ipcom_bitarray_shiftL() :: offset=%lu bitcount=%lu ",
                   bitarray->offset,bit_count);

        bitarray->offset = (bitarray->offset + bit_count) % bitarray->alloc;

        return IPCOM_SUCCESS;
    }
    else
        return IPCOM_ERR_FAILED;


}

/*
 *===========================================================================
 *                    ipcom_bitarray_shiftR
 *===========================================================================
 * Description: right shift bitarray
 * Parameters: bitcount := number of bit shifts
 * Returns: IPCOM_SUCCESS or IPCOM_ERR_FAILED
 *
 */
IP_PUBLIC Ip_err ipcom_bitarray_shiftR(Ipcom_bitarray *bitarray, Ip_u32 bit_count)
{

    if (!bitarray)
    {
        IPCOM_LOG0(ERR, "ipcom_bitarray_shiftL() :: invalid parameters");
        return IPCOM_ERR_FAILED;
    }

    if (bit_count >= bitarray->alloc)
    {
        ipcom_bitarray_reset(bitarray);
        return IPCOM_SUCCESS;
    }

    bitarray->offset = (bitarray->offset - bit_count) % bitarray->alloc;
    return ipcom_bitarray_clear(bitarray,0, bit_count - 1);


}



/*
 *===========================================================================
 *                    ipcom_bitarray_clear
 *===========================================================================
 * Description: clears a bit range or bit (range = 0) in the bitarray.
 *              for example bitpos=0 range=2 sets bits 0,1,2
 *              in bit array
 * Parameters: bitpos := 0 <= bitpos < sizeofbitarray
 *             range  := 0 <= range  < sizeofbitarray
 * Returns:  IPCOM_SUCCESS or IPCOM_ERR_FAILED
 *
 */
IP_PUBLIC Ip_err ipcom_bitarray_clear(Ipcom_bitarray *bitarray, Ip_u32 bitpos, Ip_u32 range)
{

   if (!bitarray
       || bitpos > (bitarray->alloc)
       || range > (bitarray->alloc))
    {
        IPCOM_LOG0(ERR, "ipcom_bitarray_clear() :: invalid parameters");
        return IPCOM_ERR_FAILED;
    }

   return ipcom_bitarray_mod(bitarray,bitpos,range,IP_FALSE);

}


/*
 *===========================================================================
 *                    ipcom_bitarray_mod
 *===========================================================================
 * Description: Private function
 * Parameters: bitpos := 0 <= bitpos<sizeofbitarray
 *             range  := 0 <= sizeofbitarray - 1
 * Returns:  IPCOM_SUCCESS or IPCOM_ERR_FAILED
 *
 */
IP_STATIC Ip_err ipcom_bitarray_mod(Ipcom_bitarray *bitarray, Ip_u32 bitpos, Ip_u32 range, Ip_bool set)
{
    int startpos,endpos;
    Ip_err err = IPCOM_ERR_FAILED;

    startpos = (bitarray->offset + bitpos) % bitarray->alloc;
    endpos = (startpos + range) % bitarray->alloc;

    IPCOM_LOG2(DEBUG2,"ipcom_bitarray_mod() :: startpos=%lu endpos =%lu ",startpos,endpos);

    if (endpos < startpos)
    {
        IPCOM_LOG2(DEBUG2,
                   "ipcom_bitarray_mod() :: startrange=%lu endrange =%lu ",
                   startpos,(bitarray->alloc-1));
        err = ipcom_bitarray_mod_private(bitarray,startpos,(bitarray->alloc-1),set);
        if (err == IPCOM_SUCCESS)
        {
            IPCOM_LOG2(DEBUG2,
                   "ipcom_bitarray_mod() :: startrange=%lu endrange =%lu ",
                   0,endpos);
            err =  ipcom_bitarray_mod_private(bitarray,0,endpos,set);
        }
    }
    else
    {
       err = ipcom_bitarray_mod_private(bitarray,startpos,endpos,set);

    }

    return err;

}

/*
 *===========================================================================
 *                    ipcom_bitarray_mod_private
 *===========================================================================
 * Description: Private function
 * Parameters: bitpos := 0 <= bitpos<sizeofbitarray
 *             range  := 0 <= sizeofbitarray - 1
 * Returns:  IPCOM_SUCCESS or IPCOM_ERR_FAILED
 *
 */
IP_STATIC Ip_err ipcom_bitarray_mod_private(Ipcom_bitarray *bitarray, Ip_u32 start, Ip_u32 end, Ip_bool set)
{
    int byte_start = 0, byte_end = 0,i;
    Ip_u8 *buffer,bit_start=0,bit_end=0;

    ip_assert(end >= start);


    byte_start = start / 8;
    byte_end = end / 8;
    bit_start = start % 8;
    bit_end = end % 8;

    if (byte_end - byte_start >= 2)
    {

        IPCOM_LOG2(DEBUG2,
                   "ipcom_bitarray_mod_private() :: startbyte=%lu endbyte =%lu ",
                   byte_start,byte_end);

        for (i=byte_start+1;i<=byte_end-1;i++)
        {
            *((Ip_u8*)(bitarray->buf)+i)=(set?0xFF:0x0);
        }
    }

    buffer = (Ip_u8*)(bitarray->buf) + byte_start;
    if (start != end)
    {
        IPCOM_LOG2(DEBUG2,
                   "ipcom_bitarray_mod_private() :: startbit=%lu startbyte =%lu ",
                   bit_start,byte_start);
        if (set)
            *buffer |= 0xFF >> bit_start ;
        else
            *buffer &= ~(0xFF >> bit_start);

        IPCOM_LOG2(DEBUG2,
                   "ipcom_bitarray_mod_private() :: endbit=%lu endbyte =%lu ",
                   bit_end,byte_end);

        buffer = (Ip_u8*)(bitarray->buf) + byte_end;
        if (set)
            *buffer |= 0xFF << (7 - bit_end);
        else
            *buffer &= ~(0xFF << (7 - bit_end));

    }
    else
    {
        IPCOM_LOG2(DEBUG2,
                   "ipcom_bitarray_mod_private() :: modifying bit %lu in byte %lu ",
                   bit_start,byte_start);
        if (set)
            *buffer |= 1<<(7-bit_start);
        else
            *buffer &= ~(1<<(7-bit_start));
    }

    return IPCOM_SUCCESS;
}


/*
*===========================================================================
*                    ipcom_bitarray_dump
*===========================================================================
* Description: Dumps the contents of the buffer to stdout.
*
* Parameters:
* Returns:
*
*/
IP_PUBLIC void
ipcom_bitarray_dump(Ipcom_bitarray *bitarray)
{
    Ip_u32 i=0,len;

    len = 0;
    for (i = bitarray->offset; len < bitarray->alloc ;len++,i = (i+1)%bitarray->alloc)
    {

        if (ipcom_bitarray_isset(bitarray,len))
            ipcom_printf("%1d",1);
        else
           ipcom_printf("%1d",0);
    }

    ipcom_printf("\r\n");




}


#ifdef IP_DEBUG
/*
*===========================================================================
*                    ipcom_bitarray_test
*===========================================================================
* Description: Unit test
* Parameters:
* Returns:
*
*/
IP_PUBLIC void
ipcom_bitarray_test(void)
{
    int i;
    Ipcom_bitarray *bitarray=IP_NULL;

#define BSIZE 32
    ipcom_debug_printf("Testing bitarray implementation \r\n");

    bitarray = ipcom_bitarray_new(BSIZE/8);
    if (!bitarray)
    {
        ipcom_debug_printf("ipcom_bitarray_test() :: Failed to allocate new array");
        return;
    }
    ipcom_bitarray_dump(bitarray);

    goto test;
 test:

    /* Set all bits */
    ipcom_bitarray_set(bitarray,0,BSIZE-1);
    ipcom_bitarray_dump(bitarray);

    /* Clear all bits */
    ipcom_bitarray_clear(bitarray,0,BSIZE-1);
    ipcom_bitarray_dump(bitarray);

    /* Set single bits */

    for (i=0;i<BSIZE;i++)
    {
        ipcom_debug_printf("setting bit %d \r\n",i);
        ipcom_bitarray_set(bitarray,i,0);
        ipcom_bitarray_dump(bitarray);
        ipcom_bitarray_reset(bitarray);
    }

    /* Set a range of bits */
    ipcom_bitarray_set(bitarray,10,BSIZE-10);
    ipcom_bitarray_dump(bitarray);
    ipcom_bitarray_reset(bitarray);


    /* Set a range of bits */
    ipcom_bitarray_set(bitarray,BSIZE-10,10);
    ipcom_bitarray_dump(bitarray);

    ipcom_bitarray_reset(bitarray);

    ipcom_bitarray_set(bitarray,BSIZE-1,0);
    ipcom_bitarray_dump(bitarray);
    ipcom_debug_printf("testing shift left \r\n");

    for (i=0;i<BSIZE;i++)
    {
        ipcom_bitarray_shiftL(bitarray,1);
        ipcom_bitarray_dump(bitarray);
    }

    ipcom_bitarray_reset(bitarray);
    ipcom_bitarray_set(bitarray,0,0);
    ipcom_bitarray_dump(bitarray);
    ipcom_debug_printf("testing shift right \r\n");

    for (i=0;i<BSIZE;i++)
    {
        ipcom_bitarray_shiftR(bitarray,1);
        ipcom_bitarray_dump(bitarray);
    }

    ipcom_debug_printf("testing set clear \r\n");
    ipcom_bitarray_reset(bitarray);
    for (i=1;i<BSIZE;i+=2) {
        ipcom_bitarray_set(bitarray,i,0);
        if (!ipcom_bitarray_isset(bitarray,i))
            ipcom_debug_printf("Bit set failed %d \r\n",i);
        else
            ipcom_debug_printf("Bit set ok %d \r\n",i);
        ipcom_bitarray_clear(bitarray,i,0);
        if (ipcom_bitarray_isset(bitarray,i))
            ipcom_debug_printf("Bit clear failed %d \r\n",i);
        else
            ipcom_debug_printf("Bit clear ok %d \r\n",i);
    }


    if (bitarray)
        ipcom_bitarray_free(bitarray);



}

/* Just for fun */
IP_PUBLIC int
ipcom_cmd_bitarraytest(int argc, char **argv)
{

    (void)argc;
    (void)argv;

    ipcom_bitarray_test();
    return 0;
}
#endif /* IP_DEBUG */

#endif /* IPCOM_USE_BITARRAY */




