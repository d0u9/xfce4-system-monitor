#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "sysmonitor.h"
#include "cpu.h"


int get_cpu_data(cpu_t * old_data);
int alloc_cpu_data(cpu_t *cpu);
void free_cpu_data(cpu_t *cpu);


/* static function decalartion */
static int get_cpu_usage(cpu_t *cpu);
static int get_cpu_info(cpu_t *cpu);
static int get_cpu_core_num(void);

/* macro definitation */
#define PROC_STAT       "/proc/stat"
#define PROC_CPUINFO    "/proc/cpuinfo"


int get_cpu_data(cpu_t * cpu)
{
        get_cpu_usage(cpu);
        get_cpu_info(cpu);

        return 0;
}

int alloc_cpu_data(cpu_t *cpu)
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


void free_cpu_data(cpu_t *cpu)
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
                                cpu->total = usage;
                        }
                        else {
                                *(cpu->cpu_cores + line_no - 1) = usage;
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
        //TODO: Add codes here to obtain info of CPU (e.g. frequence of CPU)

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
