/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipdnsc_api.c,v $ $Revision: 1.18 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/src/ipdnsc_api.c,v $
 *   $Author: markus $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * DNS Public API functions.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipdnsc_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>

#include "ipdnsc.h"
#include "ipdnsc_h.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */
/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC Ip_s32 ipdnsc_hostent_alias_count(struct Ip_hostent *he);
IP_STATIC Ip_s32 ipdnsc_hostent_addr_count(struct Ip_hostent *he);

IP_STATIC struct Ip_hostent*
ipdnsc_hostent_convert(struct Ip_hostent *he);

IP_STATIC struct Ip_hostent*
ipdnsc_hostent_merge(struct Ip_hostent *he1, struct Ip_hostent *he2);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                   LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipdnsc_hostent_alias_count
 *===========================================================================
 * Description: Counts the number of entries in a hostent structure alias
 *              list including the last null pointer.
 * Parameters:  he - pointer to the hostent structure
 * Returns:     The number of entries
 */
IP_STATIC Ip_s32
ipdnsc_hostent_alias_count(struct Ip_hostent *he)
{
    Ip_s32 i=0;

    while (he->h_aliases[i] != IP_NULL)
    {
        ++i;
    }

    /* Add one for the NULL terminator */
    return (i+1);
}


/*
 *===========================================================================
 *                    ipdnsc_hostent_addr_count
 *===========================================================================
 * Description: Counts the number of entries in a hostent structure
 *              address list including the last null pointer.
 * Parameters:  he - pointer to the hostent structure
 * Returns:     The number of entries
 */
IP_STATIC Ip_s32
ipdnsc_hostent_addr_count(struct Ip_hostent *he)
{
    Ip_s32 i=0;

    while (he->h_addr_list[i] != IP_NULL)
    {
        ++i;
    }

    /* Add one for the NULL terminator */
    return (i+1);
}


/*
 *===========================================================================
 *                    ipdnsc_hostent_convert
 *===========================================================================
 * Description: Converts a AF_INET hostent structure to a AF_INET6 hostent
 *              structure. ipv4 addresses are converted to ipv4 mapped ipv6
 *              addresses
 * Parameters:  he - pointer to the hostent structure to convert
 * Returns:     pointer to the new hostent structure or NULL if failed
 */
IP_STATIC struct Ip_hostent *
ipdnsc_hostent_convert(struct Ip_hostent *he)
{
    struct Ip_hostent *he_new;
    Ip_s32 num_alias, num_addr, i;
    Ip_u8 buf[16];
    Ip_u8 in6addr_mapped[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff};


    /* Verify the supplied host entry */
    if (he->h_addrtype != IP_AF_INET || he->h_length != IPDNSC_INADDRSZ)
        return IP_NULL;

    /* Get the number of alias and address entries */
    num_alias = ipdnsc_hostent_alias_count(he);
    num_addr = ipdnsc_hostent_addr_count(he);

    /* Create the new host entry structure */
    he_new = ipdnsc_hostent_create(IP_AF_INET6);

    if (he_new == IP_NULL)
        return IP_NULL;

    /* Allocate memory for and copy the aliases */
    for (i=0; i<(num_alias-1); i++)
    {
        if (ipdnsc_hostent_insert_alias(he_new, he->h_aliases[i]))
        {
            goto err_out;
        }
    }

    /* Allocate memory for and copy the addresses */
    for (i=0; i<(num_addr-1); i++)
    {
        ipcom_memcpy(buf, in6addr_mapped, sizeof(in6addr_mapped));
        ipcom_memcpy(buf+sizeof(in6addr_mapped), he->h_addr_list[i], he->h_length);
        if (ipdnsc_hostent_insert_addr(he_new, (char *)buf))
        {
            goto err_out;
        }
    }

    /* Insert the hostname */
    if (ipdnsc_hostent_insert_name(he_new, he->h_name))
    {
        goto err_out;
    }

    return he_new;

err_out:
    ipdnsc_hostent_free(he_new);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipdnsc_hostent_merge
 *===========================================================================
 * Description: Merge two AF_INET6 hostent structures to a common
 * Parameters:  he1, he2 - pointers to the hostent structures to be merged
 * Returns:     pointer to the new hostent structure or NULL if failed
 */
IP_STATIC struct Ip_hostent*
ipdnsc_hostent_merge(struct Ip_hostent *he1, struct Ip_hostent *he2)
{
    Ip_s32 i, num_addr1, num_alias1, num_addr2, num_alias2;
    struct Ip_hostent *he;

    if (he1 == IP_NULL || he2 == IP_NULL)
        return IP_NULL;

    /* Make sure hostnames are the same */
    if (ipcom_strcasecmp(he1->h_name, he2->h_name))
        return IP_NULL;

    /* Get the number of alias and address entries */
    num_addr1 = ipdnsc_hostent_addr_count(he1);
    num_addr2 = ipdnsc_hostent_addr_count(he2);
    num_alias1 = ipdnsc_hostent_alias_count(he1);
    num_alias2 = ipdnsc_hostent_alias_count(he2);

    /* Create the new host entry structure */
    he = ipdnsc_hostent_create(IP_AF_INET6);
    if (he == IP_NULL)
        return IP_NULL;

    /* Allocate memory for and copy the aliases */
    for (i=0; i<(num_alias1-1); i++)
    {
        if (ipdnsc_hostent_insert_alias(he, he1->h_aliases[i]))
        {
            goto err_out;
        }
    }
    for (i=0; i<(num_alias2-1); i++)
    {
        Ip_s32 j, duplicate;

        /* Avoid duplicates */
        duplicate = IP_FALSE;
        for (j=0; j<(num_alias1-1); j++)
        {
            if (!ipcom_strcasecmp(he1->h_aliases[j], he2->h_aliases[i]))
            {
                duplicate = IP_TRUE;
                break;
            }
        }

        if (duplicate == IP_FALSE)
        {
            if (ipdnsc_hostent_insert_alias(he, he2->h_aliases[i]))
            {
                goto err_out;
            }
        }
    }

    /* Allocate memory for and copy the addresses */
    for (i=0; i<(num_addr1-1); i++)
    {
        if (ipdnsc_hostent_insert_addr(he, he1->h_addr_list[i]))
        {
            goto err_out;
        }
    }
    for (i=0; i<(num_addr2-1); i++)
    {
        if (ipdnsc_hostent_insert_addr(he, he2->h_addr_list[i]))
        {
            goto err_out;
        }
    }

    /* Insert the hostname */
    if (ipdnsc_hostent_insert_name(he, he1->h_name))
    {
        goto err_out;
    }

    return he;

err_out:
    ipdnsc_hostent_free(he);
    return IP_NULL;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipdnsc_hostent_copy
 *===========================================================================
 * Description: Copies a hostent structure to a new hostent structure
 * Parameters:  he - pointer to the hostent structure to copy
 * Returns:     pointer to the new hostent structure or NULL if failed
 */
IP_GLOBAL struct Ip_hostent *
ipdnsc_hostent_copy(struct Ip_hostent *he)
{
    struct Ip_hostent *he_new;
    Ip_s32 num_alias, num_addr, i;

    /* Get the number of alias and address entries */
    num_alias = ipdnsc_hostent_alias_count(he);
    num_addr = ipdnsc_hostent_addr_count(he);

    /* Create the new host entry structure */
    he_new = ipdnsc_hostent_create(he->h_addrtype);

    if (!he_new)
        return IP_NULL;

    /* Allocate memory for and copy the aliases */
    for (i=0; i<(num_alias-1); i++)
    {
        if (ipdnsc_hostent_insert_alias(he_new, he->h_aliases[i]))
        {
            goto err_out;
        }
    }

    /* Allocate memory for and copy the addresses */
    for (i=0; i<(num_addr-1); i++)
    {
        if (ipdnsc_hostent_insert_addr(he_new, he->h_addr_list[i]))
        {
            goto err_out;
        }
    }


    /* Insert the hostname */
    if (ipdnsc_hostent_insert_name(he_new, he->h_name))
    {
        goto err_out;
    }

    return he_new;

err_out:
    ipdnsc_hostent_free(he_new);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipdnsc_hostent_insert_name
 *===========================================================================
 * Description: Inserts a host name in a hostent structure
 * Parameters:  he - pointer to the hostent structure
 *              name - the name to insert
 * Returns:     0 for OK, -1 for fail.
 */
IP_GLOBAL Ip_s32
ipdnsc_hostent_insert_name(struct Ip_hostent *he, char *name)
{
    if (he == IP_NULL)
        return -1;

    if (ipcom_strlen(name) > (IPDNSC_MAXNAME-1))
        return -1;

    /* Free the memory for the current name if any */
    if (he != IP_NULL && he->h_name != IP_NULL)
    {
        ipcom_free(he->h_name);
    }
    /* Allocate storage for the host name */
    he->h_name = ipcom_malloc(ipcom_strlen(name)+1);
    if (he->h_name == IP_NULL)
    {
        return -1;
    }
    /* Copy the host name */
    ipcom_strcpy(he->h_name, name);

    return 0;

}


/*
 *===========================================================================
 *                    ipdnsc_hostent_insert_alias
 *===========================================================================
 * Description: Inserts an alias name in a hostent structure
 * Parameters:  he - pointer to the hostent structure
 *              name - the name to insert
 * Returns:     0 for OK, -1 for fail.
 */
IP_GLOBAL Ip_s32
ipdnsc_hostent_insert_alias(struct Ip_hostent *he, char *name)
{
    Ip_s32 num_alias, i=0;
    char **tmp;

    if (ipcom_strlen(name) > (IPDNSC_MAXNAME-1))
        return -1;

    /* Find out the current number of aliases */
    num_alias = ipdnsc_hostent_alias_count(he);

    /* Allocate memory for the another alias list entry */
    tmp = ipcom_realloc(he->h_aliases, (num_alias+1) * sizeof(char *));
    if(tmp == IP_NULL)
        return -1;
    he->h_aliases = tmp;

    /* Allocate memory for the alias */
    he->h_aliases[num_alias-1] = ipcom_malloc(ipcom_strlen(name)+1);
    if (he->h_aliases[num_alias-1] == IP_NULL)
    {
        /* We have to free to whole list here */
        while (he->h_aliases[i] != IP_NULL)
        {
		    ipcom_free(he->h_aliases[i]);
            i++;
        }
        /* Free the alias list */
	    ipcom_free(he->h_aliases);
        he->h_aliases = IP_NULL;
        return -1;
    }

    /* Set the alias */
    ipcom_strcpy(he->h_aliases[num_alias-1], name);

    /* Null terminate the list */
    he->h_aliases[num_alias] = IP_NULL;

    return 0;

}


/*
 *===========================================================================
 *                    ipdnsc_hostent_insert_addr
 *===========================================================================
 * Description: Inserts an address in a hostent structure
 * Parameters:  he - pointer to the hostent structure
 *              addr - the addr to insert
 * Returns:     0 for OK, -1 for fail.
 */
IP_GLOBAL Ip_s32
ipdnsc_hostent_insert_addr(struct Ip_hostent *he, char *addr)
{
    Ip_s32 num_addr, i=0;
    char **tmp;

    /* Find out the current number of addresses */
    num_addr = ipdnsc_hostent_addr_count(he);

    /* Allocate memory for the another address list entry */
    tmp = ipcom_realloc(he->h_addr_list, (num_addr+1) * sizeof(char *));
    if(tmp == IP_NULL)
        return -1;
    he->h_addr_list = tmp;

    /* Allocate memory for the address */
    he->h_addr_list[num_addr-1] = ipcom_malloc(he->h_length);
    if (he->h_addr_list[num_addr-1] == IP_NULL)
    {
        /* We have to free to whole list here */
        while (he->h_addr_list[i] != IP_NULL)
        {
		    ipcom_free(he->h_addr_list[i]);
            i++;
        }
        /* Free the address list */
	    ipcom_free(he->h_addr_list);
        he->h_addr_list = IP_NULL;
        return -1;
    }

    /* Set the address */
    ipcom_memcpy(he->h_addr_list[num_addr-1], addr, he->h_length);

    /* Null terminate the list */
    he->h_addr_list[num_addr] = IP_NULL;

    return 0;

}


/*
 *===========================================================================
 *                    ipdnsc_hostent_create
 *===========================================================================
 * Description: Creates a hostent structure with empty alias and address
 *              lists
 * Parameters:  type  - the type of hostent structure
 * Returns:     pointer to the newly created hostent structure
 */
IP_GLOBAL struct Ip_hostent*
ipdnsc_hostent_create(Ip_s32 type)
{
    struct Ip_hostent *he;

    /* Allocate the host entry structure */
    he = ipcom_malloc(sizeof(*he));
    if (he == IP_NULL)
    {
        return IP_NULL;
    }
    ipcom_memset(he, 0, sizeof(*he));

    /* Set the type and length fields */
    he->h_addrtype = type;
    if (type == IP_AF_INET)
    {
        he->h_length = IPDNSC_INADDRSZ;
    }
    else if (type == IP_AF_INET6)
    {
        he->h_length = IPDNSC_IN6ADDRSZ;
    }
    else
    {
        ipcom_free(he);
        return IP_NULL;
    }

    /* Allocate memory for the alias list */
    he->h_aliases = ipcom_malloc(sizeof(char *));
    if (he->h_aliases == IP_NULL)
    {
        ipcom_free(he);
        return IP_NULL;
    }
    he->h_aliases[0] = IP_NULL;

    /* Allocate memory for the address list */
    he->h_addr_list = ipcom_malloc(sizeof(char *));
    if (he->h_addr_list == IP_NULL)
    {
        ipcom_free(he->h_aliases);
        ipcom_free(he);
        return IP_NULL;
    }
    he->h_addr_list[0] = IP_NULL;

    return he;
}


/*
 *===========================================================================
 *                    ipdnsc_hostent_free
 *===========================================================================
 * Description: Frees all memory allocated in a hostent structure
 * Parameters:  he - pointer to the hostent structure
 * Returns:     Nothing
 */
IP_GLOBAL void
ipdnsc_hostent_free(struct Ip_hostent *he)
{
    Ip_s32 i;

    if (he == IP_NULL)
        return;

    /* Free the host name */
    if (he->h_name)
    {
        ipcom_free(he->h_name);
    }

    /* Free all host addresses */
    if (he->h_addr_list)
    {
        i=0;
        while (he->h_addr_list[i] != IP_NULL)
        {
		    ipcom_free(he->h_addr_list[i]);
            i++;
        }
        /* Free the host address list */
        ipcom_free(he->h_addr_list);
    }

    /* Free all aliases */
    if (he->h_aliases)
    {
        i=0;
        while (he->h_aliases[i] != IP_NULL)
        {
		    ipcom_free(he->h_aliases[i]);
            i++;
        }
        /* Free the alias list */
	    ipcom_free(he->h_aliases);
    }

    /* Free the host entry structure */
    ipcom_free(he);

}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_freehostent
 *===========================================================================
 * Description: See ipdnsc.h for description.
 * Parameters:
 * Returns:
 */
IP_PUBLIC void
ipdnsc_freehostent(struct Ip_hostent *he)
{
    ipdnsc_hostent_free(he);
}


/*
 *===========================================================================
 *                    ipdnsc_getipnodebyname
 *===========================================================================
 * Description: See ipdnsc.h for description.
 * Parameters:
 * Returns:
 */
IP_PUBLIC struct Ip_hostent*
ipdnsc_getipnodebyname(const char *name, int af, int flags, int *error_num)
{
	int literal_ipv4;
    int ifc_ipv4 = 1;
    struct Ip_in_addr inaddr4;
    int literal_ipv6;
    int ifc_ipv6 = 1;
#ifdef IPCOM_USE_INET6
	struct Ip_in6_addr inaddr6;
#endif
    struct Ip_hostent *he, *he_ipv6, *he_ipv4, *he_ipv4_mapped;
    Ipdnsc_ns_spec dns;

    he = he_ipv6 = he_ipv4 = he_ipv4_mapped = IP_NULL;

    /* Check if supplied name is a literal address string */
    literal_ipv4 = ipcom_inet_pton(IP_AF_INET, (IP_CONST char *)name, &inaddr4);

#ifdef IPCOM_USE_INET6
    literal_ipv6 = ipcom_inet_pton(IP_AF_INET6, (IP_CONST char *)name, &inaddr6);
#else
    literal_ipv6 = 0;
#endif

    /* Check if active interface flag is given */
	if ((flags & IPDNSC_AI_ADDRCONFIG) != 0)
    {
        ifc_ipv4 = 0;

#ifndef IPCOM_USE_INET6
        ifc_ipv6 = 0;
#endif /* IPCOM_USE_INET6 */
    }

    /************* Check arguments ***************/
    *error_num = IPDNSC_HOST_NOT_FOUND;
    if (af != IP_AF_INET && af != IP_AF_INET6)
    {
        /* Address family not supported */
  	    *error_num = IPDNSC_NO_RECOVERY;
        return IP_NULL;
    }
    if (literal_ipv6 && (af == IP_AF_INET))
    {
        /* Address family AF_INET not consistent with literal ipv6 address */
  	    *error_num = IPDNSC_NO_RECOVERY;
        return IP_NULL;
    }
    if (literal_ipv4 && (af == IP_AF_INET6) && !(flags & IPDNSC_AI_V4MAPPED))
    {
        /* Address family AF_INET6 not consistent with literal ipv4 address unless AI_V4MAPPED flag was set */
  	    *error_num = IPDNSC_NO_RECOVERY;
        return IP_NULL;
    }
    if (literal_ipv4 && !ifc_ipv4)
    {
        /* Literal ipv4 address not possible with no configured ipv4 interfaces */
  	    *error_num = IPDNSC_NO_RECOVERY;
        return IP_NULL;
    }
    if ((af == IP_AF_INET) && !ifc_ipv4)
    {
        /* Address family AF_INET not possible with no configured ipv4 interfaces */
  	    *error_num = IPDNSC_NO_RECOVERY;
        return IP_NULL;
    }
    if ((af == IP_AF_INET6) && !ifc_ipv6)
    {
        /* Address family AF_INET6 not possible with no configured ipv6 interfaces */
  	    *error_num = IPDNSC_NO_RECOVERY;
        return IP_NULL;
    }
    /********************* Arguments OK ***********************/

    /* If name was a literal address no resolver calls are required */
	if (literal_ipv4 || literal_ipv6)
    {
        Ip_s32 type;
        char *ptr;
        Ip_u8 addr[16], buf[40];
        Ip_u8 in6addr_mapped[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff};

        if (literal_ipv4)
        {
            if (flags & IPDNSC_AI_V4MAPPED)
            {
                type = IP_AF_INET6;
                ipcom_memcpy(addr, in6addr_mapped, sizeof(in6addr_mapped));
                ipcom_memcpy(addr+sizeof(in6addr_mapped), (char *)&inaddr4, IPDNSC_INADDRSZ);
                ptr = (char *)addr;
                if (ipcom_inet_ntop(IP_AF_INET6, addr, (char *)buf, sizeof(buf)) == IP_NULL)
                {
                    return IP_NULL;
                }
                name = (char *)buf;
            }
            else
            {
                type = IP_AF_INET;
                ptr = (char *)&inaddr4;
            }
        }
#ifdef IPCOM_USE_INET6
        else if (literal_ipv6)
        {
            type = IP_AF_INET6;
            ptr = (char *)&inaddr6;
        }
#endif
        else
        {
            return IP_NULL;
        }

        /* Create a host entry structure */
        he = ipdnsc_hostent_create(type);
        if (!he)
        {
            return IP_NULL;
        }
        /* Insert the address */
        if (ipdnsc_hostent_insert_addr(he, ptr))
        {
            goto err_out;
        }
        /* Insert the hostname */
        if (ipdnsc_hostent_insert_name(he, (char *)name))
        {
            goto err_out;
        }

        return he;
	}


    /************** Supplied name was a domain name ******************/
    if(ipdnsc.init != IP_TRUE)
    {
        *error_num = IPDNSC_NO_RECOVERY;
        return IP_NULL;
    }

    /* Configure the DNS */
    if (ipdnsc_default(&dns))
    {
        *error_num = IPDNSC_NO_RECOVERY;
	    return IP_NULL;
    }

    /* Resolve the ipv6 address if required */
    if (ifc_ipv6 && af == IP_AF_INET6)
    {
        he_ipv6 = ipdnsc_resolve_name((Ip_u8 *)name, IP_AF_INET6, IP_TRUE, &dns, (Ip_s32 *)error_num);
        if (!he_ipv6 && !(flags & IPDNSC_AI_V4MAPPED))
        {
            goto err_out;
        }
	}

    /* Resolve the ipv4 address if required */
    if (ifc_ipv4)
    {
        if (af == IP_AF_INET)
        {
            he_ipv4 = ipdnsc_resolve_name((Ip_u8 *)name, IP_AF_INET, IP_TRUE, &dns, (Ip_s32 *)error_num);
            if (!he_ipv4)
            {
                goto err_out;
            }
        }
        else
        {
            /* mapped ipv4 addresses always wanted ||
               mapped ipv4 addresses wanted if no ipv6 addresses found */
            if (((flags & IPDNSC_AI_V4MAPPED) && (flags & IPDNSC_AI_ALL)) || ((flags & IPDNSC_AI_V4MAPPED) && !he_ipv6))
            {
                he_ipv4 = ipdnsc_resolve_name((Ip_u8 *)name, IP_AF_INET, IP_TRUE, &dns, (Ip_s32 *)error_num);

                if (!he_ipv4 && !he_ipv6)
                {
                    goto err_out;
                }

                if (he_ipv4)
                {
                    he_ipv4_mapped = ipdnsc_hostent_convert(he_ipv4);
                    ipdnsc_hostent_free(he_ipv4);
                    he_ipv4 = IP_NULL;
                    if (!he_ipv4_mapped)
                    {
                        goto err_out;
                    }
                }
            }
        }
    }

    if (he_ipv6 && he_ipv4_mapped)
    {
        he = ipdnsc_hostent_merge(he_ipv6, he_ipv4_mapped);
        ipdnsc_hostent_free(he_ipv6);
        ipdnsc_hostent_free(he_ipv4_mapped);
    }
    else if (he_ipv6 && !he_ipv4_mapped)
    {
        he = he_ipv6;
    }
    else if (!he_ipv6 && he_ipv4_mapped)
    {
        he = he_ipv4_mapped;
    }
    else
    {
        he = he_ipv4;
    }

    return (he);

err_out:
    if (he_ipv6)
        ipdnsc_hostent_free(he_ipv6);

    if (he_ipv4)
        ipdnsc_hostent_free(he_ipv4);

    if (he_ipv4_mapped)
        ipdnsc_hostent_free(he_ipv4_mapped);


    ipdnsc_hostent_free(he);
    return IP_NULL;

}


/*
 *===========================================================================
 *                    ipdnsc_getipnodebyaddr
 *===========================================================================
 * Description: See ipdnsc.h for description.
 * Parameters:
 * Returns:
 */
IP_PUBLIC struct Ip_hostent*
ipdnsc_getipnodebyaddr(const void *src, Ip_size_t len, int af, int *error_num)
{
    Ip_u8 *ipaddr = (Ip_u8 *)src;
    struct Ip_hostent *he = IP_NULL;
    struct Ip_hostent *he_ipv4;
#ifdef IPCOM_USE_INET6
    struct Ip_hostent *he_ipv6;
#endif
    Ipdnsc_ns_spec dns;

    /* Check that an address was supplied */
	if (ipaddr == IP_NULL)
    {
		*error_num = IPDNSC_NO_RECOVERY;
		return IP_NULL;
	}

    /* Check remaining arguments */
    if (af == IP_AF_INET)
    {
		if (len != IPDNSC_INADDRSZ)
        {
			*error_num = IPDNSC_NO_RECOVERY;
			return IP_NULL;
		}
    }
    else if (af == IP_AF_INET6)
    {
		if (len != IPDNSC_IN6ADDRSZ)
        {
			*error_num = IPDNSC_NO_RECOVERY;
			return IP_NULL;
		}
    }
    else
    {
        *error_num = IPDNSC_NO_RECOVERY;
	    return IP_NULL;
    }

    if(ipdnsc.init != IP_TRUE)
    {
        *error_num = IPDNSC_NO_RECOVERY;
        return IP_NULL;
    }

    /* Configure the DNS */
    if (ipdnsc_default(&dns))
    {
        *error_num = IPDNSC_NO_RECOVERY;
	    return IP_NULL;
    }

    /* First: resolve ipv4 mapped or ipv4 compatible addresses */
#ifdef IPCOM_USE_INET6
	if (af == IP_AF_INET6)
    {
        if (IP_IN6_IS_ADDR_V4COMPAT((struct Ip_in6_addr *)ipaddr) ||
            IP_IN6_IS_ADDR_V4MAPPED((struct Ip_in6_addr *)ipaddr))
        {
            he_ipv4 = ipdnsc_resolve_addr(ipaddr+12, IP_AF_INET, IP_TRUE, &dns, (Ip_s32 *)error_num);
		    if (he_ipv4 == IP_NULL)
            {
			    return IP_NULL;
		    }

            /* Convert the resolved address to an ipv4 mapped ipv6 address */
            he = ipdnsc_hostent_convert(he_ipv4);
            if (!he)
            {
                ipdnsc_hostent_free(he_ipv4);
			    return IP_NULL;
            }
            ipdnsc_hostent_free(he_ipv4);

            /* Copy back the first 12 bytes - in case it was a ipv4 compatible address */
            ipcom_memcpy(he->h_addr_list[0], ipaddr, 12);

            /* We're  done */
            goto out;
        }
    }
#endif

    /* Second: resolve ipv4 address */
	if (af == IP_AF_INET)
    {
        he_ipv4 = ipdnsc_resolve_addr(ipaddr, IP_AF_INET, IP_TRUE, &dns, (Ip_s32 *)error_num);
		if (he_ipv4 == IP_NULL)
        {
            return IP_NULL;
		}
        he = he_ipv4;

        /* We're  done */
        goto out;
    }

#ifdef IPCOM_USE_INET6
    /* Third: resolve ipv6 address */
	if (af == IP_AF_INET6)
    {
        if (IP_IN6_IS_ADDR_UNSPECIFIED((struct Ip_in6_addr *)ipaddr))
        {
		    *error_num = IPDNSC_HOST_NOT_FOUND;
		    return IP_NULL;
	    }

        he_ipv6 = ipdnsc_resolve_addr(ipaddr, IP_AF_INET6, IP_TRUE, &dns, (Ip_s32 *)error_num);
		if (he_ipv6 == IP_NULL)
        {
		    return IP_NULL;
		}
        he = he_ipv6;

        /* We're  done */
        goto out;
    }
#endif

out:
    return he;

}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
