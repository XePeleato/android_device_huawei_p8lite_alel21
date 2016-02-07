/*
 ******************************************************************************
 *                     INTERPEAK EXAMPLE SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipdhcpc_example.c,v $ $Revision: 1.8.18.4 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipdhcpc_example.c,v $
 *   $Author: dbush $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01g,04sep09,h_s  Fix the callback compile error when using gnu.
01f,02oct09,spw  Declare option, reply callback only if param not null. 
01e,02dec08,spw  Fix WIND00146283 and add REPLY_CB_EVENT_OFFER case
01d,04jun08,spw  Change Syslog facility, cleanup
01c,11oct07,uol  Defect WIND00107491, add DHCP capabilities in bootrom.
01b,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01a,11dec06,kch  Integrated to VxWorks build infrastructure.
*/


/****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipdhcpc_config.h"
#include "ipcom_config.h"

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPCOM_USE_CLIB_PROTO
#undef IPCOM_USE_CLIB_PROTO
#endif

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_shell.h>
#include <ipcom_syslog.h>
#include <ipdhcpc.h>

/* Syslog facility code */
#ifdef IPCOM_SYSLOG_PRIORITY
#undef IPCOM_SYSLOG_PRIORITY
#define IPCOM_SYSLOG_PRIORITY      IPCOM_LOG_DEBUG2
#endif
#ifdef IPCOM_SYSLOG_FACILITY
#undef IPCOM_SYSLOG_FACILITY
#endif
#define IPCOM_SYSLOG_FACILITY IPCOM_LOG_IPDHCPC

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

IP_EXTERN FUNCPTR _ipdhcpc_callback_hook;

/* typedef to make compiler happy... */
typedef Ip_err (*ipdhcpc_option_cb)(Ipdhcpc_handle *handle, Ip_u8 *option);

#if DHCPC_INSTALL_OPTION_CALLBACK_HOOK == TRUE
    /*
     * forward declarations for the user-defined dhcp client callback hooks
     * to satisfy the compiler.
     */
    /* Fix the callback compile error when using gnu */
    #if (DHCPC_OPTION_CALLBACK_HOOK != 0)
    Ip_err DHCPC_OPTION_CALLBACK_HOOK (Ipdhcpc_handle *, Ip_u8 *);
    #endif
#else
    /*
     * DHCPC_OPTION_CALLBACK_HOOK is applicable only if 
     * DHCPC_INSTALL_OPTION_CALLBACK_HOOK is set to TRUE. Explicitly re-define 
     * these hooks to NULL so that we don't get compiler errors
     */
    #undef DHCPC_OPTION_CALLBACK_HOOK
    #define DHCPC_OPTION_CALLBACK_HOOK NULL
#endif /* DHCPC_INSTALL_OPTION_CALLBACK_HOOK */
    
#if DHCPC_INSTALL_REPLY_CALLBACK_HOOK == TRUE
    /*
     * forward declarations for the user-defined dhcp client callback hooks
     * to satisfy the compiler.
     */
    /* Fix the callback compile error when using gnu */
    #if (DHCPC_REPLY_CALLBACK_HOOK != 0)
    Ip_err DHCPC_REPLY_CALLBACK_HOOK (Ipdhcpc_handle *, int);
    #endif
#else
    /*
     * DHCPC_REPLY_CALLBACK_HOOK is applicable only if 
     * DHCPC_INSTALL_REPLY_CALLBACK_HOOK is set to TRUE. Explicitly re-define 
     * these hooks to NULL so that we don't get compiler errors
     */
    #undef DHCPC_REPLY_CALLBACK_HOOK
    #define DHCPC_REPLY_CALLBACK_HOOK NULL
#endif /* DHCPC_INSTALL_REPLY_CALLBACK_HOOK */       
    
/*****************************************************************************
 *
 * ipdhcpc_option_default_callback - default dhcp client callback routine
 *
 * This is an example implementation of the dhcp client option callback
 * routine. It is called if no dhcp client option callback routine is
 * assigned to the macro DHCPC_OPTION_CALLBACK_HOOK.
 *
 * NOMANUAL
 */
_WRS_INLINE
Ip_err ipdhcpc_option_default_callback(Ipdhcpc_handle *handle, Ip_u8 *option)
{
    (void)handle;

    switch (*option)
    {
    case IPDHCPC_OPTCODE_VENDOR_ENCAPSULATED_OPTIONS:
        break;
    default:
        break;
    }

    /* Let IPDHCPC act on option */
    return IP_TRUE;
}

/*****************************************************************************
 *
 * ipdhcpc_reply_example_callback - default dhcp client callback routine
 *
 * This is an example implementation of the dhcp client reply callback
 * routine.
 *
 * NOMANUAL
 */
_WRS_INLINE
Ip_err ipdhcpc_reply_example_callback(Ipdhcpc_handle *handle, int event)
{
    Ip_err	ret;
    char yiaddr[16];
          
    /* Informational only in this implementation. */
    
    if (handle == IP_NULL)
    	goto fail;
    
    switch(event)
    {
    case IPDHCPC_REPLY_CB_EVENT_OFFER:
    	
        IPCOM_LOG1(INFO,
                   "ipdhcpc_reply_cb :: received OFFER on interface '%d'.", 
                   handle->ifindex);

        break;
    
    case IPDHCPC_REPLY_CB_EVENT_ACK:
        
    	IPCOM_LOG2(INFO, 
                   "ipdhcpc_reply_example_callback :: received ACK on interface '%d' with yiaddr '%s'.",
                   handle->ifindex, 
                   ipcom_inet_ntop(IP_AF_INET, 
                    		       &handle->info.ipaddr_n,
                    		       yiaddr,
                    		       sizeof(yiaddr)));
    	
        /* We still need to call ipdhcpc_iflease in a custom reply callback! */
        
        ret = ipdhcpc_iflease(handle->ifindex, handle);
    	
        if (ret != IPCOM_SUCCESS)
        {
            IPCOM_LOG2(ERR, 
                       "ipdhcpc_reply_example_callback :: ipdhcpc_iflease() failed on interface '%d', err = %d.",
                       handle->ifindex, 
                       ret);
            goto fail;
        }

        break;

    case IPDHCPC_REPLY_CB_EVENT_NAK:
        IPCOM_LOG1(INFO, 
                   "ipdhcpc_reply_example_callback :: received NAK on interface '%d'.",
                   handle->ifindex);
        break;

    case IPDHCPC_REPLY_CB_EVENT_EXPIRE:   	
        IPCOM_LOG1(INFO, 
                   "ipdhcpc_reply_example_callback :: Lease expired on interface '%d'.",
                   handle->ifindex);
        break;

    case IPDHCPC_REPLY_CB_EVENT_BOOTREPLY:
    	
    	/* We still need to call ipdhcpc_iflease in a custom reply callback! */
    	
    	ret = ipdhcpc_iflease(handle->ifindex, handle);
        if (ret != IPCOM_SUCCESS)
        {
            IPCOM_LOG2(ERR, 
                       "ipdhcpc_reply_example_callback :: ipdhcpc_iflease() failed on interface '%d', err = %d.",
                       handle->ifindex, 
                       ret);
            goto fail;
        }
    	break;
    	
    case IPDHCPC_REPLY_CB_EVENT_FAIL_IFLOST :
    default:

    	/* We still need to call ipdhcpc_handle_free in a custom reply callback! */
    	
        (void)ipdhcpc_handle_free(handle->ifindex, handle);
        break;
    }
    
    return IPCOM_SUCCESS;

fail:
   return IPCOM_ERR_FAILED;
}


/*****************************************************************************
 *
 * ipdhcpc_option_callback - dhcp client option callback routine
 *
 * This routine called once for each option prior to IPDHCPC is acting on the
 * option, i.e. this callback can be used to change the contents of the DHCP
 * server reply (caution!)
 * 
 * .IP <option>
 * Pointer to start of option, i.e. option[0] = Code; option[1] = Len;
 * option[2-i] = Optional option data
 * .IP <handle>
 * Pointer to request handle
 *
 * RETURNS:
 * IP_TRUE to let IPDHCPC act on the option (recommended) or IP_FALSE if
 * option should be ignored by IPDHCPC.
 *
 * NOMANUAL
 */
IP_PUBLIC Ip_err
ipdhcpc_option_callback(Ipdhcpc_handle *handle, Ip_u8 *option)
{
    ipdhcpc_option_cb dhcpc_cb;
	/*eliminate warning when compiling with gnu*/
    dhcpc_cb = (NULL!=DHCPC_OPTION_CALLBACK_HOOK) ? DHCPC_OPTION_CALLBACK_HOOK :
               _ipdhcpc_callback_hook ? _ipdhcpc_callback_hook :
               ipdhcpc_option_default_callback;

    return dhcpc_cb(handle, option);
}

/*****************************************************************************
 *
 * ipdhcpc_reply_callback - dhcp client reply callback routine
 *
 * this callback can be used to change the contents of the DHCP
 * server reply (caution!)
 * 
 * .IP <handle>
 * Pointer to ipdhcpc handle object, containing, i.e. option[0] = Code; option[1] = Len;
 * option[2-i] = Optional option data
 * 
 * .IP <event>
 * The reply type (ACK, NACK, etc)
 *
 * RETURNS:
 * IP_TRUE to let IPDHCPC act on the option (recommended) or IP_FALSE if
 * option should be ignored by IPDHCPC.
 *
 * NOMANUAL
 */
IP_PUBLIC Ip_err
ipdhcpc_reply_callback(Ipdhcpc_handle *handle, int event)
{
    Ipdhcpc_reply_cb dhcpc_cb;
    /*eliminate warning when compiling with gnu*/
    dhcpc_cb = (NULL!=DHCPC_REPLY_CALLBACK_HOOK) ? DHCPC_REPLY_CALLBACK_HOOK :
               ipdhcpc_reply_cb;   

    return dhcpc_cb(handle, event);
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
