#include <unistd.h>
#include <stdio.h>
#include "../network.h"

// You have to utilise GDB to find out how variables change.

int main(void)
{
        net_t net;
        init_net(&net);
        update_net(&net);

        sleep(1);
        update_net(&net);
        update_speed_str(&net, 1000);

        sleep(1);
        update_net(&net);
        update_speed_str(&net, 1000);

        sleep(1);
        update_net(&net);
        update_speed_str(&net, 1000);

        sleep(1);
        update_net(&net);
        update_speed_str(&net, 1000);
}

