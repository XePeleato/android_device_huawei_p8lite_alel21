

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "linux/semaphore.h"

#include "BSP.h"
#include "rfilesystem_transfer.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

//Frame
typedef struct
{
   unsigned int opsPid;
   unsigned int totalFrame;
   unsigned int curFrame;
   unsigned int curlen;
   int reserve;
}RFILE_MNTN_HEADER;

//OPEN
typedef struct
{
    unsigned int opsType;
    unsigned int pathLen;
    unsigned int modeLen;
}RFILE_OPEN_HEAD;


typedef int (*fileOps)(RFILE_MNTN_HEADER *header, char * data);

extern RFILECONTEXT_A rFileContext_a;

extern fileOps rfileOpsSet[42];
extern RFILECONTEXT_A rFileContext_cmd;

extern int rfile_filesystem_sync(void);
extern int rfile_a_give_c_sem(void);

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
struct semaphore     sem_to_rfile_sync_req;

/*****************************************************************************
  3 函数实现
*****************************************************************************/


int set_rFileContext_a_value( int value )
{

	rFileContext_a.iccOpened = value;
    return 0;

}


int test_rfile_filesystem_sync( void )
{
     int  ret = 0;

     ret = rfile_filesystem_sync();
     if(ret < 0)
     {
         return -1;
     }

     return 0;
}


int test_rfile_a_give_c_sem( void )
{
     int  ret = 0;

     ret = rfile_a_give_c_sem();
     if(ret < 0)
     {
         return -1;
     }

     return 0;

}


int test_rfile_icc_read_cb( void )
{
     int  ret       = 0;
     int  channel_id = 0;
     int  datasize  = 0;

     ret = rfile_icc_read_cb(channel_id,datasize);

     return ret;

}


int test_rfile_transfer_init( void )
{
     int  ret       = 0;

     ret = rfile_transfer_init();
     if(-1 == ret)
     {
         return -1;
     }

     return 0;

}



int test_rfile_process_ccpu_pkt_success( void )
{
     int  ret       = 0;
     int  opstype   = 0;
     int  pathlen   = 0;
     int  modelen   = 0;

     char *         send_data_buf = NULL;
     RFILE_MNTN_HEADER header     = {0};
     RFILE_OPEN_HEAD  fopen_head  = {0};

     rfile_regProxyFunc();

     /*准备数据*/
     pathlen = strlen("/yaffs0/nv.bak");
     modelen = strlen("a+");

     /*拷贝数据*/
     send_data_buf = (char*)malloc(sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD)+ pathlen+modelen);

     memset(send_data_buf,0,sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD)+ pathlen+modelen);

     memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD),"/yaffs0/nv.bak",pathlen);

     memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD)+pathlen,"a+",modelen);

     /*fopen opstype*/
     memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER),&opstype,sizeof(unsigned int));

     /*fopen pathlen*/
     memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER)+sizeof(unsigned int),&pathlen,sizeof(unsigned int));

     /*fopen modelen*/
     memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER)+sizeof(unsigned int)*2,&modelen,sizeof(unsigned int));

     rFileContext_a.sendBuf = send_data_buf;

     ret = rfile_process_ccpu_pkt(&header);
     if(-1 == ret)
     {
         return -1;
     }

     return 0;

}
int test_rfile_process_ccpu_pkt_fail_opstype_excude( void )
{
     int  ret       = 0;
     int  opstype   = 64;
     int  pathlen   = 0;
     int  modelen   = 0;

     char *         send_data_buf = NULL;
     RFILE_MNTN_HEADER header     = {0};
     RFILE_OPEN_HEAD  fopen_head  = {0};

     rfile_regProxyFunc();

     /*准备数据*/
     pathlen = strlen("/yaffs0/nv.bak");
     modelen = strlen("a+");

     /*拷贝数据*/
     send_data_buf = (char*)malloc(sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD)+ pathlen+modelen);

     memset(send_data_buf,0,sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD)+ pathlen+modelen);

     memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD),"/yaffs0/nv.bak",pathlen);

     memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD)+pathlen,"a+",modelen);

     /*fopen opstype*/
     memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER),&opstype,sizeof(unsigned int));

     /*fopen pathlen*/
     memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER)+sizeof(unsigned int),&pathlen,sizeof(unsigned int));

     /*fopen modelen*/
     memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER)+sizeof(unsigned int)*2,&modelen,sizeof(unsigned int));

     rFileContext_a.sendBuf = send_data_buf;

     ret = rfile_process_ccpu_pkt(&header);
     if(-1 == ret)
     {
         return -1;
     }

     return 0;

}
int test_rfile_process_ccpu_pkt_fail_ops_null( void )
{
     int  ret       = 0;
     int  opstype   = 0;
     int  pathlen   = 0;
     int  modelen   = 0;

     char *         send_data_buf = NULL;
     RFILE_MNTN_HEADER header     = {0};
     RFILE_OPEN_HEAD  fopen_head  = {0};

     /*准备数据*/
     pathlen = strlen("/yaffs0/nv.bak");
     modelen = strlen("a+");

     rfileOpsSet[0] = 0;

     /*拷贝数据*/
     send_data_buf = (char*)malloc(sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD)+ pathlen+modelen);

     memset(send_data_buf,0,sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD)+ pathlen+modelen);

     memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD),"/yaffs0/nv.bak",pathlen);

     memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD)+pathlen,"a+",modelen);

     /*fopen opstype*/
     memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER),&opstype,sizeof(unsigned int));

     /*fopen pathlen*/
     memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER)+sizeof(unsigned int),&pathlen,sizeof(unsigned int));

     /*fopen modelen*/
     memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER)+sizeof(unsigned int)*2,&modelen,sizeof(unsigned int));

     rFileContext_a.sendBuf = send_data_buf;

     ret = rfile_process_ccpu_pkt(&header);
     if(-1 == ret)
     {
         return -1;
     }

     return 0;

}
int test_rfile_init_thread( void )
{
	rfile_init_thread();

	return 0;

}



int test_rfile_init_cb( void )
{
	int ret = 0;

	ret = rfile_init_cb();
	if(-1 == ret)
	{
		return -1;
	}

	return 0;
}



int test_rfile_process_task( void )
{
	int ret = 0;

	ret = rfile_process_task();
	if(-1 == ret)
	{
		return -1;
	}

	return 0;
}


int test_rfile_write_cmdtoccpu_pkt(void)
{
	int ret;
	int opspid = 0;
	char send_buf[10] = {0};
	int send_len = 1;
	ret = rfile_write_cmdtoccpu_pkt(opspid,&send_buf,send_len);
	if(ret == 0)
	{
		return 0;
	}
	else 
	{
		return -1;
	}
}

int test_rfile_ifc_cp2ap_ack(void)
{
	int ret;
	rfile_ifc_cp2ap_ack();
	return 0;
}

int test_rfile_icc_read_cmd_cb_iccOpened(void)
{
	int ret;
	unsigned int chenalId = 0;
	signed int u32size = 1;
	int sendbuf = 0;
	rFileContext_cmd.iccOpened = 1;
	rFileContext_cmd.sendBuf = (void*)&sendbuf;
	ret = rfile_icc_read_cmd_cb(chenalId,u32size);
	if(ret == 0)
	{
		return 0;
	}
	else 
	{
		return -1;
	}
}

int test_rfile_icc_read_cmd_cb(void)
{
	int ret;
	unsigned int chenalId = 0;
	signed int u32size = 1;
	int sendbuf = 0x12345678;
	rFileContext_cmd.iccOpened = 1;
	rFileContext_cmd.sendBuf = (void*)&sendbuf;
	ret = rfile_icc_read_cmd_cb(chenalId,u32size);
	if(ret == 0)
	{
		return 0;
	}
	else 
	{
		return -1;
	}
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

