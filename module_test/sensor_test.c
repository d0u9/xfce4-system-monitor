#include <unistd.h>
#include <stdio.h>
#include "../sensor.h"

// You have to utilise GDB to find out how variables change.

int main(void)
{
        sensor_t sensor;
        core_s_t *core = NULL;

        init_sensor(&sensor);
        update_sensor(&sensor);
        core = max_temp_core(&sensor.cpu_list, core);
        printf("label = %s, input = %d\n", core->label, core->input);

        while (1) {
                printf("====================\n");
                update_sensor(&sensor);
                core = max_temp_core(&sensor.cpu_list, core);
                printf("label = %s, input = %d\n", core->label, core->input);
                sleep(1);
        }

        free_sensor(&sensor);
}
