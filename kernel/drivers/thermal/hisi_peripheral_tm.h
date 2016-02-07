#ifndef __HISI_PERIPHERAL_TM_H
#define __HISI_PERIPHERAL_TM_H
#include <linux/thermal.h>

struct periph_tsens_tm_device_sensor {
	struct thermal_zone_device		*tz_dev;
	enum thermal_device_mode		mode;
	unsigned int			sensor_num;
	struct work_struct			work;
	const char 			*ntc_name;
	int				chanel;
	int				state;
};

struct hisi_peripheral_tm_chip {
	struct platform_device		*pdev;
	struct device			*dev;
	struct work_struct			tsens_work;
	struct delayed_work 		tsens_periph_tm_work;
	int				average_period;
	uint32_t				tsens_num_sensor;
	struct periph_tsens_tm_device_sensor sensor[0];
};

#endif
