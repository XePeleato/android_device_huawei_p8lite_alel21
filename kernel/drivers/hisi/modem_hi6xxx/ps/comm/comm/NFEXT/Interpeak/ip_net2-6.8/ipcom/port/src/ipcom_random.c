/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_random.c,v $ $Revision: 1.43.24.1 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_random.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_random.c,v $ $Revision: 1.43.24.1 $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Roger Boden <roger@interpeak.se>
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

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_KERNEL
#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_hash.h>
#include <ipcom_os.h>
#include <ipcom_syslog.h>
#include <ipcom_time.h>
#include <ipcom_tmo.h>
#include <ipcom_type.h>

#ifdef IPCOM_USE_EGD
#ifdef IPMCRYPTO
#include <ip/openssl/md5.h>
#else
#ifdef OPENSSL_FIPS
#include <openssl/sha.h>
#else
#include <openssl/md5.h>
#endif /* OPENSSL_FIPS */
#endif /* IPMCRYPTO */
#endif /* IPCOM_USE_EGD */


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#if defined(IP_PORT_VXWORKS) && (CPU==SIMLINUX || CPU==SIMNT || CPU==SIMSOLARIS || CPU==SIMSPARCSOLARIS) \
    || defined(IP_PORT_UNIX)

#  ifndef IPCOM_RANDOM_LAPS
#    define IPCOM_RANDOM_LAPS   16
#  endif
#  define IPCOM_EGD_SLEEP_TIME 950
#else
#  ifndef IPCOM_RANDOM_LAPS
#    define IPCOM_RANDOM_LAPS   128
#  endif
#  define IPCOM_EGD_SLEEP_TIME 450
#endif

/* #define IPCOM_EGD_DEBUG */

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM

#ifdef OPENSSL_FIPS
#define ipcom_egd_hash_init SHA1_Init
#define ipcom_egd_hash_update SHA1_Update
#define ipcom_egd_hash_final SHA1_Final
#define Ipcom_egd_hash_ctx SHA_CTX
#else
#define ipcom_egd_hash_init MD5_Init
#define ipcom_egd_hash_update MD5_Update
#define ipcom_egd_hash_final MD5_Final
#define Ipcom_egd_hash_ctx MD5_CTX
#endif

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
#ifdef IP_PORT_UNIX
    extern Ip_u32 pthread_self(void);
#endif

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_GLOBAL Ip_err ipcom_random_init(void);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

#ifdef IPCOM_USE_EGD
IP_STATIC int    ipcom_egd_laps;
#ifdef OPENSSL_FIPS
IP_STATIC Ip_u8  ipcom_random_state[20];
#else
IP_STATIC Ip_u8  ipcom_random_state[16];
#endif
IP_STATIC volatile int ipcom_egd_tmo_flag;

#ifdef IPCOM_EGD_DEBUG
IP_STATIC Ip_s32 ipcom_egd_raw_data[IPCOM_RANDOM_LAPS];
#endif
#else
IP_STATIC Ip_u32 ipcom_rnd_seed = 1;
IP_STATIC Ip_u32 ipcom_rnd_state = 0;
#endif /* IPCOM_USE_EGD */

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

#ifdef IPCOM_USE_EGD
/*
 *===========================================================================
 *                    ipcom_random_tmo_handler
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_s32
ipcom_random_tmo_handler(struct Ipcom_tmo_struct *tmo, void* cookie)
{
    int* ptmo_flag = (int*) cookie;

    (void) tmo;

    *ptmo_flag = 1;

    /* Do not request another timeout. */
    return 0;
}

/*
 *===========================================================================
 *                    ipcom_random_bingo_lotto
 *===========================================================================
 * Description: This function gathers random data. This is achieved by requesting a timeout
 *              callback each 50 milli second and counting the number of laps the process
 *              is able to perform before the timeout signal. Since the process runs
 *              as a low priority process many other processes will be scheduled in before
 *              ipcom_egd gets any CPU cycles. This is actually the "phenomenon" that
 *              creates the randomness.
 * Parameters:
 * Returns:
 */
IP_STATIC Ip_s32
ipcom_random_bingo_lotto(void)
{
    int laps = 0;

    while(ipcom_egd_tmo_flag == 0)
    {
#ifdef IP_PORT_LYNXOS
	yield();
#else
#if (defined(IP_PORT_OSE) || defined(IP_PORT_OSE5)) && (defined(IP_TARGET_SOFT) || defined(softOSE) || defined(IP_TARGET_SOLARIS2) || defined(IP_TARGET_LINUX) || defined(IP_TARGET_WIN32) || defined(IP_TARGET_BSD))
        delay(1);
#endif
#endif
        laps++;
    }

    return laps;
}


/*
 *===========================================================================
 *                    ipcom_random_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_random_get(Ip_u8 *buf)
{
    Ipcom_egd_hash_ctx ctx;
    static Ip_u32 counter = 0;

    counter++;
    ipcom_egd_hash_init(&ctx);
    ipcom_egd_hash_update(&ctx, ipcom_random_state, sizeof(ipcom_random_state));
    ipcom_egd_hash_update(&ctx, (void*) &counter, sizeof(counter));
    ipcom_egd_hash_final(ipcom_random_state, &ctx);

    ipcom_memcpy(buf, ipcom_random_state, sizeof(ipcom_random_state));
}


/*
 *===========================================================================
 *                    ipcom_egd
 *===========================================================================
 * Description: This process gathers random data by calling ipcom_random_bingo_lotto()
 *
 *              Applications can access the random data before the full entropy
 *              has been gathered. This will of course mean that the quality of
 *              the random data will not be as good, but in some situations that
 *              may be a better trade-off than no random data at all.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC
IPCOM_PROCESS(ipcom_egd)
{
    Ipcom_egd_hash_ctx md5_ctx;
    Ipcom_egd_hash_ctx tmp_md5_ctx;
    Ip_s32 rnd;
    Ipcom_tmo tmo;
#ifdef IPCOM_EGD_DEBUG
    int i;
#endif

    ipcom_proc_init();

    IPCOM_LOG0(DEBUG, "ipcom_egd :: starting");

    ipcom_egd_hash_init(&md5_ctx);
    ipcom_egd_laps = 0;
#ifdef IPCOM_EGD_DEBUG
    ipcom_memset(ipcom_egd_raw_data, 0, sizeof(ipcom_egd_raw_data));
#endif

    while(ipcom_egd_laps < IPCOM_RANDOM_LAPS)
    {
        ipcom_egd_tmo_flag = 0;
        if(ipcom_tmo_request(&tmo, ipcom_random_tmo_handler, (int*)&ipcom_egd_tmo_flag, 50) != IPCOM_SUCCESS)
        {
            IPCOM_LOG0(ERR, "ipcom_egd :: timeout request failed, seed aborted");
            goto exit;
        }
        rnd = ipcom_random_bingo_lotto();
#ifdef IPCOM_EGD_DEBUG
        ipcom_egd_raw_data[ipcom_egd_laps] = rnd;
#endif
        ipcom_egd_hash_update(&md5_ctx, (void*) &rnd, sizeof(Ip_u32));
        ipcom_memcpy(&tmp_md5_ctx, &md5_ctx, sizeof(Ipcom_egd_hash_ctx));
        ipcom_egd_hash_final(ipcom_random_state, &tmp_md5_ctx);
        ipcom_egd_laps++;
        ipcom_millisleep(IPCOM_EGD_SLEEP_TIME);
    }

 exit:
#ifdef IPCOM_EGD_DEBUG
    for (i=0; i < ipcom_egd_laps; i++)
    {
        IPCOM_LOG2(INFO, "ipcom_egd_raw_data[%d] = %d", i, ipcom_egd_raw_data[i]);
        ipcom_millisleep(100);
    }
#endif
    IPCOM_LOG0(DEBUG, "ipcom_egd :: terminating");

    ipcom_proc_exit();
}


/*
 *===========================================================================
 *                    ipcom_egd_random
 *===========================================================================
 * Description: This function will return the required number of random data,
 *              regardless of the seed state. A caller that requires high quality
 *              random data should check the random seed state first by calling
 *              ipcom_egd_random_seed_state().
 * Parameters:  rand_size:
 *                  the number of random bytes requested.
 *              buf:
 *                  the buffer to store the random data in.
 * Returns:
 */
IP_STATIC void
ipcom_egd_random(Ip_u8 *buf, Ip_s32 rand_size)
{
    Ip_u8 tmp_buf[sizeof(ipcom_random_state)];

    while(rand_size >= (Ip_s32) sizeof(ipcom_random_state))
    {
        ipcom_random_get(buf);
        buf += sizeof(ipcom_random_state);
        rand_size -= sizeof(ipcom_random_state);
    }

    if(rand_size > 0)
    {
        ipcom_random_get(tmp_buf);
        ipcom_memcpy(buf, tmp_buf, rand_size);
    }
}

#endif /* IPCOM_USE_EGD */


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_random_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipcom_random_init(void)
{
    struct Ip_timeval tval;
    Ip_err rc = IPCOM_SUCCESS;
    unsigned int deliberately_uninitialized;

    /* Make sure we have some random seed at boot. Good in general and in
     * particular since IPNET uses ipcom_random to generate random MAC
     * addresses in the unix port. */
    (void) ipcom_gettimeofday(&tval, IP_NULL);
    ipcom_srandom((unsigned int)(tval.tv_sec + tval.tv_usec));
    ipcom_srandom(ipcom_getpid());
    ipcom_srandom(deliberately_uninitialized);

#ifdef IPCOM_USE_EGD
    {
        Ipcom_proc_attr  attr;

        ipcom_proc_attr_init(&attr);
        attr.priority = IPCOM_PRIORITY_BACKGROUND;
        attr.flags = IPCOM_PROC_FLAG_FP;
        rc = ipcom_proc_acreate("ipcom_egd", ipcom_egd, &attr, IP_NULL);
    }
#endif

    return rc;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_random
 *===========================================================================
 * Description: Returns a pseudo-random number in the range [0..4294967295]
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_RANDOM) && IPCOM_RANDOM == 1
IP_PUBLIC Ip_u32
ipcom_random(void)
{
    Ip_u32 random_val;

#ifdef IPVALGRIND
    random_val = 0;
#else
#ifdef IPCOM_USE_EGD
    ipcom_egd_random((Ip_u8*)&random_val, sizeof(random_val));
#else
    random_val = ipcom_rnd_state;

#ifdef IPCOM_USE_TIME
    {
        struct Ip_tms tms;
        random_val += (Ip_u32)ipcom_times(&tms);
    }
#endif

    random_val = (Ip_u32)ipcom_hash_update(&random_val,
                                           sizeof(random_val),
                                           ipcom_rnd_seed);
    ipcom_rnd_state = random_val;
#endif
#endif /* IPVALGRIND */

    return random_val;
}
#endif


/*
 *===========================================================================
 *                    ipcom_srandom
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_SRANDOM) && IPCOM_SRANDOM == 1
IP_PUBLIC void
ipcom_srandom(unsigned int seed)
{
#ifdef IPCOM_USE_EGD
    Ip_u8 *pseed;
    unsigned int i;
    int j;

    pseed = (Ip_u8*)&seed;

    for (i=0,j=0; i<sizeof(ipcom_random_state); i++)
    {
        ipcom_random_state[i] += pseed[j];
        j++;
        if (j%sizeof(unsigned int) == 0)
            j = 0;
    }

#else
    ipcom_rnd_seed += seed;
#endif
}
#endif


/*
 *===========================================================================
 *                    ipcom_rand
 *===========================================================================
 * Description: Returns a pseudo-random number in the range [0..IP_RAND_MAX]
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_RAND) && IPCOM_RAND == 1
IP_PUBLIC int
ipcom_rand(void)
{
    long rnd = ipcom_random();
    if (rnd < 0)
        rnd = -rnd;
    return (int)IP_MIN(rnd, IP_RAND_MAX);
}
#endif


/*
 *===========================================================================
 *                    ipcom_srand
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_SRAND) && IPCOM_SRAND == 1
IP_PUBLIC void
ipcom_srand(unsigned int seed)
{
    ipcom_srandom(seed);
}
#endif


/*
 *===========================================================================
 *                    ipcom_random_seed_state
 *===========================================================================
 * Description:
 *   Returns a percentage (0-100) indicating how far the seeding of the
 *   random generator has proceeded.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_s32
ipcom_random_seed_state(void)
{
#ifdef IPCOM_USE_EGD
    return (ipcom_egd_laps*100) / IPCOM_RANDOM_LAPS;
#else
    return 100;
#endif
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
