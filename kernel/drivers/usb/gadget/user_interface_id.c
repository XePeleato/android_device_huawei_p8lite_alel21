#define ADB             0
#define RNDIS_CTRL      1
#define RNDIS_DATA      2
#define MTP             3
#define MASS_STORAGE    4
#define ACM0            5   /*ashell*/
#define ACM1            6   /*AT*/
#define ACM2            7   /*ctrl*/
#define ACM3            8   /*OM*/
#define ACM4            9   /*cshell*/
#define ACM5            10   /*HSO-OM*/
#define ACM6            11   /*HSO-GPS*/
#define MAX_ID          12   /*total id number*/
#define LAST_PORT       MAX_ID

#define MAX_PORT        (LAST_PORT+1) 

#define ADB_NAME            "adb"
#define RNDIS_CTRL_NAME     "rndis_ctrl"
#define RNDIS_DATA_NAME     "rndis_data"
#define MTP_NAME            "mtp"
#define MASS_STORAGE_NAME   "mass_storage"
#define ACM0_NAME           "gser0"   /*ashell*/
#define ACM1_NAME           "gser1"    /*AT*/
#define ACM2_NAME           "gser2"   /*ctrl*/
#define ACM3_NAME           "gser3"    /*OM*/
#define ACM4_NAME           "gser4"   /*cshell*/
#define ACM5_NAME           "gser5"    /*HSO-OM*/
#define ACM6_NAME           "gser6"   /*HSO-GPS*/
#define MAX_ID_NAME         NULL

#define NO_USE            0xff
#define MAX_NUM           5

struct f_map{
    char *name;
    int  port;
    unsigned char  num[MAX_NUM];
};
struct f_map balong_map[MAX_PORT] =
{
    {
        .name = ADB_NAME,
        .port = ADB,
    },
    {
        .name = RNDIS_CTRL_NAME,
        .port = RNDIS_CTRL,
    },
    {
        .name = RNDIS_DATA_NAME,
        .port = RNDIS_DATA,
    },
    {
        .name = MTP_NAME,
        .port = MTP,
    },
    {
        .name = MASS_STORAGE_NAME,
        .port = MASS_STORAGE,
    },
    {   /*ASHELL*/
        .name = ACM0_NAME,
        .port = ACM0,
    },
    {   /*AT*/
        .name = ACM1_NAME,
        .port = ACM1,
    },
    {   /*ctrl*/
        .name = ACM2_NAME,
        .port = ACM2,
    },
    {   /*OM*/
        .name = ACM3_NAME,
        .port = ACM3,
    },
    {   /*cshell*/
        .name = ACM4_NAME,
        .port = ACM4,
    },
    {   /*HSO-OM*/
        .name = ACM5_NAME,
        .port = ACM5,
    },
    {   /*HSO-GPS*/
        .name = ACM6_NAME,
        .port = ACM6,
    },
    {   /*MAX_ID*/
        .name = MAX_ID_NAME,
        .port = MAX_ID,
    }
};
int gser_num;
int rndis_num;
int port_num;

unsigned char port_map[MAX_NUM][MAX_PORT] =
{
    /*ADB*/ /*RNDIS_CTRL*/ /*RNDIS_DATA*/ /*MTP*/ /*MASS_STORAGE*/ /*ASHELL*/ /*AT*/ /*CTRL*/ /*OM*/ /*CSHELL*/ /*HSO-OM*/ /*HSO-GPS*/ /*MAX_ID*/
    {2,     0,             1,             NO_USE, NO_USE,              5,         3,     7,       4,     6,       8,         9,          10},
    {2,     0,             1,             NO_USE, NO_USE,          3,         5,     7,       6,     4,       8,         9,          10},
    {1,     NO_USE,        NO_USE,        0,      NO_USE,          2,         4,     6,       5,     3,       7,         8,          9},
    {3,     0,             1,             4,      5,               6,         2,     NO_USE,    NO_USE,NO_USE,  7}, /* test on testchip */
};

int user_interface_id(struct usb_function *f, int num)
{
    int i;
    char fname[20];
    printk(KERN_INFO"f-name:%s\n", f->name);
    if (!strcmp(f->name, "gser")) {
        snprintf(fname, 20, "%s%d", f->name, gser_num); 
        gser_num++;
    } else if (!strcmp(f->name, "rndis")) {
        if (rndis_num == 0) {
            snprintf(fname, 20, "%s%s", f->name, "_ctrl"); 
        } else {
            snprintf(fname, 20, "%s%s", f->name, "_data"); 
        }
        rndis_num++;
    } else {
        snprintf(fname, 20, "%s", f->name); 
    }
    printk(KERN_INFO"fname:%s\n", fname);
    for (i=0; i<MAX_PORT; i++) {
        if (!strcmp(fname, balong_map[i].name)) {
            printk(KERN_INFO"got i:%d port:%d id:%d\n", i,
                balong_map[i].port, port_map[num][balong_map[i].port]);
            return port_map[num][balong_map[i].port];
        }
    }
    return -1;
}
