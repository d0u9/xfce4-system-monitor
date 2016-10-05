#ifndef __SENSOR_H
#define __SENSOR_H

#include "list.h"

#define MAX_CPU_LABEL_LEN	32

struct core_s {
	list_t  list;
	int  index;
	int  input;
	int  crit;
	int  max;
	int  crit_alarm;
	char label[MAX_CPU_LABEL_LEN];
};

struct cpu_s {
	list_t list;
	list_t core_list;
	list_t *cur_core;
	int    index;
};

struct sensor {
	list_t cpu_list;
	list_t *cur_cpu;
};


extern int init_sensor(struct sensor *sensor);
extern int update_sensor(struct sensor *sensor);
extern void free_sensor(struct sensor *sensor);
extern struct core_s *max_temp_core(list_t *cpu_list, struct core_s *ret);

#endif
