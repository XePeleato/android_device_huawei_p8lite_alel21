

#ifndef __HWIFI_BUS_H__
#define __HWIFI_BUS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */

#include "common.h"
#include "hwifi_utils.h"

/*
 * 2 Macro Definition
 */


/*
 * 3 Enum Type Definition
 */


/*
 * 4 Global Variable Declaring
 */


/*
 * 5 Message Header Definition
 */


/*
 * 6 Message Definition
 */


/*
 * 7 STRUCT Type Definition
 */
struct iodevice
{
    int (*enable)(struct iodevice *dev);
    int (*disable)(struct iodevice *dev);
    int (*transfer)(struct iodevice *dev, unsigned char *buf, uint32 count);
    int (*transfer_sg)(struct iodevice *io_dev, struct scatterlist *sg, uint32 sg_len, uint32 rw_sz);
    uint8 (*get_state)(struct iodevice *dev);
    void (*set_state)(struct iodevice *dev, uint8 state);
    int (*reset_flowctrl)(struct iodevice *dev);
    int (*interrupt_pending)(struct iodevice *dev);
    uint32 (*get_max_transfer_size)(struct iodevice *dev);

    /*assem sdio channel info reset*/
    void (*rx_info_reset)(struct iodevice *, uint8);
    int32 (*suspend)(struct iodevice *);
    int32 (*resume)(struct iodevice *);
};


/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */
STATIC inline int hsdio_bus_enable(struct iodevice *io_dev)
{
    if (unlikely((NULL == io_dev) || (NULL == io_dev->enable)))
    {
        HWIFI_WARNING("io device is NULL");
        return -EFAIL;
    }

    return io_dev->enable(io_dev);
}

STATIC inline int hsdio_bus_disable(struct iodevice *io_dev)
{
    if (unlikely((NULL == io_dev) || (NULL == io_dev->disable)))
    {
        HWIFI_WARNING("io device is NULL");
        return -EFAIL;
    }

    return io_dev->disable(io_dev);
}

#ifdef WLAN_PERFORM_DEBUG
int bus_transfer(struct iodevice *io_dev, unsigned char *buf, uint32 count);

#else
STATIC inline int bus_transfer(struct iodevice *io_dev, unsigned char *buf, uint32 count)
{
    if (unlikely((NULL == io_dev) || (NULL == io_dev->transfer)))
    {
        HWIFI_WARNING("io device is NULL");
        return -EFAIL;
    }

    if (unlikely((NULL == buf) || (0 == count)))
    {
        HWIFI_WARNING("buf or count is 0");
        return SUCC; /* report succ */
    }

    return io_dev->transfer(io_dev, buf, count);
}

STATIC inline int bus_transfer_sg(struct iodevice *io_dev, struct scatterlist *sg, 
                                    uint32 sg_len, uint32 rw_sz)
{
    HWIFI_BUG_ON(!io_dev);
    if(HWIFI_WARN_ON(!io_dev->transfer_sg))
    {
        return -EFAIL;
    }
    return io_dev->transfer_sg(io_dev, sg, sg_len, rw_sz);
}

#endif

STATIC  inline uint8 bus_get_state(struct iodevice *io_dev)
{
    HWIFI_BUG_ON(!io_dev);
    HWIFI_BUG_ON(!io_dev->get_state) ;   
    return io_dev->get_state(io_dev);
}
STATIC  inline void bus_set_state(struct iodevice *io_dev, uint8 state)
{
    HWIFI_BUG_ON(!io_dev);
    HWIFI_BUG_ON(!io_dev->set_state) ;   
    io_dev->set_state(io_dev, state);
}

STATIC  inline void bus_rx_info_reset(struct iodevice *io_dev, uint8 locked)
{
    HWIFI_BUG_ON(!io_dev);
    HWIFI_BUG_ON(!io_dev->rx_info_reset) ;
    io_dev->rx_info_reset(io_dev, locked);
}

STATIC inline int32 hsdio_bus_suspend(struct iodevice *io_dev)
{
    HWIFI_BUG_ON(!io_dev);
    HWIFI_BUG_ON(!io_dev->suspend);
    return io_dev->suspend(io_dev);
}

STATIC inline int32 hsdio_bus_resume(struct iodevice *io_dev)
{
    HWIFI_BUG_ON(!io_dev);
    HWIFI_BUG_ON(!io_dev->resume);
    return io_dev->resume(io_dev);
}

STATIC inline int bus_reset_flowctrl(struct iodevice *io_dev)
{
    if (unlikely((NULL == io_dev) || (NULL == io_dev->reset_flowctrl)))
    {
        HWIFI_WARNING("io device is NULL");
        return -EFAIL;
    }

    return io_dev->reset_flowctrl(io_dev);
}

STATIC inline int bus_interrupt_pending(struct iodevice *io_dev)
{
    if (unlikely((NULL == io_dev) || (NULL == io_dev->interrupt_pending)))
    {
        HWIFI_WARNING("io device is NULL");
        return 0; /* Î´ÓÐÖÐ¶Ï */
    }

    return io_dev->interrupt_pending(io_dev);
}

STATIC inline uint32 bus_get_max_transfer_size(struct iodevice *io_dev)
{
    if (unlikely((NULL == io_dev) || (NULL == io_dev->get_max_transfer_size)))
    {
        HWIFI_WARNING("io device is NULL");
        return 0;
    }

    return io_dev->get_max_transfer_size(io_dev);
}



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwifi_bus.h */
