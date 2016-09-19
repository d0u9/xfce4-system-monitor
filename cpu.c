#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "sysmonitor.h"
#include "cpu.h"


int init_cpu(cpu_t *cpu);
int update_cpu(cpu_t *cpu);
void free_cpu(cpu_t *cpu);


/* static function decalartion */
static int calculate_cpu_usage(cpu_core_t *previous, cpu_core_t *core);
static int get_cpu_usage(cpu_t *cpu);
static int get_cpu_info(cpu_t *cpu);
static int get_cpu_core_num(void);

/* macro definitation */
#define PROC_STAT       "/proc/stat"
#define PROC_CPUINFO    "/proc/cpuinfo"


int update_cpu(cpu_t *cpu)
{
        int ret1 = -1, ret2 = -1;
ret1 = get_cpu_usage(cpu); ret2 = get_cpu_info(cpu);

        if (ret1 < 0 || ret2 < 0)
                return -1;

        return 0;
}


int init_cpu(cpu_t *cpu)
{
        int core_num = 0;
        cpu_core_t *cpu_cores = NULL;

        if ((core_num = get_cpu_core_num()) < 1)
                return -1;

        cpu->core_num = core_num;

        if (!(cpu_cores = calloc(core_num, sizeof(cpu_core_t)))) {
                return -1;
        }
        cpu->cpu_cores = cpu_cores;

        return 0;
}


void free_cpu(cpu_t *cpu)
{
        free(cpu->cpu_cores);
}


static int get_cpu_usage(cpu_t *cpu)
{
        cpu_core_t      usage;
        FILE *stat_file = NULL;
        char line[MAX_FILE_LINE_LEN] = {0};

        if (!(stat_file = fopen(PROC_STAT, "r"))) {
                return -1;
        }

        int line_no = 0;
        while (fgets(line, MAX_FILE_LINE_LEN, stat_file)) {
                if (strncmp(line, "cpu", 3 ) == 0) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
                        sscanf(line, "%*s %lu %lu %lu %lu %lu %lu %lu, %lu, %lu, %lu",
                               &usage.user, &usage.nice, &usage.system, &usage.idle,
                               &usage.iowait, &usage.irq, &usage.softirq,
                               &usage.steal, &usage.guest, &usage.guest_nice);
#else
                        sscanf(line, "%*s %lu %lu %lu %lu %lu %lu %lu",
                               &usage.user, &usage.nice, &usage.system,
                               &usage.idle, &usage.iowait,
                               &usage.irq, &usage.softirq);
#endif
                        if (line_no == 0) {
                                cpu_core_t previous = cpu->total;
                                cpu->total = usage;
                                if (calculate_cpu_usage(&previous, &cpu->total) < 0)
                                        goto error;
                        }
                        else {
                                cpu_core_t previous = cpu->cpu_cores[line_no - 1];
                                cpu->cpu_cores[line_no - 1]  = usage;
                                if (calculate_cpu_usage(&previous,
                                                        &cpu->cpu_cores[line_no -1]) < 0)
                                        goto error;
                        }
                }

                line_no++;
        }

        if (!feof(stat_file))
                goto error;

        return 0;
error:
        fclose(stat_file);
        return -1;

}


static int get_cpu_info(cpu_t *cpu)
{
        cpu_core_t *core = NULL;
        FILE    *cpuinfo_file = NULL;
        char    line[MAX_FILE_LINE_LEN + 1] = {0};
        int     core_index = -1;

        if (!(cpuinfo_file = fopen(PROC_CPUINFO, "r")))
                return -1;

        while (fgets(line, MAX_FILE_LINE_LEN, cpuinfo_file)) {
                if (strncmp("processor", line, 9) == 0) {
                        sscanf(line, "%*s %*s %d", &core_index);
                        core = &(cpu->cpu_cores[core_index]);
                } else if (strncmp("cpu MHz", line, 7) == 0) {
                        memset(core->freq, 0, MAX_FREQ_STR_LEN);
                        sscanf(line, "%*s %*s %*s %s", core->freq);
                }
        }

        fclose(cpuinfo_file);

        if (!feof(cpuinfo_file))
                return -1;

        return 0;
}


static int get_cpu_core_num(void)
{
#ifdef _SC_NPROCESSORS_ONLN
        return (int)(sysconf(_SC_NPROCESSORS_ONLN));
#else
        FILE *stat_file = NULL;
        char line[MAX_FILE_LINE_LEN + 1] = {0};
        int  nb_lines = 0;

        if (!(stat_file = fopen(PROC_STAT, "r")))
                return -1;

        while (fgets(line, MAX_FILE_LINE_LEN, stat_file)) {
                if (strncmp(line, "cpu", 3 ) == 0)
                        nb_lines++;
                else
                        break;
        }

        fclose(stat_file);
        if (ferror(stat_file))
                return -1;

        return nb_lines > 1 ? nb_lines - 1 : -1;
#endif
}


static int calculate_cpu_usage(cpu_core_t *previous, cpu_core_t *core)
{
        gulong used = 0, total = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
        used = core->user + core->nice + core->system + core->irq + core->softirq +core->guest + core->guest_nice;
        total = used + core->idle + core->iowait + core->steal;
#else
        used = core->user + core->nice + core->system + core->irq + core->softirq;
        total = used + core->idle + core->iowait;
#endif

        if ((total - previous->total) != 0) {
                g_print("used = %lu, total = %lu\n", used - previous->used, total - previous->total);
                core->load = (used - previous->used) * 10000 / (total - previous->total);
        } else {
                core->load = 0;
        }

        core->total = total;
        core->used = used;

        return 0;
}
