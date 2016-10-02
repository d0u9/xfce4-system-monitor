#ifndef __SENSOR_H
#define __SENSOR_H

#define MAX_CPU_LABEL_LEN       32

#include "list.h"

struct core_s {
        list_t  list;
        int     index;
        int     input;
        int     crit;
        int     max;
        int     crit_alarm;
        char    label[MAX_CPU_LABEL_LEN];
};
typedef struct core_s     core_s_t;

struct cpu_s {
        list_t  list;
        list_t  core_list;
        list_t  *cur_core;
        int     index;
};
typedef struct cpu_s      cpu_s_t;

typedef struct {
        list_t  cpu_list;
        list_t  *cur_cpu;
} sensor_t;


extern int init_sensor(sensor_t *sensor);
extern int update_sensor(sensor_t *sensor);
extern void free_sensor(sensor_t *sensor);
extern core_s_t *max_temp_core(list_t *cpu_list, core_s_t *ret);

#endif
