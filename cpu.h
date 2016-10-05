#ifndef __CPU_H
#define __CPU_H

#include <gtk/gtk.h>
#include <linux/version.h>

#define MAX_FREQ_STR_LEN        16
#define CPU_SCALE               128

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
	gulong total;
	gulong used;
	gulong load;
	char   freq[MAX_FREQ_STR_LEN];
} cpu_core_t;

typedef struct {
	int core_num;
	cpu_core_t total;
	cpu_core_t *cpu_cores;
} cpu_t;


extern int init_cpu(cpu_t *cpu);
extern int update_cpu(cpu_t *cpu);
extern void free_cpu(cpu_t *cpu);

#endif
