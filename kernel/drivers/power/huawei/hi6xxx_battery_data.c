#include <linux/string.h>
#include "hisi_battery_data.h"
#include "auto-generate/hisi_battery_data_array.c"


#define BAT_DATA_ARRY_ALL_SIZE (sizeof(bat_data_array_all)/sizeof(bat_data_array_all[0]))


struct hisi_smartstar_coul_battery_data *get_battery_data(unsigned int id_voltage, unsigned int product_index)
{
    int i = 0;    
    p_bat_data_array pa = 0;

    if ( product_index >= BAT_DATA_ARRY_ALL_SIZE) {
        return NULL;
    }
    /*lint -e838*/
    pa = bat_data_array_all[product_index];
	
    /* try to find battery data according to voltage */
    for (i = 1; (*pa)[i] != 0; i++) {
        if ((id_voltage >= (*pa)[i]->id_voltage_min)
            && (id_voltage <= (*pa)[i]->id_voltage_max)) {
            break;
        }
    }
    /*lint +e838*/
    /* if not found, return default data */
    if (0 == (*pa)[i]) {
        return (*pa)[0];
    }
    else {
        return (*pa)[i];
    }
}


