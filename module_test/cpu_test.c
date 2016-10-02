#include <unistd.h>
#include <stdio.h>
#include "../cpu.h"

// You have to utilise GDB to find out how variables change.

int main(void)
{
        cpu_t cpu;
        init_cpu(&cpu);

        while (1) {
                update_cpu(&cpu);
                printf("%.1f %%\n\n", cpu.total.load / 100.0);
                sleep(1);
        }
}


