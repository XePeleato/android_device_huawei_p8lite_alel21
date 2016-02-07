#include "gtest/gtest.h"
#include "llt_mockcpp.h"

using namespace testing;
#ifdef __cplusplus
extern "C"
{
#endif
    /* 被测函数 */
    extern void *kmalloc(int, int);
    extern int acm_async_write(void *acm_dev, char *pVirAddr, char *pPhyAddr, unsigned int size);

    extern int test_udi_acm_init(void);
    extern int test_udi_acm_open_invalid_devid(void);
    extern int test_udi_acm_open_success(void);
    extern int test_acm_open_dev_path_null(void);
    extern int test_acm_open_reopen(void);
    extern int test_acm_open_success(void);
    extern int test_udi_acm_close_success(void);
    extern int test_udi_acm_read_not_openned(void);
    extern int test_udi_acm_read_buf_null(void);
    extern int test_udi_acm_read_size_zero(void);
    extern int test_udi_acm_read_success(void);
    extern int test_udi_acm_write_not_openned(void);
    extern int test_udi_acm_write_buf_null(void);
    extern int test_udi_acm_write_size_zero(void);
    extern int test_udi_acm_write_success(void);
    extern int test_udi_acm_ioctl_not_openned(void);
    extern int test_udi_acm_ioctl_acm_set_read_cb(void);
    extern int test_udi_acm_ioctl_udi_set_read_cb(void);
    extern int test_udi_acm_ioctl_acm_set_write_cb(void);
    extern int test_udi_acm_ioctl_udi_set_write_cb(void);
    extern int test_udi_acm_ioctl_acm_set_event_cb(void);
    extern int test_udi_acm_ioctl_acm_set_free_cb(void);
    extern int test_udi_acm_ioctl_write_async_no_buf(void);
    extern int test_udi_acm_ioctl_write_async_zero_len(void);
    extern int test_udi_acm_ioctl_write_async_success(void);
    extern int test_udi_acm_ioctl_get_read_buf(void);
    extern int test_udi_acm_ioctl_return_buf(void);
    extern int test_udi_acm_ioctl_relloc_read_buf(void);
    extern int test_udi_acm_ioctl_import_done(void);
    extern int test_udi_acm_ioctl_send_buf_can_dma(void);
    extern int test_udi_acm_ioctl_write_do_copy(void);
    extern int test_udi_acm_ioctl_invalid_cmd(void);
    extern int test_acm_async_write_not_openned(void);
    extern int test_acm_async_write_buf_null(void);
    extern int test_acm_async_write_size_zero(void);
    extern int test_acm_async_write_success(void);
	extern int acm_ncopy_queue(struct acm_ncopy_ctx * acm_ncopy, unsigned char *buf, unsigned int size);

	
	extern int test_get_dev_ctx(unsigned int dev_id);
	extern void* acm_ncopy_port_ready(void *tty);
    extern int atomic_notifier_chain_register(struct atomic_notifier_head *nh,
		struct notifier_block *n);

#ifdef __cplusplus
}
#endif

struct gserial_str_stub {
    int *in;
    int *out;
};

struct gserial_str_stub g_ports;

void *ut_kmalloc(int size,int flag)
{
    return 0;
}

void *ut_acm_ncopy_port_ready(void *tty)
{
    return ((void *) &g_ports);
}

int ut_atomic_notifier_chain_register(struct atomic_notifier_head *nh,
		struct notifier_block *n)
{
}
/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_init, UT002_acm_init_success)
{
    int ret = 0;
    MOCKER(atomic_notifier_chain_register).stubs().will(invoke(ut_atomic_notifier_chain_register));
    ret = test_udi_acm_init();
    EXPECT_EQ(0, ret);
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_open, UT001_invalid_devid)
{
    int ret = 0;

    ret = test_udi_acm_open_invalid_devid();
    EXPECT_EQ(-1, ret);

}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_open, UT002_udi_open_success)
{
    int ret = 0;

    MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_open_success();
    EXPECT_EQ(0, ret);
	GlobalMockObject::reset();

}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_acm_open, UT003_dev_path_null)
{
    int ret = 0;

    ret = test_acm_open_dev_path_null();
    EXPECT_EQ(0, ret);
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_acm_open, UT004_acm_reopen)
{
    int ret = 0;

    ret = test_acm_open_reopen();
    EXPECT_EQ(0, ret);

}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_acm_open, UT005_acm_open_success)
{
    int ret = 0;

    ret = test_acm_open_success();
    EXPECT_EQ(0, ret );
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/

TEST(test_udi_acm_close, UT001_acm_close_success)
{
    int ret = 0;

	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_close_success();
    EXPECT_EQ(0, ret);
   	GlobalMockObject::reset();
}



/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/

TEST(test_udi_acm_read, UT001_udi_not_open)
{
    int ret = 0;

    ret = test_udi_acm_read_not_openned();
    EXPECT_EQ(-1, ret);
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/

TEST(test_udi_acm_read, UT002_acm_buf_null)
{
    int ret = 0;

    ret = test_udi_acm_read_buf_null();
    EXPECT_EQ(-1, ret);
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/

TEST(test_udi_acm_read, UT003_acm_size_zero)
{
    int ret = 0;

    ret = test_udi_acm_read_size_zero();
    EXPECT_EQ(-1, ret);
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/

TEST(test_udi_acm_read, UT004_acm_read_success)
{
    int ret = 0;

	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_read_success();
    EXPECT_EQ(0, ret);
   	GlobalMockObject::reset();
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/

TEST(test_udi_acm_write, UT001_acm_not_open)
{
    int ret = 0;

    ret = test_udi_acm_write_not_openned();
    EXPECT_EQ(-1, ret);
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/

TEST(test_udi_acm_write, UT002_acm_buf_null)
{
    int ret = 0;

    ret = test_udi_acm_write_buf_null();
    EXPECT_EQ(-1, ret);
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/

TEST(test_udi_acm_write, UT003_acm_size_zero)
{
    int ret = 0;

    ret = test_udi_acm_write_size_zero();
    EXPECT_EQ(-1, ret);
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/

TEST(test_udi_acm_write, UT004_acm_write_success)
{
    int ret = 0;
	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_write_success();
    EXPECT_EQ(0, ret);
	GlobalMockObject::reset();
}




/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_ioctl, UT001_acm_not_open)
{
    int ret = 0;

    ret = test_udi_acm_ioctl_not_openned();
    EXPECT_EQ(-1, ret);
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_ioctl, UT002_acm_set_read_cb)
{
    int ret = 0;

	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_ioctl_acm_set_read_cb();
    EXPECT_EQ(0, ret);
	GlobalMockObject::reset();
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_ioctl, UT003_udi_set_read_cb)
{
    int ret = 0;


	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_ioctl_udi_set_read_cb();
    EXPECT_EQ(0, ret);
	GlobalMockObject::reset();
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_ioctl, UT004_acm_set_write_cb)
{
    int ret = 0;

	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_ioctl_acm_set_write_cb();
    EXPECT_EQ(0, ret);
	GlobalMockObject::reset();
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_ioctl, UT005_udi_set_write_cb)
{
    int ret = 0;

	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_ioctl_acm_set_write_cb();
    EXPECT_EQ(0, ret);
	GlobalMockObject::reset();

}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_ioctl, UT006_acm_set_event_cb)
{
    int ret = 0;

	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_ioctl_acm_set_event_cb();
    EXPECT_EQ(0, ret);
	GlobalMockObject::reset();
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_ioctl, UT007_udi_set_free_cb)
{
    int ret = 0;

	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_ioctl_acm_set_free_cb();
    EXPECT_EQ(0, ret);
	GlobalMockObject::reset();
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_ioctl, UT008_udi_write_async_no_buf_fail)
{
    int ret = 0;

	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_ioctl_write_async_no_buf();
    EXPECT_EQ(-1, ret);
	GlobalMockObject::reset();


}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_ioctl, UT009_udi_write_async_zero)
{
    int ret = 0;

	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_ioctl_write_async_zero_len();
    EXPECT_EQ(-1, ret);
	GlobalMockObject::reset();


}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_ioctl, UT010_udi_write_async_success)
{
    int ret = 0;

	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    MOCKER(acm_async_write).stubs().will(returnValue(sizeof("abcdefg")));

    ret = test_udi_acm_ioctl_write_async_success();
    EXPECT_EQ(0, ret);
	GlobalMockObject::reset();
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_ioctl, UT011_acm_get_read_buf)
{
    int ret = 0;

	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_ioctl_get_read_buf();
    EXPECT_EQ(0, ret);
	GlobalMockObject::reset();

}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_ioctl, UT012_acm_return_buf)
{
    int ret = 0;
	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_ioctl_return_buf();
    EXPECT_EQ(0, ret);
	GlobalMockObject::reset();
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_ioctl, UT013_acm_relloc_read_buf)
{
    int ret = 0;

	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_ioctl_relloc_read_buf();
    EXPECT_EQ(0, ret);
	GlobalMockObject::reset();

}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_ioctl, UT014_acm_is_import_done)
{
    int ret = 0;
	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_ioctl_import_done();
    EXPECT_EQ(0, ret);
	GlobalMockObject::reset();
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_ioctl, UT015_acm_send_buf_can_dma)
{
    int ret = 0;

	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_ioctl_send_buf_can_dma();
    EXPECT_EQ(0, ret);
	GlobalMockObject::reset();

}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_ioctl, UT016_acm_write_do_copy)
{
    int ret = 0;

	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_ioctl_write_do_copy();
    EXPECT_EQ(0, ret);
	GlobalMockObject::reset();

}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_udi_acm_ioctl, UT017_acm_invlid_cmd)
{
    int ret = 0;

	MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
    ret = test_udi_acm_ioctl_invalid_cmd();
    EXPECT_EQ(-1, ret);
	GlobalMockObject::reset();

}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_acm_async_write, UT001_acm_not_open)
{
    int ret = 0;

    ret = test_acm_async_write_not_openned();
    EXPECT_EQ(-1, ret);
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_acm_async_write, UT002_acm_buf_null)
{
    int ret = 0;

    ret = test_acm_async_write_buf_null();
    EXPECT_EQ(-1, ret);
}


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_acm_async_write, UT003_acm_size_zero)
{
    int ret = 0;

    ret = test_acm_async_write_size_zero();
    EXPECT_EQ(-1, ret);
}

#if 0


/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/

/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_acm_async_write, UT004_acm_async_write_success)
{
    int ret = 0;

    MOCKER(acm_ncopy_port_ready).stubs().will(invoke(ut_acm_ncopy_port_ready));
	MOCKER(acm_ncopy_queue).stubs().will(returnValue(0));
    ret = test_acm_async_write_success();
    EXPECT_EQ(1, ret);
   	GlobalMockObject::reset();
}


#endif
