

#ifndef _AGENT_H_
#define _AGENT_H__

#include "teek_ns_client.h"

int agent_init(void);

int TC_NS_register_agent(TC_NS_DEV_File* dev_file, unsigned int agent_id,
			 TC_NS_Shared_MEM* shared_mem);
int TC_NS_unregister_agent(unsigned long agent_id);

unsigned int TC_NS_incomplete_proceed(TC_NS_SMC_CMD *smc_cmd,
				      unsigned int agent_id,
				      uint8_t flags);
int TC_NS_wait_event(unsigned long agent_id);
int TC_NS_send_event_reponse(unsigned long agent_id);
int TC_NS_alloc_exception_mem(unsigned int agent_id);
int TC_NS_store_exception_info(void);

int is_agent_alive(unsigned int agent_id);


#endif //_AGENT_H_
