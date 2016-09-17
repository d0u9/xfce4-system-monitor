#ifndef __CPU_H
#define __CPU_H

#include <gtk/gtk.h>
#include <linux/version.h>

typedef struct {
        gulong user;
        gulong nice;
        gulong system;
        gulong idle;
        gulong iowait;
        gulong irq;
        gulong softirq;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
        gulong steal;
        gulong guest;
        gulong guest_nice;
#endif
} cpu_core_t;

typedef struct {
        int     core_num;
        cpu_core_t      total;
        cpu_core_t      *cpu_cores;
} cpu_t;


extern int get_cpu_data(cpu_t *old_data);
extern int alloc_cpu_data(cpu_t *cpu);
extern void free_cpu_data(cpu_t *cpu);

#endif
