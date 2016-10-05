#ifndef __CPU_H
#define __CPU_H

#include <gtk/gtk.h>
#include <linux/version.h>

#define MAX_FREQ_STR_LEN        16
#define CPU_SCALE               128

struct cpu_core {
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
};

struct cpu {
	int core_num;
	struct cpu_core total;
	struct cpu_core *cpu_cores;
};


extern int init_cpu(struct cpu *cpu);
extern int update_cpu(struct cpu *cpu);
extern void free_cpu(struct cpu *cpu);

#endif
