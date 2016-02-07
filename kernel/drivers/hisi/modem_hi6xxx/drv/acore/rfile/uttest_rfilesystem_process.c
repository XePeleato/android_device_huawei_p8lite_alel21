

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "linux/semaphore.h"
#include "mtd/mtd-abi.h"
#include "linux/list.h"

#include "BSP.h"
#include "rfilesystem_process.h"
#include "rfilesystem_transfer.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
#define NAND_BLK_SIZE   128*1024
#define NAND_PER_PAGE   1024
#define ICC_BUF         64*1024

extern RFILECONTEXT_A rFileContext_a;

extern struct rfile_ops rfile_linux_ops;
extern unsigned int g_rfile_wt_expect_cur;
extern fp_list_t fp_list_file;
extern fp_list_t fp_list_dir;
extern int g_mass_wt_expect_cur;
extern int g_rfilesystem_init;
/*****************************************************************************
  3 函数实现
*****************************************************************************/



int test_debug_print_mnt_header( void )
{
	RFILE_MNTN_HEADER mntn_h = {0};

	mntn_h.curFrame = 1;
	mntn_h.curlen   = 125;
	mntn_h.opsPid   = 2;
	mntn_h.totalFrame = 1;
	mntn_h.reserve  = 0x5a5a5a5a;

	debug_print_mnt_header(&mntn_h);

    return 0;

}


int test_rfile_regProxyFunc( void )
{
	rfile_regProxyFunc();

	return 0;
}


int test_rfile_write_ccpu_pkt( void )
{
    int ret      = 0;
    int opspid   = 0;
    char buf[10] = {0};

    ret = rfile_write_ccpu_pkt(opspid,buf,sizeof(buf));
    if(-1 == ret)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}



int test_rfile_read_ccpu_data( void )
{
	int  ret       = 0;
	int  i         = 0;
	int  opstype   = 0;
	int  pathlen   = 0;
	int  modelen   = 0;

	char *         send_data_buf = 0;
	RFILE_MNTN_HEADER header     = {0};
	RFILE_OPEN_HEAD  fopen_head  = {0};

	/*准备数据*/
	pathlen = strlen("/yaffs0/nv.bak");
	modelen = strlen("a+");

	header.totalFrame = 1;
	header.curFrame   = 1;
	header.opsPid     = 0;
	header.curlen     = 10;

    fopen_head.opsType = 0;
    fopen_head.pathLen = pathlen;
    fopen_head.modeLen = modelen;

	/*拷贝数据*/
	send_data_buf = (char*)malloc(sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD)+ pathlen+modelen);

	memset(send_data_buf,0,sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD)+ pathlen+modelen);

	memcpy(send_data_buf,&header,sizeof(RFILE_MNTN_HEADER));

	memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER),&fopen_head,sizeof(RFILE_OPEN_HEAD));

	memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD),"/yaffs0/nv.bak",pathlen);

	memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD)+pathlen,"a+",modelen);

	/*fopen opstype*/
	memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER),&opstype,sizeof(unsigned int));

	/*fopen pathlen*/
	memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER)+sizeof(unsigned int),&pathlen,sizeof(unsigned int));

	/*fopen modelen*/
	memcpy(send_data_buf+sizeof(RFILE_MNTN_HEADER)+sizeof(unsigned int)*2,&modelen,sizeof(unsigned int));

	rFileContext_a.sendBuf = send_data_buf;

    char buf[sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD)];

    rfile_read_ccpu_data(buf,sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD));

    for(i = 0;i<sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_OPEN_HEAD);i++ )
    {
        printf("the data is %d\n",buf[i]);
    }

    if((buf[4] == 1)&&(buf[8]== 1)&&(buf[12]== 10)&&(buf[24]== 14)&&(buf[28]== 2))
    {
    	return 0;
    }
    else
    {
    	return -1;
    }


}
int test_rfile_calc_read_frame_num( int bytes )
{
    int ret = 0;

    ret = rfile_calc_read_frame_num(bytes);

    return ret;

}


int test_rfile_calc_massread_frame_num( int bytes )
{
    int ret = 0;

    ret = rfile_calc_massread_frame_num(bytes);

    return ret;
}


int test_rfile_sflags( char *mode,int *flag)
{
    int ret    = -1;
    int append = 0;

    ret = rfile_sflags(mode,flag,&append);

    return ret;
}



int test_rfile_xcopy_file( void  )
{
    int ret = 0;

    char sour_buf[] = "/yaffs0/nv.bak";
    char dest_buf[] = "/yaffs0/nv.baknew";

    ret = rfile_xcopy_file(sour_buf,dest_buf);
    if(-1 == ret)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}


int test_rfile_get_effectblk_addr( void )
{
    int fd = 0;
    int ret = 0;
    int usr_logic_blk = 10;

    ret = rfile_get_effectblk_addr(fd,usr_logic_blk,NAND_BLK_SIZE);
    if(9*NAND_BLK_SIZE != ret)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}


int test_rfile_mass_write( void )
{
     int fd = 0;
     int ret = 0;
     int blksize = 128*1024;
     int erase_addr = 10;

     char buf[]= "/abcdefghijklmnopqrstuvwxyz";

     ret = rfile_mass_write(fd,buf,blksize,erase_addr);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }

}


int test_rfile_get_massread_op_para_buf_null( void )
{
     int fd         = 0;
     int ret        = 0;
     int length     = 0;
     int offset     = 10;
     char *buf      = 0;
     char partion_name[] = "/yaffs0";
     RFILE_MASSREAD_HEAD pmassread_head = {0};

     ret = rfile_get_massread_op_para(buf,partion_name,&offset,&length,&pmassread_head);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }

}


int test_rfile_get_massread_op_para_partion_name_null( void )
{
     int fd         = 0;
     int ret        = 0;
     int length     = 0;
     int offset     = 10;
     char *buf      = malloc(10);
     char *partion_name = 0;
     RFILE_MASSREAD_HEAD pmassread_head = {0};

     ret = rfile_get_massread_op_para(buf,partion_name,&offset,&length,&pmassread_head);
     if(-1 == ret)
     {
    	 free(buf);
         return -1;
     }
     else
     {
    	 free(buf);
         return 0;
     }
}


int test_rfile_get_massread_op_para_offset_null( void )
{
     int fd         = 0;
     int ret        = 0;
     int length     = 0;
     char *offset   = 0;
     char *buf           = malloc(10);
     char partion_name[] = "/yaffs0";
     RFILE_MASSREAD_HEAD pmassread_head = {0};

     ret = rfile_get_massread_op_para(buf,partion_name,offset,&length,&pmassread_head);
     if(-1 == ret)
     {
    	 free(buf);
         return -1;
     }
     else
     {
    	 free(buf);
         return 0;
     }
}


int test_rfile_get_massread_op_para_length_null( void )
{
     int fd         = 0;
     int ret        = 0;
     int *length    = 0;
     int  offset    = 0;
     char *buf           = malloc(10);
     char partion_name[] = "/yaffs0";
     RFILE_MASSREAD_HEAD pmassread_head = {0};

     ret = rfile_get_massread_op_para(buf,partion_name,&offset,length,&pmassread_head);
     if(-1 == ret)
     {
    	 free(buf);
         return -1;
     }
     else
     {
    	 free(buf);
         return 0;
     }
}


int test_rfile_get_massread_op_para_massread_head_null( void )
{
     int fd         = 0;
     int ret        = 0;
     int length     = 0;
     int offset     = 0;
     char *buf           = malloc(10);
     char partion_name[] = "/yaffs0";
     RFILE_MASSREAD_HEAD *pmassread_head = 0;

     ret = rfile_get_massread_op_para(buf,partion_name,&offset,&length,pmassread_head);
     if(-1 == ret)
     {
    	 free(buf);
         return -1;
     }
     else
     {
    	 free(buf);
         return 0;
     }
}



int test_rfile_get_massread_op_para_success( void )
{
     int fd         = 0;
     int ret        = 0;
     int length     = 0;
     int offset     = 0;
     int offset_sim = 0x5a5a5a5a;
     int length_sim = 0xa5a5a5a5;
     char *buf      = 0;
     char path_name[]     = "/yaffs0";
     char partion_name[8] = {0};
     RFILE_MASSREAD_HEAD pmassread_head = {0};

     buf = (char*)malloc(sizeof(RFILE_MASSREAD_HEAD)+sizeof(path_name)+2*sizeof(int));
     if(NULL == buf)
     {
    	 printf("malloc fail\n");
         return -1;
     }

     memset(buf,0,sizeof(RFILE_MASSREAD_HEAD)+sizeof(path_name)+2*sizeof(int));

     memcpy(buf+sizeof(RFILE_MASSREAD_HEAD),"/yaffs0",7);

     memcpy(buf+sizeof(RFILE_MASSREAD_HEAD)+sizeof(path_name),&offset_sim,sizeof(int));

     memcpy(buf+sizeof(RFILE_MASSREAD_HEAD)+sizeof(path_name)+sizeof(int),&length_sim,sizeof(int));

     pmassread_head.opsType = 0;
     pmassread_head.partition_name_len = sizeof(path_name);
     pmassread_head.offset = 4;
     pmassread_head.length = 4;

     ret = rfile_get_massread_op_para(buf,partion_name,&offset,&length,&pmassread_head);
     if(-1 == ret)
     {
    	 free(buf);
         return -1;
     }
     else
     {
    	 printf("the paration_name is %s,offset = %x,lenght = %x \n",partion_name,offset,length);
    	 free(buf);
         return 0;
     }
}


int test_rfile_get_masswrite_op_para_buf_null( void )
{
     int fd         = 0;
     int ret        = 0;
     int length     = 0;
     int offset     = 10;
     char *buf      = 0;
     char partion_name[] = "/yaffs0";
     RFILE_MASSWRITE_HEAD pmasswrite_head = {0};

     ret = rfile_get_masswrite_op_para(buf,partion_name,&offset,&length,&pmasswrite_head);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }

}


int test_rfile_get_masswrite_op_para_partion_name_null( void )
{
     int fd         = 0;
     int ret        = 0;
     int length     = 0;
     int offset     = 10;
     char *buf      = malloc(10);
     char *partion_name = 0;
     RFILE_MASSWRITE_HEAD pmasswrite_head = {0};

     ret = rfile_get_masswrite_op_para(buf,partion_name,&offset,&length,&pmasswrite_head);
     if(-1 == ret)
     {
    	 free(buf);
         return -1;
     }
     else
     {
    	 free(buf);
         return 0;
     }
}


int test_rfile_get_masswrite_op_para_offset_null( void )
{
     int fd         = 0;
     int ret        = 0;
     int length     = 0;
     char *offset   = 0;
     char *buf           = malloc(10);
     char partion_name[] = "/yaffs0";
     RFILE_MASSWRITE_HEAD pmasswrite_head = {0};

     ret = rfile_get_masswrite_op_para(buf,partion_name,offset,&length,&pmasswrite_head);
     if(-1 == ret)
     {
    	 free(buf);
         return -1;
     }
     else
     {
    	 free(buf);
         return 0;
     }
}


int test_rfile_get_masswrite_op_para_length_null( void )
{
     int fd         = 0;
     int ret        = 0;
     int *length    = 0;
     int  offset    = 0;
     char *buf           = malloc(10);
     char partion_name[] = "/yaffs0";
     RFILE_MASSWRITE_HEAD pmasswrite_head = {0};

     ret = rfile_get_masswrite_op_para(buf,partion_name,&offset,length,&pmasswrite_head);
     if(-1 == ret)
     {
    	 free(buf);
         return -1;
     }
     else
     {
    	 free(buf);
         return 0;
     }
}


int test_rfile_get_masswrite_op_para_masswrite_head_null( void )
{
     int fd         = 0;
     int ret        = 0;
     int length     = 0;
     int offset     = 0;
     char *buf           = malloc(10);
     char partion_name[] = "/yaffs0";
     RFILE_MASSWRITE_HEAD *pmasswrite_head = 0;

     ret = rfile_get_masswrite_op_para(buf,partion_name,&offset,&length,pmasswrite_head);
     if(-1 == ret)
     {
    	 free(buf);
         return -1;
     }
     else
     {
    	 free(buf);
         return 0;
     }
}



int test_rfile_get_masswrite_op_para_success( void )
{
     int fd         = 0;
     int ret        = 0;
     int length     = 0;
     int offset     = 0;
     int offset_sim = 0x5a5a5a5a;
     int length_sim = 0xa5a5a5a5;
     char *buf      = 0;
     char path_name[]     = "/yaffs0";
     char partion_name[8] = {0};
     RFILE_MASSWRITE_HEAD pmasswrite_head = {0};

     buf = (char*)malloc(sizeof(RFILE_MASSWRITE_HEAD)+sizeof(path_name)+2*sizeof(int));
     if(NULL == buf)
     {
    	 printf("malloc fail\n");
         return -1;
     }

     memset(buf,0,sizeof(RFILE_MASSWRITE_HEAD)+sizeof(path_name)+2*sizeof(int));

     memcpy(buf+sizeof(RFILE_MASSWRITE_HEAD),"/yaffs0",7);

     memcpy(buf+sizeof(RFILE_MASSWRITE_HEAD)+sizeof(path_name),&offset_sim,sizeof(int));

     memcpy(buf+sizeof(RFILE_MASSWRITE_HEAD)+sizeof(path_name)+sizeof(int),&length_sim,sizeof(int));

     pmasswrite_head.opsType = 0;
     pmasswrite_head.partition_name_len = sizeof(path_name);
     pmasswrite_head.offset = 4;
     pmasswrite_head.size = 4;

     ret = rfile_get_masswrite_op_para(buf,partion_name,&offset,&length,&pmasswrite_head);
     if(-1 == ret)
     {
    	 free(buf);
         return -1;
     }
     else
     {
    	 printf("the paration_name is %s,offset = %x,lenght = %x \n",partion_name,offset,length);
    	 free(buf);
         return 0;
     }
}



int test_rfile_mass_read_all_buf_null( void )
{
     int ret = 0;
     int fd  = 0;
     char *buffer_temp  = (char*)malloc(10);
     char *buffer       = 0;
     int read_length    = 0;
     RFILE_MASS_RW_PARA  read_blk_para = {0};
     struct mtd_info_user     mtd_info = {0} ;

     ret = rfile_mass_read_all(fd,read_length,buffer_temp,buffer,read_blk_para,mtd_info);
     if(-1 == ret)
     {
    	 free(buffer_temp);
         return -1;
     }
     else
     {
    	 free(buffer_temp);
         return 0;
     }
}


int test_rfile_mass_read_all_buf_temp_null( void )
{
     int ret = 0;
     int fd  = 0;
     char *buffer_temp  = 0;
     char *buffer       = (char*)malloc(10);
     int read_length    = 0;
     RFILE_MASS_RW_PARA  read_blk_para = {0};
     struct mtd_info_user     mtd_info = {0} ;

     ret = rfile_mass_read_all(fd,read_length,buffer_temp,buffer,read_blk_para,mtd_info);
     if(-1 == ret)
     {
    	 free(buffer);
         return -1;
     }
     else
     {
    	 free(buffer);
         return 0;
     }
}



int test_rfile_mass_read_all_mem_over( void )
{
     int ret = 0;
     int fd  = 0;
     char *buffer_temp  = NULL;
     char *buffer       = NULL;
     int read_length    = 0;
     RFILE_MASS_RW_PARA  read_blk_para = {0};
     struct mtd_info_user     mtd_info = {0} ;

     buffer = (char*)malloc(NAND_BLK_SIZE);
     if(NULL == buffer)
     {
    	 printf("buffer malloc fail \n");
    	 return -1;
     }

     buffer_temp = (char*)malloc(NAND_BLK_SIZE);
     if(NULL ==buffer_temp)
     {
    	 printf("buffer temp malloc fail\n");
    	 return -1;
     }

     read_length = 10;

     read_blk_para.blk_begin        = 1;
     read_blk_para.blk_end          = 1;;
     read_blk_para.first_blk_offset = 0;
     read_blk_para.last_blk_offset  = 0;
     read_blk_para.physical_offset  = 64*NAND_BLK_SIZE;

     mtd_info.erasesize = NAND_BLK_SIZE;
     mtd_info.size      = 64*NAND_BLK_SIZE;

     ret = rfile_mass_read_all(fd,read_length,buffer_temp,buffer,read_blk_para,mtd_info);
     if(-1 == ret)
     {
    	 free(buffer);
    	 free(buffer_temp);
         return -1;
     }
     else
     {
    	 free(buffer);
    	 free(buffer_temp);
         return 0;
     }
}



int test_rfile_mass_read_all_sys_ioctl_fail( void )
{
     int ret = 0;
     int fd  = 0;
     char *buffer_temp  = NULL;
     char *buffer       = NULL;
     int read_length    = 0;
     RFILE_MASS_RW_PARA  read_blk_para = {0};
     struct mtd_info_user     mtd_info = {0} ;

     buffer = (char*)malloc(NAND_BLK_SIZE);
     if(NULL == buffer)
     {
    	 printf("buffer malloc fail \n");
    	 return -1;
     }

     buffer_temp = (char*)malloc(NAND_BLK_SIZE);
     if(NULL ==buffer_temp)
     {
    	 printf("buffer temp malloc fail\n");
    	 return -1;
     }

     read_length = 10;

     read_blk_para.blk_begin        = 1;
     read_blk_para.blk_end          = 1;;
     read_blk_para.first_blk_offset = 0;
     read_blk_para.last_blk_offset  = 0;
     read_blk_para.physical_offset  = 64*NAND_BLK_SIZE - NAND_BLK_SIZE;

     mtd_info.erasesize = NAND_BLK_SIZE;
     mtd_info.size      = 64*NAND_BLK_SIZE;

     ret = rfile_mass_read_all(fd,read_length,buffer_temp,buffer,read_blk_para,mtd_info);
     if(-1 == ret)
     {
    	 free(buffer);
    	 free(buffer_temp);
         return -1;
     }
     else
     {
    	 free(buffer);
    	 free(buffer_temp);
         return 0;
     }
}



int test_rfile_mass_read_all_in_one_blk( void )
{
     int ret = 0;
     int fd  = 0;
     char *buffer_temp  = NULL;
     char *buffer       = NULL;
     int read_length    = 0;
     RFILE_MASS_RW_PARA  read_blk_para = {0};
     struct mtd_info_user     mtd_info = {0} ;

     buffer = (char*)malloc(NAND_BLK_SIZE);
     if(NULL == buffer)
     {
    	 printf("buffer malloc fail \n");
    	 return -1;
     }

     buffer_temp = (char*)malloc(NAND_BLK_SIZE);
     if(NULL ==buffer_temp)
     {
    	 printf("buffer temp malloc fail\n");
    	 return -1;
     }

     read_length = NAND_PER_PAGE;

     read_blk_para.blk_begin        = 1;
     read_blk_para.blk_end          = 1;
     read_blk_para.first_blk_offset = NAND_PER_PAGE*1;
     read_blk_para.last_blk_offset  = NAND_PER_PAGE*2;
     read_blk_para.physical_offset  = NAND_BLK_SIZE;

     mtd_info.erasesize = NAND_BLK_SIZE;
     mtd_info.size      = 64*NAND_BLK_SIZE;

     ret = rfile_mass_read_all(fd,read_length,buffer_temp,buffer,read_blk_para,mtd_info);
     if(-1 == ret)
     {
    	 free(buffer);
    	 free(buffer_temp);
         return -1;
     }
     else
     {
    	 free(buffer);
    	 free(buffer_temp);
         return 0;
     }
}


int test_rfile_mass_read_all_in_one_blk_total( void )
{
     int ret = 0;
     int fd  = 0;
     char *buffer_temp  = NULL;
     char *buffer       = NULL;
     int read_length    = 0;
     RFILE_MASS_RW_PARA  read_blk_para = {0};
     struct mtd_info_user     mtd_info = {0} ;

     buffer = (char*)malloc(NAND_BLK_SIZE);
     if(NULL == buffer)
     {
    	 printf("buffer malloc fail \n");
    	 return -1;
     }

     buffer_temp = (char*)malloc(NAND_BLK_SIZE);
     if(NULL ==buffer_temp)
     {
    	 printf("buffer temp malloc fail\n");
    	 return -1;
     }

     read_length = NAND_BLK_SIZE;

     read_blk_para.blk_begin        = 1;
     read_blk_para.blk_end          = 1;
     read_blk_para.first_blk_offset = 0;
     read_blk_para.last_blk_offset  = 0;
     read_blk_para.physical_offset  = NAND_BLK_SIZE;

     mtd_info.erasesize = NAND_BLK_SIZE;
     mtd_info.size      = 64*NAND_BLK_SIZE;

     ret = rfile_mass_read_all(fd,read_length,buffer_temp,buffer,read_blk_para,mtd_info);
     if(-1 == ret)
     {
    	 free(buffer);
    	 free(buffer_temp);
         return -1;
     }
     else
     {
    	 free(buffer);
    	 free(buffer_temp);
         return 0;
     }
}


int test_rfile_mass_read_all_in_two_blk( void )
{
     int ret = 0;
     int fd  = 0;
     char *buffer_temp  = NULL;
     char *buffer       = NULL;
     int read_length    = 0;
     RFILE_MASS_RW_PARA  read_blk_para = {0};
     struct mtd_info_user     mtd_info = {0} ;

     buffer = (char*)malloc(NAND_BLK_SIZE);
     if(NULL == buffer)
     {
    	 printf("buffer malloc fail \n");
    	 return -1;
     }

     buffer_temp = (char*)malloc(NAND_BLK_SIZE);
     if(NULL ==buffer_temp)
     {
    	 printf("buffer temp malloc fail\n");
    	 return -1;
     }

     read_length = NAND_PER_PAGE*20;

     read_blk_para.blk_begin        = 1;
     read_blk_para.blk_end          = 2;
     read_blk_para.first_blk_offset = NAND_PER_PAGE*120;
     read_blk_para.last_blk_offset  = NAND_PER_PAGE*12;
     read_blk_para.physical_offset  = NAND_BLK_SIZE;

     mtd_info.erasesize = NAND_BLK_SIZE;
     mtd_info.size      = 64*NAND_BLK_SIZE;

     ret = rfile_mass_read_all(fd,read_length,buffer_temp,buffer,read_blk_para,mtd_info);
     if(-1 == ret)
     {
    	 free(buffer);
    	 free(buffer_temp);
         return -1;
     }
     else
     {
    	 free(buffer);
    	 free(buffer_temp);
         return 0;
     }
}



int test_rfile_mass_read_all_in_three_blk( void )
{
     int ret = 0;
     int fd  = 0;
     char *buffer_temp  = NULL;
     char *buffer       = NULL;
     int read_length    = 0;
     RFILE_MASS_RW_PARA  read_blk_para = {0};
     struct mtd_info_user     mtd_info = {0} ;

     buffer = (char*)malloc(NAND_BLK_SIZE*5);
     if(NULL == buffer)
     {
    	 printf("buffer malloc fail \n");
    	 return -1;
     }

     buffer_temp = (char*)malloc(NAND_BLK_SIZE*5);
     if(NULL ==buffer_temp)
     {
    	 printf("buffer temp malloc fail\n");
    	 return -1;
     }

     read_length = NAND_PER_PAGE*148;

     read_blk_para.blk_begin        = 1;
     read_blk_para.blk_end          = 3;
     read_blk_para.first_blk_offset = NAND_PER_PAGE*120;
     read_blk_para.last_blk_offset  = NAND_PER_PAGE*12;
     read_blk_para.physical_offset  = NAND_BLK_SIZE;

     mtd_info.erasesize = NAND_BLK_SIZE;
     mtd_info.size      = 64*NAND_BLK_SIZE;

     ret = rfile_mass_read_all(fd,read_length,buffer_temp,buffer,read_blk_para,mtd_info);
     if(-1 == ret)
     {
    	 free(buffer);
    	 free(buffer_temp);
         return -1;
     }
     else
     {
    	 free(buffer);
    	 free(buffer_temp);
         return 0;
     }
}



int test_rfile_mass_write_all_buf_null( void )
{
     int ret = 0;
     int fd  = 0;
     char *buffer_temp  = (char*)malloc(10);
     char *buffer       = 0;
     int write_length   = 0;
     RFILE_MASS_RW_PARA  write_blk_para = {0};
     struct mtd_info_user      mtd_info = {0} ;

     ret = rfile_mass_write_all(fd,write_length,buffer_temp,buffer,write_blk_para,mtd_info);
     if(-1 == ret)
     {
    	 free(buffer_temp);
         return -1;
     }
     else
     {
    	 free(buffer_temp);
         return 0;
     }
}


int test_rfile_mass_write_all_buf_temp_null( void )
{
     int ret = 0;
     int fd  = 0;
     char *buffer_temp  = 0;
     char *buffer       = (char*)malloc(10);
     int write_length   = 0;
     RFILE_MASS_RW_PARA  write_blk_para = {0};
     struct mtd_info_user      mtd_info = {0} ;

     ret = rfile_mass_write_all(fd,write_length,buffer_temp,buffer,write_blk_para,mtd_info);
     if(-1 == ret)
     {
    	 free(buffer);
         return -1;
     }
     else
     {
    	 free(buffer);
         return 0;
     }
}


int test_rfile_mass_write_all_mem_over( void )
{
     int ret = 0;
     int fd  = 0;
     char *buffer_temp  = NULL;
     char *buffer       = NULL;
     int write_length   = 0;
     RFILE_MASS_RW_PARA  write_blk_para = {0};
     struct mtd_info_user      mtd_info = {0} ;

     buffer = (char*)malloc(NAND_BLK_SIZE);
     if(NULL == buffer)
     {
    	 printf("buffer malloc fail \n");
    	 return -1;
     }

     buffer_temp = (char*)malloc(NAND_BLK_SIZE);
     if(NULL ==buffer_temp)
     {
    	 printf("buffer temp malloc fail\n");
    	 return -1;
     }

     write_length = 10;

     write_blk_para.blk_begin        = 1;
     write_blk_para.blk_end          = 1;;
     write_blk_para.first_blk_offset = 0;
     write_blk_para.last_blk_offset  = 0;
     write_blk_para.physical_offset  = 64*NAND_BLK_SIZE;

     mtd_info.erasesize = NAND_BLK_SIZE;
     mtd_info.size      = 64*NAND_BLK_SIZE;

     ret = rfile_mass_write_all(fd,write_length,buffer_temp,buffer,write_blk_para,mtd_info);
     if(-1 == ret)
     {
    	 free(buffer);
    	 free(buffer_temp);
         return -1;
     }
     else
     {
    	 free(buffer);
    	 free(buffer_temp);
         return 0;
     }
}



int test_rfile_mass_write_all_sys_ioctl_fail( void )
{
     int ret = 0;
     int fd  = 0;
     char *buffer_temp  = NULL;
     char *buffer       = NULL;
     int write_length   = 0;
     RFILE_MASS_RW_PARA  write_blk_para = {0};
     struct mtd_info_user      mtd_info = {0} ;

     buffer = (char*)malloc(NAND_BLK_SIZE);
     if(NULL == buffer)
     {
    	 printf("buffer malloc fail \n");
    	 return -1;
     }

     buffer_temp = (char*)malloc(NAND_BLK_SIZE);
     if(NULL ==buffer_temp)
     {
    	 printf("buffer temp malloc fail\n");
    	 return -1;
     }

     write_length = 10;

     write_blk_para.blk_begin        = 1;
     write_blk_para.blk_end          = 1;;
     write_blk_para.first_blk_offset = 0;
     write_blk_para.last_blk_offset  = 0;
     write_blk_para.physical_offset  = 64*NAND_BLK_SIZE-NAND_BLK_SIZE;

     mtd_info.erasesize = NAND_BLK_SIZE;
     mtd_info.size      = 64*NAND_BLK_SIZE;

     ret = rfile_mass_write_all(fd,write_length,buffer_temp,buffer,write_blk_para,mtd_info);
     if(-1 == ret)
     {
    	 free(buffer);
    	 free(buffer_temp);
         return -1;
     }
     else
     {
    	 free(buffer);
    	 free(buffer_temp);
         return 0;
     }
}



int test_rfile_mass_write_all_in_one_blk( void )
{
     int ret = 0;
     int fd  = 0;
     char *buffer_temp  = NULL;
     char *buffer       = NULL;
     int write_length   = 0;
     RFILE_MASS_RW_PARA  write_blk_para = {0};
     struct mtd_info_user      mtd_info = {0} ;

     buffer = (char*)malloc(NAND_BLK_SIZE);
     if(NULL == buffer)
     {
    	 printf("buffer malloc fail \n");
    	 return -1;
     }

     buffer_temp = (char*)malloc(NAND_BLK_SIZE);
     if(NULL ==buffer_temp)
     {
    	 printf("buffer temp malloc fail\n");
    	 return -1;
     }

     write_length = NAND_PER_PAGE;

     write_blk_para.blk_begin        = 1;
     write_blk_para.blk_end          = 1;
     write_blk_para.first_blk_offset = NAND_PER_PAGE*1;
     write_blk_para.last_blk_offset  = NAND_PER_PAGE*2;
     write_blk_para.physical_offset  = NAND_BLK_SIZE;

     mtd_info.erasesize = NAND_BLK_SIZE;
     mtd_info.size      = 64*NAND_BLK_SIZE;

     ret = rfile_mass_write_all(fd,write_length,buffer_temp,buffer,write_blk_para,mtd_info);
     if(-1 == ret)
     {
    	 free(buffer);
    	 free(buffer_temp);
         return -1;
     }
     else
     {
    	 free(buffer);
    	 free(buffer_temp);
         return 0;
     }
}


int test_rfile_mass_write_all_in_one_blk_total( void )
{
     int ret = 0;
     int fd  = 0;
     char *buffer_temp  = NULL;
     char *buffer       = NULL;
     int write_length   = 0;
     RFILE_MASS_RW_PARA  write_blk_para = {0};
     struct mtd_info_user      mtd_info = {0} ;

     buffer = (char*)malloc(NAND_BLK_SIZE);
     if(NULL == buffer)
     {
    	 printf("buffer malloc fail \n");
    	 return -1;
     }

     buffer_temp = (char*)malloc(NAND_BLK_SIZE);
     if(NULL ==buffer_temp)
     {
    	 printf("buffer temp malloc fail\n");
    	 return -1;
     }

     write_length = NAND_BLK_SIZE;

     write_blk_para.blk_begin        = 1;
     write_blk_para.blk_end          = 1;
     write_blk_para.first_blk_offset = 0;
     write_blk_para.last_blk_offset  = 0;
     write_blk_para.physical_offset  = NAND_BLK_SIZE;

     mtd_info.erasesize = NAND_BLK_SIZE;
     mtd_info.size      = 64*NAND_BLK_SIZE;

     ret = rfile_mass_write_all(fd,write_length,buffer_temp,buffer,write_blk_para,mtd_info);
     if(-1 == ret)
     {
    	 free(buffer);
    	 free(buffer_temp);
         return -1;
     }
     else
     {
    	 free(buffer);
    	 free(buffer_temp);
         return 0;
     }
}


int test_rfile_mass_write_all_in_two_blk( void )
{
     int ret = 0;
     int fd  = 0;
     char *buffer_temp  = NULL;
     char *buffer       = NULL;
     int write_length   = 0;
     RFILE_MASS_RW_PARA  write_blk_para = {0};
     struct mtd_info_user      mtd_info = {0} ;

     buffer = (char*)malloc(NAND_BLK_SIZE);
     if(NULL == buffer)
     {
    	 printf("buffer malloc fail \n");
    	 return -1;
     }

     buffer_temp = (char*)malloc(NAND_BLK_SIZE);
     if(NULL ==buffer_temp)
     {
    	 printf("buffer temp malloc fail\n");
    	 return -1;
     }

     write_length = NAND_PER_PAGE*20;

     write_blk_para.blk_begin        = 1;
     write_blk_para.blk_end          = 2;
     write_blk_para.first_blk_offset = NAND_PER_PAGE*120;
     write_blk_para.last_blk_offset  = NAND_PER_PAGE*12;
     write_blk_para.physical_offset  = NAND_BLK_SIZE;

     mtd_info.erasesize = NAND_BLK_SIZE;
     mtd_info.size      = 64*NAND_BLK_SIZE;

     ret = rfile_mass_write_all(fd,write_length,buffer_temp,buffer,write_blk_para,mtd_info);
     if(-1 == ret)
     {
    	 free(buffer);
    	 free(buffer_temp);
         return -1;
     }
     else
     {
    	 free(buffer);
    	 free(buffer_temp);
         return 0;
     }
}


int test_rfile_mass_write_all_in_three_blk( void )
{
     int ret = 0;
     int fd  = 0;
     char *buffer_temp  = NULL;
     char *buffer       = NULL;
     int write_length   = 0;
     RFILE_MASS_RW_PARA  write_blk_para = {0};
     struct mtd_info_user      mtd_info = {0} ;

     buffer = (char*)malloc(NAND_BLK_SIZE*5);
     if(NULL == buffer)
     {
    	 printf("buffer malloc fail \n");
    	 return -1;
     }

     buffer_temp = (char*)malloc(NAND_BLK_SIZE*5);
     if(NULL ==buffer_temp)
     {
    	 printf("buffer temp malloc fail\n");
    	 return -1;
     }

     write_length = NAND_PER_PAGE*148;

     write_blk_para.blk_begin        = 1;
     write_blk_para.blk_end          = 3;
     write_blk_para.first_blk_offset = NAND_PER_PAGE*120;
     write_blk_para.last_blk_offset  = NAND_PER_PAGE*12;
     write_blk_para.physical_offset  = NAND_BLK_SIZE;

     mtd_info.erasesize = NAND_BLK_SIZE;
     mtd_info.size      = 64*NAND_BLK_SIZE;

     ret = rfile_mass_write_all(fd,write_length,buffer,buffer_temp,write_blk_para,mtd_info);
     if(-1 == ret)
     {
    	 free(buffer);
    	 free(buffer_temp);
         return -1;
     }
     else
     {
    	 free(buffer);
    	 free(buffer_temp);
         return 0;
     }
}



int test_fopenProxy_wrong_pkg( void )
{
     int     ret = 0;
     int  opstype   = FOPEN;
     int  pathlen   = 0;
     int  modelen   = 0;
     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FOPEN;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*准备数据*/
     pathlen = strlen("/yaffs0/nv.bak");
     modelen = strlen("a+");

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_OPEN_HEAD)+ pathlen + modelen);

     memset(data,0,sizeof(RFILE_OPEN_HEAD)+ pathlen+modelen);

     memcpy(data+sizeof(RFILE_OPEN_HEAD),"/yaffs0/nv.bak",pathlen);

     memcpy(data+sizeof(RFILE_OPEN_HEAD)+pathlen,"a+",modelen);

     /*fopen opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fopen pathlen*/
     memcpy(data+sizeof(unsigned int),&pathlen,sizeof(unsigned int));

     /*fopen modelen*/
     memcpy(data+sizeof(unsigned int)*2,&modelen,sizeof(unsigned int));

     ret = fopenProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fopenProxy( void )
{
     int     ret = 0;
     int  opstype   = FOPEN;
     int  pathlen   = 0;
     int  modelen   = 0;
     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FOPEN;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*准备数据*/
     pathlen = strlen("/yaffs0/nv.bak");
     modelen = strlen("a+");

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_OPEN_HEAD)+ pathlen + modelen);

     memset(data,0,sizeof(RFILE_OPEN_HEAD)+ pathlen+modelen);

     memcpy(data+sizeof(RFILE_OPEN_HEAD),"/yaffs0/nv.bak",pathlen);

     memcpy(data+sizeof(RFILE_OPEN_HEAD)+pathlen,"a+",modelen);

     /*fopen opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fopen pathlen*/
     memcpy(data+sizeof(unsigned int),&pathlen,sizeof(unsigned int));

     /*fopen modelen*/
     memcpy(data+sizeof(unsigned int)*2,&modelen,sizeof(unsigned int));
	
     ret = fopenProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fopenProxy_pathlen_wrong( void )
{
     int     ret = 0;
     int  opstype   = FOPEN;
     int  pathlen   = 0;
     int  modelen   = 0;
     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FOPEN;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*准备数据*/
     modelen = strlen("a+");

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_OPEN_HEAD)+ pathlen + modelen);

     memset(data,0,sizeof(RFILE_OPEN_HEAD)+ pathlen+modelen);

     memcpy(data+sizeof(RFILE_OPEN_HEAD),"/yaffs0/nv.bak",pathlen);

     memcpy(data+sizeof(RFILE_OPEN_HEAD)+pathlen,"a+",modelen);

     /*fopen opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fopen pathlen*/
     memcpy(data+sizeof(unsigned int),&pathlen,sizeof(unsigned int));

     /*fopen modelen*/
     memcpy(data+sizeof(unsigned int)*2,&modelen,sizeof(unsigned int));

     ret = fopenProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fopenProxy_mode_wrong( void )
{
     int     ret = 0;
     int  opstype   = FOPEN;
     int  pathlen   = 0;
     int  modelen   = 0;
     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FOPEN;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*准备数据*/
     pathlen = strlen("/yaffs0/nv.bak");

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_OPEN_HEAD)+ pathlen + modelen);

     memset(data,0,sizeof(RFILE_OPEN_HEAD)+ pathlen+modelen);

     memcpy(data+sizeof(RFILE_OPEN_HEAD),"/yaffs0/nv.bak",pathlen);

     memcpy(data+sizeof(RFILE_OPEN_HEAD)+pathlen,"a+",modelen);

     /*fopen opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fopen pathlen*/
     memcpy(data+sizeof(unsigned int),&pathlen,sizeof(unsigned int));

     /*fopen modelen*/
     memcpy(data+sizeof(unsigned int)*2,&modelen,sizeof(unsigned int));

     ret = fopenProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fopenProxy_fopen_unreg( void )
{
     int     ret = 0;
     int  opstype   = FOPEN;
     int  pathlen   = 0;
     int  modelen   = 0;
     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FOPEN;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*准备数据*/
     pathlen = strlen("/yaffs0/nv.bak");
     modelen = strlen("a+");

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_OPEN_HEAD)+ pathlen + modelen);

     memset(data,0,sizeof(RFILE_OPEN_HEAD)+ pathlen+modelen);

     memcpy(data+sizeof(RFILE_OPEN_HEAD),"/yaffs0/nv.bak",pathlen);

     memcpy(data+sizeof(RFILE_OPEN_HEAD)+pathlen,"a+",modelen);

     /*fopen opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fopen pathlen*/
     memcpy(data+sizeof(unsigned int),&pathlen,sizeof(unsigned int));

     /*fopen modelen*/
     memcpy(data+sizeof(unsigned int)*2,&modelen,sizeof(unsigned int));

     rfile_linux_ops.bsp_fopen = NULL;

     ret = fopenProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fopenProxy_fopen_reg_success( void )
{
     int     ret = 0;
     int  opstype   = FOPEN;
     int  pathlen   = 0;
     int  modelen   = 0;
     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FOPEN;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*准备数据*/
     pathlen = strlen("/yaffs0/nv.bak");
     modelen = strlen("a+");

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_OPEN_HEAD)+ pathlen + modelen);

     memset(data,0,sizeof(RFILE_OPEN_HEAD)+ pathlen+modelen);

     memcpy(data+sizeof(RFILE_OPEN_HEAD),"/yaffs0/nv.bak",pathlen);

     memcpy(data+sizeof(RFILE_OPEN_HEAD)+pathlen,"a+",modelen);

     /*fopen opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fopen pathlen*/
     memcpy(data+sizeof(unsigned int),&pathlen,sizeof(unsigned int));

     /*fopen modelen*/
     memcpy(data+sizeof(unsigned int)*2,&modelen,sizeof(unsigned int));

     rfile_linux_ops.bsp_fopen = bsp_linux_fopen;

     ret = fopenProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fcloseProxy_wrong_pkg( void )
{
     int      ret   = 0;
     int  opstype   = FOPEN;
     RFILE     fp    = {0};

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FCLOSE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_CLOSE_HEAD));

     memset(data,0,sizeof(RFILE_CLOSE_HEAD));

     /*fclose opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fclose fp*/
     memcpy(data+sizeof(unsigned int),&fp,sizeof(unsigned int));

     ret = fcloseProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fcloseProxy_para_err( void )
{
     int      ret   = 0;
     int  opstype   = FOPEN;
     RFILE     fp    = {0};
	 
     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FCLOSE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_CLOSE_HEAD));

     memset(data,0,sizeof(RFILE_CLOSE_HEAD));

     /*fclose opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fclose fp*/
     ((RFILE_CLOSE_HEAD*)data)->fp = 0;
	 
     ret = fcloseProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fcloseProxy_fclose_unreg( void )
{
     int      ret   = 0;
     int  opstype   = FOPEN;
     RFILE     fp    = {0};
	 char buf[100] = {0};
	 int buf_ret;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FCLOSE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_CLOSE_HEAD));

     memset(data,0,sizeof(RFILE_CLOSE_HEAD));

     /*fclose opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fclose fp*/
	 INIT_LIST_HEAD(&(fp_list_file.list));
	 buf_ret = fp_list_add_file(buf);
     ((RFILE_CLOSE_HEAD*)data)->fp = buf_ret;

     rfile_linux_ops.bsp_fclose = NULL;

     ret = fcloseProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fcloseProxy_fclose_checkfp_fail( void )
{
     int      ret   = 0;
     int  opstype   = FOPEN;
     RFILE     fp    = {0};

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FCLOSE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_CLOSE_HEAD));

     memset(data,0,sizeof(RFILE_CLOSE_HEAD));

     /*fclose opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));
	 INIT_LIST_HEAD(&(fp_list_file.list));
	 
     /*fclose fp*/
     ((RFILE_CLOSE_HEAD*)data)->fp = 1;

     rfile_linux_ops.bsp_fclose = bsp_linux_fclose;

     ret = fcloseProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fcloseProxy_fclose_fpcheck_fail( void )
{
     int      ret   = 0;
     int  opstype   = FOPEN;
     RFILE     fp    = {0};
	 char buf[100] = {0};
	 int buf_ret;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FCLOSE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_CLOSE_HEAD));

     memset(data,0,sizeof(RFILE_CLOSE_HEAD));

     /*fclose opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fclose fp*/
	 INIT_LIST_HEAD(&(fp_list_file.list));
     ((RFILE_CLOSE_HEAD*)data)->fp = 1;

     rfile_linux_ops.bsp_fclose = bsp_linux_fclose;

     ret = fcloseProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fcloseProxy_fclose_success( void )
{
     int      ret   = 0;
     int  opstype   = FOPEN;
     RFILE     fp    = {0};
	 char buf[100] = {0};
	 int buf_ret;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FCLOSE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_CLOSE_HEAD));

     memset(data,0,sizeof(RFILE_CLOSE_HEAD));

     /*fclose opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fclose fp*/
	 INIT_LIST_HEAD(&(fp_list_file.list));
	 buf_ret = fp_list_add_file(buf);
     ((RFILE_CLOSE_HEAD*)data)->fp = buf_ret;

     rfile_linux_ops.bsp_fclose = bsp_linux_fclose;

     ret = fcloseProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_freadProxy_wrong_pkg( void )
{
     int      ret   = 0;
     int  opstype   = FREAD;
     RFILE     fp    = {0};

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FREAD;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_READ_HEAD));

     memset(data,0,sizeof(RFILE_READ_HEAD));

     /*fclose opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fclose fp*/
     ((RFILE_READ_HEAD*)data)->fp = &fp;

     ((RFILE_READ_HEAD*)data)->size = 0;

     rfile_linux_ops.bsp_fclose = NULL;

     ret = freadProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_freadProxy_one_fread_unreg( void )
{
     int      ret   = 0;
     int  opstype   = FREAD;
     RFILE     fp    = {0};

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FREAD;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_READ_HEAD));

     memset(data,0,sizeof(RFILE_READ_HEAD));

     /*fclose opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fclose fp*/
     ((RFILE_READ_HEAD*)data)->fp = &fp;

     ((RFILE_READ_HEAD*)data)->size = ICC_BUF;

     rfile_linux_ops.bsp_fread = NULL;

     ret = freadProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_freadProxy_fpcheck_fail( void )
{
     int      ret   = 0;
     int  opstype   = FREAD;
     RFILE     fp    = {0};
	 char buf[100] = {0};
	 int buf_ret;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FREAD;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_READ_HEAD));

     memset(data,0,sizeof(RFILE_READ_HEAD));

     /*fclose opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fclose fp*/
	 INIT_LIST_HEAD(&(fp_list_file.list));
     ((RFILE_READ_HEAD*)data)->fp = 1;

     ((RFILE_READ_HEAD*)data)->size = ICC_BUF-sizeof(RFILE_READ_HEAD)-sizeof(RFILE_MNTN_HEADER);

     rfile_linux_ops.bsp_fread = bsp_linux_fread;
	
     ret = freadProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_freadProxy_lessthan_ICC_BUF( void )
{
     int      ret   = 0;
     int  opstype   = FREAD;
     RFILE     fp    = {0};
	 char buf[100] = {0};
	 int buf_ret;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FREAD;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_READ_HEAD));

     memset(data,0,sizeof(RFILE_READ_HEAD));

     /*fclose opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fclose fp*/
	 INIT_LIST_HEAD(&(fp_list_file.list));
	 buf_ret = fp_list_add_file(buf);
     ((RFILE_READ_HEAD*)data)->fp = buf_ret;

     ((RFILE_READ_HEAD*)data)->size = ICC_BUF-sizeof(RFILE_READ_HEAD)-sizeof(RFILE_MNTN_HEADER);

     rfile_linux_ops.bsp_fread = bsp_linux_fread;
	
     ret = freadProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_freadProxy_morethan_ICC_BUF( void )
{
     int      ret   = 0;
     int  opstype   = FREAD;
     RFILE     fp    = {0};
	 char buf[100] = {0};
	 int buf_ret;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FREAD;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_READ_HEAD));

     memset(data,0,sizeof(RFILE_READ_HEAD));

     /*fclose opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fclose fp*/
	 INIT_LIST_HEAD(&(fp_list_file.list));
	 buf_ret = fp_list_add_file(buf);
     ((RFILE_READ_HEAD*)data)->fp = buf_ret;

     ((RFILE_READ_HEAD*)data)->size = ICC_BUF;

     rfile_linux_ops.bsp_fread = bsp_linux_fread;

     ret = freadProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_freadProxy_fp_null( void )
{
     int      ret   = 0;
     int  opstype   = FREAD;
     RFILE     fp    = {0};

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FREAD;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_READ_HEAD));

     memset(data,0,sizeof(RFILE_READ_HEAD));

     memcpy(data,&opstype,sizeof(unsigned int));

     ((RFILE_READ_HEAD*)data)->fp = 0;

     ((RFILE_READ_HEAD*)data)->size = ICC_BUF;

     rfile_linux_ops.bsp_fread = bsp_linux_fread;

     ret = freadProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fwriteProxy_wrong_pkg( void )
{
     int      ret   = 0;
     int  opstype   = FWRITE;
     RFILE     fp    = {0};

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FWRITE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(ICC_BUF-sizeof(RFILE_WRITE_HEAD)-sizeof(RFILE_MNTN_HEADER));

     memset(data,0,sizeof(RFILE_WRITE_HEAD));

     /*fclose opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fclose fp*/
     ((RFILE_WRITE_HEAD*)data)->fp = &fp;

     ((RFILE_WRITE_HEAD*)data)->size = ICC_BUF;

     rfile_linux_ops.bsp_fwrite = bsp_linux_fwrite;

     ret = fwriteProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fwriteProxy_fp_null( void )
{
     int      ret   = 0;
     int  opstype   = FWRITE;
     RFILE     fp    = {0};

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FWRITE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(ICC_BUF-sizeof(RFILE_WRITE_HEAD)-sizeof(RFILE_MNTN_HEADER));

     memset(data,0,sizeof(RFILE_WRITE_HEAD));

     /*fclose opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fclose fp*/
     ((RFILE_WRITE_HEAD*)data)->fp = NULL;

     ((RFILE_WRITE_HEAD*)data)->size = ICC_BUF;

     rfile_linux_ops.bsp_fwrite = bsp_linux_fwrite;

     g_rfile_wt_expect_cur = 0;

     ret = fwriteProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fwriteProxy_fwrite_unreg( void )
{
     int      ret   = 0;
     int  opstype   = FWRITE;
     RFILE     fp    = {0};

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FWRITE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(ICC_BUF-sizeof(RFILE_WRITE_HEAD)-sizeof(RFILE_MNTN_HEADER));

     memset(data,0,sizeof(RFILE_WRITE_HEAD));

     /*fclose opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fclose fp*/
     ((RFILE_WRITE_HEAD*)data)->fp = &fp;

     ((RFILE_WRITE_HEAD*)data)->size = ICC_BUF;

     rfile_linux_ops.bsp_fwrite = NULL;

     g_rfile_wt_expect_cur = 0;

     ret = fwriteProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fwriteProxy_fwrite_checkfp_null( void )
{
     int      ret   = 0;
     int  opstype   = FWRITE;
     RFILE     fp    = {0};

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FWRITE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(ICC_BUF-sizeof(RFILE_WRITE_HEAD)-sizeof(RFILE_MNTN_HEADER));

     memset(data,0,sizeof(RFILE_WRITE_HEAD));

     /*fclose opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fclose fp*/
	 INIT_LIST_HEAD(&(fp_list_file.list));
     ((RFILE_WRITE_HEAD*)data)->fp = 1;

     ((RFILE_WRITE_HEAD*)data)->size = 1;

     rfile_linux_ops.bsp_fwrite = bsp_linux_fwrite;

     g_rfile_wt_expect_cur = 0;

     ret = fwriteProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fwriteProxy_fwrite_frame_not_last( void )
{
     int      ret   = 0;
     int  opstype   = FWRITE;
     RFILE     fp    = {0};
	 char buf[100] = {0};
	 int buf_ret;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FWRITE;
     mnt_h.totalFrame = 2;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(ICC_BUF-sizeof(RFILE_WRITE_HEAD)-sizeof(RFILE_MNTN_HEADER));

     memset(data,0,sizeof(RFILE_WRITE_HEAD));

     /*fclose opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fclose fp*/
	 INIT_LIST_HEAD(&(fp_list_file.list));
	 buf_ret = fp_list_add_file(buf);
     ((RFILE_WRITE_HEAD*)data)->fp = buf_ret;

     ((RFILE_WRITE_HEAD*)data)->size = 10;

     rfile_linux_ops.bsp_fwrite = bsp_linux_fwrite;

     g_rfile_wt_expect_cur = 0;

     ret = fwriteProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fwriteProxy_fwrite_writesize_exclude( void )
{
     int      ret   = 0;
     int  opstype   = FWRITE;
     RFILE     fp    = {0};
	 char buf[100] = {0};
	 int buf_ret;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FWRITE;
     mnt_h.totalFrame = 2;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(ICC_BUF-sizeof(RFILE_WRITE_HEAD)-sizeof(RFILE_MNTN_HEADER));

     memset(data,0,sizeof(RFILE_WRITE_HEAD));

     /*fclose opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fclose fp*/
	 INIT_LIST_HEAD(&(fp_list_file.list));
	 buf_ret = fp_list_add_file(buf);
     ((RFILE_WRITE_HEAD*)data)->fp = buf_ret;

     ((RFILE_WRITE_HEAD*)data)->size = 64*1024;

     rfile_linux_ops.bsp_fwrite = bsp_linux_fwrite;

     g_rfile_wt_expect_cur = 0;

     ret = fwriteProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fwriteProxy_fwrite_frame_last( void )
{
     int      ret   = 0;
     int  opstype   = FWRITE;
     RFILE     fp    = {0};
	 char buf[100] = {0};
	 int buf_ret;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FWRITE;
     mnt_h.totalFrame = 2;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(ICC_BUF-sizeof(RFILE_WRITE_HEAD)-sizeof(RFILE_MNTN_HEADER));

     memset(data,0,sizeof(RFILE_WRITE_HEAD));

     /*fwrite opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fwrite fp*/
	 INIT_LIST_HEAD(&(fp_list_file.list));
	 buf_ret = fp_list_add_file(buf);
     ((RFILE_WRITE_HEAD*)data)->fp = buf_ret;

     ((RFILE_WRITE_HEAD*)data)->size = 10;

     rfile_linux_ops.bsp_fwrite = bsp_linux_fwrite;

     g_rfile_wt_expect_cur = 1;

     ret = fwriteProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fseekProxy_wrong_pkg( void )
{
     int      ret   = 0;
     int  opstype   = FSEEK;
     RFILE     fp    = {0};

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FSEEK;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_SEEK_HEAD));

     memset(data,0,sizeof(RFILE_SEEK_HEAD));

     /*fseek opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fseek fp*/
     ((RFILE_SEEK_HEAD*)data)->fp     = &fp;

     ((RFILE_SEEK_HEAD*)data)->offset = 0;

     ((RFILE_SEEK_HEAD*)data)->whence = 0;

     rfile_linux_ops.bsp_fseek = bsp_linux_fseek;

     ret = fseekProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fseekProxy_fp_null( void )
{
     int      ret   = 0;
     int  opstype   = FSEEK;
     RFILE     fp    = {0};

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FSEEK;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_SEEK_HEAD));

     memset(data,0,sizeof(RFILE_SEEK_HEAD));

     /*fseek opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fseek fp*/
     ((RFILE_SEEK_HEAD*)data)->fp     = NULL;

     ((RFILE_SEEK_HEAD*)data)->offset = 0;

     ((RFILE_SEEK_HEAD*)data)->whence = 0;

     rfile_linux_ops.bsp_fseek = bsp_linux_fseek;

     ret = fseekProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fseekProxy_fseek_unreg( void )
{
     int      ret   = 0;
     int  opstype   = FSEEK;
     RFILE     fp    = {0};

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FSEEK;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_SEEK_HEAD));

     memset(data,0,sizeof(RFILE_SEEK_HEAD));

     /*fseek opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fseek fp*/
     ((RFILE_SEEK_HEAD*)data)->fp     = &fp;

     ((RFILE_SEEK_HEAD*)data)->offset = 0;

     ((RFILE_SEEK_HEAD*)data)->whence = 0;

     rfile_linux_ops.bsp_fseek = NULL;

     ret = fseekProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fseekProxy_fseek_fpcheck_fail( void )
{
     int      ret   = 0;
     int  opstype   = FSEEK;
     RFILE     fp    = {0};
	 char buf[100] = {0};
	 int buf_ret;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FSEEK;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_SEEK_HEAD));

     memset(data,0,sizeof(RFILE_SEEK_HEAD));

     /*fseek opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fseek fp*/
	 INIT_LIST_HEAD(&(fp_list_file.list));
     ((RFILE_SEEK_HEAD*)data)->fp     = 1;

     ((RFILE_SEEK_HEAD*)data)->offset = 0;

     ((RFILE_SEEK_HEAD*)data)->whence = 0;

     rfile_linux_ops.bsp_fseek = bsp_linux_fseek;

     ret = fseekProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fseekProxy_fseek_success( void )
{
     int      ret   = 0;
     int  opstype   = FSEEK;
     RFILE     fp    = {0};
	 char buf[100] = {0};
	 int buf_ret;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FSEEK;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_SEEK_HEAD));

     memset(data,0,sizeof(RFILE_SEEK_HEAD));

     /*fseek opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fseek fp*/
	 INIT_LIST_HEAD(&(fp_list_file.list));
	 buf_ret = fp_list_add_file(buf);
     ((RFILE_SEEK_HEAD*)data)->fp     = buf_ret;

     ((RFILE_SEEK_HEAD*)data)->offset = 0;

     ((RFILE_SEEK_HEAD*)data)->whence = 0;

     rfile_linux_ops.bsp_fseek = bsp_linux_fseek;

     ret = fseekProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fremoveProxy_wrong_pkg( void )
{
     int      ret   = 0;
     int  opstype   = REMOVE;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0/nv.bak";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = REMOVE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_REMOVE_HEAD));

     memset(data,0,sizeof(RFILE_REMOVE_HEAD));

     /*fremove opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fremove fp*/
     ((RFILE_REMOVE_HEAD*)data)->pathLen   = strlen(buf);

     rfile_linux_ops.bsp_remove = bsp_linux_remove;

     ret = fremoveProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fremoveProxy_pathlen_zero( void )
{
     int      ret   = 0;
     int  opstype   = REMOVE;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0/nv.bak";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = REMOVE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_REMOVE_HEAD));

     memset(data,0,sizeof(RFILE_REMOVE_HEAD));

     /*fremove opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fremove fp*/
     ((RFILE_REMOVE_HEAD*)data)->pathLen   = 0;

     rfile_linux_ops.bsp_remove = bsp_linux_remove;

     ret = fremoveProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fremoveProxy_remov_unreg( void )
{
     int      ret   = 0;
     int  opstype   = REMOVE;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0/nv.bak";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = REMOVE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_REMOVE_HEAD));

     memset(data,0,sizeof(RFILE_REMOVE_HEAD));

     /*fremove opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fremove fp*/
     ((RFILE_REMOVE_HEAD*)data)->pathLen   = strlen(buf);

     rfile_linux_ops.bsp_remove = NULL;

     ret = fremoveProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fremoveProxy_remov_success( void )
{
     int      ret   = 0;
     int  opstype   = REMOVE;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0/nv.bak";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = REMOVE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_REMOVE_HEAD)+strlen(buf)+1);
     memset(data,0,sizeof(RFILE_REMOVE_HEAD)+strlen(buf)+1);

     /*fremove opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fremove fp*/
     ((RFILE_REMOVE_HEAD*)data)->pathLen   = strlen(buf);
     memcpy(data+sizeof(RFILE_REMOVE_HEAD),buf,sizeof(buf));

     rfile_linux_ops.bsp_remove = bsp_linux_remove;
     rfile_linux_ops.bsp_access= bsp_linux_access;

     ret = fremoveProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_ftellProxy_wrong_pkg( void )
{
     int      ret   = 0;
     int  opstype   = FTELL;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0/nv.bak";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FTELL;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_TELL_HEAD));

     memset(data,0,sizeof(RFILE_TELL_HEAD));

     /*opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fp*/
     ((RFILE_TELL_HEAD*)data)->fp   = &fp;

     rfile_linux_ops.bsp_ftell = bsp_linux_ftell;

     ret = ftellProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_ftellProxy_fp_null( void )
{
     int      ret   = 0;
     int  opstype   = FTELL;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0/nv.bak";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FTELL;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_TELL_HEAD));

     memset(data,0,sizeof(RFILE_TELL_HEAD));

     /*opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fp*/
     ((RFILE_TELL_HEAD*)data)->fp   = 0;

     rfile_linux_ops.bsp_ftell = bsp_linux_ftell;

     ret = ftellProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_ftellProxy_fun_null( void )
{
     int      ret   = 0;
     int  opstype   = FTELL;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0/nv.bak";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FTELL;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_TELL_HEAD));

     memset(data,0,sizeof(RFILE_TELL_HEAD));

     /*opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fp*/
     ((RFILE_TELL_HEAD*)data)->fp   = &fp;

     rfile_linux_ops.bsp_ftell = 0;

     ret = ftellProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_ftellProxy_fpcheck_fail( void )
{
     int      ret   = 0;
     int  opstype   = FTELL;
     RFILE     fp    = {0};
	 char buf1[100] = {0};
	 int buf_ret;

     char *  data = NULL;
     char buf[]   = "/yaffs0/nv.bak";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FTELL;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_TELL_HEAD));

     memset(data,0,sizeof(RFILE_TELL_HEAD));

     /*opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fp*/
	 INIT_LIST_HEAD(&(fp_list_file.list));
     ((RFILE_TELL_HEAD*)data)->fp   = 1;
	 
     rfile_linux_ops.bsp_ftell = bsp_linux_ftell;

     ret = ftellProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_ftellProxy_success( void )
{
     int      ret   = 0;
     int  opstype   = FTELL;
     RFILE     fp    = {0};
	 char buf1[100] = {0};
	 int buf_ret;

     char *  data = NULL;
     char buf[]   = "/yaffs0/nv.bak";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FTELL;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_TELL_HEAD));

     memset(data,0,sizeof(RFILE_TELL_HEAD));

     /*opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fp*/
	 INIT_LIST_HEAD(&(fp_list_file.list));
	 buf_ret = fp_list_add_file(buf1);
     ((RFILE_TELL_HEAD*)data)->fp   = buf_ret;

     rfile_linux_ops.bsp_ftell = bsp_linux_ftell;

     ret = ftellProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fmkdirProxy_wrong_pkg( void )
{
     int      ret   = 0;
     int  opstype   = FMKDIR;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FMKDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_MKDIR_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_MKDIR_HEAD)+sizeof(buf));

     /*fmkdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fmkdir pathlen*/
     ((RFILE_MKDIR_HEAD*)data)->pathlen   = strlen(buf);

     rfile_linux_ops.bsp_mkdir = bsp_linux_mkdir;

     ret = fmkdirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fmkdirProxy_pathlen_null( void )
{
     int      ret   = 0;
     int  opstype   = FMKDIR;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FMKDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_MKDIR_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_MKDIR_HEAD)+sizeof(buf));

     /*fmkdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fmkdir pathlen*/
     ((RFILE_MKDIR_HEAD*)data)->pathlen   = 0;

     rfile_linux_ops.bsp_mkdir = bsp_linux_mkdir;

     ret = fmkdirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fmkdirProxy_mkdir_unreg( void )
{
     int      ret   = 0;
     int  opstype   = FMKDIR;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FMKDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_MKDIR_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_MKDIR_HEAD)+sizeof(buf));

     /*fmkdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fmkdir pathlen*/
     ((RFILE_MKDIR_HEAD*)data)->pathlen   = strlen(buf);

     rfile_linux_ops.bsp_mkdir = NULL;

     ret = fmkdirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fmkdirProxy_mkdir_success( void )
{
     int      ret   = 0;
     int  opstype   = FMKDIR;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FMKDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_MKDIR_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_MKDIR_HEAD)+sizeof(buf));

     /*fmkdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fmkdir pathlen*/
     ((RFILE_MKDIR_HEAD*)data)->pathlen   = strlen(buf);

     rfile_linux_ops.bsp_mkdir = bsp_linux_mkdir;

     ret = fmkdirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_frmdirProxy_wrong_pkg( void )
{
     int      ret   = 0;
     int  opstype   = FRMDIR;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FRMDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_RMDIR_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_RMDIR_HEAD)+sizeof(buf));

     /*frmdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*frmdir pathlen*/
     ((RFILE_RMDIR_HEAD*)data)->pathlen   = strlen(buf);

     rfile_linux_ops.bsp_rmdir = bsp_linux_rmdir;

     ret = frmdirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_frmdirProxy_pathlen_null( void )
{
     int      ret   = 0;
     int  opstype   = FRMDIR;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FRMDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_RMDIR_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_RMDIR_HEAD)+sizeof(buf));

     /*frmdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*frmdir pathlen*/
     ((RFILE_RMDIR_HEAD*)data)->pathlen   = 0;

     rfile_linux_ops.bsp_rmdir = bsp_linux_rmdir;

     ret = frmdirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_frmdirProxy_rmdir_unreg( void )
{
     int      ret   = 0;
     int  opstype   = FRMDIR;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FRMDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_RMDIR_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_RMDIR_HEAD)+sizeof(buf));

     /*frmdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*frmdir pathlen*/
     ((RFILE_RMDIR_HEAD*)data)->pathlen   = strlen(buf);

     rfile_linux_ops.bsp_rmdir = NULL;

     ret = frmdirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_frmdirProxy_rmdir_success( void )
{
     int      ret   = 0;
     int  opstype   = FRMDIR;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FRMDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_RMDIR_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_RMDIR_HEAD)+sizeof(buf));

     /*frmdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*frmdir pathlen*/
     ((RFILE_RMDIR_HEAD*)data)->pathlen   = strlen(buf);

     rfile_linux_ops.bsp_rmdir = bsp_linux_rmdir;

     ret = frmdirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fopendirProxy_wrong_pkg( void )
{
     int      ret   = 0;
     int  opstype   = FOPENDIR;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FOPENDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_OPENDIR_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_OPENDIR_HEAD)+sizeof(buf));

     /*fopendir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fopendir pathlen*/
     ((RFILE_OPENDIR_HEAD*)data)->pathlen   = strlen(buf);

     rfile_linux_ops.bsp_opendir = bsp_linux_opendir;

     ret = fopendirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fopendirProxy_pathlen_null( void )
{
     int      ret   = 0;
     int  opstype   = FOPENDIR;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FOPENDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_OPENDIR_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_OPENDIR_HEAD)+sizeof(buf));

     /*fopendir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fopendir pathlen*/
     ((RFILE_OPENDIR_HEAD*)data)->pathlen   = 0;

     rfile_linux_ops.bsp_opendir = bsp_linux_opendir;

     ret = fopendirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fopendirProxy_opendir_unreg( void )
{
     int      ret   = 0;
     int  opstype   = FOPENDIR;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FOPENDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_OPENDIR_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_OPENDIR_HEAD)+sizeof(buf));

     /*fopendir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fopendir pathlen*/
     ((RFILE_OPENDIR_HEAD*)data)->pathlen   = strlen(buf);

     rfile_linux_ops.bsp_opendir = NULL;

     ret = fopendirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fopendirProxy_opendir_success( void )
{
     int      ret   = 0;
     int  opstype   = FOPENDIR;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FOPENDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_OPENDIR_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_OPENDIR_HEAD)+sizeof(buf));

     /*fopendir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*fopendir pathlen*/
     ((RFILE_OPENDIR_HEAD*)data)->pathlen   = strlen(buf);

     rfile_linux_ops.bsp_opendir = bsp_linux_opendir;

     ret = fopendirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_freaddirProxy_wrong_pkg( void )
{
     int      ret   = 0;
     int  opstype   = FREADDIR;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FREADDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     data = (char*)malloc(sizeof(RFILE_READDIR_HEAD));

     memset(data,0,sizeof(RFILE_READDIR_HEAD));

     memcpy(data,&opstype,sizeof(unsigned int));

    ((RFILE_READDIR_HEAD*)data)->dir   = 1;

     rfile_linux_ops.bsp_readdir = bsp_linux_readdir;

     ret = freaddirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}


int test_freaddirProxy_dir_null( void )
{
     int      ret   = 0;
     int  opstype   = FREADDIR;
     RFILE     fp    = {0};

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FREADDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_READDIR_HEAD));

     memset(data,0,sizeof(RFILE_READDIR_HEAD));

     /*freaddir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*freaddir dir*/
     ((RFILE_READDIR_HEAD*)data)->dir   = 0;

     rfile_linux_ops.bsp_readdir = bsp_linux_readdir;

     ret = freaddirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_freaddirProxy_readdir_unreg( void )
{
     int      ret   = 0;
     int  opstype   = FREADDIR;
     RFILE     fp    = {0};

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FREADDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_READDIR_HEAD));

     memset(data,0,sizeof(RFILE_READDIR_HEAD));

     /*freaddir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*freaddir dir*/
     ((RFILE_READDIR_HEAD*)data)->dir   = 1;

     rfile_linux_ops.bsp_readdir = NULL;

     ret = freaddirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_freaddirProxy_readdir_fpcheck_fail( void )
{
     int      ret   = 0;
     int  opstype   = FREADDIR;
     RFILE     fp    = {0};
	 char buf[100] = {0};
	 int buf_ret;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FREADDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_READDIR_HEAD));

     memset(data,0,sizeof(RFILE_READDIR_HEAD));

     /*freaddir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*freaddir dir*/
	 INIT_LIST_HEAD(&(fp_list_dir.list));
     ((RFILE_READDIR_HEAD*)data)->dir   = 1;

     rfile_linux_ops.bsp_readdir = bsp_linux_readdir;

     ret = freaddirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_freaddirProxy_readdir_success( void )
{
     int      ret   = 0;
     int  opstype   = FREADDIR;
     RFILE     fp    = {0};
	 char buf[100] = {0};
	 int buf_ret;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FREADDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_READDIR_HEAD));

     memset(data,0,sizeof(RFILE_READDIR_HEAD));

     /*freaddir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*freaddir dir*/
	 INIT_LIST_HEAD(&(fp_list_dir.list));
	 buf_ret = fp_list_add_dir(buf);
     ((RFILE_READDIR_HEAD*)data)->dir   = buf_ret;

     rfile_linux_ops.bsp_readdir = bsp_linux_readdir;

     ret = freaddirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fstatdirProxy_wrong_pkg( void )
{
     int      ret   = 0;
     int  opstype   = FSTAT;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FSTAT;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_STAT_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_STAT_HEAD)+sizeof(buf));

     /*statdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*statdir pathlen*/
     ((RFILE_STAT_HEAD*)data)->pathlen   = strlen(buf);

     rfile_linux_ops.bsp_stat = bsp_linux_stat;

     ret = fstatdirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fstatdirProxy_pathlen_null( void )
{
     int      ret   = 0;
     int  opstype   = FSTAT;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FSTAT;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_STAT_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_STAT_HEAD)+sizeof(buf));

     /*statdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*statdir pathlen*/
     ((RFILE_STAT_HEAD*)data)->pathlen   = 0;

     rfile_linux_ops.bsp_stat = bsp_linux_stat;

     ret = fstatdirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fstatdirProxy_statdir_unreg( void )
{
     int      ret   = 0;
     int  opstype   = FSTAT;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FSTAT;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_STAT_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_STAT_HEAD)+sizeof(buf));

     /*statdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*statdir pathlen*/
     ((RFILE_STAT_HEAD*)data)->pathlen   = strlen(buf);

     rfile_linux_ops.bsp_stat = NULL;

     ret = fstatdirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fstatdirProxy_statdir_success( void )
{
     int      ret   = 0;
     int  opstype   = FSTAT;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FSTAT;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_STAT_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_STAT_HEAD)+sizeof(buf));

     /*statdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*statdir pathlen*/
     ((RFILE_STAT_HEAD*)data)->pathlen   = strlen(buf);

     rfile_linux_ops.bsp_stat = bsp_linux_stat;

     ret = fstatdirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fxdeletedirProxy_wrong_pkg( void )
{
     int      ret   = 0;
     int  opstype   = FXDELETE;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FXDELETE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_XDELETE_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_XDELETE_HEAD)+sizeof(buf));

     /*xdeletedir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*xdeletedir pathlen*/
     ((RFILE_XDELETE_HEAD*)data)->source_path_len   = strlen(buf);

     rfile_linux_ops.bsp_xdelete = bsp_linux_xdelete;

     ret = fxdeletedirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fxdeletedirProxy_pathlen_null( void )
{
     int      ret   = 0;
     int  opstype   = FXDELETE;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FXDELETE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_XDELETE_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_XDELETE_HEAD)+sizeof(buf));

     /*xdeletedir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*xdeletedir pathlen*/
     ((RFILE_XDELETE_HEAD*)data)->source_path_len   = 0;

     rfile_linux_ops.bsp_xdelete = bsp_linux_xdelete;

     ret = fxdeletedirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fxdeletedirProxy_xdeletedir_unreg( void )
{
     int      ret   = 0;
     int  opstype   = FXDELETE;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FXDELETE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_XDELETE_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_XDELETE_HEAD)+sizeof(buf));

     /*xdeletedir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*xdeletedir pathlen*/
     ((RFILE_XDELETE_HEAD*)data)->source_path_len   = strlen(buf);

     rfile_linux_ops.bsp_xdelete = NULL;

     ret = fxdeletedirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fxdeletedirProxy_xdeletedir_success( void )
{
     int      ret   = 0;
     int  opstype   = FXDELETE;
     RFILE     fp    = {0};

     char *  data = NULL;
     char buf[]   = "/yaffs0";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FXDELETE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_XDELETE_HEAD)+sizeof(buf));

     memset(data,0,sizeof(RFILE_XDELETE_HEAD)+sizeof(buf));

     /*xdeletedir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*xdeletedir pathlen*/
     ((RFILE_XDELETE_HEAD*)data)->source_path_len   = strlen(buf);

     rfile_linux_ops.bsp_xdelete = bsp_linux_xdelete;

     ret = fxdeletedirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_faccessProxy_accessdir_wrong_pkg( void )
{
     int      ret   = 0;
     int  opstype   = FACCESS;
     RFILE     fp    = {0};

     char *  data = NULL;
     char path[] = "/yaffs0";
     char mode[] = "b+";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FACCESS;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_ACCESS_HEAD)+sizeof(path)+sizeof(mode));

     memset(data,0,sizeof(RFILE_ACCESS_HEAD)+sizeof(path)+sizeof(mode));

     /*accessdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*accessdir pathlen*/
     ((RFILE_ACCESS_HEAD*)data)->pathLen   = strlen(path);

     /*accessdir modelen*/
     ((RFILE_ACCESS_HEAD*)data)->modeLen   = strlen(mode);

     rfile_linux_ops.bsp_access = bsp_linux_access;

     ret = faccessProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}


int test_faccessProxy_accessdir_modelen_null( void )
{
     int      ret   = 0;
     int  opstype   = FACCESS;
     RFILE     fp    = {0};

     char *  data = NULL;
     char path[] = "/yaffs0";
     char mode[] = "b+";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FACCESS;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_ACCESS_HEAD)+sizeof(path)+sizeof(mode));

     memset(data,0,sizeof(RFILE_ACCESS_HEAD)+sizeof(path)+sizeof(mode));

     /*accessdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*accessdir pathlen*/
     ((RFILE_ACCESS_HEAD*)data)->pathLen   = strlen(path);

     /*accessdir modelen*/
     ((RFILE_ACCESS_HEAD*)data)->modeLen   = 0;

     rfile_linux_ops.bsp_access = bsp_linux_access;

     ret = faccessProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}



int test_faccessProxy_accessdir_pathlen_null( void )
{
     int      ret   = 0;
     int  opstype   = FACCESS;
     RFILE     fp    = {0};

     char *  data = NULL;
     char path[] = "/yaffs0";
     char mode[] = "b+";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FACCESS;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_ACCESS_HEAD)+sizeof(path)+sizeof(mode));

     memset(data,0,sizeof(RFILE_ACCESS_HEAD)+sizeof(path)+sizeof(mode));

     /*accessdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*accessdir pathlen*/
     ((RFILE_ACCESS_HEAD*)data)->pathLen   = 0;

     /*accessdir modelen*/
     ((RFILE_ACCESS_HEAD*)data)->modeLen   = strlen(mode);

     rfile_linux_ops.bsp_access = bsp_linux_access;

     ret = faccessProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}


int test_faccessProxy_accessdir_accessdir_unreg( void )
{
     int      ret   = 0;
     int  opstype   = FACCESS;
     RFILE     fp    = {0};

     char *  data = NULL;
     char path[] = "/yaffs0";
     char mode[] = "b+";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FACCESS;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_ACCESS_HEAD)+sizeof(path)+sizeof(mode));

     memset(data,0,sizeof(RFILE_ACCESS_HEAD)+sizeof(path)+sizeof(mode));

     /*accessdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*accessdir pathlen*/
     ((RFILE_ACCESS_HEAD*)data)->pathLen   = strlen(path);

     /*accessdir modelen*/
     ((RFILE_ACCESS_HEAD*)data)->modeLen   = strlen(mode);

     rfile_linux_ops.bsp_access = NULL;

     ret = faccessProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}


int test_faccessProxy_accessdir_success( void )
{
     int      ret   = 0;
     int  opstype   = FACCESS;
     RFILE     fp    = {0};

     char *  data = NULL;
     char path[] = "/yaffs0";
     char mode[] = "b+";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FACCESS;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_ACCESS_HEAD)+sizeof(path)+sizeof(mode));

     memset(data,0,sizeof(RFILE_ACCESS_HEAD)+sizeof(path)+sizeof(mode));

     /*accessdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*accessdir pathlen*/
     ((RFILE_ACCESS_HEAD*)data)->pathLen   = strlen(path);

     /*accessdir modelen*/
     ((RFILE_ACCESS_HEAD*)data)->modeLen   = strlen(mode);

     rfile_linux_ops.bsp_access = bsp_linux_access;

     ret = faccessProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}


int test_fxcopyProxy_wrong_pkg( void )
{
     int      ret   = 0;
     int  opstype   = FXCOPY;
     RFILE     fp    = {0};

     char *  data = NULL;
     char sour_path[] = "/yaffs0";
     char dest_path[] = "/yaffs1";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FXCOPY;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_XCOPY_HEAD)+sizeof(sour_path)+sizeof(dest_path));

     memset(data,0,sizeof(RFILE_XCOPY_HEAD)+sizeof(sour_path)+sizeof(dest_path));

     /*accessdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*accessdir pathlen*/
     ((RFILE_XCOPY_HEAD*)data)->source_path_len   = strlen(sour_path);

     /*accessdir modelen*/
     ((RFILE_XCOPY_HEAD*)data)->dest_path_len     = strlen(dest_path);

     rfile_linux_ops.bsp_xcopy = bsp_linux_xcopy;

     ret = fxcopydirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}



int test_fxcopyProxy_dest_pathlen_null( void )
{
     int      ret   = 0;
     int  opstype   = FXCOPY;
     RFILE     fp    = {0};

     char *  data = NULL;
     char sour_path[] = "/yaffs0";
     char dest_path[] = "/yaffs1";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FXCOPY;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_XCOPY_HEAD)+sizeof(sour_path)+sizeof(dest_path));

     memset(data,0,sizeof(RFILE_XCOPY_HEAD)+sizeof(sour_path)+sizeof(dest_path));

     /*accessdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*accessdir pathlen*/
     ((RFILE_XCOPY_HEAD*)data)->source_path_len   = strlen(sour_path);

     /*accessdir modelen*/
     ((RFILE_XCOPY_HEAD*)data)->dest_path_len     = 0;

     rfile_linux_ops.bsp_xcopy = bsp_linux_xcopy;

     ret = fxcopydirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}



int test_fxcopyProxy_source_pathlen_null( void )
{
     int      ret   = 0;
     int  opstype   = FXCOPY;
     RFILE     fp    = {0};

     char *  data = NULL;
     char sour_path[] = "/yaffs0";
     char dest_path[] = "/yaffs1";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FXCOPY;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_XCOPY_HEAD)+sizeof(sour_path)+sizeof(dest_path));

     memset(data,0,sizeof(RFILE_XCOPY_HEAD)+sizeof(sour_path)+sizeof(dest_path));

     /*accessdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*accessdir pathlen*/
     ((RFILE_XCOPY_HEAD*)data)->source_path_len   = 0;

     /*accessdir modelen*/
     ((RFILE_XCOPY_HEAD*)data)->dest_path_len     = strlen(dest_path);

     rfile_linux_ops.bsp_xcopy = bsp_linux_xcopy;

     ret = fxcopydirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}



int test_fxcopyProxy_xcopydir_unreg( void )
{
     int      ret   = 0;
     int  opstype   = FXCOPY;
     RFILE     fp    = {0};

     char *  data = NULL;
     char sour_path[] = "/yaffs0";
     char dest_path[] = "/yaffs1";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FXCOPY;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_XCOPY_HEAD)+sizeof(sour_path)+sizeof(dest_path));

     memset(data,0,sizeof(RFILE_XCOPY_HEAD)+sizeof(sour_path)+sizeof(dest_path));

     /*accessdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*accessdir pathlen*/
     ((RFILE_XCOPY_HEAD*)data)->source_path_len   = strlen(sour_path);

     /*accessdir modelen*/
     ((RFILE_XCOPY_HEAD*)data)->dest_path_len     = strlen(dest_path);

     rfile_linux_ops.bsp_xcopy = NULL;

     ret = fxcopydirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}


int test_fxcopydirProxy_xcopydir_success( void )
{
     int      ret   = 0;
     int  opstype   = FXCOPY;
     RFILE     fp    = {0};

     char *  data = NULL;
     char sour_path[] = "/yaffs0";
     char dest_path[] = "/yaffs1";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FXCOPY;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_XCOPY_HEAD)+sizeof(sour_path)+sizeof(dest_path));

     memset(data,0,sizeof(RFILE_XCOPY_HEAD)+sizeof(sour_path)+sizeof(dest_path));

     /*accessdir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*accessdir pathlen*/
     ((RFILE_XCOPY_HEAD*)data)->source_path_len   = strlen(sour_path);

     /*accessdir modelen*/
     ((RFILE_XCOPY_HEAD*)data)->dest_path_len     = strlen(dest_path);

     rfile_linux_ops.bsp_xcopy = bsp_linux_xcopy;

     ret = fxcopydirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}



int test_frenameProxy_wrong_pkg( void )
{
     int      ret   = 0;
     int  opstype   = FRENAME;
     RFILE     fp    = {0};

     char *  data = NULL;
     char new_path[] = "/yaffs0";
     char old_path[] = "/yaffs1";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FRENAME;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_RENAME_HEAD)+sizeof(new_path)+sizeof(old_path));

     memset(data,0,sizeof(RFILE_RENAME_HEAD)+sizeof(new_path)+sizeof(old_path));

     /*renamedir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*renamedir pathlen*/
     ((RFILE_RENAME_HEAD*)data)->old_path_len   = strlen(old_path);

     /*renamedir modelen*/
     ((RFILE_RENAME_HEAD*)data)->new_path_len   = strlen(new_path);

     rfile_linux_ops.bsp_rename = bsp_linux_rename;

     ret = frenameProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}



int test_frenameProxy_new_pathlen_null( void )
{
     int      ret   = 0;
     int  opstype   = FRENAME;
     RFILE     fp    = {0};

     char *  data = NULL;
     char new_path[] = "/yaffs0";
     char old_path[] = "/yaffs1";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FRENAME;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_RENAME_HEAD)+sizeof(new_path)+sizeof(old_path));

     memset(data,0,sizeof(RFILE_RENAME_HEAD)+sizeof(new_path)+sizeof(old_path));

     /*renamedir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*renamedir pathlen*/
     ((RFILE_RENAME_HEAD*)data)->old_path_len   = strlen(old_path);

     /*renamedir modelen*/
     ((RFILE_RENAME_HEAD*)data)->new_path_len   = 0;

     rfile_linux_ops.bsp_rename = bsp_linux_rename;

     ret = frenameProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}



int test_frenameProxy_old_pathlen_null( void )
{
     int      ret   = 0;
     int  opstype   = FRENAME;
     RFILE     fp    = {0};

     char *  data = NULL;
     char new_path[] = "/yaffs0";
     char old_path[] = "/yaffs1";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FRENAME;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_RENAME_HEAD)+sizeof(new_path)+sizeof(old_path));

     memset(data,0,sizeof(RFILE_RENAME_HEAD)+sizeof(new_path)+sizeof(old_path));

     /*renamedir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*renamedir pathlen*/
     ((RFILE_RENAME_HEAD*)data)->old_path_len   = 0;

     /*renamedir modelen*/
     ((RFILE_RENAME_HEAD*)data)->new_path_len   = strlen(new_path);

     rfile_linux_ops.bsp_rename = bsp_linux_rename;

     ret = frenameProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}



int test_frenameProxy_renamedir_unreg( void )
{
     int      ret   = 0;
     int  opstype   = FRENAME;
     RFILE     fp    = {0};

     char *  data = NULL;
     char new_path[] = "/yaffs0";
     char old_path[] = "/yaffs1";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FRENAME;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_RENAME_HEAD)+sizeof(new_path)+sizeof(old_path));

     memset(data,0,sizeof(RFILE_RENAME_HEAD)+sizeof(new_path)+sizeof(old_path));

     /*renamedir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*renamedir pathlen*/
     ((RFILE_RENAME_HEAD*)data)->old_path_len   = strlen(old_path);

     /*renamedir modelen*/
     ((RFILE_RENAME_HEAD*)data)->new_path_len   = strlen(new_path);

     rfile_linux_ops.bsp_rename = NULL;

     ret = frenameProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}


int test_frenamedirProxy_renameir_success( void )
{
     int      ret   = 0;
     int  opstype   = FRENAME;
     RFILE     fp    = {0};

     char *  data = NULL;
     char new_path[] = "/yaffs0";
     char old_path[] = "/yaffs1";

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FRENAME;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_RENAME_HEAD)+sizeof(new_path)+sizeof(old_path));

     memset(data,0,sizeof(RFILE_RENAME_HEAD)+sizeof(new_path)+sizeof(old_path));

     /*renamedir opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*renamedir pathlen*/
     ((RFILE_RENAME_HEAD*)data)->old_path_len   = strlen(old_path);

     /*renamedir modelen*/
     ((RFILE_RENAME_HEAD*)data)->new_path_len   = strlen(new_path);

     rfile_linux_ops.bsp_rename = bsp_linux_rename;

     ret = frenameProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}



int test_bsp_linux_fopen_path_null( void )
{
     int      ret   = 0;
     char path[] = "/yaffs0/nv.bak";
     char mode[] = "b+";
	 	
     ret = bsp_linux_fopen(0,&mode);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}



int test_bsp_linux_fopen_mode_null( void )
{
     int      ret   = 0;
     char path[] = "/yaffs0/nv.bak";
     char mode[] = "b+";
	 
     ret = bsp_linux_fopen(&path,0);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_fopen( void )
{
     int      ret   = 0;
     char path[] = "/yaffs0/nv.bak";
     char mode[] = "b+";
	 g_rfilesystem_init = 1;
	 
     ret = bsp_linux_fopen(&path,&mode);

     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}



int test_bsp_linux_fclose_stream_null( void )
{
     int      ret   = 0;
     RFILE stream={0};

     ret = bsp_linux_fclose(0);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_fclose( void )
{
     int      ret   = 0;
     RFILE stream={0};
	 g_rfilesystem_init = 1;

     ret = bsp_linux_fclose(&stream);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}



int test_bsp_linux_fread_buf_null( void )
{
     int      ret   = 0;
     char buf[20]   = {0};
     int      len   = 20;
     int      fp    = 1;

     ret = bsp_linux_fread(0,len,&fp);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_fread( void )
{
     int      ret   = 0;
     char buf[20]   = {0};
     int      len   = 20;
     RFILE      fp    = {0};
	 g_rfilesystem_init = 1;

     ret = bsp_linux_fread(buf,len,&fp);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_fwrite_buf_null( void )
{
     int      ret   = 0;
     char buf[20]   = {0};
     int      len   = 20;
     RFILE      fp    = {0};

     ret = bsp_linux_fwrite(0,len,&fp);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_fwrite( void )
{
     int      ret   = 0;
     char buf[20]   = {0};
     int      len   = 20;
     RFILE      fp    = {0};
	 g_rfilesystem_init = 1;

     ret = bsp_linux_fwrite(buf,len,&fp);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}



int test_bsp_linux_fseek_stream_null( void )
{
     int      ret   = 0;
     char buf[20]   = {0};
     int      len   = 20;
     RFILE      fp    = {0};
     int     whence = 0;

     ret = bsp_linux_fseek(0,len,whence);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_fseek( void )
{
     int      ret   = 0;
     char buf[20]   = {0};
     int      len   = 20;
     RFILE      fp    = {0};
     int     whence = 0;
	 g_rfilesystem_init = 1;
	 
     ret = bsp_linux_fseek(&fp,len,whence);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_remove_pathname_null( void )
{
     int      ret   = 0;
     char   pathname[] = "/yaffs0/nv.bak";


     ret = bsp_linux_remove(0);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_remove( void )
{
     int      ret   = 0;
     char   pathname[] = "/yaffs0/nv.bak";
	 g_rfilesystem_init = 1;

     ret = bsp_linux_remove(pathname);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_ftell_stream_null( void )
{
     int      ret   = 0;
     int      fp    = 1;


     ret = bsp_linux_ftell(0);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_ftell( void )
{
     int      ret   = 0;
     RFILE      fp    = {0};
	 g_rfilesystem_init = 1;

     ret = bsp_linux_ftell(&fp);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_mkdir_path_null( void )
{
     int      ret      = 0;
     char     pathname[] = "/yaffs0/nv.bak";


     ret = bsp_linux_mkdir(0);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_mkdir( void )
{
     int      ret      = 0;
     char pathname[]   = "/yaffs0";
	 g_rfilesystem_init = 1;

     ret = bsp_linux_mkdir(pathname);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_rmdir_path_null( void )
{
     int      ret      = 0;

     ret = bsp_linux_rmdir(0);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_rmdir( void )
{
     int      ret      = 0;
     char pathname[]   = "/yaffs0";
	 g_rfilesystem_init = 1;

     ret = bsp_linux_rmdir(pathname);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_opendir_path_null( void )
{
     int      ret      = 0;
     char     pathname[] = "/yaffs0/";


     ret = bsp_linux_opendir(NULL);
     if(0 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_opendir( void )
{
     int      ret      = 0;
     char pathname[]   = "/yaffs0/";
	 g_rfilesystem_init = 1;

     ret = bsp_linux_opendir(pathname);
     if(0 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}



int test_bsp_linux_readdir_path_null( void )
{
     int      ret      = 0;
     char     pathname[] = "/yaffs0/";
     DRV_DIRENT_S  dirent = {0};


     ret = bsp_linux_readdir(0,&dirent);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_readdir( void )
{
     int      ret      = 0;
     char pathname[]   = "/yaffs0/";
     DRV_DIRENT_S  dirent = {0};
	 g_rfilesystem_init = 1;

     ret = bsp_linux_readdir(pathname,&dirent);
     if(ret >= 0)
     {
         return 0;
     }
     else
     {
         return -1;
     }
}



int test_bsp_linux_closedir_dirent_null( void )
{
     int      ret      = 0;
     DRV_DIRENT_S  dirent = {0};


     ret = bsp_linux_closedir(0);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_closedir( void )
{
     int      ret      = 0;
     DRV_DIRENT_S  dirent = {0};
	g_rfilesystem_init = 1;

     ret = bsp_linux_closedir(&dirent);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}



int test_bsp_linux_stat_dirent_null( void )
{
     int      ret      = 0;

     ret = bsp_linux_stat(0,0);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_stat( void )
{
     int      ret      = 0;
     char pathname[]   = "/yaffs0/";
     DRV_STAT_S   stat = {0};
	 g_rfilesystem_init = 1;

     ret = bsp_linux_stat(pathname,&stat);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_access_dirent_null( void )
{
     int      ret      = 0;

     ret = bsp_linux_access(0,0);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_access( void )
{
     int      ret      = 0;
     char pathname[]   = "/yaffs0/";
	 g_rfilesystem_init = 1;
	 
     ret = bsp_linux_access(pathname,0);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_xdelete_path_null( void )
{
     int      ret      = 0;

     ret = bsp_linux_xdelete(0);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_xdelete( void )
{
     int      ret      = 0;
     char pathname[]   = "/yaffs0/";
	 g_rfilesystem_init = 1;
	 
     ret = bsp_linux_xdelete(pathname);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_rename_path_null( void )
{
     int      ret      = 0;

     ret = bsp_linux_rename(0,0);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_rename( void )
{
     int      ret      = 0;
     char old_pathname[]   = "/yaffs0/";
     char new_pathname[]   = "/yaffs1/";
	 g_rfilesystem_init = 1;

     ret = bsp_linux_rename(old_pathname,new_pathname);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_mass_read_path_null( void )
{
     int      ret      = 0;
     char     pathname[]   = "/yaffs0/";
     char        buf[20]   = {0};

     ret = bsp_mass_read(0,0,20,buf);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_mass_read_path_err( void )
{
     int      ret          = 0;
     char     pathname[]   = "xloader";
     char        buf[20]   = {0};

     ret = bsp_mass_read(pathname,0,20,buf);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_mass_read( void )
{
     int      ret          = 0;
     char     pathname[]   = "boot";
     char        buf[20]   = {0};
	 g_rfilesystem_init = 1;
	 
     ret = bsp_mass_read(pathname,0,20,buf);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}



int test_bsp_mass_write_path_null( void )
{
     int      ret          = 0;
     char     pathname[]   = "xloader";
     char        buf[20]   = {0};

     ret = bsp_mass_write(0,0,20,buf);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_mass_write_path_err( void )
{
     int      ret      = 0;
     char     pathname[]   = "xloader";
     char        buf[20]   = {0};

     ret = bsp_mass_write(pathname,0,20,buf);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}



int test_bsp_mass_write( void )
{
     int      ret      = 0;
     char     pathname[]   = "boot";
     char        buf[20]   = {0};
	 g_rfilesystem_init = 1;
	 
     ret = bsp_mass_write(pathname,0,20,buf);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}



int test_bsp_linux_xcopy_path_null( void )
{
     int      ret      = 0;
     char     pathname[]   = "/yaffs0/";

     ret = bsp_linux_xcopy(0,0);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_bsp_linux_xcopy( void )
{
     int      ret      = 0;
     char     from_pathname[] = "/yaffs0/test_bsp_linux_xcopy/";
     char     to_pathname[]   = "/yaffs1/test_bsp_linux_xcopy";
	 g_rfilesystem_init = 1;

     ret = bsp_linux_xcopy(from_pathname,to_pathname);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}


int test_rfile_calc_begin_blk( void )
{
     int      offset      = 0;
     int      blksize     = 512;
     int      ret         = 0;

     ret = rfile_calc_begin_blk(offset,blksize);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}



int test_rfile_get_rw_info( void )
{
     int      offset      = 0;   /*用户偏移量*/
     int      blksize     = 512; /*存储介质blk大小*/
     int      totallen    = 0;   /*用户需要读取数据长度*/
     int      phy_blk_beg = 0;   /*物理介质起始读取blk*/
     int      ret         = 0;
     RFILE_MASS_RW_PARA rw_blk_info = {0};

     ret = rfile_get_rw_info(&rw_blk_info,offset,blksize,phy_blk_beg);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}



int test_rfile_read_ccpu_pkt_reserve_fail( void )
{
     int   ret = 0;
     RFILE_MNTN_HEADER mntn_header = {0};

     mntn_header.reserve = 0x5555;


     ret = rfile_read_ccpu_pkt(&mntn_header);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}



int test_rfile_read_ccpu_pkt( void )
{
     int   ret = 0;
     RFILE_MNTN_HEADER mntn_header = {0};

     mntn_header.reserve = 0x55aa;


     ret = rfile_read_ccpu_pkt(&mntn_header);
     if(-1 == ret)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}



int test_fclosedirProxy_wrong_pkg( void )
{
     int      ret   = 0;
     int  opstype   = FCLOSEDIR;
     int      dir   = 0;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FCLOSEDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_CLOSEDIR_HEAD));

     memset(data,0,sizeof(RFILE_CLOSEDIR_HEAD));

     /*opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*dir*/
     ((RFILE_CLOSEDIR_HEAD*)data)->dir   = &dir;

     rfile_linux_ops.bsp_closedir = bsp_linux_closedir;

     ret = fclosedirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fclosedirProxy_dir_null( void )
{
     int      ret   = 0;
     int  opstype   = FCLOSEDIR;
     int      dir   = 0;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FCLOSEDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_CLOSEDIR_HEAD));

     memset(data,0,sizeof(RFILE_CLOSEDIR_HEAD));

     /*opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*dir*/
     ((RFILE_CLOSEDIR_HEAD*)data)->dir   = 0;

     rfile_linux_ops.bsp_closedir = bsp_linux_closedir;

     ret = fclosedirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fclosedirProxy_fun_unreg( void )
{
     int      ret   = 0;
     int  opstype   = FCLOSEDIR;
     int      dir   = 0;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FCLOSEDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_CLOSEDIR_HEAD));

     memset(data,0,sizeof(RFILE_CLOSEDIR_HEAD));

     /*opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*dir*/
     ((RFILE_CLOSEDIR_HEAD*)data)->dir   = &dir;

     rfile_linux_ops.bsp_closedir = 0;

     ret = fclosedirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fclosedirProxy_fpcheck_fail( void )
{
     int      ret   = 0;
     int  opstype   = FCLOSEDIR;
     int      dir   = 0;
	 char buf[100] = {0};
	 int buf_ret;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FCLOSEDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_CLOSEDIR_HEAD));

     memset(data,0,sizeof(RFILE_CLOSEDIR_HEAD));

     /*opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*dir*/
	 INIT_LIST_HEAD(&(fp_list_dir.list));
     ((RFILE_CLOSEDIR_HEAD*)data)->dir   = 1;

     rfile_linux_ops.bsp_closedir = bsp_linux_closedir;

     ret = fclosedirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}
int test_fclosedirProxy_success( void )
{
     int      ret   = 0;
     int  opstype   = FCLOSEDIR;
     int      dir   = 0;
	 char buf[100] = {0};
	 int buf_ret;

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FCLOSEDIR;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_CLOSEDIR_HEAD));

     memset(data,0,sizeof(RFILE_CLOSEDIR_HEAD));

     /*opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));

     /*dir*/
	 INIT_LIST_HEAD(&(fp_list_dir.list));
	 buf_ret = fp_list_add_dir(buf);
     ((RFILE_CLOSEDIR_HEAD*)data)->dir   = buf_ret;

     rfile_linux_ops.bsp_closedir = bsp_linux_closedir;

     ret = fclosedirProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}

/*****************************************************************************
被测函数  :  test_rfile_num2string_null
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
int test_rfile_num2string_null()
{
	int ret;
	char string[20];
	
	ret = rfile_num2string(30,NULL); 
	
	if(ret == -1)
	{
		return -1;
	}
	else 
	{
		return 0;
	}
}

/*****************************************************************************
被测函数  :  test_rfile_num2string_success
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
int test_rfile_num2string_success()
{
	int ret;
	char string[20];
	
	ret = rfile_num2string(30,string); 
	
	if(ret == 0 && string[0] == '3' && string[1] == '0')
	{
		return 0;
	}
	else 
	{
		return -1;
	}
}

/*****************************************************************************
被测函数  :  test_rfile_xdelete_null
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
int test_rfile_xdelete_null()
{
	int ret;
	char *pathname = NULL;
	
	ret = rfile_xdelete(pathname) ;
	
	if(ret == 0)
	{
		return 0;
	}
	else 
	{
		return -1;
	}
}

/*****************************************************************************
被测函数  :  rfile_xdelete
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
int test_rfile_xdelete()
{
	int ret;
	char *pathname = "/yaffs0";
	
	ret = rfile_xdelete(pathname) ;
}

int test_fp_list_add_file()
{
	void* buf = 0x12345;
	void* ret;
	int buf_ret;
	INIT_LIST_HEAD(&(fp_list_file.list));
	buf_ret = fp_list_add_file(buf);
	if(buf_ret == 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int test_fp_list_add_file_same()
{
	void* buf = 0x12345;
	void* ret;
	int buf_ret1,buf_ret2;
	INIT_LIST_HEAD(&(fp_list_file.list));
	buf_ret1 = fp_list_add_file(buf);
	buf_ret2 = fp_list_add_file(buf);
	if(buf_ret1 == 0x12345 && buf_ret2 == 0x12346)
	{
		return 0;

	}
	else
	{
		return -1;
	}
}

int test_fp_list_check_file_fail()
{
	void* buf = 0x12345;
	void* ret;
	int buf_ret;
	INIT_LIST_HEAD(&(fp_list_file.list));
	buf_ret = fp_list_add_file(buf);
	ret = fp_check_file(0x12346);
	if(ret == buf)
	{
		return 0;

	}
	else
	{
		return -1;
	}
}

int test_fp_list_check_file_success()
{
	void* buf = 0x12345;
	void* ret;
	int buf_ret;
	INIT_LIST_HEAD(&(fp_list_file.list));
	buf_ret = fp_list_add_file(buf);
	ret = fp_check_file(buf_ret);
	if(ret == buf)
	{
		return 0;

	}
	else
	{
		return -1;
	}
}

int test_fp_list_check_file()
{
	void* buf = 0x12345;
	void* ret;
	int buf_ret = 0x12345;
	fp_list_t t;
	t.fp_v = (void*)0x12345;
	t.fp_hashed = 0x12345;
	
	INIT_LIST_HEAD(&(fp_list_file.list));
	list_add(&(t.list),&(fp_list_file.list));
//	fp_list_file.
	ret = fp_check_file(buf_ret);
	if(ret == buf)
	{
		return 0;

	}
	else
	{
		return -1;
	}
}

int test_fp_list_delete_file_fail()
{
	void* buf = 0x12345;
	void* ret;
	int buf_ret = 0x12345;
	fp_list_t t;
	t.fp_v = (void*)0x12345;
	t.fp_hashed = 0x12345;
	
	INIT_LIST_HEAD(&(fp_list_file.list));
	list_add(&(t.list),&(fp_list_file.list));
	fp_list_del_file(0x12346);
	ret = fp_check_file(buf_ret);
	
	if(ret == 0)
	{
		return -1;
	}
	else 
	{
		return 0;
	}
}

int test_fp_list_delete_file_success()
{
	void* buf = 0x12345;
	void* ret;
	int buf_ret = 0x12345;
	fp_list_t t;
	t.fp_v = (void*)0x12345;
	t.fp_hashed = 0x12345;
	
	INIT_LIST_HEAD(&(fp_list_file.list));
	list_add(&(t.list),&(fp_list_file.list));
	fp_list_del_file(0x12345);
	ret = fp_check_file(buf_ret);
	
	if(ret == 0)
	{
		return 0;
	}
	else 
	{
		return -1;
	}
}

int test_fp_list_add_dir()
{
	void* buf = 0x12345;
	void* ret;
	int buf_ret;
	INIT_LIST_HEAD(&(fp_list_dir.list));
	buf_ret = fp_list_add_dir(buf);
	if(buf_ret == 0)
	{
		return -1;

	}
	else
	{
		return 0;
	}
}

int test_fp_list_add_dir_same()
{
	void* buf = 0x12345;
	void* ret;
	int buf_ret1,buf_ret2;
	INIT_LIST_HEAD(&(fp_list_dir.list));
	buf_ret1 = fp_list_add_dir(buf);
	buf_ret2 = fp_list_add_dir(buf);
	if(buf_ret1 == 0x12345 && buf_ret2 == 0x12346)
	{
		return 0;

	}
	else
	{
		return -1;
	}
}

int test_fp_list_check_dir_fail()
{
	void* buf = 0x12345;
	void* ret;
	int buf_ret;
	INIT_LIST_HEAD(&(fp_list_dir.list));
	buf_ret = fp_list_add_dir(buf);
	ret = fp_check_dir(0x12346);
	if(ret == buf)
	{
		return 0;

	}
	else
	{
		return -1;
	}
}

int test_fp_list_check_dir_success()
{
	void* buf = 0x12345;
	void* ret;
	int buf_ret;
	INIT_LIST_HEAD(&(fp_list_dir.list));
	buf_ret = fp_list_add_dir(buf);
	ret = fp_check_dir(buf_ret);
	if(ret == buf)
	{
		return 0;

	}
	else
	{
		return -1;
	}
}

int test_fp_list_check_dir()
{
	void* buf = 0x12345;
	void* ret;
	int buf_ret = 0x12345;
	fp_list_t t;
	t.fp_v = (void*)0x12345;
	t.fp_hashed = 0x12345;
	
	INIT_LIST_HEAD(&(fp_list_dir.list));
	list_add(&(t.list),&(fp_list_dir.list));
	ret = fp_check_dir(buf_ret);
	if(ret == buf)
	{
		return 0;

	}
	else
	{
		return -1;
	}
}

int test_fp_list_delete_dir_fail()
{
	void* buf = 0x12345;
	void* ret;
	int buf_ret = 0x12345;
	fp_list_t t;
	t.fp_v = (void*)0x12345;
	t.fp_hashed = 0x12345;
	
	INIT_LIST_HEAD(&(fp_list_dir.list));
	list_add(&(t.list),&(fp_list_dir.list));
	fp_list_del_dir(0x12346);
	ret = fp_check_dir(buf_ret);
	
	if(ret == 0)
	{
		return -1;
	}
	else 
	{
		return 0;
	}
}

int test_fp_list_delete_dir_success()
{
	void* buf = 0x12345;
	void* ret;
	int buf_ret = 0x12345;
	fp_list_t t;
	t.fp_v = (void*)0x12345;
	t.fp_hashed = 0x12345;
	
	INIT_LIST_HEAD(&(fp_list_dir.list));
	list_add(&(t.list),&(fp_list_dir.list));
	fp_list_del_dir(0x12345);
	ret = fp_check_dir(buf_ret);
	
	if(ret == 0)
	{
		return 0;
	}
	else 
	{
		return -1;
	}
}

/*****************************************************************************
被测函数  :  bsp_ccpu_rest_rfile_handle
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
int test_bsp_ccpu_rest_rfile_handle()
{
	fp_list_t t1;
	FILE f;
	t1.fp_v = (void*)&f;
	t1.fp_hashed = 0x12345;
	fp_list_t t2;
	t2.fp_v = (void*)&f;
	t2.fp_hashed = 0x12345;
	f._file = 0;
	
	int ret;
	
	INIT_LIST_HEAD(&(fp_list_file.list));
	INIT_LIST_HEAD(&(fp_list_dir.list));
	list_add(&(t1.list),&(fp_list_file.list));
	list_add(&(t2.list),&(fp_list_dir.list));
	
	ret = bsp_ccpu_rest_rfile_handle();
	
	if(ret == 0)
	{
		return 0;
	}
	else 
	{
		return -1;
	}
}

/*****************************************************************************
被测函数  :  fmassreadProxy
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
int test_fmassreadProxy_wrong_pkg( void )
{
     int      ret   = 0;
     int  opstype   = 0;
     RFILE     fp    = {0};

     char *  data = NULL;

     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = 0;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_MASSREAD_HEAD));

     memset(data,0,sizeof(RFILE_MASSREAD_HEAD));

     ret = fmassreadProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}


int test_fmassreadProxy_one_frame_success( void )
{
	int      ret   = 0;
     int  opstype   = FMASS_READ;
     int      dir   = 0;

     char *  data = NULL;
	 char path[] = "/dev/block/mmcblk0";
     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FMASS_READ;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_MASSREAD_HEAD)+sizeof(path));

     memset(data,0,sizeof(RFILE_MASSREAD_HEAD)+sizeof(path));

     /*opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));
	 memcpy(data+sizeof(RFILE_MASSREAD_HEAD),path,sizeof(path));
     /*dir*/
     ((RFILE_MASSREAD_HEAD*)data)->partition_name_len   = sizeof(path);
	 ((RFILE_MASSREAD_HEAD*)data)->offset   = 3;
	 ((RFILE_MASSREAD_HEAD*)data)->length   = 20;
     rfile_linux_ops.bsp_mass_read = bsp_mass_read;

     ret = fmassreadProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}

int test_fmassreadProxy_two_frame_success( void )
{
	int      ret   = 0;
     int  opstype   = FMASS_READ;
     int      dir   = 0;

     char *  data = NULL;
	 char path[] = "/dev/block/mmcblk0";
     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FMASS_READ;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_MASSREAD_HEAD)+sizeof(path));

     memset(data,0,sizeof(RFILE_MASSREAD_HEAD)+sizeof(path));

     /*opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));
	 memcpy(data+sizeof(RFILE_MASSREAD_HEAD),path,sizeof(path));
     /*dir*/
     ((RFILE_MASSREAD_HEAD*)data)->partition_name_len   = sizeof(path);
	 ((RFILE_MASSREAD_HEAD*)data)->offset   = 3;
	 ((RFILE_MASSREAD_HEAD*)data)->length   = 7*1024;
     rfile_linux_ops.bsp_mass_read = bsp_mass_read;

     ret = fmassreadProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}

/*****************************************************************************
被测函数  :  fmasswriteProxy
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
int test_fmasswriteProxy_wrong_pkg( void )
{
	int      ret   = 0;
     int  opstype   = FMASS_WRITE;
     int      dir   = 0;

     char *  data = NULL;
	 char path[] = "/dev/block/mmcblk0";
     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FMASS_WRITE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 2;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_MASSWRITE_HEAD)+sizeof(path));

     memset(data,0,sizeof(RFILE_MASSWRITE_HEAD)+sizeof(path));

     /*opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));
	 memcpy(data+sizeof(RFILE_MASSWRITE_HEAD),path,sizeof(path));
     /*dir*/
     ((RFILE_MASSWRITE_HEAD*)data)->partition_name_len   = sizeof(path);
	 ((RFILE_MASSWRITE_HEAD*)data)->offset   = 3;
	 ((RFILE_MASSWRITE_HEAD*)data)->size   = 20;
     rfile_linux_ops.bsp_mass_write = bsp_mass_write;

     ret = fmasswriteProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}


/*****************************************************************************
被测函数  :  fmasswriteProxy
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
int test_fmasswriteProxy_one_frame_success( void )
{
	int      ret   = 0;
     int  opstype   = FMASS_WRITE;
     int      dir   = 0;

     char *  data = NULL;
	 char path[] = "/dev/block/mmcblk0";
     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FMASS_WRITE;
     mnt_h.totalFrame = 1;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_MASSWRITE_HEAD)+sizeof(path));

     memset(data,0,sizeof(RFILE_MASSWRITE_HEAD)+sizeof(path));

     /*opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));
	 memcpy(data+sizeof(RFILE_MASSWRITE_HEAD),path,sizeof(path));
     /*dir*/
     ((RFILE_MASSWRITE_HEAD*)data)->partition_name_len   = sizeof(path);
	 ((RFILE_MASSWRITE_HEAD*)data)->offset   = 3;
	 ((RFILE_MASSWRITE_HEAD*)data)->size   = 20;
     rfile_linux_ops.bsp_mass_write = bsp_mass_write;
	g_mass_wt_expect_cur = 0;
	
     ret = fmasswriteProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}

/*****************************************************************************
被测函数  :  fmasswriteProxy
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
int test_fmasswriteProxy_two_frame_success( void )
{
	int      ret   = 0;
     int  opstype   = FMASS_WRITE;
     int      dir   = 0;

     char *  data = NULL;
	 char path[] = "/dev/block/mmcblk0";
     RFILE_MNTN_HEADER mnt_h = {0};

     mnt_h.opsPid     = FMASS_WRITE;
     mnt_h.totalFrame = 2;
     mnt_h.curFrame   = 1;
     mnt_h.curlen     = 0;

     /*拷贝数据*/
     data = (char*)malloc(sizeof(RFILE_MASSWRITE_HEAD)+sizeof(path));

     memset(data,0,sizeof(RFILE_MASSWRITE_HEAD)+sizeof(path));

     /*opstype*/
     memcpy(data,&opstype,sizeof(unsigned int));
	 memcpy(data+sizeof(RFILE_MASSWRITE_HEAD),path,sizeof(path));
     /*dir*/
     ((RFILE_MASSWRITE_HEAD*)data)->partition_name_len   = sizeof(path);
	 ((RFILE_MASSWRITE_HEAD*)data)->offset   = 3;
	 ((RFILE_MASSWRITE_HEAD*)data)->size   = 7*1024;
     rfile_linux_ops.bsp_mass_write = bsp_mass_write;
	 g_mass_wt_expect_cur = 0;
	 
     ret = fmasswriteProxy(&mnt_h,data);
     if(-1 == ret)
     {
    	 free(data);
         return -1;
     }
     else
     {
    	 free(data);
         return 0;
     }
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

