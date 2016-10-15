#define _SVID_SOURCE

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include "sysmonitor.h"
#include "sensor.h"

#define SYS_HWMON	"/sys/class/hwmon"
#define MAX_DIR_DEPTH	3

#define reset_sensor_cursor(sensor)	sensor->cur_cpu = &sensor->cpu_list
#define reset_cpu_cursor(cpu)		cpu->cur_core = &cpu->core_list


int init_sensor(struct sensor *sensor);
int update_sensor(struct sensor *sensor);
void free_sensor(struct sensor *sensor);
struct core_s *max_temp_core(struct list_head *cpu_list, struct core_s *ret);

// axuiliary functions
static int get_first_line(const char *file, char *buf, int len);
static int filter_temp_file(const struct dirent *dir);
static int filter_valid_file(const struct dirent *dir);
static void free_namelist(struct dirent **namelist, int n);

// iterate over directories
static int parse_hwmon(const char *path, struct sensor *sensor);
static int is_valid_dir(const char *path);
static int process_valid_dir(const char *path, struct sensor *sensor);

// CPU
static int get_cpu_temp(const char *path, struct sensor *sensor);
static void free_cpu_s_list(struct list_head *cpu_list);
static int get_core_temp(const char *path, struct cpu_s *cpu);
static int parse_core_temp_dir(const char *file, struct core_s *core,
			       const char *suffix);
static void free_core_s_list(struct list_head *core_list);

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

static int parse_core_temp_dir(const char *file, struct core_s *core,
			       const char *suffix)
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

static int get_core_temp(const char *path, struct cpu_s *cpu)
{
	char str[MAX_PATH_STR_LEN] = {0};
	char str2[MAX_PATH_STR_LEN] = {0};
	char *file_suffix = str2;
	struct dirent **namelist = NULL;

	reset_cpu_cursor(cpu);
	int n = scandir(path, &namelist, filter_temp_file, alphasort);

	struct core_s *core = NULL;
	int current_index = -1;
	for (int i = 0; i < n; ++i) {
		int new_index = strtol(namelist[i]->d_name + 4,&file_suffix, 0);

		if (new_index != current_index) {
			if (cpu->cur_core->next == &cpu->core_list) {
				core = (struct core_s*)
					calloc(1, sizeof(struct core_s));
				INIT_LIST_HEAD(&core->list);
				list_add(&core->list, cpu->cur_core);
				cpu->cur_core = &core->list;
			} else {
				cpu->cur_core = cpu->cur_core->next;
				core = list_entry(cpu->cur_core, struct core_s, list);
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

static int get_cpu_temp(const char *path, struct sensor *sensor)
{
	if (sensor->cur_cpu->next == &sensor->cpu_list) {
		struct cpu_s *cpu = (struct cpu_s*)
			calloc(1, sizeof(struct cpu_s));
		INIT_LIST_HEAD(&cpu->list);
		INIT_LIST_HEAD(&cpu->core_list);
		list_add(&cpu->list, sensor->cur_cpu);
		sensor->cur_cpu = &cpu->list;
	} else {
		sensor->cur_cpu = sensor->cur_cpu->next;
	}
	get_core_temp(path, list_entry(sensor->cur_cpu, struct cpu_s, list));

	// TODO: free remaining nodes;

	return 0;
}

static int process_valid_dir(const char *path, struct sensor *sensor)
{
	char str[MAX_PATH_STR_LEN] = {0};
	sprintf(str, "%s/name", path);
	get_first_line(str, str, MAX_PATH_STR_LEN);

	if (strncmp("coretemp", str, 8) == 0)
		get_cpu_temp(path, sensor);

	return 0;
}

static int parse_hwmon(const char *path, struct sensor *sensor)
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

static void free_core_s_list(struct list_head *core_list)
{
	struct list_head *cur, *tmp, *head;
	head = core_list;
	struct core_s *core;
	list_for_each_safe(cur, tmp, head) {
		core = list_entry(cur, struct core_s, list);
		list_del(cur);
		free(core);
	}
}

static void free_cpu_s_list(struct list_head *cpu_list)
{
	struct list_head *cur, *tmp, *head;
	head = cpu_list;
	struct cpu_s *cpu;
	list_for_each_safe(cur, tmp, head) {
		cpu = list_entry(cur, struct cpu_s, list);
		free_core_s_list(&cpu->core_list);
	}
}

int init_sensor(struct sensor *sensor)
{
	INIT_LIST_HEAD(&sensor->cpu_list);
	reset_sensor_cursor(sensor);

	return 0;
}


void free_sensor(struct sensor *sensor)
{
	free_cpu_s_list(&sensor->cpu_list);
}

int update_sensor(struct sensor *sensor)
{
	reset_sensor_cursor(sensor);
	return parse_hwmon(SYS_HWMON, sensor);
}

struct core_s *max_temp_core(struct list_head *cpu_list, struct core_s *ret)
{
	struct list_head *cur_cpu, *head_cpu, *cur_core, *head_core;
	ret = NULL;
	int max = 0;
	head_cpu = cpu_list;
	list_for_each(cur_cpu, head_cpu) {
		head_core = &(list_entry(cur_cpu, struct cpu_s, list)->core_list);
		list_for_each(cur_core, head_core) {
			if (list_entry(cur_core, struct core_s, list)->input >= max) {
				ret = list_entry(cur_core, struct core_s, list);
				max = ret->input;
			}
		}
	}

	return ret;
}
