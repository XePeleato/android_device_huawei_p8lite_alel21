#ifndef _PARTITION_H_
#define _PARTITION_H_

#include "hisi_partition.h"

struct partition {
	const char *name;
	unsigned size_kb;
};

#ifdef CONFIG_MACH_K3V2OEM1
static struct partition partitions[] = {
	{ "-",              128 },	/* ptable       128K */
	{ "xloader",        256 },	/* xloader      256K */
	{ "round",         3712 },	/* round 4M    3712K */
	{ "nvme",        4*1024 },	/* NV             4M */
	{ "misc",        4*1024 },	/* misc           4M */
	{ "splash",      8*1024 },	/* apanic         8M */
	{ "oeminfo",    32*1024 },	/* oeminfo       32M */
	{ "reserved1",   8*1024 },	/* reserved1      8M */
	{ "reserved2",   8*1024 },	/* reserved2      8M */
	{ "recovery2",   8*1024 },	/* recovery2      8M */
	{ "recovery",    8*1024 },	/* recovery       8M */
	{ "boot",        8*1024 },	/* boot           8M */
	{ "modemimage", 32*1024 },	/* modemimage    32M */
	{ "modemnvm1",   8*1024 },	/* modemnvm1      8M */
	{ "modemnvm2",   8*1024 },	/* modemnvm2      8M */
	{ "system",    512*1024 },	/* system       512M */
	{ "cache",     256*1024 },	/* cache        256M */
	{ "cust",      256*1024 },	/* cust         256M */
	{ "userdata", 6292*1024 },	/* userdata    6292M */
	{ 0, 0 },
};
#elif defined CONFIG_MACH_HI6620SFT
/* !!! warning !!! when partitions changed,please change  vendor\hisi\build\scripts\gen_img_vrl_tbl.py  accordingly */
static struct partition partitions[] = {
	{ "-", 512 }, /* 0 ptable 512K */
	{ "mcuimage", 256 }, /* 1 mcuimage 256K */
	{ "teeos", 512 }, /* 2 teeos 512K */
	{ "log", 256 }, /* 3 log 256K */
	{ "round", 512 }, /* 4 round 2M 512K */
	/*fix me, when move fastboot2 from boot partition to here */
	{ "fastboot2", 2*1024 }, /* 5 fastboot2 2M */
	{ "misc", 2*1024 }, /* 6 misc 4M */
	{ "modemnvbkup", 2*1024 }, /* 7 modemnvbkup 2M */
	{ "nvme", 4*1024 }, /* 8 NV 4M */
	{ "oeminfo", 32*1024 }, /* 9 oeminfo 32M */
	{ "splash", 8*1024 }, /* 10 android use 8M */
	{ "modemnvm1", 8*1024 }, /* 11 modemnvm1 8M */
	{ "modemnvm2", 8*1024 }, /* 12 modemnvm2 8M */
	{ "modemimage", 64*1024 },/* 13 modemimage 64M */
	{ "dsp", 8*1024 }, /* 14 bbe16 8M */
	{ "hifi", 4*1024 }, /* 15 hifi 2M */
	{ "modemlog", 16*1024 }, /* 16 modemlog 16M */
	{ "boot", 16*1024 }, /* 17 boot 16M */
	{ "recovery", 16*1024 }, /* 18 recovery 16M */
	{ "recovery2", 16*1024 }, /* 19 recovery2 16M */
	{ "splash2", 64*1024 }, /* 20 recovery log 64M */
	{ "cache", 256*1024 }, /* 21 cache 256M */
	{ "3rdmodem", 128*1024 }, /* 22 3rdmodem/dump 128M */
	{ "hisitest0", 2*1024 }, /* 23 reserved0 2M */
	{ "hisitest1", 2*1024 }, /* 24 reserved1 2M */
	{ "hisitest2", 4*1024 }, /* 25 reserved2 4M */
	{ "system", 1536*1024 }, /* 26 system 1536M */
	{ "cust", 512*1024 }, /* 27 customer 512M */
	{ "userdata", 4920*1024 }, /* 28 userdata 4920M */
	{ 0, 0 }, /* total 7632M*/
};

#elif defined CONFIG_MACH_HI3635FPGA

static struct partition partitions[] = {
	{"-",			128		},	/* ptable		128K */
	{"vrl",			128		},	/* VRL			128K p1*/
	{"vrl_backup",		128		},	/* VRL_backup		128K p2*/
	{"reserved1",		1664		},	/* reserved1		1664K p3*/
	{"fastboot",		1792		},	/* fastboot		1792K p4*/
	{"fw_lpm3",		256		},	/* fw_lpm3		256K p5*/
	{"nvme",		4*1024		},	/* nvme			4M p6*/
	{"modemnvm_factory",	4*1024		},	/* modemnvm_factory	4M p7*/
	{"hibench",		4*1024		},	/* fw_iom3		4M p8*/
	{"fw_hifi",		2*1024		},	/* fw_hifi		2M p9*/
	{"teeos",		2*1024		},	/* teeos		2M p10*/
	{"misc",		4*1024		},	/* misc			4M p11*/
	{"reserved2",		2*1024		},	/* reserved2		2M p12*/
	{"boot",		14*1024		},	/* boot			14M p13*/
	{"recovery",		8*1024		},	/* recovery 		8M p14*/
	{"recovery2",		8*1024		},	/* recovery2		8M p15*/
	{"oeminfo",		32*1024		},	/* oeminfo		32M p16*/
	{"sensorhub",		32*1024		},	/* sensorhub		32M p17*/
	{"splash",		8*1024		},	/* splash		8M p18*/
	{"splash3",		8*1024		},	/* splash3		8M p19*/
	{"dts",			32*1024		},	/* dts			32M p20*/
	{"secure_storage",	20*1024		},	/* securestorage	20M p21*/
	{"dfx",			8*1024		},	/* dfx			8M p22*/
	{"modemnvm_backup",	4*1024		},	/* modemnvm_backup	4M p23*/
	{"modemnvm_img",	8*1024		},	/* modemnvm_img		8M p24*/
	{"modemnvm_system",	4*1024		},	/* modemnvm system	4M p25*/
	{"modem",		60*1024		},	/* modemimage		60M p26*/
	{"modem_dsp",		4*1024		},	/* modem_DSP		4M p27*/
	{"modem_om",		12*1024		},	/* modem_om		12M p28*/
	{"modemnvm_update",	24*1024		},	/* modemnvm_update	24M p29*/
	{"3rdmodem",		128*1024	},	/* 3rdmodem		128M p30*/
	{"3rdmodemnvm",		32*1024		},	/* 3rdmodemnvm		32M p31*/
	{"3rdmodemnvmbkp",	16*1024		},	/* 3rdmodemnvmbkp	16M p32*/
	{"splash2",		64*1024		},	/* splash2		64M p33*/
	{"hisitest0",		2*1024		},	/* hisitest0		2M p34*/
	{"hisitest1",		2*1024		},	/* hisitest1		2M p35*/
	{"cust",		512*1024	},	/* cust			512M p36*/

#if ((defined EMULATOR_3635) || (defined EMULATOR_3650))
	{"system",		512*1024	},	/* system		512M p37*/
	{"cache",		1280*1024	},	/* cache		1280M p38*/
	{"userdata",		128*1024	},	/* userdata		128M p39*/
#else
	{"system",		1536*1024	},	/* system		1536M p37*/
	{"cache",		256*1024	},	/* cache		256M p38*/
	{"userdata",		512*1024	},	/* userdata		512M p39*/
#endif

	{ 0, 0 },

};
#elif defined CONFIG_MACH_HI3635

static struct partition partitions[] = {
        {"-",                       512 },                /* ptable               512K */
        {"vrl",                     256 },                /* VRL                  256K p1*/
        {"vrl_backup",              256 },                /* VRL backup           256K p2*/
        {"mcuimage",                256 },                /* mcuimage             256K p3*/
        {"reserved0",               768 },                /* reserved0            768K p4*/
        {"fastboot",                4*1024 },             /* fastboot             4M   p5*/
        {"modemnvm_factory",        4*1024 },             /* modemnvm_factory     4M   p6*/
        {"nvme",                    6*1024 },             /* nvme                 6M   p7*/
        {"oeminfo",                 32*1024},             /* oeminfo              32M  p8*/
        {"splash",                  8*1024 },             /* splash               8M   p9*/
        {"modemnvm_backup",         4*1024 },             /* modemnvm_backup      4M   p10*/
        {"modemnvm_img",            8*1024 },             /* modemnvm_img         8M   p11*/
        {"modemnvm_system",         4*1024 },             /* modemnvm_system      4M   p12*/
        {"securetystorage",         32*1024},             /* securetystorage      32M  p13*/
        {"3rdmodemnvm",             16*1024},             /* 3rdmodemnvm          16M  p14*/
        {"3rdmodemnvmback",         16*1024},             /* 3rdmodemnvmback      16M  p15*/
        {"reserved1",               12*1024},             /* reserved1            12M  p16*/
        {"modem_om",                12*1024},             /* modem_om             12M  p17*/
        {"splash2",                 68*1024},             /* splash2              68M  p18*/
        {"misc",                    2*1024 },             /* misc                 2M   p19*/
        {"modemnvm_update",         24*1024},             /* modemnvm_update      24M  p20*/
        {"recovery2",               64*1024},             /* recovery2            64M  p21*/
        {"reserved2",               64*1024},             /* reserved2            64M  p22*/
        {"teeos",                   4*1024},              /* teeos                4M   p23*/
        {"trustfirmware",           2*1024},              /* trustfirmware        2M   p24*/
        {"sensorhub",               16*1024},             /* sensorhub            16M  p25*/
        {"hifi",                    12*1024},             /* hifi                 12M  p26*/
        {"boot",                    24*1024},             /* boot                 24M  p27*/
        {"recovery",                32*1024},             /* recovery             32M  p28*/
        {"dtimage",                 32*1024},             /* dtimage              32M  p29*/
        {"modem",                   64*1024},             /* modem                64M  p30*/
        {"modem_dsp",               8*1024 },             /* modem_dsp            8M   p31*/
        {"dfx",                     16*1024},             /* dfx                  16M  p32*/
        {"3rdmodem",                64*1024},             /* 3rdmodem             64M  p33*/
        {"cache",                   256*1024},            /* cache                256M p34*/
        {"hisitest0",               2*1024 },             /* hisitest0            2M   p35*/
        {"hisitest1",               2*1024 },             /* hisitest1            2M   p36*/
        {"hisitest2",               4*1024 },             /* hisitest2            4M   p37*/
        {"system",                  2560*1024},           /* system               2560 p38*/
        {"cust",                    512*1024},            /* cust                 512M p39*/
        {"userdata",                4096*1024},           /* userdata             4096M p40*/
        {0,0},                              		  /* total 7544M*/
};
#else /*CONFIG_MACH_HI6620OEM*/
/*for v8r1*/
/* !!! warning !!! when partitions changed,please change  vendor\hisi\build\scripts\gen_img_vrl_tbl.py  accordingly */
static struct partition partitions[] = {    	
        {"-",                   512 },        /* 0  ptable            512K   */	
        {PART_VRL,              256 },        /* 1  vrl               256K   */
        {PART_VRL_BKUP,         256 },        /* 2  vrl_backup        256K   */
        {PART_MCUIMAGE,         256 },        /* 3  mcuimage          256K   */
        {PART_RESERVED0,        768 },        /* 4  reserved0         768K   */
        {PART_FASTBOOT,         4*1024 },     /* 5  fastboot2         4M     */
        {PART_MODEMNVBKUP,      6*1024 },     /* 6  modemnvbkup       6M     */
        {PART_NVME,             4*1024 },     /* 7  nvme              4M     */
        {PART_OEMINFO,          32*1024 },    /* 8  oeminfo           32M    */
        {PART_SPLASH,           8*1024 },     /* 9  splash            8M     */
        {PART_MODEMNVM1,        16*1024 },    /* 10 modemnvm1         16M    */
        {PART_MODEMNVM2,        16*1024 },    /* 11 modemnvm2         16M    */
        {PART_MODEMNVM3,        16*1024 },    /* 12 modemnvm3         16M    */
        {PART_SECURETYSTORAGE,  32*1024 },    /* 13 securetystorage   32M    */
        {PART_3RDMODEMNVM,      16*1024 },    /* 14 3rdmodemnvm       16M    */
        {PART_3RDMODEMNVMBACK,  16*1024 },    /* 15 3rdmodemnvb ack   16M    */
        {PART_RESERVED1,        12*1024 },    /* 16 reserved1         12M    */
        {PART_MODEMLOG,         16*1024 },    /* 17 modemlog          16M    */
        {PART_SPLASH2,          64*1024 },    /* 18 splash2           64M    */
        {PART_MISC,             2*1024 },     /* 19 misc              2M     */
        {PART_MODEMNVRESERVED,  24*1024},     /* 20 modemnvreserved   24M    */
        {PART_RECOVERY2,        64*1024},     /* 21 recovery2         64M    */
        {PART_RESERVED2,        32*1024 },    /* 22 reserved2         64M    */
        {PART_TEEOS,            4*1024 },     /* 24 teeos             4M     */
        {PART_TRUSTFIRMWARE,    2*1024 },     /* 25 trustfirmware     2M     */
        {PART_SENSORHUB,        16*1024 },    /* 26 sensorhub         16M    */
        {PART_HIFI,             12*1024 },    /* 27 hifi              12M    */
        {PART_BOOT,             24*1024 },    /* 28 boot              24M    */
        {PART_RECOVERY,         64*1024 },    /* 29 recovery          32M    */
        {PART_DTIMAGE,          32*1024 },    /* 30 dtimage           32M    */
        {PART_MODEMIMAGE,       64*1024 },    /* 31 modemimage        64M    */
        {PART_DSP,              8*1024 },     /* 32 dsp               8M     */
        {PART_DFX,              16*1024 },    /* 33 dfx               16M    */
        {PART_3RDMODEM,         4*1024 },    /* 34 3rdmodem          16M    */
        {PART_CACHE,            256*1024 },   /* 35 cache             256M   */
        {PART_HISITEST0,        2*1024 },     /* 36 hisitest0         2M     */
        {PART_HISITEST1,        2*1024 },     /* 37 hisitest1         2M     */
        {PART_HISITEST2,        4*1024 },     /* 38 hisitest2         4M     */
#ifdef CONFIG_MARKET_OVERSEA
        {PART_SYSTEM,           2432*1024 },  /* 33 system            2432M  */
        {PART_CUST,             256*1024 },   /* 34 cust              256M   */
        {PART_USERDATA,         2560*1024 },  /* 35 usrdata           2560M  */
#elif defined CONFIG_MARKET_INTERNAL
        {PART_SYSTEM,           1792*1024 },  /* 33 system            1792M  */
        {PART_CUST,             512*1024 },   /* 34 cust              512M   */
        {PART_USERDATA,         2560*1024 },  /* 35 usrdata           2560M  */
#else
        {PART_SYSTEM,           2560*1024 },  /* 33 system            2560M  */
        {PART_CUST,             512*1024 },   /* 34 cust              512M   */
        {PART_USERDATA,         2560*1024 },  /* 35 usrdata           2560M  */
#endif
        {0,0}                              /* total 7168M,(7544M if usrdata == 4096) ,*/ 
};


#endif

#endif

