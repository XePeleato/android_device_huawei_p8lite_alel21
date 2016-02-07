#ifndef   MNTN_WATCHPOINT_TEST_H
#define   MNTN_WATCHPOINT_TEST_H


extern char *wp_tst_get_mem(unsigned int type, unsigned int osize, unsigned int *psize);
extern void wp_tst_free_mem(char *mem_to_free, int type);
extern void wp_tst_write_data(unsigned int tst_addr, unsigned int val);
extern unsigned int wp_tst_read_data(unsigned int tst_addr);

#endif
