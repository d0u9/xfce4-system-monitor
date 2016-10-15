#ifndef __SENSOR_H
#define __SENSOR_H

#include "trilib/list.h"

#define MAX_CPU_LABEL_LEN	32

struct core_s {
	struct list_head list;
	int  index;
	int  input;
	int  crit;
	int  max;
	int  crit_alarm;
	char label[MAX_CPU_LABEL_LEN];
};

struct cpu_s {
	struct list_head list;
	struct list_head core_list;
	struct list_head *cur_core;
	int    index;
};

struct sensor {
	struct list_head cpu_list;
	struct list_head *cur_cpu;
};


extern int init_sensor(struct sensor *sensor);
extern int update_sensor(struct sensor *sensor);
extern void free_sensor(struct sensor *sensor);
extern
struct core_s *max_temp_core(struct list_head *cpu_list, struct core_s *ret);

#endif
