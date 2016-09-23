#define _SVID_SOURCE

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include "sysmonitor.h"
#include "sensor.h"

#define SYS_HWMON       "/sys/class/hwmon"
#define MAX_DIR_DEPTH   3

#define reset_sensor_cursor(sensor)     sensor->cur_cpu = &sensor->cpu_list
#define reset_cpu_cursor(cpu)           cpu->cur_core = &cpu->core_list


int init_sensor(sensor_t *sensor);
int update_sensor(sensor_t *sensor);
void free_sensor(sensor_t *sensor);
core_s_t *max_temp_core(list_t *cpu_list, core_s_t *ret);


// axuiliary functions
static int get_first_line(const char *file, char *buf, int len);
static int filter_temp_file(const struct dirent *dir);
static int filter_valid_file(const struct dirent *dir);
static void free_namelist(struct dirent **namelist, int n);

// iterate over directories
static int parse_hwmon(const char *path, sensor_t *sensor);
static int is_valid_dir(const char *path);
static int process_valid_dir(const char *path, sensor_t *sensor);

// CPU
static int get_cpu_temp(const char *path, sensor_t *sensor);
static void free_cpu_s_list(list_t *cpu_list);
static int get_core_temp(const char *path, cpu_s_t *cpu);
static int parse_core_temp_dir(const char *file, core_s_t *core, const char *suffix);
static void free_core_s_list(list_t *core_list);

static int get_first_line(const char *file, char *buf, int len)
{
        FILE *fp = NULL;
        if ((fp = fopen(file, "r")) == NULL)
                return -1;
        fgets(buf, len, fp);
        fclose(fp);

        return 0;
}


static int filter_temp_file(const struct dirent *dir)
{
#if _BSD_SOURCE
        if (dir->d_type != DT_REG)
                return 0;
#endif
        if (strncmp("temp", dir->d_name, 4) == 0)
                return-1;

        return 0;
}


static int filter_valid_file(const struct dirent *dir)
{
#if _BSD_SOURCE
        if (dir->d_type != DT_DIR)
                return 0;
#endif
        if (strncmp("hwmon", dir->d_name, 5) == 0 ||
            strncmp("device", dir->d_name, 6) == 0)
                return -1;

        return 0;
}


static void free_namelist(struct dirent **namelist, int n)
{
        while (n-- > 0)
                free(namelist[n]);
        if (namelist)
                free(namelist);
}


static int is_valid_dir(const char *path)
{
        char str[MAX_PATH_STR_LEN] = {0};
        sprintf(str, "%s/name", path);
        int p = (access(str, F_OK) == 0 ? 1 : 0);
        return p;
}


static int parse_core_temp_dir(const char *file, core_s_t *core, const char *suffix)
{
        char str[MAX_PATH_STR_LEN] = {0};
        if (strncmp("crit_alarm", suffix, 10) == 0) {
                get_first_line(file, str, MAX_PATH_STR_LEN);
                core->crit_alarm = strtol(str, NULL, 0);
                goto ok;
        }

        if (strncmp("crit", suffix, 4) == 0) {
                get_first_line(file, str, MAX_PATH_STR_LEN);
                core->crit = strtol(str, NULL, 0);
                goto ok;
        }

        if (strncmp("input", suffix, 5) == 0) {
                get_first_line(file, str, MAX_PATH_STR_LEN);
                core->input = strtol(str, NULL, 0);
                goto ok;
        }

        if (strncmp("max", suffix, 3) == 0) {
                get_first_line(file, str, MAX_PATH_STR_LEN);
                core->max = strtol(str, NULL, 0);
                goto ok;
        }

        if (strncmp("label", suffix, 5) == 0) {
                get_first_line(file, core->label, MAX_PATH_STR_LEN);
        }

ok:
        return 0;
}


static int get_core_temp(const char *path, cpu_s_t *cpu)
{
        char str[MAX_PATH_STR_LEN] = {0};
        char str2[MAX_PATH_STR_LEN] = {0};
        char *file_suffix = str2;
        struct dirent **namelist = NULL;

        reset_cpu_cursor(cpu);
        int n = scandir(path, &namelist, filter_temp_file, alphasort);

        core_s_t *core = NULL;
        int current_index = -1;
        for (int i = 0; i < n; ++i) {
                int new_index = strtol(namelist[i]->d_name + 4,&file_suffix, 0);

                if (new_index != current_index) {
                        if (cpu->cur_core->next == &cpu->core_list) {
                                core = (core_s_t*)calloc(1, sizeof(core_s_t));
                                list_init(&core->list);
                                list_add(cpu->cur_core, &core->list);
                                cpu->cur_core = &core->list;
                        } else {
                                cpu->cur_core = cpu->cur_core->next;
                                core = entry_of(cpu->cur_core, core_s_t, list);
                        }
                        core->index = new_index;
                        current_index = new_index;
                }

                sprintf(str, "%s/%s", path, namelist[i]->d_name);
                parse_core_temp_dir(str, core, file_suffix + 1);
        }
        free_namelist(namelist, n);
        // TODO: free remaining nodes;

        return 0;
}


static int get_cpu_temp(const char *path, sensor_t *sensor)
{
        if (sensor->cur_cpu->next == &sensor->cpu_list) {
                cpu_s_t *cpu = (cpu_s_t*)calloc(1, sizeof(cpu_s_t));
                list_init(&cpu->list);
                list_init(&cpu->core_list);
                list_add(sensor->cur_cpu, &cpu->list);
                sensor->cur_cpu = &cpu->list;
        } else {
                sensor->cur_cpu = sensor->cur_cpu->next;
        }
        get_core_temp(path, entry_of(sensor->cur_cpu, cpu_s_t, list));

        // TODO: free remaining nodes;

        return 0;
}


static int process_valid_dir(const char *path, sensor_t *sensor)
{
        char str[MAX_PATH_STR_LEN] = {0};
        sprintf(str, "%s/name", path);
        get_first_line(str, str, MAX_PATH_STR_LEN);

        if (strncmp("coretemp", str, 8) == 0)
                get_cpu_temp(path, sensor);

        return 0;
}


static int parse_hwmon(const char *path, sensor_t *sensor)
{
        static int depth = 0;
        char str[MAX_PATH_STR_LEN] = {0};
        struct dirent **namelist = NULL;

        if (++depth > MAX_DIR_DEPTH) goto out;

        if (is_valid_dir(path)) {
                process_valid_dir(path, sensor);
                goto out;
        }

        int n = scandir(path, &namelist, filter_valid_file, alphasort);

        for (int i = 0; i < n; ++i) {
                sprintf(str, "%s/%s", path, namelist[i]->d_name);
                parse_hwmon(str, sensor);
        }

        if (!namelist) {
                return 0;
        }
        free_namelist(namelist, n);

out:
        --depth;
        return 0;
}


static void free_core_s_list(list_t *core_list)
{
        list_t *cur, *tmp, *head;
        head = core_list;
        core_s_t *core;
        list_for_each_safe(cur, tmp, head) {
                cur = list_remove(cur);
                core = entry_of(cur, core_s_t, list);
                free(core);
        }
}



static void free_cpu_s_list(list_t *cpu_list)
{
        list_t *cur, *tmp, *head;
        head = cpu_list;
        cpu_s_t *cpu;
        list_for_each_safe(cur, tmp, head) {
                cpu = entry_of(cur, cpu_s_t, list);
                free_core_s_list(&cpu->core_list);
        }
}


int init_sensor(sensor_t *sensor)
{
        list_init(&sensor->cpu_list);
        reset_sensor_cursor(sensor);

        return 0;
}


void free_sensor(sensor_t *sensor)
{
        free_cpu_s_list(&sensor->cpu_list);
}


int update_sensor(sensor_t *sensor)
{
        reset_sensor_cursor(sensor);
        return parse_hwmon(SYS_HWMON, sensor);
}


core_s_t *max_temp_core(list_t *cpu_list, core_s_t *ret)
{
        list_t *cur_cpu, *head_cpu, *cur_core, *head_core;
        ret = NULL;
        int max = 0;
        head_cpu = cpu_list;
        list_for_each(cur_cpu, head_cpu) {
                head_core = &(entry_of(cur_cpu, cpu_s_t, list)->core_list);
                list_for_each(cur_core, head_core) {
                        if (entry_of(cur_core, core_s_t, list)->input >= max) {
                                ret = entry_of(cur_core, core_s_t, list);
                                max = ret->input;
                        }
                }
        }

        return ret;
}


